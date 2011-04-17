/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.12.2010 11:43:12
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDebug>
#include <QPainter>
#include <lapackpp/blas3pp.h>
#include <lapackpp/laslv.h>
#include <lapackpp/lavli.h>
#include <lapackpp/laexcp.h>
#include "GrayscaleFilter.h"
#include "Align.h"

namespace FaceDetect {

/**
 * Vytvorenie inštancie triedy pre zarovnávanie fotografií.
 */
Align::Align(QObject *parent):
	QObject(parent),
	m_faceFeaturesSum(FaceFeaturesCount * 2),
	m_avgFaceFeatures(FaceFeaturesCount * 2),
	m_normalizedFaceFeatures(FaceFeaturesCount * 2),
	m_faceCount(0),
	m_normalized(false),
	m_collectStatistics(false),
	m_imageSize(128)
{
	for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
		m_faceFeaturesSum(i) = 0;
		m_avgFaceFeatures(i) = 0;
		m_normalizedFaceFeatures(i) = 0;
	}
}

Align::~Align()
{
}

/**
 * Počas vkladania záznamov pre zarovnanie sa môžu zaznamenávať štatistiky,
 * pomocou ktorých sa dá zostaviť rozdelenie kontrolných bodov. Zapnutie záznamu
 * štatistíky vyžadujem dodatočnú RAM pre zaznamenanie kontrolných bodov
 * všetkých fotografií.
 * \sa getStatisticsImage()
 */
void Align::setCollectStatistics(bool statistics)
{
	m_collectStatistics = statistics;
	if (!m_collectStatistics) {
		m_faceData.clear();
	}
}

/**
 * Nastavenie veľkosti, ktorú má mať výsledná transformovaná fotografia.
 * Štandardná veľkosť je 128 (obrázky sú teda veľkosti 128x128).
 */
void Align::setImageSize(int imageSize)
{
	m_imageSize = imageSize;
	m_normalized = false;
}

/**
 * Spracovanie fotografie \a info.
 */
void Align::addImage(const FaceFileScanner::ImageInfo &info)
{
	if (!info.isValid()) {
		return;
	}

	for (auto face = info.faceBegin(); face != info.faceEnd(); ++face) {
		// Kointrola, či sú všetky požadované body zadané
		if (!checkControlPoints(*face)) {
			continue;
		}
		m_faceCount++;
		if (m_faceCount == 1) {
			m_avgFaceFeatures = getControlPointsVector(*face);
			m_faceFeaturesSum = m_avgFaceFeatures;
		}
		else {
			m_normalized = false;
			// Výpočet transformovaných vlastností
			LaVectorDouble featVector = getControlPointsVector(*face);
			LaGenMatDouble aMatrix = fillFeaturesMatrix(featVector);
			LaVectorDouble tVector = getTransformVector(aMatrix, false);
			LaGenMatDouble newFeatures(FaceFeaturesCount * 2, 1);
			// $t_new = A \cdot t$
			Blas_Mat_Mat_Mult(aMatrix, tVector, newFeatures);
			// Výpočet priemerných hodnôt
			for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
				m_faceFeaturesSum(i) += newFeatures(i, 0);
			}
			calcAvg();
		}

		// Záznam dát pre prípad, že potrebujeme štatistiky
		if (m_collectStatistics) {
			m_faceData.append(*face);
		}
	}
}

/**
 * Vráti počet obrázkov, ktoré boli spracované.
 */
std::size_t Align::faceCount() const
{
	return m_faceCount;
}

/**
 * Vráti transformačnú maticu pre tvár \a face.
 */
QTransform Align::getTransform(const FaceFileScanner::FaceData &face) const
{
	// Kointrola, či sú všetky požadované body zadané
	if (!checkControlPoints(face)) {
		return QTransform();
	}
	LaVectorDouble featVector = getControlPointsVector(face);
	LaGenMatDouble aMatrix = fillFeaturesMatrix(featVector);
	LaVectorDouble tVector = getTransformVector(aMatrix, true);
	return QTransform(tVector(0), tVector(1), -tVector(1), tVector(0), tVector(2), tVector(3));
}

/**
 * V prípade, že je povolené zbieranie štatistických dát vráti táto metóda
 * obrázok s rozdelením kontrolných bodov po zarovnaní.
 * \sa setCollectStatistics()
 */
QImage Align::getStatisticsImage() const
{
	if (m_faceCount < 2) {
		return QImage(QSize(1, 1), QImage::Format_ARGB32);
	}
	normalize();
	LaGenMatDouble imgMatrix(m_imageSize, m_imageSize);
	for (int x = 0; x < m_imageSize; ++x) {
		for (int y = 0; y < m_imageSize; ++y) {
			imgMatrix(y, x) = 0;
		}
	}
	foreach (const FaceFileScanner::FaceData &data, m_faceData) {
		QTransform transform = getTransform(data);
		QPoint point;
		point = transform.map(data.leftEye);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
		point = transform.map(data.rightEye);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
		point = transform.map(data.nose);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
		point = transform.map(data.mouth);
		if (checkPointRange(point)) {
			imgMatrix(point.y(), point.x())++;
		}
	}

	// Nájdenie najväčšieho prvku
	double max = 0;
	for (int x = 0; x < m_imageSize; ++x) {
		for (int y = 0; y < m_imageSize; ++y) {
			if (imgMatrix(y, x) > max) {
				max = imgMatrix(y, x);
			}
		}
	}

	// Prepočet hodnôt rozdelenia na rozsah 0 - 1.
	if (max != 0) {
		for (int x = 0; x < m_imageSize; ++x) {
			for (int y = 0; y < m_imageSize; ++y) {
				imgMatrix(y, x) = (imgMatrix(y, x) / max) * 256.0;
			}
		}
	}

	// Nastavenie hodnôt pixelov obrázku
	QImage image(QSize(m_imageSize, m_imageSize), QImage::Format_ARGB32);
	for (int x = 0; x < m_imageSize; ++x) {
		for (int y = 0; y < m_imageSize; ++y) {
			int value = qMin(int(imgMatrix(y, x)), 255);
			image.setPixel(x, y, qRgb(value, value, value));
		}
	}

	// Prevod obrázku v odtieňoch šedej na farebnú mapu
	QLinearGradient imgGradient;
	imgGradient.setColorAt(0.0, Qt::darkCyan);
	imgGradient.setColorAt(0.05, Qt::cyan);
	imgGradient.setColorAt(0.1, Qt::green);
	imgGradient.setColorAt(0.2, Qt::yellow);
	imgGradient.setColorAt(1.0, Qt::red);
	GrayscaleFilter filter;
	filter.setGrayscaleGradient(imgGradient);
	filter.filter(image);
	return image;
}

/**
 * Vráti priemerné hodnoty vlastností po normalizácii.
 */
FaceFileScanner::FaceData Align::getAverageFeatures() const
{
	if (m_normalized) {
		normalize();
	}

	FaceFileScanner::FaceData ret;
	ret.leftEye = QPoint(m_normalizedFaceFeatures(0), m_normalizedFaceFeatures(1));
	ret.rightEye = QPoint(m_normalizedFaceFeatures(2), m_normalizedFaceFeatures(3));
	ret.nose = QPoint(m_normalizedFaceFeatures(4), m_normalizedFaceFeatures(5));
	ret.mouth = QPoint(m_normalizedFaceFeatures(6), m_normalizedFaceFeatures(7));
	return ret;
}

/**
 * Kontrola správnosti všetkých kontrolných bodov.
 */
bool Align::checkControlPoints(const FaceFileScanner::FaceData &data) const
{
	if (data.leftEye != QPoint() && data.rightEye != QPoint() && data.nose != QPoint() && data.mouth != QPoint()) {
		return true;
	}
	else {
		return false;
	}
}

/**
 * Prevod kontrolných bodov na vektor.
 */
LaVectorDouble Align::getControlPointsVector(const FaceFileScanner::FaceData &data) const
{
	LaVectorDouble ret(FaceFeaturesCount * 2);
	ret(0) = data.leftEye.x();
	ret(1) = data.leftEye.y();
	ret(2) = data.rightEye.x();
	ret(3) = data.rightEye.y();
	ret(4) = data.nose.x();
	ret(5) = data.nose.y();
	ret(6) = data.mouth.x();
	ret(7) = data.mouth.y();
	return ret;
}

/**
 * Výpočet transformačného vektoru pre maticu \a aMatrix zostavenú zo vstupných
 * bodov. V prípade, že je zapnutá normalizácia vlastnosti budú upravené tak,
 * aby oči boli v jednej rovine vo vzdialenosti 128. Transformačný vektor má 4
 * položky, ktoré sú ekvivalentmi s položkami (m11, m12, m13, m23).
 */
LaVectorDouble Align::getTransformVector(const LaGenMatDouble &aMatrix, bool normalized) const
{
	if (normalized) {
		normalize();
	}

	LaGenMatDouble nMatrix(4, 4);
	Blas_Mat_Mat_Mult(aMatrix, aMatrix, nMatrix, true);

	LaVectorLongInt pivots(4);
	LUFactorizeIP(nMatrix, pivots);
	LaLUInverseIP(nMatrix, pivots);

	LaGenMatDouble varCovMatrix(4, 1);
	if (normalized) {
		Blas_Mat_Mat_Mult(aMatrix, m_normalizedFaceFeatures, varCovMatrix, true);
	}
	else {
		Blas_Mat_Mat_Mult(aMatrix, m_avgFaceFeatures, varCovMatrix, true);
	}
	LaGenMatDouble tVector(4, 1);
	Blas_Mat_Mat_Mult(nMatrix, varCovMatrix, tVector);
	return tVector;
}

/**
 * Transformácia vstupného vektoru \a input na výstupný vektor (návratová
 * hodnota).
 */
LaVectorDouble Align::transform(const LaVectorDouble &input, const LaVectorDouble &transVector) const
{
	LaGenMatDouble ret(input.rows(), 1);
	LaGenMatDouble aMatrix = fillFeaturesMatrix(input);
	// $o = A \cdot t$
	Blas_Mat_Mat_Mult(aMatrix, transVector, ret);
	return ret;
}

/**
 * Vytvorenie matice z vektoru, v ktorom sú kontrolné body vo formáte (x1, y1,
 * x2, y2 ...). Vzniknutá matica má potom riadky (x1, x2, 1, 0), (x2, -x1, 0,
 * 1), (y1, y2, 1, 0) ...
 */
LaGenMatDouble Align::fillFeaturesMatrix(const LaVectorDouble &input) const
{
	LaGenMatDouble aMatrix(input.rows(), 4);
	for (int row = 0; row < input.rows(); ++row) {
		aMatrix(row, 0) = input(row);
		aMatrix(row, 1) = ((row & 0x01) == 0) ? (-input(row + 1)) : (input(row - 1));
		aMatrix(row, 2) = ((row & 0x01) == 0) ? 1 : 0;
		aMatrix(row, 3) = ((row & 0x01) == 0) ? 0 : 1;
	}
	return aMatrix;
}

/**
 * Výpočet priemerných hodnôt kontrolných bodov. Výsledok výpočtu sa bude
 * nachádzať v premennej m_avgFaceFeatures.
 */
void Align::calcAvg() const
{
	for (int i = 0; i < m_faceFeaturesSum.rows(); ++i) {
		m_avgFaceFeatures(i) = m_faceFeaturesSum(i) / double(m_faceCount);
	}
}

/**
 * Prepočet kontrolných bodov tak, aby vrchné 2 body boli rovnobežné s x-ovou
 * osou. Normalizované hodnoty sa uložia do premennej m_normalizedFaceFeatures.
 */
void Align::normalize() const
{
	if (!m_normalized) {
		// Virtuálna čiara vedená cez stred tváre od očí po ústa
		LaVectorDouble centerLine(4);
		centerLine(0) = (m_avgFaceFeatures(0) + m_avgFaceFeatures(2)) / 2.0;
		centerLine(1) = (m_avgFaceFeatures(1) + m_avgFaceFeatures(3)) / 2.0;
		centerLine(2) = m_avgFaceFeatures(6);
		centerLine(3) = m_avgFaceFeatures(7);
		// Cieľové hodnoty centrálnej čiary po transformácii
		LaVectorDouble transformedLine(4);
		transformedLine(0) = m_imageSize / 2;
		transformedLine(1) = m_imageSize / 10;
		transformedLine(2) = m_imageSize / 2;
		transformedLine(3) = (m_imageSize * 9) / 10;

		LaGenMatDouble aMatrix = fillFeaturesMatrix(centerLine);
		LaVectorDouble tVector(4);
		try {
			LaLinearSolve(aMatrix, tVector, transformedLine);
			m_normalizedFaceFeatures = transform(m_avgFaceFeatures, tVector);
		}
		catch (LaException &) {
			qWarning() << "Failed to normalize features";
		}

		m_normalized = true;
	}
}

/**
 * Metóda testuje sa nachádza bod \a point vo vnútri štvorca s tvárou
 * (štandardne 128x128 bodov).
 */
bool Align::checkPointRange(const QPoint &point) const
{
	if (point.x() < 0 || point.y() < 0) {
		return false;
	}
	if (point.x() >= m_imageSize || point.y() >= m_imageSize) {
		return false;
	}
	return true;
}

} /* end of namespace FaceDetect */


/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  26.03.2011 11:53:17
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QRectF>
#include <QPolygonF>
#include <cstring>
#include "FaceDetector.h"
#include "utils/PolygonRasterizer.h"

using std::memset;

namespace FaceDetect {

/**
 * Vytvorenie detektoru tváre používajúceho neurónovú sieť \a neuralNet.
 */
FaceDetector::FaceDetector(NeuralNet *neuralNet, QObject *parent):
	QThread(parent),
	m_neuralNet(neuralNet)
{
}

FaceDetector::~FaceDetector()
{
}

/**
 * Vráti, ktorý sa práve spracúva.
 */
QImage FaceDetector::image() const
{
	if (m_segmenter.isNull()) {
		return QImage();
	}
	else {
		return m_segmenter->image();
	}
}

/**
 * Nastavenie obrázku, ktorý sa má spracovať.
 */
void FaceDetector::setImage(const QImage &image)
{
	m_segmenter = QSharedPointer<ImageSegmenter>(new ImageSegmenter(image, m_settings));
	m_statistics.resize(image.width() * image.height(), 0);
}

/**
 * Nastavenie spôsobu rozkladania obrazu na fragmenty.
 */
void FaceDetector::setupSegmenter(const ImageSegmenter::Settings &settings)
{
	m_settings = settings;
	if (!m_segmenter.isNull()) {
		m_segmenter->setupSegmenter(m_settings);
	}
}

/**
 * Preskenovanie aktuálneho súboru s obrázkom.
 */
void FaceDetector::scanImage()
{
	// Veľkosť dát
	std::size_t statisticsWidth = m_segmenter->image().width();
	std::size_t statisticsHeight = m_segmenter->image().height();

	Utils::PolygonRasterizer rasterizer;
	// Inverzia transformácie obrazu
	QTransform rectTransform = m_settings.transform.inverted();
	int segmentCount = m_segmenter->segmentCount();
	// Výpočet hodnoty každého segmentu
	for (int segmentId = 0; segmentId < segmentCount; ++segmentId) {
		// Výpočet vstupu a výstupu
		QImage segmentImage = m_segmenter->segmentImage(segmentId);

		// Oblasť segmentu
		QRect segmentRect = m_segmenter->segmentRect(segmentId);
		// Transformácia polygónu
		QPolygon poly(segmentRect);
		poly = rectTransform.map(poly);

		LaVectorDouble inputVector = m_localFilter.filterVector(segmentImage);
		LaVectorDouble outputVector = m_neuralNet->calcOutput(inputVector);
		// Zamietnuté
		if (outputVector(0) < m_neuralNet->binaryThreshold()) {
			outputVector(0) = 0.5 * outputVector(0) / (m_neuralNet->binaryThreshold());
		}
		// Obsahuje tvár
		else {
			double val = qMax(0.0, outputVector(0) - m_neuralNet->binaryThreshold()) * 1.0 / (1.0 - m_neuralNet->binaryThreshold());
			outputVector(0) = val * 0.5 + 0.5;
			m_scanResult << DetectionWindow{val, poly};
		}
		// Rasterizácia polgónu
		rasterizer.processPolygon(poly);
		const QVector<Utils::PolygonRasterizer::ScanLine> lines = rasterizer.scanLines();
		int y = rasterizer.boundingRect().top();
		std::size_t memBegin = statisticsWidth * y;
		std::size_t lineBegin, lineEnd;
		foreach (const Utils::PolygonRasterizer::ScanLine &line, lines) {
			if (y < 0) {
				y++;
				memBegin = statisticsWidth * y;
				continue;
			}
			else if (static_cast<std::size_t>(y) >= statisticsHeight) {
				break;
			}
			lineBegin = static_cast<std::size_t>(qMax(line.minX, 0)) + memBegin;
			lineEnd = static_cast<std::size_t>(qMin(line.maxX, int(statisticsWidth))) + memBegin;
			for (std::size_t memPos = lineBegin; memPos < lineEnd; ++memPos) {
				m_statistics[memPos] = qMax(m_statistics[memPos], static_cast<float>(outputVector(0)));
			}
			y++;
			memBegin = statisticsWidth * y;
		}
	}
	qSort(m_scanResult.begin(), m_scanResult.end(), [](DetectionWindow x, DetectionWindow y) { return x.value < y.value; });

	m_scanResultImage = QImage(m_segmenter->image().size(), QImage::Format_ARGB32);
	for (std::size_t yPos = 0; yPos < statisticsHeight; ++yPos) {
		for (std::size_t xPos = 0; xPos < statisticsWidth; ++xPos) {
			float val = m_statistics[yPos * statisticsWidth + xPos];
			int colorVal;
			if (val < 0) {
				colorVal = 0;
			}
			else if (val >= 1) {
				colorVal = 255;
			}
			else {
				colorVal = val * 255;
			}
			m_scanResultImage.setPixel(QPoint(int(xPos), int(yPos)), qRgb(colorVal, colorVal, colorVal));
		}
	}
}

/**
 * Nastavenie lokálneho filtra detekčného okna.
 */
void FaceDetector::setLocalFilter(const ImageFilter &localFilter)
{
	m_localFilter = localFilter;
}

/**
 * Nastavenie globálneho filtra pred rozdelením na segmenty.
 */
void FaceDetector::setGlobalFilter(const ImageFilter &globalFilter)
{
	m_segmenter->setGlobalFilter(globalFilter);
}

/**
 * Vráti výsledok skenovania súboru ako obrázok.
 */
QImage FaceDetector::scanResultImage() const
{
	return m_scanResultImage;
}

/**
 * Vráti výsledok skenovania ako zoznam okien s hodnotou vyššou ako hranica a
 * ako value je hodnota v rozsahu 0 - 1 (0 je hranica). Poradie je od najmenšej
 * hodnoty po najväčšiu.
 */
QVector<FaceDetector::DetectionWindow> FaceDetector::scanResult() const
{
	return m_scanResult;
}

} /* end of namespace FaceDetect */


/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  16.03.2011 13:34:55
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QPainter>
#include <QPointF>
#include <QPolygonF>
#include "ImageSegmenter.h"

namespace FaceDetect {

/**
 * Vytvorenie inštancie triedy pre segmentáciu obrásku \a image.
 */
ImageSegmenter::ImageSegmenter(const QImage &image):
	m_sourceImage(image),
	m_segmentSize(5, 5),
	m_xStep(1),
	m_yStep(1),
	m_dirty(true),
	m_segmentCount(0)
{
}

ImageSegmenter::~ImageSegmenter()
{
}

/**
 * Vráti cieľovú veľkosť segmentov.
 */
QSize ImageSegmenter::segmentSize() const
{
	return m_segmentSize;
}

/**
 * Nastavenie cieľovej veľkosti segmentov na \a size.
 */
void ImageSegmenter::setSegmentSize(const QSize &size)
{
	if (m_segmentSize != size) {
		m_segmentSize = size;
		m_dirty = true;
	}
}

/**
 * Vráti transformáciu používanú na bitmapu.
 */
QTransform ImageSegmenter::transform() const
{
	return m_transform;
}

/**
 * Nastavenie transformačnej matice bitmapy.
 */
void ImageSegmenter::setTransform(const QTransform &transform)
{
	if (m_transform != transform) {
		m_transform = transform;
		m_dirty = true;
	}
}

/**
 * Vráti x-ový krok posunu segmentu.
 */
int ImageSegmenter::xStep()
{
	return m_xStep;
}

/**
 * Nastavenie x-ového kroku posunu segmentu.
 */
void ImageSegmenter::setXStep(int step)
{
	if (m_xStep != step) {
		m_xStep = step;
		m_dirty = true;
	}
}

/**
 * Vráti y-ový krok posunu segmentu.
 */
int ImageSegmenter::yStep()
{
	return m_yStep;
}

/**
 * Nastavenie y-ového kroku posunu segmentu.
 */
void ImageSegmenter::setYStep(int step)
{
	if (m_yStep != step) {
		m_yStep = step;
		m_dirty = true;
	}
}

/**
 * Nastavenie x a y-ového kroku posunu segmentu.
 * \sa setXStep(), setYStep()
 */
void ImageSegmenter::setStep(int xStep, int yStep)
{
	if (m_xStep != xStep || m_yStep != yStep) {
		m_xStep = xStep;
		m_yStep = yStep;
		m_dirty = true;
	}
}

/**
 * Vráti \e true, ak je povolený filter pre prevod do odtieňov šedej.
 */
bool ImageSegmenter::grayscaleFilter() const
{
	return m_grayscaleFilter.filters() & ImageFilter::GrayscaleFilter;
}

/**
 * Nastavenie filtrovania odtieňov šedej.
 */
void ImageSegmenter::setGrayscaleFilter(bool filter)
{
	if (filter != grayscaleFilter()) {
		if (filter) {
			m_grayscaleFilter.setFilters(ImageFilter::GrayscaleFilter);
		}
		else {
			m_grayscaleFilter.setFilters(ImageFilter::NoFilter);
		}
		if (!m_transformedImage.isNull()) {
			m_dirty = true;
		}
	}
}

/**
 * Vráti oblasť, v ktorej sa nachádza celá transformovaná bitmapa.
 */
QRect ImageSegmenter::boundingRect() const
{
	recalcSegments();
	return m_boundingRect;
}

/**
 * Vráti celkový počet segmentov v bitmape.
 */
int ImageSegmenter::segmentCount() const
{
	recalcSegments();
	return m_segmentCount;
}

/**
 * Vráti oblasť segmentu pre \a segment. Táto oblasť zodpovedá oblasti v
 * transformvanej bitmape.
 */
QRect ImageSegmenter::segmentRect(int segment) const
{
	recalcSegments();
	// Kontrola čísla segmentu
	Q_ASSERT(segment < m_segmentCount);

	// Nájdenie riadku v ktorom sa segment nachádza
	auto lineIt = qUpperBound(m_lineCounts.begin(), m_lineCounts.end(), segment);
	// Výpočet riadku a stĺpca
	int row = lineIt - m_lineCounts.begin() - 1;
	int col = segment - (lineIt == m_lineCounts.begin() ? 0 : *(lineIt - 1));
	// Výpočet koordinátov riadku a stĺpca
	int x = m_lineStarts[row] + col * m_xStep - m_segmentSize.width() / 2;
	int y = m_boundingRect.top() + row * m_yStep - m_segmentSize.height() / 2;
	return QRect(QPoint(x, y), m_segmentSize);
}

/**
 * Vráti obrázok nachádzajúci sa v segmente \a segment.
 */
QImage ImageSegmenter::segmentImage(int segment) const
{
	recalcSegments();
	Q_ASSERT(segment < m_segmentCount);

	QRect rect = segmentRect(segment);

	if (m_transformedImage.isNull()) {
		QImage image(m_segmentSize, QImage::Format_ARGB32);
		image.fill(qRgb(0, 0, 0));
		QPainter painter(&image);
		// Aplikujeme rovnakú transformáciu ako na obrázok a posunieme podľa
		// koordinátov regiónu
		QTransform transform = m_transform;
		transform *= QTransform::fromTranslate(-rect.left(), -rect.top());
		painter.setTransform(transform);

		painter.drawImage(QPoint(0, 0), m_sourceImage);
		painter.end();

		if (grayscaleFilter()) {
			return m_grayscaleFilter.filterImage(image);
		}
		else {
			return image;
		}
	}
	else {
		return m_transformedImage.copy(rect.translated(-m_boundingRect.topLeft()));
	}
}

/**
 * Prepočet jednotlivých segmentov.
 */
void ImageSegmenter::recalcSegments() const
{
	if (!m_dirty) {
		return;
	}

	QPointF p[4];
	// Výpočet vnútra štvorca
	p[0] = m_transform.map(QPointF(0, 0));
	p[1] = m_transform.map(QPointF(m_sourceImage.width(), 0));
	p[2] = m_transform.map(QPointF(m_sourceImage.width(), m_sourceImage.height()));
	p[3] = m_transform.map(QPointF(0, m_sourceImage.height()));
	// floor, ceil nahradené round-om aby aj pri veľkosti 1x1px bol nájdený stred
	int x1 = qRound(qMin(qMin(p[0].x(), p[1].x()), qMin(p[2].x(), p[3].x())));
	int y1 = qRound(qMin(qMin(p[0].y(), p[1].y()), qMin(p[2].y(), p[3].y())));
	int x2 = qRound(qMax(qMax(p[0].x(), p[1].x()), qMax(p[2].x(), p[3].x())));
	int y2 = qRound(qMax(qMax(p[0].y(), p[1].y()), qMax(p[2].y(), p[3].y())));
	m_boundingRect = QRect(QPoint(x1, y1), QPoint(x2, y2));

	m_lineCounts.clear();
	m_lineStarts.clear();
	int yCount = (y2 - y1) / m_yStep + 1;
	m_lineCounts.reserve(yCount);
	m_lineStarts.reserve(yCount);

	// Hľadanie oblastí, ktorých stred je vo vnútri pixmapy
	m_segmentCount = 0;
	// Prechádzanie všetkých oblastí po y osi
	for (int y = y1; y <= y2; y += m_yStep) {
		// Nadobúda hodnotu 0 alebo 1 podľa toho, či sa ide zapisovať do int1/int2
		int pointId = 0;
		// Priesečník
		double intX1 = 0;
		double intX2 = 0;
		double xInters = 0;
		// Hľadáme priesečník na všetkých 4 hranách
		for (int line = 0; line < 4; ++line) {
			int line2 = line == 3 ? 0 : line + 1;
			// U rovnobežnej hrany sú krajné priesečníky x jej začiatok a koniec
			if (p[line].y() == p[line2].y()) {
				intX1 = p[line].x();
				intX2 = p[line2].x();
				break;
			}
			// Vyhľadanie prvého, alebo druhého priesečníku a zápis do intX1/intX2
			bool ok;
			xInters = calcIntersect(p[line], p[line2], y, &ok);
			if (ok) {
				if (pointId == 0) {
					intX1 = xInters;
				}
				else {
					intX2 = xInters;
				}
				pointId++;
			}
		}
		int pixelX1 = qRound(qMin(intX1, intX2));
		int pixelX2 = qRound(qMax(intX1, intX2));
		// Výpočet počtu položiek v riadku
		int lineCount = (pixelX2 - pixelX1) / m_xStep + 1;
		// Zaznamenanie počiatočného segmentu a počtu riadkov
		m_lineCounts << m_segmentCount;
		m_lineStarts << pixelX1;
		m_segmentCount += lineCount;
	}

	if (long(m_boundingRect.width()) * m_boundingRect.height() <= MaxImageResolution) {
		m_transformedImage = QImage(m_boundingRect.size(), QImage::Format_ARGB32);
		m_transformedImage.fill(qRgb(0, 0, 0));
		QPainter imagePainter(&m_transformedImage);
		QTransform transform = m_transform;
		transform *= QTransform::fromTranslate(-m_boundingRect.left(), -m_boundingRect.top());
		imagePainter.setTransform(transform);
		imagePainter.drawImage(QPoint(0, 0), m_sourceImage);
		imagePainter.end();
		if (grayscaleFilter()) {
			m_transformedImage = m_grayscaleFilter.filterImage(m_transformedImage);
		}
	}
	else {
		m_transformedImage = QImage();
	}

	m_dirty = false;
}

/**
 * Výpočet priesečníku úsečky definovanej bodmi \a p1 a \a p2 s rovnobežkou k
 * x-ovej osi v pozícii \a yPos. Ak bol nájdený priesečník návratová hodnota
 * bude x-ová súradnica v ktorej úsečka pretína rovnobežku. Ak sa nepretínajú
 * návratová hodnota bude 0 a hodota \a ok bude \e false.
 */
inline double ImageSegmenter::calcIntersect(const QPointF &p1, const QPointF &p2, double yPos, bool *ok) const
{
	// V rovnobežnej úsečke s x-ovou osou nie je možné zistiť priesečník
	if (p1.y() == p2.y()) {
		if (ok != 0) {
			*ok = false;
		}
		return 0;
	}

	// Úsečka nepretína zvolenú rovnobežku s x-ovou osou
	if (yPos < qMin(p1.y(), p2.y()) || yPos >= qMax(p1.y(), p2.y())) {
		if (ok != 0) {
			*ok = false;
		}
		return 0;
	}

	if (ok != 0) {
		*ok = true;
	}

	// Výpočet priesečníku
	double xInters = (yPos - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y()) + p1.x();
	return xInters;
}

} /* end of namespace FaceDetect */


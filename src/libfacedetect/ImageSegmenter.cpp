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
#include "utils/PolygonRasterizer.h"
#include "ImageSegmenter.h"

namespace FaceDetect {

/**
 * Vytvorenie inštancie triedy pre segmentáciu obrásku \a image.
 */
ImageSegmenter::ImageSegmenter(const QImage &image, const ImageSegmenter::Settings &settings):
	m_sourceImage(image),
	m_dirty(true),
	m_dirtyImage(false),
	m_segmentCount(0)
{
	setupSegmenter(settings);
}

ImageSegmenter::~ImageSegmenter()
{
}

/**
 * Nastavenie vlastností segmentera.
 */
void ImageSegmenter::setupSegmenter(const ImageSegmenter::Settings &settings)
{
	setTransform(settings.transform);
	setSegmentSize(settings.segmentSize);
	setStep(settings.xStep, settings.yStep);
}

/**
 * Vráti zdrojový obrázok pred segmentáciou.
 */
const QImage &ImageSegmenter::image() const
{
	return m_sourceImage;
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
			m_grayscaleFilter.enableGrayscaleFilter();
		}
		else {
			m_grayscaleFilter.disableGrayscaleFilter();
		}
		if (!m_transformedImage.isNull()) {
			m_dirtyImage = true;
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
	repaintImage();
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
inline void ImageSegmenter::recalcSegments() const
{
	if (!m_dirty) {
		return;
	}

	m_lineCounts.clear();
	m_lineStarts.clear();

	QVector<QPoint> p(4);
	// Výpočet vnútra štvorca
	p[0] = m_transform.map(QPoint(0, 0));
	p[1] = m_transform.map(QPoint(m_sourceImage.width(), 0));
	p[2] = m_transform.map(QPoint(m_sourceImage.width(), m_sourceImage.height()));
	p[3] = m_transform.map(QPoint(0, m_sourceImage.height()));

	// Rasterizátor
	Utils::PolygonRasterizer rasterizer;
	// Spracovanie polygónu tranformovaného obdĺžnika
	rasterizer.processPolygon(p);
	m_boundingRect = rasterizer.boundingRect();
	// Získanie začiatkov a koncov polygónu pre každý riadok
	QVector<Utils::PolygonRasterizer::ScanLine> scanLine = rasterizer.scanLines();

	m_segmentCount = 0;
	int y1 = m_boundingRect.top();
	int y2 = m_boundingRect.bottom();
	// Prechádzame všetky položky
	for (int y = y1; y <= y2; y += m_yStep) {
		int pixelX1 = scanLine[y - y1].minX;
		int pixelX2 = scanLine[y - y1].maxX;
		// Výpočet počtu položiek v riadku
		int lineCount = (pixelX2 - pixelX1) / m_xStep + 1;
		if (lineCount < 0) {
			continue;
		}
		// Zaznamenanie počiatočného segmentu a počtu riadkov
		m_lineCounts << m_segmentCount;
		m_lineStarts << pixelX1;
		m_segmentCount += lineCount;
	}

	if (long(m_boundingRect.width()) * m_boundingRect.height() <= MaxImageResolution) {
		m_dirtyImage = true;
	}
	else {
		m_transformedImage = QImage();
		m_dirtyImage = false;
	}

	m_dirty = false;
}

/**
 * Vykreslenie obrázku podľa transformácie a jeho zápis do m_transformedImage.
 */
inline void ImageSegmenter::repaintImage() const
{
	if (!m_dirtyImage) {
		return;
	}
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
	m_dirtyImage = false;
}

} /* end of namespace FaceDetect */


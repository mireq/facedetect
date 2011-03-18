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

#include <QDebug>

namespace FaceDetect {

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

QSize ImageSegmenter::segmentSize() const
{
	return m_segmentSize;
}

void ImageSegmenter::setSegmentSize(const QSize &size)
{
	if (m_segmentSize != size) {
		m_segmentSize = size;
		m_dirty = true;
	}
}

QTransform ImageSegmenter::transform() const
{
	return m_transform;
}

void ImageSegmenter::setTransform(const QTransform &transform)
{
	if (m_transform != transform) {
		m_transform = transform;
		m_dirty = true;
	}
}

int ImageSegmenter::xStep()
{
	return m_xStep;
}

void ImageSegmenter::setXStep(int step)
{
	if (m_xStep != step) {
		m_xStep = step;
		m_dirty = true;
	}
}

int ImageSegmenter::yStep()
{
	return m_yStep;
}

void ImageSegmenter::setYStep(int step)
{
	if (m_yStep != step) {
		m_yStep = step;
		m_dirty = true;
	}
}

void ImageSegmenter::setStep(int xStep, int yStep)
{
	if (m_xStep != xStep || m_yStep != yStep) {
		m_xStep = xStep;
		m_yStep = yStep;
		m_dirty = true;
	}
}

int ImageSegmenter::segmentCount() const
{
	recalcRegions();
	return m_segmentCount;
}

QRect ImageSegmenter::segmentRect(int segment) const
{
	recalcRegions();
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

QImage ImageSegmenter::segmentImage(int segment) const
{
	recalcRegions();
	Q_ASSERT(segment < m_segmentCount);

	QImage image(m_segmentSize, QImage::Format_ARGB32);
	QPainter painter(&image);
	QRect rect = segmentRect(segment);
	// Aplikujeme rovnakú transformáciu ako na obrázok a posunieme podľa
	// koordinátov regiónu
	QTransform transform = m_transform;
	transform *= QTransform::fromTranslate(-rect.left(), -rect.top());
	painter.setTransform(transform);

	painter.drawImage(QPoint(0, 0), m_sourceImage);
	painter.end();

	return image;
}

QImage ImageSegmenter::trans() const
{
	recalcRegions();
	return m_transformedImage;
}

QRect ImageSegmenter::boundingRect() const
{
	recalcRegions();
	return m_boundingRect;
}

void ImageSegmenter::recalcRegions() const
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
	m_boundingRect = QRect(x1, y1, x2, y2);

	m_lineCounts.clear();
	m_lineStarts.clear();
	int yCount = (y2 - y1) / m_yStep + 1;
	m_lineCounts.reserve(yCount);
	m_lineStarts.reserve(yCount);

	/*
	// Vytvorenie transformovaného štvorca
	QPolygonF imgRect;
	imgRect << p[0] << p[1] << p[2] << p[3] << p[0];
	*/
	// Hľadanie oblastí, ktorých stred je vo vnútri pixmapy
	m_segmentCount = 0;
	// Prechádzanie všetkých oblastí po y osi
	for (int y = y1; y <= y2; y += m_yStep) {
		/*
		int rowCount = 0;
		int firstX = 0;
		// Prechádzanie všetkých oblastí po x osi
		for (int x = x1; x <= x2; x += m_xStep) {
			// Bod je vo vnútri štvorca
			if (imgRect.containsPoint(QPointF(x, y), Qt::OddEvenFill)) {
				if (rowCount == 0) {
					firstX = x;
				}
				rowCount++;
			}
			// Od prvého bodu, ktorý nie je v konvexnej oblasti nie je žiaden bod
			// vo vnútri
			else if (rowCount > 0) {
				break;
			}
		}
		*/
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

	m_dirty = false;
}

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


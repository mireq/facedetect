/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  26.03.2011 16:45:10
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <limits>
#include "PolygonRasterizer.h"
using std::numeric_limits;

namespace FaceDetect {
namespace Utils {

/**
 * Spracovanie polygónu \a polygon na jednotlivé riadky.
 */
void PolygonRasterizer::processPolygon(const QPolygon &polygon)
{
	m_polygon = polygon;
	m_scanLines.clear();
	m_boundingRect = QRect();
	if (m_polygon.isEmpty()) {
		return;
	}

	int x1, y1, x2, y2;
	x1 = numeric_limits<int>::max();
	x2 = numeric_limits<int>::min();
	y1 = numeric_limits<int>::max();
	y2 = numeric_limits<int>::min();
	// Výpočet štvorca, v ktorom sa nachádza polygón
	foreach (const QPoint &point, polygon) {
		x1 = qMin(x1, point.x());
		x2 = qMax(x2, point.x());
		y1 = qMin(y1, point.y());
		y2 = qMax(y2, point.y());
	}
	m_boundingRect = QRect(QPoint(x1, y1), QPoint(x2, y2));

	// Inicializácia poľa riadkov
	m_scanLines.resize(y2 - y1 + 1);
	for (int y = y1; y < y2; ++y) {
		m_scanLines[y - y1].minX = numeric_limits<int>::max(); // Minimum
		m_scanLines[y - y1].maxX = numeric_limits<int>::min(); // Maximum
	}

	int polygonSize = m_polygon.size();
	// Hľadanie oblastí, ktorých stred je vo vnútri pixmapy
	for (int firstPoint = 0; firstPoint < polygonSize; ++firstPoint) {
		int secondPoint = (firstPoint + 1) % polygonSize;
		if (m_polygon[firstPoint].y() != m_polygon[secondPoint].y()) {
			// Prírastky na x-ovej a y-ovej osi
			int dx = m_polygon[secondPoint].x() - m_polygon[firstPoint].x();
			int dy = m_polygon[secondPoint].y() - m_polygon[firstPoint].y();

			// Definícia inkrementácie na x-ovej osi
			int incXHigh, incXLow;
			if (dx >= 0) {
				incXHigh = incXLow = 1;
			}
			else {
				dx = -dx;
				incXHigh = incXLow = -1;
			}

			// Definícia inkrementácie na y-ovej osi
			int incYHigh, incYLow;
			if (dy >= 0) {
				incYHigh = incYLow = 1;
			}
			else {
				dy = -dy;
				incYHigh = incYLow = -1;
			}

			// Definícia krátkej a dlhej strany
			int longD, shortD;
			if (dx >= dy) {
				longD = dx;
				shortD = dy;
				incYLow = 0;
			}
			else {
				shortD = dx;
				longD = dy;
				incXLow = 0;
			}

			// Inicializácia $d_0$
			int d = 2 * shortD - longD;

			// Definícia inkrementácií na základe posledného bodu
			int incDLow = 2 * shortD;
			int incDHigh = 2 * shortD - 2 * longD;

			int xCurrent, yCurrent;
			xCurrent = m_polygon[firstPoint].x();
			yCurrent = m_polygon[firstPoint].y();

			// Prechádzame celú čiaru
			for (int i = 0; i <= longD; ++i) {
				int lineIndex = yCurrent - y1;
				m_scanLines[lineIndex].minX = qMin(m_scanLines[lineIndex].minX, xCurrent);
				m_scanLines[lineIndex].maxX = qMax(m_scanLines[lineIndex].maxX, xCurrent);
				if (d >= 0) {
					xCurrent += incXHigh;
					yCurrent += incYHigh;
					d += incDHigh;
				}
				else {
					xCurrent += incXLow;
					yCurrent += incYLow;
					d += incDLow;
				}
			}
		}
	}
}

/**
 * Vráti zoznam riadkov pre vyplnenie polygónu. Počet riadkov je zodpovedajúci
 * výške polygónu (<tt>boundingRect.bottom() - boundingRect.top() + 1</tt>).
 * Riadky začínajú na vrchu polygónu (pri vykresľovaní treba k indexu riadku
 * prirádať pozíciu hornej hrany štvorca polygónu).
 */
const QVector<PolygonRasterizer::ScanLine> &PolygonRasterizer::scanLines() const
{
	return m_scanLines;
}

/**
 * Vráti štvorec, v ktorom sa nachádza polygón.
 */
QRect PolygonRasterizer::boundingRect() const
{
	return m_boundingRect;
}

} /* end of namespace Utils */
} /* end of namespace FaceDetect */


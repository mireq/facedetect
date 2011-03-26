/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  26.03.2011 16:45:23
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef POLYGONRASTERIZER_C7NXFS9U
#define POLYGONRASTERIZER_C7NXFS9U

#include <QPair>
#include <QPolygon>
#include <QVector>

namespace FaceDetect {
namespace Utils {

/**
 * \brief Rasterizátor konvexných polygónov
 *
 * Táto trieda vie previesť konvexný polygón na pole riadkov so zažiatočným a
 * koncovým bodom výplňe polygónu.
 */
class PolygonRasterizer
{
public:
	/**
	 * Riadok rasterizovaného polygónu.
	 */
	struct ScanLine {
		int minX; /**< Začiatok vyplnenej oblasti */
		int maxX; /**< Koniec vyplnenej oblasti   */
	};

	void processPolygon(const QPolygon &polygon);
	const QVector<ScanLine> &scanLines() const;
	QRect boundingRect() const;

private:
	/// Spracovaný polygón.
	QPolygon m_polygon;
	/// Riadky polygónu (začiatočné a koncové pozície výplne).
	QVector<ScanLine> m_scanLines;
	/// Štvorec, v ktorej sa nachádza polygón.
	QRect m_boundingRect;
}; /* -----  end of class PolygonRasterizer  ----- */

} /* end of namespace Utils */
} /* end of namespace FaceDetect */

#endif /* end of include guard: POLYGONRASTERIZER_C7NXFS9U */


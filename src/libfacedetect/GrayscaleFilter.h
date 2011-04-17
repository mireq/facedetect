/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 17:27:21
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef GRAYSCALEFILTER_J3QTUECK
#define GRAYSCALEFILTER_J3QTUECK

#include <QLinearGradient>
#include "ImageFilterBase.h"

namespace FaceDetect {

/**
 * Filter pre prevod obrazu do odtieňov šedej.
 */
class GrayscaleFilter: public ImageFilterBase
{
public:
	GrayscaleFilter();
	~GrayscaleFilter();
	void filter(QImage &sourceImage) const;
	QLinearGradient grayscaleGradient() const;
	void setGrayscaleGradient(const QLinearGradient &gradient);

private:
	void filterGrayscale(QImage &sourceImage) const;
	/// Gradient používaný pri prevode na odtiene "šedej".
	QLinearGradient m_grayscaleGradient;
	/// Počet odtieňov šedej
	static const int GrayscaleColorCount = 256;
	/// Farebná paleta s farbami šedej od 0 po 255.
	mutable QVector<QRgb> m_colorTable;
}; /* -----  end of class GrayscaleFilter  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: GRAYSCALEFILTER_J3QTUECK */


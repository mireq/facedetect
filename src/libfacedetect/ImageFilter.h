/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  18.03.2011 14:53:30
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef IMAGEFILTER_FKHAR0M2
#define IMAGEFILTER_FKHAR0M2

#include <QColor>
#include <QImage>
#include <QLinearGradient>
#include <QVector>

namespace FaceDetect {

class ImageFilter
{
public:
	enum Filter {
		NoFilter = 0x00,
		GrayscaleFilter = 0x01
	};
	Q_DECLARE_FLAGS(Filters, Filter);

	ImageFilter();
	Filters filters() const;
	void setFilters(Filters filters);
	QLinearGradient grayscaleGradient() const;
	void setGrayscaleGradient(const QLinearGradient &gradient);

	QImage filterImage(const QImage &sourceImage) const;

private:
	void filterGrayscale(QImage &sourceImage) const;

private:
	Filters m_filters;
	QLinearGradient m_grayscaleGradient;
	mutable QVector<QRgb> m_colorTable;
	static const int sm_grayscaleColorCount = 256;
}; /* -----  end of class ImageFilter  ----- */

} /* end of namespace FaceDetect */

Q_DECLARE_OPERATORS_FOR_FLAGS(FaceDetect::ImageFilter::Filters);

#endif /* end of include guard: IMAGEFILTER_FKHAR0M2 */


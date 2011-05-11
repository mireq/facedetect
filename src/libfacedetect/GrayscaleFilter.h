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
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include "utils/QtSerialization.h"
#include "ImageFilterBase.h"

namespace FaceDetect {

/**
 * \brief Filter pre prevod obrazu do odtieňov šedej.
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
	/**
	 * Serializácia filtra pre prevod do odtieňov šedej.
	 */
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		QGradientStops stops = m_grayscaleGradient.stops();
		ar & boost::serialization::make_nvp("stops", stops);
		m_grayscaleGradient.setStops(stops);
	};

	void filterGrayscale(QImage &sourceImage) const;

	/// Gradient používaný pri prevode na odtiene "šedej".
	QLinearGradient m_grayscaleGradient;
	/// Počet odtieňov šedej
	static const int GrayscaleColorCount = 256;
	/// Farebná paleta s farbami šedej od 0 po 255.
	mutable QVector<QRgb> m_colorTable;

friend class boost::serialization::access;
}; /* -----  end of class GrayscaleFilter  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: GRAYSCALEFILTER_J3QTUECK */


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

#include <QList>
#include <QVariant>
#include "GrayscaleFilter.h"
#include "IlluminationFilter.h"
#include "SobelFilter.h"
#include "GaborFilter.h"
#include "ImageFilterBase.h"

namespace FaceDetect {

/**
 * \brief Filter bitmapových obrázkov.
 *
 * Tento filter slúži na prípravu bitmapových obrázkov.
 */
class ImageFilter: public ImageFilterBase
{
public:
	/**
	 * Typy aktivovaných filtrov
	 */
	enum Filter {
		NoFilter = 0x00,           /**< Žiaden filter                      */
		GrayscaleFilter = 0x01,    /**< Filter na prevod do odtieňov šedej */
		IlluminationFilter = 0x02, /**< Filtern na korekciu osvetlenia     */
		SobelFilter = 0x04,        /**< Filter sobel pre detekciu hrán     */
		GaborFilter = 0x08         /**< Gaborova vlnková transformáia      */
	};
	Q_DECLARE_FLAGS(Filters, Filter)

	ImageFilter();
	Filters filters() const;
	void setFilters(Filters filters);
	void enableGrayscaleFilter();
	void enableGrayscaleFilter(const QLinearGradient &gradient);
	void disableGrayscaleFilter();
	void enableIlluminationFilter();
	void disableIlluminationFilter();
	void enableSobelFilter();
	void disableSobelFilter();
	void enableGaborFilter();
	void disableGaborFilter();

	QImage filterImage(const QImage &sourceImage) const;
	LaVectorDouble filterVector(const QImage &sourceImage) const;

	// Len pre štatistické účely
	void setIlluminationPlaneOnly(bool planeOnly);
	void setIlluminationCorrectHistogram(bool correctHistogram);
	void setOnlyGaborWavelet(bool wavelet);

	void setGaborParameters(const GaborFilter::GaborParameters &parameters);
	void setGaborParameters(const QList<GaborFilter::GaborParameters> &parameters);
	void filter(QImage &sourceImage) const;

	QVariant filterData() const;
	void setFilterData(const QVariant &settings);

	int subImageCount() const;

private:
	void normalizeImage(QImage &sourceImage) const;

private:
	/// Aktivované filtre.
	Filters m_filters;
	/// Filter pre prevod do odtieňov šedej
	FaceDetect::GrayscaleFilter m_grayscaleFilter;
	/// Filter pre korekciu osvetlenia.
	FaceDetect::IlluminationFilter m_illuminationFilter;
	/// Filter pre detekciu hrán - sobel
	FaceDetect::SobelFilter m_sobelFilter;
	/// Filtre pre detekciu hrán - gabor
	QVector<FaceDetect::GaborFilter> m_gaborFilters;
	/// Vykreslenie len gaborovej vlnkovej funkcie
	bool m_onlyGaborWavelet;
}; /* -----  end of class ImageFilter  ----- */

} /* end of namespace FaceDetect */

Q_DECLARE_OPERATORS_FOR_FLAGS(FaceDetect::ImageFilter::Filters)

#endif /* end of include guard: IMAGEFILTER_FKHAR0M2 */


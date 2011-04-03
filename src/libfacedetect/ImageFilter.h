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
#include <lapackpp/lavd.h>

namespace FaceDetect {

/**
 * \brief Filter bitmapových obrázkov.
 *
 * Tento filter slúži na prípravu bitmapových obrázkov.
 */
class ImageFilter
{
public:
	/**
	 * Typy aktivovaných filtrov
	 */
	enum Filter {
		NoFilter = 0x00,           /**< Žiaden filter                      */
		GrayscaleFilter = 0x01,    /**< Filter na prevod do odtieňov šedej */
		IlluminationFilter = 0x02, /**< Filtern na korekciu osvetlenia     */
		SobelFilter = 0x04         /**< Filter sobel pre detekciu hrán     */
	};
	Q_DECLARE_FLAGS(Filters, Filter)

	ImageFilter();
	Filters filters() const;
	void setFilters(Filters filters);
	QLinearGradient grayscaleGradient() const;
	void setGrayscaleGradient(const QLinearGradient &gradient);

	QImage filterImage(const QImage &sourceImage) const;
	LaVectorDouble filterVector(const QImage &sourceImage) const;

	// Len pre štatistické účely
	void setIlluminationPlaneOnly(bool planeOnly);
	void setIlluminationCorrectHistogram(bool correctHistogram);

private:
	void filterHelper(QImage &sourceImage) const;
	void equalizeHistogram(QImage &sourceImage) const;
	void normalizeImage(QImage &sourceImage) const;
	void filterGrayscale(QImage &sourceImage) const;
	void filterIllumination(QImage &sourceImage) const;
	void filterSobel(QImage &sourceImage) const;

private:
	/// Aktivované filtre.
	Filters m_filters;
	/// Zobrazenie korekčnej iluminačnej plochy.
	bool m_illuminationPlaneOnly;
	/// Roztiahnutie svetlosti pri korekcii osvetlenia.
	bool m_illuminationCorrectHistogram;
	/// Gradient používaný pri prevode na odtiene "šedej".
	QLinearGradient m_grayscaleGradient;
	/// Parametre osvetľovacieho gradientu
	mutable LaGenMatDouble m_illuminationPlane;
	/// Matica koordinátov
	mutable LaGenMatDouble m_imageCoordinateMatrix;
	/// Spracovaná matica pre výpočet svetelnej korekcie
	mutable LaGenMatDouble m_ilCorrectionMatrix;
	/// Farebná paleta s farbami šedej od 0 po 255.
	mutable QVector<QRgb> m_colorTable;
	/// Počet odtieňov šedej
	static const int GrayscaleColorCount = 256;
}; /* -----  end of class ImageFilter  ----- */

} /* end of namespace FaceDetect */

Q_DECLARE_OPERATORS_FOR_FLAGS(FaceDetect::ImageFilter::Filters)

#endif /* end of include guard: IMAGEFILTER_FKHAR0M2 */


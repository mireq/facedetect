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
#include <valarray>

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
	 * Vlastnosti gaborovej vlnkovej funkcie.
	 */
	struct GaborParameters {
		/**
		 * Inicializácia štandardných hodnôt.
		 */
		GaborParameters(): lambda(4.0), theta(0.0), psi(0.0), sigma(2.0), gamma(1.0), lum(0.0) {};
		double lambda; /**< Parameter lambda (dĺžka vlny).              */
		double theta;  /**< Parameter theta (uhol sínusoidy).           */
		double psi;    /**< Parameter psi (posun sínusoidy).            */
		double sigma;  /**< Parameter sigma (veľkosť gaussovej obálky). */
		double gamma;  /**< Parameter gamma (priestorový pomer strán).  */
		double lum;    /**< Korekcia hodnoty osvetlenia.                */
	};

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
	QLinearGradient grayscaleGradient() const;
	void setGrayscaleGradient(const QLinearGradient &gradient);

	QImage filterImage(const QImage &sourceImage) const;
	LaVectorDouble filterVector(const QImage &sourceImage) const;

	// Len pre štatistické účely
	void setIlluminationPlaneOnly(bool planeOnly);
	void setIlluminationCorrectHistogram(bool correctHistogram);
	void setOnlyGaborWavelet(bool wavelet);
	void setGaborParameters(const ImageFilter::GaborParameters &parameters);

private:
	void filterHelper(QImage &sourceImage) const;
	void equalizeHistogram(QImage &sourceImage) const;
	void normalizeImage(QImage &sourceImage) const;
	void filterGrayscale(QImage &sourceImage) const;
	void filterIllumination(QImage &sourceImage) const;
	void filterSobel(QImage &sourceImage) const;
	void filterGabor(QImage &sourceImage) const;
	void calcGaborConvolution(QImage *sourceImage = 0) const;
	void applyConvolution(QImage &sourceImage, const std::valarray<double> &convolX, const std::valarray<double> &convolY, int matrixSizeX, int matrixSizeY, double luminanceCorrection) const;

private:
	/// Aktivované filtre.
	Filters m_filters;
	/// Zobrazenie korekčnej iluminačnej plochy.
	bool m_illuminationPlaneOnly;
	/// Roztiahnutie svetlosti pri korekcii osvetlenia.
	bool m_illuminationCorrectHistogram;
	/// Vrátenie len gaborovej vlnkovej funkcie.
	bool m_onlyGaborWavelet;
	/// Parametre gabor filtra.
	GaborParameters m_gaborParameters;
	/// Ak je \e true je potrebné prepočítať konvolučnú maticu.
	mutable bool m_gaborRecalcConvolution;
	/// Konvolučná matica pre gabor filter v horizontálnom smere.
	mutable std::valarray<double> m_gaborConvolutionX;
	/// Konvolučná matica pre gabor filter vo vertikálnom smere.
	mutable std::valarray<double> m_gaborConvolutionY;
	/// Veľkosť konvolučnej matice pre gabor filter.
	mutable QSize m_gaborConvolutionSize;
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


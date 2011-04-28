/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 20:45:58
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef GABORFILTER_SCVINMZ1
#define GABORFILTER_SCVINMZ1

#include <valarray>
#include "ImageFilterBase.h"

namespace FaceDetect {

/**
 * \brief Gaborov filter pre detekciu hrán.
 */
class GaborFilter: public ImageFilterBase
{
public:
	/**
	 * \brief Vlastnosti gaborovej vlnkovej funkcie.
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

	GaborFilter();
	~GaborFilter();
	void filter(QImage &sourceImage) const;

	void setGaborParameters(const GaborFilter::GaborParameters &parameters);
	// Len pre štatistické účely
	void setOnlyGaborWavelet(bool wavelet);

private:
	void calcGaborConvolution(QImage *sourceImage = 0) const;
	void applyConvolution(QImage &sourceImage, const std::valarray<double> &convolX, const std::valarray<double> &convolY, int matrixSizeX, int matrixSizeY, double luminanceCorrection) const;

private:
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

}; /* -----  end of class GaborFilter  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: GABORFILTER_SCVINMZ1 */


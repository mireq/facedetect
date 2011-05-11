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
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include "utils/QtSerialization.h"
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
		GaborParameters(): lambda(2.0), theta(0.0), psi(0.0), sigma(1.0), gamma(1.0), lum(0.0) {};
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

	GaborFilter::GaborParameters gaborParameters() const;
	void setGaborParameters(const GaborFilter::GaborParameters &parameters);
	// Len pre štatistické účely
	bool onlyGaborWavelet() const;
	void setOnlyGaborWavelet(bool wavelet);

private:
	void calcGaborConvolution(QImage *sourceImage = 0) const;
	void applyConvolution(QImage &sourceImage, const std::valarray<double> &convolX, const std::valarray<double> &convolY, int matrixSizeX, int matrixSizeY, double luminanceCorrection) const;
	/**
	 * Serializácia gaborovho filtra.
	 */
	template<class Archive> void serialize(Archive &ar, const unsigned int version) {
		Q_UNUSED(version);
		ar & boost::serialization::make_nvp("onlyGaborWavelet", m_onlyGaborWavelet);
		ar & boost::serialization::make_nvp("lambda", m_gaborParameters.lambda);
		ar & boost::serialization::make_nvp("theta", m_gaborParameters.theta);
		ar & boost::serialization::make_nvp("psi", m_gaborParameters.psi);
		ar & boost::serialization::make_nvp("sigma", m_gaborParameters.sigma);
		ar & boost::serialization::make_nvp("gamma", m_gaborParameters.gamma);
		ar & boost::serialization::make_nvp("lum", m_gaborParameters.lum);
	};

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

friend class boost::serialization::access;
}; /* -----  end of class GaborFilter  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: GABORFILTER_SCVINMZ1 */


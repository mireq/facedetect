/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 20:45:55
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "GaborFilter.h"
#include <boost/math/constants/constants.hpp>
#include <lapackpp/blas3pp.h>
#include <lapackpp/laexcp.h>

#include <QDebug>
using std::ceil;
using std::cos;
using std::exp;
using std::sin;

namespace FaceDetect {

GaborFilter::GaborFilter():
	ImageFilterBase(),
	m_onlyGaborWavelet(false),
	m_gaborRecalcConvolution(true)
{
}

GaborFilter::~GaborFilter()
{
}

/**
 * Použitie gabor filtra na obrázok \a sourceImage. Parametre filtra sa
 * nastavujú volaním setGabofParameters.
 */
void GaborFilter::filter(QImage &sourceImage) const
{
	if (m_onlyGaborWavelet) {
		calcGaborConvolution(&sourceImage);
	}
	else {
		if (m_gaborRecalcConvolution) {
			calcGaborConvolution();
			m_gaborRecalcConvolution = false;
		}
		applyConvolution(sourceImage, m_gaborConvolutionX, m_gaborConvolutionY, m_gaborConvolutionSize.width(), m_gaborConvolutionSize.height(), m_gaborParameters.lum);
	}
}

/**
 * Nastavenie parametrov gabor filtra.
 */
void GaborFilter::setGaborParameters(const GaborFilter::GaborParameters &parameters)
{
	m_gaborParameters = parameters;
	m_gaborRecalcConvolution = true;
}

/**
 * Nastavenie vrátenia len gaborovej vlnky.
 */
void GaborFilter::setOnlyGaborWavelet(bool wavelet)
{
	m_onlyGaborWavelet = wavelet;
}

/**
 * Výpočet konvolučnej matice gabor filtra.
 */
void GaborFilter::calcGaborConvolution(QImage *sourceImage) const
{
	double sigmaX = m_gaborParameters.sigma;
	double sigmaY = m_gaborParameters.sigma / m_gaborParameters.gamma;
	double sigmaXQuad = sigmaX * sigmaX;
	double sigmaYQuad = sigmaY * sigmaY;

	double nstds = 3;
	double xMaxf = qMax(qAbs(nstds * sigmaX * cos(m_gaborParameters.theta)), qAbs(nstds * sigmaY * sin(m_gaborParameters.theta)));
	xMaxf = ceil(qMax(1.0, xMaxf));
	double yMaxf = qMax(qAbs(nstds * sigmaX * sin(m_gaborParameters.theta)), qAbs(nstds * sigmaY * cos(m_gaborParameters.theta)));
	yMaxf = ceil(qMax(1.0, yMaxf));
	int xMax = xMaxf;
	int yMax = yMaxf;
	int xMin = -xMax;
	int yMin = -yMax;
	m_gaborConvolutionSize = QSize(xMax - xMin + 1, yMax - yMin + 1);
	if (sourceImage != 0) {
		*sourceImage = QImage(m_gaborConvolutionSize, QImage::Format_ARGB32);
	}
	m_gaborConvolutionX = std::valarray<double>(m_gaborConvolutionSize.width() * m_gaborConvolutionSize.height());
	m_gaborConvolutionY = m_gaborConvolutionX;
	double theta = m_gaborParameters.theta;
	double xTheta = 0;
	double yTheta = 0;
	double pi = boost::math::constants::pi<double>();
	std::size_t bytePos = 0;
	for (int yPos = yMin; yPos <= yMax; ++yPos) {
		for (int xPos = xMin; xPos <= xMax; ++xPos) {
			xTheta = static_cast<double>(xPos) * cos(theta) + static_cast<double>(yPos) * sin(theta);
			yTheta = static_cast<double>(-xPos) * sin(theta) + static_cast<double>(yPos) * cos(theta);
			double xThetaQuad = xTheta * xTheta;
			double yThetaQuad = yTheta * yTheta;
			double expVal = exp(-0.5 * (xThetaQuad / sigmaXQuad + yThetaQuad / sigmaYQuad));
			double phase = 2.0 * pi * xTheta / m_gaborParameters.lambda + m_gaborParameters.psi;
			double real = expVal * cos(phase);
			double imag = expVal * sin(phase);
			m_gaborConvolutionX[bytePos] = real;
			m_gaborConvolutionY[bytePos] = imag;
			if (sourceImage != 0) {
				int pixelVal = imag * 128 + 127;
				sourceImage->setPixel(xPos - xMin, yPos - yMin, qRgb(pixelVal, pixelVal, pixelVal));
			}
			bytePos ++;
		}
	}
}

/**
 * Aplikovanie všeobecnej konvolučnej matice \a convolX v horizontálnom smere a
 * \a convolY vo vertikálnom smere na obrázok \a sourceImage kde veľkost
 * konvolučnej matice je \a matrixSizeX a \a matrixSizeY.
 */
void GaborFilter::applyConvolution(QImage &sourceImage, const std::valarray<double> &convolX, const std::valarray<double> &convolY, int matrixSizeX, int matrixSizeY, double luminanceCorrection) const
{
	if (sourceImage.depth() < 8) {
		return;
	}

	QImage imgCopy = sourceImage;

	int imgWidth = imgCopy.width();
	int imgHeight = imgCopy.height();
	int depth = imgCopy.depth() / 8;
	int bytesPerLine = imgWidth * depth;
	int convolCenterX = (matrixSizeX - 1) / 2;
	int convolCenterY = (matrixSizeY - 1) / 2;
	int convolByteX = matrixSizeX * depth;
	std::valarray<double> rawImage;
	double maxRaw = 0;
	if (luminanceCorrection == 0) {
		rawImage = std::valarray<double>(imgHeight * imgWidth * depth);
	}

	// Pre každý pixel obrázku
	int bytePos = 0;
	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		int yMin = yPos - convolCenterY;
		int yMax = yMin + matrixSizeY;
		uchar *sourceLine = sourceImage.scanLine(yPos);
		for (int xPos = 0; xPos < bytesPerLine; ++xPos) {
			double reSum = 0;
			double imSum = 0;
			int xMin = xPos - convolCenterX * depth;
			int xMax = xMin + convolByteX;
			int convolPos = 0;
			for (int yMat = yMin; yMat < yMax; ++yMat) {
				if (yMat >= 0 && yMat < imgHeight) {
					const uchar *line = imgCopy.scanLine(yMat);
					for (int xMat = xMin; xMat < xMax; xMat += depth) {
						if (xMat < 0 || xMat >= bytesPerLine - 1) {
							reSum += sourceLine[xPos] * convolX[convolPos];
							imSum += sourceLine[xPos] * convolY[convolPos];
						}
						else {
							reSum += line[xMat] * convolX[convolPos];
							imSum += line[xMat] * convolY[convolPos];
						}
						convolPos++;
					}
				}
				else {
					for (int xMat = xMin; xMat < xMax; xMat += depth) {
						reSum += sourceLine[xPos] * convolX[convolPos];
						imSum += sourceLine[xPos] * convolY[convolPos];
						convolPos++;
					}
				}
			}
			if (luminanceCorrection == 0) {
				double val = sqrt((reSum * reSum) + (imSum * imSum));
				if (maxRaw < val) {
					maxRaw = val;
				}
				rawImage[bytePos] = val;
			}
			else {
				sourceLine[xPos] = uchar(sqrt((reSum * reSum) + (imSum * imSum)) * luminanceCorrection);
			}
			bytePos++;
		}
	}

	if (luminanceCorrection == 0 && maxRaw != 0) {
		bytePos = 0;
		for (int yPos = 0; yPos < imgHeight; ++yPos) {
			uchar *sourceLine = sourceImage.scanLine(yPos);
			for (int xPos = 0; xPos < bytesPerLine; ++xPos) {
				sourceLine[xPos] = uchar(255 * rawImage[bytePos] / maxRaw);
				bytePos++;
			}
		}
	}
}
} /* end of namespace FaceDetect */


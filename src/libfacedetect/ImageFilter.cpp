/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  18.03.2011 14:53:27
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QGenericMatrix>
#include <QPainter>
#include <cmath>
#include <boost/math/constants/constants.hpp>
#include <lapackpp/blas3pp.h>
#include <lapackpp/laslv.h>
#include <lapackpp/lavli.h>
#include <lapackpp/laexcp.h>
#include "ImageFilter.h"

using std::ceil;
using std::cos;
using std::exp;
using std::sin;
using std::sqrt;

namespace FaceDetect {

ImageFilter::ImageFilter():
	m_filters(NoFilter),
	m_illuminationPlaneOnly(false),
	m_illuminationCorrectHistogram(true),
	m_onlyGaborWavelet(false),
	m_gaborRecalcConvolution(true)
{
	m_grayscaleGradient.setColorAt(0, Qt::black);
	m_grayscaleGradient.setColorAt(1, Qt::white);
}

/**
 * Vráti aktivované filtre.
 */
ImageFilter::Filters ImageFilter::filters() const
{
	return m_filters;
}

/**
 * Nastavenie aktivovaných filtrov.
 */
void ImageFilter::setFilters(Filters filters)
{
	m_filters = filters;
}

/**
 * Vráti prechod používaný pri prevode do odtieňov šedej.
 */
QLinearGradient ImageFilter::grayscaleGradient() const
{
	return m_grayscaleGradient;
}

/**
 * Štandardne má počiatočnú hodnotu čiernu a konečnú bielu. Nastavením iného
 * gradientu sa dá dosiahnuť aby obrázok v odtieňoch šedej mal v skutočnosti
 * farebnú mapu zodpovedajúcu gradientu.
 */
void ImageFilter::setGrayscaleGradient(const QLinearGradient &gradient)
{
	m_grayscaleGradient = gradient;
	m_grayscaleGradient.setStart(0, 0);
	m_grayscaleGradient.setFinalStop(GrayscaleColorCount, 0);
}

/**
 * Použitie aktivovaných filtrov na obrázkok \a sourceImage. Výsledkom použitia
 * filtrov je návratová hodnota
 */
QImage ImageFilter::filterImage(const QImage &sourceImage) const
{
	QImage ret = sourceImage;
	filterHelper(ret);
	return ret;
}

/**
 * Použitie aktivovaných filtrov na obrázok a prevod na vektor.
 */
LaVectorDouble ImageFilter::filterVector(const QImage &src) const
{
	const QImage *sourceImage = &src;
	QImage img;
	if (m_filters != NoFilter) {
		img = src;
		filterHelper(img);
		filterGrayscale(img);
		sourceImage = &img;
	}

	int rows = sourceImage->height();
	int cols = sourceImage->width();
	LaVectorDouble vect(rows * cols, 1);
	double *data = vect.addr();
	int addr = 0;
	for (int row = 0; row < rows; ++row) {
		const uchar *line = sourceImage->scanLine(row);
		for (int col = 0; col < cols; ++col) {
			data[addr] = static_cast<double>(line[col]) / 256.0;
			++addr;
		}
	}
	return vect;
}

/**
 * Ak je \a planeOnly nastavené na \e true namiesto korigovaného obrázku vráti
 * filter len korekčnú plochu.
 */
void ImageFilter::setIlluminationPlaneOnly(bool planeOnly)
{
	m_illuminationPlaneOnly = planeOnly;
}

/**
 * Ak je \a correctHistogram nastavené na \e true bude histogram obrázku
 * s upraveným osvetlením roztiahnutý na celý rozsah.
 */
void ImageFilter::setIlluminationCorrectHistogram(bool correctHistogram)
{
	m_illuminationCorrectHistogram = correctHistogram;
}

/**
 * Nastavenie vrátenia len gaborovej vlnky.
 */
void ImageFilter::setOnlyGaborWavelet(bool wavelet)
{
	m_onlyGaborWavelet = wavelet;
}

/**
 * Nastavenie parametrov gabor filtra.
 */
void ImageFilter::setGaborParameters(const ImageFilter::GaborParameters &parameters)
{
	m_gaborParameters = parameters;
}

/**
 * Použitie filtrov na zdrojový obrázok \a sourceImage pričom samotný zdrojový
 * obrázok je modifikovaný.
 */
void ImageFilter::filterHelper(QImage &sourceImage) const
{
	if (m_filters & GrayscaleFilter) {
		filterGrayscale(sourceImage);
		// Ak nie je gradient čiernobiely nastavíme farby podľa gradientu
		if (m_grayscaleGradient.stops().count() != 2 ||
		    m_grayscaleGradient.stops()[0].second != Qt::black ||
		    m_grayscaleGradient.stops()[1].second != Qt::white) {
			QImage gradImage(QSize(GrayscaleColorCount, 1), QImage::Format_RGB888);
			QPainter gradPainter(&gradImage);
			gradPainter.setPen(Qt::NoPen);
			gradPainter.fillRect(QRect(0, 0, GrayscaleColorCount, 1), m_grayscaleGradient);
			gradPainter.end();

			QVector<QRgb> colorTable(GrayscaleColorCount);
			for (int colorIdx = 0; colorIdx < GrayscaleColorCount; ++colorIdx) {
				colorTable[colorIdx] = gradImage.pixel(colorIdx, 0);
			}
			sourceImage.setColorTable(colorTable);
		}
	}
	if (m_filters & IlluminationFilter) {
		filterIllumination(sourceImage);
	}
	if (m_filters & SobelFilter) {
		filterSobel(sourceImage);
	}
	if (m_filters & GaborFilter) {
		filterGabor(sourceImage);
	}
}

/**
 * Ekvalizácia histogramu.
 */
void ImageFilter::equalizeHistogram(QImage &sourceImage) const
{
	if (sourceImage.depth() < 8) {
		return;
	}

	int imgWidth = sourceImage.width();
	int imgHeight = sourceImage.height();
	int depth = sourceImage.depth() / 8;
	uchar *imgData = 0;

	// Početnosť hodnoty pixelov
	int frequencies[256];
	for (int freq = 0; freq < 256; ++freq) {
		frequencies[freq] = 0;
	}

	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		int dataPos = 0;
		for (int xPos = 0; xPos < imgWidth; ++xPos) {
			for (int bit = 0; bit < depth; ++bit) {
				frequencies[imgData[dataPos]]++;
				dataPos++;
			}
		}
	}

	// Vypočet kumulatívnych početností
	int cumulative = 0;
	for (int freq = 0; freq < 256; ++freq) {
		cumulative += frequencies[freq];
		frequencies[freq] = cumulative;
	}

	// Korekcia histogramu
	double alpha = 255.0 / (imgWidth * imgHeight * depth);
	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		int dataPos = 0;
		for (int xPos = 0; xPos < imgWidth; ++xPos) {
			for (int bit = 0; bit < depth; ++bit) {
				imgData[dataPos] = frequencies[imgData[dataPos]] * alpha;
				dataPos++;
			}
		}
	}
}

/**
 * Normalizácia obrázku na plný rozsah 0 - 255.
 */
void ImageFilter::normalizeImage(QImage &sourceImage) const
{
	if (sourceImage.depth() < 8) {
		return;
	}

	int imgWidth = sourceImage.width();
	int imgHeight = sourceImage.height();
	int depth = sourceImage.depth() / 8;
	uchar *imgData = 0;

	int min = 255;
	int max = 0;
	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		for (int dataPos = 0; dataPos < imgWidth * depth; ++dataPos) {
			if (imgData[dataPos] < min) {
				min = imgData[dataPos];
			}
			if (imgData[dataPos] > max) {
				max = imgData[dataPos];
			}
		}
	}
	if (max == min) {
		return;
	}

	double inv = 255.0 / (max - min);
	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		for (int dataPos = 0; dataPos < imgWidth * depth; ++dataPos) {
			imgData[dataPos] = imgData[dataPos] * inv + min;
		}
	}
}

/**
 * Prevod obrázku do indexovaných farieb s farebnou tabuľkou, ktorá obsahuje
 * odtiene šedej od (0, 0, 0) po (255, 255, 255).
 */
void ImageFilter::filterGrayscale(QImage &sourceImage) const
{
	if (sourceImage.format() == QImage::Format_Indexed8) {
		return;
	}
	if (m_colorTable.isEmpty()) {
		m_colorTable.resize(256);
		for (int i = 0; i < 256; ++i) {
			m_colorTable[i] = qRgb(i, i, i);
		}
	}
	sourceImage = sourceImage.convertToFormat(QImage::Format_Indexed8, m_colorTable);
}

/**
 * Korekcia osvetlenia pomocou najlepšieho gradientu.
 */
void ImageFilter::filterIllumination(QImage &sourceImage) const
{
	int depth = sourceImage.depth() / 8;
	int imgWidth = sourceImage.width();
	int imgHeight = sourceImage.height();
	int resolution = imgWidth * imgHeight;
	int byteCount = resolution * depth;

	// Inicializácia matíc
	if (m_illuminationPlane.rows() != 3) {
		m_illuminationPlane = LaGenMatDouble(3, 1);
	}
	if (m_imageCoordinateMatrix.rows() != byteCount) {
		m_imageCoordinateMatrix = LaGenMatDouble(byteCount, 3);
		int row = 0;
		int column = 0;
		int bit = 0;
		for (int pixel = 0; pixel < byteCount; ++pixel) {
			m_imageCoordinateMatrix(pixel, 0) = column;
			m_imageCoordinateMatrix(pixel, 1) = row;
			m_imageCoordinateMatrix(pixel, 2) = 1;
			bit++;
			if (bit == depth) {
				bit = 0;
				column++;
			}
			if (column == imgWidth) {
				column = 0;
				row++;
			}
		}

		m_ilCorrectionMatrix = LaGenMatDouble(3, byteCount);
		LaGenMatDouble pom = LaGenMatDouble(3, 3);
		// Výpočet $\textbf{N}^T \textbf{N}$
		Blas_Mat_Mat_Mult(m_imageCoordinateMatrix, m_imageCoordinateMatrix, pom, true);

		// Inverzia pomocnej premennej ($(\textbf{N}^T \textbf{N})^-1$)
		LaVectorLongInt pivots(3);
		LUFactorizeIP(pom, pivots);
		LaLUInverseIP(pom, pivots);

		// $(\textbf{N}^T \textbf{N})^-1$) \textbf{N}^T$
		Blas_Mat_Mat_Mult(pom, m_imageCoordinateMatrix, m_ilCorrectionMatrix, false, true);
	}

	// Naplnenie vektoru $\textbf{x}$
	LaVectorDouble xVect(resolution * depth);
	uchar *imgData = sourceImage.bits();
	int xPos = 0;
	int yPos = 0;

	for (int pixel = 0; pixel < byteCount; ++pixel) {
		xVect(pixel) = imgData[xPos];
		xPos++;
		if (xPos == imgWidth * depth) {
			xPos = 0;
			yPos++;
			imgData = sourceImage.scanLine(yPos);
		}
	}

	Blas_Mat_Mat_Mult(m_ilCorrectionMatrix, xVect, m_illuminationPlane);

	// IP(x, y) = a * x + b * y + c
	double a = m_illuminationPlane(0, 0);
	double b = m_illuminationPlane(1, 0);
	double c = m_illuminationPlane(2, 0);
	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		imgData = sourceImage.scanLine(yPos);
		int bitPos = 0;
		for (int xPos = 0; xPos < imgWidth; xPos++) {
			for (int bitId = 0; bitId < depth; ++bitId) {
				int ipVal = a * bitPos + b * yPos + c;
				int bit = imgData[bitPos] - ipVal + 127;
				bit = qMin(qMax(bit, 0), 255);
				if (m_illuminationPlaneOnly) {
					imgData[bitPos] = ipVal;
				}
				else {
					imgData[bitPos] = bit;
				}
				bitPos++;
			}
		}
	}

	if (m_illuminationPlaneOnly || !m_illuminationCorrectHistogram) {
		return;
	}

	equalizeHistogram(sourceImage);
}

/**
 * Detekcia hrán obrazu filtrom sobel.
 */
void ImageFilter::filterSobel(QImage &sourceImage) const
{
	if (sourceImage.depth() < 8) {
		return;
	}

	QImage imgCopy = sourceImage;

	int imgWidth = imgCopy.width();
	int imgHeight = imgCopy.height();
	int depth = imgCopy.depth() / 8;
	int bytesPerLine = imgWidth * depth;
	double maxComb = sqrt(20.0);

	uchar *copyLine = 0;
	uchar *sourceLine = 0;
	uchar *prevLine = 0;
	uchar *nextLine = 0;
	int imgFragment[3][3];
	imgFragment[1][1] = 0;

	for (int yPos = 0; yPos < imgHeight; ++yPos) {
		if (yPos == 0) {
			prevLine = 0;
		}
		else {
			prevLine = imgCopy.scanLine(yPos - 1);
		}
		if (yPos == imgHeight - 1) {
			nextLine = 0;
		}
		else {
			nextLine = imgCopy.scanLine(yPos + 1);
		}
		copyLine = imgCopy.scanLine(yPos);
		sourceLine = sourceImage.scanLine(yPos);

		for (int bytePos = 0; bytePos < bytesPerLine; ++bytePos) {
			int xGrad = 0;
			int yGrad = 0;
			if (prevLine == 0) {
				imgFragment[0][0] = copyLine[bytePos];
				imgFragment[0][1] = copyLine[bytePos];
				imgFragment[0][2] = copyLine[bytePos];
			}
			else {
				imgFragment[0][0] = bytePos - depth < 0 ? copyLine[bytePos] : prevLine[bytePos - depth];
				imgFragment[0][1] = prevLine[bytePos];
				imgFragment[0][2] = bytePos + depth >= bytesPerLine ? copyLine[bytePos] : prevLine[bytePos + depth];
			}

			imgFragment[1][0] = bytePos - depth < 0 ? copyLine[bytePos] : copyLine[bytePos - depth];
			imgFragment[1][2] = bytePos + depth >= bytesPerLine ? copyLine[bytePos] : copyLine[bytePos + depth];

			if (nextLine == 0) {
				imgFragment[2][0] = copyLine[bytePos];
				imgFragment[2][1] = copyLine[bytePos];
				imgFragment[2][2] = copyLine[bytePos];
			}
			else {
				imgFragment[2][0] = bytePos - depth < 0 ? copyLine[bytePos] : nextLine[bytePos - depth];
				imgFragment[2][1] = nextLine[bytePos];
				imgFragment[2][2] = bytePos + depth >= bytesPerLine ? copyLine[bytePos] : nextLine[bytePos + depth];
			}
			xGrad = imgFragment[0][0] - imgFragment[0][2] + 2 * imgFragment[1][0] - 2 * imgFragment[1][2] + imgFragment[0][0] - imgFragment[0][2];
			yGrad = imgFragment[0][0] + 2 * imgFragment[0][1] + imgFragment[0][2] - imgFragment[2][0] - 2 * imgFragment[2][1] - imgFragment[2][2];
			sourceLine[bytePos] = sqrt(xGrad * xGrad + yGrad * yGrad) / maxComb;
		}
	}

	//normalizeImage(sourceImage);
}

/**
 * Použitie gabor filtra na obrázok \a sourceImage. Parametre filtra sa
 * nastavujú volaním setGabofParameters.
 */
void ImageFilter::filterGabor(QImage &sourceImage) const
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
 * Výpočet konvolučnej matice gabor filtra.
 */
void ImageFilter::calcGaborConvolution(QImage *sourceImage) const
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
void ImageFilter::applyConvolution(QImage &sourceImage, const std::valarray<double> &convolX, const std::valarray<double> &convolY, int matrixSizeX, int matrixSizeY, double luminanceCorrection) const
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


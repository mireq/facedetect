/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  18.03.2011 14:53:27
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "ImageFilter.h"

namespace FaceDetect {

ImageFilter::ImageFilter():
	ImageFilterBase(),
	m_filters(NoFilter)
{
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
 * Povolenie prevodu do odtieňov šedej.
 */
void ImageFilter::enableGrayscaleFilter()
{
	m_filters |= ImageFilter::GrayscaleFilter;
}

/**
 * Povolenie prevodu do odtieňov šedej a nastavenie vlastného lineárneho
 * gradientu.
 */
void ImageFilter::enableGrayscaleFilter(const QLinearGradient &gradient)
{
	enableGrayscaleFilter();
	m_grayscaleFilter.setGrayscaleGradient(gradient);
}

/**
 * Zákaz prevodu od odtieňov šedej.
 */
void ImageFilter::disableGrayscaleFilter()
{
	m_filters &= ~(ImageFilter::GrayscaleFilter);
}

/**
 * Povolenie filtra korigujúceho osvetlenie.
 */
void ImageFilter::enableIlluminationFilter()
{
	m_filters |= ImageFilter::IlluminationFilter;
}

/**
 * Zákaz filtra korigujúceho osvetlenie.
 */
void ImageFilter::disableIlluminationFilter()
{
	m_filters &= ~(ImageFilter::IlluminationFilter);
}

/**
 * Zapnutie filtra sobel pre detekciu hrán.
 */
void ImageFilter::enableSobelFilter()
{
	m_filters |= ImageFilter::SobelFilter;
}

/**
 * Vypnutie filtra sobel pre detekciu hrán.
 */
void ImageFilter::disableSobelFilter()
{
	m_filters &= ~(ImageFilter::SobelFilter);
}

/**
 * Zapnutie gaborovho filtra pre detekciu hrán.
 */
void ImageFilter::enableGaborFilter()
{
	m_filters |= ImageFilter::GaborFilter;
}

/**
 * Vypnutie gaborovho filtra pre detekciu hrán.
 */
void ImageFilter::disableGaborFilter()
{
	m_filters &= ~(ImageFilter::GaborFilter);
}

/**
 * Použitie aktivovaných filtrov na obrázkok \a sourceImage. Výsledkom použitia
 * filtrov je návratová hodnota
 */
QImage ImageFilter::filterImage(const QImage &sourceImage) const
{
	QImage ret = sourceImage;
	filter(ret);
	return ret;
}

/**
 * Použitie aktivovaných filtrov na obrázok a prevod na vektor.
 */
LaVectorDouble ImageFilter::filterVector(const QImage &src) const
{
	const QImage *sourceImage = &src;
	QImage img;
	if (m_filters != NoFilter || sourceImage->depth() % 8 != 0) {
		img = src;
		filter(img);
		sourceImage = &img;
	}

	if (sourceImage->depth() % 8 != 0) {
		img = img.convertToFormat(QImage::Format_ARGB32);
		sourceImage = &img;
	}

	int rows = sourceImage->height();
	int cols = sourceImage->width() * (sourceImage->depth() / 8);
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
 * Nastavenie parametru filtra korigujúceho osvetlenie.
 * \sa FaceDetect::IlluminationFilter::setIlluminationPlaneOnly()
 */
void ImageFilter::setIlluminationPlaneOnly(bool planeOnly)
{
	m_illuminationFilter.setIlluminationPlaneOnly(planeOnly);
}

/**
 * Nastavenie parametru filtra korigujúceho osvetlenie.
 * \sa FaceDetect::IlluminationFilter::setIlluminationCorrectHistogram()
 */
void ImageFilter::setIlluminationCorrectHistogram(bool correctHistogram)
{
	m_illuminationFilter.setIlluminationCorrectHistogram(correctHistogram);
}

/**
 * Nastavenie vrátenia len gaborovej vlnkovej funkcie.
 * \sa FaceDetect::GaborFilter::setOnlyGaborWavelet()
 */
void ImageFilter::setOnlyGaborWavelet(bool wavelet)
{
	m_gaborFilter.setOnlyGaborWavelet(wavelet);
}

/**
 * Nastavenie parametrov gabor filtra.
 * \sa FaceDetect::GaborFilter::setGaborParameters()
 */
void ImageFilter::setGaborParameters(const GaborFilter::GaborParameters &parameters)
{
	m_gaborFilter.setGaborParameters(parameters);
}

/**
 * Použitie filtrov na zdrojový obrázok \a sourceImage pričom samotný zdrojový
 * obrázok je modifikovaný.
 */
void ImageFilter::filter(QImage &sourceImage) const
{
	if (m_filters & GrayscaleFilter) {
		m_grayscaleFilter.filter(sourceImage);
	}
	if (m_filters & IlluminationFilter) {
		m_illuminationFilter.filter(sourceImage);
	}
	if (m_filters & SobelFilter) {
		m_sobelFilter.filter(sourceImage);
	}
	if (m_filters & GaborFilter) {
		m_gaborFilter.filter(sourceImage);
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


} /* end of namespace FaceDetect */


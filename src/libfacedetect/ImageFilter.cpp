/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  18.03.2011 14:53:27
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "ImageFilter.h"
#include <cstring>
#include <QVariant>
#include <QVariantMap>
#include <boost/math/constants/constants.hpp>
#include <QDebug>

using std::memcpy;

namespace FaceDetect {

ImageFilter::ImageFilter():
	ImageFilterBase(),
	m_filters(NoFilter),
	m_onlyGaborWavelet(false)
{
	m_gaborFilters.resize(1);
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
	if (m_filters != NoFilter) {
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
	int addr = 0;
	for (int row = 0; row < rows; ++row) {
		const uchar *line = sourceImage->scanLine(row);
		for (int col = 0; col < cols; ++col) {
			vect(addr) = static_cast<double>(line[col]) / 256.0;
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
	//m_gaborFilter.setOnlyGaborWavelet(wavelet);
	for (auto filter = m_gaborFilters.begin(); filter != m_gaborFilters.end(); ++filter) {
		filter->setOnlyGaborWavelet(wavelet);
	}
	m_onlyGaborWavelet = wavelet;
}

/**
 * Nastavenie parametrov gabor filtra.
 * \sa FaceDetect::GaborFilter::setGaborParameters()
 */
void ImageFilter::setGaborParameters(const GaborFilter::GaborParameters &parameters)
{
	//m_gaborFilter.setGaborParameters(parameters);
	if (m_gaborFilters.count() != 1) {
		m_gaborFilters.resize(1);
	}
	m_gaborFilters.first().setGaborParameters(parameters);
	setOnlyGaborWavelet(m_onlyGaborWavelet);
}

/**
 * Nastavenie množiny gaborovych filtrov.
 */
void ImageFilter::setGaborParameters(const QList<GaborFilter::GaborParameters> &parameters)
{
	if (m_gaborFilters.count() != parameters.count()) {
		m_gaborFilters.resize(parameters.count());
	}
	auto gaborIt = m_gaborFilters.begin();
	for (auto parameterIt = parameters.begin(); parameterIt != parameters.end(); ++parameterIt) {
		gaborIt->setGaborParameters(*parameterIt);
		++gaborIt;
	}
	setOnlyGaborWavelet(m_onlyGaborWavelet);
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
	if (m_filters & GaborFilter && m_gaborFilters.count() > 0) {
		if (m_gaborFilters.count() == 1) {
			m_gaborFilters.first().filter(sourceImage);
		}
		else {
			QImage copy = sourceImage;
			sourceImage = sourceImage.scaled(sourceImage.width(), sourceImage.height() * m_gaborFilters.count());
			QImage workingImage;
			int lineOffset = 0;
			int lineSkip = m_gaborFilters.count();
			int sourceHeight = copy.height();
			for (auto filter = m_gaborFilters.begin(); filter != m_gaborFilters.end(); ++filter) {
				workingImage = copy;
				filter->filter(workingImage);
				int targetLine = lineOffset;
				for (int sourceLine = 0; sourceLine < sourceHeight; ++sourceLine) {
					uchar *src = workingImage.scanLine(sourceLine);
					uchar *target = sourceImage.scanLine(targetLine);
					memcpy(target, src, workingImage.bytesPerLine());
					targetLine += lineSkip;
				}
				lineOffset++;
			}
		}
	}
}

/**
 * Vráti nastavenie všetkých filtrov.
 */
QVariant ImageFilter::filterData() const
{
	QVariantMap grayscaleSettings;
	grayscaleSettings["enabled"] = bool(m_filters & GrayscaleFilter);

	QVariantMap illuminationSettings;
	illuminationSettings["enabled"] = bool(m_filters & IlluminationFilter);
	illuminationSettings["illuminationPlaneOnly"] = m_illuminationFilter.illuminationPlaneOnly();
	illuminationSettings["illuminationCorrectHistogram"] = m_illuminationFilter.illuminationCorrectHistogram();

	QVariantMap sobelSettings;
	sobelSettings["enalbed"] = bool(m_filters & SobelFilter);

	QVariantMap gaborSettings;
	gaborSettings["enabled"] = bool(m_filters & GaborFilter);
	QVariantList gaborFilterList;
	foreach (const FaceDetect::GaborFilter &filter, m_gaborFilters) {
		QVariantMap gaborVarParams;
		FaceDetect::GaborFilter::GaborParameters gaborParams = filter.gaborParameters();
		gaborVarParams["lambda"] = gaborParams.lambda;
		gaborVarParams["theta"] = gaborParams.theta;
		gaborVarParams["psi"] = gaborParams.psi;
		gaborVarParams["sigma"] = gaborParams.sigma;
		gaborVarParams["gamma"] = gaborParams.gamma;
		gaborVarParams["lum"] = gaborParams.lum;
		gaborFilterList << gaborVarParams;
	}
	gaborSettings["filters"] = gaborFilterList;

	QVariantMap filterSettings;
	filterSettings["grayscale"] = grayscaleSettings;
	filterSettings["illumination"] = illuminationSettings;
	filterSettings["sobel"] = sobelSettings;
	filterSettings["gabor"] = gaborSettings;
	return filterSettings;
}

/**
 * Nastavenie všetkých filtrov.
 * \todo Doplniť popis nastavenia.
 */
void ImageFilter::setFilterData(const QVariant &settings)
{
	QVariantMap filters = settings.value<QVariantMap>();
	// Prevod do odtieňov šedej
	{
		QVariantMap settings = filters["grayscale"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			enableGrayscaleFilter();
		}
		else {
			disableGrayscaleFilter();
		}
	}
	// Korekcia osvetlenia
	{
		QVariantMap settings = filters["illumination"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			enableIlluminationFilter();
			setIlluminationPlaneOnly(settings["illuminationPlaneOnly"].value<bool>());
			setIlluminationCorrectHistogram(settings["illuminationCorrectHistogram"].value<bool>());
		}
		else {
			disableIlluminationFilter();
		}
	}
	// Sobelov filter
	{
		QVariantMap settings = filters["sobel"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			enableSobelFilter();
		}
		else {
			disableSobelFilter();
		}
	}
	// Gaborov filter
	{
		QVariantMap settings = filters["gabor"].value<QVariantMap>();
		if (settings["enabled"].value<bool>()) {
			enableGaborFilter();
			QVariantList gaborFilters = settings["filters"].value<QVariantList>();
			QList<FaceDetect::GaborFilter::GaborParameters> parameters;
			foreach (const QVariant &gaborFilterVar, gaborFilters) {
				QVariantMap gaborFilter = gaborFilterVar.value<QVariantMap>();
				FaceDetect::GaborFilter::GaborParameters gaborParameters;
				gaborParameters.lambda = gaborFilter["lambda"].value<double>();
				gaborParameters.theta = gaborFilter["theta"].value<double>() / 180.0 * boost::math::constants::pi<double>();
				gaborParameters.psi = gaborFilter["psi"].value<double>();
				gaborParameters.sigma = gaborFilter["sigma"].value<double>();
				gaborParameters.gamma = gaborFilter["gamma"].value<double>();
				gaborParameters.lum = gaborFilter["lum"].value<double>();
				parameters << gaborParameters;
			}
			setGaborParameters(parameters);
		}
		else {
			disableGaborFilter();
		}
	}
}

/**
 * Vráti kombinovaný počet obrázkov, ktoré vráti filter.
 */
int ImageFilter::subImageCount() const
{
	if (m_filters & ImageFilter::GaborFilter) {
		// Ak nie je povolený ani jeden gaborov filter ignoruje sa to
		return qMax(m_gaborFilters.count(), 1);
	}
	else {
		return 1;
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


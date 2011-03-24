/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  18.03.2011 14:53:27
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QPainter>
#include "ImageFilter.h"

namespace FaceDetect {

ImageFilter::ImageFilter():
	m_filters(NoFilter)
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

} /* end of namespace FaceDetect */


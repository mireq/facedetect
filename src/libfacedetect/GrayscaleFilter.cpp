/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  14.04.2011 17:27:19
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QPainter>
#include "GrayscaleFilter.h"

namespace FaceDetect {

GrayscaleFilter::GrayscaleFilter()
{
	m_grayscaleGradient.setColorAt(0, Qt::black);
	m_grayscaleGradient.setColorAt(1, Qt::white);
}

GrayscaleFilter::~GrayscaleFilter()
{
}

void GrayscaleFilter::filter(QImage &sourceImage) const
{
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

/**
 * Vráti prechod používaný pri prevode do odtieňov šedej.
 */
QLinearGradient GrayscaleFilter::grayscaleGradient() const
{
	return m_grayscaleGradient;
}

/**
 * Štandardne má počiatočnú hodnotu čiernu a konečnú bielu. Nastavením iného
 * gradientu sa dá dosiahnuť aby obrázok v odtieňoch šedej mal v skutočnosti
 * farebnú mapu zodpovedajúcu gradientu.
 */
void GrayscaleFilter::setGrayscaleGradient(const QLinearGradient &gradient)
{
	m_grayscaleGradient = gradient;
	m_grayscaleGradient.setStart(0, 0);
	m_grayscaleGradient.setFinalStop(GrayscaleColorCount, 0);
}

/**
 * Prevod obrázku do indexovaných farieb s farebnou tabuľkou, ktorá obsahuje
 * odtiene šedej od (0, 0, 0) po (255, 255, 255).
 */
void GrayscaleFilter::filterGrayscale(QImage &sourceImage) const
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


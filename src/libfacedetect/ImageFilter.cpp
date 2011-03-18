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

ImageFilter::Filters ImageFilter::filters() const
{
	return m_filters;
}

void ImageFilter::setFilters(Filters filters)
{
	m_filters = filters;
}

QLinearGradient ImageFilter::grayscaleGradient() const
{
	return m_grayscaleGradient;
}

void ImageFilter::setGrayscaleGradient(const QLinearGradient &gradient)
{
	m_grayscaleGradient = gradient;
	m_grayscaleGradient.setStart(0, 0);
	m_grayscaleGradient.setFinalStop(sm_grayscaleColorCount, 0);
}

QImage ImageFilter::filterImage(const QImage &sourceImage) const
{
	QImage ret = sourceImage;
	if (m_filters & GrayscaleFilter) {
		filterGrayscale(ret);
		// Ak nie je gradient čiernobiely nastavíme farby podľa gradientu
		if (m_grayscaleGradient.stops().count() != 2 ||
		    m_grayscaleGradient.stops()[0].second != Qt::black ||
		    m_grayscaleGradient.stops()[1].second != Qt::white) {
			QImage gradImage(QSize(sm_grayscaleColorCount, 1), QImage::Format_RGB888);
			QPainter gradPainter(&gradImage);
			gradPainter.setPen(Qt::NoPen);
			gradPainter.fillRect(QRect(0, 0, sm_grayscaleColorCount, 1), m_grayscaleGradient);
			gradPainter.end();

			QVector<QRgb> colorTable(sm_grayscaleColorCount);
			for (int colorIdx = 0; colorIdx < sm_grayscaleColorCount; ++colorIdx) {
				colorTable[colorIdx] = gradImage.pixel(colorIdx, 0);
			}
			ret.setColorTable(colorTable);
		}
	}
	return ret;
}

void ImageFilter::filterGrayscale(QImage &sourceImage) const
{
	if (m_colorTable.isEmpty()) {
		m_colorTable.resize(256);
		for (int i = 0; i < 256; ++i) {
			m_colorTable[i] = qRgb(i, i, i);
		}
	}
	sourceImage = sourceImage.convertToFormat(QImage::Format_Indexed8, m_colorTable);
}

} /* end of namespace FaceDetect */


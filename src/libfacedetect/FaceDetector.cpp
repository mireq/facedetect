/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  26.03.2011 11:53:17
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include "ImageFilter.h"
#include "FaceDetector.h"

#include <QPainter>
#include <QRectF>
#include <QPolygonF>
#include <QLabel>
#include <cstring>
#include "utils/PolygonRasterizer.h"

using std::memset;
#include <QLabel>
#include <QEventLoop>

namespace FaceDetect {

FaceDetector::FaceDetector(NeuralNet *neuralNet, QObject *parent):
	QThread(parent),
	m_neuralNet(neuralNet)
{
}

FaceDetector::~FaceDetector()
{
}

QImage FaceDetector::image() const
{
	if (m_segmenter.isNull()) {
		return QImage();
	}
	else {
		return m_segmenter->image();
	}
}

void FaceDetector::setImage(const QImage &image)
{
	m_segmenter = QSharedPointer<ImageSegmenter>(new ImageSegmenter(image, m_settings));
	m_segmenter->setGrayscaleFilter(true);
	m_statistics = LaGenMatFloat(image.height(), image.width());
}

void FaceDetector::setupSegmenter(const ImageSegmenter::Settings &settings)
{
	m_settings = settings;
	if (!m_segmenter.isNull()) {
		m_segmenter->setupSegmenter(m_settings);
	}
}

void FaceDetector::scanImage()
{
	// Veľkosť dát
	std::size_t statisticsWidth = m_segmenter->image().width();
	std::size_t statisticsHeight = m_segmenter->image().height();
	std::size_t statisticsDataSize = statisticsWidth * statisticsHeight;
	// Vyčistenie štatistiky
	float *bitmapData = m_statistics.addr();
	memset(bitmapData, 0, sizeof(float) * statisticsDataSize);

	Utils::PolygonRasterizer rasterizer;
	ImageFilter filter;
	// Inverzia transformácie obrazu
	QTransform rectTransform = m_settings.transform.inverted();
	int segmentCount = m_segmenter->segmentCount();
	// Výpočet hodnoty každého segmentu
	for (int segmentId = 0; segmentId < segmentCount; ++segmentId) {
		// Výpočet vstupu a výstupu
		QImage segmentImage = m_segmenter->segmentImage(segmentId);
		LaVectorDouble inputVector = filter.filterVector(segmentImage);
		LaVectorDouble outputVector = m_neuralNet->calcOutput(inputVector);

		// Oblasť segmentu
		QRect segmentRect = m_segmenter->segmentRect(segmentId);
		// Transformácia polygónu
		QPolygon poly(segmentRect);
		poly = rectTransform.map(poly);
		// Rasterizácia polgónu
		rasterizer.processPolygon(poly);
		const QVector<Utils::PolygonRasterizer::ScanLine> lines = rasterizer.scanLines();
		int y = rasterizer.boundingRect().top();
		std::size_t memBegin = statisticsWidth * y;
		std::size_t lineBegin, lineEnd;
		foreach (const Utils::PolygonRasterizer::ScanLine &line, lines) {
			if (y < 0) {
				y++;
				memBegin = statisticsWidth * y;
				continue;
			}
			else if (static_cast<std::size_t>(y) >= statisticsHeight) {
				break;
			}
			lineBegin = static_cast<std::size_t>(qMax(line.minX, 0)) + memBegin;
			lineEnd = static_cast<std::size_t>(qMin(line.maxX, int(statisticsWidth))) + memBegin;
			for (std::size_t memPos = lineBegin; memPos < lineEnd; ++memPos) {
				bitmapData[memPos] = qMax(bitmapData[memPos], static_cast<float>(outputVector(0)));
			}
			y++;
			memBegin = statisticsWidth * y;
		}
	}

	QImage outImage(m_segmenter->image().size(), QImage::Format_ARGB32);
	for (std::size_t yPos = 0; yPos < statisticsHeight; ++yPos) {
		for (std::size_t xPos = 0; xPos < statisticsWidth; ++xPos) {
			float val = bitmapData[yPos * statisticsWidth + xPos];
			int colorVal;
			if (val < 0) {
				colorVal = 0;
			}
			else if (val >= 1) {
				colorVal = 255;
			}
			else {
				colorVal = val * 255;
			}
			outImage.setPixel(QPoint(int(xPos), int(yPos)), qRgb(colorVal, colorVal, colorVal));
		}
	}
	QLabel *label = new QLabel();
	label->setPixmap(QPixmap::fromImage(outImage));
	label->show();
	QEventLoop loop(this);
	loop.exec();
}

} /* end of namespace FaceDetect */


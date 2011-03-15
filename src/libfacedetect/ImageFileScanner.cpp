/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  22.02.2011 20:09:04
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include "ImageFileScanner.h"

namespace FaceDetect {

ImageFileScanner::ImageFileScanner(QObject *parent):
	FileScanner(parent)
{
}

ImageFileScanner::~ImageFileScanner()
{
}

void ImageFileScanner::scanFile(const QString &fileName)
{
	QFileInfo fileInfo(fileName);
	QByteArray extension = fileInfo.suffix().toLatin1();
	if (!QImageReader::supportedImageFormats().contains(extension)) {
		return;
	}

	QImage image(fileName);
	if (image.isNull()) {
		return;
	}

	LaVectorDouble inVector(sm_inputVectorSize);
	LaVectorDouble outVector(1);
	outVector(0) = 0;
	std::size_t xCount = image.width() / sm_imageWidth;
	std::size_t yCount = image.height() / sm_imageHeight;
	for (std::size_t yTile = 0; yTile < yCount; ++yTile) {
		std::size_t yBegin = yTile * sm_imageHeight;
		for (std::size_t xTile = 0; xTile < xCount; ++xTile) {
			std::size_t xBegin = xTile * sm_imageWidth;
			for (std::size_t yOffset = 0; yOffset < sm_imageHeight; ++yOffset) {
				for (std::size_t xOffset = 0; xOffset < sm_imageWidth; ++xOffset) {
					QRgb color = image.pixel(xBegin + xOffset, yBegin + yOffset);
					int gray = (qRed(color) * 11 + qGreen(color) * 16 + qBlue(color) * 5) / 32;
					inVector(xOffset + yOffset * sm_imageHeight) = static_cast<double>(gray) / 256.0;
				}
			}
			emit imageScanned(inVector, outVector);
		}
	}
}

} /* end of namespace FaceDetect */


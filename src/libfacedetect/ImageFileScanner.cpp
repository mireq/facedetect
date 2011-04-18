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
#include "ImageFilter.h"
#include "ImageSegmenter.h"
#include "ImageFileScanner.h"

namespace FaceDetect {

/**
 * Vytvorenie inštancie skeneru.
 */
ImageFileScanner::ImageFileScanner(QObject *parent):
	FileScanner(parent)
{
}

ImageFileScanner::~ImageFileScanner()
{
}

/**
 * Skenovanie súboru \a fileName;
 */
void ImageFileScanner::scanFile(const QString &fileName)
{
	QFileInfo fileInfo(fileName);

	// Kontrola podpory formátu
	QByteArray extension = fileInfo.suffix().toLatin1();
	if (!QImageReader::supportedImageFormats().contains(extension)) {
		return;
	}

	// Otvorenie obrázku
	QImage image(fileName);
	if (image.isNull()) {
		return;
	}

	// Vytvorenie vstupného a výstupného vektoru
	LaVectorDouble inVector(InputVectorSize);
	LaVectorDouble outVector(1);
	// Výstup pre fotografie bez tváre je vždy 0
	outVector(0) = 0;

	ImageFilter filter;
	filter.enableIlluminationFilter();
	filter.enableGrayscaleFilter();
	ImageSegmenter segmenter(image);
	segmenter.setGrayscaleFilter(true);
	segmenter.setSegmentSize(QSize(ImageWidth, ImageHeight));
	segmenter.setStep(ImageWidth / 2, ImageHeight / 3);
	//segmenter.setStep(ImageWidth, ImageHeight);
	for (int segment = 0; segment < segmenter.segmentCount(); ++segment) {
		QImage segmentImage = segmenter.segmentImage(segment);
		inVector = filter.filterVector(segmentImage);
		emit imageScanned(inVector, outVector);
	}
	/*
	// Výpočet počtu segmentov
	int xCount = image.width() / ImageWidth;
	int yCount = image.height() / ImageHeight;
	// Prechádzanie cez všetky segmenty obrázku
	for (int yTile = 0; yTile < yCount; ++yTile) {
		int yBegin = yTile * ImageHeight;
		for (int xTile = 0; xTile < xCount; ++xTile) {
			int xBegin = xTile * ImageWidth;
			// Naplnenie vstupného vektoru
			for (int yOffset = 0; yOffset < ImageHeight; ++yOffset) {
				for (int xOffset = 0; xOffset < ImageWidth; ++xOffset) {
					QRgb color = image.pixel(xBegin + xOffset, yBegin + yOffset);
					int gray = (qRed(color) * 11 + qGreen(color) * 16 + qBlue(color) * 5) / 32;
					inVector(xOffset + yOffset * ImageHeight) = static_cast<double>(gray) / 256.0;
				}
			}
			emit imageScanned(inVector, outVector);
		}
	}*/
}

} /* end of namespace FaceDetect */


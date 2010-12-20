/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 09:28:32
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QPainter>
#include "libfacedetect/FaceFileReader.h"
#include "FaceImageProvider.h"

using FaceDetect::FaceFileReader;


FaceImageProvider::FaceImageProvider():
	QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

FaceImageProvider::~FaceImageProvider()
{
}

QImage FaceImageProvider::requestImage(const QString &id, QSize *size, const QSize & /*requestedSize*/)
{
	FaceFileReader reader;
	reader.readFile(id);
	QImage image = reader.readImage();
	*size = image.size();
	return image;
}


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
	QVector<FaceFileReader::FaceData> faceData = reader.faceData();
	QPainter painter(&image);
	foreach (FaceFileReader::FaceData data, faceData) {
		if (data.leftEye != QPoint()) {
			painter.setPen(Qt::green);
			painter.drawLine(data.leftEye + QPoint(-15, 0), data.leftEye + QPoint(15, 0));
			painter.drawLine(data.leftEye + QPoint(0, -15), data.leftEye + QPoint(0, 15));
		}
		if (data.rightEye != QPoint()) {
			painter.setPen(Qt::green);
			painter.drawLine(data.rightEye + QPoint(-15, 0), data.rightEye + QPoint(15, 0));
			painter.drawLine(data.rightEye + QPoint(0, -15), data.rightEye + QPoint(0, 15));
		}
		if (data.nose != QPoint()) {
			painter.setPen(Qt::red);
			painter.drawLine(data.nose + QPoint(-15, 0), data.nose + QPoint(15, 0));
			painter.drawLine(data.nose + QPoint(0, -15), data.nose + QPoint(0, 15));
		}
		if (data.mouth != QPoint()) {
			painter.setPen(Qt::blue);
			painter.drawLine(data.mouth + QPoint(-15, 0), data.mouth + QPoint(15, 0));
			painter.drawLine(data.mouth + QPoint(0, -15), data.mouth + QPoint(0, 15));
		}
	}
	painter.end();
	return image;
}


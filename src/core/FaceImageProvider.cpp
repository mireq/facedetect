/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 09:28:32
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QPainter>
#include "FaceImageProvider.h"

FaceImageProvider::FaceImageProvider():
	QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

FaceImageProvider::~FaceImageProvider()
{
}

QImage FaceImageProvider::requestImage(const QString &id, QSize *size, const QSize & /*requestedSize*/)
{
	QStringList path = id.split("/");
	if (path.size() > 0 && (path[0] == "original")) {
		path.pop_front();
		FaceDetect::FaceFileScanner::ImageInfo imageInfo = m_scanner->readFile(path.join("/"));
		QImage image = imageInfo.getImage();
		*size = image.size();
		return image;
	}
	else if (path.size() > 0 && (path[0] == "transformed") && m_scanner != 0) {
		path.pop_front();
		FaceDetect::FaceFileScanner::ImageInfo imageInfo = m_scanner->readFile(path.join("/"));
		QImage image(QSize(128, 128), QImage::Format_ARGB32);
		if (imageInfo.faceEnd() - imageInfo.faceBegin() == 1) {
			QTransform transform = m_align->getTransform(*imageInfo.faceBegin());
			QPainter painter(&image);
			painter.setTransform(transform);
			painter.drawImage(QPoint(0, 0), imageInfo.getImage());
		}
		*size = image.size();
		return image;
	}
	else if (path.size() > 0 && path[0] == "statimage" && m_scanner != 0) {
		QImage image = m_align->getStatisticsImage();
		*size = image.size();
		return image;
	}
	return QImage();
}

void FaceImageProvider::bindScanner(FaceDetect::FaceFileScanner *scanner)
{
	m_scanner = scanner;
}

void FaceImageProvider::bindAlign(FaceDetect::Align *align)
{
	m_align = align;
}


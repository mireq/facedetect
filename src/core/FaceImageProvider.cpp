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
#include "FaceBrowserModel.h"
#include "FaceImageProvider.h"

using FaceDetect::FaceFileReader;

FaceImageProvider::FaceImageProvider():
	QDeclarativeImageProvider(QDeclarativeImageProvider::Image),
	m_facesModel(0)
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
		FaceFileReader reader;
		reader.readFile(path.join("/"));
		QImage image = reader.readImage();
		*size = image.size();
		return image;
	}
	else if (path.size() > 0 && (path[0] == "transformed") && m_facesModel != 0) {
		path.pop_front();
		FaceFileReader reader;
		reader.readFile(path.join("/"));
		QVector<FaceDetect::FaceFileReader::FaceData> data = reader.faceData();
		QImage image(QSize(128, 128), QImage::Format_ARGB32);
		if (data.size() == 1) {
			QTransform transform = m_facesModel->getTransform(data[0]);
			QPainter painter(&image);
			painter.setTransform(transform);
			painter.drawImage(QPoint(0, 0), reader.readImage());
		}
		*size = image.size();
		return image;
	}
	else if (path.size() > 0 && path[0] == "statimage" && m_facesModel != 0) {
		QImage image = m_facesModel->getStatisticsImage();
		*size = image.size();
		return image;
	}
	return QImage();
}

void FaceImageProvider::bindFacesModel(FaceBrowserModel *model)
{
	m_facesModel = model;
}


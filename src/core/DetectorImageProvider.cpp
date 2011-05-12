/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  12.05.2011 06:50:05
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QMetaObject>
#include "gui/qml/QmlWin.h"
#include "DetectorImageProvider.h"

DetectorImageProvider::DetectorImageProvider(QmlWin *parent):
	QDeclarativeImageProvider(QDeclarativeImageProvider::Image),
	m_parent(parent)
{
}

DetectorImageProvider::~DetectorImageProvider()
{
}

QImage DetectorImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
	Q_UNUSED(requestedSize);
	QImage image(id);
	if (image.isNull()) {
		image = QImage(":/qml/img/photo.png", "PNG");
	}
	/*
	else {
		QMetaObject::invokeMethod(m_parent, "detect", Qt::QueuedConnection, Q_ARG(QImage, image));
	}
	*/
	*size = image.size();
	return image;
}


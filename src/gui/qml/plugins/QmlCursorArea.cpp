/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 12:56:08
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDebug>
#include "QmlCursorArea.h"

QmlCursorAreaAttached::QmlCursorAreaAttached(QObject *parent):
	QObject(parent)
{
}

QmlCursorAreaAttached::~QmlCursorAreaAttached()
{
}

Qt::CursorShape QmlCursorAreaAttached::cursor() const
{
	QDeclarativeItem *parentItem = qobject_cast<QDeclarativeItem *>(parent());
	if (parentItem != 0) {
		return parentItem->cursor().shape();
	}
	else {
		return Qt::ArrowCursor;
	}
}

void QmlCursorAreaAttached::setCursor(Qt::CursorShape shape)
{
	QDeclarativeItem *parentItem = qobject_cast<QDeclarativeItem *>(parent());
	if (parentItem != 0) {
		parentItem->setCursor(shape);
	}
	else {
		qWarning() << "Parent" << parent() << "is not QDelarativeItem";
	}
}

QmlCursorArea::QmlCursorArea(QObject *parent):
	QObject(parent)
{
}

QmlCursorArea::~QmlCursorArea()
{
}

QmlCursorAreaAttached *QmlCursorArea::qmlAttachedProperties(QObject *parent)
{
	return new QmlCursorAreaAttached(parent);
}


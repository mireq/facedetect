/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03.05.2011 12:56:10
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef QMLCURSORAREA_NVH1B76P
#define QMLCURSORAREA_NVH1B76P

#include <QtDeclarative>
#include <QObject>

class QmlCursorAreaAttached: public QObject
{
Q_OBJECT
Q_PROPERTY(Qt::CursorShape cursor READ cursor WRITE setCursor);
public:
	QmlCursorAreaAttached(QObject *parent = 0);
	~QmlCursorAreaAttached();
	Qt::CursorShape cursor() const;
	void setCursor(Qt::CursorShape shape);

}; /* -----  end of class QmlCursorAreaAttached  ----- */

class QmlCursorArea: public QObject
{
Q_OBJECT
public:
	QmlCursorArea(QObject *parent = 0);
	~QmlCursorArea();
	static QmlCursorAreaAttached *qmlAttachedProperties(QObject *parent);

private:
}; /* -----  end of class QmlCursorArea  ----- */

QML_DECLARE_TYPEINFO(QmlCursorArea, QML_HAS_ATTACHED_PROPERTIES)

#endif /* end of include guard: QMLCURSORAREA_NVH1B76P */


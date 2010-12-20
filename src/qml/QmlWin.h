/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 09:54:39
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef QMLWIN_R54R5F4S
#define QMLWIN_R54R5F4S

#include <QDeclarativeView>
class FaceFileScanner;

class QmlWin: public QDeclarativeView
{
Q_OBJECT
public:
	QmlWin(QWidget *parent = 0);
	~QmlWin();

private:
	FaceFileScanner *m_scanner;
}; /* -----  end of class QmlWin  ----- */

#endif /* end of include guard: QMLWIN_R54R5F4S */


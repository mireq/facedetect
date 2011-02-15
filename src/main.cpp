/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 00:31:52
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QApplication>
#include "qml/QmlWin.h"
#include "libfacedetect/FaceFileScanner.h"

int main(int argc, char *argv[])
{
	qRegisterMetaType<FaceDetect::FaceFileScanner::ImageInfo>("FaceDetect::FaceFileScanner::ImageInfo");
	QApplication::setApplicationName("Face detector");
	QApplication::setOrganizationDomain("linuxos.sk");
	QApplication::setOrganizationName("LinuxOS.sk");
	QApplication app(argc, argv);
	QmlWin win;
	win.show();
	return app.exec();
}


/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 00:31:52
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QApplication>
#include <QDebug>
#include <stdlib.h>
#include <time.h>
#include "gui/console/ConsoleInterface.h"
#include "gui/qml/QmlWin.h"
#include "libfacedetect/FaceFileScanner.h"

int main(int argc, char *argv[])
{
	srand(time(0));
	qRegisterMetaType<FaceDetect::FaceFileScanner::ImageInfo>("FaceDetect::FaceFileScanner::ImageInfo");
	QApplication::setApplicationName("Face detector");
	QApplication::setOrganizationDomain("linuxos.sk");
	QApplication::setOrganizationName("LinuxOS.sk");

	QString interface = "console";
	{
		QApplication app(argc, argv);
		QStringList arguments = app.arguments();
		int interfaceArgId = arguments.indexOf("--interface");
		if (interfaceArgId != -1) {
			if (arguments.size() - 1 <= interfaceArgId) {
				qDebug("Missing argument (console|touch)");
				return -1;
			}
			interface = arguments[interfaceArgId + 1];
		}
	}
	if (interface == "console") {
		QApplication app(argc, argv);
		ConsoleInterface interface;
		interface.run();
		return app.exec();
	}
	else if (interface == "touch") {
		QApplication app(argc, argv);
		QmlWin win;
		win.show();
		return app.exec();
	}
	else {
		qDebug("Unsupported interface");
	}
}


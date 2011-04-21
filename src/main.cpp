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
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
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
				qDebug() << QObject::tr("Missing argument (console|touch)");
				return -1;
			}
			interface = arguments[interfaceArgId + 1];
		}
	}
	QString lang = QLocale::system().name();
	if (interface == "console") {
		QApplication app(argc, argv);
		QTranslator *qtTranslator = new QTranslator(&app);
		qtTranslator->load(QString("qt_") + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		QTranslator *appTranslator = new QTranslator(&app);
		appTranslator->load(QString("facedetect_") + lang);
		app.installTranslator(qtTranslator);
		app.installTranslator(appTranslator);

		ConsoleInterface interface;
		interface.run();
		return app.exec();
	}
	else if (interface == "touch") {
		QApplication app(argc, argv);
		QTranslator *qtTranslator = new QTranslator(&app);
		qtTranslator->load(QString("qt_") + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		QTranslator *appTranslator = new QTranslator(&app);
		appTranslator->load(QString("facedetect_") + lang);
		app.installTranslator(qtTranslator);
		app.installTranslator(appTranslator);

		QmlWin win;
		win.show();
		return app.exec();
	}
	else {
		qDebug() << QObject::tr("Unsupported interface");
	}
}


/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 00:31:52
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

/*
#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include "core/FaceBrowserModel.h"
#include "core/FaceFileScanner.h"
#include "core/FaceImageProvider.h"
#include "libfacedetect/Align.h"
#include "libfacedetect/FaceFileReader.h"

#include <iostream>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QDeclarativeView view;
	view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
	QDeclarativeContext *rootContext = view.rootContext();
	QDeclarativeEngine *engine = view.engine();
	FaceDetect::FaceFileReader::setDataDir("/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/");
	engine->addImageProvider(QLatin1String("faceimage"), new FaceImageProvider);

	FaceBrowserModel model;
	rootContext->setContextProperty("facesModel", &model);
	view.setSource(QUrl("qrc:/qml/main.qml"));

	FaceFileScanner scanner;
	scanner.setScanPath("/home/mirec/Documents/Skola/diplomka/resources/colorferet/dvd1/data/ground_truths/xml/");
	QObject::connect(&scanner, SIGNAL(fileScanned(QString)), &model, SLOT(addDefinitionFile(QString)));
	scanner.start();

	view.show();
	return app.exec();
}
*/

#include <QApplication>
#include "qml/QmlWin.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QmlWin win;
	win.show();
	return app.exec();
}


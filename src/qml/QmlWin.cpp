/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 09:54:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include "core/FaceBrowserModel.h"
#include "core/FaceFileScanner.h"
#include "core/FaceImageProvider.h"
#include "libfacedetect/FaceFileReader.h"
#include "QmlWin.h"

using FaceDetect::FaceFileReader;

#include <QDebug>

QmlWin::QmlWin(QWidget *parent):
	QDeclarativeView(parent)
{
	setResizeMode(QDeclarativeView::SizeRootObjectToView);
	QStringList arguments = qApp->arguments();
	int datadirIdx;
	QString datadir;
	if ((datadirIdx = arguments.indexOf("--datadir")) != -1) {
		if (datadirIdx < arguments.count() - 1) {
			datadir = arguments[datadirIdx + 1];
			FaceFileReader::setDataDir(datadir);
		}
	}
	engine()->addImageProvider(QLatin1String("faceimage"), new FaceImageProvider);

	FaceBrowserModel *model = new FaceBrowserModel(this);
	m_scanner = new FaceFileScanner(this);
	rootContext()->setContextProperty("facesModel", model);
	rootContext()->setContextProperty("faceScanner", m_scanner);
	setSource(QUrl("qrc:/qml/main.qml"));

	m_scanner->setScanPath(datadir + "/data/ground_truths/xml/");
	QObject::connect(m_scanner, SIGNAL(fileScanned(QString)), model, SLOT(addDefinitionFile(QString)));
	m_scanner->start();
}

QmlWin::~QmlWin()
{
	m_scanner->stop();
}


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
#include <QGLWidget>
#include <QSettings>
#include "libfacedetect/FaceFileScanner.h"
#include "core/FaceImageProvider.h"
#include "QmlWin.h"

using FaceDetect::FaceFileScanner;

QmlWin::QmlWin(QWidget *parent):
	QDeclarativeView(parent)
{
	QSettings settings;
	settings.beginGroup("paths");
	m_scanPath = settings.value("database").toString();
	settings.endGroup();

	// Nastavenie OpenGL
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setViewport(new QGLWidget());
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	setResizeMode(QDeclarativeView::SizeRootObjectToView);
	QStringList arguments = qApp->arguments();

	FaceImageProvider *imageProvider = new FaceImageProvider;
	engine()->addImageProvider(QLatin1String("faceimage"), imageProvider);

	m_aligner = new FaceDetect::Align(this);
	m_aligner->setCollectStatistics(true);
	m_faceBrowserModel = new FaceBrowserModel(m_aligner, this);
	m_faceFileScanner = new FaceFileScanner(this);
	imageProvider->bindScanner(m_faceFileScanner);
	imageProvider->bindAlign(m_aligner);
	rootContext()->setContextProperty("runtime", this);
	setSource(QUrl("qrc:/qml/main.qml"));
	connect(m_faceFileScanner, SIGNAL(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)), this, SLOT(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)));
	m_faceFileScanner->setBasePath(m_scanPath);
}

QmlWin::~QmlWin()
{
	QSettings settings;
	settings.beginGroup("paths");
	settings.setValue("database", m_scanPath);
	settings.endGroup();
	m_faceFileScanner->stop();
}

QString QmlWin::scanPath() const
{
	return m_scanPath;
}

void QmlWin::setScanPath(const QString &scanPath)
{
	if (m_scanPath != scanPath) {
		m_scanPath = scanPath;
		m_faceFileScanner->setBasePath(m_scanPath);
		emit scanPathChanged(m_scanPath);
	}
}

FaceBrowserModel *QmlWin::faceBrowserModel() const
{
	return m_faceBrowserModel;
}

FaceFileScanner *QmlWin::faceFileScanner() const
{
	return m_faceFileScanner;
}

void QmlWin::imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &img)
{
	m_faceBrowserModel->addDefinitionFile(img);
	m_aligner->addImage(img);
}


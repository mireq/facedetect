/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 09:54:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QAction>
#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QGLWidget>
#include <QKeySequence>
#include <QSettings>
#include <QUrl>
#include "libfacedetect/FaceFileScanner.h"
#include "core/FaceImageProvider.h"
#include "plugins/QmlFaceDetectPlugin.h"
#include "QmlWin.h"

using FaceDetect::Align;
using FaceDetect::FaceFileScanner;

QmlWin::QmlWin(QWidget *parent):
	QDeclarativeView(parent)
{
	QAction *actionQuit = new QAction(this);
	actionQuit->setShortcut(QKeySequence("CTRL+Q"));
	addAction(actionQuit);
	connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

	QmlFaceDetectPlugin plugin;
	plugin.registerTypes("org.facedetect");

	loadSettings();

	// Nastavenie OpenGL
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setViewport(new QGLWidget());
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setResizeMode(QDeclarativeView::SizeRootObjectToView);

	m_imageProvider = new FaceImageProvider;
	engine()->addImageProvider(QLatin1String("faceimage"), m_imageProvider);
	initializeScanner();
	rootContext()->setContextProperty("runtime", this);
	setSource(QUrl("qrc:/qml/main.qml"));
}

QmlWin::~QmlWin()
{
	saveSettings();
	m_faceFileScanner->stop();
}

QString QmlWin::facesPath() const
{
	return m_facesPath;
}

void QmlWin::setFacesPath(const QString &facesPath)
{
	if (m_facesPath != facesPath) {
		m_facesPath = facesPath;
		emit facesPathChanged(m_facesPath);
		m_faceFileScanner->stop();
		decltype(m_aligner) aligner = m_aligner;
		decltype(m_faceBrowserModel) faceBrowserModel = m_faceBrowserModel;
		m_faceFileScanner->setBasePath(m_facesPath);
		m_faceBrowserModel->clear();
		m_aligner.clear();
		m_faceBrowserModel.clear();
		initializeScanner();
		emit faceBrowserModelChanged(m_faceBrowserModel.data());
		emit faceFileScannerChanged(m_faceFileScanner.data());
	}
}

QString QmlWin::nonFacesPath() const
{
	return m_nonFacesPath;
}

void QmlWin::setNonFacesPath(const QString &nonFacesPath)
{
	m_nonFacesPath = nonFacesPath;
}

FaceBrowserModel *QmlWin::faceBrowserModel() const
{
	return m_faceBrowserModel.data();
}

FaceFileScanner *QmlWin::faceFileScanner() const
{
	return m_faceFileScanner.data();
}

void QmlWin::imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &img)
{
	m_faceBrowserModel->addDefinitionFile(img);
	m_aligner->addImage(img);
}

void QmlWin::loadSettings()
{
	m_settings.beginGroup("paths");
	m_facesPath = m_settings.value("faces").toString();
	m_nonFacesPath = m_settings.value("nofaces").toString();
	m_settings.endGroup();
}

void QmlWin::saveSettings()
{
	m_settings.beginGroup("paths");
	m_settings.setValue("faces", m_facesPath);
	m_settings.setValue("nofaces", m_nonFacesPath);
	m_settings.endGroup();
}

void QmlWin::initializeScanner()
{
	if (m_aligner.isNull()) {
		m_aligner = QSharedPointer<Align>(new Align(this));
		m_aligner->setImageSize(128);
		m_aligner->setCollectStatistics(true);
		m_imageProvider->bindAlign(m_aligner.data());
	}
	if (m_faceBrowserModel.isNull()) {
		m_faceBrowserModel = QSharedPointer<FaceBrowserModel>(new FaceBrowserModel(m_aligner.data(), this));
	}
	if (m_faceFileScanner.isNull()) {
		m_faceFileScanner = QSharedPointer<FaceFileScanner>(new FaceFileScanner(this));
		m_faceFileScanner->setFilterFrontal(true);
		m_faceFileScanner->setBasePath(m_facesPath);
		m_imageProvider->bindScanner(m_faceFileScanner.data());
		connect(m_faceFileScanner.data(), SIGNAL(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)), this, SLOT(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)));
	}
}


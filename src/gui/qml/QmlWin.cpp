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
#include <QBuffer>
#include <QDataStream>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QGLWidget>
#include <QKeySequence>
#include <QMetaObject>
#include <QSettings>
#include <QUrl>
#include "libfacedetect/FaceFileScanner.h"
#include "core/FaceImageProvider.h"
#include "core/FilterImageProvider.h"
#include "plugins/QmlFaceDetectPlugin.h"
#include "QmlWin.h"

#include <QDebug>
using FaceDetect::Align;
using FaceDetect::FaceFileScanner;
using FaceDetect::ImageFileScanner;

QmlWin::QmlWin(QWidget *parent):
	QDeclarativeView(parent),
	m_trainingDatabase(new FaceDetect::TrainingImageDatabase(this))
{
	QAction *actionQuit = new QAction(this);
	actionQuit->setShortcut(QKeySequence("CTRL+Q"));
	addAction(actionQuit);
	connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

	loadSettings();
	createFilterSettings();

	QmlFaceDetectPlugin plugin;
	plugin.registerTypes("org.facedetect");

	// Nastavenie OpenGL
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setViewport(new QGLWidget());
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setResizeMode(QDeclarativeView::SizeRootObjectToView);

	m_imageProvider = new FaceImageProvider;
	engine()->addImageProvider(QLatin1String("faceimage"), m_imageProvider);
	m_filterImageProvider = new FilterImageProvider;
	engine()->addImageProvider(QLatin1String("filter"), m_filterImageProvider);
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
		m_faceFileScanner->stop();
		m_facesPath = facesPath;
		emit facesPathChanged(m_facesPath);
		decltype(m_aligner) aligner = m_aligner;
		decltype(m_faceBrowserModel) faceBrowserModel = m_faceBrowserModel;
		m_faceBrowserModel->clear();
		m_aligner.clear();
		m_faceBrowserModel.clear();
		m_faceFileScanner->setBasePath(m_facesPath);
		initializeScanner();
		emit faceBrowserModelChanged(m_faceBrowserModel.data());
	}
}

QString QmlWin::nonFacesPath() const
{
	return m_nonFacesPath;
}

void QmlWin::setNonFacesPath(const QString &nonFacesPath)
{
	if (m_nonFacesPath != nonFacesPath) {
		m_nonFaceFileScanner->stop();
		m_nonFacesPath = nonFacesPath;
		m_nonFaceFileScanner->setScanPath(m_nonFacesPath);
		emit nonFacesPathChanged(m_nonFacesPath);
		initializeScanner();
	}
}

FaceBrowserModel *QmlWin::faceBrowserModel() const
{
	return m_faceBrowserModel.data();
}

FaceFileScanner *QmlWin::faceFileScanner() const
{
	return m_faceFileScanner.data();
}

FaceDetect::ImageFileScanner *QmlWin::nonFaceFileScanner() const
{
	return m_nonFaceFileScanner.data();
}

QVariant QmlWin::filterSettings() const
{
	return m_filterSettings;
}

void QmlWin::setFilterSettings(const QVariant &filterSettings)
{
	if (m_filterSettings != filterSettings) {
		m_filterSettings = filterSettings;
		m_filter.setFilterData(m_filterSettings);
		emit filterSettingsChanged(m_filterSettings);
	}
}

QString QmlWin::encodeFilterString() const
{
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	{
		QDataStream stream(&buffer);
		stream << m_filterSettings;
	}
	return buffer.data().toBase64();
}

void QmlWin::startTraining()
{
	m_steps.append(ProcessStep(this, "scanFaces"));
	m_steps.append(ProcessStep(this, "scanNonFaces"));
	startNextStep();
}

void QmlWin::imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &img)
{
	m_faceBrowserModel->addDefinitionFile(img);
	m_aligner->addImage(img);
}

void QmlWin::startNextStep()
{
	if (m_steps.isEmpty()) {
		return;
	}
	ProcessStep step = m_steps.takeFirst();
	QMetaObject::invokeMethod(step.object, step.method.constData(), Qt::QueuedConnection);
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
	if (!m_faceFileScanner.isNull()) {
		m_faceFileScanner->reset();
	}
	if (!m_nonFaceFileScanner.isNull()) {
		m_nonFaceFileScanner->reset();
	}
	m_trainingDatabase->clear();

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
		connect(m_faceFileScanner.data(), SIGNAL(finished()), SLOT(startNextStep()));
		connect(m_faceFileScanner.data(), SIGNAL(terminated()), SLOT(startNextStep()));
	}
	if (m_nonFaceFileScanner.isNull()) {
		m_nonFaceFileScanner = QSharedPointer<ImageFileScanner>(new ImageFileScanner());
		m_nonFaceFileScanner->setScanPath(m_nonFacesPath);
		connect(m_nonFaceFileScanner.data(), SIGNAL(finished()), SLOT(startNextStep()));
		connect(m_nonFaceFileScanner.data(), SIGNAL(terminated()), SLOT(startNextStep()));
	}
}

void QmlWin::createFilterSettings()
{
	m_filterSettings = m_filter.filterData();
}

void QmlWin::scanFaces()
{
	m_faceFileScanner->start();
}

void QmlWin::scanNonFaces()
{
	m_nonFaceFileScanner->start();
}


/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 09:54:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <fstream>
#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QDataStream>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QGLWidget>
#include <QKeySequence>
#include <QMetaObject>
#include <QMessageBox>
#include <QPalette>
#include <QSettings>
#include <QUrl>
#include "libfacedetect/BPNeuralNet.h"
#include "libfacedetect/FaceStructuredNet.h"
#include "libfacedetect/FaceFileScanner.h"
#include "core/DetectorImageProvider.h"
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
	m_neuralNet(0),
	m_trainingDatabase(new FaceDetect::TrainingImageDatabase(this)),
	m_trainingSetPercent(80),
	m_learningSpeed(1)
{
	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::white);
	pal.setColor(QPalette::Base, Qt::white);
	pal.setColor(QPalette::Text, Qt::black);
	setPalette(pal);

	qRegisterMetaType<std::size_t>("std::size_t");
	qRegisterMetaType<LaVectorDouble>("LaVectorDouble");

	QAction *actionQuit = new QAction(this);
	actionQuit->setShortcut(QKeySequence("CTRL+Q"));
	addAction(actionQuit);
	connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

	loadSettings();
	createFilterSettings();
	createNeuralNet();
	createNetTrainer();

	QmlFaceDetectPlugin plugin;
	plugin.registerTypes("org.facedetect");

	// Nastavenie OpenGL
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	//setViewport(new QGLWidget());
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setResizeMode(QDeclarativeView::SizeRootObjectToView);

	m_detectorImageProvider = new DetectorImageProvider(this);
	engine()->addImageProvider(QLatin1String("detector"), m_detectorImageProvider);
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
	stop();
}

QString QmlWin::facesPath() const
{
	return m_facesPath;
}

void QmlWin::setFacesPath(const QString &facesPath)
{
	if (m_facesPath != facesPath) {
		stop();
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
		stop();
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

FaceDetect::NeuralNet *QmlWin::neuralNet() const
{
	return m_neuralNet;
}

FaceDetect::NetTrainer *QmlWin::netTrainer() const
{
	return m_trainer.data();
}

QVariant QmlWin::filterSettings() const
{
	return m_filterSettings;
}

void QmlWin::setFilterSettings(const QVariant &filterSettings)
{
	if (m_filterSettings != filterSettings) {
		stop();
		m_trainingDatabase->clear();
		m_faceFileScanner->reset();
		m_nonFaceFileScanner->reset();
		m_filterSettings = filterSettings;
		m_filter.setFilterData(m_filterSettings);
		updateFilters();
		emit filterSettingsChanged(m_filterSettings);
	}
}

QString QmlWin::netType() const
{
	return m_netType;
}

void QmlWin::setNetType(const QString &netType)
{
	if (m_netType != netType) {
		stop();
		if (!m_neuralNets.contains(netType)) {
			m_neuralNets[netType] = QSharedPointer<FaceDetect::NeuralNet>(FaceDetect::NeuralNet::create(netType.toLatin1().constData()));
			connect(m_neuralNets[netType].data(), SIGNAL(initializedChanged(bool)), SLOT(onNetInitializedChanged()));
		}
		m_netType = netType;
		m_neuralNet = m_neuralNets[netType].data();
		m_neuralNet->setInputVectorSize(m_trainingDatabase->inputVectorSize());
		m_neuralNet->setOutputVectorSize(m_trainingDatabase->outputVectorSize());
		emit netTypeChanged(m_netType);
		emit neuralNetChanged(m_neuralNet);
	}
}

int QmlWin::trainingSetPercent() const
{
	return m_trainingSetPercent;
}

void QmlWin::setTrainingSetPercent(int percent)
{
	if (percent != m_trainingSetPercent) {
		stop();
		m_trainingSetPercent = percent;
		trainingSetPercentChanged(percent);
	}
}

double QmlWin::learningSpeed() const
{
	return m_learningSpeed;
}

void QmlWin::setLearningSpeed(double speed)
{
	if (speed != m_learningSpeed) {
		stop();
		m_learningSpeed = speed;
		learningSpeedChanged(speed);
	}
}

FaceDetect::FaceDetector *QmlWin::faceDetector() const
{
	return m_faceDetector.data();
}

QPoint QmlWin::detectCenter() const
{
	return m_detectCenter;
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
	stopFaceDetector();
	if (!containsStep(this, "scanFaces")) {
		m_steps.append(ProcessStep(this, "scanFaces"));
	}
	if (!containsStep(this, "scanNonFaces")) {
		m_steps.append(ProcessStep(this, "scanNonFaces"));
	}
	if (!containsStep(this, "trainNet")) {
		m_trainer->stop();
		m_steps.append(ProcessStep(this, "trainNet"));
		m_neuralNet->setLearningSpeed(m_learningSpeed);
		startNextStep();
	}
}

void QmlWin::stop()
{
	m_steps.clear();
	m_faceFileScanner->stop();
	m_nonFaceFileScanner->stop();
	m_trainer->stop();
	stopFaceDetector();
}

void QmlWin::saveNet(const QString &saveFileName)
{
	stop();
	QByteArray fileName = saveFileName.toUtf8();
	std::ofstream ofs(fileName.constData());
	if (ofs.is_open()) {
		boost::archive::text_oarchive oa(ofs);
		const FaceDetect::NeuralNet *net = m_neuralNet;
		FaceDetect::ImageFilter filter = m_globalFilter;
		filter.mergeLocalFilter(m_localFilter);
		oa << boost::serialization::make_nvp("filter", filter);
		oa << boost::serialization::make_nvp("net", net);
	}
}

void QmlWin::loadNet(const QString &loadFileName)
{
	stop();
	QByteArray fileName = loadFileName.toUtf8();
	std::ifstream ifs(fileName.constData());
	if (ifs.is_open()) {
		try {
			boost::archive::text_iarchive ia(ifs);
			FaceDetect::NeuralNet *net = 0;
			FaceDetect::ImageFilter filter;
			ia >> boost::serialization::make_nvp("filter", filter);
			ia >> boost::serialization::make_nvp("net", net);
			m_globalFilter = filter.globalPart();
			m_localFilter = filter.localPart();
			m_neuralNet = net;
			connect(net, SIGNAL(initializedChanged(bool)), SLOT(onNetInitializedChanged()));
			emit neuralNetChanged(net);
			m_neuralNets[net->netType()] = QSharedPointer<FaceDetect::NeuralNet>(net);
			m_netType = m_neuralNet->netType();
			emit netTypeChanged(m_netType);
			if (m_filterSettings != filter.filterData()) {
				m_filterSettings = filter.filterData();
				emit filterSettingsChanged(m_filterSettings);
			}
		}
		catch(boost::archive::archive_exception&) {
			QMessageBox::warning(this, tr("Bad format"), tr("Bad net format"));
		}
	}
}

void QmlWin::detect(const QString &fileName)
{
	QImage image(fileName);
	if (!image.isNull()) {
		detect(image);
	}
}

void QmlWin::detect(const QImage &image)
{
	if (!m_neuralNet->isInitialized()) {
		return;
	}
	stop();
	if (!m_faceDetector.isNull()) {
		m_faceDetector->stop();
	}
	auto faceDetectorTmp = m_faceDetector;
	m_faceDetector = QSharedPointer<FaceDetect::FaceDetector>(new FaceDetect::FaceDetector(m_neuralNet));
	m_faceDetector->setImage(image);
	FaceDetect::ImageSegmenter::Settings settings;
	settings.xStep = 1;
	settings.yStep = 1;
	settings.segmentSize = QSize(20, 20);
	m_faceDetector->setLocalFilter(m_localFilter);
	m_faceDetector->setGlobalFilter(m_globalFilter);
	m_faceDetector->setupSegmenter(settings);
	emit faceDetectorChanged(m_faceDetector.data());
	connect(m_faceDetector.data(), SIGNAL(scanResultReturned(FaceDetect::FaceDetector::DetectionWindow)), SLOT(onScanResultReturned(FaceDetect::FaceDetector::DetectionWindow)));
	connect(m_faceDetector.data(), SIGNAL(progressChanged(double,QPolygon)), SLOT(onDetectorProgressChanged(double,QPolygon)));
	m_faceDetector->start();
}

void QmlWin::stopFaceDetector()
{
	if (!m_faceDetector.isNull()) {
		m_faceDetector->stop();
		auto detector = m_faceDetector;
		m_faceDetector.clear();
		emit faceDetectorChanged(m_faceDetector.data());
	}
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

void QmlWin::onImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image)
{
	m_trainingDatabase->addImage(image);
}

void QmlWin::onEpochFinished(const FaceDetect::NetTrainer::EpochStats &stats)
{
	emit epochFinished(stats.epoch, stats.mseA, stats.mseE, stats.mseBinA, stats.mseBinE, int(stats.sizeA), int(stats.sizeE));
}

void QmlWin::onErrorCalculated(std::size_t sample, std::size_t sampleCount, double errorSum)
{
	emit errorCalculated(int(sample), errorSum / static_cast<double>(sampleCount));
}

void QmlWin::onScanResultReturned(const FaceDetect::FaceDetector::DetectionWindow &window)
{
	QVariantMap result;
	result["value"] = window.value;
	result["rect"] = window.polygon.boundingRect();
	emit scanResultReturned(result);
}

void QmlWin::onNetInitializedChanged()
{
	if (!m_neuralNet->isInitialized()) {
		stopFaceDetector();
	}
}

void QmlWin::onDetectorProgressChanged(double progress, const QPolygon &rect)
{
	Q_UNUSED(progress);
	m_detectCenter = rect.boundingRect().center();
	emit detectCenterChanged(m_detectCenter);
}

void QmlWin::onImageScanned(const LaVectorDouble &input, const LaVectorDouble &output)
{
	m_trainingDatabase->addImage(input, output);
}

bool QmlWin::containsStep(const QObject *object, const QByteArray &name) const
{
	foreach (const ProcessStep &step, m_steps) {
		if (step.method == name && step.object == object) {
			return true;
		}
	}
	return false;
}

void QmlWin::loadSettings()
{
	m_settings.beginGroup("paths");
	m_facesPath = m_settings.value("faces").toString();
	m_nonFacesPath = m_settings.value("nofaces").toString();
	m_faceCachePath = m_settings.value("faceCache").toString();
	m_trainingDatabase->setCacheDir(m_faceCachePath);
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
		connect(m_faceFileScanner.data(), SIGNAL(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)), SLOT(onImageScanned(FaceDetect::FaceFileScanner::ImageInfo)));
	}
	if (m_nonFaceFileScanner.isNull()) {
		m_nonFaceFileScanner = QSharedPointer<ImageFileScanner>(new ImageFileScanner());
		m_nonFaceFileScanner->setScanPath(m_nonFacesPath);
		connect(m_nonFaceFileScanner.data(), SIGNAL(finished()), SLOT(startNextStep()));
		connect(m_nonFaceFileScanner.data(), SIGNAL(terminated()), SLOT(startNextStep()));
		connect(m_nonFaceFileScanner.data(), SIGNAL(imageScanned(LaVectorDouble,LaVectorDouble)), SLOT(onImageScanned(LaVectorDouble,LaVectorDouble)));
	}
	m_nonFaceFileScanner->setLocalFilter(m_localFilter);
	m_nonFaceFileScanner->setGlobalFilter(m_globalFilter);
}

void QmlWin::createFilterSettings()
{
	m_filterSettings = m_filter.filterData();
	updateFilters();
}

void QmlWin::createNeuralNet()
{
	m_neuralNets["bp"] = QSharedPointer<FaceDetect::NeuralNet>(new FaceDetect::BPNeuralNet());
	m_neuralNet = m_neuralNets["bp"].data();
	connect(m_neuralNet, SIGNAL(initializedChanged(bool)), SLOT(onNetInitializedChanged()));
	m_netType = "bp";
}

void QmlWin::createNetTrainer()
{
	m_trainer = QSharedPointer<FaceDetect::NetTrainer>(new FaceDetect::NetTrainer);
	m_trainer->setNumEpoch(100);
	connect(m_trainer.data(), SIGNAL(epochFinished(FaceDetect::NetTrainer::EpochStats)), this, SLOT(onEpochFinished(FaceDetect::NetTrainer::EpochStats)));
	connect(m_trainer.data(), SIGNAL(errorCalculated(std::size_t,std::size_t,double)), this, SLOT(onErrorCalculated(std::size_t,std::size_t,double)));
}

void QmlWin::updateFilters()
{
	m_globalFilter = m_filter;
	m_globalFilter.disableIlluminationFilter();
	m_localFilter = m_filter;
	m_localFilter.disableGaborFilter();
	m_localFilter.disableGrayscaleFilter();
	m_localFilter.disableSobelFilter();
	m_trainingDatabase->setGlobalFilter(m_globalFilter);
	m_trainingDatabase->setLocalFilter(m_localFilter);
	if (!m_nonFaceFileScanner.isNull()) {
		m_nonFaceFileScanner->setLocalFilter(m_localFilter);
		m_nonFaceFileScanner->setGlobalFilter(m_globalFilter);
	}
}

void QmlWin::scanFaces()
{
	m_faceFileScanner->start();
}

void QmlWin::scanNonFaces()
{
	m_nonFaceFileScanner->start();
}

void QmlWin::trainNet()
{
	m_trainer->setTrainingDataReader(m_trainingDatabase);
	m_trainer->setTrainingSetSize(m_trainingDatabase->trainingSetSize() * (static_cast<double>(m_trainingSetPercent) / 100.0));
	m_trainer->trainNet(m_neuralNet);
}


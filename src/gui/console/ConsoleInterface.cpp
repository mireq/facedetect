/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.02.2011 15:27:45
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <cstdio>
#include <fstream>
#include <QApplication>
#include <QMetaType>
#include <QSettings>
#include "libfacedetect/FaceDetector.h"
#include "libfacedetect/ImageSegmenter.h"
#include "ConsoleInterface.h"

ConsoleInterface::ConsoleInterface(QObject *parent):
	QObject(parent),
	m_illuminationPlaneOnly(false),
	m_noIlluminationCorrectHistogram(true),
	m_grayscaleFilter(false),
	m_illuminationFilter(false),
	m_sobelFilter(false),
	m_gaborFilter(false),
	m_onlyGaborWavelet(false),
	m_trainingSetPercent(100),
	m_quiet(false),
	m_printAlign(false),
	m_faceCount(0),
	m_nonFaceCount(0),
	m_cout(stdout),
	m_trainingDatabase(new FaceDetect::TrainingImageDatabase(this))
{
	QSettings settings;
	settings.beginGroup("paths");
	m_facesPath = settings.value("faces").toString();
	m_nonFacesPath = settings.value("nonfaces").toString();
	m_faceCachePath = settings.value("facecache").toString();
	settings.endGroup();

	parseCommandline();
	qRegisterMetaType<std::size_t>("std::size_t");
	qRegisterMetaType<LaVectorDouble>("LaVectorDouble");
}

ConsoleInterface::~ConsoleInterface()
{
}

void ConsoleInterface::run()
{
	m_trainingDatabase->setCacheDir(m_faceCachePath);
	if (!m_filterImage.isEmpty()) {
		m_steps.append(ProcessStep(this, "saveFilterImage"));
	}
	if (m_printAlign) {
		m_steps.append(ProcessStep(this, "startPrintAlign"));
		m_steps.append(ProcessStep(this, "printAlign"));
	}
	m_steps.append(ProcessStep(this, "startTraining"));
	m_steps.append(ProcessStep(this, "detectFaces"));
	m_steps.append(ProcessStep{qApp, "quit"});
	startNextStep();
}

void ConsoleInterface::startNextStep()
{
	if (m_steps.isEmpty()) {
		return;
	}
	ProcessStep step = m_steps.takeFirst();
	QMetaObject::invokeMethod(step.object, step.method.constData(), Qt::QueuedConnection);
}

void ConsoleInterface::saveFilterImage()
{
	FaceDetect::ImageFilter filter;
	FaceDetect::ImageFilter::Filters filterFlags;
	if (m_grayscaleFilter) {
		filterFlags |= FaceDetect::ImageFilter::GrayscaleFilter;
	}
	if (m_illuminationFilter) {
		filterFlags |= FaceDetect::ImageFilter::IlluminationFilter;
	}
	if (m_sobelFilter) {
		filterFlags |= FaceDetect::ImageFilter::SobelFilter;
	}
	if (m_gaborFilter) {
		filterFlags |= FaceDetect::ImageFilter::GaborFilter;
	}
	filter.setFilters(filterFlags);
	filter.setIlluminationPlaneOnly(m_illuminationPlaneOnly);
	filter.setIlluminationCorrectHistogram(!m_noIlluminationCorrectHistogram);
	filter.setOnlyGaborWavelet(m_onlyGaborWavelet);
	filter.setGaborParameters(m_gaborParameters);

	QImage inputImage(m_filterImage);
	if (inputImage.isNull()) {
		return;
	}
	QImage output = filter.filterImage(inputImage);
	output.save(m_filterImage + ".tr.png", "PNG");
	startNextStep();
}

void ConsoleInterface::startPrintAlign()
{
	if (!m_quiet) {
		m_cout << "Scanning face database " << m_facesPath << "\n";
		m_cout.flush();
	}
	m_faceData.clear();
	m_aligner = QSharedPointer<FaceDetect::Align>(new FaceDetect::Align());
	m_aligner->setImageSize(128);
	m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(new FaceDetect::FaceFileScanner());
	m_faceScanner->setFilterFrontal(true);
	m_faceScanner->setBasePath(m_facesPath);
	connect(m_faceScanner.data(), SIGNAL(progressChanged(double)), this, SLOT(updateProgress(double)));
	connect(m_faceScanner.data(), SIGNAL(finished()), SLOT(startNextStep()));
	connect(m_faceScanner.data(), SIGNAL(terminated()), SLOT(startNextStep()));
	connect(m_faceScanner.data(), SIGNAL(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)), SLOT(alignImageScanned(FaceDetect::FaceFileScanner::ImageInfo)));
	m_faceScanner->start();
}

void ConsoleInterface::printAlign()
{
	foreach (const FaceDetect::FaceFileScanner::FaceData &data, m_faceData) {
		QTransform transform = m_aligner->getTransform(data);
		if (transform.type() != QTransform::TxNone) {
			printFaceFeaturesData(data, transform);
		}
	}
	m_cout << "-- average --\n";
	printFaceFeaturesData(m_aligner->getAverageFeatures());
	m_cout.flush();

	m_aligner = QSharedPointer<FaceDetect::Align>(0);
	m_faceData.clear();
	startNextStep();
}

void ConsoleInterface::alignImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image)
{
	m_aligner->addImage(image);
	for (auto face = image.faceBegin(); face != image.faceEnd(); ++face) {
		m_faceData.append(*face);
	}
}

void ConsoleInterface::startTraining()
{
	bool unserialized = false;
	FaceDetect::BPNeuralNet *net = 0;
	if (!m_loadNetFile.isEmpty()) {
		QByteArray fileName = m_loadNetFile.toUtf8();
		std::ifstream ifs(fileName.constData());
		if (ifs.is_open()) {
			boost::archive::text_iarchive ia(ifs);
			ia >> net;
			unserialized = true;
		}
	}
	if (net == 0) {
		net = new FaceDetect::BPNeuralNet();
	}
	m_neuralNet = QSharedPointer<FaceDetect::NeuralNet>(net);

	if (!unserialized) {
		m_steps.prepend(ProcessStep(this, "trainNet"));
		m_steps.prepend(ProcessStep(this, "scanNonFaceDatabase"));
		m_steps.prepend(ProcessStep(this, "scanFaceDatabase"));
	}
	startNextStep();
}

void ConsoleInterface::detectFaces()
{
	foreach (const QString &file, m_detectFiles) {
		scanImageFile(file);
	}
	startNextStep();
}

void ConsoleInterface::scanFaceDatabase()
{
	if (!m_quiet) {
		m_cout << "Scanning face database " << m_facesPath << "\n";
		m_cout.flush();
	}
	m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(new FaceDetect::FaceFileScanner());
	m_faceScanner->setFilterFrontal(true);
	m_faceScanner->setBasePath(m_facesPath);
	connect(m_faceScanner.data(), SIGNAL(progressChanged(double)), this, SLOT(updateProgress(double)));
	connect(m_faceScanner.data(), SIGNAL(finished()), SLOT(onFaceScanningFinished()));
	connect(m_faceScanner.data(), SIGNAL(terminated()), SLOT(onFaceScanningFinished()));
	connect(m_faceScanner.data(), SIGNAL(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)), SLOT(onImageScanned(FaceDetect::FaceFileScanner::ImageInfo)));
	m_faceScanner->start();
}

void ConsoleInterface::scanNonFaceDatabase()
{
	if (!m_quiet) {
		m_cout << "\rScanning nonface database " << m_nonFacesPath << "\n";
		m_cout.flush();
	}
	m_nonfaceScanner = QSharedPointer<FaceDetect::ImageFileScanner>(new FaceDetect::ImageFileScanner());
	m_nonfaceScanner->setScanPath(m_nonFacesPath);
	connect(m_nonfaceScanner.data(), SIGNAL(finished()), SLOT(onNonFaceScanningFinished()));
	connect(m_nonfaceScanner.data(), SIGNAL(terminated()), SLOT(onNonFaceScanningFinished()));
	connect(m_nonfaceScanner.data(), SIGNAL(imageScanned(LaVectorDouble,LaVectorDouble)), SLOT(onImageScanned(LaVectorDouble,LaVectorDouble)));
	m_nonfaceScanner->start();
}

void ConsoleInterface::onFaceScanningFinished()
{
	m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(0);
	startNextStep();
	m_faceCount = m_trainingDatabase->trainingSetSize() - m_nonFaceCount;
	if (!m_quiet) {
		m_cout << "\rFace count: " << m_faceCount << "\n";
		m_cout.flush();
	}
}

void ConsoleInterface::onNonFaceScanningFinished()
{
	m_nonfaceScanner = QSharedPointer<FaceDetect::ImageFileScanner>(0);
	startNextStep();
	m_nonFaceCount = m_trainingDatabase->trainingSetSize() - m_faceCount;
	if (!m_quiet) {
		m_cout << "\rNon face count: " << m_nonFaceCount << "\n";
		m_cout.flush();
	}
}

void ConsoleInterface::trainNet()
{
	m_trainingDatabase->shuffle();
	m_trainer = QSharedPointer<FaceDetect::NetTrainer>(new FaceDetect::NetTrainer);
	m_trainer->setNumEpoch(55);
	m_trainer->setTrainingDataReader(m_trainingDatabase);
	m_trainer->setTrainingSetSize(m_trainingDatabase->trainingSetSize() * (static_cast<double>(m_trainingSetPercent) / 100.0));
	m_neuralNet->setLearningSpeed(0.0004);

	if (!m_quiet) {
		m_cout << "\rStarting training\n";
		m_cout.flush();
	}
	connect(m_trainer.data(), SIGNAL(finished()), this, SLOT(onTrainingFinished()));
	connect(m_trainer.data(), SIGNAL(terminated()), this, SLOT(onTrainingFinished()));
	connect(m_trainer.data(), SIGNAL(epochFinished(int,double,double)), this, SLOT(printTrainingEpoch(int,double,double)));
	connect(m_trainer.data(), SIGNAL(sampleFinished(std::size_t,int)), this, SLOT(printTrainingSample(std::size_t,int)));
	m_trainer->trainNet(m_neuralNet.data());
}

void ConsoleInterface::onTrainingFinished()
{
	saveNeuralNet();
	m_trainer = QSharedPointer<FaceDetect::NetTrainer>(0);
	startNextStep();
}

void ConsoleInterface::onImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image)
{
	m_trainingDatabase->addImage(image);
}

void ConsoleInterface::onImageScanned(const LaVectorDouble &input, const LaVectorDouble &output)
{
	m_trainingDatabase->addImage(input, output);
}

void ConsoleInterface::updateProgress(double progress)
{
	if (!m_quiet) {
		m_cout << "\r" << QString("%1%").arg(progress * 100, 5, 'f', 1);
		m_cout.flush();
	}
}

void ConsoleInterface::printTrainingEpoch(int epoch, double msea, double msee)
{
	if (!m_quiet) {
		m_cout << "\rEpoch: " << epoch << ", MSEA: " << msea << ", MSEE:" << msee << "\n";
		m_cout.flush();
	}
}

void ConsoleInterface::printTrainingSample(std::size_t sample, int epoch)
{
	if (!m_quiet) {
		m_cout << "\rEpoch: " << epoch << " - " << QString("%1%").arg((static_cast<double>(sample) / m_trainingDatabase->trainingSetSize()) * 100.0, 5, 'f', 1);
		m_cout.flush();
	}
}
void ConsoleInterface::parseCommandline()
{
	QStringList arguments = qApp->arguments();
	m_filterImage = getArgument(arguments, "--filterImage");
	m_illuminationPlaneOnly = getBoolArgument(arguments, "--illuminationPlaneOnly");
	m_noIlluminationCorrectHistogram = getBoolArgument(arguments, "--noIlluminationCorrectHistogram");
	m_grayscaleFilter = getBoolArgument(arguments, "--grayscaleFilter");
	m_illuminationFilter = getBoolArgument(arguments, "--illuminationFilter");
	m_sobelFilter = getBoolArgument(arguments, "--sobelFilter");
	m_gaborFilter = getBoolArgument(arguments, "--gaborFilter");
	m_onlyGaborWavelet = getBoolArgument(arguments, "--onlyGaborWavelet");
	if (getBoolArgument(arguments, "--gaborLambda")) {
		m_gaborParameters.lambda = getDoubleArgument(arguments, "--gaborLambda");
	}
	if (getBoolArgument(arguments, "--gaborTheta")) {
		m_gaborParameters.theta = getDoubleArgument(arguments, "--gaborTheta");
	}
	if (getBoolArgument(arguments, "--gaborPsi")) {
		m_gaborParameters.psi = getDoubleArgument(arguments, "--gaborPsi");
	}
	if (getBoolArgument(arguments, "--gaborSigma")) {
		m_gaborParameters.sigma = getDoubleArgument(arguments, "--gaborSigma");
	}
	if (getBoolArgument(arguments, "--gaborGamma")) {
		m_gaborParameters.gamma = getDoubleArgument(arguments, "--gaborGamma");
	}
	if (getBoolArgument(arguments, "--gaborLuminance")) {
		m_gaborParameters.lum = getDoubleArgument(arguments, "--gaborLuminance");
	}
	m_loadNetFile = getArgument(arguments, "--loadNet");
	m_saveNetFile = getArgument(arguments, "--saveNet");
	m_detectFiles = getArguments(arguments, "--detect");
	m_faceCachePath = getArgument(arguments, "--faceCache");
	m_quiet = getBoolArgument(arguments, "--quiet");
	m_printAlign = getBoolArgument(arguments, "--printAlign");
	m_trainingSetPercent = getIntArgument(arguments, "--trainingSetPercent");
}

QString ConsoleInterface::getArgument(const QStringList &arguments, const QString &argumentName)
{
	int argumentIdx = arguments.indexOf(argumentName);
	if (argumentIdx < 0) {
		return QString();
	}

	// Za názvom argumentu musí nasledovať hodnota
	if (argumentIdx + 1 >= arguments.count()) {
		return QString();
	}

	return arguments.at(argumentIdx + 1);
}

bool ConsoleInterface::getBoolArgument(const QStringList &arguments, const QString &argumentName)
{
	return arguments.contains(argumentName);
}

double ConsoleInterface::getDoubleArgument(const QStringList &arguments, const QString &argumentName)
{
	int argumentIdx = arguments.indexOf(argumentName);
	if (argumentIdx < 0) {
		return 0;
	}

	// Za názvom argumentu musí nasledovať hodnota
	if (argumentIdx + 1 >= arguments.count()) {
		return 0;
	}

	return arguments.at(argumentIdx + 1).toDouble();
}

int ConsoleInterface::getIntArgument(const QStringList &arguments, const QString &argumentName)
{
	int argumentIdx = arguments.indexOf(argumentName);
	if (argumentIdx < 0) {
		return 0;
	}
	if (argumentIdx + 1 >= arguments.count()) {
		return 0;
	}

	return arguments.at(argumentIdx + 1).toInt();
}

QStringList ConsoleInterface::getArguments(const QStringList &arguments, const QString &argumentName)
{
	QStringList ret;
	int argumentIdx = 0;
	while (argumentIdx >= 0) {
		argumentIdx = arguments.indexOf(argumentName, argumentIdx + 1);
		// Žiaden ďalší argument sa nenašiel
		if (argumentIdx < 0) {
			return ret;
		}
		// Za argumentom musí nasledovať hodnota
		if (argumentIdx + 1 >= arguments.count()) {
			return ret;
		}
		ret << arguments.at(argumentIdx + 1);
	}
	return ret;
}

void ConsoleInterface::saveNeuralNet()
{
	if (m_saveNetFile.isEmpty()) {
		return;
	}
	QByteArray fileName = m_saveNetFile.toUtf8();
	std::ofstream ofs(fileName.constData());
	if (ofs.is_open()) {
		boost::archive::text_oarchive oa(ofs);
		FaceDetect::BPNeuralNet *net = dynamic_cast<FaceDetect::BPNeuralNet*>(m_neuralNet.data());
		oa << net;
	}
}

void ConsoleInterface::scanImageFile(const QString &file)
{
	QImage image(file);
	if (!m_quiet) {
		m_cout << "\rScanning image " << file << "\n";
		m_cout.flush();
	}
	if (image.isNull()) {
		return;
	}

	FaceDetect::FaceDetector detector(m_neuralNet.data());
	detector.setImage(image);
	FaceDetect::ImageSegmenter::Settings settings;
	settings.xStep = 1;
	settings.yStep = 1;
	settings.segmentSize = QSize(20, 20);
	detector.setupSegmenter(settings);
	detector.scanImage();
}

void ConsoleInterface::printFaceFeaturesData(const FaceDetect::FaceFileScanner::FaceData &data, const QTransform &transform) const
{
	QPointF point;
	point = transform.map(QPointF(data.leftEye));
	m_cout << point.x() << "\t" << point.y() << "\t";
	point = transform.map(QPointF(data.rightEye));
	m_cout << point.x() << "\t" << point.y() << "\t";
	point = transform.map(QPointF(data.nose));
	m_cout << point.x() << "\t" << point.y() << "\t";
	point = transform.map(QPointF(data.mouth));
	m_cout << point.x() << "\t" << point.y();
	m_cout << "\n";
}


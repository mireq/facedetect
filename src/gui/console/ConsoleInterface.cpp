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
#include "libfacedetect/ImageFilter.h"
#include "libfacedetect/ImageSegmenter.h"
#include "ConsoleInterface.h"

ConsoleInterface::ConsoleInterface(QObject *parent):
	QObject(parent),
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
	net->setTrainingDataReader(m_trainingDatabase);
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
	m_cout << "Scanning face database " << m_facesPath << "\n";
	m_cout.flush();
	m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(new FaceDetect::FaceFileScanner());
	m_faceScanner->setBasePath(m_facesPath);
	connect(m_faceScanner.data(), SIGNAL(progressChanged(double)), this, SLOT(updateProgress(double)));
	connect(m_faceScanner.data(), SIGNAL(finished()), SLOT(onFaceScanningFinished()));
	connect(m_faceScanner.data(), SIGNAL(terminated()), SLOT(onFaceScanningFinished()));
	connect(m_faceScanner.data(), SIGNAL(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)), SLOT(onImageScanned(FaceDetect::FaceFileScanner::ImageInfo)));
	m_faceScanner->start();
}

void ConsoleInterface::scanNonFaceDatabase()
{
	m_cout << "\rScanning nonface database " << m_nonFacesPath << "\n";
	m_cout.flush();
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
}

void ConsoleInterface::onNonFaceScanningFinished()
{
	m_nonfaceScanner = QSharedPointer<FaceDetect::ImageFileScanner>(0);
	startNextStep();
}

void ConsoleInterface::trainNet()
{
	m_cout.flush();
	m_trainingDatabase->shuffle();
	m_neuralNet->setLearningSpeed(0.005);

	m_cout << "\rStarting training\n";
	connect(m_neuralNet.data(), SIGNAL(finished()), this, SLOT(onTrainingFinished()));
	connect(m_neuralNet.data(), SIGNAL(terminated()), this, SLOT(onTrainingFinished()));
	connect(m_neuralNet.data(), SIGNAL(epochFinished(int,double)), this, SLOT(printTrainingEpoch(int,double)));
	connect(m_neuralNet.data(), SIGNAL(sampleFinished(std::size_t,int)), this, SLOT(printTrainingSample(std::size_t,int)));
	m_neuralNet->start();
}

void ConsoleInterface::onTrainingFinished()
{
	saveNeuralNet();
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
	m_cout << "\r" << QString("%1%").arg(progress * 100, 5, 'f', 1);
	m_cout.flush();
}

void ConsoleInterface::printTrainingEpoch(int epoch, double mse)
{
	m_cout << "\rEpoch: " << epoch << ", MSE: " << mse << "\n";
	m_cout.flush();
}

void ConsoleInterface::printTrainingSample(std::size_t sample, int epoch)
{
	m_cout << "\rEpoch: " << epoch << " - " << QString("%1%").arg((static_cast<double>(sample) / m_trainingDatabase->trainingSetSize()) * 100.0, 5, 'f', 1);
	m_cout.flush();
}
void ConsoleInterface::parseCommandline()
{
	QStringList arguments = qApp->arguments();
	m_loadNetFile = getArgument(arguments, "--loadNet");
	m_saveNetFile = getArgument(arguments, "--saveNet");
	m_detectFiles = getArguments(arguments, "--detect");
	m_faceCachePath = getArgument(arguments, "--faceCache");
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
	m_cout << "\rScanning image " << file << "\n";
	m_cout.flush();
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


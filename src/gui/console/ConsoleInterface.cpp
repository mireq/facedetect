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
#include "libfacedetect/ImageFilter.h"
#include "libfacedetect/ImageSegmenter.h"
#include "ConsoleInterface.h"

ConsoleInterface::ConsoleInterface(QObject *parent):
	QObject(parent),
	m_cout(stdout),
	m_trainingDatabase(new FaceDetect::TrainingImageDatabase(this))
{
	parseCommandline();
	qRegisterMetaType<std::size_t>("std::size_t");
	qRegisterMetaType<LaVectorDouble>("LaVectorDouble");
}

ConsoleInterface::~ConsoleInterface()
{
}

void ConsoleInterface::run()
{
	QSettings settings;
	settings.beginGroup("paths");
	m_facesPath = settings.value("faces").toString();
	m_nonFacesPath = settings.value("nonfaces").toString();
	settings.endGroup();
	QMetaObject::invokeMethod(this, "startScan", Qt::QueuedConnection);
}

void ConsoleInterface::startScan()
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
	if (unserialized) {
		onTrainingFinished();
	}
	else {
		scanFaces();
	}
}

void ConsoleInterface::scanFaces()
{
	m_cout << "Scanning face database " << m_facesPath << "\n";
	m_cout.flush();
	m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(new FaceDetect::FaceFileScanner());
	m_faceScanner->setBasePath(m_facesPath);
	connect(m_faceScanner.data(), SIGNAL(progressChanged(double)), this, SLOT(updateProgress(double)));
	connect(m_faceScanner.data(), SIGNAL(scanningChanged(bool)), this, SLOT(onFaceScanningStatusChanged(bool)));
	connect(m_faceScanner.data(), SIGNAL(imageScanned(FaceDetect::FaceFileScanner::ImageInfo)), SLOT(onImageScanned(FaceDetect::FaceFileScanner::ImageInfo)));
	m_faceScanner->start();
}

void ConsoleInterface::scanNonFaces()
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

void ConsoleInterface::startTraining()
{
	m_cout.flush();
	m_trainingDatabase->shuffle();

	m_cout << "\rStarting training\n";
	m_neuralNet->setLearningSpeed(0.01);
	connect(m_neuralNet.data(), SIGNAL(finished()), this, SLOT(onTrainingFinished()));
	connect(m_neuralNet.data(), SIGNAL(terminated()), this, SLOT(onTrainingFinished()));
	connect(m_neuralNet.data(), SIGNAL(epochFinished(int,double)), this, SLOT(printTrainingEpoch(int,double)));
	connect(m_neuralNet.data(), SIGNAL(sampleFinished(std::size_t,int)), this, SLOT(printTrainingSample(std::size_t,int)));
	m_neuralNet->start();
}

void ConsoleInterface::detectFaces()
{
	foreach (const QString &file, m_detectFiles) {
		scanImageFile(file);
	}
	qApp->quit();
}

void ConsoleInterface::onFaceScanningStatusChanged(bool scanning)
{
	if (!scanning) {
		m_faceScanner = QSharedPointer<FaceDetect::FaceFileScanner>(0);
		scanNonFaces();
	}
}

void ConsoleInterface::onNonFaceScanningFinished()
{
	startTraining();
}

void ConsoleInterface::onImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image)
{
	m_trainingDatabase->addImage(image);
}

void ConsoleInterface::onImageScanned(const LaVectorDouble &input, const LaVectorDouble &output)
{
	m_trainingDatabase->addImage(input, output);
}

void ConsoleInterface::onTrainingFinished()
{
	saveNeuralNet();
	detectFaces();
	//qApp->quit();
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
	if (image.isNull()) {
		return;
	}
}


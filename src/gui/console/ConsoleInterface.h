/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.02.2011 15:27:48
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef CONSOLEINTERFACE_9JAL83V
#define CONSOLEINTERFACE_9JAL83V

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QTextStream>
#include <lapackpp/lavd.h>
#include "libfacedetect/BPNeuralNet.h"
#include "libfacedetect/FaceFileScanner.h"
#include "libfacedetect/ImageFileScanner.h"
#include "libfacedetect/ImageFilter.h"
#include "libfacedetect/NetTrainer.h"
#include "libfacedetect/TrainingImageDatabase.h"

class ConsoleInterface: public QObject
{
Q_OBJECT
public:
	ConsoleInterface(QObject *parent = 0);
	~ConsoleInterface();
	void run();

private slots:
	void startNextStep();

	void saveFilterImage();
	void startPrintAlign();
	void printAlign();
	void alignImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image);

	void startTraining();
	void detectFaces();

	void scanFaceDatabase();
	void scanNonFaceDatabase();
	void onFaceScanningFinished();
	void onNonFaceScanningFinished();
	void trainNet();
	void onTrainingFinished();

	// Záznam skenovaných fotografií
	void onImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image);
	void onImageScanned(const LaVectorDouble &input, const LaVectorDouble &output);

	// Výpisy
	void updateProgress(double progress);
	void printTrainingEpoch(int epoch, double msea, double msee, double msebina = 0, double msebine = 0, double thresholda = 0, double thresholde = 0);
	void printTrainingSample(std::size_t sample, int epoch);

private:
	void parseCommandline();
	QString getArgument(const QStringList &arguments, const QString &argumentName);
	bool getBoolArgument(const QStringList &arguments, const QString &argumentName);
	double getDoubleArgument(const QStringList &arguments, const QString &argumentName);
	int getIntArgument(const QStringList &arguments, const QString &argumentName);
	QStringList getArguments(const QStringList &arguments, const QString &argumentName);
	void saveNeuralNet();
	void scanImageFile(const QString &file);
	void printFaceFeaturesData(const FaceDetect::FaceFileScanner::FaceData &data, const QTransform &transform = QTransform()) const;

private:
	QString m_filterImage;
	bool m_illuminationPlaneOnly;
	bool m_noIlluminationCorrectHistogram;
	bool m_grayscaleFilter;
	bool m_illuminationFilter;
	bool m_sobelFilter;
	bool m_gaborFilter;
	bool m_onlyGaborWavelet;
	int m_trainingSetPercent;
	FaceDetect::ImageFilter::GaborParameters m_gaborParameters;

	QString m_loadNetFile;
	QString m_saveNetFile;
	QStringList m_detectFiles;

	QString m_facesPath;
	QString m_nonFacesPath;
	QString m_faceCachePath;
	bool m_quiet;
	bool m_printAlign;
	bool m_printTraining;
	std::size_t m_faceCount;
	std::size_t m_nonFaceCount;
	mutable QTextStream m_cout;
	QSharedPointer<FaceDetect::FaceFileScanner> m_faceScanner;
	QSharedPointer<FaceDetect::ImageFileScanner> m_nonfaceScanner;
	QSharedPointer<FaceDetect::NeuralNet> m_neuralNet;
	QSharedPointer<FaceDetect::Align> m_aligner;
	QSharedPointer<FaceDetect::NetTrainer> m_trainer;
	QVector<FaceDetect::FaceFileScanner::FaceData> m_faceData;
	FaceDetect::TrainingImageDatabase *m_trainingDatabase;
	struct ProcessStep {
		ProcessStep(QObject *o, const QByteArray &m): object(o), method(m) {};
		QObject *object;
		QByteArray method;
	};
	QList<ProcessStep> m_steps;
}; /* -----  end of class ConsoleInterface  ----- */

#endif /* end of include guard: CONSOLEINTERFACE_9JAL83V */


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
	void printTrainingEpoch(int epoch, double mse);
	void printTrainingSample(std::size_t sample, int epoch);

private:
	void parseCommandline();
	QString getArgument(const QStringList &arguments, const QString &argumentName);
	QStringList getArguments(const QStringList &arguments, const QString &argumentName);
	void saveNeuralNet();
	void scanImageFile(const QString &file);

private:
	QString m_loadNetFile;
	QString m_saveNetFile;
	QStringList m_detectFiles;

	QString m_facesPath;
	QString m_nonFacesPath;
	QString m_faceCachePath;
	QTextStream m_cout;
	QSharedPointer<FaceDetect::FaceFileScanner> m_faceScanner;
	QSharedPointer<FaceDetect::ImageFileScanner> m_nonfaceScanner;
	QSharedPointer<FaceDetect::NeuralNet> m_neuralNet;
	FaceDetect::TrainingImageDatabase *m_trainingDatabase;
	struct ProcessStep {
		ProcessStep(QObject *o, const QByteArray &m): object(o), method(m) {};
		QObject *object;
		QByteArray method;
	};
	QList<ProcessStep> m_steps;
}; /* -----  end of class ConsoleInterface  ----- */

#endif /* end of include guard: CONSOLEINTERFACE_9JAL83V */


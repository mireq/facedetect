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

#include <QSharedPointer>
#include <QTextStream>
#include <QObject>
#include <lapackpp/lavd.h>
#include "libfacedetect/BPNeuralNet.h"
#include "libfacedetect/FaceFileScanner.h"
#include "libfacedetect/ImageFileScanner.h"
#include "libfacedetect/TrainingImageDatabase.h"
class QStringList;

class ConsoleInterface: public QObject
{
Q_OBJECT
public:
	ConsoleInterface(QObject *parent = 0);
	~ConsoleInterface();
	void run();

private slots:
	void scanFaces();
	void onFaceScanningStatusChanged(bool scanning);
	void onNonFaceScanningFinished();
	void onImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image);
	void onImageScanned(const LaVectorDouble &input, const LaVectorDouble &output);
	void onTrainingFinished();
	void updateProgress(double progress);
	void printTrainingEpoch(int epoch, double mse);
	void printTrainingSample(std::size_t sample, int epoch);

private:
	void parseCommandline();
	QString getArgument(const QStringList &arguments, const QString &argumentName);
	void saveNeuralNet();

private:
	QString m_loadNetFile;
	QString m_saveNetFile;

	QString m_facesPath;
	QString m_nonFacesPath;
	QTextStream m_cout;
	QSharedPointer<FaceDetect::FaceFileScanner> m_faceScanner;
	QSharedPointer<FaceDetect::ImageFileScanner> m_nonfaceScanner;
	QSharedPointer<FaceDetect::NeuralNet> m_neuralNet;
	FaceDetect::TrainingImageDatabase *m_trainingDatabase;
}; /* -----  end of class ConsoleInterface  ----- */

#endif /* end of include guard: CONSOLEINTERFACE_9JAL83V */


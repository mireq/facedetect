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
#include "libfacedetect/BPNeuralNet.h"
#include "libfacedetect/FaceFileScanner.h"
#include "libfacedetect/TrainingImageDatabase.h"

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
	void onImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image);
	void onTrainingFinished();
	void updateProgress(double progress);

private:
	QString m_facesPath;
	QTextStream m_cout;
	QSharedPointer<FaceDetect::FaceFileScanner> m_faceScanner;
	QSharedPointer<FaceDetect::NeuralNet> m_neuralNet;
	FaceDetect::TrainingImageDatabase *m_trainingDatabase;
}; /* -----  end of class ConsoleInterface  ----- */

#endif /* end of include guard: CONSOLEINTERFACE_9JAL83V */


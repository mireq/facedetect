/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  20.12.2010 09:54:39
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef QMLWIN_R54R5F4S
#define QMLWIN_R54R5F4S

#include <QDeclarativeView>
#include <QSettings>
#include <QSharedPointer>
#include "core/FaceBrowserModel.h"
#include "libfacedetect/Align.h"
#include "libfacedetect/FaceFileScanner.h"
#include "libfacedetect/ImageFileScanner.h"
#include "libfacedetect/ImageFilter.h"
#include "libfacedetect/NetTrainer.h"
#include "libfacedetect/NeuralNet.h"
#include "libfacedetect/TrainingImageDatabase.h"

class FaceImageProvider;
class FilterImageProvider;

class QmlWin: public QDeclarativeView
{
Q_OBJECT
Q_PROPERTY(QString facesPath READ facesPath WRITE setFacesPath NOTIFY facesPathChanged);
Q_PROPERTY(QString nonFacesPath READ nonFacesPath WRITE setNonFacesPath NOTIFY nonFacesPathChanged);
Q_PROPERTY(QObject *faceBrowserModel READ faceBrowserModel NOTIFY faceBrowserModelChanged);
Q_PROPERTY(QObject *faceFileScanner READ faceFileScanner NOTIFY faceFileScannerChanged);
Q_PROPERTY(QObject *nonFaceFileScanner READ nonFaceFileScanner NOTIFY nonFaceFileScannerChanged);
Q_PROPERTY(QObject *neuralNet READ neuralNet NOTIFY neuralNetChanged);
Q_PROPERTY(QObject *netTrainer READ netTrainer NOTIFY netTrainerChanged);
Q_PROPERTY(QVariant filterSettings READ filterSettings WRITE setFilterSettings NOTIFY filterSettingsChanged);
Q_PROPERTY(QString netType READ netType WRITE setNetType NOTIFY netTypeChanged);
Q_PROPERTY(int trainingSetPercent READ trainingSetPercent WRITE setTrainingSetPercent NOTIFY trainingSetPercentChanged);
Q_PROPERTY(double learningSpeed READ learningSpeed WRITE setLearningSpeed NOTIFY learningSpeedChanged);
public:
	QmlWin(QWidget *parent = 0);
	~QmlWin();
	QString facesPath() const;
	void setFacesPath(const QString &facesPath);
	QString nonFacesPath() const;
	void setNonFacesPath(const QString &nonFacesPath);
	FaceBrowserModel *faceBrowserModel() const;
	FaceDetect::FaceFileScanner *faceFileScanner() const;
	FaceDetect::ImageFileScanner *nonFaceFileScanner() const;
	FaceDetect::NeuralNet *neuralNet() const;
	FaceDetect::NetTrainer *netTrainer() const;
	QVariant filterSettings() const;
	void setFilterSettings(const QVariant &filterSettings);
	QString netType() const;
	void setNetType(const QString &netType);
	int trainingSetPercent() const;
	void setTrainingSetPercent(int percent);
	double learningSpeed() const;
	void setLearningSpeed(double speed);
	Q_INVOKABLE QString encodeFilterString() const;
	Q_INVOKABLE void startTraining();
	Q_INVOKABLE void stop();

signals:
	void facesPathChanged(const QString &url);
	void nonFacesPathChanged(const QString &url);
	void faceBrowserModelChanged(FaceBrowserModel *model);
	void faceFileScannerChanged(FaceDetect::FaceFileScanner *scanner);
	void nonFaceFileScannerChanged(FaceDetect::ImageFileScanner *scanner);
	void filterSettingsChanged(const QVariant &filterSettings);
	void neuralNetChanged(FaceDetect::NeuralNet *net);
	void netTrainerChanged(FaceDetect::NetTrainer *trainer);
	void netTypeChanged(const QString &netType);
	void trainingSetPercentChanged(int percent);
	void learningSpeedChanged(double speed);
	void epochFinished(int epoch, double mseA, double mseE, double mseBinA, double mseBinE, int sizeA, int sizeE);
	void errorCalculated(int sample, double mse);

private slots:
	void imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &img);
	void startNextStep();
	// Záznam skenovaných fotografií
	void onImageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image);
	void onImageScanned(const LaVectorDouble &input, const LaVectorDouble &output);
	void onEpochFinished(const FaceDetect::NetTrainer::EpochStats &stats);
	void onErrorCalculated(std::size_t sample, std::size_t sampleCount, double errorSum);

private:
	bool containsStep(const QObject *object, const QByteArray &name) const;
	void loadSettings();
	void saveSettings();
	void initializeScanner();
	void createFilterSettings();
	void createNeuralNet();
	void createNetTrainer();
	void updateFilters();
	Q_INVOKABLE void scanFaces();
	Q_INVOKABLE void scanNonFaces();
	Q_INVOKABLE void trainNet();

private:
	struct ProcessStep {
		ProcessStep(QObject *o, const QByteArray &m): object(o), method(m) {};
		QObject *object;
		QByteArray method;
	};
	QList<ProcessStep> m_steps;

	QSharedPointer<FaceDetect::Align> m_aligner;
	QSharedPointer<FaceBrowserModel> m_faceBrowserModel;
	QSharedPointer<FaceDetect::FaceFileScanner> m_faceFileScanner;
	QSharedPointer<FaceDetect::ImageFileScanner> m_nonFaceFileScanner;
	QSharedPointer<FaceDetect::NetTrainer> m_trainer;
	FaceDetect::NeuralNet *m_neuralNet;
	QMap<QString,QSharedPointer<FaceDetect::NeuralNet>> m_neuralNets;
	FaceDetect::TrainingImageDatabase *m_trainingDatabase;
	FaceImageProvider *m_imageProvider;
	FilterImageProvider *m_filterImageProvider;
	FaceDetect::ImageFilter m_filter;
	FaceDetect::ImageFilter m_localFilter;
	FaceDetect::ImageFilter m_globalFilter;
	QString m_facesPath;
	QString m_nonFacesPath;
	QString m_faceCachePath;
	QVariant m_filterSettings;
	QSettings m_settings;
	QString m_netType;
	int m_trainingSetPercent;
	double m_learningSpeed;
}; /* -----  end of class QmlWin  ----- */

#endif /* end of include guard: QMLWIN_R54R5F4S */


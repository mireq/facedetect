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
Q_PROPERTY(QVariant filterSettings READ filterSettings WRITE setFilterSettings NOTIFY filterSettingsChanged);
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
	QVariant filterSettings() const;
	void setFilterSettings(const QVariant &filterSettings);
	Q_INVOKABLE QString encodeFilterString() const;
	Q_INVOKABLE void startTraining();

signals:
	void facesPathChanged(const QString &url);
	void nonFacesPathChanged(const QString &url);
	void faceBrowserModelChanged(FaceBrowserModel *model);
	void faceFileScannerChanged(FaceDetect::FaceFileScanner *scanner);
	void nonFaceFileScannerChanged(FaceDetect::ImageFileScanner *scanner);
	void filterSettingsChanged(const QVariant &filterSettings);

private slots:
	void imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &img);
	void startNextStep();

private:
	void loadSettings();
	void saveSettings();
	void initializeScanner();
	void createFilterSettings();
	Q_INVOKABLE void scanFaces();
	Q_INVOKABLE void scanNonFaces();

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
	FaceDetect::TrainingImageDatabase *m_trainingDatabase;
	FaceImageProvider *m_imageProvider;
	FilterImageProvider *m_filterImageProvider;
	FaceDetect::ImageFilter m_filter;
	QString m_facesPath;
	QString m_nonFacesPath;
	QVariant m_filterSettings;
	QSettings m_settings;
}; /* -----  end of class QmlWin  ----- */

#endif /* end of include guard: QMLWIN_R54R5F4S */


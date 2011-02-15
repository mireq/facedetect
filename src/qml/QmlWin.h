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
#include <QUrl>
#include "core/FaceBrowserModel.h"
#include "libfacedetect/Align.h"
#include "libfacedetect/FaceFileScanner.h"

class QmlWin: public QDeclarativeView
{
Q_OBJECT
Q_PROPERTY(QString scanPath READ scanPath WRITE setScanPath NOTIFY scanPathChanged);
Q_PROPERTY(QObject *faceBrowserModel READ faceBrowserModel NOTIFY faceBrowserModelChanged);
Q_PROPERTY(QObject *faceFileScanner READ faceFileScanner NOTIFY faceFileScannerChanged);
public:
	QmlWin(QWidget *parent = 0);
	~QmlWin();
	QString scanPath() const;
	void setScanPath(const QString &scanPath);
	FaceBrowserModel *faceBrowserModel() const;
	FaceDetect::FaceFileScanner *faceFileScanner() const;

signals:
	void scanPathChanged(const QUrl &url);
	void faceBrowserModelChanged(FaceBrowserModel *model);
	void faceFileScannerChanged(FaceDetect::FaceFileScanner *scanner);

private slots:
	void imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &img);

private:
	FaceBrowserModel *m_faceBrowserModel;
	FaceDetect::FaceFileScanner *m_faceFileScanner;
	FaceDetect::Align *m_aligner;
	QString m_scanPath;
}; /* -----  end of class QmlWin  ----- */

#endif /* end of include guard: QMLWIN_R54R5F4S */


/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 14:38:23
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEFILESCANNER_YXFS5ITD
#define FACEFILESCANNER_YXFS5ITD

#include <QImage>
#include <QMetaType>
#include <QPoint>
#include <QVector>
#include <QUrl>
#include "core/FileScanner.h"

namespace FaceDetect {

class FaceFileScanner: public FileScanner
{
Q_OBJECT
public:
	struct FaceData {
		QPoint leftEye;
		QPoint rightEye;
		QPoint nose;
		QPoint mouth;
	};

	typedef QVector<FaceData>::const_iterator FaceDataIterator;

	class ImageInfo
	{
	public:
		ImageInfo();
		~ImageInfo();
		bool isValid() const;
		QUrl url() const;
		QUrl definitionUrl() const;
		FaceDataIterator faceBegin() const;
		FaceDataIterator faceEnd() const;
		QImage getImage() const;
		QVector<ImageInfo> splitFaces() const;

	private:
		void setUrl(const QUrl &url);
		void setDefinitionUrl(const QUrl &url);
		void setFaceData(const QVector<FaceData> &faceData);

	private:
		bool m_valid;
		QUrl m_url;
		QUrl m_definitionUrl;
		QVector<FaceData> m_faceData;
		friend class FaceFileScanner;
	}; /* -----  end of class ImageInfo  ----- */

	explicit FaceFileScanner(QObject *parent = 0);
	~FaceFileScanner();
	QUrl basePath();
	void setBasePath(const QUrl &url);
	virtual void scanFile(const QString &fileName);
	ImageInfo readFile(const QString &fileName);

signals:
	void imageScanned(const FaceDetect::FaceFileScanner::ImageInfo &image);

private:
	enum ReadState {
		NoState,
		RecordingsState,
		RecordingState,
		URLState,
		SubjectState,
		ApplicationState,
		FaceState
	};
	QUrl m_basePath;
}; /* -----  end of class FaceFileScanner  ----- */

} /* end of namespace FaceDetect */

Q_DECLARE_METATYPE(FaceDetect::FaceFileScanner::ImageInfo);

#endif /* end of include guard: FACEFILESCANNER_YXFS5ITD */


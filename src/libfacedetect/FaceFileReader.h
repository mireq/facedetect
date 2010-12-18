/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 00:35:09
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEFILEREADER_ZDL9FS1Y
#define FACEFILEREADER_ZDL9FS1Y

#include <QString>
#include <QImage>
#include <QUrl>

namespace FaceDetect {

class FaceFileReader
{
public:
	struct FaceData {
		QPoint leftEye;
		QPoint rightEye;
		QPoint nose;
		QPoint mouth;
	};
	FaceFileReader();
	~FaceFileReader();
	void setSearchPrefix(const QString &prefix);
	bool readFile(const QString &fileName);
	QString definitionFile() const;
	QImage readImage() const;
	QUrl imageUrl() const;
	QVector<FaceFileReader::FaceData> faceData() const;
	static void setDataDir(const QString &dataDir);

private:
	QImage readImage(const QString &fileName, const QLatin1String &format) const;
	enum ReadState {
		NoState,
		RecordingsState,
		RecordingState,
		URLState,
		SubjectState,
		ApplicationState,
		FaceState
	};
	QString m_definitionFile;
	QString m_prefix;
	QString m_url;
	QLatin1String m_format;
	QVector<FaceData> m_faceData;
	static QString sm_dataDir;
}; /* -----  end of class FaceFileReader  ----- */

} /* end of namespace FaceDetect */

#endif /* end of include guard: FACEFILEREADER_ZDL9FS1Y */


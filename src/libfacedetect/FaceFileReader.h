/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 00:35:09
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

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
	static QImage readImage(const QString &fileName, const QLatin1String &format);

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
	QString m_definitionFile;
	QString m_prefix;
	QString m_url;
	QLatin1String m_format;
	QVector<FaceData> m_faceData;
}; /* -----  end of class FaceFileReader  ----- */

} /* end of namespace FaceDetect */

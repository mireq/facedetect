/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 00:35:07
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <cstdio>
#include <bzlib.h>
#include "FaceFileReader.h"

using std::FILE;
using std::fopen;
using std::fclose;

namespace FaceDetect {

QString FaceFileReader::sm_dataDir;

FaceFileReader::FaceFileReader():
	m_prefix(sm_dataDir),
	m_format("PPM")
{
}

FaceFileReader::~FaceFileReader()
{
}

void FaceFileReader::setSearchPrefix(const QString &prefix)
{
	m_prefix = prefix;
}

bool FaceFileReader::readFile(const QString &fileName)
{
	m_definitionFile = fileName;
	QFile definitionFile(fileName);
	if (!definitionFile.open(QIODevice::ReadOnly)) {
		return false;
	}

	ReadState state = NoState;
	QXmlStreamReader definitionReader(&definitionFile);
	QString textAcumulator;
	FaceData faceData;
	while (!definitionReader.atEnd()) {
		definitionReader.readNext();
		switch (definitionReader.tokenType()) {
			case QXmlStreamReader::NoToken:
				break;
			case QXmlStreamReader::Invalid:
				qWarning() << "Invalid token";
				break;
			case QXmlStreamReader::StartDocument:
				if (definitionReader.documentVersion() != "1.0") {
					qWarning() << "Bad document version" << definitionReader.documentVersion();
				}
				break;
			case QXmlStreamReader::EndDocument:
				break;
			case QXmlStreamReader::StartElement:
				switch (state) {
					case NoState:
						if (definitionReader.qualifiedName() == "Recordings") {
							state = RecordingsState;
						}
						break;
					case RecordingsState:
						if (definitionReader.qualifiedName() == "Recording") {
							state = RecordingState;
						}
						break;
					case RecordingState:
						if (definitionReader.qualifiedName() == "URL") {
							state = URLState;
						}
						else if (definitionReader.qualifiedName() == "Subject") {
							state = SubjectState;
						}
						if (definitionReader.attributes().hasAttribute("relative")) {
							m_url = definitionReader.attributes().value("relative").toString();
						}
						break;
					case SubjectState:
						if (definitionReader.qualifiedName() == "Application") {
							state = ApplicationState;
						}
						break;
					case ApplicationState:
						if (definitionReader.qualifiedName() == "Face") {
							state = FaceState;
						}
						break;
					case FaceState:
						if (definitionReader.attributes().hasAttribute("x") && definitionReader.attributes().hasAttribute("y")) {
							QString x = definitionReader.attributes().value("x").toString();
							QString y = definitionReader.attributes().value("y").toString();
							if (definitionReader.qualifiedName() == "LeftEye") {
								faceData.leftEye.setX(x.toInt());
								faceData.leftEye.setY(y.toInt());
							}
							if (definitionReader.qualifiedName() == "RightEye") {
								faceData.rightEye.setX(x.toInt());
								faceData.rightEye.setY(y.toInt());
							}
							if (definitionReader.qualifiedName() == "Nose") {
								faceData.nose.setX(x.toInt());
								faceData.nose.setY(y.toInt());
							}
							if (definitionReader.qualifiedName() == "Mouth") {
								faceData.mouth.setX(x.toInt());
								faceData.mouth.setY(y.toInt());
							}
						}
						break;
					default:
						break;
				}
				textAcumulator.clear();
				break;
			case QXmlStreamReader::EndElement:
				switch (state) {
					case FaceState:
						if (definitionReader.qualifiedName() == "Face") {
							state = ApplicationState;
							m_faceData << faceData;
							faceData.leftEye = QPoint();
							faceData.rightEye = QPoint();
						}
						break;
					case ApplicationState:
						if (definitionReader.qualifiedName() == "Application") {
							state = SubjectState;
						}
						break;
					case SubjectState:
						if (definitionReader.qualifiedName() == "Subject") {
							state = RecordingState;
						}
						break;
					case URLState:
						if (definitionReader.qualifiedName() == "URL") {
							state = RecordingState;
						}
						break;
					case RecordingState:
						if (definitionReader.qualifiedName() == "Recording") {
							state = RecordingsState;
						}
						break;
					case RecordingsState:
						if (definitionReader.qualifiedName() == "Recordings") {
							state = NoState;
						}
						break;
					default:
						break;
				}
				textAcumulator.clear();
				break;
			case QXmlStreamReader::Characters:
				if (!definitionReader.isWhitespace()) {
					textAcumulator += definitionReader.text();
				}
				break;
			case QXmlStreamReader::Comment:
				break;
			case QXmlStreamReader::DTD:
				break;
			case QXmlStreamReader::EntityReference:
				break;
			case QXmlStreamReader::ProcessingInstruction:
				break;
			default:
				break;
		}
	}
	return true;
}

QString FaceFileReader::definitionFile() const
{
	return m_definitionFile;
}

QImage FaceFileReader::readImage() const
{
	return readImage(m_prefix + m_url, m_format);
}

QImage FaceFileReader::readImage(const QString &fileName, const QLatin1String &format) const
{
	QImage ret;

	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() == "bz2") {
		fileInfo.setFile(fileName.left(fileName.size() - 8) + ".png");
		if (fileInfo.exists()) {
			if (ret.load(fileInfo.absoluteFilePath(), "PNG")) {
				return ret;
			}
		}
		fileInfo.setFile(fileName.left(fileName.size() - 4));
		if (fileInfo.exists()) {
			if (ret.load(fileInfo.absoluteFilePath(), format.latin1())) {
				return ret;
			}
		}
	}

	FILE *inputFile;
	inputFile = fopen(fileName.toUtf8().constData(), "r");
	if (!inputFile) {
		qWarning() << QString("Could not open file: %1").arg(fileName);
		return ret;
	}

	int bzError;
	BZFILE *bzFile;
	bzFile = BZ2_bzReadOpen(&bzError, inputFile, 0, 0, NULL, 0);
	if (bzError != BZ_OK) {
		BZ2_bzReadClose(&bzError, bzFile);
		fclose(inputFile);
		return ret;
	}

	bzError = BZ_OK;
	int nBuf;
	char buf[65536];
	QByteArray imageData;
	while (bzError == BZ_OK) {
		nBuf = BZ2_bzRead(&bzError, bzFile, buf, sizeof(buf));
		imageData.append(buf, nBuf);
	}
	fclose(inputFile);
	if (bzError != BZ_STREAM_END) {
		qWarning() << QString("Could not read file: %1").arg(fileName);
	}
	else {
		if (!ret.loadFromData(imageData, format.latin1())) {
			qWarning() << QString("Bad image format of file: %1").arg(fileName);
		}
	}
	BZ2_bzReadClose(&bzError, bzFile);

	return ret;
}

QVector<FaceFileReader::FaceData> FaceFileReader::faceData() const
{
	return m_faceData;
}

void FaceFileReader::setDataDir(const QString &dataDir)
{
	sm_dataDir = dataDir;
}

QUrl FaceFileReader::imageUrl() const
{
	return QUrl("image://faceimage/" + m_definitionFile);
}

} /* end of namespace FaceDetect */


/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 14:38:20
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDebug>
#include <QFileInfo>
#include <QStringList>
#include <QXmlStreamReader>
#include <bzlib.h>
#include "FaceFileScanner.h"

using std::FILE;
using std::fopen;
using std::fclose;

namespace FaceDetect {

FaceFileScanner::ImageInfo::ImageInfo():
	m_valid(false)
{
}

FaceFileScanner::ImageInfo::~ImageInfo()
{
}

bool FaceFileScanner::ImageInfo::isValid() const
{
	return m_valid;
}

QUrl FaceFileScanner::ImageInfo::url() const
{
	return m_url;
}

QUrl FaceFileScanner::ImageInfo::definitionUrl() const
{
	return m_definitionUrl;
}

FaceFileScanner::FaceDataIterator FaceFileScanner::ImageInfo::faceBegin() const
{
	return m_faceData.begin();
}

FaceFileScanner::FaceDataIterator FaceFileScanner::ImageInfo::faceEnd() const
{
	return m_faceData.end();
}

QImage FaceFileScanner::ImageInfo::getImage() const
{
	const char *format = "PNG";
	QImage ret;
	QString fileName = m_url.toLocalFile();
	QFileInfo fileInfo(fileName);

	if (fileInfo.suffix() == "png") {
		if (ret.load(fileInfo.absoluteFilePath(), format)) {
			return ret;
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
		if (!ret.loadFromData(imageData, "PPM")) {
			qWarning() << QString("Bad image format of file: %1").arg(fileName);
		}
	}
	BZ2_bzReadClose(&bzError, bzFile);

	return ret;
}

QVector<FaceFileScanner::ImageInfo> FaceFileScanner::ImageInfo::splitFaces() const
{
	QVector<ImageInfo> ret;
	ImageInfo workingCopy = *this;
	foreach (const FaceData &faceData, m_faceData) {
		workingCopy.m_faceData.clear();
		workingCopy.m_faceData.append(faceData);
		ret.append(workingCopy);
	}
	return ret;
}

void FaceFileScanner::ImageInfo::setUrl(const QUrl &url)
{
	if (m_url == url) {
		return;
	}
	m_url = url;
	QFileInfo fileInfo(m_url.toLocalFile());
	m_valid = fileInfo.isReadable();
}

void FaceFileScanner::ImageInfo::setDefinitionUrl(const QUrl &url)
{
	m_definitionUrl = url;
}

void FaceFileScanner::ImageInfo::setFaceData(const QVector<FaceData> &faceData)
{
	m_faceData = faceData;
}

FaceFileScanner::FaceFileScanner(QObject *parent):
	FileScanner(parent)
{
}

FaceFileScanner::~FaceFileScanner()
{
}

QUrl FaceFileScanner::basePath()
{
	return m_basePath;
}

void FaceFileScanner::setBasePath(const QUrl &url)
{
	m_basePath = url;
	QStringList path = url.toLocalFile().split("/");
	if (path.size() > 0 && path.last() == "") {
		path.takeLast();
	}
	setScanPath(path.join("/") + "/data/ground_truths/xml/");
}

void FaceFileScanner::scanFile(const QString &fileName)
{
	// Zaujímajú ma len xml súbory
	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() != QLatin1String("xml")) {
		return;
	}

	ImageInfo img = readFile(fileName);
	if (!img.isValid()) {
		return;
	}
	emit imageScanned(img);
}

FaceFileScanner::ImageInfo FaceFileScanner::readFile(const QString &fileName)
{
	ImageInfo ret;
	ret.setDefinitionUrl(QUrl::fromLocalFile(fileName));
	QFile definitionFile(fileName);
	QFileInfo definitionFileInfo(fileName);
	if (!definitionFile.open(QIODevice::ReadOnly)) {
		return ret;
	}
	QVector<FaceData> data;
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
							QFileInfo inf(basePath().toLocalFile() + "/" + definitionReader.attributes().value("relative").toString());
							if (inf.isReadable()) {
								ret.setUrl(inf.absoluteFilePath());
							}
							inf.setFile(inf.absolutePath() + "/" + inf.baseName() + ".png");
							ret.setUrl(inf.absoluteFilePath());
							//ret.setUrl(basePath().toLocalFile() + "/" + definitionReader.attributes().value("relative").toString());
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
							data << faceData;
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
	ret.setFaceData(data);
	return ret;
}

} /* end of namespace FaceDetect */


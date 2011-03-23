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

/**
 * Metóda vráti \e true, ak je URL obrázku čitateľné.
 * \sa url()
 */
bool FaceFileScanner::ImageInfo::isValid() const
{
	return m_valid;
}

/**
 * Vráti URL obrázku.
 */
QUrl FaceFileScanner::ImageInfo::url() const
{
	return m_url;
}

/**
 * Vráti URL XML súboru s definovanými informáciami o obrázku.
 */
QUrl FaceFileScanner::ImageInfo::definitionUrl() const
{
	return m_definitionUrl;
}

/**
 * Vráti iterátor pre začiatok zoznamu tvárí.
 */
FaceFileScanner::FaceDataIterator FaceFileScanner::ImageInfo::faceBegin() const
{
	return m_faceData.begin();
}

/**
 * Vráti iterátor pre koniec zoznamu tvárí.
 */
FaceFileScanner::FaceDataIterator FaceFileScanner::ImageInfo::faceEnd() const
{
	return m_faceData.end();
}

/**
 * Vráti obrázok, ktorý sa nachádza na adrese url(). Potporované formáty sú PNG
 * a PPM komprimované algoritmom BZ2.
 */
QImage FaceFileScanner::ImageInfo::getImage() const
{
	// Formát obrázku, kotrý hľadáme je PNG
	const char *format = "PNG";
	QImage ret;
	QString fileName = m_url.toLocalFile();
	QFileInfo fileInfo(fileName);

	// Ak je prípona "png" priamo otvoríme obrázok.
	if (fileInfo.suffix() == "png") {
		if (ret.load(fileInfo.absoluteFilePath(), format)) {
			return ret;
		}
	}

	// Inak sa obrázok otvorí priamo ako súbor
	FILE *inputFile;
	inputFile = fopen(fileName.toUtf8().constData(), "r");
	if (!inputFile) {
		qWarning() << QString("Could not open file: %1").arg(fileName);
		return ret;
	}

	// Predpokladá sa, že súbor je komprimovaný algoritmom BZ2
	// Otvorenie archívu BZ2
	int bzError;
	BZFILE *bzFile;
	bzFile = BZ2_bzReadOpen(&bzError, inputFile, 0, 0, NULL, 0);
	if (bzError != BZ_OK) {
		BZ2_bzReadClose(&bzError, bzFile);
		fclose(inputFile);
		return ret;
	}

	// Čítanie dát archívu
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

/**
 * Vytvorenie samostatného objektu FaceFileScanner::ImageInfo pre každú tvár,
 * ktorá sa nachádza na fotografií.
 */
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

/**
 * Nastavenie URL súboru z obrázkom.
 */
void FaceFileScanner::ImageInfo::setUrl(const QUrl &url)
{
	if (m_url == url) {
		return;
	}
	m_url = url;
	// Kontrola čitateľnosti súboru
	QFileInfo fileInfo(m_url.toLocalFile());
	m_valid = fileInfo.isReadable();
}

/**
 * Nastavenie URL s definíciou informácii o fotografii.
 */
void FaceFileScanner::ImageInfo::setDefinitionUrl(const QUrl &url)
{
	m_definitionUrl = url;
}

/**
 * Nastavenie dát tvárí.
 */
void FaceFileScanner::ImageInfo::setFaceData(const QVector<FaceData> &faceData)
{
	m_faceData = faceData;
}

/**
 * \econstruct
 */
FaceFileScanner::FaceFileScanner(QObject *parent):
	FileScanner(parent)
{
}

FaceFileScanner::~FaceFileScanner()
{
}

/**
 * Vráti cestu k základnému adresáru.
 * \sa setBasePath()
 */
QUrl FaceFileScanner::basePath()
{
	return m_basePath;
}

/**
 * Nastavenie základného adresára, v ktorom sa nachádza databáza tvárí. Samotné
 * definičné súbory sa hľadajú v podadresári "data/ground_truths/xml/".
 * \sa basePath()
 */
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

/**
 * Načítanie informácií o definičnom súbore \a fileName.
 */
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
	FaceData faceData;

	// Pomocná premenná na akumuláciu textu počas čítania XML
	QString textAcumulator;
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


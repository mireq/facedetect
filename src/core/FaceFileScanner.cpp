/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  19.12.2010 14:38:20
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QFileInfo>
#include "libfacedetect/FaceFileReader.h"
#include "FaceFileScanner.h"

using FaceDetect::FaceFileReader;

FaceFileScanner::FaceFileScanner(QObject *parent):
	FileScanner(parent)
{
}

FaceFileScanner::~FaceFileScanner()
{
}

void FaceFileScanner::scanFile(const QString &fileName)
{
	// Zaujímajú ma len xml súbory
	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() != QLatin1String("xml")) {
		return;
	}

	FaceFileReader reader;
	if (!reader.readFile(fileName)) {
		return;
	}

	// Akceptujeme len súbory, na ktorých je jediná tvár
	if (reader.faceData().count() != 1) {
		return;
	}

	// Tvár musí mať definované všetky kontrolné body
	FaceFileReader::FaceData face = reader.faceData()[0];
	if (face.leftEye == QPoint() || face.rightEye == QPoint() || face.nose == QPoint() || face.mouth == QPoint()) {
		return;
	}

	emit fileScanned(fileName);
}


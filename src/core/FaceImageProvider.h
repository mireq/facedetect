/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  07.12.2010 09:28:34
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FACEIMAGEPROVIDER_N9V13OYL
#define FACEIMAGEPROVIDER_N9V13OYL

#include <QDeclarativeImageProvider>
#include <QPointer>
#include "libfacedetect/Align.h"
#include "libfacedetect/FaceFileScanner.h"

class FaceImageProvider: public QDeclarativeImageProvider
{
public:
	FaceImageProvider();
	~FaceImageProvider();
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
	void bindScanner(FaceDetect::FaceFileScanner *scanner);
	void bindAlign(FaceDetect::Align *align);

private:
	QPointer<FaceDetect::FaceFileScanner> m_scanner;
	QPointer<FaceDetect::Align> m_align;
}; /* -----  end of class FaceImageProvider  ----- */

#endif /* end of include guard: FACEIMAGEPROVIDER_N9V13OYL */


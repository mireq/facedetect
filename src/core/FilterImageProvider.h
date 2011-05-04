/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  04.05.2011 16:10:04
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef FILTERIMAGEPROVIDER_JB2C7OLM
#define FILTERIMAGEPROVIDER_JB2C7OLM

#include <QDeclarativeImageProvider>
#include "libfacedetect/ImageFilter.h"

class FilterImageProvider: public QDeclarativeImageProvider
{
public:
	FilterImageProvider();
	~FilterImageProvider();
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
	FaceDetect::ImageFilter m_filter;
}; /* -----  end of class FilterImageProvider  ----- */

#endif /* end of include guard: FILTERIMAGEPROVIDER_JB2C7OLM */


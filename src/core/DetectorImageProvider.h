/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  12.05.2011 06:49:57
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef DETECTORIMAGEPROVIDER_3T5HJ9TP
#define DETECTORIMAGEPROVIDER_3T5HJ9TP

#include <QDeclarativeImageProvider>
class QmlWin;

class DetectorImageProvider: public QDeclarativeImageProvider
{
public:
	DetectorImageProvider(QmlWin *parent);
	~DetectorImageProvider();
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
	QmlWin *m_parent;
}; /* -----  end of class DetectorImageProvider  ----- */

#endif /* end of include guard: DETECTORIMAGEPROVIDER_3T5HJ9TP */


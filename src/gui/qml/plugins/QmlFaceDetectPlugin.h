/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 11:45:36
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef QMLFACEDETECTPLUGIN_O2BT9V1O
#define QMLFACEDETECTPLUGIN_O2BT9V1O

#include <QDeclarativeExtensionPlugin>

class QmlFaceDetectPlugin: public QDeclarativeExtensionPlugin
{
Q_OBJECT
public:
	QmlFaceDetectPlugin(QObject *parent = 0);
	~QmlFaceDetectPlugin();
	void registerTypes(const char *uri);
}; /* -----  end of class QmlFaceDetectPlugin  ----- */

#endif /* end of include guard: QMLFACEDETECTPLUGIN_O2BT9V1O */


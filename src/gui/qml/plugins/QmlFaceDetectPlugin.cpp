/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 11:45:34
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QtDeclarative>
#include "QmlQwtPlotWidget.h"
#include "QmlFaceDetectPlugin.h"

QmlFaceDetectPlugin::QmlFaceDetectPlugin(QObject *parent):
	QDeclarativeExtensionPlugin(parent)
{
}

QmlFaceDetectPlugin::~QmlFaceDetectPlugin()
{
}

void QmlFaceDetectPlugin::registerTypes(const char *uri)
{
	qmlRegisterType<QmlQwtPlotWidget>(uri, 1, 0, "PlotWidget");
}

#ifdef qmlfacedetectplugin_EXPORTS
Q_EXPORT_PLUGIN2(qmlfacedetectplugin, QmlFaceDetectPlugin);
#endif /* qmlfacedetectplugin_EXPORTS */


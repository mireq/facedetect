/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 12:44:00
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef QMLQWTPLOTWIDGET_G5DFJVX5
#define QMLQWTPLOTWIDGET_G5DFJVX5

#include <QGraphicsProxyWidget>
#include <qwt_plot.h>

class QmlQwtPlotWidget: public QGraphicsProxyWidget
{
Q_OBJECT
public:
	QmlQwtPlotWidget(QGraphicsItem *parent = 0);
	~QmlQwtPlotWidget();

private:
}; /* -----  end of class QmlQwtPlotWidget  ----- */

#endif /* end of include guard: QMLQWTPLOTWIDGET_G5DFJVX5 */


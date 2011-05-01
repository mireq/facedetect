/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 12:43:56
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <qwt_plot.h>
#include "QmlQwtPlotWidget.h"

QmlQwtPlotWidget::QmlQwtPlotWidget(QGraphicsItem *parent):
	QGraphicsProxyWidget(parent)
{
	QwtPlot *plot = new QwtPlot();
	setWidget(plot);
}

QmlQwtPlotWidget::~QmlQwtPlotWidget()
{
}


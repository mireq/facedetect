/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 12:43:56
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <QWidget>
#include "QmlQwtPlotWidget.h"

QmlQwtPlotWidget::QmlQwtPlotWidget(QGraphicsItem *parent):
	QGraphicsProxyWidget(parent)
{
	QwtPlot *plot = new QwtPlot();
	// Nastavenie priehľadnosti
	plot->setAttribute(Qt::WA_TranslucentBackground);
	/*
	plot->canvas()->setFrameShape(QFrame::NoFrame);
	plot->setCanvasBackground(Qt::transparent);
	plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, false);
	plot->canvas()->setAutoFillBackground(false);
	*/
	plot->setCanvasBackground(Qt::white);
	setWidget(plot);
}

QmlQwtPlotWidget::~QmlQwtPlotWidget()
{
}


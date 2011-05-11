/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 12:43:56
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include "QmlQwtPlotCurve.h"
#include "QmlQwtPlotWidget.h"

QmlQwtPlotWidget::QmlQwtPlotWidget(QGraphicsItem *parent):
	QGraphicsProxyWidget(parent),
	m_updateRequired(false)
{
	m_plot = new QwtPlot();

	m_updateTimer = new QTimer(this);
	m_updateTimer->setSingleShot(true);
	connect(m_updateTimer, SIGNAL(timeout()), SLOT(onUpdateTimeout()));

	// Nastavenie priehľadnosti
	m_plot->setAttribute(Qt::WA_TranslucentBackground);
	/*
	plot->canvas()->setFrameShape(QFrame::NoFrame);
	plot->setCanvasBackground(Qt::transparent);
	plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, false);
	plot->canvas()->setAutoFillBackground(false);
	*/
	m_plot->setCanvasBackground(Qt::white);
	setWidget(m_plot);
}

QmlQwtPlotWidget::~QmlQwtPlotWidget()
{
	foreach (QmlQwtPlotCurve *curve, m_curves) {
		curve->curve()->detach();
	}
}

QString QmlQwtPlotWidget::title() const
{
	return m_plot->title().text();
}

void QmlQwtPlotWidget::setTitle(const QString &title)
{
	if (m_plot->title().text() != title) {
		m_plot->setTitle(title);
		emit titleChanged(m_plot->title().text());
	}
}

void QmlQwtPlotWidget::setAxisScale(Axis axis, double min, double max, double stepSize)
{
	m_plot->setAxisScale(qmlAxisToPlot(axis), min, max, stepSize);
}

QDeclarativeListProperty<QmlQwtPlotCurve> QmlQwtPlotWidget::curves()
{
	return QDeclarativeListProperty<QmlQwtPlotCurve>(this, 0, curveAppend, curveCount, curveAt, curveClear);
}

void QmlQwtPlotWidget::curveAppend(QDeclarativeListProperty<QmlQwtPlotCurve> *property, QmlQwtPlotCurve *value)
{
	QmlQwtPlotWidget *w(static_cast<QmlQwtPlotWidget *>(property->object));
	value->curve()->attach(w->m_plot);
	connect(value, SIGNAL(dataChanged()), w, SLOT(update()));
	w->m_curves.append(value);
}

int QmlQwtPlotWidget::curveCount(QDeclarativeListProperty<QmlQwtPlotCurve> *property)
{
	QmlQwtPlotWidget *w(static_cast<QmlQwtPlotWidget *>(property->object));
	return w->m_curves.count();
}

QmlQwtPlotCurve *QmlQwtPlotWidget::curveAt(QDeclarativeListProperty<QmlQwtPlotCurve> *property, int index)
{
	QmlQwtPlotWidget *w(static_cast<QmlQwtPlotWidget *>(property->object));
	return w->m_curves.at(index);
}

void QmlQwtPlotWidget::curveClear(QDeclarativeListProperty<QmlQwtPlotCurve> *property)
{
	QmlQwtPlotWidget *w(static_cast<QmlQwtPlotWidget *>(property->object));
	foreach (QmlQwtPlotCurve *curve, w->m_curves) {
		disconnect(curve, SIGNAL(dataChanged()), w, SLOT(update()));
		curve->curve()->detach();
	}
	w->m_curves.clear();
}

QwtPlot::Axis QmlQwtPlotWidget::qmlAxisToPlot(QmlQwtPlotWidget::Axis axis)
{
	switch (axis) {
		case YLeft:
			return QwtPlot::yLeft;
		case YRight:
			return QwtPlot::yRight;
		case XBottom:
			return QwtPlot::xBottom;
		case XTop:
			return QwtPlot::xTop;
		default:
			return QwtPlot::yLeft;
	}
}

QmlQwtPlotWidget::Axis QmlQwtPlotWidget::plotAxisToQml(QwtPlot::Axis axis)
{
	switch (axis) {
		case QwtPlot::yLeft:
			return YLeft;
		case QwtPlot::yRight:
			return YRight;
		case QwtPlot::xBottom:
			return XBottom;
		case QwtPlot::xTop:
			return XTop;
		default:
			return YLeft;
	}
}

void QmlQwtPlotWidget::update()
{
	if (m_updateTimer->isActive()) {
		m_updateRequired = true;
	}
	else {
		m_plot->replot();
		m_updateTimer->start(100);
	}
}

void QmlQwtPlotWidget::onUpdateTimeout()
{
	if (m_updateRequired) {
		m_updateRequired = false;
		m_plot->replot();
	}
}


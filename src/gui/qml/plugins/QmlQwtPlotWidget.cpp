/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  30.04.2011 12:43:56
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <qwt_legend.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include "QmlQwtPlotCurve.h"
#include "QmlQwtPlotWidget.h"

QmlQwtPlotWidget::QmlQwtPlotWidget(QGraphicsItem *parent):
	QGraphicsProxyWidget(parent),
	m_updateRequired(false)
{
	m_scaleEngine.resize(4);
	for (int scale = 0; scale < 4; ++scale) {
		m_scaleEngine[scale] = LinearScaleEngine;
	}

	m_plot = new QwtPlot();

	// Zmena palety
	QPalette pal = m_plot->palette();
	pal.setColor(QPalette::Window, Qt::white);
	m_plot->setPalette(pal);

	// Vytvorenie legendy
	m_legend = new QwtLegend();
	m_plot->insertLegend(m_legend, QwtPlot::RightLegend);

	// Časovač aktualizácie
	m_updateTimer = new QTimer(this);
	m_updateTimer->setSingleShot(true);
	connect(m_updateTimer, SIGNAL(timeout()), SLOT(onUpdateTimeout()));

	// Nastavenie priehľadnosti
	m_plot->setAttribute(Qt::WA_TranslucentBackground);
	m_plot->canvas()->setFrameStyle(QFrame::Plain | QFrame::StyledPanel);
	/*
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

QmlQwtPlotWidget::AxisScaleEngine QmlQwtPlotWidget::axisScaleEngine(QmlQwtPlotWidget::Axis axis) const
{
	return m_scaleEngine[axis];
}

void QmlQwtPlotWidget::setAxisScaleEngine(QmlQwtPlotWidget::Axis axis, QmlQwtPlotWidget::AxisScaleEngine newScaleEngine)
{
	if (axisScaleEngine(axis) != newScaleEngine) {
		m_scaleEngine[axis] = newScaleEngine;
		QwtPlot::Axis plotAxis = qmlAxisToPlot(axis);
		switch (newScaleEngine) {
			case LinearScaleEngine:
				m_plot->setAxisScaleEngine(plotAxis, new QwtLinearScaleEngine());
				break;
			case Log10ScaleEngine:
				m_plot->setAxisScaleEngine(plotAxis, new QwtLog10ScaleEngine());
				break;
		}
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
	w->m_curves.append(value);
	if (!value->legendVisible()) {
		w->onLegendVisibleChanged();
	}
	connect(value, SIGNAL(dataChanged()), w, SLOT(update()));
	connect(value, SIGNAL(legendVisibleChanged(bool)), w, SLOT(onLegendVisibleChanged()));
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
		m_updateTimer->start(250);
	}
}

void QmlQwtPlotWidget::onUpdateTimeout()
{
	if (m_updateRequired) {
		m_updateRequired = false;
		m_plot->replot();
	}
}

void QmlQwtPlotWidget::onLegendVisibleChanged()
{
	foreach (QmlQwtPlotCurve *plotCurve, m_curves) {
		QWidget *legendItem = m_legend->find(plotCurve->curve());
		legendItem->setVisible(plotCurve->legendVisible());
	}
}


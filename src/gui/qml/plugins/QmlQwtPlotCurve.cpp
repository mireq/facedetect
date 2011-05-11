/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  10.05.2011 13:22:09
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#include <QDebug>
#include <qwt_plot_curve.h>
#include "QmlQwtPlotCurve.h"

QmlQwtPlotCurve::QmlQwtPlotCurve(QObject *parent):
	QObject(parent)
{
	m_curve = QSharedPointer<QwtPlotCurve>(new QwtPlotCurve());
	m_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

QmlQwtPlotCurve::~QmlQwtPlotCurve()
{
}

QColor QmlQwtPlotCurve::color() const
{
	return m_curve->pen().color();
}

void QmlQwtPlotCurve::setColor(const QColor &color)
{
	if (m_curve->pen().color() != color) {
		m_curve->setPen(color);
		emit colorChanged(color);
		emit dataChanged();
	}
}

QmlQwtPlotWidget::Axis QmlQwtPlotCurve::xAxis() const
{
	return QmlQwtPlotWidget::plotAxisToQml(static_cast<QwtPlot::Axis>(m_curve->xAxis()));
}

void QmlQwtPlotCurve::setXAxis(QmlQwtPlotWidget::Axis axis)
{
	if (xAxis() != axis) {
		m_curve->setXAxis(QmlQwtPlotWidget::qmlAxisToPlot(axis));
		emit xAxisChanged(axis);
		emit dataChanged();
	}
}

QmlQwtPlotWidget::Axis QmlQwtPlotCurve::yAxis() const
{
	return QmlQwtPlotWidget::plotAxisToQml(static_cast<QwtPlot::Axis>(m_curve->yAxis()));
}

void QmlQwtPlotCurve::setYAxis(QmlQwtPlotWidget::Axis axis)
{
	if (yAxis() != axis) {
		m_curve->setYAxis(QmlQwtPlotWidget::qmlAxisToPlot(axis));
		emit yAxisChanged(axis);
		emit dataChanged();
	}
}

QwtPlotCurve *QmlQwtPlotCurve::curve() const
{
	return m_curve.data();
}

void QmlQwtPlotCurve::addSample(float x, float y)
{
	m_data << QPointF(x, y);
	m_curve->setSamples(m_data);
	emit dataChanged();
}

void QmlQwtPlotCurve::clearSamples()
{
	m_data.clear();
	m_curve->setSamples(m_data);
	emit dataChanged();
}


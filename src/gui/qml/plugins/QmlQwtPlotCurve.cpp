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
	QObject(parent),
	m_legendVisible(true),
	m_penStyle(Qt::SolidLine)
{
	m_curve = QSharedPointer<QwtPlotCurve>(new QwtPlotCurve());
	m_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	m_curve->setLegendAttribute(QwtPlotCurve::LegendShowLine);
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

QString QmlQwtPlotCurve::title() const
{
	return m_curve->title().text();
}

void QmlQwtPlotCurve::setTitle(const QString &title)
{
	if (m_curve->title().text() != title) {
		m_curve->setTitle(title);
		emit titleChanged(m_curve->title().text());
	}
}

bool QmlQwtPlotCurve::visible() const
{
	return m_curve->isVisible();
}

void QmlQwtPlotCurve::setVisible(bool visible)
{
	if (m_curve->isVisible() != visible) {
		m_curve->setVisible(visible);
		emit visibleChanged(m_curve->isVisible());
	}
}

bool QmlQwtPlotCurve::legendVisible() const
{
	return m_legendVisible;
}

void QmlQwtPlotCurve::setLegendVisible(bool legendVisible)
{
	if (m_legendVisible != legendVisible) {
		m_legendVisible = legendVisible;
		emit legendVisibleChanged(m_legendVisible);
	}
}

Qt::PenStyle QmlQwtPlotCurve::penStyle() const
{
	return m_penStyle;
}

void QmlQwtPlotCurve::setPenStyle(Qt::PenStyle penStyle)
{
	if (m_penStyle != penStyle) {
		m_penStyle = penStyle;
		QPen pen = m_curve->pen();
		pen.setStyle(m_penStyle);
		m_curve->setPen(pen);
		emit penStyleChanged(m_penStyle);
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


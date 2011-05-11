/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  10.05.2011 13:23:33
 *         Author:  Miroslav Bendík
 *        Company:  LinuxOS.sk
 * =====================================================================
 */

#ifndef QMLQWTPLOTCURVE_9JVO4DO2
#define QMLQWTPLOTCURVE_9JVO4DO2

#include <QColor>
#include <QObject>
#include <QPointF>
#include <QSharedPointer>
#include <QVector>
#include <qwt_plot.h>
#include "QmlQwtPlotWidget.h"
class QwtPlotCurve;

class QmlQwtPlotCurve: public QObject
{
Q_OBJECT
Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged);
Q_PROPERTY(QmlQwtPlotWidget::Axis xAxis READ xAxis WRITE setXAxis NOTIFY xAxisChanged);
Q_PROPERTY(QmlQwtPlotWidget::Axis yAxis READ yAxis WRITE setYAxis NOTIFY yAxisChanged);
Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged);
Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged);
Q_PROPERTY(bool legendVisible READ legendVisible WRITE setLegendVisible NOTIFY legendVisibleChanged);
Q_PROPERTY(Qt::PenStyle penStyle READ penStyle WRITE setPenStyle NOTIFY penStyleChanged);
public:
	QmlQwtPlotCurve(QObject *parent = 0);
	~QmlQwtPlotCurve();
	QColor color() const;
	void setColor(const QColor &color);
	QmlQwtPlotWidget::Axis xAxis() const;
	void setXAxis(QmlQwtPlotWidget::Axis axis);
	QmlQwtPlotWidget::Axis yAxis() const;
	void setYAxis(QmlQwtPlotWidget::Axis axis);
	QString title() const;
	void setTitle(const QString &title);
	bool visible() const;
	void setVisible(bool visible);
	bool legendVisible() const;
	void setLegendVisible(bool legendVisible);
	Qt::PenStyle penStyle() const;
	void setPenStyle(Qt::PenStyle penStyle);
	QwtPlotCurve *curve() const;
	Q_INVOKABLE void addSample(float x, float y);
	Q_INVOKABLE void clearSamples();

signals:
	void colorChanged(const QColor &color);
	void xAxisChanged(QmlQwtPlotWidget::Axis axis);
	void yAxisChanged(QmlQwtPlotWidget::Axis axis);
	void titleChanged(const QString &title);
	void visibleChanged(bool visible);
	void legendVisibleChanged(bool legendVisible);
	void penStyleChanged(Qt::PenStyle penStyle);
	void dataChanged();

private:
	QSharedPointer<QwtPlotCurve> m_curve;
	QVector<QPointF> m_data;
	bool m_legendVisible;
	Qt::PenStyle m_penStyle;
}; /* -----  end of class QmlQwtPlotCurve  ----- */

#endif /* end of include guard: QMLQWTPLOTCURVE_9JVO4DO2 */


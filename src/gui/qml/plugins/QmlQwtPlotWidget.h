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

#include <QDeclarativeListProperty>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <qwt_plot.h>
class QmlQwtPlotCurve;
class QwtLegend;

class QmlQwtPlotWidget: public QGraphicsProxyWidget
{
Q_OBJECT
Q_PROPERTY(QDeclarativeListProperty<QmlQwtPlotCurve> curves READ curves);
Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged);
Q_ENUMS(AxisScaleEngine);
Q_ENUMS(Axis);
public:
	enum Axis {
		YLeft,
		YRight,
		XBottom,
		XTop
	};
	enum AxisScaleEngine {
		LinearScaleEngine,
		Log10ScaleEngine
	};
	QmlQwtPlotWidget(QGraphicsItem *parent = 0);
	~QmlQwtPlotWidget();
	QString title() const;
	void setTitle(const QString &title);
	Q_INVOKABLE QmlQwtPlotWidget::AxisScaleEngine axisScaleEngine(QmlQwtPlotWidget::Axis axis) const;
	Q_INVOKABLE void setAxisScaleEngine(QmlQwtPlotWidget::Axis axis, QmlQwtPlotWidget::AxisScaleEngine scaleEngine);
	Q_INVOKABLE void setAxisScale(Axis axis, double min, double max, double stepSize = 0);
	QDeclarativeListProperty<QmlQwtPlotCurve> curves();
	static void curveAppend(QDeclarativeListProperty<QmlQwtPlotCurve> *property, QmlQwtPlotCurve *value);
	static int curveCount(QDeclarativeListProperty<QmlQwtPlotCurve> *property);
	static QmlQwtPlotCurve *curveAt(QDeclarativeListProperty<QmlQwtPlotCurve> *property, int index);
	static void curveClear(QDeclarativeListProperty<QmlQwtPlotCurve> *property);

	static QwtPlot::Axis qmlAxisToPlot(QmlQwtPlotWidget::Axis axis);
	static QmlQwtPlotWidget::Axis plotAxisToQml(QwtPlot::Axis axis);

signals:
	void titleChanged(const QString &title);

public slots:
	void update();

private slots:
	void onUpdateTimeout();
	void onLegendVisibleChanged();

private:
	QList<QmlQwtPlotCurve*> m_curves;
	QwtPlot *m_plot;
	QTimer *m_updateTimer;
	bool m_updateRequired;
	QVector<QmlQwtPlotWidget::AxisScaleEngine> m_scaleEngine;
	QwtLegend *m_legend;
}; /* -----  end of class QmlQwtPlotWidget  ----- */

#endif /* end of include guard: QMLQWTPLOTWIDGET_G5DFJVX5 */


#pragma once

#include <QGraphicsSimpleTextItem>
#include <QScatterSeries>
#include <QLegendMarker>
#include <QButtonGroup>
#include <QMouseEvent>
#include <QPushButton>
#include <QLineSeries>
#include <QChartView>
#include <QValueAxis>
#include <QDateTime>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <cmath>
#include "obj.h"

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
	Q_OBJECT

public:
    ChartView(QWidget *parent = nullptr);
	virtual ~ChartView();
    void chartInit(double minx, double maxx, double miny, double maxy, QString title);
    void init(double minx, double maxx, double miny, double maxy, QString title);
    void dispMap(QVector<QPointF> map[], double minx, double maxx, double miny, double maxy, bool CCU);
    void updateLocal(Frame &frame);
    void updateGlobal(Frame &frame);
    void setPointSizeObj(int &size);
    void setPointSizeMap(int &size);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private slots:
    void on_LegendMarkerClicked();

private:
    QPointF m_lastPoint;
    bool m_isPress;
    QGraphicsSimpleTextItem *m_coordItem = nullptr;
    QGraphicsSimpleTextItem *m_coordItem1 = nullptr;
    QChart *_chart = nullptr;
    double minx, maxx, miny, maxy;
    bool first = false;
    bool mapIn = false;
    QRectF area_;
};

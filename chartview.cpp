#include "chartview.h"
/* @brief:构造函数
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
ChartView::ChartView(QWidget *parent)
    : QChartView(parent), m_isPress(false), m_coordItem(nullptr), _chart(new QChart())
{
	this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setMouseTracking(false);   
}

/* @brief:析构函数
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
ChartView::~ChartView()
{
    delete _chart;
}

/* @brief:鼠标按下事件
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_lastPoint = event->pos();
        m_isPress = true;
    }

    QChartView::mousePressEvent(event); //仍可相应原事件
}

/* @brief:鼠标移动事件
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_coordItem)
    {
        m_coordItem = new QGraphicsSimpleTextItem(this->chart());
        m_coordItem->setZValue(5);
        m_coordItem->setPos(60, 20);
        m_coordItem->show();
    }
    const QPoint curPos = event->pos();
    QPointF curVal = this->chart()->mapToValue(QPointF(curPos));
    QString coordStr = QString("X = %1\nY = %2").arg(curVal.x(), 0, 'f', 6).arg(curVal.y(), 0, 'f', 6);
    m_coordItem->setText(coordStr);

    if (m_isPress)
    {
        QPointF offset = curPos - m_lastPoint;
        m_lastPoint = curPos;
        this->chart()->scroll(-offset.x(), offset.y());
    }
}

/* @brief:鼠标释放事件
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
	m_isPress = false;
    if (event->button() == Qt::RightButton)     //鼠标右键，重置坐标系
    {
        this->chart()->axisX()->setRange(minx, maxx);
        this->chart()->axisY()->setRange(miny, maxy);
	}

    QChartView::mouseReleaseEvent(event);
}

/* @brief:滚轮滚动事件
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::wheelEvent(QWheelEvent *event)
{
//    float length = (oPlotAreaRect.width() + oPlotAreaRect.height())/2.0;
//    length *= rVal;
//    // 2. 水平调整
//    oPlotAreaRect.setWidth(length);
//    // 3. 竖直调整
//    oPlotAreaRect.setHeight(length);
    qreal rVal = std::pow(0.999, event->delta()); // 设置比例
    // 1. 读取视图基本信息
    QRectF oPlotAreaRect = this->chart()->plotArea();
    float length = (oPlotAreaRect.width() + oPlotAreaRect.height())/2.0;
    QPointF oCenterPoint = oPlotAreaRect.center();
    // 2. 水平调整
    oPlotAreaRect.setWidth(oPlotAreaRect.width() * rVal);
    // 3. 竖直调整
    oPlotAreaRect.setHeight(oPlotAreaRect.height() * rVal);
//    oPlotAreaRect.setWidth(length* rVal);
    // 3. 竖直调整
//    oPlotAreaRect.setHeight(length * rVal);
//    qDebug()<<oPlotAreaRect.width()<<" "<<oPlotAreaRect.height()<<" "<<length*rVal;

    // 4.1 计算视点，视点不变，围绕中心缩放
    //QPointF oNewCenterPoint(oCenterPoint);
    // 4.2 计算视点，让鼠标点击的位置移动到窗口中心
    //QPointF oNewCenterPoint(pEvent->pos());
    // 4.3 计算视点，让鼠标点击的位置尽量保持不动(等比换算，存在一点误差)
    QPointF oNewCenterPoint(2 * oCenterPoint - event->pos() - (oCenterPoint - event->pos()) / rVal);
    // 5. 设置视点
    oPlotAreaRect.moveCenter(oNewCenterPoint);
    // 6. 提交缩放调整
    this->chart()->zoomIn(oPlotAreaRect);
}



/* @brief: 显示帧信息
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void ChartView::on_LegendMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
//    switch (marker->type())
//    {
//        case QLegendMarker::LegendMarkerTypeXY:
//        {
            marker->series()->setVisible(!marker->series()->isVisible());
            marker->setVisible(true);
//        }
//    }
}

/* @brief:初始化坐标系
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::chartInit(double minx, double maxx, double miny, double maxy, QString title)
{

    this->minx = minx;
    this->maxx = maxx;
    this->miny = miny;
    this->maxy = maxy;

    _chart->setTitle(title);
    this->setChart(_chart);

    QValueAxis *axisX = new QValueAxis ();
    QValueAxis *axisY = new QValueAxis ();
    axisX->setMinorTickCount(4);    //设置副标尺个数
    axisY->setMinorTickCount(4);
    axisX->setRange(minx, maxx);     //设置坐标轴范围
    axisY->setRange(miny, maxy);
    axisX->setTitleText("X");
    axisY->setTitleText("Y");

    QScatterSeries *series[3];

    for(int i = 0; i < 3; i++)
    {
        series[i] = new QScatterSeries(this);

        _chart->addSeries(series[i]);
        series[i]->setPen(QPen(Qt::NoPen));
        series[i]->setMarkerSize(3);
        _chart->setAxisX(axisX, series[i]);
        _chart->setAxisY(axisY, series[i]);
    }

    series[0]->setName("origin");
    series[1]->setName("edge");
    series[2]->setName("obstacle");

    series[0]->append(0, 0);
    series[0]->setColor(QColor(0, 0, 0));
    QLineSeries *series_car = new QLineSeries(this);
    _chart->addSeries(series_car);
    series_car->setName("car");


    //series_car->setColor(QColor(237,145,33));
    _chart->setAxisX(axisX, series_car);
    _chart->setAxisY(axisY, series_car);


    //点击选择是否显示系列
    foreach (QLegendMarker* marker, _chart->legend()->markers())
    {
        disconnect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
        connect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
    }
}

void ChartView::init(double minx, double maxx, double miny, double maxy, QString title)
{
    this->minx = minx;
    this->maxx = maxx;
    this->miny = miny;
    this->maxy = maxy;

    _chart->setTitle(title);
    this->setChart(_chart);

    QValueAxis *axisX = new QValueAxis ();
    QValueAxis *axisY = new QValueAxis ();
    axisX->setMinorTickCount(4);    //设置副标尺个数
    axisY->setMinorTickCount(4);
    axisX->setRange(minx, maxx);     //设置坐标轴范围
    axisY->setRange(miny, maxy);
    axisX->setTitleText("X");
    axisY->setTitleText("Y");

    QScatterSeries *series[9];

    for(int i = 0; i < 3; i++)
    {
        series[i] = new QScatterSeries(this);

        _chart->addSeries(series[i]);
        series[i]->setPen(QPen(Qt::NoPen));
        series[i]->setMarkerSize(5);
        _chart->setAxisX(axisX, series[i]);
        _chart->setAxisY(axisY, series[i]);
    }

    series[0]->setName("origin");
    series[1]->setName("edge");
    series[2]->setName("obstacle");

    series[0]->append(0, 0);
    series[0]->setColor(QColor(0, 0, 0));

    QLineSeries *series_car = new QLineSeries(this);
    _chart->addSeries(series_car);
    series_car->setName("car");
    //series_car->setColor(QColor(237,145,33));
    _chart->setAxisX(axisX, series_car);
    _chart->setAxisY(axisY, series_car);


    //点击选择是否显示系列
    foreach (QLegendMarker* marker, _chart->legend()->markers())
    {
        disconnect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
        connect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
    }
}

/* @brief:显示地图
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::dispMap(QVector<QPointF> map[],double minx, double maxx, double miny, double maxy, bool CCU)
{

    if(maxx - minx > maxy - miny)               //计算坐标系坐标范围
    {
        this->minx = minx;
        this->maxx = maxx;
        this->miny = (maxy + miny)/2 - (maxx - minx)/2;
        this->maxy = (maxy + miny)/2 + (maxx - minx)/2;
    } else{
        this->minx = (maxx + minx)/2 - (maxy - miny)/2;
        this->maxx = (maxx + minx)/2 + (maxy - miny)/2;
        this->miny = miny;
        this->maxy = maxy;
    }

    QScatterSeries *series[6];
    QValueAxis *axisX = (QValueAxis *)this->chart()->axisX();
    QValueAxis *axisY = (QValueAxis *)this->chart()->axisY();

    axisX->setRange(this->minx, this->maxx);     //设置坐标轴范围
    axisY->setRange(this->miny, this->maxy);
    if(CCU){
        for(int i = 0; i<6; i++)
            series[i] =  (QScatterSeries *)this->chart()->series().at(i+4);
    }else{
        for(int i = 0; i<6; i++)
        {
            if(mapIn) series[i] = (QScatterSeries *)this->chart()->series().at(i+4);
            else
            {
                series[i] = new QScatterSeries(this);
                _chart->addSeries(series[i]);
                series[i]->setPen(QPen(Qt::NoPen));
                series[i]->setMarkerSize(1);
                _chart->setAxisX(axisX, series[i]);
                _chart->setAxisY(axisY, series[i]);
            }
        }
        if(!mapIn)
        {
            series[0]->setName("f0");
            series[1]->setName("f1");
            series[2]->setName("ll");
            series[3]->setName("lr");
            series[4]->setName("rl");
            series[5]->setName("rr");
        }
    }

    for(int i = 0; i<6; i++) series[i]->replace(map[i]);

    foreach (QLegendMarker* marker, _chart->legend()->markers())
    {
        disconnect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
        connect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
    }
    mapIn = true;
}

/* @brief:更新本地坐标系
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::updateLocal(Frame &frame)
{
    QScatterSeries *series_edge = (QScatterSeries *)this->chart()->series().at(1);
    QScatterSeries *series_obs = (QScatterSeries *)this->chart()->series().at(2);
    QLineSeries *series_car = (QLineSeries *)this->chart()->series().at(3);

    series_edge->clear();
    series_obs->clear();
    series_car->clear();

    for(int i = 0; i < frame.points.size(); i++)
    {
        if(frame.points[i].type == 0) series_edge->append(frame.points[i].xyz(0), frame.points[i].xyz(1)); else
        if(frame.points[i].type == 1) series_obs->append(frame.points[i].xyz(0), frame.points[i].xyz(1));
    }
    for(int i = 0; i < 5; i++) series_car->append(frame.car[i].xyz(0),frame.car[i].xyz(1));
}

/* @brief:更新utm坐标系
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::updateGlobal(Frame &frame)
{
    QScatterSeries *series_edge = (QScatterSeries *)this->chart()->series().at(1);
    QScatterSeries *series_obs = (QScatterSeries *)this->chart()->series().at(2);
    QLineSeries *series_car = (QLineSeries *)this->chart()->series().at(3);

    series_edge->clear();
    series_obs->clear();
    series_car->clear();

    for(int i = 0; i < frame.points.size(); i++)
    {
        if(frame.points[i].type == 0) series_edge->append(frame.points[i].utm(0), frame.points[i].utm(1));
        else if(frame.points[i].type == 1) series_obs->append(frame.points[i].utm(0), frame.points[i].utm(1));
    }
    for(int i = 0; i < 5; i++) series_car->append(frame.car[i].utm(0),frame.car[i].utm(1));
}


/* @brief:设置障碍物点尺寸
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::setPointSizeObj(int &size)
{
    QScatterSeries *series_edge = (QScatterSeries *)this->chart()->series().at(1);
    QScatterSeries *series_obs = (QScatterSeries *)this->chart()->series().at(2);

    series_edge->setMarkerSize(size);
    series_obs->setMarkerSize(size);
}

/* @brief:设置障碍物点尺寸
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
void ChartView::setPointSizeMap(int &size)
{
    QScatterSeries *series[6];

    for(int i = 0; i<6; i++)
    {
        series[i] = (QScatterSeries *)this->chart()->series().at(i+4);
        series[i]->setMarkerSize(size);
    }
}

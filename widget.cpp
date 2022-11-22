#include "widget.h"
#include "./ui_widget.h"

template <typename T>
bool Widget::CheckEmpty(T t){
  if(t.empty())
  {
    QMessageBox::information(this, "Error", "没有障碍物信息！");
    on_pushButton_pause_clicked();
    return true;
  }
  else
  {
    return false;
  }
}

int Widget::CheckPlayMode(){
  //icu
  if(ui->checkBox_icu -> isChecked() &&
     !ui->checkBox_prediction -> isChecked() &&
     !ui->checkBox_control -> isChecked()){
    return PlayMode::kICU;
  }else if(!ui->checkBox_icu -> isChecked() &&
           ui->checkBox_prediction -> isChecked() &&
           !ui->checkBox_control -> isChecked()){//prediction
    return PlayMode::KPrediction;
  }else if(!ui->checkBox_icu -> isChecked() &&
           !ui->checkBox_prediction -> isChecked() &&
           ui->checkBox_control -> isChecked()){ //control
    return PlayMode::kCtrl;
  }else if(ui->checkBox_icu -> isChecked() &&
           ui->checkBox_prediction -> isChecked() &&
           !ui->checkBox_control -> isChecked()){ //icu&prediction
    return PlayMode::kICUAndPrediction;
  }else if(!ui->checkBox_icu -> isChecked() &&
           ui->checkBox_prediction -> isChecked() &&
           ui->checkBox_control -> isChecked()){ //prediction & control

    return PlayMode::kPredictionAndControl;
  }else if(ui->checkBox_icu -> isChecked() &&
           ui->checkBox_prediction -> isChecked() &&
           ui->checkBox_control -> isChecked()){ //icu&prediction & control
    return PlayMode::kAll;
  }else{
    return PlayMode::kError;
  }
  return PlayMode::kError;
}
/* @brief:构造函数
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget)
{
  setWindowTitle("replay tool");
  setMinimumSize(1230,800);
  min_utm_x = 10000000, max_utm_x = 0, min_utm_y = 10000000, max_utm_y = 0;
  plot_min_x_ = 10000000, plot_max_x_ = 0, plot_min_y_ = 10000000, plot_max_y_ = 0;
  ctrl_min_y_ = 100, ctrl_max_y_ = 0;

  ui->setupUi(this);
//  tooltip_ = new QCPToolTip(ui->control_plot);
  //ICU界面初始化
  ui->chartview->chartInit(-60, 60, -30, 90, "激光雷达坐标系");         //初始化坐标系
  ui->globalview_icu->init(-100, 100, -100, 100, "UTM坐标系");

  TimerInit();
  WidgetInit();
  UIInit();
  ComboBoxInit();

  control_start_ = 0;

  trans_bp_imu = calcTransformMatrix(lidar_offset_x, lidar_offset_y, lidar_offset_z, lidar_yaw, lidar_pitch, lidar_roll);
}

/* @brief:析构函数
 * @param [in]:NONE
 * @param [out]:NONE
 * @return:NONE
 */
Widget::~Widget()
{
  delete ui;
}

void Widget::TimerInit()
{
  fTimer_ = new QTimer(this);                              //设置定时器
  fTimer_->stop();
  fTimer_->setInterval(time_interval);

  connect(fTimer_, SIGNAL(timeout()), this, SLOT(on_timeout()));
}

void Widget::WidgetInit()
{

  tracer_ = new QCPItemTracer(ui->control_plot); //生成游标
  tracer_->setPen(QPen(Qt::red));//圆圈轮廓颜色
  tracer_->setBrush(QBrush(Qt::red));//圆圈圈内颜色
  tracer_->setStyle(QCPItemTracer::tsCircle);//圆圈
  tracer_->setSize(5);//设置大小
  tracer_->setVisible(false);


  tracer_label_ = new QCPItemText(ui->control_plot); //生成游标说明
  tracer_label_->setLayer("overlay");//设置图层为overlay，因为需要频繁刷新
  tracer_label_->setPen(QPen(Qt::black));//设置游标说明颜色
  tracer_label_->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);//左上
  tracer_label_->position->setParentAnchor(tracer_->position);//将游标说明锚固在tracer位置处，实现自动跟随
  tracer_label_->setVisible(false);


  //输出信息的lineEdit设置为只读
  ui->lineEdit_index->setReadOnly(true);
  ui->lineEdit_obscount->setReadOnly(true);
  ui->lineEdit_framecount->setReadOnly(true);

  //播放功能中 button设置为false
  ui->pushButton_play->setEnabled(false);
  ui->pushButton_pause->setEnabled(false);
  ui->pushButton_pre->setEnabled(false);
  ui->pushButton_next->setEnabled(false);
  ui->pushButton_reset->setEnabled(false);

  //radioButton初始化
  group_button_ = new QButtonGroup(this);                   //初始化前向后向icu ccu control选择
  group_button_->addButton(ui->radioButton_front, 0);
  group_button_->addButton(ui->radioButton_back, 1);
  ui->radioButton_front->setChecked(true);

  //checkbox初始化
  ui->checkBox_icu->setTristate(false);
  ui->checkBox_prediction->setTristate(false);
  ui->checkBox_control->setTristate(false);
  ui->checkBox_icu->setEnabled(false);
  ui->checkBox_prediction->setEnabled(false);
  ui->checkBox_control->setEnabled(false);

  ui->horizontalSlider_local->setRange(1, 10);
  ui->horizontalSlider_local->setValue(5);
  ui->objSizeLocal->setText(QString::number(ui->horizontalSlider_local->value()));

  ui->horizontalSlider_global->setRange(1, 10);
  ui->horizontalSlider_global->setValue(5);
  ui->objSizeGlobal->setText(QString::number(ui->horizontalSlider_global->value()));

  QList<QLineEdit *> lineEdit = ui->groupBox_5->findChildren<QLineEdit *>();
  for(int i = 0; i < lineEdit.count(); ++i)
  {
    lineEdit[i]->setReadOnly(true);
  }
}

void Widget::UIInit(){

  //prediction界面初始化
  ui->prediction_plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectPlottables);
  ui->prediction_plot->axisRect()->setRangeZoom(Qt::Vertical|Qt::Horizontal);
  //设置横纵坐标系精度
  ui->prediction_plot->xAxis->setNumberPrecision(9);
  ui->prediction_plot->yAxis->setNumberPrecision(10);

  //添加图层
  ui->prediction_plot->addGraph();
  prediction_graph_manage_.insert("map_edge", 0);
  ui->prediction_plot->addGraph();
  prediction_graph_manage_.insert("map_route", 1);

  //control界面初始化
  ui->control_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                  QCP::iSelectLegend | QCP::iSelectPlottables);
  ui->control_plot->axisRect()->setupFullAxesBox();
  ui->control_plot->yAxis2->setVisible(true);
  ui->control_plot->yAxis2->setTickLabels(true);
  ui->control_plot->xAxis->setLabel("x Axis");
  ui->control_plot->yAxis->setLabel("纵向");
  ui->control_plot->yAxis2->setLabel("横向");

  QFont legendFont = font();
  legendFont.setPointSize(10);
  ui->control_plot->legend->setFont(legendFont);
  ui->control_plot->legend->setSelectedFont(legendFont);
  ui->control_plot->legend->setSelectableParts(QCPLegend::spItems);
  ui->control_plot->legend->setVisible(false);
  //设置横纵坐标系精度
//  ui->control_plot->xAxis->setNumberPrecision(9);
  ui->control_plot->yAxis->setNumberPrecision(10);
  ui->control_plot->yAxis2->setNumberPrecision(10);
  //添加图层
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("gps_heading", 0);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("gps_speed", 1);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("rest_dis", 2);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("bia_distance", 3);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("preview_dis", 4);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("curve_prepoint", 5);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("angle_deviation_at_prepoint", 6);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("heading_angle_deviation_at_prepoint", 7);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("record_m_angle_head", 8);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("dev_angle_ctl", 9);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("dev_heading_ctl", 10);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("front_wheel_angle", 11);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("wire_status_front_wheel_angle", 12);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("desired_speed", 13);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("desired_lidar_speed", 14);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("speed_preview", 15);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("vehicle_velocity", 16);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("pitch_filter_record", 17);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("slope_vehicle", 18);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("slope_preview", 29);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("speed_ctl_state", 20);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("spd_e", 21);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("spd_ec", 22);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("base_ctl_value", 23);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("delta_ctl_value", 24);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("total_ctl_value", 25);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("shift_ctrl", 26);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("wire_status_gear_shift", 27);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("throttle_pedal", 28);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("wire_status_acc_actuate_percent", 29);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("brake_pedal", 30);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("wire_status_electric_brake_actuate_percent", 31);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("hydraulic_brake", 32);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("wire_status_mechanical_brake_actuate_percent", 33);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("wire_status_front_brake_percent", 34);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("object_stop_flag", 35);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("dis_path", 36);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("path_wheel_angle_pre", 37);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis);
  control_graph_manage_.insert("order2_kalman_speed", 38);
  ui->control_plot->addGraph(ui->control_plot->xAxis,ui->control_plot->yAxis2);
  control_graph_manage_.insert("lat_bia_dis_pre", 39);
  for(int i = 0; i < 40; ++i)
  {
    ui->control_plot->graph(i)->setName(control_graph_manage_.key(i));
    ui->control_plot->graph(i)->setVisible(false);
  }
//  ui->control_plot->legend->clearItems();
//  ui->control_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignCenter | Qt::AlignTop);

  // connect slot that ties some axis selections together (especially opposite axes):
  connect(ui->control_plot, SIGNAL(selectionChangedByUser()), this, SLOT(SelectionChanged()));
  // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
  connect(ui->control_plot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(MousePress(QMouseEvent *)));
  connect(ui->control_plot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(MouseWheel()));
  connect(ui->control_plot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(MouseMove(QMouseEvent*)));

  // make bottom and left axes transfer their ranges to top and right axes:
  connect(ui->control_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->control_plot->xAxis2, SLOT(setRange(QCPRange)));
//  connect(ui->control_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->control_plot->yAxis2, SLOT(setRange(QCPRange)));

  ui->control_plot->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->control_plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ContextMenuRequest(QPoint)));

//combobox signal
  connect(ui->comboBox_horizontal, SIGNAL(selectionChanged()), this, SLOT(ComboBoxSelectionChanged()));
  connect(ui->comboBox_vertical, SIGNAL(selectionChanged()), this, SLOT(ComboBoxSelectionChanged()));
}

void Widget::ComboBoxInit(){
  // horizontal init
  ui->comboBox_horizontal->addItem("航向");
  ui->comboBox_horizontal->addItem("横向偏差");
  ui->comboBox_horizontal->addItem("横向控制预瞄距离");
  ui->comboBox_horizontal->addItem("控制曲率");
  ui->comboBox_horizontal->addItem("预瞄点-当前点方位角偏差");
  ui->comboBox_horizontal->addItem("预瞄点-当前点航向角偏差");
  ui->comboBox_horizontal->addItem("关键点-当前点航向偏差");
  ui->comboBox_horizontal->addItem("方位/横向偏差转角指令");
  ui->comboBox_horizontal->addItem("航向偏差转角指令");
  ui->comboBox_horizontal->addItem("总转角指令");
  ui->comboBox_horizontal->addItem("前轮转角");
  ui->comboBox_horizontal->addItem("前馈转向");
  ui->comboBox_horizontal->addItem("优化预测横向偏差");

  control_info_manage_.insert("航向","gps_heading");
  control_info_manage_.insert("横向偏差","bia_distance");
  control_info_manage_.insert("横向控制预瞄距离","preview_dis");
  control_info_manage_.insert("控制曲率","curve_prepoint");
  control_info_manage_.insert("预瞄点-当前点方位角偏差","angle_deviation_at_prepoint");
  control_info_manage_.insert("预瞄点-当前点航向角偏差","heading_angle_deviation_at_prepoint");
  control_info_manage_.insert("关键点-当前点航向偏差","record_m_angle_head");
  control_info_manage_.insert("方位/横向偏差转角指令","dev_angle_ctl");
  control_info_manage_.insert("航向偏差转角指令","dev_heading_ctl");
  control_info_manage_.insert("总转角指令","front_wheel_angle");
  control_info_manage_.insert("前轮转角","wire_status_front_wheel_angle");
  control_info_manage_.insert("前馈转向","wire_status_front_wheel_angle");
  control_info_manage_.insert("优化预测横向偏差","lat_bia_dis_pre");

  // vertical init
  ui->comboBox_vertical->addItem("gps速度");
  ui->comboBox_vertical->addItem("路径停车距离误差");
  ui->comboBox_vertical->addItem("总期望车速");
  ui->comboBox_vertical->addItem("感知期望车速");
  ui->comboBox_vertical->addItem("道路期望车速");
  ui->comboBox_vertical->addItem("实际车速");
  ui->comboBox_vertical->addItem("俯仰角滤波");
  ui->comboBox_vertical->addItem("车辆坡度滤波");
  ui->comboBox_vertical->addItem("预瞄坡度滤波");
  ui->comboBox_vertical->addItem("速度控制状态");
  ui->comboBox_vertical->addItem("速度偏差");
  ui->comboBox_vertical->addItem("速度偏差变化滤波");
  ui->comboBox_vertical->addItem("速度控制基础量");
  ui->comboBox_vertical->addItem("速度控制调节量");
  ui->comboBox_vertical->addItem("速度控制总控制量");
  ui->comboBox_vertical->addItem("档位控制量");
  ui->comboBox_vertical->addItem("档位反馈");
  ui->comboBox_vertical->addItem("油门控制量");
  ui->comboBox_vertical->addItem("油门反馈");
  ui->comboBox_vertical->addItem("电制动控制量");
  ui->comboBox_vertical->addItem("电制动反馈");
  ui->comboBox_vertical->addItem("液压制动控制量");
  ui->comboBox_vertical->addItem("制动反馈");
  ui->comboBox_vertical->addItem("前制动反馈");
  ui->comboBox_vertical->addItem("object_stop_flag");
  ui->comboBox_vertical->addItem("障碍物路径距离");
  ui->comboBox_vertical->addItem("二阶卡尔曼滤波速度");

  control_info_manage_.insert("gps速度","gps_speed");
  control_info_manage_.insert("路径停车距离误差","rest_dis");
  control_info_manage_.insert("总期望车速","desired_speed");
  control_info_manage_.insert("感知期望车速","desired_lidar_speed");
  control_info_manage_.insert("道路期望车速","speed_preview");
  control_info_manage_.insert("实际车速","vehicle_velocity");
  control_info_manage_.insert("俯仰角滤波","pitch_filter_record");
  control_info_manage_.insert("车辆坡度滤波","slope_vehicle");
  control_info_manage_.insert("预瞄坡度滤波","slope_preview");
  control_info_manage_.insert("速度控制状态","speed_ctl_state");
  control_info_manage_.insert("速度偏差","spd_e");
  control_info_manage_.insert("速度偏差变化滤波","spd_ec");
  control_info_manage_.insert("速度控制基础量","base_ctl_value");
  control_info_manage_.insert("速度控制调节量","delta_ctl_value");
  control_info_manage_.insert("速度控制总控制量","total_ctl_value");
  control_info_manage_.insert("档位控制量","shift_ctrl");
  control_info_manage_.insert("档位反馈","wire_status_gear_shift");
  control_info_manage_.insert("油门控制量","throttle_pedal");
  control_info_manage_.insert("油门反馈","wire_status_acc_actuate_percent");
  control_info_manage_.insert("电制动控制量","brake_pedal");
  control_info_manage_.insert("电制动反馈","wire_status_electric_brake_actuate_percent");
  control_info_manage_.insert("液压制动控制量","hydraulic_brake");
  control_info_manage_.insert("制动反馈","wire_status_mechanical_brake_actuate_percent");
  control_info_manage_.insert("前制动反馈","wire_status_front_brake_percent");
  control_info_manage_.insert("object_stop_flag","object_stop_flag");
  control_info_manage_.insert("障碍物路径距离","dis_path");
  control_info_manage_.insert("二阶卡尔曼滤波速度","order2_kalman_speed");

}

void Widget::PlotScatters(QVector<double> x, QVector<double> y,Qt::GlobalColor color,int index,bool add,int shape,int shape_size, bool prediction){

  if(x.isEmpty() || y.isEmpty())
    return;

  QCPScatterStyle::ScatterShape dot_shape;
  switch(shape)
  {
  case 0:
    dot_shape = QCPScatterStyle::ssCircle;break;//空心圆
  case 1:
    dot_shape = QCPScatterStyle::ssDisc;break;//实心点
  case 2:
    dot_shape = QCPScatterStyle::ssStar;break;//星星
  case 3:
    dot_shape = QCPScatterStyle::ssPlusCircle;break;//圆中带十字
  default:
    dot_shape = QCPScatterStyle::ssCircle;break;//空心圆
  }

  //设置画笔颜色
  QColor my_color(color);
  my_color.setAlpha(250);//深颜色
  QPen pen;
  pen.setColor(my_color);
  QCPScatterStyle my_scatter_style(dot_shape,shape_size);
  my_scatter_style.setPen(pen);
  //ui->plot->addGraph();
  if(prediction)
  {
    if(add)
    {
      ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsLine);
    }
    else
    {
      ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsNone);
    }
    ui->prediction_plot->graph(index)->setScatterStyle(my_scatter_style);
    ui->prediction_plot->graph(index)->addData(x,y);
    ui->prediction_plot->replot();
  }
  else
  {
    if(add)
    {
      ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsLine);
    }
    else
    {
      ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsNone);
    }
    ui->control_plot->graph(index)->setScatterStyle(my_scatter_style);
    ui->control_plot->graph(index)->addData(x,y);
    ui->control_plot->replot();
  }
}

void Widget::PlotScatters(QVector<QPointF> point,Qt::GlobalColor color,int index,bool add,int shape,int shape_size,bool prediction){

  if(point.isEmpty())
    return;
  QVector<double> x, y;
  for(int i = 0; i < point.size(); ++i){
    x.push_back(point[i].x());
    y.push_back(point[i].y());
  }

  QCPScatterStyle::ScatterShape dot_shape;
  switch(shape)
  {
  case 0:
    dot_shape = QCPScatterStyle::ssCircle;break;//空心圆
  case 1:
    dot_shape = QCPScatterStyle::ssDisc;break;//实心点
  case 2:
    dot_shape = QCPScatterStyle::ssStar;break;//星星
  case 3:
    dot_shape = QCPScatterStyle::ssPlusCircle;break;//圆中带十字
  default:
    dot_shape = QCPScatterStyle::ssCircle;break;//空心圆
  }

  //设置画笔颜色
  QColor my_color(color);
  my_color.setAlpha(250);//深颜色
  QPen pen;
  pen.setColor(my_color);
  QCPScatterStyle my_scatter_style(dot_shape,shape_size);
  my_scatter_style.setPen(pen);
  //ui->plot->addGraph();
  if(prediction)
  {
    if(add)
    {
      ui->prediction_plot->graph(index)->setLineStyle(QCPGraph::lsLine);
    }
    else
    {
      ui->prediction_plot->graph(index)->setLineStyle(QCPGraph::lsNone);
    }
    ui->prediction_plot->graph(index)->setScatterStyle(my_scatter_style);
    ui->prediction_plot->graph(index)->setData(x,y);
    ui->prediction_plot->replot();
  }
  else
  {
    if(add)
    {
      ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsLine);
    }
    else
    {
      ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsNone);
    }
    ui->control_plot->graph(index)->setScatterStyle(my_scatter_style);
    ui->control_plot->graph(index)->setData(x,y);
    ui->control_plot->replot();
  }
}

void Widget::PlotScatter(double x, double y, Qt::GlobalColor color, int index, bool add, int shape, int shape_size, bool prediction){

  QCPScatterStyle::ScatterShape dot_shape;
  switch(shape)
  {
  case 0:
    dot_shape = QCPScatterStyle::ssCircle;break;//空心圆
  case 1:
    dot_shape = QCPScatterStyle::ssDisc;break;//实心点
  case 2:
    dot_shape = QCPScatterStyle::ssStar;break;//星星
  case 3:
    dot_shape = QCPScatterStyle::ssPlusCircle;break;//圆中带十字
  default:
    dot_shape = QCPScatterStyle::ssCircle;break;//空心圆
  }

  //设置画笔颜色
  QColor my_color(color);
  my_color.setAlpha(250);//深颜色
  QPen pen;
  pen.setColor(my_color);
  QCPScatterStyle my_scatter_style(dot_shape,shape_size);
  my_scatter_style.setPen(pen);
  //ui->plot->addGraph();
  if(prediction)
  {
    if(add)
    {
      ui->prediction_plot->graph(index)->setLineStyle(QCPGraph::lsLine);
    }
    else
    {
      ui->prediction_plot->graph(index)->setLineStyle(QCPGraph::lsNone);
    }
    ui->prediction_plot->graph(index)->setScatterStyle(my_scatter_style);
    ui->prediction_plot->graph(index)->addData(x,y);
    ui->prediction_plot->replot();
  }
  else
  {
    if(add)
    {
      if(ui->control_plot->graph(index)->dataCount() >= 1)
        ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsLine);
    }
    else
    {
      ui->control_plot->graph(index)->setLineStyle(QCPGraph::lsNone);
    }
    ui->control_plot->graph(index)->setScatterStyle(my_scatter_style);
    ui->control_plot->graph(index)->addData(x,y);
//    ui->control_plot->replot();
  }
}

void Widget::UpdateCCUMap(){
  QVector<QPointF> edge;
  QVector<QPointF> route;
  for(int i = 0; i < ccu_map_.size(); ++i){
    edge.push_back(ccu_map_[i].edge[0]);
    edge.push_back(ccu_map_[i].edge[1]);
    route.push_back(ccu_map_[i].route);
  }
//  PlotScatters(edge, Qt::black, prediction_graph_manage_.value("map_edge"), false, 1, 3, true);
//  PlotScatters(route, Qt::darkYellow, prediction_graph_manage_.value("map_route"), false, 1, 3, true);
  PlotScatters(route, Qt::black, prediction_graph_manage_.value("map_route"), false, 1, 3, true);
  ui->prediction_plot->rescaleAxes();
  AdjustPlotRange(true);
}

void Widget::UpdateCarBox(){
  QPen pen;
  pen.setColor(Qt::black);
  pen.setStyle(Qt::SolidLine);
  pen.setWidth(1);
  int size = ui->prediction_plot->itemCount();
  for(int i = 0; i < size; ++i){
    ui->prediction_plot->removeItem(ui->prediction_plot->item(0));
  }
  car_arrows_.clear();
  car_arrows_.resize(1e7);//无穷大
  if(!vehicle_box_in_){
    if(prediction_info_.empty()){
      return;
    }else{
      int index = 0;
      for(int i = 0; i < 48; ++i){
        for(int j = 1; j < 4; ++j){ //0为车辆中心点
          //        qDebug()<<QString::number(prediction_info_[prediction_index_].car[i][j].x(),'f',6)<<"  "
          //               <<QString::number(prediction_info_[prediction_index_].car[i][j].y(), 'f', 6);
          car_arrows_[index] = new QCPItemLine(ui->prediction_plot);
          car_arrows_[index]->start->setCoords(prediction_info_[prediction_index_].car[i][j].x(),
                                               prediction_info_[prediction_index_].car[i][j].y());
          car_arrows_[index]->end->setCoords(prediction_info_[prediction_index_].car[i][j+1].x(),
              prediction_info_[prediction_index_].car[i][j+1].y());
          car_arrows_[index]->setHead(QCPLineEnding::esNone);
          car_arrows_[index]->setPen(pen);
          index++;
        } //for j
        car_arrows_[index] = new QCPItemLine(ui->prediction_plot);
        car_arrows_[index]->start->setCoords(prediction_info_[prediction_index_].car[i][4].x(),
            prediction_info_[prediction_index_].car[i][4].y());
        car_arrows_[index]->end->setCoords(prediction_info_[prediction_index_].car[i][1].x(),
            prediction_info_[prediction_index_].car[i][1].y());
        car_arrows_[index]->setHead(QCPLineEnding::esNone);
        car_arrows_[index]->setPen(pen);
        index++;
      }// for i
      ui->prediction_plot->replot();
    }// if-else
  }
  else
  {
    if(car_.empty())
    {
      return;
    }
    else
    {
      int index = 0;
      for(int i = 0; i < car_.size(); ++i){
        for(int j = 1; j < 4; ++j){ //0为车辆中心点
          qDebug()<<QString::number(car_[i][j].x(),'f',6)<<"  "<<QString::number(car_[i][j].y(), 'f', 6);
          car_arrows_[index] = new QCPItemLine(ui->prediction_plot);
          car_arrows_[index]->start->setCoords(car_[i][j].x(), car_[i][j].y());
          car_arrows_[index]->end->setCoords(car_[i][j+1].x(), car_[i][j+1].y());
          car_arrows_[index]->setHead(QCPLineEnding::esNone);
          car_arrows_[index]->setPen(pen);
          index++;
        } //for j
        car_arrows_[index] = new QCPItemLine(ui->prediction_plot);
        car_arrows_[index]->start->setCoords(car_[i][4].x(), car_[i][4].y());
        car_arrows_[index]->end->setCoords(car_[i][1].x(), car_[i][1].y());
        car_arrows_[index]->setHead(QCPLineEnding::esNone);
        car_arrows_[index]->setPen(pen);
        index++;
      }// for i
      ui->prediction_plot->replot();
    }
  }
}

void Widget::UpdateObsBox(){
  QPen pen;
  pen.setColor(Qt::red);
  pen.setStyle(Qt::SolidLine);
  pen.setWidth(1);
//  int size = ui->prediction_plot->itemCount();
//  for(int i = 0; i < size; ++i){
//    ui->prediction_plot->removeItem(ui->prediction_plot->item(0));
//  }
  obs_arrows_.clear();
  obs_arrows_.resize(1e7);//无穷大
  if(prediction_info_.empty() || prediction_info_[prediction_index_].obs.num == 0){
    return;
  }else{
    int index = 0;
    int num = prediction_info_[prediction_index_].obs.num;
    for(int i = 0; i < num - 1; ++i){
      obs_arrows_[index] = new QCPItemLine(ui->prediction_plot);
      obs_arrows_[index]->start->setCoords(prediction_info_[prediction_index_].obs.vertex[i].x(),
                                           prediction_info_[prediction_index_].obs.vertex[i].y());
      obs_arrows_[index]->end->setCoords(prediction_info_[prediction_index_].obs.vertex[i+1].x(),
          prediction_info_[prediction_index_].obs.vertex[i+1].y());
      obs_arrows_[index]->setHead(QCPLineEnding::esNone);
      obs_arrows_[index]->setPen(pen);
      index++;
    }
    obs_arrows_[index] = new QCPItemLine(ui->prediction_plot);
    obs_arrows_[index]->start->setCoords(prediction_info_[prediction_index_].obs.vertex[num-1].x(),
        prediction_info_[prediction_index_].obs.vertex[num-1].y());
    obs_arrows_[index]->end->setCoords(prediction_info_[prediction_index_].obs.vertex[0].x(),
        prediction_info_[prediction_index_].obs.vertex[0].y());
    obs_arrows_[index]->setHead(QCPLineEnding::esNone);
    obs_arrows_[index]->setPen(pen);
    index++;
    ui->prediction_plot->replot();
  }
}

void Widget::ShowTracer(QString str){
  tracer_->position->setCoords(mouse_pos_.x(), mouse_pos_.y());//设置游标位置
  tracer_label_->setText(str);//设置游标说明内容
  tracer_label_->setVisible(true);
  tracer_->setVisible(true);
  ui->control_plot->replot();//绘制器一定要重绘，否则看不到游标位置更新情况
}

void Widget::PlayICUFrame(){
  if(frame_num_ < 0){
    frame_num_++;
    QMessageBox::information(this, "Error", "已经是第一帧！");
    return;
  }
  if(frame_num_ >= res.size()){
    frame_num_--;
    QMessageBox::information(this, "Error", "已经是一帧！");
    on_pushButton_pause_clicked();
    return;
  }
  ui->chartview->updateLocal(res[frame_num_]);
  ui->globalview_icu->updateGlobal(res[frame_num_]);
  DispICUInfo();
}

void Widget::PlayPredictionInfo(){

  if(prediction_index_ < 0){
    prediction_index_++;
    QMessageBox::information(this, "Error", "已经是第一帧！");
    return;
  }
  if(prediction_index_ >= prediction_info_.size()){
    prediction_index_--;
    on_pushButton_pause_clicked();
    QMessageBox::information(this, "Error", "已经是最后一帧！");
    return;
  }
  UpdateCarBox();
  UpdateObsBox();
  DispPredictionInfo();
}

void Widget::PlayControlInfo(bool still)
{
  if(still) // only control
  {
    if(CheckEmpty(control_info_)){
      QMessageBox::information(this, "Error", "没有控制信息！");
    }
    int start = ui->lineEdit_start->text().toInt();
    int end = ui->lineEdit_end->text().toInt();
    if(start < 1 || start > control_info_.size())
    {
      QMessageBox::information(this, "Error", "起始位置非法");
      start = 1;
    }
    if(end < 1 || end > control_info_.size() )
    {
      QMessageBox::information(this, "Error", "终点位置非法");
      end = control_info_.size();
    }
    if(start >= end)
    {
      QMessageBox::information(this, "Error", "位置设置非法");
      if(start > end){
        int tem = end;
        end = start;
        start = tem;
      }else{
        start = 1;
      }
    }
    ui->lineEdit_start->setText(QString::number(start));
    ui->lineEdit_end->setText(QString::number(end));
    control_start_ = start - 1;
    control_end_ = end - 1;
    qDebug() << control_start_ << " " << control_end_;
    QVector<double> x;
    QVector<double> y[40];
    for(int i = control_start_; i < control_end_ && i < control_info_.size(); ++i)
    {
      x.push_back(i);
      for(int j = 0; j < 40; ++j)
      {
        y[j].push_back(control_info_[i].info[j]);
      }
    }
    for(int i = 0; i < 40; ++i)
    {
      ui->control_plot->graph(i)->data().data()->clear();
      PlotScatters(x, y[i], Qt::black, i, true, 1, 3, false);
      ComboBoxSelectionChanged();
    }
//    PlotScatters(x,actual_y,Qt::black, control_graph_manage_.value("actual_speed"), true, 1, 3, false);
//    PlotScatters(x,expected_y,Qt::darkYellow, control_graph_manage_.value("expected_speed"), true, 1, 3,false);
//    PlotScatters(x,lateral_y,Qt::darkBlue, control_graph_manage_.value("lateral_deviation"), true, 1, 3,false);
//    AdjustPlotRange(false);
    ui->control_plot->rescaleAxes(true);
  }
  else // 联合播放
  {
    if(control_end_ >= control_info_.size()){
      QMessageBox::information(this, "Error", "已经是最后一行数据！");
      on_pushButton_pause_clicked();
      return;
    }
    double x = control_end_;
    for(int i = 0; i < 40 ; ++i)
    {
      double y = control_info_[control_end_].info[i];
      PlotScatter(x, y, Qt::black, i, true, 1, 3, false);
    }
    ui->lineEdit_start->setText(QString::number(control_start_));
    ui->lineEdit_end->setText(QString::number(control_end_));
    if(control_end_ - control_start_ >= 50)
    {
      ui->control_plot->xAxis->setRange(control_end_ - 50,control_end_ + 5);
    }
    else
    {
      ui->control_plot->xAxis->setRange(control_start_, control_start_ + 50);
    }
    ui->control_plot->replot();
  }
}

bool Widget::SearchSyncTime(PlayMode mode)
{
  int sync_icu = -1;
  int sync_prediction = -1;
  int sync_ctrl = -1;
  if(sync_ctrl_index_ >= control_info_.size() || sync_ctrl_index_ < 0) sync_ctrl_index_ = 0;
  TimeStamp initial;
  if(mode == PlayMode::kICUAndPrediction) //icu && prediction
  {
    //TODO tired 不知道怎么写，没有什么思路
  }
  else if (mode == PlayMode::kPredictionAndControl)//prediction&&control
  {
//    qDebug()<<control_end_;
    if(control_info_[sync_ctrl_index_].time > prediction_info_[prediction_index_].time)
    {
//      qDebug()<<"prediction";
      initial = control_info_[sync_ctrl_index_].time;
      sync_ctrl = sync_ctrl_index_;
    }
    else if(control_info_[sync_ctrl_index_].time < prediction_info_[prediction_index_].time)
    {
//      qDebug()<<"control";
      initial = prediction_info_[prediction_index_].time;
      sync_prediction = prediction_index_;
    }
    else if(control_info_[sync_ctrl_index_].time == prediction_info_[prediction_index_].time)
    {
//      control_end_ = control_end_;
//      qDebug()<<"equal";
      control_start_ = sync_ctrl_index_;
      control_end_ = control_start_;
      return true;
    }
    for(int i_ctrl = sync_ctrl_index_, i_prediction = prediction_index_; i_ctrl < control_info_.size() && i_prediction < prediction_info_.size(); )
    {
      if(sync_ctrl != -1)
      {
        if(initial == prediction_info_[i_prediction].time)
        {
          sync_prediction = i_prediction;
          break;
        }
        else
        {
          i_prediction++;
        }
      }
      else if(sync_prediction != -1)
      {
        if(initial == control_info_[i_ctrl].time)
        {
          sync_ctrl = i_ctrl;
          break;
        }
        else
        {
          i_ctrl++;
        }
      }
    }
    if(sync_ctrl < 0 || sync_prediction < 0)
    {
      return false;
    }
    else
    {
      control_start_ = sync_ctrl;
      control_end_ = sync_ctrl;
      prediction_index_ = sync_prediction;
      return true;
    }
  }
  else if(mode == 5)//icu&&prediction&&control
  {
    //TODO!!!!!!
  }
  return false;
}

void Widget::AdjustPlotRange(bool prediction){
  if(prediction)
  {
    if(ccu_map_.empty())
    {
      ui->prediction_plot->xAxis->setRange(415000,416250);
      ui->prediction_plot->yAxis->setRange(4.62779e+06,4.62846e+06);
      ui->prediction_plot->replot();
      return;
    }
    else
    {
      double x_middle = (plot_max_x_ + plot_min_x_)/2;
      double y_middle = (plot_max_y_ + plot_min_y_)/2;
      double _long = (plot_max_x_ - plot_min_x_) > (plot_max_y_ - plot_min_y_) ? (plot_max_x_ - plot_min_x_):(plot_max_y_ - plot_min_y_);
      double half_long = _long / 2.0;

      ui->prediction_plot->xAxis->setRange(x_middle - half_long -5e-4,x_middle + half_long + 5e-4);
      ui->prediction_plot->yAxis->setRange(y_middle - half_long -5e-4,y_middle + half_long + 5e-4);
      ui->prediction_plot->replot();
    }
  }
  else
  {
    qDebug() << " control start "<<control_start_<<" "<<control_end_;
    double x_middle = (control_start_ + control_end_)/2;
    double y_middle = (ctrl_max_y_ + ctrl_min_y_)/2;
    double x_long = control_end_ - control_start_;
    double y_long = ctrl_max_y_ - ctrl_min_y_;
    double x_half_long = x_long / 2.0;
    double y_half_long = y_long / 2.0;
    ui->control_plot->xAxis->setRange(x_middle - x_half_long - 0.5,x_middle + x_half_long + 0.5);
    ui->control_plot->yAxis->setRange(y_middle - y_half_long - 0.5,y_middle + y_half_long + 10);
    ui->control_plot->replot();
  }
}

void Widget::keyPressEvent(QKeyEvent *k)
{
  if(k->key() == Qt::Key_S || k->key() == Qt::Key_D){
    on_pushButton_next_clicked();
  }
  else if(k->key() == Qt::Key_A || k -> key() == Qt::Key_W){
    on_pushButton_pre_clicked();
  }
}

void Widget::MousePress(QMouseEvent * event)
{
  QList<QCPAxis*>axes;
  if (ui->control_plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    axes << ui->control_plot->xAxis;
    ui->control_plot->axisRect()->setRangeDragAxes(axes);
    ui->control_plot->axisRect()->setRangeDrag(ui->control_plot->xAxis->orientation());
  }
  else if (ui->control_plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    axes << ui->control_plot->yAxis;
    ui->control_plot->axisRect()->setRangeDragAxes(axes);
    ui->control_plot->axisRect()->setRangeDrag(ui->control_plot->yAxis->orientation());
  }
  else if (ui->control_plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis))
  {
    axes << ui->control_plot->yAxis2;
    ui->control_plot->axisRect()->setRangeDragAxes(axes);
    ui->control_plot->axisRect()->setRangeDrag(ui->control_plot->yAxis2->orientation());
  }
  else
  {
    axes << ui->control_plot->xAxis;
    axes << ui->control_plot->yAxis;
    axes << ui->control_plot->yAxis2;
    ui->control_plot->axisRect()->setRangeDragAxes(axes);
    ui->control_plot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  }
  int x_pos = event->pos().x();
  int y_pos = event->pos().y();

  float x_val = ui->control_plot->xAxis->pixelToCoord(x_pos);
  float y_val;
  if(left_){
    y_val = ui->control_plot->yAxis->pixelToCoord(y_pos);
    qDebug()<<"left";
  }
  else{
//    y_val = ui->control_plot->yAxis2->pixelToCoord(y_pos);
     y_val = ui->control_plot->yAxis->pixelToCoord(y_pos);
     qDebug()<<"right";
  }
  mouse_pos_ = QPointF(x_val, y_val);
  qDebug() << mouse_pos_;
  bool select = false;
  for (int i=0; i<ui->control_plot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->control_plot->graph(i);
    QCPPlottableLegendItem *item = ui->control_plot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      select = true;
    }
  }
  if(!select)
  {
    tracer_->setVisible(false);
    tracer_label_->setVisible(false);
    ui->control_plot->replot();
  }
}

void Widget::MouseWheel()
{
  QList<QCPAxis*>axes;
  if (ui->control_plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    axes << ui->control_plot->xAxis;
    ui->control_plot->axisRect()->setRangeZoomAxes(axes);
    ui->control_plot->axisRect()->setRangeZoom(ui->control_plot->xAxis->orientation());
  }
  else if (ui->control_plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    axes << ui->control_plot->yAxis;
    ui->control_plot->axisRect()->setRangeZoomAxes(axes);
    ui->control_plot->axisRect()->setRangeZoom(ui->control_plot->yAxis->orientation());
  }
  else if (ui->control_plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis))
  {
    axes << ui->control_plot->yAxis2;
    ui->control_plot->axisRect()->setRangeZoomAxes(axes);
    ui->control_plot->axisRect()->setRangeZoom(ui->control_plot->yAxis2->orientation());
  }
  else
  {
    axes << ui->control_plot->xAxis;
    axes << ui->control_plot->yAxis;
    axes << ui->control_plot->yAxis2;
    ui->control_plot->axisRect()->setRangeZoomAxes(axes);
    ui->control_plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
  }
}

void Widget::MouseMove(QMouseEvent * event)
{
  int x_pos = event->pos().x();
  int y_pos = event->pos().y();

  float x_val = ui->control_plot->xAxis->pixelToCoord(x_pos);
  float y_val = ui->control_plot->xAxis->pixelToCoord(y_pos);

//  mouse_pos_ = QPointF(x_val, y_val);
//  qDebug() << mouse_pos_;
}


void Widget::SelectionChanged()
{
  // synchronize selection of graphs with selection of corresponding legend items:
  bool select = false;
  for (int i=0; i<ui->control_plot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->control_plot->graph(i);
    QCPPlottableLegendItem *item = ui->control_plot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      select = true;
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
      QString str = control_info_manage_.key(control_graph_manage_.key(i));
      QPointF mouse_pos;
//      qDebug()<<graph->valueAxis()->axisType();
      if(graph->valueAxis()->axisType() == QCPAxis::atLeft){
        left_ = true;
        ui->control_plot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
      }else if(graph->valueAxis()->axisType() == QCPAxis::atRight){
        left_= false;
        ui->control_plot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
      }
      ShowTracer(str);
    }
  }

  if (ui->control_plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->control_plot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
      ui->control_plot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) || ui->control_plot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->control_plot->xAxis->selectedParts().testFlag(QCPAxis::spAxisLabel) || ui->control_plot->xAxis2->selectedParts().testFlag(QCPAxis::spAxisLabel))
  {
    ui->control_plot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
    ui->control_plot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->control_plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
      ui->control_plot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->control_plot->yAxis->selectedParts().testFlag(QCPAxis::spAxisLabel))
  {
    ui->control_plot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
  }
  if(ui->control_plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
     ui->control_plot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels) ||
     ui->control_plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxisLabel))
  {
    ui->control_plot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
  }

}

void Widget::ComboBoxHorizontalSelectionChanged()
{
  QStringList str_list = ui->comboBox_horizontal->currentText();
  QHash<QString, QString>::const_iterator it = control_info_manage_.constBegin();
  while (it != control_info_manage_.constEnd()) {
    if(str_list.indexOf(it.key()) != -1)
    {
      ui->control_plot->graph(control_graph_manage_.value(it.value()))->setVisible(true);
    }
    else
    {
      ui->control_plot->graph(control_graph_manage_.value(it.value()))->setVisible(false);
    }
    ++it;
  }
  ui->control_plot->replot();
}

void Widget::ComboBoxVerticalSelectionChanged()
{
  QStringList str_list = ui->comboBox_vertical->currentText();
  QHash<QString, QString>::const_iterator it = control_info_manage_.constBegin();
  while (it != control_info_manage_.constEnd()) {
//    cout << it.key() << ": " << it.value() << Qt::endl;
    if(str_list.indexOf(it.key()) != -1)
    {
      ui->control_plot->graph(control_graph_manage_.value(it.value()))->setVisible(true);
    }
    else
    {
      ui->control_plot->graph(control_graph_manage_.value(it.value()))->setVisible(false);
    }
    ++it;
  }
  ui->control_plot->replot();
}

void  Widget::ComboBoxSelectionChanged()
{
  QStringList str_list1 = ui->comboBox_vertical->currentText();
  QStringList str_list2 = ui->comboBox_horizontal->currentText();
  QStringList str_list = str_list1 + str_list2;
  QHash<QString, QString>::const_iterator it = control_info_manage_.constBegin();
  while (it != control_info_manage_.constEnd()) {
//    cout << it.key() << ": " << it.value() << Qt::endl;
    if(str_list.indexOf(it.key()) != -1)
    {
      ui->control_plot->graph(control_graph_manage_.value(it.value()))->setVisible(true);
    }
    else
    {
      ui->control_plot->graph(control_graph_manage_.value(it.value()))->setVisible(false);
    }
    ++it;
  }
  ui->control_plot->yAxis->rescale(true);
  ui->control_plot->yAxis2->rescale(true);
  if(CheckPlayMode() == PlayMode::kCtrl)
    ui->control_plot->xAxis->rescale(true);
  ui->control_plot->replot();
}

void Widget::ContextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  double legend_counts = ui->control_plot->legend->selectTest(pos, false);
  double graph_counts = ui->control_plot->graph()->selectTest(pos,false);

  //图例菜单
  if (( legend_counts >= 0) || ( graph_counts >= 0))
  {
    menu->addAction("显示", this, SLOT(ShowGraph()));
    menu->addAction("隐藏", this, SLOT(HideGraph()));
    menu->addAction("清除", this, SLOT(ClearCtrlAllGraphData()));
  }

  menu->popup(ui->control_plot->mapToGlobal(pos));
}

void Widget::ShowGraph()
{
  for (int i = 0; i < ui->control_plot->graphCount(); ++i)
  {
    QCPGraph *graphItem = ui->control_plot->graph(i);

    if (graphItem->selected())
    {
      QString str = control_info_manage_.value(control_graph_manage_.key(i));
      ui->comboBox_horizontal->ResetCheckBoxState(str);
      ui->comboBox_vertical->ResetCheckBoxState(str);
      graphItem->setVisible(true);
    }
  }
  ui->control_plot->replot();
  ui->control_plot->rescaleAxes();
}

void Widget::HideGraph()
{
  for (int i=0; i< ui->control_plot->graphCount(); ++i)
  {
    QCPGraph *graphItem = ui->control_plot->graph(i);

    if (graphItem->selected())
    {
      QString str = control_info_manage_.key(control_graph_manage_.key(i));
      ui->comboBox_horizontal->ResetCheckBoxState(str);
      ui->comboBox_vertical->ResetCheckBoxState(str);
      graphItem->setVisible(false);
    }
  }
  ui->control_plot->replot();
  ui->control_plot->rescaleAxes();
}

void Widget::ClearCtrlAllGraphData(){
  for (int i=0; i< ui->control_plot->graphCount(); ++i)
  {
    ui->control_plot->graph(i)->data().data()->clear();
  }
}

void Widget::on_pushButton_play_clicked()
{
  if(CheckPlayMode() == PlayMode::kError)
  {
    QMessageBox::information(this, "Error", "播放模式选择错误，请检查播放模式!");
    on_pushButton_pause_clicked();
    return;
  }
  else if(CheckPlayMode() == PlayMode::kCtrl)
  {
    bool still  = true;
    control_start_ = ui->lineEdit_start->text().toInt();
    control_end_ = ui->lineEdit_end->text().toInt();
    PlayControlInfo(still);
//    AdjustPlotRange(false);
  }
  else if(CheckPlayMode() == PlayMode::KPrediction && vehicle_box_in_)
  {
    UpdateCarBox();
  }
  else
  {
    if(CheckPlayMode() == PlayMode::kPredictionAndControl)
    {
      if(prediction_index_ == 0){
        ClearCtrlAllGraphData();
        sync_ctrl_index_ = 0;
      }else{
        sync_ctrl_index_ = control_end_;
      }
      if(!SearchSyncTime(PlayMode::kPredictionAndControl))
      {
        QMessageBox::information(this, "Error", "时间戳无法同步，检查文件内容！");
        return;
      }
    }
    fTimer_->start();
  }
  ui->checkBox_icu->setEnabled(false);
  ui->checkBox_prediction->setEnabled(false);
  ui->checkBox_control->setEnabled(false);
  ui->pushButton_play->setEnabled(false);
  ui->pushButton_pause->setEnabled(true);
  ui->pushButton_pre->setEnabled(false);
  ui->pushButton_next->setEnabled(false);
  ui->pushButton_reset->setEnabled(false);
  ui->lineEdit_num->setReadOnly(true);
  QList<QLineEdit *> lineEdit = ui->groupBox_5->findChildren<QLineEdit *>();
  for(int i = 0; i < lineEdit.count(); ++i)
  {
    lineEdit[i]->setReadOnly(true);
  }
}

void Widget::on_pushButton_pause_clicked(){
  fTimer_->stop();
  if(icu_info_in_) ui->checkBox_icu->setEnabled(true);
  if(prediction_info_in_) ui->checkBox_prediction->setEnabled(true);
  if(control_info_in_) ui->checkBox_control->setEnabled(true);
  ui->pushButton_play->setEnabled(true);
  ui->pushButton_pause->setEnabled(false);
  ui->pushButton_pre->setEnabled(true);
  ui->pushButton_next->setEnabled(true);
  ui->pushButton_reset->setEnabled(true);
  ui->lineEdit_num->setReadOnly(false);
  QList<QLineEdit *> lineEdit = ui->groupBox_5->findChildren<QLineEdit *>();
  for(int i = 0; i < lineEdit.count(); ++i)
  {
    lineEdit[i]->setReadOnly(false);
  }
}

void Widget::on_pushButton_pre_clicked(){
  if(CheckPlayMode() == PlayMode::kICU){ //icu
    if(CheckEmpty(res)){
      return;
    }
    frame_num_--;
    PlayICUFrame();
  }else if(CheckPlayMode() == PlayMode::KPrediction){//prediction
    prediction_index_--;
    PlayPredictionInfo();
  }else if(CheckPlayMode() == PlayMode::kCtrl){ //control
    if(CheckEmpty(control_info_)){
      return;
    }
    bool still = false;
    PlayControlInfo(still);
  }else if(CheckPlayMode() == PlayMode::kICUAndPrediction){ //icu&prediction
    if(CheckEmpty(res) || CheckEmpty(prediction_info_)){
      return;
    }
    //TODO icu&prediction数据播放

  }else if(CheckPlayMode() == PlayMode::kPredictionAndControl){ //prediction & control
    if(CheckEmpty(control_info_) || CheckEmpty(prediction_info_)){
      return;
    }
    //TODO prediction&control数据播放

  }else if(CheckPlayMode() == PlayMode::kAll){ //icu&prediction & control
    if(CheckEmpty(res) || CheckEmpty(control_info_) || CheckEmpty(prediction_info_)){
      return;
    }
    //TODO icu&prediction&control数据播放
  }else{
    QMessageBox::information(this, "Error", "播放模式选择错误，请检查播放模式!");
    return;
  }
}

/* @brief: 图表显示下一帧
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::on_pushButton_next_clicked()
{
  if(CheckPlayMode() == PlayMode::kICU){ //icu
    if(CheckEmpty(res)){
      return;
    }
    frame_num_++;
    PlayICUFrame();
  }else if(CheckPlayMode() == PlayMode::KPrediction){//prediction
    if(CheckEmpty(prediction_info_)){
      return;
    }
    prediction_index_++;
    PlayPredictionInfo();
  }else if(CheckPlayMode() == PlayMode::kCtrl){ //control
    if(CheckEmpty(control_info_)){
      return;
    }
    bool still = false;
    PlayControlInfo(false);
  }else if(CheckPlayMode() == PlayMode::kICUAndPrediction){ //icu&prediction
    if(CheckEmpty(res) || CheckEmpty(prediction_info_)){
      return;
    }
    //TODO icu&prediction数据播放

  }else if(CheckPlayMode() == PlayMode::kPredictionAndControl){ //prediction & control
    if(CheckEmpty(control_info_) || CheckEmpty(prediction_info_)){
      return;
    }
    control_end_++;
    prediction_index_++;
    if(!test_)
      PlayPredictionInfo();
    ui->lineEdit_hour->setText(QString::number(prediction_info_[prediction_index_].time.h));
    ui->lineEdit_min->setText(QString::number(prediction_info_[prediction_index_].time.m));
    ui->lineEdit_sec->setText(QString::number(prediction_info_[prediction_index_].time.s));
    ui->lineEdit_msec->setText(QString::number(prediction_info_[prediction_index_].time.ms));

    ui->lineEdit_ctrl_hour->setText(QString::number(control_info_[control_end_].time.h));
    ui->lineEdit_ctrl_min->setText(QString::number(control_info_[control_end_].time.m));
    ui->lineEdit_ctrl_sec->setText(QString::number(control_info_[control_end_].time.s));
    ui->lineEdit_ctrl_msec->setText(QString::number(control_info_[control_end_].time.ms));

    bool still = false;
    PlayControlInfo(false);
  }else if(CheckPlayMode() == PlayMode::kAll){ //icu&prediction & control
    if(CheckEmpty(res) || CheckEmpty(control_info_) || CheckEmpty(prediction_info_)){
      return;
    }
    //TODO icu&prediction&control数据播放
  }else{
    QMessageBox::information(this, "Error", "播放模式选择错误，请检查播放模式!");
    return;
  }
}

/* @brief: 时间槽函数
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::on_timeout()
{
  on_pushButton_next_clicked();
}

/* @brief: 加载地图
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::on_pushButton_loadmap_clicked()
{
  if(LoadMap()) {
    ui->globalview_icu->dispMap(map, min_utm_x, max_utm_x, min_utm_y, max_utm_y, false);
    UpdateCCUMap();
  }
}

void Widget::on_pushButton_loadfile_clicked(){
  if(OpenFile()){
    ui->pushButton_play->setEnabled(true);
    ui->pushButton_pre->setEnabled(true);
    ui->pushButton_next->setEnabled(true);
    ui->pushButton_reset->setEnabled(true);
    //    on_pushButton_reset_clicked();
  }
}

/* @brief: 图表切换到指定帧
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::on_pushButton_reset_clicked()
{
  //icu
  int num = ui->lineEdit_num->text().toInt() - 1;
  if(CheckPlayMode() == PlayMode::kICU){
    if(CheckEmpty(res)){
      return;
    }
    frame_num_ =  num;
//    PlayICUFrame();
  }else if(CheckPlayMode() == PlayMode::KPrediction){//prediction
    if(CheckEmpty(prediction_info_)){
      return;
    }
    prediction_index_ = num;
//    PlayPredictionInfo();
  }else if(CheckPlayMode() == PlayMode::kICUAndPrediction){ //icu&prediction
    if(CheckEmpty(res) || CheckEmpty(prediction_info_)){
      return;
    }
    //TODO icu&prediction数据播放

  }else if(CheckPlayMode() == PlayMode::kPredictionAndControl){ //prediction & control
    if(CheckEmpty(control_info_) || CheckEmpty(prediction_info_)){
      return;
    }
    sync_ctrl_index_ = num;
    prediction_index_ = num;
    ClearCtrlAllGraphData();
    if(!SearchSyncTime(PlayMode::kPredictionAndControl))
    {
      QMessageBox::information(this, "Error", "时间戳无法同步，检查文件内容！");
      return;
    }
  }else if(CheckPlayMode() == PlayMode::kAll){ //icu&prediction & control
    if(CheckEmpty(res) || CheckEmpty(control_info_) || CheckEmpty(prediction_info_)){
      return;
    }
    //TODO icu&prediction&control数据播放
  }
  on_pushButton_play_clicked();
}

/* @brief: 搜索特定时间的帧并显示
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::on_pushButton_search_clicked()
{
//  qDebug()<<"search!";
  int hour = ui->lineEdit_hour->text().toInt();
  int min = ui->lineEdit_min->text().toInt();
  int sec = ui->lineEdit_sec->text().toInt();
  int msec = ui->lineEdit_msec->text().toInt();
  if(CheckPlayMode() == PlayMode::kICU){ //icu
    for(int i = 0; i < res.size();i++)
      for(int j = 0; j < res[i].points.size(); j++){
        if(res[i].giu.time.h >= hour && res[i].giu.time.m >= min && res[i].giu.time.s >= sec && res[i].giu.time.ms >= msec){
          frame_num_ = i;
          PlayICUFrame();
          return;
        }
      }
  }else if(CheckPlayMode() == PlayMode::KPrediction || CheckPlayMode() == PlayMode::kPredictionAndControl){//prediction || prediction&&control
    if(hour == 0 && min == 0 && sec == 0 && msec == 0){
      for(int i = prediction_index_; i < prediction_info_.size(); ++i)
      {
        if(prediction_info_[i].is_impact == 1)
        {
          prediction_index_ = i;
          if(!test_)
            PlayPredictionInfo();
          return;
        }
      }
      for(int i = 0; i < prediction_index_; ++i)
      {
        if(prediction_info_[i].is_impact == 1)
        {
          prediction_index_ = i;
          if(!test_)
            PlayPredictionInfo();
          return;
        }
      }
    }
    else
    {
      for(int i = 0; i < prediction_info_.size(); ++i)
      {
        if(prediction_info_[i].time.h >= hour && prediction_info_[i].time.m >= min  &&
           prediction_info_[i].time.s >= sec && prediction_info_[i].time.ms >= msec)
        {
          prediction_index_ = i;
          if(!test_)
            PlayPredictionInfo();
          return;
        }
      }
    }
  }else if(CheckPlayMode() == PlayMode::kCtrl){ //control
    for(int i = 0; i < control_info_.size(); ++i)
    {
      if(control_info_[i].time.h >= hour && control_info_[i].time.m >= min  &&
         control_info_[i].time.s >= sec && control_info_[i].time.ms >= msec)
      {
        control_start_ = i;
        bool still = true;
        PlayControlInfo(true);
        return;
      }
    }
  }else if(CheckPlayMode() == PlayMode::kICUAndPrediction){ //icu&prediction
  }else if(CheckPlayMode() == PlayMode::kAll){ //icu&prediction & control
  }
  else{
    QMessageBox::information(this, "Error", "播放模式选择错误，请检查播放模式!");
    return;
  }
  QMessageBox::information(this, "Error", "未找到指定帧！");
}

/* @brief: 重设播放间隔
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::on_pushButton_set_clicked()
{
  fTimer_->setInterval(ui->interval->text().toInt());
}

/* @brief: 读入参数文件
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::on_pushButton_param_clicked()
{
  OpenParam();
}

/* @brief: 加载地图并保存信息
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return bool
 */
bool Widget::LoadMap()
{
  QString fileName[6] = {"F_0_0_0.lpx", "F_1_0_0.lpx", "left_path_left_curb.txt.sort", "left_path_right_curb.txt.sort",
                         "right_path_left_curb.txt.sort", "right_path_right_curb.txt.sort"};
  QString dlg = "选择一个文件夹";
  QString path = "/home/lei/docs";
  QString pathName = QFileDialog::getExistingDirectory(this, dlg, path);
  for (int i = 0; i < 6; ++i) {
    QFile inFile(pathName + '/' + fileName[i]);
    if(!inFile.exists())
    {
      QMessageBox::information(this, "Error", "地图文件不存在！");
      return false;
    }//infile exist
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::information(this, "Error", "文件读取错误！");
      return false;
    }//infile open
    qDebug() << "打开文件" << fileName[i] << "成功！";
    InputMap(inFile, i);
    map_in_ = true;
    inFile.close();
  } //for
  return true;
}

bool Widget::OpenFile(){

  QString dlg = "打开文件";
  QString path = "/home/lei/docs";
  QString filter="所有文件(*.*);;表格文件(*.csv);;日志文件(*.log)";

  QStringList str_path_list = QFileDialog::getOpenFileNames(this, dlg, path, filter);
  if(!(str_path_list.size() > 0)){
    QMessageBox::information(this, "Error", "未选择日志文件!");
    return false;
  }
  for(int i = 0; i < str_path_list.size(); i++){
    QString str_path = str_path_list[i];
    QFile file(str_path);
    //文件是否存在
    if(!file.exists())
    {
      QMessageBox::information(this, "Error", "日志文件不存在！");
      return false;
    }
    else if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::information(this, "Error", "文件读取错误！");
      return false;
    }
    else if(str_path.right(3) != "csv" && str_path.right(3) != "log" && str_path.right(3) != "lpx")
    {
      QMessageBox::information(this, "Error", "文件格式错误！");
      return false;
    }
    else if(str_path.indexOf("icu") != -1)//input icu
    {
      icu_info_in_ = true;
      ui->checkBox_icu->setEnabled(true);
      if(str_path.right(3) == "csv")
      {
        InputICUCsv(file);
      }
      else if(str_path.right(3) == "log")
      {
        InputICULog(file);
      }
    }
    else if(str_path.indexOf("result") != -1)//input prediction file
    {
      prediction_info_in_ = true;
      ui->checkBox_prediction->setEnabled(true);
      vehicle_box_in_ = false;
      InputCCUCsv(file);
    }
    else if(str_path.indexOf("box") != -1)
    {
      prediction_info_in_ = true;
      vehicle_box_in_ = true;
      car_.clear();
      InputCCUCsv(file);
    }
    else if(str_path.indexOf("front") != -1)
    {
      prediction_info_in_ = true;
      ui->checkBox_prediction->setEnabled(true);
      vehicle_box_in_ = false;
      InputCCUTempCsv(file);
      test_ = true;
    }
    else if(str_path.indexOf("ccu") != -1)//input control file
    {
      control_info_in_ = true;
      ui->checkBox_control->setEnabled(true);
      InputControlCsv(file);
    }
    else if(str_path.indexOf("F_") != -1)//input ccu map
    {
      InputMap(file,0);
    }
    //        inFiles.push_back(file);
    qDebug() << "打开文件" << str_path << "成功！";
  }
  sort(res.begin(),res.end(),
       [](const Frame &a, const Frame &b){
    return a.giu.time < b.giu.time;
  });
  sort(prediction_info_.begin(),prediction_info_.end(),
       [](const PredictionInfo &a, const PredictionInfo &b){
    return a.time < b.time;
  });
  sort(control_info_.begin(),control_info_.end(),
       [](const ControlInfo &a, const ControlInfo &b){
    return a.time < b.time;
  });
  return true;
}

/* @brief: 读入参数文件
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return bool
 */
bool Widget::OpenParam()
{
  QString dlg = "打开一个参数文件";
  QString path = "/home/ubuntu/LogShow用docs";
  QString filter="所有文件(*.*)";
  QString fileName = QFileDialog::getOpenFileName(this, dlg, path, filter);
  if(fileName.isEmpty()) return false;
  QFile inFile(fileName);
  if(!inFile.exists())
  {
    QMessageBox::information(this, "Error", "配置文件不存在！");
    return false;
  }
  if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QMessageBox::information(this, "Error", "文件读取错误！");
    return false;
  }

  InputParam(inFile);

  inFile.close();
  return true;
}

/* @brief: 解析地图文件，存入map中
 * @param [in]:  QFile &inFile, int i
 * @param [out]: NONE
 * @return 无
 */
void Widget::InputMap(QFile &inFile, int i)
{
  QByteArray rawData = inFile.readLine();
  QPointF point;
  while(rawData != "")
  {
    char a = ',';
    QList<QByteArray> ba = rawData.split(a);
    point.setX(ba[6].toDouble());
    point.setY(ba[5].toDouble());
    if(point.x() > max_utm_x) max_utm_x = point.x();
    if(point.x() < min_utm_x) min_utm_x = point.x();
    if(point.y() > max_utm_y) max_utm_y = point.y();
    if(point.y() < min_utm_y) min_utm_y = point.y();

    //        qDebug() << QString("%1").arg(max_utm_x, 0, 'f', 10) << QString("%1").arg(min_utm_x, 0, 'f', 10) << QString("%1").arg(max_utm_y, 0, 'f', 10) << QString("%1").arg(min_utm_y, 0, 'f', 10);
    //"F_0_0_0.lpx", "F_1_0_0.lpx", "left_path_left_curb.txt.sort", "left_path_right_curb.txt.sort",
    //"right_path_left_curb.txt.sort", "right_path_right_curb.txt.sort"};
    //路径文件

    if(i == 0 || i == 1){
      ccu_map_in_ = true;
      float head = ba[7].toDouble();
      float l_edge = ba[9].toDouble();
      float r_edge = ba[10].toDouble();
//      qDebug()<<QString::number(point.x(),'f',6)<<" "<<QString::number(point.y(),'f',6);
      if(point.x() > plot_max_x_) plot_max_x_ = point.x();
      if(point.x() < plot_min_x_) plot_min_x_ = point.x();
      if(point.y() > plot_max_y_) plot_max_y_ = point.y();
      if(point.y() < plot_min_y_) plot_min_y_ = point.y();
      ccu_map_.push_back(CCUMap(point,l_edge,r_edge,head));
    }
    map[i].push_back(point);
    rawData = inFile.readLine();
  }
  if(i == 0 || i ==1)
  {
    UpdateCCUMap();
  }
}

/* @brief: 解析配置文件
 * @param [in]:  QFile &inFile
 * @param [out]: NONE
 * @return 无
 */
void Widget::InputParam(QFile &inFile)
{
  QByteArray rawData = inFile.readLine();
  while(rawData != "")
  {
    char a = ' ';
    QList<QByteArray> ba = rawData.split(a);
    if (ba[0] == "front_lidar_gps_yaw") lidar_yaw = ba[2].toDouble();
    if (ba[0] == "front_lidar_gps_pitch") lidar_pitch = ba[2].toDouble();
    if (ba[0] == "front_lidar_gps_roll") lidar_roll = ba[2].toDouble();
    if (ba[0] == "front_lidar_gps_offset_x") lidar_offset_x = ba[2].toDouble();
    if (ba[0] == "front_lidar_gps_offset_y") lidar_offset_y = ba[2].toDouble();
    if (ba[0] == "front_lidar_gps_offset_z") lidar_offset_z = ba[2].toDouble();

    if (ba[0] == "left_bp_lidar_offset_x") left_bp_lidar_offset_x = ba[0].toDouble();
    if (ba[0] == "right_bp_lidar_offset_x") right_bp_lidar_offset_x = ba[2].toDouble();
    if (ba[0] == "to_ego_length") to_ego_length = ba[2].toDouble();
    if (ba[0] == "right_chain_y") right_chain_y = ba[2].toDouble();

    if(ba[0] == "vehicle_length") vehicle_length = customSplit(ba[2]);
    if(ba[0] == "vehicle_width") vehicle_width = customSplit(ba[2]);
    if(ba[0] == "vehicle_height") vehicle_height = customSplit(ba[2]);
    if(ba[0] == "vehicle_centroid_head_length") vehicle_centroid_head_length = customSplit(ba[2]);
    if(ba[0] == "Rotate_Lidar_Dist_Long_Deviation")  Rotate_Lidar_Dist_Long_Deviation = customSplit(ba[2]);
    if(ba[0] == "Rotate_Lidar_Dist_Lat_Deviation") Rotate_Lidar_Dist_Lat_Deviation = customSplit(ba[2]);
    trans_bp_imu = calcTransformMatrix(lidar_offset_x, lidar_offset_y, lidar_offset_z, lidar_yaw, lidar_pitch, lidar_roll);
    rawData = inFile.readLine();
  }
}

/* @brief: 解析csv文件，存入res中
 * @param [in]:  QFile &inFile
 * @param [out]: NONE
 * @return 无
 */
void Widget::InputICUCsv(QFile &inFile)
{
  res.clear();

  Frame frame;
  QByteArray rawData = inFile.readLine();
  QByteArray datetimestamp;
  QByteArray timestamp;
  QList<QByteArray> times;
  QList<QByteArray> xyz;
  Obj obj;
  int type, num, flag, frameN = 0;
  char a;

  rawData = inFile.readLine();
  //    int count = 0;
  while(rawData != "")
  {
    //        count++;
    a = ',';

    QList<QByteArray> ba = rawData.split(a);
    //        qDebug()<<count<<" "<<ba.size();
    if(ba.size() < 14) continue;
    type = ba[13].toShort();
    num = ba[14].toInt();
    if(ba[0].toInt() != frameN)
    {
      if(frameN != 0)
      {
        //添加汽车信息
        frame.car[0].xyz(0) = left_bp_lidar_offset_x;
        frame.car[0].xyz(1) = 0;
        frame.car[1].xyz(0) = right_bp_lidar_offset_x;
        frame.car[1].xyz(1) = 0;
        frame.car[2].xyz(0) = right_bp_lidar_offset_x;
        frame.car[2].xyz(1) = - lidar_offset_y - to_ego_length - right_chain_y;
        frame.car[3].xyz(0) = left_bp_lidar_offset_x;
        frame.car[3].xyz(1) = - lidar_offset_y - to_ego_length - right_chain_y;
        frame.car[4].xyz(0) = left_bp_lidar_offset_x;
        frame.car[4].xyz(1) = 0;
        for(int i = 0; i<5; i++)
        {
          frame.car[i].xyz(3) = 1;
          frame.car[i].utm = frame.trans_utm_imu * trans_bp_imu * frame.car[i].xyz;
          //          std::cout << frameN << ":" << frame.car[i].xyz << "/" << frame.car[i].utm << std::endl;
        }

        res.push_back(frame);
      }
      frame.points.clear();

      if(type == 19) datetimestamp = ba[10]; else datetimestamp = ba[11];
      frame.giu.time = TimeSplit(datetimestamp, false);

      frame.giu.gps_data.longitude = ba[3].toDouble();
      frame.giu.gps_data.latitude = ba[2].toDouble();
      frame.giu.gps_data.heading = ba[4].toDouble();
      frame.giu.gps_data.altitude = ba[5].toDouble();
      frame.giu.imu_data.yaw = ba[4].toDouble() * (-1);
      frame.giu.imu_data.pitch = ba[7].toDouble();
      frame.giu.imu_data.roll = ba[8].toDouble();

      //            qDebug() << "input gnss::"<< timestamp <<"," << frame.giu.gps_data.longitude <<"," << frame.giu.gps_data.latitude
      //                     <<"," << frame.giu.gps_data.heading <<"," << frame.giu.imu_data.pitch <<"," << frame.giu.imu_data.roll ;

      convertWGS84ToUTM(frame.giu.gps_data.longitude, frame.giu.gps_data.latitude, frame.giu.gps_data.altitude,
                        &frame.giu.utm_data.utm_x, &frame.giu.utm_data.utm_y, &frame.giu.utm_data.utm_z);
      frame.trans_utm_imu = calcTransformMatrix(frame.giu.utm_data.utm_x, frame.giu.utm_data.utm_y, frame.giu.utm_data.utm_z,
                                                frame.giu.imu_data.yaw, frame.giu.imu_data.pitch, frame.giu.imu_data.roll);
      frameN++;
    }
    if(type == 19) obj.type = 0; else obj.type = 1;
    //读入角点
    for(int i = 0; i<num; i++)
    {
      a = '/';
      xyz = ba[i+21].split(a);
      obj.xyz(0) = xyz[0].toDouble();
      obj.xyz(1) = xyz[1].toDouble();
      obj.xyz(2) = xyz[2].toDouble();
      obj.xyz(3) = 1;

      //计算utm坐标
      obj.utm = frame.trans_utm_imu * trans_bp_imu * obj.xyz;
      frame.points.push_back(obj);
    }


    rawData = inFile.readLine();
  }
  res.push_back(frame);
  return;
}

/* @brief: 解析log文件，存入res中
 * @param [in]:  QFile &inFile
 * @param [out]: NONE
 * @return 无
 */
void Widget::InputICULog(QFile &inFile)
{
  res.clear();

  int k = group_button_->checkedId();
  //qDebug() << k << "front";
  QByteArray rawData = inFile.readLine();
  Frame frame;
  Obj obj;
  while(rawData != "")
  {
    if((k == 0 && rawData.contains("icu_status_buff"))
       || (k == 1 && rawData.contains("BACK FUSION OBJECTS SIZE")))
      //if(rawData.contains("icu_status_buff")) //分帧符
    {
      res.push_back(frame);
      frame.points.clear();

      if(k == 0)
      {
        frame.car[0].xyz(0) = left_bp_lidar_offset_x;
        frame.car[0].xyz(1) = 0;
        frame.car[1].xyz(0) = right_bp_lidar_offset_x;
        frame.car[1].xyz(1) = 0;
        frame.car[2].xyz(0) = right_bp_lidar_offset_x;
        frame.car[2].xyz(1) = - lidar_offset_y - to_ego_length - right_chain_y;
        frame.car[3].xyz(0) = left_bp_lidar_offset_x;
        frame.car[3].xyz(1) = - lidar_offset_y - to_ego_length - right_chain_y;
        frame.car[4] = frame.car[0];
      }else
      {
        frame.car[0].xyz(0) = left_bp_lidar_offset_x;
        frame.car[0].xyz(1) = lidar_offset_y + right_chain_y;
        frame.car[1].xyz(0) = right_bp_lidar_offset_x;
        frame.car[1].xyz(1) = lidar_offset_y + right_chain_y;
        frame.car[2].xyz(0) = right_bp_lidar_offset_x;
        frame.car[2].xyz(1) = - to_ego_length;
        frame.car[3].xyz(0) = left_bp_lidar_offset_x;
        frame.car[3].xyz(1) = - to_ego_length;
        frame.car[4] = frame.car[0];
        //                .xyz(0) = left_bp_lidar_offset_x;
        //                frame.car[4].xyz(1) = lidar_offset_y + 3.75;
      }
      for(int i = 0; i<5; i++)
      {
        frame.car[i].xyz(3) = 1;
        frame.car[i].utm = frame.trans_utm_imu * trans_bp_imu * frame.car[i].xyz;
        //std::cout << frameN << ":" << frame.car[i].xyz << "/" << frame.car[i].utm << std::endl;
      }
    } else
      if(rawData.contains("input gnss::"))
      {
        rawData.remove(0, 12);
        char a = ',';
        QList<QByteArray> ba = rawData.split(a);
        frame.giu.gps_data.longitude = ba[1].toDouble();
        frame.giu.gps_data.latitude = ba[2].toDouble();
        frame.giu.gps_data.heading = ba[3].toDouble();
        frame.giu.imu_data.pitch = ba[4].toDouble();
        frame.giu.imu_data.roll = ba[5].toDouble();
      }
    //temp
    /*****************************/
    char a = ' ';
    QByteArray temp = rawData.mid(65);
    //qDebug() << temp;
    QList<QByteArray> ba = temp.split(a);
    if(ba.size() == 3)
    {
      obj.xyz(0) = ba[0].toDouble();
      obj.xyz(1) = ba[1].toDouble();
      obj.type = 1;
      frame.points.push_back(obj);
    }
    /*****************************/
    if(rawData.contains("corner_point") || (rawData.contains("CONTOUR POINTS") && !rawData.contains("SIZE")))
    {
      if(rawData.contains("corner_point"))    //障碍物
      {
        rawData = rawData.chopped(1);
        char a = ' ';
        QList<QByteArray> ba = rawData.split(a);
        obj.xyz(0) = ba[ba.size() - 3].toDouble();
        obj.xyz(1) = ba[ba.size() - 2].toDouble();
        obj.type = 1;
        frame.points.push_back(obj);
      } else
        if(rawData.contains("CONTOUR POINTS") && !rawData.contains("SIZE"))  //边界点
        {
          rawData = rawData.chopped(1);
          char a = ' ';
          QList<QByteArray> ba;

          if(k == 1)
          {
            for(int i = 70; i<rawData.size(); i++)
            {
              if(rawData[i] == 'C' && rawData[i-1] != ' ')
              {
                rawData.insert(i, ' ');
                i++;
                //qDebug() << rawData;
              }
            }
            ba = rawData.split(a);
            obj.xyz(0) = ba[ba.size() - 11].toDouble();
            obj.xyz(1) = ba[ba.size() - 6].toDouble();
          } else
          {
            ba = rawData.split(a);
            obj.xyz(0) = ba[ba.size() - 6].toDouble();
            obj.xyz(1) = ba[ba.size() - 1].toDouble();
          }
          //qDebug() << obj.xyz(0) << obj.xyz(1);
          obj.type = 0;
          frame.points.push_back(obj);
        }
      if(frame.points.size() == 1)
      {
        frame.giu.time.y = (rawData[1] - '0') * 1000 + (rawData[2] - '0') * 100 + (rawData[3] - '0') * 10 + rawData[4] - '0';
        frame.giu.time.mon = (rawData[6] - '0') * 10 + rawData[7] - '0';
        frame.giu.time.d = (rawData[9] - '0') * 10 + rawData[10] - '0';
        frame.giu.time.h = (rawData[12] - '0') * 10 + rawData[13] - '0';
        frame.giu.time.m = (rawData[15] - '0') * 10 + rawData[16] - '0';
        frame.giu.time.s = (rawData[18] - '0') * 10 + rawData[19] - '0';
        frame.giu.time.ms = (rawData[21] - '0') * 100000 + (rawData[22] - '0') * 10000 + (rawData[23] - '0') * 1000
            +(rawData[24] - '0') * 100 + (rawData[25] - '0') * 10 + rawData[26] - '0';
      }
    }
    rawData = inFile.readLine();
  }
  res.push_back(frame);
  return;
}

void Widget::InputCCUCsv(QFile &infile){

  prediction_index_ = 0;
  PredictionInfo info;
  QByteArray raw_data = infile.readLine();
  char a;
  if(!vehicle_box_in_){
    while(raw_data != ""){
      raw_data = infile.readLine();
      a = ',';
      QList<QByteArray> ba = raw_data.split(a);
      if(ba.size() < 84) continue;
      TimeStamp time = TimeSplit(ba[0],true);
      time.stamp = ba[1].toDouble();
      info.time = time;
      QPointF point(ba[2].toDouble(), ba[3].toDouble()); //current utm
      info.current_utm = point;
      info.current_head = ba[5].toDouble();
      point = QPointF(ba[12].toDouble(), ba[13].toDouble());
      info.key_utm = point;
      info.key_head = ba[14].toDouble();
      //读取车身数据15~62
      for(int i = 0; i < 48; ++i){
        QList<QByteArray> raw_car = ba[15+i].split('/');
        for(int j = 0; j < 5; ++j){
          //key point coordinate
          double x = raw_car[1+2*j].toDouble();
          double y = raw_car[2+2*j].toDouble();
          double head = info.key_head * M_PI / 180.0;
          info.car[i][j] = QPointF(info.key_utm.x() + x * cos(head) + y * sin(head),
                                   info.key_utm.y() - x * sin(head) + y * cos(head));
        }
      }
      info.is_impact = ba[63].toInt() ? true : false;
      //读取障碍物数据
      Obstacle obs;
      TimeStamp obs_time = TimeConvert(ba[72].toLongLong());
      obs.id = ba[73].toInt();
      obs.obs2route_dis = ba[74].toDouble();
      obs.type = ba[75].toInt();
      obs.head = ba[76].toFloat();
      obs.length = ba[77].toFloat();
      obs.width = ba[78].toFloat();
      obs.height = ba[79].toFloat();
      obs.key_id = ba[82].toInt();
      obs.num = ba[83].toInt();
      int num = ba[83].toInt();
      for(int i = 0; i < num; ++i){
        QList<QByteArray> raw_vertex = ba[84+i].split('/');
        double x = raw_vertex[0].toDouble();
        double y = raw_vertex[1].toDouble();
        double head = info.key_head * M_PI / 180.0;
        QPointF vertex(info.key_utm.x() + x * cos(head) + y * sin(head),
                       info.key_utm.y() - x * sin(head) + y * cos(head));
        obs.vertex.push_back(vertex);
      }
      info.obs = obs;
      prediction_info_.push_back(info);
    }
  }else{
    while(raw_data != ""){
      QVector<QPointF> car;
      raw_data = infile.readLine();
      a = ',';
      QList<QByteArray> ba = raw_data.split(a);
      if(ba.size() < 16) continue;
      QPointF point;
      point = QPointF(ba[1].toDouble(), ba[2].toDouble());
      car.push_back(point);
      point = QPointF(ba[8].toDouble(), ba[9].toDouble());
      car.push_back(point);
      point = QPointF(ba[10].toDouble(), ba[11].toDouble());
      car.push_back(point);
      point = QPointF(ba[14].toDouble(), ba[15].toDouble());
      car.push_back(point);
      point = QPoint(ba[12].toDouble(), ba[13].toDouble());
      car.push_back(point);
//      for(int i = 0; i < 5; ++i)
//      {
//        qDebug()<<QString::number(car[i].x(),'f',6)<<"  "<<QString::number(car[i].y(), 'f', 6);
//      }
      car_.push_back(car);
      car.clear();
    }
  }
}

void Widget::InputCCUTempCsv(QFile &infile){
  prediction_index_ = 0;
  PredictionInfo info;
  QByteArray raw_data = infile.readLine();
  char a;
  while(raw_data != ""){
    raw_data = infile.readLine();
    a = ',';
    QList<QByteArray> ba = raw_data.split(a);
    if(ba.size() < 84) continue;
    TimeStamp time = TimeSplit(ba[0],true);
    time.stamp = ba[1].toDouble();
    info.time = time;
    prediction_info_.push_back(info);
  }
}

void Widget::InputControlCsv(QFile &infile){

  ControlInfo info;
  QByteArray raw_data = infile.readLine();
  char a;

  while(raw_data != ""){

    raw_data = infile.readLine();
    a = ',';
    QList<QByteArray> ba = raw_data.split(a);
    if(ba.size() < 84) continue;
    TimeStamp time = TimeSplit(ba[0],true);
    info.time = time;
    info.info[KGpsHeading] = ba[3].toDouble();
    info.info[kGpsSpeed] = ba[4].toDouble();
    info.info[kRestDis]= ba[27].toDouble();
    info.info[kBiaDistance] = ba[31].toDouble();
    info.info[kPreviewDis] = ba[32].toDouble();
    info.info[kCurvePrepoint] = ba[33].toDouble();
    info.info[kAngleDeviationAtPrepoint] = ba[34].toDouble();
    info.info[kHeadingAngleDeviationAtPrepoint] = ba[35].toDouble();
    info.info[kRecordMAngleHead] = ba[36].toDouble();
    info.info[kDevAngleCtl] = ba[37].toDouble();
    info.info[kDevHeadingCtl] = ba[38].toDouble();
    info.info[kFrontWheelAngle] = ba[39].toDouble();
    info.info[kWireStatusFrontWheelAngle] = ba[40].toDouble();
    info.info[kDesiredSpeed] = ba[41].toDouble();
    info.info[kDesiredLidarSpeed] = ba[42].toDouble();
    info.info[kSpeedPreview] = ba[43].toDouble();
    info.info[kVehicleVelocity] = ba[44].toDouble();
    info.info[kPitchFilterRecord] = ba[51].toDouble();
    info.info[kSlopeVehicle] = ba[53].toDouble();
    info.info[kSlopePreview] = ba[55].toDouble();
    info.info[kSpeedCtlState] = ba[56].toDouble();
    info.info[kSpdE] = ba[57].toDouble();
    info.info[kSpdEc] = ba[58].toDouble();
    info.info[kBaseCtlValue] = ba[59].toDouble();
    info.info[kDeltaCtlValue] = ba[60].toDouble();
    info.info[kTotalCtlValue] = ba[62].toDouble();
    info.info[kShiftCtrl] = ba[69].toDouble();
    info.info[kWireStatusGearShift] = ba[71].toDouble();
    info.info[kThrottlePedal] = ba[72].toDouble();
    info.info[kWireStatusAccActuatePercent] = ba[73].toDouble();
    info.info[kBrakePedal] = ba[75].toDouble();
    info.info[kWireStatusElectricBrakeActuatePercent] = ba[76].toDouble();
    info.info[kHydraulicBrake] = ba[78].toDouble();
    info.info[kWireStatusMechanicalBrakeActuatePercent] = ba[79].toDouble();
    info.info[kWireStatusFrontBrakePercent] = ba[80].toDouble();
    info.info[kObjectStopFlag] = ba[105].toDouble();
    info.info[kDisPath] = ba[107].toDouble();
    info.info[kPathWheelAnglePre] = ba[120].toDouble();
    info.info[kOrder2KalmanSpeed] = ba[124].toDouble();
    info.info[klatBiaDisPre] = ba[140].toDouble();
    control_info_.push_back(info);
  }
  control_end_ = control_info_.size();
  ui->lineEdit_end->setText(QString::number(control_end_));
}

/* @brief: 显示帧信息
 * @param [in]:  NONE
 * @param [out]: NONE
 * @return 无
 */
void Widget::DispICUInfo()
{
  ui->lineEdit_framecount->setText(QString::number(res.size()));
  ui->lineEdit_index->setText( QString::number(frame_num_+1));
  ui->lineEdit_obscount->setText( QString::number(res[frame_num_].points.size()));
  if(res[frame_num_].points.size() > 0)
  {
    ui->lineEdit_hour->setText(QString::number(res[frame_num_].giu.time.h));
    ui->lineEdit_min->setText(QString::number(res[frame_num_].giu.time.m));
    ui->lineEdit_sec->setText(QString::number(res[frame_num_].giu.time.s));
    ui->lineEdit_msec->setText(QString::number(res[frame_num_].giu.time.ms));
  }
}

void Widget::DispPredictionInfo(){
  ui->lineEdit_framecount->setText(QString::number(prediction_info_.size()));
  ui->lineEdit_obscount->setText(QString::number(prediction_info_[prediction_index_].obs.num));
  ui->lineEdit_index->setText(QString::number(prediction_index_));
  ui->lineEdit_hour->setText(QString::number(prediction_info_[prediction_index_].time.h));
  ui->lineEdit_min->setText(QString::number(prediction_info_[prediction_index_].time.m));
  ui->lineEdit_sec->setText(QString::number(prediction_info_[prediction_index_].time.s));
  ui->lineEdit_msec->setText(QString::number(prediction_info_[prediction_index_].time.ms));
}

double Widget::customSplit(QByteArray ba){
  char a = '#';
  QList<QByteArray> baa = ba.split(a);
  return baa[0].toDouble();
}

TimeStamp Widget::TimeSplit(QByteArray ba, bool CCU)
{
  QList<QByteArray> temp;
  QByteArray temp_2;
  TimeStamp time;
  //CCU 2022-09-17-10:25:44.014
  if(CCU){
    temp = ba.split('-');
    time.y = temp[0].toInt();
    time.mon = temp[1].toShort();
    time.d = temp[2].toShort();
    temp = temp[3].split(':');
    time.h = temp[0].toShort();
    time.m = temp[1].toShort();
    temp = temp[2].split('.');
    time.s = temp[0].toShort();
    time.ms = temp[1].toInt();
  }
  //ICU 2022-7-21 17:52:53:790
  else{
    temp = ba.split(' ');
    temp_2 = temp[1];
    temp = temp[0].split('-');
    time.y = temp[0].toInt();
    time.mon = temp[1].toShort();
    time.d = temp[2].toShort();
    temp = temp_2.split(':');
    time.h = temp[0].toShort();
    time.m = temp[1].toShort();
    time.s = temp[2].toShort();
    time.ms = temp[3].toInt();
  }
  return time;
}

TimeStamp Widget::TimeConvert(long long stamp)
{
  TimeStamp time_stamp;
  int ms = stamp % 1000;//取毫秒
  time_t tick = (time_t)(stamp/1000);//转换时间
  struct tm tm;
  char s[40];
  tm = *localtime(&tick);
  time_stamp = TimeStamp(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
  return time_stamp;
}

void Widget::on_horizontalSlider_local_valueChanged(int value)
{
  ui->objSizeLocal->setText(QString::number(value));
  ui->chartview->setPointSizeObj(value);
}

void Widget::on_horizontalSlider_global_obj_valueChanged(int value)
{
  ui->objSizeGlobal->setText(QString::number(value));
  ui->globalview_icu->setPointSizeObj(value);
}

//add by lmz
//递归遍历函数
QStringList Widget::recursiveAllFiles(QString &strFolder,QStringList &strList)
{
  QDir dir(strFolder);
  QFileInfoList listInfo =dir.entryInfoList(QDir::Dirs | QDir::Files| QDir::NoDotAndDotDot);

  foreach (QFileInfo fileInfo, listInfo)
  {
    if(fileInfo.isDir())
    {
      QString dirPath = fileInfo.absoluteFilePath();
      recursiveAllFiles(dirPath,strList);
    }
    else
    {
      strList.push_back(fileInfo.absoluteFilePath());
    }
  }
  return strList;
}

void Widget::on_pushButton_inputdir_clicked()
{
  QString dirpath = QFileDialog::getExistingDirectory(this,"选择目录","./",QFileDialog::ShowDirsOnly);
  ui->lineEdit_dir->setText(dirpath);
}

//自动化解析 lmz
void Widget::on_autoAnalyse()
{
  //遍历输出
  QStringList filelist;
  //    QString dirPath = "/media/lmz/tage/log_analysis/public/logs";
  //    QString dirPath = "/media/nvidia/perception/log_analysis/public/logs";
  QString dirPath = ui->lineEdit_dir->text();
  qDebug() << dirPath;
  filelist = recursiveAllFiles(dirPath,filelist);

  //遍历所选文件夹下所有文件
  QString fwritestr = "";
  QString bwritestr = "";
  QString ffileDir = "";
  QString bfileDir = "";
  int flag = 0;
  QString fileName;
  QList<QString> list;
  while(flag < filelist.count()){
    fileName = filelist.at(flag);
    //        qDebug() << fileName;
    if(fileName.isEmpty()) return;
    list = fileName.split('/');

    //筛查前向日志
    while(list.contains("front_perception_logs") && list.at(list.indexOf("front_perception_logs")+1) == "logs" ){
      ffileDir = fileName.mid(0,fileName.indexOf("front_perception_logs")+21);
      QString str = analyse_sum(fileName);
      if(str != "") {
        fwritestr.append(str);
        qDebug() << "打开文件" << fileName << "成功！";
      }else if(str == ""){
        qDebug() << "文件" << fileName << "不符合时间要求，略过分析";
      }
      flag ++;
      if(flag >= filelist.count()) break;
      fileName = filelist.at(flag);
      if(fileName.isEmpty()) return;
      list = fileName.split('/');
    }
    //解析
    if(fwritestr != ""){
      qDebug() << "拼接时间线完成，开始解析:";
      wricornerstr.append(ffileDir+"\n");
      qDebug() << ffileDir;
      //            Savelog_File(fwritestr);
      //            qDebug() << "保存完成";
      analyse_log(fwritestr);
      fwritestr = "";
    }

    //筛查后向日志
    while(list.contains("back_perception_logs") && list.at(list.indexOf("back_perception_logs")+1) == "logs" ){ //用两个while判断不同的矿区车辆属性，不再构造结构体
      bfileDir = fileName.mid(0,fileName.indexOf("back_perception_logs")+21);
      QString str = analyse_sum(fileName); //拼接有效文件
      if(str != "") {
        bwritestr.append(str);
        qDebug() << "打开文件" << fileName << "成功！";
      }else if(str == ""){
        qDebug() << "文件" << fileName << "不符合时间要求，略过分析";
      }
      flag ++;
      if(flag >= filelist.count()) break;
      fileName = filelist.at(flag);
      if(fileName.isEmpty()) return;
      list = fileName.split('/');
    }
    //解析
    if(bwritestr != ""){
      qDebug() << "拼接时间线完成，开始解析:";
      wricornerstr.append(bfileDir+"\n");
      qDebug() << bfileDir;
      //        Savelog_File(fwritestr);
      //        qDebug() << "保存完成";
      analyse_log(bwritestr);
      bwritestr = "";
    }

    flag ++;

  }

  Savecornercsv_File();
  qDebug() << "解析完成";
  QString message_content = "请到"+qApp->applicationDirPath()+"目录下查看解析结果";
  QMessageBox::information(this, tr("解析结果生成提示"),
                           message_content, QMessageBox::Ok);
  wricornerstr = "";
}

//拼接文件内容(拼接时间线)
QString Widget::analyse_sum(QString &fileName)
{
  QFileInfo  fileInfo(fileName);

  QFile inFile(fileName);
  if(!inFile.exists()) return "";
  if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) return "";
  QString strsum = "";
  //解析文件
  QByteArray rawData = inFile.readLine();
  if(Checktime(rawData,fileName) != 1) return ""; //默认只判定第一行，大大减少分析时间
  while(rawData != "")
  {
    strsum.append(rawData);
    rawData = inFile.readLine();
  }
  inFile.close();
  return strsum;
}

//解析log日志
void Widget::analyse_log(QString &filesum)
{
  QList<QString> filecontentlist = filesum.split("\n");
  filecontentlist << "";
  //解析文件
  //    QByteArray rawData = inFile.readLine();

  int cornerexist = 0;
  int corner_framenum = 0;
  QString obstacles_start = "";
  QString obstacles_end = "";
  QString obstacles_newstart = "";
  long long int minute;
  long long int second;
  long long int msecond;
  long long int ob_starttime = 0;
  long long int ob_endtime = 0;
  long long int ob_newstarttime = 0;
  long long int time_interval = Strtonum(ui->interval_2->text()) * 1000000;

  for(int i=0;i<filecontentlist.count();i++)
  {
    QString rawData = filecontentlist.at(i);
    if(rawData == "" && corner_framenum != 0){
      QString aa = "";
      wricornerstr.append(aa+","+obstacles_start+","+obstacles_end+","+"此时间段共有"+QString::number(corner_framenum)+"帧出现障碍物"+"\n");
      corner_framenum = 0;
    }else{
      if(rawData.contains("0micu_status_buff") || rawData.contains("BACK FUSION OBJECTS SIZE")) //分帧符
      {
        if(cornerexist == 1){
          corner_framenum = corner_framenum + 1;
        }
        cornerexist = 0;

      } else
        if(rawData.contains("corner_point"))    //障碍物 用逻辑抓出出现障碍物的时间段
        {
          minute = Strtonum(rawData.mid(15,2));
          second = Strtonum(rawData.mid(18,2));
          msecond = Strtonum(rawData.mid(21,6));
          if(ob_starttime == 0){
            obstacles_start = rawData.left(28);
            ob_starttime = minute * 60000000 + second * 1000000 + msecond;
            obstacles_end = rawData.left(28);
            ob_endtime = minute * 60000000 + second * 1000000 + msecond;

          }else{
            ob_newstarttime = minute * 60000000 + second * 1000000 + msecond;
            //                    qDebug() << "ob_newstarttime:" << ob_newstarttime;
            if(ob_newstarttime - ob_endtime < time_interval){ //间隔判定 ！！！！
              obstacles_end = rawData.left(28);
              ob_endtime = minute * 60000000 + second * 1000000 + msecond;
              //                        qDebug() << "<ob_endtime:" << ob_endtime;
            }else{
              QString aa = "";
              wricornerstr.append(aa+","+obstacles_start+","+obstacles_end+","+"此时间段共有"+QString::number(corner_framenum)+"帧出现障碍物"+"\n");
              obstacles_start = rawData.left(28);
              ob_starttime = minute * 60000000 + second * 1000000 + msecond;
              obstacles_end = rawData.left(28); //防止连续多帧长跨度
              ob_endtime = minute * 60000000 + second * 1000000 + msecond;
              //                        qDebug() << ">ob_endtime:" << ob_endtime;
              corner_framenum = 0;
            }
          }
          cornerexist = 1;
        }
    }
  }
}

//get datatime
QString Widget::Gettime()
{
  QDateTime dateTime(QDateTime::currentDateTime());
  QString datatime = dateTime.addDays(-2).toString("yyyy-MM-dd");
  return datatime;
}

//planning time
int Widget::Checktime(QByteArray &rawData,QString &fileName)
{
  //time filter

  QList<QString> list = fileName.split('/');
  QString datatime;
  if(list.contains("front_perception_logs")){
    datatime = list.at(list.indexOf("front_perception_logs")-3);

  }else{
    datatime = list.at(list.indexOf("back_perception_logs")-3);

  }

  int year = Strtonum(datatime.mid(0,4));
  int month = Strtonum(datatime.mid(5,2));
  int day = Strtonum(datatime.mid(8,2));

  int rawyear = Strtonum(rawData.mid(1,4));
  int rawmonth = Strtonum(rawData.mid(6,2));
  int rawday = Strtonum(rawData.mid(9,2));

  if(year == rawyear && month == rawmonth && day == rawday){
    filetime = datatime;
    return 1;
  }else{
    return 0;
  }
}

//str to number
float Widget::Strtonum(QString str)
{
  //    bool ok = false;
  //    return str.toInt(&ok, 10);
  return str.toFloat();
}

//打印输出 出现障碍物的时间段
void Widget::Savecornercsv_File()
{
  QString datatime = filetime;
  QString csvFileName1 = qApp->applicationDirPath()+"/";
  QString csvFileName2 = "_icu_cornerexist_"+ui->interval_2->text()+"秒间隔.csv";
  QString csvFileName = csvFileName1 + datatime + csvFileName2;
  QFile file(csvFileName);
  if(!file.exists())
  {
    file.open(QIODevice::WriteOnly);
    QTextStream csvOutPut(&file);
    QString str ="属性,开始时间,结束时间,状况\n";
    csvOutPut << str.toUtf8();
    file.close();
  }
  if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    qDebug() << "0000000000000000000000";
  }
  else
  {
    QTextStream csvOutPut(&file);

    csvOutPut << wricornerstr.toUtf8();

    file.flush();

    file.close();
  }


}

//打印输出 拼接好的时间线
void Widget::Savelog_File(QString &filecontent){
  QString datatime = filetime;
  QString csvFileName1 = qApp->applicationDirPath();
  QString csvFileName2 = "_icu_cornerexist_"+ui->interval_2->text()+"秒间隔.log";
  QString csvFileName = csvFileName1 + datatime + csvFileName2;
  QFile file(csvFileName);
  if(!file.exists())
  {
    file.open(QIODevice::WriteOnly);
    QTextStream csvOutPut(&file);
    QString str ="aaaaa\n";
    csvOutPut << str.toUtf8();
    file.close();
  }
  if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    qDebug() << "0000000000000000000000";
  }
  else
  {
    QTextStream csvOutPut(&file);
    csvOutPut << filecontent.toUtf8();
    file.flush();
    file.close();
  }
}

void Widget::on_pushButton_analysis_clicked()
{

  //    QProcess::startDetached("./loaddata.sh");

  on_autoAnalyse();
}

//add by lmz

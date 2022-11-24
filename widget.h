#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QChartView>
#include <QtCharts>
#include <QDialog>
#include <QDebug>
#include <QTimer>
#include "convert.h"
#include "qcustomplot.h"
//#include "qcustomtooltip.h"
#include "chartview.h"
#include "MultiSelectComboBox.h"

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

extern bool  mapIn;

class Widget : public QWidget
{
  Q_OBJECT

public:
  Widget(QWidget *parent = nullptr);
  ~Widget();
protected:
  virtual void keyPressEvent(QKeyEvent *k) override;
private slots:

  void MousePress(QMouseEvent * event);
  void MouseWheel();
  void MouseMove(QMouseEvent * event);
  void SelectionChanged();
  void ComboBoxHorizontalSelectionChanged();
  void ComboBoxVerticalSelectionChanged();
  void ComboBoxSelectionChanged();
  void ContextMenuRequest(QPoint pos);
  void ShowGraph();
  void HideGraph();
  void ClearCtrlAllGraphData();

  void on_pushButton_loadmap_clicked();          //加载地图
  void on_pushButton_loadfile_clicked();
  void on_pushButton_param_clicked();    //设置参数

  void on_pushButton_search_clicked();     //查找指定时间的图像
  void on_pushButton_set_clicked();           //设置播放时间间隔
  void on_pushButton_play_clicked();
  void on_pushButton_pause_clicked();
  void on_pushButton_pre_clicked();          //显示上一帧
  void on_pushButton_next_clicked();          //显示下一帧
  void on_pushButton_reset_clicked();         //复位

  void on_timeout();                                              //时间槽

  void on_horizontalSlider_local_valueChanged(int value);         //设置点大小_local
  void on_horizontalSlider_global_obj_valueChanged(int value);    //设置点大小_globa


  //自动解析：add by lmz
  //解析并输出障碍物出现时间
  void on_pushButton_analysis_clicked();
  void on_pushButton_inputdir_clicked();

  QStringList recursiveAllFiles(QString &strFolder,QStringList &strList); //递归获取日志

  void analyse_log(QString &filesum);
  QString analyse_sum(QString &fileName); //拼接时间线
  int Checktime(QByteArray &rawData,QString &fileName); //筛选合格文件
  float Strtonum(QString str);
  QString Gettime();

  void Savecornercsv_File();
  void Savelog_File(QString &filecontent);
  void on_autoAnalyse(); //前置筛选工作
  //add by lmz

private:
  template <typename T>
  bool CheckEmpty(T t);
  /*
   *  0 icu 1 prediction 2 control
   *  3 icu&prediction 4 prediction&control
   *  5 icu&prediction&control
   *  -1 !!
   */
  int CheckPlayMode();
  int FindTime(QString time);


  void TimerInit();
  void WidgetInit();
  void UIInit();
  void ComboBoxInit();

  void PlayICUFrame();
  void PlayPredictionInfo();
  void PlayControlInfo(bool still);

  bool LoadMap(); //加载地图
  void InputMap(QFile &inFile, int i);    //解析地图文件
  void InputParam(QFile &inFile);     //解析参数文件

  bool OpenParam();    //打开参数文件
  void InputICUCsv(QFile &inFile);    //解析csv文件
  void InputICULog(QFile &inFile);    //解析log文件
  void DispICUInfo();    //显示指定帧信息

/*测试时间戳同步用临时函数*/
  void InputCCUTempCsv(QFile &inFile);

  bool OpenFile();
  void InputCCUCsv(QFile &inFile);

  void InputControlCsv(QFile &infile);

  void UpdateCCUMap();
  void PlotScatters(QVector<double> x, QVector<double> y,Qt::GlobalColor color, int index, bool add, int shape,int shape_size, bool prediction);
  void PlotScatters(QVector<QPointF> point,Qt::GlobalColor color, int index,bool add,int shape, int shape_size, bool prediction);
  void PlotScatter(double x, double y, Qt::GlobalColor color, int index, bool add, int shape, int shape_size, bool prediction);
  void UpdateCarBox();
  void UpdateObsBox();
  void DispPredictionInfo();
  void ShowTracer(QString str);

  void AdjustPlotRange(bool prediction);
  bool SearchSyncTime(PlayMode mode);

  double customSplit(QByteArray ba);
  TimeStamp TimeSplit(QByteArray ba, bool CCU);
  TimeStamp TimeConvert(long long stamp);

private:

  Ui::Widget *ui;

  QCPItemTracer *tracer_;
  QCPItemText *tracer_label_;

  QPointF mouse_pos_;
  bool left_;

//  QCPToolTip *tooltip_;
  int time_interval = 100;
  QTimer *fTimer_;

  Eigen::Matrix4d trans_bp_imu;
  QVector<QPointF> map[6];
  ChartView *chartview;
  double min_utm_x, max_utm_x, min_utm_y, max_utm_y;
  double plot_max_x_, plot_min_x_, plot_max_y_, plot_min_y_;
  double ctrl_max_y_, ctrl_min_y_;

  QVector<Frame> res;
  int frame_num_;

  QButtonGroup *group_button_;

  QVector<CCUMap> ccu_map_;
  QVector<PredictionInfo> prediction_info_;
  QVector<ControlInfo> control_info_;
  QVector< QVector<QPointF> > car_;

  int prediction_index_;
  int control_start_, control_end_;
  int sync_ctrl_index_;

  QMap<QString, int> prediction_graph_manage_, control_graph_manage_;
  QHash<QString,QString> control_info_manage_;
  QVector<QCPItemLine *> car_arrows_;
  QVector<QCPItemLine *> obs_arrows_;

  bool map_in_ = false;
  bool ccu_map_in_ = false;
  bool vehicle_box_in_ = false;

  bool icu_info_in_ = false;
  bool prediction_info_in_ = false;
  bool control_info_in_ = false;

  bool test_ = false;

  //add by lmz
  QProcess *process;
  QString wricornerstr = "";
  QString filetime;
  //add by lmz

  double lidar_offset_x = 0;//-0.28;
  double lidar_offset_y = 5.6;//8.96;    //计算车长
  double lidar_offset_z = 3.6;
  double lidar_yaw = 0;
  double lidar_pitch = -0.3104;
  double lidar_roll = 0.02667;

  double left_bp_lidar_offset_x = -1.8;//-3.4;   //计算车宽
  double right_bp_lidar_offset_x = 1.8;//3;     //计算车宽
  double to_ego_length = 0.9;//1.2;     //计算车长
  double right_chain_y = 0.8;     //计算车长

  float vehicle_length = 9.1; //车长
  float vehicle_width = 3.5;  //车宽
  float vehicle_height = 3.6; //车宽

  float vehicle_centroid_head_length = 6.5; //后轴中心-车头距离
  float Rotate_Lidar_Dist_Long_Deviation = 5.6; //前激光-后轴中心纵向
  float Rotate_Lidar_Dist_Lat_Deviation = 0.0; //前激光-后轴中心横向
};
#endif // MAINWINDOW_H

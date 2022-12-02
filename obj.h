#ifndef OBJ_H
#define OBJ_H

//#include <pcl/point_cloud.h>
//#include <pcl/point_types.h>
#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
#include <proj_api.h>
#include <QVector>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
#include <QPoint>

#define Pi 3.141592635897932846

QPointF calcPoint(QPointF center, double head, double length);

enum ControlInfoItems{
  KGpsHeading = 0, kGpsSpeed, kRestDis, kBiaDistance, kPreviewDis,
  kCurvePrepoint, kAngleDeviationAtPrepoint, kHeadingAngleDeviationAtPrepoint,
  kRecordMAngleHead, kDevAngleCtl, kDevHeadingCtl, kFrontWheelAngle,
  kWireStatusFrontWheelAngle, kDesiredSpeed, kDesiredLidarSpeed,
  kSpeedPreview, kVehicleVelocity, kPitchFilterRecord, kSlopeVehicle,
  kSlopePreview, kSpeedCtlState, kSpdE, kSpdEc, kBaseCtlValue,
  kDeltaCtlValue, kTotalCtlValue, kShiftCtrl, kWireStatusGearShift,
  kThrottlePedal, kWireStatusAccActuatePercent, kBrakePedal,
  kWireStatusElectricBrakeActuatePercent, kHydraulicBrake,
  kWireStatusMechanicalBrakeActuatePercent, kWireStatusFrontBrakePercent,
  kObjectStopFlag, kDisPath, kPathWheelAnglePre, kOrder2KalmanSpeed,
  klatBiaDisPre,
};

enum PlayMode{
  kError = -1,
  kICU = 0,
  KPrediction,
  kCtrl,
  kICUAndPrediction,
  kPredictionAndControl,
  kAll,
};

struct TimeStamp{
  int y;
  short mon;
  short d;
  short h;
  short m;
  short s;
  int ms;
  double stamp;
  TimeStamp(int year, short month, short day, short hour, short min, short sec, short msec):
    y(year), mon(month), d(day), h(hour), m(min), s(sec), ms(msec){}
  TimeStamp():h(0),m(0),s(0),ms(0){}
  bool operator < (const TimeStamp & t) const
  {
    if(y < t.y) return true; else if(y > t.y)return false;
    if(mon < t.mon) return true; else if(mon > t.mon) return false;
    if(d < t.d) return true; else if(d > t.d) return false;
    if(h < t.h) return true; else if(h > t.h) return false;
    if(m < t.m) return true; else if(m > t.m) return false;
    if(s < t.s) return true; else if(s > t.s) return false;
    if(ms < t.ms) return true; else return false;
  }

  bool operator > (const TimeStamp &t) const
  {
    if(y > t.y) return true; else if(y < t.y) return false;
    if(mon > t.mon) return true; else if(mon < t.mon) return false;
    if(d > t.d) return true; else if(d < t.d) return false;
    if(h > t.h) return true; else if(h < t.h) return false;
    if(m > t.m) return true; else if(m < t.m) return false;
    if(s > t.s) return true; else if(s < t.s) return false;
    if(ms > t.ms) return true; else return false;
  }
  bool operator == (const TimeStamp &t) const
  {
    if(y != t.y) return false;
    else if(mon != t.mon) return false;
    else if(d != t.d) return false;
    else if(h != t.h) return false;
    else if(m != t.m) return false;
    else if(s != t.s) return false;
    else if(abs(ms - t.ms) > 49) return false;
    else return true;
  }
};
typedef struct{
  Eigen::Vector4d xyz;
  Eigen::Vector4d utm;
  short type; //障碍物类型：（0:挡墙；1: 障碍物;
  //3: F0； 4：F1； 5：ll； 6: lr; 7: rl; 8: rr）
}Obj;

typedef struct{
  double longitude;//经度
  double latitude;//纬度
  double altitude;//高程
  double heading;//航向角
}GPSData;

typedef struct{
  double yaw;//航向角
  double pitch;//俯仰角
  double roll;//翻滚角
}IMUData;

typedef struct{
  double utm_x;
  double utm_y;
  double utm_z;
}UTMData;

typedef struct{
  //long timestamp;
  GPSData gps_data;
  IMUData imu_data;
  UTMData utm_data;
  //    short h, m ,s;
  //    int ms;
  TimeStamp time;
}Data;

typedef struct{
  QVector<Obj> points;
  Obj car[5];
  Data giu;
  Eigen::Matrix4d trans_utm_imu;
} Frame;

typedef struct{
  int id; //
  int key_id;
  int type;
  float obs2route_dis;
  float length, width, height;
  float head;
  int num;//num of vertex
  QPointF center;
  QVector<QPointF> vertex;
  TimeStamp time_stamp;
  GPSData gps;
} Obstacle;

class CCUMap{
public:
  QPointF route;
  QPointF edge[2];//left right
  float l_edge;
  float r_edge;
  float head; //degree 0~360
  CCUMap(QPointF rp,float l, float r, float h){
    route = rp;
    l_edge = l;
    r_edge = r;
    head = h;
    edge[0] = calcPoint(rp,head, -l);
    edge[1] = calcPoint(rp,head, r);
  }
};

class ControlInfo{
public:
  TimeStamp time;
  double info[40];
//  double gps_heading;//航向°√
//  double gps_speed; //gps速度√
//  double rest_dis; //路径停车距离误差√
//  double bia_distance; //横向偏差√
//  double preview_dis; //横向控制预瞄距离√
//  double curve_prepoint; //控制曲率√
//  double angle_deviation_at_prepoint; //预瞄点-当前点方位角偏差°√
//  double heading_angle_deviation_at_prepoint; //预瞄点-当前点航向角偏差°√
//  double record_m_angle_head; //关键点-当前点航向偏差°√
//  double dev_angle_ctl; //方位/横向偏差转角指令°√
//  double dev_heading_ctl; //航向偏差转角指令°√
//  double front_wheel_angle;//总转角指令°√
//  double wire_status_front_wheel_angle;// 前轮转角°√
//  double desired_speed;//总期望车速km/h√
//  double desired_lidar_speed;//感知期望车速km/h√
//  double speed_preview;//道路期望车速km/h√
//  double vehicle_velocity;//实际车速km/h√
//  double pitch_filter_record;//俯仰角滤波°√
//  double slope_vehicle;//车辆坡度滤波%√
//  double slope_preview;//预瞄坡度滤波%√
//  double speed_ctl_state;//速度控制状态√
//  double spd_e; //速度偏差√
//  double spd_ec; // 速度偏差变化滤波√
//  double base_ctl_value;//速度控制基础量√
//  double delta_ctl_value;//速度控制调节量√
//  double total_ctl_value;//速度控制总控制量√
//  double shift_ctrl;//档位控制量√
//  double wire_status_gear_shift;//档位反馈√
//  double throttle_pedal;//油门控制量√
//  double wire_status_acc_actuate_percent;//油门反馈√
//  double brake_pedal;//电制动控制量√
//  double wire_status_electric_brake_actuate_percent;//电制动反馈√
//  double hydraulic_brake;//液压制动控制量√
//  double wire_status_mechanical_brake_actuate_percent;//制动反馈√
//  double wire_status_front_brake_percent;//前制动反馈√
//  double object_stop_flag;//√
//  double dis_path;//障碍物路径距离√
//  double path_wheel_angle_pre; //前馈转向√
//  double order2_kalman_speed;//二阶卡尔曼滤波速度√
//  double lat_bia_dis_pre;//优化预测横向偏差√

  double actual_speed;
  double expected_speed;
};

class PredictionInfo{
public:
  TimeStamp time;
  TimeStamp obs_time; //用于和感知进行时间戳匹配
  QPointF current_utm;
  double current_head;
  QPointF key_utm;
  double key_head;
  QPointF car[48][5];// 0车中心???1 2 3 4车身四个点
  QPointF vehicle[4];
  bool is_impact;
  Obstacle obs;
  QVector<Obstacle> all_obs;
};

class FrontSenseInfo{
public:
  TimeStamp time;
  QPointF local_car[4];
  double gps_lng;
  double gps_lat;
  double gps_heading;
  QPointF car[4];
  Obstacle obs;
};
#endif // OBJ_H

#include "convert.h"

void convertWGS84ToUTM(double longitude, double latitude, double heading, double *utm_x, double *utm_y, double *utm_z)
{

    int utm_zone_num = std::floor(longitude/6) + 31;
    projPJ utm_, wgs84_;
    string proj4_param_str = "+proj=utm +zone=" + to_string(utm_zone_num) + " +datum=WGS84 +units=m +no_defs";
    wgs84_ = pj_init_plus("+proj=longlat +datum=WGS84 +no_defs ");
    utm_ = pj_init_plus(proj4_param_str.c_str());

    longitude *= DEG_TO_RAD;
    latitude *= DEG_TO_RAD;

    pj_get_def(wgs84_, 0);
    pj_transform(wgs84_, utm_, 1, 1, &longitude, &latitude, &heading);
//    const char* strRelease = pj_get_release();
//    char* strError = pj_strerrno(2);
//    int iErrorNo = *pj_get_errno_ref();

    *utm_x = longitude;
    *utm_y = latitude;
    *utm_z = heading;
}

void convertUTMToWGS84(double *longitude, double *latitude,  double utm_x, double utm_y){
    int utm_zone_num = std::floor(116/6) + 31;
    projPJ utm_, wgs84_;
    string proj4_param_str = "+proj=utm +zone=" + to_string(utm_zone_num) + " +datum=WGS84 +units=m +no_defs";
    wgs84_ = pj_init_plus("+proj=longlat +datum=WGS84 +no_defs ");
    utm_ = pj_init_plus(proj4_param_str.c_str());
    pj_transform(utm_, wgs84_, 1, 1, &utm_x, &utm_y,nullptr);
    *longitude = utm_x * RAD_TO_DEG;
    *latitude = utm_y * RAD_TO_DEG;
}

//void calcCorner(double heading, QPointF centrid,QPointF * corner)
void calcCorner(double heading, QPointF centrid,QPointF * corner,double length, double width, double  vehicle_centroid_head_length)
{

//    float length  = 9.1;//8.85;
//    float width = 3.5;//3.11;
//    float vehicle_centroid_head_length = 6.5;//9.2;s
     heading  = heading * 3.1415926 / 180.0;
//     qDebug()<< length <<" "<<width<<" "<<vehicle_centroid_head_length;
    float cos_heading = cos(heading);
    float sin_heading = sin(heading);
    float half_length = length/2;
    float half_width = width/2;
    float centroidToBack = length - vehicle_centroid_head_length;
    //float centerToCentroid = centroidHeadLength - half_length;
    float dx1 = sin_heading * centroidToBack;
    float dx2 = cos_heading * half_width;
    float dy1 = cos_heading * centroidToBack;
    float dy2 = sin_heading * half_width;
    float dx = sin_heading * vehicle_centroid_head_length;//dx1
    float dy = cos_heading * vehicle_centroid_head_length;//dy1

    QPointF points[4];
    //左下角
    points[0].setX(centrid.x() - dx1 - dx2);
    points[0].setY(centrid.y() - dy1 + dy2);
    //左上角
    points[1].setX(centrid.x() + dx - dx2);
    points[1].setY(centrid.y() + dy + dy2);
    //右上角
    points[2].setX(centrid.x() + dx + dx2);
    points[2].setY(centrid.y() + dy - dy2);
    //右下角
   points[3].setX(centrid.x() - dx1 + dx2);
   points[3].setY(centrid.y() - dy1 - dy2);

//   qDebug()<<"1111"<<centrid.utm_x <<" "<<centrid.utm_y ;
//   qDebug()<<points[0].x()<<" "<<points[0].y();
   for(int i = 0; i < 4; ++i){
        *(corner + i) = points[i];
    }
}

Eigen::Matrix4d calcTransformMatrix(double offset_x, double offset_y, double offset_z, double yaw, double pitch, double roll)
{
    Eigen::Matrix4d calc_transform = Eigen::Matrix4d::Identity();

    Eigen::Vector3d eular_angle(yaw*DEG_TO_RAD, roll*DEG_TO_RAD, pitch*DEG_TO_RAD);
    Eigen::Matrix3d R;
    R = Eigen::AngleAxisd(eular_angle[0], Eigen::Vector3d::UnitZ())
    * Eigen::AngleAxisd(eular_angle[1], Eigen::Vector3d::UnitY())
    * Eigen::AngleAxisd(eular_angle[2], Eigen::Vector3d::UnitX());
    Eigen::Quaterniond q;
    q = R;
    //Eigen::Vector3d t = Eigen::Vector3d(offset_x, offset_y, offset_z);

    calc_transform.block<3,3>(0,0) = q.toRotationMatrix();
    calc_transform(0, 3) = offset_x;
    calc_transform(1, 3) = offset_y;
    calc_transform(2, 3) = offset_z;

    return calc_transform;
}

float distance(QPointF p1, QPointF p2){
    float delta_x = p1.x() - p2.x();
    float delta_y = p1.y() - p2.y();
    float dis_2 = pow(delta_x, 2) + pow(delta_y, 2);
    float dis = sqrt(dis_2);
    return dis;
}
/*
 * @param 计算以center为中心，head为航向的相对坐标系转化为绝对坐标系
 * @input
 * @return
 */
QPointF calcPoint(QPointF center, double head, double length){
  QPointF result;
  head = head * M_PI / 180.0;
  double x = center.x() + length * cos(head);
  double y = center.y() - length * sin(head);
  result.setX(x);
  result.setY(y);
  return result;
}

QPointF calcPoint(double gps_lng, double gps_lat, double head, QPointF point){
  QPointF result;
  double utm_x,utm_y,utm_z;
  double heading = head * M_PI / 180.0;
  convertWGS84ToUTM(gps_lat, gps_lng, head, &utm_x, &utm_y ,&utm_z);
//qDebug() <<"utm"<< QString::number(utm_x,6,'f') << " "
//                 << QString::number(utm_y,6,'f');
  Eigen::Matrix<double,2,2> rotation_matrix;
  rotation_matrix << cos(heading),sin(heading),
                    -sin(heading),cos(heading);
  Eigen::Matrix<double,2,1> traslation_matrix;
  traslation_matrix << utm_x, utm_y;
  Eigen::Matrix <double,2,1> origin;
  origin << point.x(), point.y();
  Eigen::Matrix<double,2,1> result_mat = rotation_matrix * origin + traslation_matrix;
  result = QPointF(result_mat(0,0), result_mat(1,0));
  return result;
}

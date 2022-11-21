#ifndef CONVERT_H
#define CONVERT_H

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
#include <proj_api.h>
#include <QDebug>
#include <QPoint>
#include "obj.h"

using namespace std;

void convertWGS84ToUTM(double longitude, double latitude, double heading, double *utm_x, double *utm_y, double *utm_z);
Eigen::Matrix4d calcTransformMatrix(double offset_x, double offset_y, double offset_z, double yaw, double pitch, double roll);
void calcCorner(double heading, QPointF centrid,QPointF * corner,double length, double width, double  vehicle_centroid_head_length);
void convertUTMToWGS84(double *longitude, double *latitude,  double utm_x, double utm_y);
float distance(QPointF p1, QPointF p2);
QPointF calcPoint(QPointF center, double head, double length);

#endif // CONVERT_H

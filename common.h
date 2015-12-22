#ifndef COMMON
#define COMMON

//Qt
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QMainWindow>
#include <QApplication>

//opencv
#include <opencv2/opencv.hpp>
using namespace cv;

#include "stdio.h"

//for Annotation
const int ALI_POINTS_NUM = 21;
const QString Author = "HQQ", Title = "Labeling_mface";
std::vector<cv::Point2f> get_meanpose(cv::Rect_<float> rect);

//for GUI
const qreal ScaleFactor = 1.1;
const int CacheWidth = 4, CacheSize = 256;

//kits
qreal qScale(QSizeF from, QSizeF to);
qreal cal_L2(QPointF p);
qreal cal_L2(QLineF l, QPointF p);
QString toString(QPoint p);
QString toString(QPointF p);
QString toString(QRect r);
QString toString(QRectF r);

std::string Qstr2stdstr(QString);

QRectF toQrect(cv::Rect_<float> rect);

#endif // COMMON


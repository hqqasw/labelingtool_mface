#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <opencv2/opencv.hpp>
#include "facetools.h"
#include "common.h"

#define IMAGE_CHCHE 256  //MB

class Sequence
{
public:
    Sequence(QString filename, QString res_file);
    ~Sequence();

    QImage get_frame(int i);
    QImage get_frame();
    QRectF &get_camera();
    int get_frame_height();
    int get_frame_width();
    int get_frame_N();
    QSize get_size();
    qreal get_frame_rate();
    void set_n(int _n);
    int get_n();
    QString get_name();
    void set_name(QString _name);
    QString get_path();
    void set_path(QString _path);

    void set_FaceID(int _FaceID);

    void delete_face();
    void add_face_press(QPointF loc);
    void add_face_move(QPointF loc);
    void add_face_release();

    void labelingone(int FaceID, int PointID, QPointF loc);

    std::vector<std::vector<Face> > res;

private:

    cv::VideoCapture capture;
    int height;
    int width;
    qreal rate;
    int N; //total frames
    int n; //frame number now

    int FaceID;

    int ID_now;

    QString name;
    QString path;

    QRectF camera;

    std::vector<cv::Mat> image_cache;
    int cache_count;
    int cache_start;

    FaceTools facetools;

    QImage mat2Qimg(cv::Mat a);
    cv::Mat get_frame_raw(int i);
    void update(); //update res according to manual change

    float overlap(Rect_<float> a, Rect_<float> b);

    void resetID(int start_frame, int end_frame, int old_ID, int new_ID);
};

#endif // SEQUENCE_H

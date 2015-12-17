#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <opencv2/opencv.hpp>
#include "facetools.h"

#define IMAGE_CHCHE 1024  //MB

class Sequence
{
public:
    Sequence(std::string filename);
    ~Sequence();

    QImage get_frame(int i);
    int get_frame_height();
    int get_frame_width();
    int get_frame_N();

    std::vector<std::vector<Face> > res;

private:

    cv::VideoCapture capture;
    int height;
    int width;
    int N; //total frames

    std::vector<cv::Mat> image_cache;
    int cache_count;

    FaceTools facetools;

    QImage mat2Qimg(cv::Mat a);
    cv::Mat get_frame_raw(int i);

};

#endif // SEQUENCE_H

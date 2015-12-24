#ifndef FACETOOLS_H
#define FACETOOLS_H

#define CONFIG_ST_FACE_API_PRIVATE

#include <opencv2/opencv.hpp>
#include <cv_face.h>
#include <QDebug>
#include "common.h"

struct Face{
    cv::Rect_<float> rect;
    std::vector<cv::Point2f> alignments;
    int ID;

    Face()
    {
        alignments = std::vector<cv::Point2f>(ALI_POINTS_NUM, Point2f(-1,-1));
        rect.width = 0;
        ID = -1;
    }
};

class FaceTools
{
public:
    FaceTools();
    ~FaceTools();

    int detect(cv::Mat image, std::vector<Face> &faces);
    int align(cv::Mat image, std::vector<Face> &faces);
    int align(cv::Mat image, Face &faces);

    int track(cv::Mat image, std::vector<Face> faces_last, std::vector<Face> &faces);
    int track(cv::Mat image, Face face_last, Face &face);

    Rect_<float> points2rect(std::vector<Point2f> alignments);

private:
    cv_handle_t handle_detect;
    cv_face_t *p_face;
    int face_count;
    cv_result_t st_result;

    cv_handle_t hTrack;

};

#endif // FACETOOLS_H

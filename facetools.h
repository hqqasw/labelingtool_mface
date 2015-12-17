#ifndef FACETOOLS_H
#define FACETOOLS_H

#define CONFIG_ST_FACE_API_PRIVATE

#include <opencv2/opencv.hpp>
#include <cv_face.h>
#include <QDebug>

#define ALI_POINTS_NUM 21

struct Face{
    cv::Rect rect;
    std::vector<cv::Point> alignments;
    int ID;

    Face()
    {
        rect.width = 0;
        ID = -1;
    }
};

class FaceTools
{
public:
    FaceTools();

    int detect(cv::Mat image, std::vector<Face> &faces);
    int align(cv::Mat image, std::vector<Face> &faces);
    int detectAndalign(cv::Mat image, std::vector<Face> &faces);

private:
    cv_handle_t handle_detect;
    cv_face_t *p_face;
    int face_count;
    cv_result_t st_result;

};

#endif // FACETOOLS_H

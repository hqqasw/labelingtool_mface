#include "facetools.h"

using namespace cv;

FaceTools::FaceTools()
{
    p_face = NULL;
    face_count = 0;
    st_result = CV_OK;

    handle_detect = cv_face_create_detector(NULL, CV_FACE_SKIP_BELOW_THRESHOLD | CV_DETECT_ENABLE_ALIGN);
    if (!handle_detect)
    {
       qDebug("Fail to init detect handle!");
        return;
    }
}

FaceTools::~FaceTools()
{

}


int FaceTools::detect(Mat image, std::vector<Face> &faces)
{
    if(image.channels() == 1)
        cvtColor(image, image, CV_GRAY2BGR);
    Mat image_color;
    if(image.channels() == 3)
        cvtColor(image, image_color, CV_BGR2BGRA);
    if(image.channels() == 4)
        image_color = image;

    st_result = cv_face_detect(handle_detect, image_color.data, CV_PIX_FMT_BGRA8888,
                               image_color.cols, image_color.rows, image_color.step,
                               CV_FACE_UP, &p_face, &face_count);
    if (st_result != CV_OK)
    {
        qDebug("cv_face_detect error!");
        return 0;
    }

    if (face_count == 0)
            return 0;

    for(int i = 0; i < face_count; i++)
    {
        Face face_tmp;
        face_tmp.rect.x = p_face[i].rect.left;
        face_tmp.rect.y = p_face[i].rect.top;
        face_tmp.rect.width = p_face[i].rect.right - p_face[i].rect.left;
        face_tmp.rect.height = p_face[i].rect.bottom - p_face[i].rect.top;
        faces.push_back(face_tmp);
    }
    return 1;
}

int FaceTools::align(Mat image, std::vector<Face> &faces)
{
    if (faces.size() == 0)
            return 0;

    if(image.channels() == 1)
        cvtColor(image, image, CV_GRAY2BGR);
    Mat image_color;
    if(image.channels() == 3)
        cvtColor(image, image_color, CV_BGR2BGRA);
    if(image.channels() == 4)
        image_color = image;

    for(int i = 0; i < faces.size(); i++)
    {
        cv_rect_t rect;

        rect.left = faces[i].rect.x;
        rect.top = faces[i].rect.y;
        rect.right = faces[i].rect.x + faces[i].rect.width;
        rect.bottom = faces[i].rect.y + faces[i].rect.height;

        cv_pointf_t *p_face_points = new cv_pointf_t[ALI_POINTS_NUM];
        st_result = cv_face_rect_align21p(handle_detect, image_color.data, CV_PIX_FMT_BGRA8888,
                    image_color.cols, image_color.rows, image_color.step,
                    rect, p_face_points);
        if (st_result != CV_OK)
        {
            qDebug("cv_face_align error!");
            return 0;
        }

        std::vector<Point2f> alignments(ALI_POINTS_NUM);
        for(int j = 0; j < ALI_POINTS_NUM; j++)
        {
            alignments[j].x = p_face_points[j].x;
            alignments[j].y = p_face_points[j].y;
        }
        faces[i].alignments.assign(alignments.begin(), alignments.end());
    }
    return 1;
}

int FaceTools::detectAndalign(Mat image, std::vector<Face> &faces)
{
    if(image.channels() == 1)
        cvtColor(image, image, CV_GRAY2BGR);
    Mat image_color;
    if(image.channels() == 3)
        cvtColor(image, image_color, CV_BGR2BGRA);
    if(image.channels() == 4)
        image_color = image;

    st_result = cv_face_detect(handle_detect, image_color.data, CV_PIX_FMT_BGRA8888,
                               image_color.cols, image_color.rows, image_color.step,
                               CV_FACE_UP, &p_face, &face_count);
    if (st_result != CV_OK)
    {
        qDebug("cv_face_detect error!");
        return 0;
    }

    for(int i = 0; i < face_count; i++)
    {
        cv_pointf_t *p_face_points = new cv_pointf_t[ALI_POINTS_NUM];
        st_result = cv_face_rect_align21p(handle_detect, image_color.data, CV_PIX_FMT_BGRA8888,
                    image_color.cols, image_color.rows, image_color.step,
                    p_face[i].rect, p_face_points);
        if (st_result != CV_OK)
        {
            qDebug("cv_face_align error!");
            return 0;
        }

        Face face_tmp;
        face_tmp.rect.x = p_face[i].rect.left;
        face_tmp.rect.y = p_face[i].rect.top;
        face_tmp.rect.width = p_face[i].rect.right - p_face[i].rect.left;
        face_tmp.rect.height = p_face[i].rect.bottom - p_face[i].rect.top;
        faces.push_back(face_tmp);

        std::vector<Point2f> alignments(ALI_POINTS_NUM);
        for(int j = 0; j < ALI_POINTS_NUM; j++)
        {
            alignments[j].x = p_face_points[j].x;
            alignments[j].y = p_face_points[j].y;
        }
        faces[i].alignments.assign(alignments.begin(), alignments.end());
    }
    return 1;


}

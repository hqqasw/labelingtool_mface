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

    hTrack = cv_face_create_frame_tracker("./data/M_Align_Deepface_21_track_1.0.model");
    if(!hTrack) {
        qDebug("Fail to init track handle!");
        return;
    }
}

FaceTools::~FaceTools()
{
    cv_face_destroy_detector(handle_detect);
    cv_face_destroy_frame_tracker(hTrack);
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

    cv_face_release_detector_result(p_face, face_count);
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

int FaceTools::align(cv::Mat image, Face &face)
{
    if(image.channels() == 1)
        cvtColor(image, image, CV_GRAY2BGR);
    Mat image_color;
    if(image.channels() == 3)
        cvtColor(image, image_color, CV_BGR2BGRA);
    if(image.channels() == 4)
        image_color = image;

    cv_rect_t rect;

    rect.left = face.rect.x;
    rect.top = face.rect.y;
    rect.right = face.rect.x + face.rect.width;
    rect.bottom = face.rect.y + face.rect.height;

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
    face.alignments.assign(alignments.begin(), alignments.end());

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

    cv_face_release_detector_result(p_face, face_count);
    return 1;
}


int FaceTools::track(cv::Mat image, std::vector<Face> faces_last, std::vector<Face> &faces)
{
    Mat image_gray = image.clone();
    if(image_gray.channels() == 4)
        cvtColor(image_gray, image_gray, CV_BGRA2GRAY);
    else if(image_gray.channels() == 3)
        cvtColor(image_gray, image_gray, CV_BGR2GRAY);

    faces.clear();
    for(int i = 0; i < faces_last.size(); i++)
    {
        cv_face_t temp_face;
        bool temp_success;

        temp_face.rect.left = faces_last[i].rect.x;
        temp_face.rect.right = faces_last[i].rect.x + faces_last[i].rect.width;
        temp_face.rect.top = faces_last[i].rect.y;
        temp_face.rect.bottom = faces_last[i].rect.y = faces_last[i].rect.height;

        cv_face_frame_track(hTrack, image_gray.data, CV_PIX_FMT_GRAY8, image_gray.cols, image_gray.rows, image_gray.step, &temp_face, &temp_success);

        if(temp_success)
        {
            Face face;
            face.rect.x = temp_face.rect.left;
            face.rect.y = temp_face.rect.top;
            face.rect.width = temp_face.rect.right - temp_face.rect.left;
            face.rect.height = temp_face.rect.bottom - temp_face.rect.top;
            faces.push_back(face);
        }
    }

    return 1;
}

int FaceTools::track(cv::Mat image, Face face_last, Face &face)
{
    Mat image_gray = image.clone();
    if(image_gray.channels() == 4)
        cvtColor(image_gray, image_gray, CV_BGRA2GRAY);
    else if(image_gray.channels() == 3)
        cvtColor(image_gray, image_gray, CV_BGR2GRAY);

    cv_face_t temp_face;
    bool temp_success;

    temp_face.rect.left = face_last.rect.x;
    temp_face.rect.right = face_last.rect.x + face_last.rect.width;
    temp_face.rect.top = face_last.rect.y;
    temp_face.rect.bottom = face_last.rect.y = face_last.rect.height;

    cv_face_frame_track(hTrack, image_gray.data, CV_PIX_FMT_GRAY8, image_gray.cols, image_gray.rows, image_gray.step, &temp_face, &temp_success);

    if(temp_success)
    {
        face.rect.x = temp_face.rect.left;
        face.rect.y = temp_face.rect.top;
        face.rect.width = temp_face.rect.right - temp_face.rect.left;
        face.rect.height = temp_face.rect.bottom - temp_face.rect.top;
        return 1;
    }

    return 0;
}



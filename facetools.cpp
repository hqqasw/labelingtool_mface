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

    hTrack = cv_face_create_frame_tracker("/home/hqq/80M.tar");
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

    Mat image_gray = image.clone();
    if(image_gray.channels() == 3)
        cvtColor(image_gray, image_gray, CV_BGR2GRAY);
    if(image_gray.channels() == 4)
        cvtColor(image_gray, image_gray, CV_BGRA2GRAY);

    for(int i = 0; i < faces.size(); i++)
    {
        cv_face_t face;

        face.rect.left = faces[i].rect.x;
        face.rect.top = faces[i].rect.y;
        face.rect.right = faces[i].rect.x + faces[i].rect.width;
        face.rect.bottom = faces[i].rect.y + faces[i].rect.height;
        face.points_count = 0;

        bool temp_success;
        cv_face_frame_track(hTrack, image_gray.data, CV_PIX_FMT_GRAY8,
                            image_gray.cols, image_gray.rows, image_gray.step,
                            &face, &temp_success);

        if(temp_success)
        {
            faces[i].rect.x = face.rect.left;
            faces[i].rect.y = face.rect.top;
            faces[i].rect.width = face.rect.right - face.rect.left;
            faces[i].rect.height = face.rect.bottom - face.rect.top;
            for(int j = 0; j < ALI_POINTS_NUM; j++)
            {
                faces[i].alignments[j].x = face.points_array[j].x;
                faces[i].alignments[j].y = face.points_array[j].y;
            }
        }
    }
    return 1;
}

int FaceTools::align(Mat image, Face &faces)
{

    Mat image_gray = image.clone();
    if(image_gray.channels() == 3)
        cvtColor(image_gray, image_gray, CV_BGR2GRAY);
    if(image_gray.channels() == 4)
        cvtColor(image_gray, image_gray, CV_BGRA2GRAY);

    cv_face_t face;

    face.rect.left = faces.rect.x;
    face.rect.top = faces.rect.y;
    face.rect.right = faces.rect.x + faces.rect.width;
    face.rect.bottom = faces.rect.y + faces.rect.height;
    face.points_count = 0;

    bool temp_success;
    cv_face_frame_track(hTrack, image_gray.data, CV_PIX_FMT_GRAY8,
                        image_gray.cols, image_gray.rows, image_gray.step,
                        &face, &temp_success);

    if(temp_success)
    {
        faces.rect.x = face.rect.left;
        faces.rect.y = face.rect.top;
        faces.rect.width = face.rect.right - face.rect.left;
        faces.rect.height = face.rect.bottom - face.rect.top;
        for(int j = 0; j < ALI_POINTS_NUM; j++)
        {
            faces.alignments[j].x = face.points_array[j].x;
            faces.alignments[j].y = face.points_array[j].y;
        }
    }
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
        for(int j = 0; j < ALI_POINTS_NUM; j++)
        {
            temp_face.points_array[j].x = faces_last[i].alignments[j].x;
            temp_face.points_array[j].y = faces_last[i].alignments[j].y;
        }
        temp_face.points_count = ALI_POINTS_NUM;


        cv_face_frame_track(hTrack, image_gray.data, CV_PIX_FMT_GRAY8, image_gray.cols, image_gray.rows, image_gray.step, &temp_face, &temp_success);

        if(temp_success)
        {
            Face face;
            face.rect.x = temp_face.rect.left;
            face.rect.y = temp_face.rect.top;
            face.rect.width = temp_face.rect.right - temp_face.rect.left;
            face.rect.height = temp_face.rect.bottom - temp_face.rect.top;

            for(int j = 0; j < ALI_POINTS_NUM; j++)
            {
                face.alignments[j].x = temp_face.points_array[j].x;
                face.alignments[j].y = temp_face.points_array[j].y;
            }
            face.ID = faces_last[i].ID;

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
    for(int j = 0; j < ALI_POINTS_NUM; j++)
    {
        temp_face.points_array[j].x = face_last.alignments[j].x;
        temp_face.points_array[j].y = face_last.alignments[j].y;
    }
    temp_face.points_count = ALI_POINTS_NUM;

    cv_face_frame_track(hTrack, image_gray.data, CV_PIX_FMT_GRAY8, image_gray.cols, image_gray.rows, image_gray.step, &temp_face, &temp_success);

    if(temp_success)
    {
        face.rect.x = temp_face.rect.left;
        face.rect.y = temp_face.rect.top;
        face.rect.width = temp_face.rect.right - temp_face.rect.left;
        face.rect.height = temp_face.rect.bottom - temp_face.rect.top;
        for(int j = 0; j < ALI_POINTS_NUM; j++)
        {
            face.alignments[j].x = temp_face.points_array[j].x;
            face.alignments[j].y = temp_face.points_array[j].y;
        }
        face.ID = face_last.ID;

        return 1;
    }

    return 0;
}

Rect_<float> FaceTools::points2rect(std::vector<Point2f> alignments)
{
    cv_pointf_t *p;
    p = new cv_pointf_t[5];
    for(int i = 0; i < 5; i++)
    {
        p[i].x = alignments[i+16].x;
        p[i].y = alignments[i+16].y;
    }
    cv_rect_t rect;
    cv_face_5points_to_rect(p, rect);
    Rect_<float> rect_f;
    rect_f.x = rect.left;
    rect_f.y = rect.top;
    rect_f.width = rect.right - rect.left;
    rect_f.height = rect.bottom - rect.top;

    return rect_f;
}



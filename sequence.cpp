#include "sequence.h"
#include <QMessageBox>

using namespace cv;

Sequence::Sequence(QString qfilename, QString res_file)
{

    std::string filename = Qstr2stdstr(qfilename);

    capture.open(filename);
    if(!capture.isOpened())
    {
        qDebug("Fail to open video!");
        return;
    }

    N = capture.get(CV_CAP_PROP_FRAME_COUNT);
    height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    rate = capture.get(CV_CAP_PROP_FPS);

    cache_count = N;
    if(N * height * width * 3 / (1024*1024) > IMAGE_CHCHE)
        cache_count = IMAGE_CHCHE*1024*1024 / (height * width * 3);
    cache_start = 0;

    image_cache = std::vector<cv::Mat>(cache_count);

    for(int i = 0; i < cache_count; i++)
    {
        capture >> image_cache[i];
    }

    ID_now = 0;
    if(res_file == NULL)
    {
        qDebug("No result file!");
    }
    else
    {
        FILE* fp;
        fp = fopen(Qstr2stdstr(res_file).c_str(),"r");
        if(fp == NULL) return;

        char tmp[50];
        fscanf(fp,"%s",tmp);
        fscanf(fp,"%d %d %d %f",&N,&width,&height,&rate);
        for(int i = 0; i < N; i++)
        {
            int face_num;
            fscanf(fp,"%d",&face_num);
            std::vector<Face> res_tmp(face_num);
            for(int j = 0; j < face_num; j++)
            {
                fscanf(fp, "%d", &res_tmp[j].ID);
                fscanf(fp,"%f %f %f %f", &res_tmp[j].rect.x, &res_tmp[j].rect.y, &res_tmp[j].rect.width, &res_tmp[j].rect.height);
                //res_tmp[j].alignments = std::vector<Point2f>(ALI_POINTS_NUM);
                for(int k = 0; k < ALI_POINTS_NUM; k++)
                {
                    fscanf(fp, "%f %f", &res_tmp[j].alignments[k].x, &res_tmp[j].alignments[k].y);
                }
                if(res_tmp[j].ID > ID_now)
                    ID_now = res_tmp[j].ID;
            }
            res.push_back(res_tmp);
        }

        fclose(fp);
    }

    n = 0;
    if(res[0].size() > 0)
        camera = toQrect(res[0][0].rect);
    else
    {
        double length = min(width, height);
        camera = QRectF(width/2 - length/5, height/2 - length/5, length*2/5, length*2/5);
    }

    FaceID = 0;
}

Sequence::~Sequence()
{
    capture.release();
}

QImage Sequence::get_frame(int i)
{
    cv::Mat image = get_frame_raw(i).clone();

    for(int k = 0; k < res[i].size(); k++)
    {
        char id_str[10];
        sprintf(id_str,"%d",res[i][k].ID);

        if(k == FaceID)
        {
            rectangle(image, res[i][k].rect, CV_RGB(255,0,0), 2);
            putText(image, id_str, Point2f(res[i][k].rect.x, res[i][k].rect.y-5),
                CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(255,0,0) );
        }
        else
        {
            rectangle(image, res[i][k].rect, CV_RGB(0,0,255), 2);
            putText(image, id_str, Point2f(res[i][k].rect.x, res[i][k].rect.y-5),
                CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(0,0,255) );
        }
        for(int j = 0; j < ALI_POINTS_NUM; j++)
        {
            if(res[i][k].alignments[j].x > 0 && res[i][k].alignments[j].y > 0)
                circle(image, res[i][k].alignments[j], 1, CV_RGB(0,255,0), 2);
        }
    }

    return mat2Qimg(image);
}

QImage Sequence::get_frame()
{
    return get_frame(n);
}

QRectF &Sequence::get_camera()
{
    return camera;
}

void Sequence::set_n(int _n)
{
    n = _n;
}

int Sequence::get_n()
{
    return n;
}


int Sequence::get_frame_N()
{
    return N;
}

int Sequence::get_frame_height()
{
    return height;
}

int Sequence::get_frame_width()
{
    return width;
}

qreal Sequence::get_frame_rate()
{
    return rate;
}

 QSize Sequence::get_size()
 {
     return QSize(width, height);
 }

 QString Sequence::get_name()
 {
     return name;
 }

 void Sequence::set_name(QString _name)
 {
     name = _name;
 }

 QString Sequence::get_path()
 {
     return path;
 }

 void Sequence::set_path(QString _path)
 {
     path = _path;
 }

 void Sequence::set_FaceID(int _FaceID)
 {
     FaceID = _FaceID;
 }

QImage Sequence::mat2Qimg(cv::Mat a)
{
    QImage qimg;
    Mat a_rgb;

    if(a.channels() == 3)
    {
        cvtColor(a, a_rgb, CV_BGR2RGB);
        qimg = QImage( (const unsigned char*)(a_rgb.data),
                       a_rgb.cols,
                       a_rgb.rows,
                       a_rgb.cols*a_rgb.channels(),
                       QImage::Format_RGB888 );
    }
    else
    {
        qimg = QImage( (const unsigned char*)(a.data),
                       a.cols,
                       a.rows,
                       a.cols*a.channels(),
                       QImage::Format_RGB888 );
    }

    return qimg;
}

cv::Mat Sequence::get_frame_raw(int i)
{

    if(i >= cache_start && i < cache_start + cache_count && i < N)
        return image_cache[i - cache_start];

    cache_start = i;
    capture.set(CV_CAP_PROP_POS_FRAMES, i);
    for(int i = 0; i < cache_count; i++)
    {
        capture >> image_cache[i];
        if( cache_start + i >= N)
            break;
    }
    return image_cache[i - cache_start];;
}

void Sequence::labelingone(int FaceID, int PointID, QPointF loc)
{
    res[n][FaceID].alignments[PointID] = Point(int(loc.x()), int(loc.y()));
    res[n][FaceID].rect = facetools.points2rect(res[n][FaceID].alignments);
}

void Sequence::delete_face()
{
    if(FaceID >= 0 && FaceID < res[n].size())
    {
        int delete_ID = res[n][FaceID].ID;
        res[n].erase(res[n].begin() + FaceID, res[n].begin() + FaceID + 1);
        for(int i = n+1; i < N; i++)
        {
            for(int j = 0; j < res[i].size(); j++)
            {
                if(res[i][j].ID == delete_ID)
                    res[i].erase(res[i].begin() + j, res[i].begin() + j + 1);
            }
        }
    }
}


void Sequence::add_face_press(QPointF loc)
{
    Face face;
    face.rect.x = loc.x();
    face.rect.y = loc.y();
    face.rect.width = 1;
    face.rect.height = 1;
    face.ID = ID_now;
    ID_now ++;

    //facetools.align(get_frame_raw(n), face);

    res[n].push_back(face);
}

void Sequence::add_face_move(QPointF loc)
{
    int face_num = res[n].size();
    Face face = res[n][face_num-1];
    float rect_size = std::max( std::min(loc.x() - face.rect.x, loc.y() - face.rect.y), 1.0);
    res[n][face_num-1].rect.width = rect_size;
    res[n][face_num-1].rect.height = rect_size;

    //facetools.align(get_frame_raw(n), res[n][face_num-1]);
}

void Sequence::add_face_release()
{
    int face_num = res[n].size();
    facetools.align(get_frame_raw(n), res[n][face_num-1]);

    //look backward
    if(n > 0)
    {
        for(int j = 0; j < res[n-1].size(); j++)
        {
            if(overlap(res[n-1][j].rect, res[n][face_num-1].rect) > 0.3)
            {
                res[n][face_num-1].ID = res[n-1][j].ID;
                break;
            }
        }
    }

    //look forward
    for(int i = n+1; i < min(n+101,N); i++)
    {
        face_num = res[i-1].size();
        int temp_success;
        Face face;
        temp_success = facetools.track(get_frame_raw(i), res[i-1][face_num - 1], face);

        if(temp_success == 1)
        {
            for(int j = 0; j < res[i].size(); j++)
            {
                if(overlap(res[i][j].rect, face.rect) > 0.3)
                {
                    temp_success = 0;
                    resetID(i, N, res[i][j].ID, face.ID);
                    break;
                }
            }
        }

        if(temp_success == 1)
            res[i].push_back(face);
        else
            break;
    }
    return;
}

float Sequence::overlap(Rect_<float> a, Rect_<float> b)
{
    float left = max(a.x, b.x);
    float right = min(a.x + a.width, b.x + b.width);
    float top = max(a.y, b.y);
    float bottom = max(a.y + a.height, b.y + b.height);

    if(left > right || top > bottom)
        return 0;

    float area = (right - left)*(bottom - top);
    return area / (a.area() + b.area() - area);
}

void Sequence::resetID(int start_frame, int end_frame, int old_ID, int new_ID)
{
    for(int i = start_frame; i < end_frame; i++)
    {
        for(int j = 0; j < res[i].size(); j++)
        {
            if(res[i][j].ID == old_ID)
            {
                res[i][j].ID = new_ID;
                break;
            }
        }
    }
}


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

    N = N * 0.05;
    cache_count = N;
    if(N * height * width * 3 / (1024*1024) > IMAGE_CHCHE)
        cache_count = IMAGE_CHCHE*1024*1024 / (height * width * 3);

    image_cache = std::vector<cv::Mat>(cache_count);

    for(int i = 0; i < cache_count; i++)
    {
        capture >> image_cache[i];
    }

    if(res_file == NULL)
    {
        capture.set(CV_CAP_PROP_POS_FRAMES, 0);
        for(int i = 0; i < N; i++)
        {
            Mat img;
            capture >> img;
            std::vector<Face> res_tmp;
            facetools.detectAndalign(img, res_tmp);
            res.push_back(res_tmp);
        }
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
                fscanf(fp,"%f %f %f %f", &res_tmp[j].rect.x, &res_tmp[j].rect.y, &res_tmp[j].rect.width, &res_tmp[j].rect.height);
                res_tmp[j].alignments = std::vector<Point2f>(ALI_POINTS_NUM);
                for(int k = 0; k < ALI_POINTS_NUM; k++)
                {
                    fscanf(fp, "%f %f", &res_tmp[j].alignments[k].x, &res_tmp[j].alignments[k].y);
                }
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
        rectangle(image, res[i][k].rect, CV_RGB(255,0,0), 2);
        for(int j = 0; j < ALI_POINTS_NUM; j++)
        {
            circle(image, res[i][k].alignments[j], 0, CV_RGB(0,255,0), 2);
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
    cv::Mat image;

    if(i < cache_count)
        return image_cache[i];


    capture.set(CV_CAP_PROP_POS_FRAMES, i);
    capture.read(image);
    return image;
}

void Sequence::labeling(int FaceID, int PointID, QPointF loc)
{
    res[n][FaceID].alignments[PointID] = Point(int(loc.x()), int(loc.y()));
    update();
}

void Sequence::update()
{
    return;
}


void Sequence::add_face()
{
    Face face;
    face.rect.x = width*2/5;
    face.rect.y = height*2/5;
    face.rect.width = min(height, width) / 5;
    face.rect.height = face.rect.width;
}

void Sequence::delete_face(int FaceID)
{
    res[n].erase(res[n].beign() + FaceID, res[n].beign() + FaceID + 1);
}

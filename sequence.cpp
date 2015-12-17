#include "sequence.h"
#include <QMessageBox>

using namespace cv;

Sequence::Sequence(std::string filename)
{
    capture.open(filename);
    if(!capture.isOpened())
    {
        qDebug("Fail to open video!");
        return;
    }

    N = capture.get(CV_CAP_PROP_FRAME_COUNT);
    height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    width = capture.get(CV_CAP_PROP_FRAME_WIDTH);

    N = N * 0.05;
    cache_count = N;
    if(N * height * width * 3 / (1024*1024) > IMAGE_CHCHE)
        cache_count = IMAGE_CHCHE*1024*1024 / (height * width * 3);

    image_cache = std::vector<cv::Mat>(cache_count);

    for(int i = 0; i < cache_count; i++)
    {
        capture >> image_cache[i];
    }

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
            circle(image, res[i][k].alignments[j], 2, CV_RGB(0,255,0), 2);
        }
    }

    return mat2Qimg(image);
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

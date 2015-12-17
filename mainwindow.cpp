#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setGeometry(QRect(20,40,800,600));

//    seq = new Sequence("test2.avi");
    pos_now = 0;

//    show_scale = double(IMAGELABEL_SIZE) / max(seq->frame_height(), seq->frame_wdith());
//    show_height = show_scale * seq->frame_height();
//    show_width = show_scale * seq->frame_wdith();

    image_label = new MyQLabel(this);
    image_label->setMouseTracking(true);
    image_label->setStyleSheet("border-width: 4px;border-style: solid;border-color: rgb(0, 0, 0);");
    connect(image_label, SIGNAL(click(int,int)), this, SLOT(image_label_click(int,int)) );
    connect(image_label, SIGNAL(move(int,int)), this, SLOT(image_label_move(int,int)) );
    connect(image_label, SIGNAL(release(int,int)), this, SLOT(image_label_release(int,int)) );
    connect(image_label, SIGNAL(roll(double)), this, SLOT(image_label_roll(double)) );

    image_label->load_sequence("test2.avi");
    N = image_label->get_N();
    int height = image_label->get_frame_height();
    int width = image_label->get_frame_width();
    qreal scale = qreal(IMAGELABEL_SIZE) / cv::max(height, width);
    image_label->setGeometry(20 + (IMAGELABEL_SIZE - width*scale)/2, 20 + (IMAGELABEL_SIZE - height*scale)/2, width*scale, height*scale);


    slider = new QSlider(Qt::Horizontal, this);
    slider->setGeometry(QRect(80, 520, 350, 20));
    slider->setMinimum(0);
    slider_label = new QLabel(this);
    slider_label->setGeometry(QRect(10, 520, 60, 20));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slider_change()));



    //frame_show = seq->frame_draw(pos_now);
    //cv::resize(frame_show, frame_show, Size(show_width, show_height));

    slider->setMaximum(N - 1);
    slider->setValue(0);
    slider_label->setText(QString::number(pos_now) + "/" + QString::number(N-1));

    image_label->update();
//    this->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::paintEvent(QPaintEvent *)
//{
////    Mat img_rgb;
////    QImage qimg;

////    if(frame_show.channels() == 3)
////    {
////            cvtColor(frame_show, img_rgb, CV_BGR2RGB);
////            qimg = QImage( (const unsigned char*)(img_rgb.data),
////                           img_rgb.cols,
////                           img_rgb.rows,
////                           img_rgb.cols*img_rgb.channels(),
////                           QImage::Format_RGB888 );
////    }
////    else
////    {
////            qimg = QImage( (const unsigned char*)(frame_show.data),
////                           img_rgb.cols,
////                           img_rgb.rows,
////                           img_rgb.cols*img_rgb.channels(),
////                           QImage::Format_RGB888 );
////    }
////    image_label->setPixmap(QPixmap::fromImage(qimg));
//    //QPainter painter(image_label);
//    //image_label->resize(image_label->pixmap()->size());
//}

void MainWindow::image_label_click(int x, int y)
{
//    circle(frame_show, Point(x, y),2 , Scalar(255,0,0), 2);
//    this->update();
    return;
}

void MainWindow::image_label_move(int x, int y)
{
    return;
}

void MainWindow::image_label_release(int x, int y)
{
    return;
}

void MainWindow::image_label_roll(double delta, int x, int y)
{
//    double scale_roll;
//    cv::pow(TOLLING_FACTOR, delta, scale_roll);
//    show_roi.width = show_roi.width / scale_roll;
//    show_roi.height = show_roi.height / scale_roll;
//    show_roi.x = show_roi.x + show_roi.width*(scale_roll - 1) * ( (double(x) + image_location.x)/IMAGELABEL_SIZE );
//    show_roi.y = show_roi.y + show_roi.height*(scale_roll - 1) * ( (double(y) + image_location.y)/IMAGELABEL_SIZE);

//    int left, right, top, bottom;
//    left = max(show_roi.x, 0);
//    right = min(show_roi.x + show_roi.width, show_width);
//    top = max(show_roi.y, 0);
//    bottom = max(show_roi.y + show_roi.height, show_height);

//    Rect roi_original;
//    roi_original.x = left / show_scale;
//    roi_original.y = top / show_scale;
//    roi_original.width = (right - left) / show_scale;
//    roi_original.height = (bottom - top) / show_scale;

//    frame_show = seq->frame_draw(i)(roi_original);

//    cv::resize(frame_show, frame_show, Size(new_height, new_width);
    return;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space)
    {
        if(pos_now < N)
        {
            pos_now++;
            slider->setValue(pos_now);
        }
    }
}


void MainWindow::slider_change()
{
    pos_now = slider->value();
    image_label->set_n(pos_now);
    //frame_show = seq->frame_draw(pos_now);
    //cv::resize(frame_show, frame_show, Size(show_width, show_height));
    slider_label->setText(QString::number(pos_now) + "/" + QString::number(N-1));
    image_label->update();
}


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv2/opencv.hpp>
#include "sequence.h"

#include <QMainWindow>
#include <QKeyEvent>
#include <QSlider>
#include <QPainter>
#include <QLayout>

#include <myqlabel.h>

using namespace cv;

#define IMAGELABEL_SIZE 480
#define TOLLING_FACTOR 1.1

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

//    Mat frame_show;

//    Sequence *seq;
    int pos_now;
    int N;
//    double show_scale;
//    int show_height;
//    int show_width;
//    cv::Point image_location;

//    cv::Rect show_roi;

    MyQLabel *image_label;

    QSlider *slider;
    QLabel *slider_label;


protected:
//    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void image_label_click(int x, int y);
    void image_label_move(int x, int y);
    void image_label_release(int x, int y);
    void image_label_roll(double delta, int x, int y);

    void slider_change();
};

#endif // MAINWINDOW_H

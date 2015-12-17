#ifndef MYQLABEL_H
#define MYQLABEL_H

#include <QLabel>
#include <QRectF>
#include <QPainter>
#include <QFrame>
#include "sequence.h"

class MyQLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyQLabel(QWidget *parent = 0);

    int get_N();
    int get_frame_height();
    int get_frame_width();
    void set_n(int m);
    void load_sequence(std::string filename);

signals:
    void click(int, int);
    void move(int, int);
    void release(int, int);
    void roll(double, int, int);

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

    void paintEvent(QPaintEvent *e);

private:
    Sequence *seq;

    int n; //frame number now
    QRectF window;

public slots:
};

#endif // MYQLABEL_H

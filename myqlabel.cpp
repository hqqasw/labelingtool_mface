#include "myqlabel.h"
#include <QMouseEvent>

MyQLabel::MyQLabel(QWidget *parent) : QLabel(parent)
{
    n = 0;
}

void MyQLabel::load_sequence(std::string filename)
{
    seq = new Sequence(filename);
}

void MyQLabel::mouseMoveEvent(QMouseEvent *ev)
{
    emit move(ev->pos().x(), ev->pos().y());
}

void MyQLabel::mousePressEvent(QMouseEvent *ev)
{
    emit click(ev->pos().x(), ev->pos().y());
}

void MyQLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    emit release(ev->pos().x(), ev->pos().y());
}

void MyQLabel::wheelEvent(QWheelEvent *ev)
{
    emit roll(ev->angleDelta().y() / double(120), ev->pos().x(), ev->pos().y());
}

void MyQLabel::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);

    QPainter painter(this);
    painter.setWindow(0,0,seq->get_frame_width(), seq->get_frame_height());
    painter.drawImage(0,0,seq->get_frame(n));

}

void MyQLabel::set_n(int m)
{
    if(m >= 0 && m < seq->get_frame_N())
        n = m;
}

int MyQLabel::get_N()
{
    return seq->get_frame_N();
}

int MyQLabel::get_frame_height()
{
    return seq->get_frame_height();
}

int MyQLabel::get_frame_width()
{
    return seq->get_frame_width();
}

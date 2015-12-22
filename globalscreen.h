#ifndef GLOBALSCREEN_H
#define GLOBALSCREEN_H

#include "common.h"
#include "sequence.h"

class GlobalScreen : public QFrame
{
    Q_OBJECT

public:
    GlobalScreen(Sequence *seq, QWidget *parent);
    ~GlobalScreen();

    QString hint();
    void play();
    void stop();

signals:
    void changed();

protected:
    void paintEvent(QPaintEvent *e);
    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);

private:
    enum FlagDrag { Empty, Global };
    QPointF toGlobal(QPointF p);

private:
    Sequence *s;
    FlagDrag flag_drag;
    QPointF start_pos;
    bool playing;
};

#endif // GLOBALSCREEN_H

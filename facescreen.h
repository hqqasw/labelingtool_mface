#ifndef FACESCREEN_H
#define FACESCREEN_H

#include "common.h"
#include "sequence.h"

class FaceScreen : public QFrame
{
    Q_OBJECT

public:
    FaceScreen(Sequence *seq, QWidget *parent);
    ~FaceScreen();

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
    QPointF toGlobal(QPointF p);
    int targetF(const std::vector<Face> faces, QPointF gp);
    int targetP(const Face face, QPointF gp, qreal limit);

private:
    Sequence *s;
    QPointF start_pos;
    bool playing;
    QString hints;

    int PointID;
    int FaceID;
};

#endif // FACESCREEN_H

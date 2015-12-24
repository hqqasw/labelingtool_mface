#include "facescreen.h"

FaceScreen::FaceScreen(Sequence *seq, QWidget *parent):
    QFrame(parent), s(seq), playing(false), adding(false)
{
    setFrameStyle(WinPanel | Sunken);
    setLineWidth(0);
    setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(changed()), parent, SLOT(refresh()));

    FaceID = 0;
    PointID = -1;
    seq->set_FaceID(FaceID);
}

FaceScreen::~FaceScreen()
{

}

QString FaceScreen::hint()
{
    return hints;
}

void FaceScreen::play()
{
    playing = true;
}

void FaceScreen::stop()
{
    playing = false;
}

void FaceScreen::add()
{
    setCursor(Qt::CrossCursor);
    adding = true;
}

void FaceScreen::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    QRectF &c = s->get_camera();

    QPainter painter(this);
    painter.drawImage(QRectF(QPointF(), size()), s->get_frame(), c);
    painter.setWindow(c.toRect());

}

void FaceScreen::wheelEvent(QWheelEvent *e)
{
    if (playing)
        return;

    QRectF &c = s->get_camera();
    qreal f = qPow(ScaleFactor, e->angleDelta().y() / qreal(120));
    qreal x = c.x() - c.width() * (f - 1) / 2;
    qreal y = c.y() - c.height() * (f - 1) / 2;
    qreal w = c.width() * f, h = c.height() * f;
    if (f < 1 && w < 16 && h < 16)
        return;
    if (f > 1 && w > s->get_frame_width() && h > s->get_frame_height())
        return;

    c = QRectF(x, y, w, h);
    //hints = "Camera : " + toString(s->camera());
    emit changed();
}

void FaceScreen::mousePressEvent(QMouseEvent *e)
{
    if (playing)
        return;

    QPointF p = e->localPos();
    QPointF gp = toGlobal(p);
    const QRectF &c = s->get_camera();
    qreal scale = qScale(size(), c.size());

    if(adding)
    {
        s->add_face_press(gp);
        emit changed();
        return;
    }

    FaceID = targetF(s->res[s->get_n()], gp);
    s->set_FaceID(FaceID);
    if(FaceID == -1)
    {
        PointID = -1;
        return;
    }
    PointID = targetP(s->res[s->get_n()][FaceID], gp, 8 * scale);
    if (PointID != -1)
    {
        start_pos = gp;
        //hints = names[flag_drag] + " : " + toString(gp);
    }
//    else
//    {
//        int besti = -1;
//        qreal bestD = qInf();
//        QPointF align;
//        for (int i = 0; i < ALI_POINTS_NUM; ++i)
//        {
//            align = QPointF(s->res[s->get_n()][FaceID].alignments[i].x, s->res[s->get_n()][FaceID].alignments[i].y);
//            qreal D = cal_L2(align - gp);
//            if (D < bestD) {
//                besti = i;
//                bestD = D;
//            }
//        }
//        s->labelingone(FaceID, besti, gp);
//        //hints = names[besti] + " : " + toString(gp);
//    }
    emit changed();
    return;
}

void FaceScreen::mouseMoveEvent(QMouseEvent *e)
{
    if (playing)
        return;

    QPointF p = e->localPos();
    QPointF gp = toGlobal(p);

    if(adding)
    {
        s->add_face_move(gp);
        emit changed();
        return;
    }

    if (PointID == -1 || !QRectF(QPointF(), size()).contains(p))
        return;

    s->labelingone(FaceID, PointID, gp);
    start_pos = gp;
    emit changed();
    return;
}

void FaceScreen::mouseReleaseEvent(QMouseEvent *)
{
    PointID = -1;
    if(adding)
    {
        setCursor(Qt::WaitCursor);
        s->add_face_release();
        adding = false;
        setCursor(Qt::ArrowCursor);
        emit changed();
    }
    return;
}

QPointF FaceScreen::toGlobal(QPointF p)
{
    const QRectF &c = s->get_camera();
    qreal x = p.x() / width() * c.width() + c.x();
    qreal y = p.y() / height() * c.height() + c.y();
    return QPointF(x, y);
}

int FaceScreen::targetF(const std::vector<Face> faces, QPointF gp)
{
    qreal min_dis = 1000000;
    qreal dis;
    int idx = -1;
    for (int i = 0; i < faces.size(); i++)
    {
        QPointF center(faces[i].rect.x + faces[i].rect.width, faces[i].rect.y + faces[i].rect.height);
        dis = cal_L2(center - gp);
        if (dis <= min_dis)
        {
            idx = i;
            min_dis = dis;
        }
    }
    return idx;
}

int FaceScreen::targetP(const Face face, QPointF gp, qreal limit)
{
//    QPointF corner_rb(face.rect.x + face.rect.width, face.rect.y + face.rect.height);
//    if((cal_L2(corner_rb - gp)) <= limit)
//        return -2;

    for (int i = 0; i < ALI_POINTS_NUM; i++)
    {
        QPointF p(face.alignments[i].x, face.alignments[i].y);
        if (cal_L2(p - gp) <= limit)
            return i;
    }
    return -1;
}



#include "globalscreen.h"

GlobalScreen::GlobalScreen(Sequence *seq, QWidget *parent):
    QFrame(parent), s(seq), flag_drag(Empty), playing(false)
{
    setFrameStyle(WinPanel | Sunken);
    setLineWidth(0);
    setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(changed()), parent, SLOT(refresh()));
}

GlobalScreen::~GlobalScreen()
{

}

QString GlobalScreen::hint()
{
    return "camera : " + toString(s->get_camera());
}

void GlobalScreen::play()
{
    flag_drag = Empty;
    playing = true;
}

void GlobalScreen::stop()
{
    playing = false;
}

void GlobalScreen::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);

    const QRectF &c = s->get_camera();
    qreal scale = qScale(size(), s->get_size());

    QPainter painter(this);
    painter.setWindow(0, 0, s->get_frame_width(), s->get_frame_height());

    painter.drawImage(0, 0, s->get_frame());

    painter.setPen(QPen(Qt::white, 2 * scale));
    painter.drawRect(c);
}

void GlobalScreen::wheelEvent(QWheelEvent *e)
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
    emit changed();
}

void GlobalScreen::mousePressEvent(QMouseEvent *e)
{
    if (playing)
        return;

    QPointF p = e->localPos();
    QPointF gp = toGlobal(p);
    QRectF &c = s->get_camera();

    if (c.contains(gp)) {
        flag_drag = Global;
        start_pos = gp;
        emit changed();
    }else {
        c.moveCenter(gp);
        emit changed();
    }
}

void GlobalScreen::mouseMoveEvent(QMouseEvent *e)
{
    if (playing)
        return;

    QPointF p = e->localPos();
    QPointF gp = toGlobal(p);
    QRectF &c = s->get_camera();

    if (flag_drag == Empty || !QRectF(QPointF(), size()).contains(p))
        return;
    c.translate(gp - start_pos);
    start_pos = gp;
    emit changed();
}

void GlobalScreen::mouseReleaseEvent(QMouseEvent *)
{
    if (playing)
        return;
    flag_drag = Empty;
}

QPointF GlobalScreen::toGlobal(QPointF p)
{
    return p * qScale(size(), s->get_size());
}


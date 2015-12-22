#include "common.h"

qreal qScale(QSizeF from, QSizeF to)
{
    qreal sw = to.width() / from.width();
    qreal sh = to.height() / from.height();
    return sw * sh < 1 ? min(sw, sh) : max(sw, sh);
}

qreal cal_L2(QPointF p)
{
    return qSqrt(p.x() * p.x() + p.y() * p.y());
}

qreal cal_L2(QLineF l, QPointF p)
{
    if (QPointF::dotProduct(p - l.p1(), l.p2() - l.p1()) < 0)
        return cal_L2(p - l.p1());
    if (QPointF::dotProduct(p - l.p2(), l.p1() - l.p2()) < 0)
        return cal_L2(p - l.p2());
    QPointF a(p - l.p1()), b(l.p2() - l.p1());
    return qAbs(a.x() * b.y() - a.y() * b.x()) / cal_L2(b);
}

QString toString(QPoint p)
{
    return QString("(%1, %2)").arg(p.x()).arg(p.y());
}

QString toString(QPointF p)
{
    return toString(p.toPoint());
}

QString toString(QRect r)
{
    return QString("%1x%2").arg(r.width()).arg(r.height()) + " from " + toString(r.topLeft());
}

QString toString(QRectF r)
{
    return toString(r.toRect());
}

QRectF toQrect(cv::Rect rect)
{
    return QRectF(qreal(rect.x), qreal(rect.y), qreal(rect.width), qreal(rect.height));
}

std::string Qstr2stdstr(QString qstring)
{
   return std::string((const char *)qstring.toLocal8Bit());
}




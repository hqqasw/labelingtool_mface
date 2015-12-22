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

QRectF toQrect(cv::Rect_<float> rect)
{
    return QRectF(qreal(rect.x), qreal(rect.y), qreal(rect.width), qreal(rect.height));
}

std::string Qstr2stdstr(QString qstring)
{
   return std::string((const char *)qstring.toLocal8Bit());
}

std::vector<cv::Point2f> get_meanpose(cv::Rect_<float> rect)
{
    vector<Point2f> mp(ALI_POINTS_NUM);
    mp[0] = Point2f(25.962151, 28.726925);
    mp[1] = Point2f(36.099197, 26.162285);
    mp[2] = Point2f(47.152222, 28.518116);
    mp[3] = Point2f(65.369766, 28.148701);
    mp[4] = Point2f(76.378151, 25.379211);
    mp[5] = Point2f(86.546562, 27.484493);
    mp[6] = Point2f(31.189026, 37.995956);
    mp[7] = Point2f(46.208347, 38.581623);
    mp[8] = Point2f(67.203201, 38.256996);
    mp[9] = Point2f(81.905579, 37.147034);
    mp[10] = Point2f(46.116287, 60.843781);
    mp[11] = Point2f(56.860767, 66.029083);
    mp[12] = Point2f(67.787758, 60.602333);
    mp[13] = Point2f(56.964706, 73.013077);
    mp[14] = Point2f(57.094910, 78.987343);
    mp[15] = Point2f(57.213093, 85.184311);
    mp[16] = Point2f(38.684288,	37.281460);
    mp[17] = Point2f(74.115799,	36.684723);
    mp[18] = Point2f(56.433048,	58.983799);
    mp[19] = Point2f(41.968956,	76.822403);
    mp[20] = Point2f(71.915581,	76.250763);


    float scale = rect.width / 112;

    for(int i = 0; i < ALI_POINTS_NUM; i++)
    {
        mp[i] *= scale;
        mp[i].x += rect.x;
        mp[i].y += rect.y;
    }

    return mp;
}




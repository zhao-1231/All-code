#include "basicitem.h"

BasicItem::BasicItem(QImage*& theImage,QObject* parent) : QObject(parent),m_showImage(theImage)
{
    //QImage(const uchar *data, int width, int height, int bytesPerLine, Format format
    m_backGroundCache = new uchar[2560*1440]{};
    m_backGroundImage = new QImage{m_backGroundCache,2560,1440,2560,QImage::Format_Grayscale8};
    scaleVal = 1.;
}

BasicItem::~BasicItem()
{
    delete[] m_backGroundCache;
    delete m_backGroundImage;

}

QRectF BasicItem::boundingRect() const
{
    if(m_showImage != nullptr)
        return QRectF{0.,0.,qreal(m_showImage->width()),qreal(m_showImage->height())};
    else
        return QRectF{0.,0.,2560.,1440.};
}

void BasicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::RenderHint::Antialiasing);
    if(m_showImage==nullptr){
        QRectF target{0.,0.,2560,1440};
        painter->drawImage(target,*m_backGroundImage,target);
    }
    else{
        QRectF target{0.,0.,qreal(m_showImage->width()),qreal(m_showImage->height())};
        painter->drawImage(target,*m_showImage,target);
    }
}

void BasicItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    //qreal qrealOriginScale = scaleVal;
    if(event->delta() > 0 && scaleVal <= 50.)   //滚轮上滑
    {
        scaleVal *= 1.2;
    }
    else if(event->delta() < 0 && scaleVal >= 0.1)   //滚轮下滑
    {
        scaleVal *= 0.8;

    }
    setScale(scaleVal);
    //emit setScene2Fit();
}

void BasicItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        startPoint = event->pos();
        mousePressed = true;
    }
}


void BasicItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF curPos = event->pos();
    QPointF tmpdiff = (curPos - startPoint) * scaleVal;
    diff = diff + tmpdiff / scaleVal;
    if(mousePressed && m_showImage != nullptr){
        moveBy(tmpdiff.x(),tmpdiff.y());
    }
}

void BasicItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        mousePressed = false;
    }
}

void BasicItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        setPos(0.,0.);
    }
}

















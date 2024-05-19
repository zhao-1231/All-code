#ifndef BASICITEM_H
#define BASICITEM_H

#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPainter>
#include <QDebug>
#pragma execution_character_set("utf-8")

class BasicItem : public QObject , public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    BasicItem(QImage*& theImage,QObject* parent = nullptr);
    ~BasicItem();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
private:
    QImage*& m_showImage;   //用于图像的显示,传递引用,避免不必要的麻烦
    QImage* m_backGroundImage;
    uchar* m_backGroundCache;
    double scaleVal;
    QPointF startPoint;
    bool mousePressed{false};
    QPointF diff{};
};

#endif // BASICITEM_H

#include "busyindicator.h"
#include <QPainter>
#include <QDebug>

BusyIndicator::BusyIndicator(QWidget *parent) :
    QDialog(parent),
    pixmap(":/icon/icons/progressindicator_big@2x.png"),
    startAngle(0),
    isStop(false)
{

    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setAutoFillBackground(true);
    setFixedSize(pixmap.size());
    timer.setInterval(50);
    connect(&timer, &QTimer::timeout,[=](){
        startAngle += 30;
        startAngle %= 360;
        update();
    });
    setModal(true);
    timer.start();

}

void BusyIndicator::onRotate()
{
    startAngle += 30;
    startAngle %= 360;
    update();
}

void BusyIndicator::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    drawPixmapType(&painter);
}

void BusyIndicator::drawPixmapType(QPainter *painter)
{
    QPointF center(pixmap.width() / qreal(2), pixmap.height() / qreal(2));
    painter->translate(center);
    painter->rotate(startAngle);
    painter->translate(-center.x(),-center.y());
    painter->drawPixmap(center/2, pixmap);

}



#include "busyindicator.h"
#include <QPainter>
#include <QDebug>

BusyIndicator::BusyIndicator(QWidget *parent) :
    QDialog(parent),
    pixmap(":/icon/icons/progressindicator_big@2x.png"),
    startAngle(0)
{

    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);
    this->setAutoFillBackground(true);
    setFixedSize(pixmap.size());
    timer.setInterval(50);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onRotate()));
    timer.start();
    setModal(true);   

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
    qDebug() << " draw indictor ";
}


void BusyIndicator::onRotate()
{
    startAngle += 30;
    startAngle %= 360;
    update();
}

BusyIndicator::~BusyIndicator(){
    this->deleteLater();
}


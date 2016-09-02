#ifndef BUSYINDICATOR_H
#define BUSYINDICATOR_H
#include <QPixmap>
#include <QWidget>
#include <QTimer>
#include <QDialog>



class BusyIndicator : public QDialog
{
    Q_OBJECT
public:
    explicit BusyIndicator(QWidget *parent = 0);
    QString bkimg;
    QPixmap pixmap;
    QTimer timer;
    int startAngle;

    void drawPixmapType(QPainter *painter);

signals:

public slots:
    void onStart(){ timer.start();exec();}
    void onStop(){timer.stop();this->accept();}
    void onRotate();


protected:
    void paintEvent(QPaintEvent *);
    bool isStop;
};

#endif // BUSYINDICATOR_H

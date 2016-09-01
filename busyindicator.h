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
    ~BusyIndicator();
    QString bkimg;
    QPixmap pixmap;
    QTimer timer;
    int startAngle;

    void drawPixmapType(QPainter *painter);

signals:

public slots:
    void onRotate();

protected:
    void paintEvent(QPaintEvent *);
};

#endif // BUSYINDICATOR_H

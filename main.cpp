#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
   // w.show();
     QDesktopWidget *pDwgt = a.desktop();
    QRect desk_rect = pDwgt->screenGeometry(pDwgt->screenNumber(QCursor::pos()));
    qDebug() << "desk rect is " << desk_rect;
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();

    w.setMaximumSize(desk_rect.size());
    w.showMaximized();


    return a.exec();
}

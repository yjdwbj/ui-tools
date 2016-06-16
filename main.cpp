#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
   // w.show();
     QDesktopWidget *pDwgt = a.desktop();
    QRect desk_rect = pDwgt->screenGeometry(pDwgt->screenNumber(QCursor::pos()));


    w.setMaximumSize(desk_rect.size());
    w.showMaximized();


    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QPushButton>

#include <QGraphicsView>
#include <QGraphicsGridLayout>
#include <QGraphicsItem>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

     QDesktopWidget *pDwgt = a.desktop();
    QRect desk_rect = pDwgt->screenGeometry(pDwgt->screenNumber(QCursor::pos()));


    w.setMaximumSize(desk_rect.size());
    w.showMaximized();
    w.show();




    return a.exec();
}

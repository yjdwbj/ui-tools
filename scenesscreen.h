#ifndef SCENESSCREEN_H
#define SCENESSCREEN_H

#include <QObject>
#include <QWidget>
#include <QFrame>

//#include "mainwindow.h"
class MainWindow;
class NewLayer;
class Compoentcontrol;
class ScenesScreen : public QFrame
{
    friend class Compoentcontrol;
    Q_OBJECT
public:
    explicit ScenesScreen(QSize size,QWidget *parent = 0);
    NewLayer *activeLayer() { return (NewLayer*)(LayerList.at(mActiveIdx));}
    void createNewLayer();
    void addMainWindow(MainWindow* m) { mWindow = m;}
    MainWindow* mWindow;

signals:

public slots:

private:

   QWidgetList LayerList;

   int mActiveIdx; //  当前激活的图层.


};

#endif // SCENESSCREEN_H

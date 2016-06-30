#ifndef SCENESSCREEN_H
#define SCENESSCREEN_H

#include <QObject>
#include <QWidget>
#include <QFrame>

//#include "mainwindow.h"
class MainWindow;
class NewLayout;
class Compoentcontrol;
class FormResizer;
class ScenesScreen : public QFrame
{
    friend class Compoentcontrol;
    Q_OBJECT
public:
    explicit ScenesScreen(QSize size,QWidget *parent = 0);
    ~ScenesScreen();
    NewLayout *activeLayer() {
        return mActiveIdx == -1 ? (NewLayout*)0 :
                                  (NewLayout*)(LayoutList.at(mActiveIdx));}
    int getActiveIndex() { return mActiveIdx;}
    void createNewLayout();
    void addMainWindow(MainWindow* m) { mWindow = m;}
    void setActiveIdx(int index) { mActiveIdx = index;}
    void setSelectObject(FormResizer *obj);
    void delSelectedLayout();
    void delSelectedObject();

    void writeToJson(QJsonObject &json);

    void delAllObjects();

    QWidgetList LayoutList;
    MainWindow* mWindow;
    NewLayout* activeObj;

signals:

public slots:

private:
   void keyReleaseEvent(QKeyEvent *);
   int mActiveIdx; //  当前激活的图层.


};

#endif // SCENESSCREEN_H

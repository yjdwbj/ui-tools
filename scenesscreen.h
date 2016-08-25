#ifndef SCENESSCREEN_H
#define SCENESSCREEN_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QListWidgetItem>
#include <QJsonValue>

//#include "mainwindow.h"
class MainWindow;
class NewLayout;
class NewLayer;
class Compoentcontrol;
class FormResizer;
class Compoent;
class ScenesScreen : public QFrame
{
    friend class Compoentcontrol;
    Q_OBJECT
public:
    explicit ScenesScreen(QSize size,QWidget *parent = 0);
    ~ScenesScreen();


    QWidget *activeObject() {
        return activeObj;
    }

    void removeActiveObj() { activeObj = 0;}
    NewLayer *createNewLayer(const QJsonValue &json, bool createflag);
    void addMainWindow(MainWindow* m) { mWindow = m;}
   // void setActiveIdx(int index) { mActiveIdx = index;}
    void setSelectObject(FormResizer *obj);
    //void delSelectedLayout();
   // void delSelectedLayer();
    void readLayer(const QJsonArray &array);

    void pasteItem(QWidget *w);


    QJsonObject writeToJson();

    void delAllObjects();

    //QWidgetList LayoutList;
    QWidgetList LayerList;
    MainWindow* mWindow;

    //QString mCopyItem;



signals:

public slots:
    void onChangedBackgroundColor();
    void onChangedBackgroundImage();
    void onDobuleClickedImage(QListWidgetItem*);

private:
   void keyReleaseEvent(QKeyEvent *);
   int mActiveIdx; //  当前激活的布局.
  // int mActiveLaySeq;

   QWidget *activeObj;
protected:
   void mousePressEvent(QMouseEvent *);

};

#endif // SCENESSCREEN_H

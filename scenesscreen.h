#ifndef SCENESSCREEN_H
#define SCENESSCREEN_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QListWidgetItem>

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

//    NewLayer *activeLayer() {
//        if(LayerList.size())
//        {
//            return (NewLayer *)(LayerList.at(mActiveLaySeq));
//        }
//        else return 0;
     //   return mActiveLaySeq == -1 ? 0 : (NewLayer*)(LayerList.at(mActiveLaySeq));
//   }
    QWidget *activeObject() {
        return activeObj;
    }


    NewLayer *createNewLayer(const QJsonObject &json);
    void addMainWindow(MainWindow* m) { mWindow = m;}
   // void setActiveIdx(int index) { mActiveIdx = index;}
    void setSelectObject(FormResizer *obj);
    //void delSelectedLayout();
   // void delSelectedLayer();
    void readLayer(const QJsonArray &array);


    QJsonObject writeToJson();

    void delAllObjects();

    //QWidgetList LayoutList;
    QWidgetList LayerList;
    MainWindow* mWindow;


signals:

public slots:
    void onChangedBackgroundColor();
    void onChangedBackgroundImage();
    void onDobuleClickedImage(QListWidgetItem*);

private:
   void keyReleaseEvent(QKeyEvent *);
   int mActiveIdx; //  当前激活的布局.
   int mActiveLaySeq;

   QWidget *activeObj;
protected:
   void mousePressEvent(QMouseEvent *);

};

#endif // SCENESSCREEN_H

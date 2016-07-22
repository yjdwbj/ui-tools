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
//    NewLayout *activeLayout() {
//        return mActiveIdx == -1 ? (NewLayout*)0 :
//                                  (NewLayout*)(LayoutList.at(mActiveIdx));}
    NewLayer *activeLayer() {
        return mActiveLaySeq == -1 ? 0 : (NewLayer*)(LayerList.at(mActiveLaySeq));
    }
    QWidget *activeObject() {
        return activeObj;
    }

   // int getActiveIndex() { return mActiveIdx;}
    int getActiveLayerIndex() { return mActiveLaySeq;}
   // void createNewLayout();
    NewLayer *createNewLayer(const QJsonObject &json);
    void addMainWindow(MainWindow* m) { mWindow = m;}
   // void setActiveIdx(int index) { mActiveIdx = index;}
    void setSelectObject(FormResizer *obj);
    //void delSelectedLayout();
   // void delSelectedLayer();
    void readLayer(const QJsonArray &array);
  //  QRect getRectFromStruct(const QJsonArray &arr) const;
   // void delSelectedObject();

    void writeToJson(QJsonObject &json);

    void delAllObjects();

    //QWidgetList LayoutList;
    QWidgetList LayerList;
    MainWindow* mWindow;


signals:

public slots:
    void onDeleteMySelf();
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

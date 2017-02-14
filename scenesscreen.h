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
class HVLineWidget;

class ScenesScreen : public QFrame
{
    friend class Compoentcontrol;
    Q_OBJECT
public:
    explicit  ScenesScreen(QSize size,QWidget *parent = 0);
    ~ScenesScreen();


    QWidget *activeObject() {
        return activeObj;
    }

    void removeActiveObj() { activeObj = 0;}
    NewLayer *createNewLayer(const QJsonValue &json, bool createflag);
    void addMainWindow(MainWindow* m) { mWindow = m;}
    void setSelectObject(FormResizer *obj);
    void readLayer(const QJsonArray &array);

    void pasteItem(QWidget *w);


    QJsonObject writeToJson();

    void delAllObjects();

    QList<QWidget*> childlist;
    MainWindow* mWindow;
    HVLineWidget *mLine;

signals:

public slots:
    void onChangedBackgroundColor();
private:
    void keyReleaseEvent(QKeyEvent *);
//    int mActiveIdx; //  当前激活的布局.

    QWidget *activeObj;
//    QLine mVLine;
//    QLine mHLine;



protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void dropEvent(QDropEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dragMoveEvent(QDragMoveEvent *);
//    void mouseMoveEvent(QMouseEvent *e);
//    void paintEvent(QPaintEvent *e);


};

#endif // SCENESSCREEN_H

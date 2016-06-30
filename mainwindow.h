#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFrame>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QListWidgetItem>

#include <QDebug>
#include <QLabel>
#include <QDir>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMdiArea>
#include <QtCore/QMargins>
#include <QMouseEvent>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMessageBox>
#include <QScrollArea>
#include <QTreeWidget>
#include "treedock.h"
//#include "propertybox.h"


class TreeDock;
//class PropertyBox;
class ScenesScreen;
class CanvasManager;
class PageView;
class CompoentControls;
class ComProperty;
class ImgProperty;
class Position;



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{

    friend class NewLabel;
    friend class ImageLayer;
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void getJsonValue(QJsonValue qjv);

    void HandleObject(QJsonObject qjo);
    void HandleArrayObject(QJsonArray array);
    void setWidget(QObject &oob);
    bool eventFilter(QObject *obj, QEvent *event);
    void addWidgetToToolBar(QWidget *);

    //QGroupBox *propertyWidget;

    ComProperty *propertyWidget;
    //QGroupBox *imgPropertyWidget;
    ComProperty *imgPropertyWidget;
    Position *posWidget;
    QFrame *mCanvas;
    TreeDock *tree;
    ScenesScreen *Scenes;

    QWidgetList *ssList;
    CanvasManager *cManager;
    QDockWidget* lDock;
    PageView *pageView;
    CompoentControls *ComCtrl;



//private slots:
//    void onCreateNewScenesScreen();
//    void onDelCurrentScenesScreen();
//    void onCreateNewProject();

private:
    void mousePressEvent(QMouseEvent *ev);
    Ui::MainWindow *ui;

    QDockWidget* rDock;

    QListWidget* rList;
    QListWidget* lList;
    QTextStream out;


    enum KType {
        Null =  0x0,
        Bool = 0x1,
        Double = 0x2,
        String = 0x3,
        Array = 0x4,
        Object = 0x5,
        Undefined = 0x80
    };

    void HandleFrameObject(QJsonObject qjo, QString ParentName);
    QWidget *pWin;



};

#endif // MAINWINDOW_H

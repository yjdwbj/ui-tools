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
#include <QPainter>
#include <QSettings>
#include "treedock.h"

#include "busyindicator.h"

#include <QRunnable>
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

class Backgroud: public QWidget
{
public:
    QPixmap backImage;

protected:
    void paintEvent(QPaintEvent *);
};

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
  //  bool eventFilter(QObject *obj, QEvent *event);
    void addWidgetToToolBar(QWidget *);

    void readMultiLanguage(QString file);

    QString mGlobalIniFile;
    QStringList bakimageList; // 背景图片列表.
   // QMap<QString,QPixmap> bakimageMap; // 背景图片
    QMap<QString,QString> bimgPath; // 背景图片的全路径
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
    QString backImage;
    Backgroud *bk;
    QSettings *mGlobalSet;
    int mRootPathLen;

    QString mJsonfile; // 控件模版文件.
    QString mLangfile; // 多国语言文件.
    QStringList mLanguageList;
    QStringList mOrderlist;
    QMap<QString,QString> mItemMap;
    QMap<QString,QPixmap> mImgMap; //这里只用来缓图片

    QJsonValue mCopyItem;



private slots:
      void onChangeBackgroud();
      void onDobuleClickedImage(QListWidgetItem *);

protected:
    void keyReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *ev);
    void closeEvent(QCloseEvent *e);
private:
    Ui::MainWindow *ui;

    QDockWidget* rDock;

    QListWidget* rList;
    QListWidget* lList;

    QString PrjJsonPath;

    void HandleFrameObject(QJsonObject qjo, QString ParentName);
   // void readExcelFile(QString xlsfile);
    void readExcelFile(char *xlsfile);
    void createCSVFile(QString xlsfile);
    void readCSVFile(QString csvfile);

 //   void closeEvent(QCloseEvent *);
    QWidget *pWin;



};


class LoadImgTask : public QRunnable
{
public:

    LoadImgTask(QWidget *parent);


   void run();
   void setDone();
private:
   bool isStop;
   BusyIndicator *rotate;
};

#endif // MAINWINDOW_H

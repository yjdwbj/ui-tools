#ifndef CANVASMANAGER_H
#define CANVASMANAGER_H
#include "core_def.h"
#include <QWidget>
#include <QPushButton>
#include <QtConcurrent>
#include <QFuture>
#include <QThread>
#include <QProgressDialog>
#include <QSlider>


class ScenesScreen;
class MainWindow;
class QStackedLayout;
class ProjectDialog;


class CanvasManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize mPageSize READ getDefaultPageSize WRITE setDefaultPageSize)

    friend class ScenesScreen;
public:
    CanvasManager(MainWindow *w);
    ScenesScreen *createNewCanvas();
//    ScenesScreen *activeSS();
    static  ScenesScreen* mActiveSS;
    int activeIndex();
    void deleteCanvas(int index);
    void deleteCanvas(ScenesScreen *ss);
    void setActiveSS(int index);
//    void setActiveSS(ScenesScreen *ss) { currentSS =ss;}
    void setDefaultPageSize(QSize s) { mPageSize = s;}
    void closeCurrentProject();

    void deleteCurrentPage();
    void OpenProject(QString file);
    void readProjectJson(const QJsonArray &array);

    QSize getDefaultPageSize() { return mPageSize;}

    QStackedLayout *stack;
    MainWindow *mWindow;


    QStringList mPrjSelectlang; // 工程选择的多国语言
    QString mProjectName;  // 工程名
    QSize mProjectSize; // 工程默认大小
    QString mProjectWidgetDir; // 自定义控件目录.
    QString mProjectImageDir; // 工程图片资源目录.
    QString mProjectFullPath;
    bool mIsOpenProject;

//    static void setXYPosition(const QPoint &p);
//    static void setXYHidden(bool b);
//    static void setXYRange(const QSize &size);



public slots:
      void onSaveProject();
      void onSaveAsProject();
      void onOpenProject();
      void onCreateNewProject();

private slots:

    void onCreateNewScenesScreen();
    void onDelCurrentScenesScreen();

    void onConfProject();
    void onRecordClick(bool);



private:
    void screenshot();
    bool mFFmpegRuning;
//    QJsonArray saveProjectJson(QProgressDialog *pd);

    void saveProject(QString fname);
//    ScenesScreen *currentSS;
    QWidgetList mCanvasList; // 画布列表

    QRect stackRect;
    QSize mPageSize;
    QTimer *autoSaveTimer;

    QPushButton *newPrj,*newPage,*delPage,*savePrj,*confPrj,*saveas;
    QString PrjJsonPath;
//    static QSlider *mHSlier;
//    static QSlider *mVSlier;

};





#endif // CANVASMANAGER_H

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
#include <QFrame>







class ScenesScreen;
class MainWindow;
class QStackedLayout;
class ProjectDialog;

class HVSlider : public QFrame
{
    Q_OBJECT
public:
    explicit HVSlider(Qt::Orientation hv,int wh,QWidget *parent=0);
    void setNewLen(int len);
//    void setObjectRect(const QRect &rect);

    void setRectPos(const QPoint &p);
    void setRectSize(const QSize &s);

protected:
    void paintEvent(QPaintEvent *);

private:
    Qt::Orientation mOrient;
    int mLen;
    int mFixHW;
    int mStep;
    int mGradPos;
    int mMaxStep;
    int mRectStart;
    int mRectLen;
};


class CanvasManager : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QSize mPageSize READ getDefaultPageSize WRITE setDefaultPageSize)

    friend class ScenesScreen;
public:
    CanvasManager(MainWindow *w);
    ScenesScreen *createNewCanvas();
    static  ScenesScreen* mActiveSS;
    static HVSlider *mHSlier;
    static HVSlider *mVSlier;
    static void setSliderPos(const QPoint &pos);
    static void setSliderSize(const QSize &size);

    int activeIndex();
    void deleteCanvas(int index);
    void deleteCanvas(ScenesScreen *ss);
    void setActiveSS(int index);
    void updateScreenshot() {screenshot();}
    void closeCurrentProject();
    void closeProjectDlg();

//    void setObjectRect(const QRect& rect);

    void deleteCurrentPage();
    void OpenProject(QString file);
    void readProjectJson(const QJsonArray &array);

    QStackedLayout *mStack;
    MainWindow *mWindow;

    QStringList mPrjSelectlang; // 工程选择的多国语言
    QString mProjectName;  // 工程名
//    QSize mProjectSize; // 工程默认大小
    QString mProjectWidgetDir; // 自定义控件目录.
    QString mProjectImageDir; // 工程图片资源目录.
    QString mProjectFullPath;
    bool mIsOpenProject;


public slots:
      void onSaveProject();
      void onSaveAsProject();
      void onOpenProject();
      void onCreateNewProject();
      void onUpdateNewProjectSize();

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
    QTimer *autoSaveTimer;

    QPushButton *newPrj,*newPage,*delPage,*savePrj,*confPrj,*saveas,*zoomPrj,*mRedo,*mUndo;
    QString PrjJsonPath;

};





#endif // CANVASMANAGER_H

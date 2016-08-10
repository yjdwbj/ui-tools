#ifndef CANVASMANAGER_H
#define CANVASMANAGER_H
#include "core_def.h"
#include <QWidget>
#include <QPushButton>


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
    ScenesScreen *activeSS();
    int activeIndex();
    void deleteCanvas(int index);
    void deleteCanvas(ScenesScreen *ss);
    void setActiveSS(int index);
    void setActiveSS(ScenesScreen *ss) { currentSS =ss;}
    void setDefaultPageSize(QSize s) { mPageSize = s;}
    void closeCurrentProject();

    void deleteCurrentPage();
    void readProjectJson(const QJsonArray &array);

    QSize getDefaultPageSize() { return mPageSize;}

    QStackedLayout *stack;
    MainWindow *mWindow;
    bool PrjIsChanged;

    QStringList mPrjSelectlang; // 工程选择的多国语言
    QString mProjectName;  // 工程名
    QSize mProjectSize; // 工程默认大小
    QString mProjectWidgetDir; // 自定义控件目录.
    QString mProjectImageDir; // 工程图片资源目录.



private slots:
    void onCreateNewProject();
    void onCreateNewScenesScreen();
    void onDelCurrentScenesScreen();
    void onSaveProject();
    void onConfProject();



private:
    void screenshot();

    void saveProject(QString fname);
    ScenesScreen *currentSS;
    QWidgetList mCanvasList; // 画布列表

    QRect stackRect;
    QSize mPageSize;

    QPushButton *newPrj,*newPage,*delPage,*savePrj,*confPrj;
    QString PrjJsonPath;

};

#endif // CANVASMANAGER_H

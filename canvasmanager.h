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


private slots:
    void onCreateNewProject();
    void onCreateNewScenesScreen();
    void onDelCurrentScenesScreen();
    void onSaveProject();



private:
    void screenshot();
    ScenesScreen *currentSS;
    QWidgetList mCanvasList; // 画布列表

    QRect stackRect;
    QSize mPageSize;

    QPushButton *newPrj,*newPage,*delPage,*savePrj;
    QString PrjJsonPath;

};

#endif // CANVASMANAGER_H

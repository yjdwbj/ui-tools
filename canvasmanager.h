#ifndef CANVASMANAGER_H
#define CANVASMANAGER_H
#include "config.h"
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
    //Q_PROPERTY(QString Name READ getName WRITE setName)
public:
    CanvasManager(MainWindow *w);
    void createNewCanvas();
    ScenesScreen *activeSS();
    void deleteCanvas(int index);
    void deleteCanvas(ScenesScreen *ss);
    void setActiveSS(int index);
    void setActiveSS(ScenesScreen *ss) { currentSS =ss;}
    void setDefaultPageSize(QSize s) { mPageSize = s;}

    void deleteCurrentPage();

    QSize getDefaultPageSize() { return mPageSize;}

    QStackedLayout *stack;

private slots:
    void onCreateNewProject();
    void onCreateNewScenesScreen();
    void onDelCurrentScenesScreen();



private:
    void screenshot();
    ScenesScreen *currentSS;
    QWidgetList ssList;
    MainWindow *mWindow;
    QRect stackRect;
    QSize mPageSize;

    QPushButton *newPrj,*newPage,*delPage;

};

#endif // CANVASMANAGER_H

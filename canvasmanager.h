#ifndef CANVASMANAGER_H
#define CANVASMANAGER_H
#include "config.h"
#include <QWidget>


class ScenesScreen;
class MainWindow;
class QStackedLayout;

class CanvasManager
{
public:
    CanvasManager(MainWindow *w);
    void createNewCanvas();
    ScenesScreen *activeSS();
    void deleteCanvas(int index);
    void deleteCanvas(ScenesScreen *);
    void setActiveSS(int index);
    void setActiveSS(ScenesScreen *ss) { currentSS =ss;}

    QStackedLayout *stack;


private:
    void screenshot();
    ScenesScreen *currentSS;
    QWidgetList ssList;
    MainWindow *mWindow;
};

#endif // CANVASMANAGER_H

#include "canvasmanager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scenesscreen.h"


static int Width  = 480;
static int Height  = 320;

CanvasManager::CanvasManager(MainWindow *w):
    mWindow(w)
{
   // w->ui->centralWidget;
}


void CanvasManager::createNewCanvas()
{
    ScenesScreen *Scenes = new ScenesScreen(QSize(Width,Height),mWindow->centralWidget());
    Scenes->addMainWindow(mWindow);
    Scenes->move(mWindow->width() * 0.12,mWindow->height()* 0.3);  // 按屏幕比例调整

    ssList.append(Scenes);
    currentSS = Scenes;
    mWindow->lDock->setEnabled(true);
}

ScenesScreen *CanvasManager::activeSS()
{
    if(!currentSS)
        createNewCanvas();
    return currentSS;
}

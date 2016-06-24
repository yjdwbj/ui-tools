#include "canvasmanager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scenesscreen.h"
#include "pageview.h"
#include <QStackedLayout>


static int Width  = 480;
static int Height  = 320;
const char DKEY_SHOT[] = "screenshot";
const char DKEY_TXT[] = "ViewName";

CanvasManager::CanvasManager(MainWindow *w):
    mWindow(w),stack(new QStackedLayout)
{
    // w->ui->centralWidget;
    mWindow->centralWidget()->setLayout(stack);
}


void CanvasManager::screenshot()
{
    // 对原来的那个页面进行截屏
    if(ssList.size())
    {
        QPixmap pixmap(stack->currentWidget()->size());


        //QPixmap pixmap(activeSS()->size());
        stack->currentWidget()->render(&pixmap,QPoint(),QRegion(stack->currentWidget()->rect()));
        QVariant vp = stack->currentWidget()->property(DKEY_SHOT);
        if( vp.isValid() && vp.toBool() == false)
        {

            mWindow->pageView->addNewPage(pixmap,
                                          stack->currentWidget()->property(DKEY_TXT).toString());
                                         // stack->currentWidget()->objectName());
            //pixmap.save("test.png");　//　这里如果有需要可以保存成文件.
            stack->currentWidget()->setProperty(DKEY_SHOT,true);
        }else{
            int index = stack->currentIndex();
            mWindow->pageView->delPage(index); // 删除当前的,更新每新的.
            mWindow->pageView->InsertPage(index,pixmap);

        }
    }

}

void CanvasManager::createNewCanvas()
{
    screenshot();
    ScenesScreen *Scenes = new ScenesScreen(QSize(Width,Height));
    //mWindow->centralWidget());
    Scenes->addMainWindow(mWindow);
    Scenes->move(mWindow->width() * 0.12,mWindow->height()* 0.3);  // 按屏幕比例调整
    Scenes->setProperty(DKEY_SHOT,false);  // 检查该页面是否创建过截图.
    ssList.append(Scenes);
    // 这里不能改变它的对像名,用一个动态属
    // Scenes->setObjectName(QString("Page_%1").arg(QString::number(ssList.size()-1)));
    Scenes->setProperty(DKEY_TXT,QString("Page_%1").arg(QString::number(ssList.size()-1)));
    currentSS = Scenes;
    mWindow->lDock->setEnabled(true);
    stack->addWidget(Scenes);
    stack->setCurrentWidget(Scenes);
}

ScenesScreen *CanvasManager::activeSS()
{
//    if(!currentSS)
//        createNewCanvas();
//    qDebug() << " Scenes size " << ssList.size();
//    return currentSS;
     return (ScenesScreen*)(stack->currentWidget());
}

void CanvasManager::setActiveSS(int index)
{
    if(index < ssList.size())
    {
        qDebug() << " show previous object" << index;
       // ScenesScreen *ss = (ScenesScreen*)(ssList.at(index));
       // currentSS = ss;
        screenshot();
        stack->setCurrentIndex(index);
        // ss->setFocus();
        // ss->show();

    }
}

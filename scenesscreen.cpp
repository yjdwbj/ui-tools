#include "scenesscreen.h"
#include "module.h"
#include <QSizeGrip>
#include <QRegion>

ScenesScreen::ScenesScreen(QSize size, QWidget *parent) : QFrame(parent),mWindow((MainWindow*)parent),mActiveIdx(-1)
{

   setObjectName("ActiveScreen");
   setStyleSheet("QFrame#ActiveScreen{border: 1.5px solid gray;"\
                 "background-color: #EEEEEE;}");
   setFixedSize(size);
   show();

}
void ScenesScreen::createNewLayer()
{
    NewLayer *nl = new NewLayer(QSize(150,200),this);

    QSizeGrip *sg = new QSizeGrip(nl);
    sg->setVisible(true);
    qDebug() << " QSize Grip Size Hint " << sg->sizeHint();
    sg->show();


    nl->SelectLayer();
    nl->addMainWindow(mWindow);
    LayerList.append(nl);
    mActiveIdx = LayerList.size() - 1;

    QRegion *qr = new QRegion(nl->geometry(),QRegion::Rectangle);
    qDebug() << "create new layer  index " << mActiveIdx;
    nl->setMask(*qr);

}



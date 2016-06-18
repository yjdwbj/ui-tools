#include "scenesscreen.h"
#include "module.h"

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
    nl->SelectLayer();
    nl->addMainWindow(mWindow);
    LayerList.append(nl);
    mActiveIdx = LayerList.size() - 1;
    qDebug() << "create new layer  index " << mActiveIdx;
}



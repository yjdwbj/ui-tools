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

ScenesScreen::~ScenesScreen()
{

}

void ScenesScreen::createNewLayout()
{
    NewLayout *nl = new NewLayout(QSize(150,200),this);
    nl->addMainWindow(mWindow);
    LayerList.append(nl);
    mActiveIdx = LayerList.size() - 1;
    nl->setObjectName(QString("%1_%2").arg(nl->metaObject()->className(),QString::number(mActiveIdx)));
    nl->onSelectMe();
}

void ScenesScreen::setSelectObject(FormResizer *obj)
{
    foreach(FormResizer *fr, this->findChildren<FormResizer*>())
    {
        if(fr == obj)
            continue;
        fr->setState(SelectionHandleOff);
    }
    obj->setState(SelectionHandleActive);
    if(!CN_NEWLAYOUT.compare(obj->metaObject()->className()))
        mActiveIdx = LayerList.indexOf(obj);  // 这里只是布局控件才更改它的数值
    mWindow->tree->setSelectTreeItem(obj);
}

void ScenesScreen::delSelectedLayout()
{

}

void ScenesScreen::delAllObjects()
{
    foreach (QWidget *w, LayerList) {
        // 这里递归删每一个新建的控件
        QString cname = w->metaObject()->className();
        if(!CN_NEWLAYOUT.compare(cname))
        {
            ((NewLayout*)w)->delMySelf();
        }
        else if(!CN_NEWFRAME.compare(cname))
        {
             ((NewFrame*)w)->delMySelf();
        }
    }
    this->deleteLater();

}

void ScenesScreen::keyReleaseEvent(QKeyEvent *s)
{
    switch (s->key()) {
    case Qt::Key_Delete:
        qDebug() << " you pressed Delete ";
        break;
    default:
        break;
    }
}


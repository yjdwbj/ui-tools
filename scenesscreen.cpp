#include "scenesscreen.h"
#include "module.h"
#include "canvasmanager.h"



ScenesScreen::ScenesScreen(QSize size, QWidget *parent)
    : QFrame(parent),
      mWindow((MainWindow*)parent),
      mActiveIdx(-1),activeObj(0)
{
    setObjectName("PageScreen");
    setStyleSheet("QFrame#PageScreen{border: 1.5px solid gray;"\
                  "background-color: #EEEEEE;}");
    setFixedSize(size);
    setFocusPolicy(Qt::ClickFocus);
    show();

}

void ScenesScreen::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        {
            QMenu *contextMenu = new QMenu(this);
            QAction delme("删除当前页面",this);
            connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMySelf()));
            contextMenu->addAction(&delme);

            contextMenu->exec(mapToGlobal(event->pos()));
        }
}

void ScenesScreen::onDeleteMySelf()
{
    mWindow->cManager->onDelCurrentScenesScreen();
}

ScenesScreen::~ScenesScreen()
{

}

void ScenesScreen::createNewLayout()
{
    NewLayout *nl = new NewLayout(QSize(150,200),this);
    nl->addMainWindow(mWindow);
    LayoutList.append(nl);
    mActiveIdx = LayoutList.size() - 1;
    //nl->setObjectName(QString("%1_%2").arg(nl->metaObject()->className(),QString::number(mActiveIdx)));
    nl->setProperty(DKEY_LOCALSEQ,QString("%1_%2").arg("布局",QString::number(mActiveIdx)));

    nl->onSelectMe();
}

void ScenesScreen::readLayout(const QJsonArray &array)
{
    foreach (QJsonValue val, array) {
        switch (val.type()) {
        case QJsonValue::Object:
        {
            QJsonObject valobj = val.toObject();
//            QSize valsize(valobj[SIZE].toObject()[WIDTH].toString().toInt(),
//                          valobj[SIZE].toObject()[HEIGHT].toString().toInt());

            QJsonObject rectobj;
            foreach (QJsonValue pval, valobj[PROPERTY].toArray()) {
                QJsonObject pobj = pval.toObject();
                if(pobj.contains(KEY_RECT))
                {
                    rectobj = pobj[KEY_RECT].toObject();

                    break;
                }
            }
            QVariantMap rect =  rectobj.toVariantMap();
            QRect oldrect = QRect(rect["x"].toString().toInt(),
                    rect["y"].toString().toInt(),
                    rect["width"].toString().toInt(),
                    rect["height"].toString().toInt());

            NewLayout *nl = new NewLayout(oldrect.size(),this);
            nl->setProperty(DKEY_LOCALSEQ,valobj[CAPTION].toString());
            nl->setGeometry(oldrect);
            nl->addMainWindow(mWindow);
            LayoutList.append(nl);
            mActiveIdx = LayoutList.size() -1;

            nl->readFromJson(valobj[LAYOUT].toArray());
            nl->onSelectMe();
        }

            break;
        default:
            break;
        }
    }
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
    obj->setFocus();

    activeObj = obj;
    //qDebug() << " active object is " << obj->objectName();
    if(!CN_NEWLAYOUT.compare(obj->metaObject()->className()))
    {
        mActiveIdx = LayoutList.indexOf(obj);  // 这里只是布局控件才更改它的数值
    }

    mWindow->tree->setSelectTreeItem(obj);
}

void ScenesScreen::delSelectedLayout()
{

    if(!CN_NEWLAYOUT.compare(activeObj->metaObject()->className()))
    {
        //删除布局控件
        QMessageBox msgBox;
        msgBox.setWindowTitle("删除提示");
        msgBox.setText("你真的要删除当前布局吗?删除之后不可以撤消,请选择<删除>删除.");
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"删除");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //qDebug() << " QMessageBox result " << ret;
        if(ret == QMessageBox::Yes)
        {
            // cManager->deleteCurrentPage();
           // ScenesScreen *ss = activeObj->parentWidget();
           int index = LayoutList.indexOf(activeObj);

           //QWidget *p = LayoutList.takeAt(index);

           ((NewLayout*)activeObj)->delMySelf();

           LayoutList.removeOne(activeObj);
           if(LayoutList.size())
           {
               if((index - 1) <  0  )
               {
                   setSelectObject((NewLayout*)(LayoutList.last()));
               }else{
                   setSelectObject((NewLayout*)(LayoutList.at(index-1)));
               }
           }else{
               mActiveIdx = -1;
           }

        }

    }else {
        //删除控件
        NewLayout *nl = (NewLayout*)(activeObj->parentWidget()->parentWidget());

        nl->deleteObject(activeObj);  // 从它的父控件删除它.

        setSelectObject(nl); // 选中它父控件.

    }


}

void ScenesScreen::delAllObjects()
{
    foreach (QWidget *w, LayoutList) {
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
        if(activeObj)
        {
          //  qDebug() << " you pressed Delete " << "active object " << activeObj->objectName();
            delSelectedLayout();
        }

        break;
    default:
        break;
    }
}

void ScenesScreen::writeToJson(QJsonObject &json)
{
    QJsonArray layoutarr;
    //qDebug() << "ScenesScreen parent obj " << &json ;

    foreach (QWidget *w, LayoutList) {
        QJsonObject layoutObj;
       // qDebug() << "ScenesScreen  sub object " << &layoutObj;
        layoutObj[NAME] = w->objectName();
        ((NewLayout*)w)->writeToJson(layoutObj);
       // qDebug() << " LayoutObj " << layoutObj;
        layoutarr.append(layoutObj);
    }

//    QVariantMap s ;
//    s[WIDTH] = QString::number(this->width());
//    s[HEIGHT] = QString::number(this->height());
    //json[SIZE] = QJsonObject::fromVariantMap(s);
    QJsonArray projson;

    QVariantMap vmap ;
    vmap["x"] = QString::number(this->x());
    vmap["y"] = QString::number(this->y());
    vmap[WIDTH] =QString::number(this->width());
    vmap[HEIGHT] = QString::number(this->height());

    QJsonObject rect;

    rect[KEY_RECT] = QJsonObject::fromVariantMap(vmap);
    projson.append(rect);
    json[PROPERTY] = projson;

    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[metaObject()->className()] = layoutarr;
    //qDebug() << "ScenesScreen array " << layoutarr;
}


#include "scenesscreen.h"
#include "module.h"
#include "canvasmanager.h"
#include <QColorDialog>



ScenesScreen::ScenesScreen(QSize size, QWidget *parent)
    : QFrame(parent),
      mWindow((MainWindow*)parent),
      mActiveIdx(-1),
      activeObj(0),
      mActiveLaySeq(-1)
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
            QAction chgcolor("修改背景色",this);
            contextMenu->addAction(&chgcolor);
            connect(&chgcolor,SIGNAL(triggered(bool)),SLOT(onChangedBackgroundColor()));
            QAction chgimg("修改背景图片",this);
            contextMenu->addAction(&chgimg);
            connect(&chgimg,SIGNAL(triggered(bool)),SLOT(onChangedBackgroundImage()));

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

void ScenesScreen::onChangedBackgroundImage()
{
    QDialog dig(mWindow);
    dig.setFixedSize(mWindow->size() * 0.5);
    dig.setWindowTitle(tr("修改背景"));

    QVBoxLayout *v = new QVBoxLayout();
    dig.setLayout( v);
    QListWidget *imglist = new QListWidget();
    imglist->setSelectionMode(QAbstractItemView::SingleSelection);
    imglist->setViewMode(QListWidget::IconMode);
    imglist->setIconSize(QSize(160,140));

    connect(imglist,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(onDobuleClickedImage(QListWidgetItem*)));
    v->addWidget(imglist);
    QMapIterator<QString,QPixmap> it(mWindow->bakimageMap);
    while(it.hasNext())
    {
        it.next();
        imglist->addItem(new QListWidgetItem(QIcon(it.value()),it.key()));
    }

    dig.setModal(true);
    dig.exec();

}

void ScenesScreen::onDobuleClickedImage(QListWidgetItem *item)
{
    setStyleSheet(QString("background-image: url(%1);").arg(mWindow->bimgPath[item->text()]));
    update();
    qDebug() << " this stylesheet " << styleSheet();
}

void ScenesScreen::onChangedBackgroundColor()
{
    QColorDialog *color =new  QColorDialog(mWindow);
    color->setWindowFlags(Qt::Window|Qt::WindowStaysOnTopHint);
    color->setOption(QColorDialog::DontUseNativeDialog);
    color->exec();

    QColor c =  color->selectedColor();

    if(c.isValid())
    {
        qDebug() << " you selected color is " << c.rgb();
    }
    this->setStyleSheet(QString("background-color: %1;").arg(c.name(QColor::HexRgb)));
    update();

    delete color;
}

NewLayer* ScenesScreen::createNewLayer(const QJsonObject &json)
{
    QRect oldrect = Compoent::getRectFromStruct(json[PROPERTY].toArray());
    if(oldrect.isEmpty())
    {
        oldrect.setWidth(200);
        oldrect.setHeight(200);
    }


    NewLayer *nlayer = new NewLayer(oldrect.size() /* + MARGIN_SIZE*/,this);
    nlayer->setGeometry(oldrect);
    LayerList.append(nlayer);
    mActiveLaySeq = LayerList.size() - 1;
    QVariant variant = json.value(PROPERTY).toVariant();
    nlayer->setProperty(DKEY_TXT,json[CAPTION].toString());
    if(json.contains(PROPERTY))
    {
        nlayer->copyProperty(variant);
        nlayer->setProperty(DKEY_DYN,variant);
    }

    nlayer->onSelectMe();
    mWindow->tree->addItemToRoot(nlayer);
  //  nlayer->setToolTip(nlayer->property(DKEY_LOCALSEQ).toString());
    foreach (QJsonValue layout, json[LAYOUT].toArray()) {
        nlayer->readFromJson(layout.toObject());
    }
    return nlayer;

}

void ScenesScreen::readLayer(const QJsonArray &array)
{
    foreach (QJsonValue val, array) {
        switch (val.type()) {
        case QJsonValue::Object:
        {
            QJsonObject valobj = val.toObject();
             NewLayer *nlayer = createNewLayer(valobj);

           //  nlayer->readFromJson(valobj[LAYOUT].toArray());
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
    QString clsname = obj->metaObject()->className();
    if(!CN_NEWLAYOUT.compare(clsname))
    {
        mActiveIdx = LayoutList.indexOf(obj);  // 这里只是布局控件才更改它的数值

    }

    mWindow->tree->setSelectTreeItem(obj);
}

void ScenesScreen::delSelectedLayer()
{

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
           LayoutList.removeOne(activeObj);
            ((NewLayout*)activeObj)->delMySelf();
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
               activeObj = 0;
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
    foreach (QWidget *w, LayerList) {
        QJsonObject layoutObj;
       // qDebug() << "ScenesScreen  sub object " << &layoutObj;
        layoutObj[NAME] = w->objectName();
        ((NewLayer*)w)->writeToJson(layoutObj);
       // qDebug() << " LayoutObj " << layoutObj;
        layoutarr.append(layoutObj);
    }

    QJsonArray projson;
    projson.append(Compoent::getRectJson(this));
    json[PROPERTY] = projson;
   // json[NAME] = this->objectName();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[CLASS] = this->metaObject()->className();
    json[LAYER] = layoutarr;
    //qDebug() << "ScenesScreen array " << layoutarr;
}


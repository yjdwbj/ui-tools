#include "scenesscreen.h"
#include "module.h"
#include "canvasmanager.h"
#include <QColorDialog>



ScenesScreen::ScenesScreen(QSize size, QWidget *parent)
    : QFrame(parent),
      mWindow((MainWindow*)parent),
   //   mActiveIdx(-1),
      activeObj(0)
      //mActiveLaySeq(-1)
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
            connect(&delme,&QAction::triggered,[=](){
                mWindow->cManager->onDelCurrentScenesScreen();
            });
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

NewLayer* ScenesScreen::createNewLayer(const QJsonValue &qv,bool createflag)
{
    QJsonObject json = qv.toObject();
    QRect oldrect = Compoent::getRectFromStruct(json[PROPERTY].toArray(),KEY_RECT);
    if(oldrect.isEmpty())
    {
        oldrect.setWidth(200);
        oldrect.setHeight(200);
    }

    NewLayer *nlayer = new NewLayer(json[CAPTION].toString(), oldrect.size() /* + MARGIN_SIZE*/,this);
    nlayer->mCreateFlag = createflag;
    nlayer->setProperty(DKEY_JSONSTR,qv);
    nlayer->setProperty(DKEY_TYPE, json[WTYPE].toString());
    nlayer->setGeometry(oldrect);
    LayerList.append(nlayer);
    //mActiveLaySeq = LayerList.size() - 1;
    nlayer->mOwerJson = qv.toObject();
 //   nlayer->initJsonValue();

    nlayer->onSelectMe();
    nlayer->updateStyleSheets();
    mWindow->tree->addItemToRoot(nlayer);

    nlayer->show();
    return nlayer;

}

void ScenesScreen::readLayer(const QJsonArray &array)
{
    // 从工程里读取图层.
    foreach (QJsonValue val, array) {
        switch (val.type()) {
        case QJsonValue::Object:
        {
             NewLayer *nlayer = createNewLayer(val,false);

             foreach (QJsonValue layout, val.toObject()[LAYOUT].toArray()) {
                 //nlayer->readFromJson(layout.toObject());
                 // 这里一定读取工程和读取自定义控件才会有的,给他一个
                 nlayer->readLayoutFromJson(layout,nlayer->mCreateFlag);
             }
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

    mWindow->tree->setSelectTreeItem(obj);
}


void ScenesScreen::delAllObjects()
{
    foreach (QWidget *w, LayerList) {
        // 这里递归删每一个新建的控件
        QString cname = w->metaObject()->className();
        if(!CN_NEWLAYOUT.compare(cname))
        {
            ((NewLayout*)w)->DeleteMe();
        }
        else /*if(!CN_NEWFRAME.compare(cname))*/
        {
             ((BaseForm*)w)->DeleteMe();
        }
    }
    this->deleteLater();

}

void ScenesScreen::keyReleaseEvent(QKeyEvent *s)
{

    // 处理一些鼠标事件.
    if(activeObj)
    {
        BaseForm *bf = (BaseForm*)activeObj;
        QPoint mpos = bf->pos();
        switch (s->key()) {
        case Qt::Key_Delete:

            bf->onDeleteMe();
            //bf->DeleteMe();
            break;
        case Qt::Key_Up:
            bf->move(mpos.x(),mpos.y()-1);
            bf->moveNewPos(bf->pos());
            break;
        case Qt::Key_Down:
            bf->move(mpos.x(),mpos.y()+1);
            bf->moveNewPos(bf->pos());
            break;
        case Qt::Key_Left:
            bf->move(mpos.x()-1,mpos.y());
            bf->moveNewPos(bf->pos());
            break;
        case Qt::Key_Right:
            bf->move(mpos.x()+1,mpos.y());
            bf->moveNewPos(bf->pos());
            break;
        default:
            break;
        }
    }

}

QJsonObject  ScenesScreen::writeToJson()
{
    QJsonArray layoutarr;
    foreach (QWidget *w, LayerList) {
        layoutarr.append(((NewLayer*)w)->writeToJson());
    }


    QJsonObject json;
   json[NAME] = metaObject()->className();
   json[WTYPE] = "page";

    QJsonArray projson;
    projson.append(Compoent::getRectJson(this));
    json[PROPERTY] = projson;
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[CLASS] = this->metaObject()->className();
    json[LAYER] = layoutarr;
    return json;
    //qDebug() << "ScenesScreen array " << layoutarr;
}


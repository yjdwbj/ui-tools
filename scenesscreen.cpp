#include "scenesscreen.h"
#include "module.h"
#include "canvasmanager.h"
#include <QColorDialog>
#include <QKeySequence>



ScenesScreen::ScenesScreen(QSize size, QWidget *parent)
    : QFrame(parent),
      activeObj(0)
{
    setObjectName("PageScreen");
    setStyleSheet("QFrame#PageScreen{border: 1.5px solid gray;"\
                  "background-color: #EEEEEE;}");
    setFixedSize(size);
    setFocusPolicy(Qt::ClickFocus);
    show();
    setAcceptDrops(true);
//    mVLine.setP1(pos());
//    mVLine.setP2(pos());
//    mHLine.setP1(pos());
//    mHLine.setP2(pos());

    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
    mLine = new HVLineWidget(this);
    mLine->setGeometry(this->geometry());
   // mLine->show();

}

void ScenesScreen::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        QAction delme(QIcon(":/icon/icons/removesubmitfield.png"),"删除当前页面",this);
        connect(&delme,&QAction::triggered,[=](){
            mWindow->cManager->onDelCurrentScenesScreen();
        });
        contextMenu->addAction(&delme);
        QAction chgcolor(QIcon(":/icon/icons/gradient.png"),"修改背景色",this);
        connect(&chgcolor,SIGNAL(triggered(bool)),SLOT(onChangedBackgroundColor()));
        QAction chgimg(QIcon(":/icon/icons/image-icon.png"),"修改背景图片",this);
        connect(&chgimg,&QAction::triggered,[=](){
            QDialog dig(mWindow);
            dig.setFixedSize(mWindow->size() * 0.5);
            dig.setWindowTitle(tr("修改背景"));

            QVBoxLayout *v = new QVBoxLayout();
            dig.setLayout( v);
            QListWidget *imglist = new QListWidget();
            imglist->setSelectionMode(QAbstractItemView::SingleSelection);
            imglist->setViewMode(QListWidget::IconMode);
            imglist->setIconSize(QSize(160,140));

            connect(imglist,&QListWidget::itemDoubleClicked,
                    [=](QListWidgetItem* item){
                setStyleSheet(QString("background-image: url(%1);").arg(mWindow->bimgPath[item->text()]));
                update();
            });
            v->addWidget(imglist);
            dig.setModal(true);
            dig.exec();
        });

        contextMenu->addSeparator();
        QAction copy(QIcon(":/icon/icons/editcopy.png"),"复制",this);
        connect(&copy,&QAction::triggered,[=](){
            BaseForm::mCopyItem = QJsonValue(writeToJson());
            BaseForm::mCopyFromType = BaseForm::ObjTypes::TYPESS;
        });

        QAction paste(QIcon(":/icon/icons/editpaste.png"),"粘贴",this);
        connect(&paste,&QAction::triggered,[=](){
            mWindow->cManager->activeSS()->pasteItem(this);
        });
        contextMenu->addAction(&copy);
        paste.setEnabled(!BaseForm::mCopyItem.isNull());
        contextMenu->addAction(&paste);

        contextMenu->exec(mapToGlobal(event->pos()));
    }
}



ScenesScreen::~ScenesScreen()
{

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
        oldrect.setX(0);
        oldrect.setY(0);
    }

    NewLayer *nlayer = new NewLayer(json, oldrect/* + MARGIN_SIZE*/,this);
    nlayer->mCreateFlag = createflag;
    nlayer->setProperty(DKEY_JSONSTR,qv);
    nlayer->setProperty(DKEY_TYPE, json[WTYPE].toString());
    childlist.append(nlayer);
    nlayer->mOwerJson = qv.toObject();
    nlayer->initialEname();
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
    foreach (QWidget *w, childlist) {
        // 这里递归删每一个新建的控件
        if(((BaseForm*)w)->mType == BaseForm::T_NewLayout)
        {
            ((NewLayout*)w)->DeleteMe();
        }
        else
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

        if(s->matches(QKeySequence::Copy))
        {
            BaseForm::mCopyItem =  QJsonValue(bf->writeToJson());
            BaseForm::mCopyFromType = bf->mType;
        }else if(s->matches(QKeySequence::Paste))
        {
            pasteItem(bf);

        }else if(s->matches(QKeySequence::Save))
        {
            mWindow->cManager->onSaveProject();
        }
        else if(s->matches(QKeySequence::SaveAs))
        {
            mWindow->cManager->onSaveAsProject();
        }else if(s->matches(QKeySequence::Find))
        {
            findDlg dlg(mWindow);
            dlg.exec();
        }

    }

}


void ScenesScreen::pasteItem(QWidget *w)
{
    qDebug() << " paste to object " << w->objectName() << w->metaObject()->className();
    if(BaseForm::mCopyItem.isNull()) return;

    BaseForm::ObjTypes fromType = BaseForm::mCopyFromType;
    if(w->inherits(this->metaObject()->className()) ||
            fromType == BaseForm::T_NewLayer )
    {
        //  qDebug() << " paste to sss object ";
        if(fromType != BaseForm::T_NewLayer)
        {
            QMessageBox::warning(this,"提示","剪切板里的对像支持粘贴到当前容器上,请选择一个<布局>对像.");
        }else
        {
            QJsonArray a;
            a.append(BaseForm::mCopyItem);
            readLayer(a);
        }
    }else{
        BaseForm *bf = (BaseForm*)w;
        BaseForm::ObjTypes curtype = bf->mType;
        //            QString cls =  BaseForm::mCopyItem.toObject()[CLASS].toString();


        switch (curtype) {
        case BaseForm::T_NewLayer:
        case BaseForm::T_NewLayout:
        {
            NewLayout* bflayout = (NewLayout*)bf;
            bflayout->readFromJson(BaseForm::mCopyItem,false);
        }

            break;
        case BaseForm::T_NewGrid:
            QMessageBox::warning(this,"提示","当前类型容器不接受粘贴!");
            break;
        case BaseForm::T_NewList:
            if(fromType == BaseForm::T_NewLayout)
            {
                NewList *nlist = (NewList*)bf;
                nlist->pasteOneLine(BaseForm::mCopyItem);
            }else{
                // 不能复制到当前对像上,请重新选择LAYOUT.
                QMessageBox::warning(this,"提示","当前的选中的对像不支持剪切板里的对像粘贴,请选择一个<布局>对像.");
            }
            break;
        case BaseForm::T_NewFrame:
            ((NewLayout*)bf->mParent)->readFromJson(BaseForm::mCopyItem,false);
            break;

        default:
            break;
        }
    }

}

QJsonObject  ScenesScreen::writeToJson()
{
    QJsonArray layoutarr;

    foreach (QWidget *w, childlist) {
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
}

void ScenesScreen::dragEnterEvent(QDragEnterEvent *e)
{

    mLine->show();
    mLine->setHidden(false);
    mLine->update();
    qDebug() << " drag enter ";
    e->accept();
}
void ScenesScreen::dragLeaveEvent(QDragLeaveEvent *)
{
    qDebug() << " drag levave";
    mLine->update();
    mLine->setHidden(true);
}

void ScenesScreen::dragMoveEvent(QDragMoveEvent *e)
{
    mLine->setPos(e->pos());
    e->accept();
}

void ScenesScreen::dropEvent(QDropEvent *e)
{
    mLine->setHidden(true);
    e->acceptProposedAction();

    QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    QPixmap pixmap;
    QPoint offset;

    QVariant qv;
    int flag ;
    dataStream >> flag  >> qv >> pixmap >> offset;
     QJsonValue val = QJsonValue::fromVariant(qv);
    if(static_cast<int>(BaseForm::ObjTypes::T_NewLayer)  == flag)
    {
        createNewLayer(val,true);
    }else if(static_cast<int>(BaseForm::ObjTypes::T_NewLayout)  == flag){
        BaseForm *active =(BaseForm*)(this->activeObject());
        if(active->mType == BaseForm::T_NewLayer)
        {
            ((NewLayer*)active)->readLayoutFromJson(val,true);
        }else if(active->mType == BaseForm::T_NewLayout)
        {
            ((NewLayout*)active)->readFromJson(val,true);
        }
    }else{
        BaseForm *active =(BaseForm*)(this->activeObject());
        ((NewLayout*)active)->readFromJson(val,true);

//        QMessageBox::warning(0,tr("提示"),tr("请先拖入创建一个图层"));
    }
    mLine->raise();
}

void ScenesScreen::mouseReleaseEvent(QMouseEvent *)
{
    mLine->setHidden(true);
}

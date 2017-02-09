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
//    QSize ps = parent->size();
//    int w = ( ps.width() - width() )/ 2;
//    int h = (ps.height()-height()) / 2;
//    move(mapFromParent(QPoint(w,h)));
    show();

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
            BaseForm::mCopyFromType = BaseForm::ObjType::TYPESS;
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
        if(((BaseForm*)w)->mType == BaseForm::TYPELAYOUT)
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
    BaseForm *bf = (BaseForm*)w;
    if(!BaseForm::mCopyItem.isNull())
    {
        //QString curobj = bf->metaObject()->className();
        BaseForm::ObjType curtype = bf->mType;
        QString cls =  BaseForm::mCopyItem.toObject()[CLASS].toString();

        if(!cls.compare(CN_NEWLAYER) )
        {
            // 复制到同级.
            QJsonArray a;
            a.append(BaseForm::mCopyItem);
            readLayer(a);

        }else if(!cls.compare(CN_NEWLAYOUT)){
            if( curtype == BaseForm::TYPELAYOUT)
            {
                NewLayout* bflayout = (NewLayout*)bf;
                bflayout->readFromJson(BaseForm::mCopyItem,true);
            }
            else if (curtype == BaseForm::TYPELAYER)
            {
                NewLayer *layer = (NewLayer *)bf;
                layer->readLayoutFromJson(BaseForm::mCopyItem,true);
            }else if(curtype == BaseForm::TYPELIST )
            {
                if(BaseForm::mCopyFromType != BaseForm::ObjType::TYPELAYOUT)
                {
                    QMessageBox::warning(this,"提示","列表容器只支持<布局>类型的粘贴!");
                }else{
                    NewList *nlist = (NewList*)bf;
                    nlist->pasteOneLine(BaseForm::mCopyItem);
                }

            }else if(curtype == BaseForm::TYPEGRID)
            {
                QMessageBox::warning(this,"提示","当前类型容器不接受粘贴!");
            }
            else{
                NewLayout* bflayout = (NewLayout*)(bf->parentWidget());
                bflayout->readFromJson(BaseForm::mCopyItem,true);
            }

        }else if(!cls.compare(CN_NEWFRAME) ||
                 !cls.compare(CN_NEWGRID) ||
                 !cls.compare(CN_NEWLIST) )
        {

            if(curtype != BaseForm::TYPELAYOUT)
            {
                // 不能复制到当前对像上,请重新选择LAYOUT.
                QMessageBox::warning(this,"提示","当前的选中的对像不支持剪切板里的对像粘贴,请选择一个<布局>对像.");
            }else
            {
                NewLayout* bflayout = (NewLayout*)bf;
                bflayout->readFromJson(BaseForm::mCopyItem,true);
            }
        }else if(this->inherits(cls.toLocal8Bit().data()))
        {
            QJsonObject ssobj = BaseForm::mCopyItem.toObject();
            readLayer(ssobj[LAYER].toArray());
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


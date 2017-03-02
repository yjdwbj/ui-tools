#include "scenesscreen.h"
#include "module.h"
#include "canvasmanager.h"
#include <QColorDialog>
#include <QKeySequence>
#include <QStatusBar>

class Compoent;
class QStatusBar;

QWidget* ScenesScreen::mActiveObj = 0;

ScenesScreen::ScenesScreen(QSize size, QWidget *parent)
    : QFrame(parent),
      mWindow((MainWindow*)parent),
      mXYLine(new HVLineWidget(this))
{
    setObjectName("PageScreen");
    setStyleSheet("QFrame#PageScreen{border: 1.5px solid gray;"\
                  "background-color: #EEEEEE;}");
    setFixedSize(size);
    setFocusPolicy(Qt::ClickFocus);
    show();
    setAcceptDrops(true);

    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);

    mXYLine->setHidden(true);
    mXYLine->setGeometry(this->geometry());
    mXYLine->raise();
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
            CanvasManager::mActiveSS->pasteItem(this);
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
    NewLayer *nlayer = new NewLayer(json, mWindow,this);
    nlayer->mCreateFlag = createflag;
    nlayer->setProperty(DKEY_JSONSTR,qv);
    nlayer->setProperty(DKEY_TYPE, json[WTYPE].toString());
    childlist.append(nlayer);
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

    mActiveObj = obj;

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
    if(mActiveObj)
    {
        BaseForm *bf = (BaseForm*)mActiveObj;
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
//    qDebug() << " paste to object " << w->objectName() << w->metaObject()->className();
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

    mWindow->statusBar()->showMessage(QString("控件数量: %1").arg(QString::number(BaseForm::mSeqEnameMap.size())));

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
    mXYLine->setHidden(false);
    e->accept();
}
void ScenesScreen::dragLeaveEvent(QDragLeaveEvent *e)
{
    mXYLine->setHidden(true);
    e->accept();
}

void ScenesScreen::dragMoveEvent(QDragMoveEvent *e)
{
    mXYLine->setPos(e->pos());
    e->accept();
}

void ScenesScreen::dropEvent(QDropEvent *e)
{
    mXYLine->setHidden(true);
    e->acceptProposedAction();

    QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    QPixmap pixmap;
    QPoint offset;

    QVariant qv;
    int flag ;
    dataStream >> flag  >> qv >> pixmap >> offset;

    QJsonValue val = QJsonValue::fromVariant(qv);

    auto lambda_func = [](QJsonValue val,QPoint pos)
    {
        QString n;
        n.sprintf("%s:%d,%s:%d",LX.toLocal8Bit().data(),pos.x(),LY.toLocal8Bit().data(),pos.y());
        QJsonObject json = val.toObject();
        json[PROPERTY] =  Compoent::changeJsonValue(val.toObject()[PROPERTY].toArray(),KEY_RECT,n);
        return json;
    };

    auto lambda_getpos = [=](QWidget *active){
        QPoint gp  = active->pos();
        BaseForm *tmp = (BaseForm*)active;
        while(tmp != tmp->mParent)
        {
            tmp = tmp->mParent;
            gp += tmp->pos();
        }

        gp = e->pos() - gp;
        if(gp.x() < 0 || gp.x() > active->width())
            gp.setX(0);
        if(gp.y() < 0 || gp.y() > active->height())
            gp.setY(0);
        return gp;
    };

    BaseForm::mReadJson = false;
    if(static_cast<int>(BaseForm::ObjTypes::T_NewLayer)  == flag)
    {

        createNewLayer(lambda_func(val,e->pos()),true);
    }else if(static_cast<int>(BaseForm::ObjTypes::T_NewLayout)  == flag){
        BaseForm *active =(BaseForm*)(mActiveObj);

       // QPoint gp = mapToGlobal(e->pos()) - mapToGlobal(active->pos());
        if(active->mType == BaseForm::T_NewLayer)
        {
            ((NewLayer*)active)->readLayoutFromJson(lambda_func(val,lambda_getpos(mActiveObj)),true);
        }else if(active->mType == BaseForm::T_NewLayout)
        {
            ((NewLayout*)active)->readFromJson(lambda_func(val,
                                                           lambda_getpos(mActiveObj)),true);
        }
    }else{
        BaseForm *active =(BaseForm*)(mActiveObj);
        ((NewLayout*)active)->readFromJson(lambda_func(val,
                                                       lambda_getpos(mActiveObj)),true);
    }
    mXYLine->raise();
    mWindow->statusBar()->showMessage(QString("控件数量: %1").arg(QString::number(BaseForm::mSeqEnameMap.size())));

}

void ScenesScreen::mouseReleaseEvent(QMouseEvent *)
{
    //    mXYLine->setHidden(true);
}


void ScenesScreen::updateNewPageSize()
{



    if(!BaseForm::cmpf(BaseForm::mWidthRate,1.0f,0.0001))
    {
        int n = float(this->width()) * BaseForm::mWidthRate;
        this->setFixedWidth(n);
    }

    if(!BaseForm::cmpf(BaseForm::mHeightRate,1.0f,0.0001))
    {
        int h = float(this->height()) * BaseForm::mHeightRate;
        this->setFixedHeight(h);
    }

    mXYLine->setGeometry(this->geometry());
    foreach (QWidget *w, childlist) {
        ((BaseForm*)w)->updateNewPageSize();
    }


    repaint();
}

HVLineWidget::HVLineWidget(QWidget *parent)
    :QWidget(parent)
{
    mVLine.setP1(pos());
    mVLine.setP2(pos());
    mHLine.setP1(pos());
    mHLine.setP2(pos());
    setAttribute(Qt::WA_AlwaysStackOnTop );
    setWindowFlags(Qt::FramelessWindowHint);
}

void HVLineWidget::mouseMoveEvent(QMouseEvent *e)
{
    mVLine.setP1(QPoint(e->pos().rx(),height()));
    mVLine.setP2(QPoint(e->pos().rx(),0));

    mHLine.setP1(QPoint(0,e->pos().ry()));
    mHLine.setP2(QPoint(width(),e->pos().ry()));

    update();
}


void HVLineWidget::setPos(const QPoint &p)
{
    mVLine.setP1(QPoint(p.x(),height()));
    mVLine.setP2(QPoint(p.x(),0));

    mHLine.setP1(QPoint(0,p.y()));
    mHLine.setP2(QPoint(width(),p.y()));
    //    CanvasManager::setXYPosition(p);
    mPos = p;

    update();
}

void HVLineWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QPen pen;
    pen.setColor(Qt::gray);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawLine(mVLine);

    painter.drawLine(mHLine);
    QString hstr;
    hstr.sprintf("x:%d,y:%d",mPos.x(),mPos.y());
    painter.drawText(mPos,hstr);
}

void HVLineWidget::mouseReleaseEvent(QMouseEvent *e)
{
    setHidden(true);
}

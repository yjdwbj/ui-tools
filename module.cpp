#include "module.h"
#include <QtCore/QMetaProperty>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStackedLayout>

#include <QtWidgets/QSpinBox>

#include <QtWidgets/QSpacerItem>

#include <QApplication>
#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSignalMapper>
#include <QLineEdit>

#include "config.h"
#include "scenesscreen.h"
#include "canvasmanager.h"

void Compoent::changeJsonValue(QString key, QVariant val)
{
   QJsonArray parr = dynValues[DKEY_DYN].toArray();
    int asize = parr.size();
    for(int i = 0;i < asize;i++)
    {
        QJsonObject obj = parr[i].toObject();
        // 这里必需要有-name 这个属性名.
        if(obj[NAME].toString().contains(key) )
        {
            if(obj.contains(DEFAULT))
            {
                obj[DEFAULT] = QJsonValue::fromVariant(val);
                parr[i] = obj;
                break;
            }else if(obj.contains(IMAGE))
            {
                obj[IMAGE] = QJsonValue::fromVariant(val);
                parr[i] = obj;
                break;
            }else if(obj.contains(LIST))
            {
                obj[LIST] = QJsonValue::fromVariant(val);
                parr[i] = obj;
                break;
            }

        }
    }
    // 这个QJsonObject 都是只读的,修改之后必需重新赋值.
    dynValues[DKEY_DYN] = parr;
}

QVariant Compoent::getJsonValue(QString key) const
{
    QJsonArray parr = dynValues[DKEY_DYN].toArray();
    int asize = parr.size();
    for(int i = 0;i < asize;i++)
    {
        QJsonObject obj = parr[i].toObject();
        if(obj[NAME].toString().contains(key))
        {
            if(obj.contains(DEFAULT))
            {
                return obj[DEFAULT].toVariant();
            }

        }
    }
    return QVariant("");
}


void Compoent::onBindValue(QWidget *w,const QVariantMap &map)
{
 //   qDebug() << " dyn size " << dynValues.size();
     QString n = w->metaObject()->className();
     QString uname = w->objectName();
     if(!n.compare("QLineEdit"))
     {
         QLineEdit *txt = (QLineEdit *)w;
         txt->setText(getJsonValue(uname).toString());
     }else if(!n.compare("QComboBox"))
     {
         QComboBox *cb = (QComboBox *)w;
         QVariant nlv =  map.value(LIST);
         if(nlv.isValid() )
         {
             // 处理图片列表与它的默认值.
             cb->clear();
             QVariantList nlist = nlv.toList();
             for(QVariantList::const_iterator it = nlist.begin();
                     it != nlist.end();++it)
             {
                 // example for key  is  "config/images/string/alarm_pol.bmp"
                 QString key = (*it).toString();
                 bool b = key.contains('/');
                 int idx = key.lastIndexOf(b ? '/' : '\\')+1;
                 cb->addItem(key.mid(idx));
             }
             changeJsonValue(LIST,nlv);

             foreach (QJsonValue val, dynValues[DKEY_DYN].toArray()) {
                 QJsonObject qobj = val.toObject();
                 if(qobj.contains(IMAGE))
                 {
                     QString defval  =qobj.value(IMAGE).toString();
                     bool b = defval.contains('/');
                     int idx =   defval.lastIndexOf( b ? '/' : '\\')+1;

                     cb->setCurrentText(defval.mid(idx));
                     break;
                 }
             }

           }else{
            cb->setCurrentText(getJsonValue(uname).toString());
         }

     }else if(!n.compare("QSpinBox"))
     {
         QSpinBox *sp = (QSpinBox *)w;
         sp->setValue(getJsonValue(uname).toDouble());
     }
 }


QJsonObject Compoent::getRectJson(QWidget *w)
{
    QJsonObject rect;
    QVariantMap vmap;
    vmap["x"] = QString::number(w->x());
    vmap["y"] = QString::number(w->y());
    vmap[WIDTH] = QString::number(w->width()-MARGIN_SIZE.width());
    vmap[HEIGHT] = QString::number(w->height()-MARGIN_SIZE.height());
    rect[KEY_RECT] = QJsonObject::fromVariantMap(vmap);
    return rect;
}

void Compoent::copyProperty(const QVariant &va)
{
    QJsonObject oo = QJsonValue::fromVariant(va).toObject();
    dynValues[DKEY_DYN] = QJsonValue::fromVariant(va);
}


void NewLabel::onEnumItemChanged(QString txt)
{
   QComboBox *cb =(QComboBox *)(QObject::sender());
   changeJsonValue(cb->objectName(),txt);
}

void NewLabel::onNumberChanged(int num)
{

    QSpinBox *sp = (QSpinBox *)(QObject::sender());
   // dynValues[sp->objectName()] = num;
    changeJsonValue(sp->objectName(),num);
}

void NewLabel::onTextChanged(QString str)
{
    QLineEdit *txt = (QLineEdit *)(QObject::sender());
  //  dynValues[txt->objectName()] = str;
    changeJsonValue(txt->objectName(),str);
}

/*------------------------------------------------------------------------*/

void NewFrame::onEnumItemChanged(QString txt)
{
   QComboBox *cb =(QComboBox *)(QObject::sender());
 //  qDebug() << " Frame enum value changed " << txt << cb->currentText();
 //  dynValues[cb->objectName()] = txt;
 changeJsonValue(cb->objectName(),txt);

    this->signalsBlocked();
}

void NewFrame::onNumberChanged(int num)
{

    QSpinBox *sp = (QSpinBox *)(QObject::sender());
    changeJsonValue(sp->objectName(),num);
  //  dynValues[sp->objectName()] = num;
}

void NewFrame::onTextChanged(QString str)
{
    QLineEdit *txt = (QLineEdit *)(QObject::sender());
  //  qDebug() << " txt changed" << txt->toPlainText();
    changeJsonValue(txt->objectName(),str);
  //  dynValues[txt->objectName()] = str;
}


NewLabel::NewLabel(QWidget *parent)
    :QLabel(parent),
     selIndex(0),disDefaultList(false)
{
    mWindow = ((NewFrame*)(parent->parentWidget()))->mWindow;
    this->setLineWidth(0);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
}


void NewLabel::addPropertyBoxSignal(QSpinBox *b)
{
    connect(b,SIGNAL(valueChanged(int)),this,SLOT(onXYWHChangedValue(int)));
}

void NewLabel::onXYWHChangedValue(int v)
{
    /* 绑定坐标控件的更新 */
    QWidget *sender =(QWidget *)(QObject::sender());

    QWidget *p = this->parentWidget();

  //  p->move(p->parentWidget()->mapFromGlobal(QCursor::pos()-mOffset));
    if(!sender->objectName().compare(X))
    {
        //o.setX(v);
        QPoint pos = p->pos();
        pos.setX(v);
        p->move(pos);

    }else if(!sender->objectName().compare(Y))
    {
        QPoint pos = p->pos();
        pos.setY(v);
        p->move(pos );

    }
}


void NewLabel::removeWidFromLayout(QLayout* layout)
{

    if(!layout)
        return;
    QLayoutItem* child;
    while(layout->count()!=0)
    {
        child = layout->takeAt(0);
        if(child->layout() != 0)
        {
            removeWidFromLayout(child->layout());
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }
}


void NewLabel::clearOtherObjectStyleSheet()
{
    /* 清除控件的红线框 */
    QList<NewLabel*> nflist =  this->parentWidget()->findChildren<NewLabel*>();
    foreach(NewLabel *x,nflist)
    {
           x->setStyleSheet("");

    }
}


QWidget *NewLabel::getQWidgetByName(QString name) const
{
    QWidgetList tlist = qApp->topLevelWidgets();

    for(QWidgetList::iterator wit = tlist.begin();wit != tlist.end();++wit)
    {
       // qDebug() << "find object by name :" << (*wit)->objectName();
        if((*wit)->objectName() == name)
        {
            return *wit;
            break;
        }
    }
    return (QWidget*)0;
}

void NewLabel::mousePressEvent(QMouseEvent *ev)
{

    /* 单击选中它的父对像 */

    NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
   // p->setState(SelectionHandleActive);
    mWindow->cManager->activeSS()->setSelectObject(p);

  //  p->setStyleSheet("NewFrame{border: 0.5px solid red;}");

    clearOtherObjectStyleSheet();
    mWindow->posWidget->setConnectNewQWidget(p);
    mWindow->propertyWidget->createPropertyBox(p);
    mWindow->imgPropertyWidget->delPropertyBox();
    mOffset = ev->pos();
    setCursor(Qt::ClosedHandCursor);
}


void NewLabel::mouseReleaseEvent(QMouseEvent *ev)
{

    /* 放开鼠标时检查它的是否出了边界要 */
    NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
    QPoint pos = p->pos();
    if(p->x() < 0)
    {
        pos.setX(0);
        p->move(pos);

    }
    if(p->y() < 0 )
    {
        pos.setY(0);
        p->move(pos);

    }

    QSize ms = p->parentWidget()->parentWidget()->size();
    if((p->x() + p->size().width()) > ms.width())
    {
        pos.setX( ms.width() - p->size().width() - MARGIN_SIZE.width() /2 );
        p->move(pos);
        qDebug() << " move pos " << pos;
    }

    if((p->y() + p->size().height()) > ms.height())
    {
        pos.setY(ms.height() - p->size().height() - MARGIN_SIZE.height() /2);
        p->move(pos);
    }
}

void NewLabel::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {
        NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
        p->move( p->pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        mWindow->posWidget->updatePosition(p->pos());
        p->blockSignals(true);
    }

}

void NewLabel::mouseDoubleClickEvent(QMouseEvent *event)
{

    NewFrame *p = (NewFrame *)this->parentWidget()->parentWidget();
    p->setState(SelectionHandleOff);


    clearOtherObjectStyleSheet();
   // p->setStyleSheet("");
    QList<NewLabel*> nlist =  p->findChildren<NewLabel*>();
    foreach (NewLabel *n, nlist) {
        if(n != this)
            n->setStyleSheet("");
    }

    setStyleSheet("QLabel{border: 1px solid red;border-style: outset;}");
    mWindow->propertyWidget->createPropertyBox(p);
    mWindow->imgPropertyWidget->createPropertyBox(this);
}


void NewLabel::onListImageChanged(QString img)
{
   foreach (QString s, myImageList) {
       QString k = s.section(":",0,0);
       if(!k.compare(img))
       {
           QString fpath = s.section(":",1,1);
           this->setPixmap(QPixmap(fpath));/* 更新图片 */
           int idx = QDir::currentPath().length() + 1;
           this->changeJsonValue(IMAGE,fpath.mid(idx));
           selIndex = myImageList.indexOf(s);
           break;
       }
   }

}

void NewLabel::updatePixmap(QString imgpath)
{
    this->setPixmap(QPixmap(imgpath));
}

void NewLabel::onPictureDialog(bool )
{
    QString key = QObject::sender()->objectName();
    // QMessageBox::warning(this,"test","your clicked me: ");
    ImageFileDialog *ifd = new ImageFileDialog(myImageList,this);

    ifd->exec();
    qDebug() << " ImageFileDialog ";
    //selectedMap sMap  = ifd->getSelectedMap();
    myImageList = ifd->getSelectedList();

    disDefaultList = myImageList.size() ? true : false;
    ifd->deleteLater();
    QJsonObject json;
    int rootlen  = QDir::currentPath().length()+1;
    QJsonArray qa;
   // qDebug() << " current path " << QDir::currentPath() << " len " << rootlen;
    foreach (QString s, myImageList) {
        // example for s   "alarm_du.bmp:/home/yjdwbj/build-ut-tools-Desktop_Qt_5_6_0_GCC_64bit-Debug/images/string/alarm_du.bmp
        QString substr = s.section(':',1,1).mid(rootlen).replace("\\","/");
        qa.append(substr);
    }
    json[LIST] = qa;
    // 把新的列表更新的json中.
    onBindValue(mWindow->imgPropertyWidget->getPropertyObject(key),json.toVariantMap());
    this->blockSignals(true);

}

void NewLabel::updateComboItems(QComboBox *cb)
{
    foreach (QString s, myImageList) {
       cb->addItem(s.section(":",0,0));
    }
    cb->setCurrentIndex(this->property(DKEY_IMGIDX).toInt());
}

void NewLabel::writeToJson(QJsonObject &json)
{
    QJsonArray projson;// 属性
    projson = dynValues[DKEY_DYN].toArray(); //复制出模版的属性来保存.
    json[NAME] = objectName();
    json[CLASS] = this->metaObject()->className();
    json[PROPERTY] = projson;
}


NewFrame::NewFrame(QWidget *parent)
    :FormResizer(parent)

{
    NewLayout * pwid = ((NewLayout*)parent->parentWidget());
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    setObjectName(CN_NEWFRAME);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setFocusPolicy(Qt::ClickFocus);
}


void NewFrame::delMySelf()
{
   QList<NewLabel*> nllist = this->findChildren<NewLabel*>();

   QListIterator<NewLabel*> it(nllist);

   while(it.hasNext())
   {
       QWidget *w = it.next();
       w->deleteLater();
   }

   mWindow->tree->deleteItem(this);
   this->deleteLater();
   mWindow->propertyWidget->delPropertyBox();
   mWindow->imgPropertyWidget->delPropertyBox();

}

void NewFrame::onSelectMe()
{
  //  setStyleSheet("NewFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
    clearOtherObjectStyleSheet();
    mWindow->cManager->activeSS()->setSelectObject(this);
    mWindow->posWidget->setConnectNewQWidget(this);
    mWindow->propertyWidget->createPropertyBox(this);
    mWindow->imgPropertyWidget->delPropertyBox();
    this->blockSignals(true);
}

void NewFrame::addMainWindow(QObject *mw)
{
    mWindow = (MainWindow*)mw;
}

void NewFrame::clearOtherObjectStyleSheet()
{
    /* 清除控件的红线框 */
    QList<NewFrame*> nflist =  this->parentWidget()->findChildren<NewFrame*>();
    foreach(NewFrame *x,nflist)
    {
           x->setStyleSheet("");
    }
}

void NewFrame::onXYWHChangedValue(int v)
{
    /* 绑定坐标控件的更新 */
    QWidget *sender =(QWidget *)(QObject::sender());
    if(!sender->objectName().compare(X))
    {
        QPoint pos = this->pos();
        pos.setX(v);
        move(pos);
    }else if(!sender->objectName().compare(Y))
    {
        QPoint pos = this->pos();
        pos.setY(v);
        move(pos );

    }

}

void NewFrame::writeToJson(QJsonObject &json)
{

    QJsonArray layoutarr;

    foreach (NewLabel *w, m_frame->findChildren<NewLabel*>()) {
        QJsonObject nlobj;
        w->writeToJson(nlobj);
        layoutarr.append(nlobj);

    }

    json[WIDGET] = layoutarr;
    QJsonArray projson;// 属性

    projson = dynValues[DKEY_DYN].toArray();
    int psize = projson.size();

    // 更新一些属性.
    for(int i =0; i < psize;i++)
    {
        QJsonObject ov = projson.at(i).toObject();

        if(ov.contains(KEY_RECT))
        {

            //ov = getRectJson(this);
            ov[KEY_RECT] = getRectJson(this)[KEY_RECT];
            projson[i] = ov;

        }else if(ov.contains(UID))
        {
            ov[UID] = this->property(DKEY_UID).toString();
            projson[i] = ov;

        }

    }

    json[PROPERTY] = projson;
    json[NAME] = objectName();
    json[CLASS] = this->metaObject()->className();
    json[CAPTION] = this->property(DKEY_CAPTION).toString();

}


NewLayout::NewLayout(QSize nsize,QWidget *parent):
    FormResizer(parent)
{
    setMinimumSize(nsize ); // 最小尺寸
    setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.
    this->setObjectName(this->metaObject()->className());
    setStyleSheet("NewLayout#%s{border: 0.5px solid #929292;}" % LAYOUT);
    setFocusPolicy(Qt::ClickFocus);
    show();
}


void NewLayout::onSelectMe()
{
   // setStyleSheet("NewFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
    clearOtherObjectStyleSheet();
    mWindow->cManager->activeSS()->setSelectObject(this);
    mWindow->posWidget->setConnectNewQWidget(this);
    mWindow->propertyWidget->createPropertyBox(this);
    mWindow->imgPropertyWidget->delPropertyBox();
    this->blockSignals(true);
}

void NewLayout::delMySelf()
{
    QListIterator<QWidget*> nf(mChList);
    while(nf.hasNext())
    {
        deleteObject(nf.next());
    }

    mWindow->tree->deleteItem(this);
    mWindow->posWidget->resetValues();

    deleteLater();
}


void NewLayout::onXYWHChangedValue(int v)
{
    /* 绑定坐标控件的更新 */
    QWidget *sender =(QWidget *)(QObject::sender());
    if(!sender->objectName().compare(X))
    {

        QPoint pos = this->pos();
        pos.setX(v);
        this->move(pos);

    }else if(!sender->objectName().compare(Y))
    {

        QPoint pos = this->pos();
        pos.setY(v);
        this->move(pos );

    }else if(!sender->objectName().compare(W))
    {
        if((this->pos().x() + v )> this->parentWidget()->size().width())
            return;
        QSize n(this->size());
        n.setWidth(v);
        this->resize(n);
    }else if(!sender->objectName().compare(H))
    {
        if((this->pos().y() + v )> this->parentWidget()->size().height())
            return;
        QSize n(this->size());
        n.setHeight(v);
        this->resize(n);
    }
    this->blockSignals(true);
}


void NewLayout::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mOffset = event->pos();
        onSelectMe();
    }else if(event->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        QAction delme("删除当前布局",this);
        connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));
        contextMenu->addAction(&delme);

        contextMenu->exec(mapToGlobal(event->pos()));
    }

}
void NewLayout::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(this->pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        mWindow->posWidget->updatePosition(this->pos());
        this->blockSignals(true);
    }

}

void NewLayout::onDeleteMe()
{
    //这里不能绕过它的父控件.

    //delMySelf();
    mWindow->cManager->activeSS()->delSelectedLayout();

}

void NewLayout::mouseReleaseEvent(QMouseEvent *event)
{
    /* 放开鼠标时检查它的是否出了边界要 */
    QWidget *p = this->parentWidget();
    QPoint pos = this->pos();
    if(this->x() < 0)
    {
        pos.setX(0 - MARGIN_SIZE.width() /2);
        this->move(pos);

    }
    if(this->y() < 0 )
    {
        pos.setY(0 -MARGIN_SIZE.width() /2);
        this->move(pos);
    }

    QSize ms = p->size();
    //左出界检查
    if((this->x()  + this->size().width()) > ms.width())
    {
        pos.setX( ms.width() - this->size().width() + MARGIN_SIZE.width() /2 );
        this->move(pos);

    }

    //上出界检查
    if((this->y() + this->size().height()) > ms.height())
    {
        pos.setY(ms.height() - this->size().height() + MARGIN_SIZE.height() /2);
        this->move(pos);
    }

    // 这里只能在释放鼠标时改变左边的控件值
    mWindow->posWidget->updateSize(this->size());
}

void NewLayout::clearOtherObjectStyleSheet()
{
    /* 清除控件的红线框 */
    QList<NewFrame*> nflist =  m_frame->findChildren<NewFrame*>();
    foreach(NewFrame *x,nflist)
    {
           x->setStyleSheet("");

    }

}

void NewLayout::clearOtherSelectHandler()
{

    QWidgetList laylist = this->parentWidget()->findChildren<QWidget*>();
    foreach (QWidget *wid, laylist) {

        if (wid == this)
            continue;

        if(!CN_NEWFRAME.compare(wid->metaObject()->className())
            || !CN_NEWLAYOUT.compare(wid->metaObject()->className()))
        {
            ((FormResizer*)wid)->setState(SelectionHandleOff);
        }
    }
}



void NewLayout::deleteObject(int index)
{
    QWidget *w = mChList.at(index);
    mChList.removeAt(index);
     ((NewFrame*)w)->delMySelf();
}

void NewLayout::deleteObject(QWidget *w)
{
    int i = mChList.indexOf(w);
    mChList.removeAt(i);
    ((NewFrame*)w)->delMySelf();

}

void NewLayout::writeToJson(QJsonObject &json)
{

        QJsonArray layoutarr;
      //  qDebug() << "NewLayout  parent object " << &json;

        foreach (QWidget *w, mChList) {
            QJsonObject frameObj;
            frameObj[NAME] = w->objectName();
            ((NewFrame*)w)->writeToJson(frameObj);
            layoutarr.append(frameObj);
        }
        // 这一句必需要在这个偱环后面.
        json[LAYOUT] = layoutarr;
        json[CLASS] = this->metaObject()->className();
        json[CAPTION] = this->property(DKEY_LOCALSEQ).toString();
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
}

void NewLayout::readFromJson(const QJsonArray &array)
{
    foreach (QJsonValue val, array) {
        switch (val.type()) {
        case QJsonValue::Object:
        {
            QJsonObject valobj = val.toObject();
             NewFrame *nf =  mWindow->ComCtrl->ReadObjectFromJson(valobj.toVariantMap(),m_frame,
                                                 valobj[CAPTION].toString(),valobj[NAME].toString());

             foreach (QJsonValue pval, valobj[PROPERTY].toArray()) {
                 QJsonObject pobj = pval.toObject();
                 if(pobj.contains(UID))
                 {
                     nf->setProperty(DKEY_UID,pobj[UID].toString());
                 }
             }
             nf->onSelectMe();

        }

            break;
        default:
            break;
        }
    }
}

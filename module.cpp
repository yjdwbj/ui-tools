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
#include <QFileDialog>
#include <QColorDialog>
#include <QRgb>
#include <QScrollBar>

#include "config.h"
#include "scenesscreen.h"
#include "canvasmanager.h"

void Compoent::changeJsonValue(QJsonArray &parr,QString key,
                               const QVariant &val)
{
    int asize = parr.size();
    for(int i = 0;i < asize;i++)
    {
        QJsonObject obj = parr[i].toObject();
        if(obj.contains(STRUCT))
        {
            QJsonArray arr = obj[STRUCT].toArray();
            changeJsonValue(arr,key,val);
            obj[STRUCT] = arr;
            parr[i] = obj;
            continue;
        }
        // 这里必需要有-name 这个属性名.
        if(obj[NAME].toString().contains(key) )
        {
            if(obj.contains(DEFAULT))
            {
                obj[DEFAULT] = QJsonValue::fromVariant(val);

            }else if(obj.contains(ENAME))
            {
                obj[ENAME] = QJsonValue::fromVariant(val);
            }
            else if(obj.contains(IMAGE))
            {
                obj[IMAGE] = QJsonValue::fromVariant(val);
//                parr[i] = obj;
//                break;
            }
            else if(obj.contains(LIST))
            {
                obj[LIST] = QJsonValue::fromVariant(val);
//                parr[i] = obj;
//                break;
            }else if(obj.contains(GRAYCOLOR)){

                obj[GRAYCOLOR] = QJsonValue::fromVariant(val);
//                parr[i] = obj;
//                break;
            }else if(obj.contains(COLOR))
            {
               obj[COLOR] = QJsonValue::fromVariant(val);
            }else if(obj.contains(BAKCOLOR))
            {
               obj[BAKCOLOR] = QJsonValue::fromVariant(val);
            }
            else if(obj.contains(BORDER))
            {

            }else if(obj.contains(KEY_RECT))
            {

            }
            parr[i] = obj;
            break;
        }
    }
}

void Compoent::changeJsonValue(QString key, QVariant val)
{
   QJsonArray parr = dynValues[DKEY_DYN].toArray();
   changeJsonValue(parr,key,val);
   // int asize = parr.size();

    // 这个QJsonObject 都是只读的,修改之后必需重新赋值.
    dynValues[DKEY_DYN] = parr;
}

void Compoent::updateRBJsonValue(QJsonArray &projson, QWidget *w)
{
    int psize =projson.size();
    for(int i =0; i < psize;i++)
    {
        QJsonObject ov = projson.at(i).toObject();

        if(ov.contains(STRUCT))
        {
            QJsonArray array =  ov[STRUCT].toArray();
            updateRBJsonValue( array,w);
            ov[STRUCT] = array;
            projson[i] = ov;

        }else if(ov.contains(KEY_RECT))
        {

            //ov = getRectJson(this);
            ov[KEY_RECT] = getRectJson(w)[KEY_RECT];
            projson[i] = ov;

        }else if(ov.contains(UID))
        {
            ov[UID] = w->property(DKEY_UID).toString();
            projson[i] = ov;

        }
        else if(ov.contains(BORDER))
        {
            ov[BORDER] = getBorderJson(w)[BORDER];
            projson[i] = ov;
        }

    }
}

QVariant Compoent::getJsonValue(const QJsonArray &parr,QString key) const
{
    int asize = parr.size();
    for(int i = 0;i < asize;i++)
    {
        QJsonObject obj = parr[i].toObject();
        if(obj.contains(STRUCT))
        {
            return getJsonValue(obj[STRUCT].toArray(),key);
        }
        if(obj[NAME].toString().contains(key))
        {
            if(obj.contains(DEFAULT))
            {
                return obj.value(DEFAULT).toVariant();
            }else if(obj.contains(COLOR))
            {
                return obj.value(COLOR).toVariant();
            }else if(obj.contains(GRAYCOLOR))
            {
                return obj.value(GRAYCOLOR).toVariant();
            }else if(obj.contains(BAKCOLOR))
            {
                return obj.value(BAKCOLOR).toVariant();
            }else if(obj.contains(ENAME))
            {
                return obj.value(ENAME).toVariant();
            }

        }
    }
    return QVariant("");
}

QVariant Compoent::getJsonValue(QString key) const
{

    return getJsonValue(dynValues[DKEY_DYN].toArray(),key);

}


void Compoent::onBindValue(QWidget *w,const QVariantMap &map)
{
 //   qDebug() << " dyn size " << dynValues.size();
     // 通过关键字来设置控件的默认值.
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
     else if(!n.compare("QPushButton"))
     {
           QColor c;
          if(!uname.compare(BAKCOLOR))
          {
               c = getJsonValue(BAKCOLOR).toString() ;
//              QPixmap p(12,12);
//              p.fill(c);

//            ((QPushButton*)w)->setIcon(p);
//            ((QPushButton*)w)->setProperty(DKEY_COLOR,c);

          }
          else if(!uname.compare(BORDER))
          {
              //unsigned int n = getJsonValue(BORDER).toUInt();
              //灰度转成ＲＧＢ
             // QColor c(n & 0xfff00000,n &0xfff00,n & 0xff);
              //QColor c = QColor::fromRgb(n);

          }
          QPixmap p(12,12);
          p.fill(c);

          ((QPushButton*)w)->setIcon(p);
          ((QPushButton*)w)->setProperty(DKEY_COLOR,c);
     }
 }


QJsonObject Compoent::getRectJson(QWidget *w)
{
    QJsonObject rect;
    QVariantMap vmap;
    vmap[LX] = QString::number(w->x());
    vmap[LY] = QString::number(w->y());
    vmap[WIDTH] = QString::number(w->width()/*-MARGIN_SIZE.width()*/);
    vmap[HEIGHT] = QString::number(w->height()/*-MARGIN_SIZE.height()*/);
    rect[KEY_RECT] = QJsonObject::fromVariantMap(vmap);
    return rect;
}

 QRect Compoent::getRectFromStruct(const QJsonArray &arr)
{
    QJsonObject rectobj ;
    foreach (QJsonValue pval, arr) {
        QJsonObject pobj = pval.toObject();
        if(pobj.contains(KEY_RECT))
        {
            rectobj =  pobj[KEY_RECT].toObject();
            break;
        }else if(pobj.contains(STRUCT)) {
            return getRectFromStruct(pobj[STRUCT].toArray());
        }
    }
    QRect rect(0,0,0,0);
    if(!rectobj.isEmpty())
    {
        QVariantMap obj =  rectobj.toVariantMap();
        rect = QRect(obj[LX].toString().toInt(),
                obj[LY].toString().toInt(),
                obj[WIDTH].toString().toInt(),
                obj[HEIGHT].toString().toInt());
    }
    return rect;


}



QJsonObject Compoent::getBorderJson(QWidget *w)
{
    QJsonObject border;
    QVariantMap vmap ;
    BaseForm *fr = (BaseForm*)w;
    vmap["left"] = fr->mBorder.x();
    vmap["top"] = fr->mBorder.y();
    vmap["right"] = fr->mBorder.width();
    vmap["bottom"] = fr->mBorder.height();
    border[BORDER] = QJsonObject::fromVariantMap(vmap);
    return border;
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



NewLabel::NewLabel(QWidget *parent)
    :QLabel(parent),
     selIndex(0),disDefaultList(false)
{
    mWindow = ((NewFrame*)(parent->parentWidget()))->mWindow;
   // mWindow = ((NewFrame*)parent)->mWindow;
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

    //NewFrame *p =(NewFrame*) (this->parentWidget());
    // 下面如果它的父控件是NewFrame->m_frame
    NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
    if(ev->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        QAction delme(QString("删除当前-%1").arg(p->property(DKEY_TXT).toString()),this);
        connect(&delme,SIGNAL(triggered(bool)),p,SLOT(onDeleteMe()));
        contextMenu->addAction(&delme);
        contextMenu->exec(mapToGlobal(ev->pos()));
    }else
    {
        // p->setState(SelectionHandleActive);
         mWindow->cManager->activeSS()->setSelectObject(p);

       //  p->setStyleSheet("NewFrame{border: 0.5px solid red;}");

         clearOtherObjectStyleSheet();
         mWindow->posWidget->setConnectNewQWidget(p);
         mWindow->propertyWidget->createPropertyBox(p);
         mWindow->imgPropertyWidget->delPropertyBox();
    }
    mOffset = ev->pos();
    setCursor(Qt::ClosedHandCursor);
}


void NewLabel::mouseReleaseEvent(QMouseEvent *ev)
{

    /* 放开鼠标时检查它的是否出了边界要 */
    NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
    //NewFrame *p =(NewFrame*) (this->parentWidget());
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
        pos.setX( ms.width() - p->size().width() /*- MARGIN_SIZE.width() /2*/ );
        p->move(pos);
        qDebug() << " move pos " << pos;
    }

    if((p->y() + p->size().height()) > ms.height())
    {
        pos.setY(ms.height() - p->size().height() /*- MARGIN_SIZE.height() /2*/);
        p->move(pos);
    }
}

void NewLabel::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {
        //NewFrame *p =(NewFrame*)(this->parentWidget());
        NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
        p->move( p->pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        mWindow->posWidget->updatePosition(p->pos());
        p->blockSignals(true);
    }

}

void NewLabel::mouseDoubleClickEvent(QMouseEvent *event)
{

   // NewFrame *p = (NewFrame *)this->parentWidget();
    NewFrame *p = (NewFrame *)this->parentWidget()->parentWidget(); // 如果父控件是NewFrame->m_frame
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


BaseForm::BaseForm(QWidget *parent)
    :FormResizer(parent), mBorderColor("#FFFFFF")
{


}


void BaseForm::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(this->pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        mWindow->posWidget->updatePosition(this->pos());
        this->blockSignals(true);
    }
}

void BaseForm::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mOffset = event->pos();
        onSelectMe();
    }else if(event->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        QAction delme(QString("删除当前-%1").arg(this->property(DKEY_TXT).toString()),this);
        connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));



        QAction hideobj("隐藏",this);

        connect(&hideobj,SIGNAL(triggered(bool)),SLOT(onSwapViewObject(bool)));

        contextMenu->addAction(&delme);
        if(!CN_NEWLAYOUT.compare(metaObject()->className()))
        {
            QAction saveTemp("保存成控件",this);
            contextMenu->addAction(&saveTemp);
            connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));
        }

        contextMenu->addAction(&hideobj);
        contextMenu->exec(mapToGlobal(event->pos()));
    }

}

void BaseForm::onSwapViewObject(bool b)
{
    mWindow->tree->onSwapShowHideObject(false);
}

void BaseForm::mouseReleaseEvent(QMouseEvent *event)
{


    /* 放开鼠标时检查它的是否出了边界要 */
    QWidget *p = this->parentWidget();
    QPoint pos = this->pos();
    if(this->x() < 0)
    {
        pos.setX(0 /*- MARGIN_SIZE.width() /2*/);
        this->move(pos);

    }
    if(this->y() < 0 )
    {
        pos.setY(0 /*-MARGIN_SIZE.width() /2*/);
        this->move(pos);
    }

    QSize ms = p->size();
    //左出界检查
    if((this->x()  + this->size().width()) > ms.width())
    {
        pos.setX( ms.width() - this->size().width() /*+ MARGIN_SIZE.width() /2*/ );
        this->move(pos);

    }

    //上出界检查
    if((this->y() + this->size().height()) > ms.height())
    {
        pos.setY(ms.height() - this->size().height() /*+ MARGIN_SIZE.height() /2*/);
        this->move(pos);
    }

    // 这里只能在释放鼠标时改变左边的控件值
    mWindow->posWidget->updateSize(this->size());
}


void BaseForm::onXYWHChangedValue(int v)
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

void BaseForm::onNumberChanged(int num)
{

    QSpinBox *sp = (QSpinBox *)(QObject::sender());
    changeJsonValue(sp->objectName(),num);
  //  dynValues[sp->objectName()] = num;
}

void BaseForm::onTextChanged(QString str)
{
    QLineEdit *txt = (QLineEdit *)(QObject::sender());

    changeJsonValue(txt->objectName(),str);
  //  dynValues[txt->objectName()] = str;
}



void BaseForm::addChildrenToTree()
{

}

void BaseForm::onDeleteMe()
{
    DeleteMe();
}

void BaseForm::onBorderChangedValue(int v)
{
    QString name = QObject::sender()->objectName();
//    qDebug() << " my object name is " << this->objectName()
//             << " class name " << this->metaObject()->className()
//             << " parent class name " << this->parentWidget()->metaObject()->className();
    if(!name.compare(X))
    {

        mBorder.setX(v);
    }else if(!name.compare(Y))
    {

        mBorder.setY(v);
    }else if(!name.compare(W))
    {

        mBorder.setWidth(v);
    }else if(!name.compare(H))
    {

        mBorder.setHeight(v);
    }

    updateBorderColor();
    update();
    //this->blockSignals(true);
}

void BaseForm::updateBorderColor()
{
//     QString str =
//                   "border-style: outset;"
//                   "border-top-width: %1px;"
//                   "border-left-width: %2px;"
//                   "border-right-width: %3px;"
//                   "border-bottom-width: %4px;"
//                   "border-color: %5;";

    QString str = QString("border-style: outset; "\
                          "border-color: %5; "\
                          "border-left-width: %1px; "\
                          "border-top-width: %2px; " \
                          "border-right-width: %3px; "\
                          "border-bottom-width: %4px;").arg(QString::number(mBorder.x()),
                                                               QString::number(mBorder.y()),
                                                               QString::number(mBorder.width()),
                                                               QString::number(mBorder.height()),
                                                               mBorderColor);

     this->setStyleSheet(QString("BaseForm#%1 { %2 }").arg(this->objectName(),str));
     //setStyleSheet(str);
   // this->setAccessibleName();
    qDebug() << " this object name " << this->objectName();
    qDebug()  << " this stylesheet " << this->styleSheet();
}

void BaseForm::onEnumItemChanged(QString txt)
{
   QComboBox *cb =(QComboBox *)(QObject::sender());
 //  qDebug() << " Frame enum value changed " << txt << cb->currentText();
 //  dynValues[cb->objectName()] = txt;
 changeJsonValue(cb->objectName(),txt);

    this->signalsBlocked();
}

void BaseForm::onColorButtonClicked()
{
    QPushButton *btn = (QPushButton*)QObject::sender();
    QColor c = QColor::fromRgb(btn->property(DKEY_COLOR).toUInt());
    QColorDialog *color =new  QColorDialog(c,  this);
    color->setOptions( QColorDialog::DontUseNativeDialog);


    color->exec();
    c =  color->selectedColor();

    if(c.isValid())
    {
       // qDebug() << " you selected color is " << c.rgb();
        QPixmap p(12,12);
        p.fill(c);
        btn->setIcon(p);
        btn->update();
        QString objname = btn->objectName();
        if(!objname.compare(BAKCOLOR))
        {
            this->changeJsonValue(BAKCOLOR,c.name());
            setStyleSheet(QString("background-color: %1;").arg(c.name(QColor::HexRgb)));

        }
        else if(!objname.compare(BORDER))
        {

            this->changeJsonValue(BORDER,qGray(c.rgb()));
            this->mBorderColor = c.name(QColor::HexArgb);
            this->updateBorderColor();
        }

    }

}


void BaseForm::removeChild(QWidget *w)
{
    childlist.removeOne(w);
}

void BaseForm::onSelectMe()
{
  //  setStyleSheet("NewFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
  //  clearOtherObjectStyleSheet();
    mWindow->cManager->activeSS()->setSelectObject(this);
    mWindow->posWidget->setConnectNewQWidget(this);
    mWindow->propertyWidget->createPropertyBox(this);
    mWindow->imgPropertyWidget->delPropertyBox();
    this->blockSignals(true);
}

void BaseForm::DeleteMe()
{
   // QListIterator<QWidget*> nf(childlist);

    QListIterator<QWidget*> item(childlist);
    while(item.hasNext())
    {
       // deleteObject(nf.next());
        QWidget *w = item.next();
        if(!CN_NEWLABEL.compare(w->metaObject()->className()))
        {
            childlist.removeOne(w);
            w->deleteLater();
        }else{
           ((BaseForm*)w)->DeleteMe();
        }

    }

    mWindow->tree->setMyParentNode();  //选中它的父控件.
    mWindow->tree->deleteItem(this);
    mWindow->posWidget->resetValues();
    mWindow->ComCtrl->ProMap.remove(property(DKEY_LOCALSEQ).toString());
    // 它是否是列表控件的一员.

    deleteLater();
}


NewFrame::NewFrame(QWidget *parent)
  //  :FormResizer(parent),Compoent()
    :BaseForm(parent)

{
   // NewLayout * pwid = ((NewLayout*)parent->parentWidget());
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    //setObjectName(CN_NEWFRAME);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setFocusPolicy(Qt::ClickFocus);

    QString uname = QString("%1_%2").arg("控件",QString::number(mWindow->ComCtrl->ProMap.size()));

    setProperty(DKEY_LOCALSEQ,uname);
    setObjectName(uname);
    setToolTip(uname);
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

   mWindow->tree->setMyParentNode();
   mWindow->tree->deleteItem(this);
   this->deleteLater();
   mWindow->propertyWidget->delPropertyBox();
   mWindow->imgPropertyWidget->delPropertyBox();

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


void NewFrame::onDeleteMe()
{
    //delMySelf();
    DeleteMe();
}

void NewFrame::readFromJson(const QJsonObject &json)
{
    QRect oldrect = Compoent::getRectFromStruct(json[PROPERTY].toArray());
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = json[CLASS].toString();
    QString caption = json[CAPTION].toString();
    qDebug() << " read from json caption is " << caption;
    QVariant variant = json[PROPERTY].toVariant();
    if(!clsName.compare(CN_NEWLABEL) || !clsName.compare(QLABEL))
    {
        NewLabel *newLabel = new NewLabel(m_frame);
        if(json.contains(PROPERTY))
        {
            newLabel->copyProperty(variant);
            newLabel->setProperty(DKEY_DYN,variant);
        }
        newLabel->setGeometry(oldrect);
        //    newLabel->addMainWindow(mWindow);
        childlist.append(newLabel);

        //   LayoutList.append(nl);
        // newLabel->onSelectMe();
        foreach (QJsonValue val, json.value(PROPERTY).toArray()) {
           QJsonObject obj = val.toObject();
           if(obj.contains(IMAGE))
           {
               QPixmap p;
               QString path = obj[IMAGE].toString();
               if(path.contains("\\"))
               {
                   path.replace("\\","/");
               }
               p.load(path);
               newLabel->setPixmap(p);
               break;
               //  ((NewLabel *)nobj)->setFixedSize(p.size());
           }

        }



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
    updateRBJsonValue(projson,this);
//    int psize = projson.size();

    // 更新一些属性.
//    for(int i =0; i < psize;i++)
//    {
//        QJsonObject ov = projson.at(i).toObject();

//        if(ov.contains(KEY_RECT))
//        {

//            //ov = getRectJson(this);
//            ov[KEY_RECT] = getRectJson(this)[KEY_RECT];
//            projson[i] = ov;

//        }else if(ov.contains(UID))
//        {
//            ov[UID] = this->property(DKEY_UID).toString();
//            projson[i] = ov;

//        }
//        else if(ov.contains(BORDER))
//        {
//            ov[BORDER] = getBorderJson(this)[BORDER];
//            projson[i] = ov;
//        }

//    }

    json[PROPERTY] = projson;
    dynValues[DKEY_DYN] = projson;
    json[NAME] = objectName();
    json[CLASS] = this->metaObject()->className();
   // json[CAPTION] = this->property(DKEY_CAPTION).toString();
    json[CAPTION] = this->property(DKEY_TXT).toString();

}


void NewFrame::mousePressEvent(QMouseEvent *ev)
{
    onSelectMe();
    mOffset = ev->pos();
    setCursor(Qt::ClosedHandCursor);
    this->blockSignals(true);
}



void NewFrame::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {
        //NewFrame *p =(NewFrame*)(this->parentWidget());

       move( pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        mWindow->posWidget->updatePosition(pos());
       this->blockSignals(true);
    }

}



NewList::NewList(const QSize size, QWidget *parent):
    //FormResizer(parent)
    BaseForm(parent)
{
  //  setMinimumSize(nsize ); // 最小尺寸
   // setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.

    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
  //  this->setObjectName(this->metaObject()->className());
    setStyleSheet("border: 0.5px solid #79AEC8;" );
    setFocusPolicy(Qt::ClickFocus);
    //setMinimumSize(size);
   // mainListWidget = new QListWidget(this);


    mainScroll =  new QScrollArea(m_frame);

    mainScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mainScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   // mainScroll->setWidgetResizable(true);

    mainWidget = new QWidget();
    mainWidget->setObjectName(LISTWIDGET);
    mainScroll->setWidget(mainWidget);
//    connect(mainScroll->verticalScrollBar(),
//            SIGNAL(valueChanged(int)),SLOT(onSliderValueChanaged(int)));

    listLayout = new QVBoxLayout(mainWidget);

    listLayout->setMargin(0);
    listLayout->setSpacing(0);
    listLayout->setSizeConstraint(QLayout::SetFixedSize);

    //mainWidget->setLayout(listLayout);
   // listLayout->setContentsMargins(0,0,0,this->height()/2);
  //  mainWidget->setStyleSheet("border: 1.5px solid red;");
    mainWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    m_frame->setStyleSheet("background-color: #79AEC8;");



    int n = mWindow->ComCtrl->ProMap.size();
    QString str = QString("%1_%2").arg("列表",QString::number(n));
    setProperty(DKEY_LOCALSEQ,str);
    setObjectName(str);
    mainWidget->setMaximumSize(999,999);

    setToolTip(str);
    onSelectMe();
    show();
    update();
    resize(size);
   // mainScroll->resize(this->size());
   // mainWidget->resize(this->size());


}

void NewList::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mOffset = event->pos();
        onSelectMe();
    }else if(event->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        QAction delme("删除当前控件",this);
        connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));
     //   QAction saveTemp("保存成控件",this);
     //   connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));
        contextMenu->addAction(&delme);
     //   contextMenu->addAction(&saveTemp);
        QAction addline("添加一行",this);
        contextMenu->addAction(&addline);
        connect(&addline,SIGNAL(triggered(bool)),SLOT(onAddOneLine()));

        contextMenu->exec(mapToGlobal(event->pos()));
    }

}


//void NewList::DeleteMe()
//{

//}

void NewList::onColorButtonClicked()
{

}


void NewList::onXYWHChangedValue(int v)
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

void NewList::onAddOneLine()
{
    onSelectMe();
    NewLayout *nl = new NewLayout(m_frame->size()-QSize(15,15),mWindow,mainWidget);
    nl->parentList = this;
  //  nl->addMainWindow(mWindow);
    childlist.append(nl);
    int n = mWindow->ComCtrl->ProMap.size();
    //mWindow->cManager->activeSS()->LayoutList.append(nl);
    nl->setProperty(DKEY_INTOLIST,true);

    QString val = QString("%1_%2").arg("布局",QString::number(n));

    nl->setProperty(DKEY_LOCALSEQ,val);
    mWindow->tree->addObjectToCurrentItem(nl);
    nl->onSelectMe();
    nl->show();;
    nl->setToolTip(nl->property(DKEY_LOCALSEQ).toString());
    listLayout->addWidget(nl);

  //  m_frame->adjustSize();
    int max = 0;
    foreach (QWidget *w, childlist) {
      //  ((NewLayout*)w)->resize(this->size());
        ((NewLayout*)w)->setMaximumSize(this->size());
        ((NewLayout*)w)->setFixedSize(this->size());
        max += w->height();
    }
   // mainScroll->verticalScrollBar()->setSliderDown(true);

   mainScroll->verticalScrollBar()->setRange(0,max);
   mainScroll->verticalScrollBar()->setValue(max);

   //qDebug() << " v bar max is " << mainScroll->verticalScrollBar()->maximum();

}

void NewList::onDeleteMe()
{
    if(childlist.size())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("删除提示");
        QString msg = QString("你真的要删除当前%1吗?删除之后不可以撤消,请选择<删除>删除.").arg(this->property(DKEY_TXT).toString());
        msgBox.setText(msg);
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"删除");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //qDebug() << " QMessageBox result " << ret;
        if(ret == QMessageBox::Yes)
        {

            DeleteMe();
        }
    }
}


void NewList::onSliderValueChanaged(int v)
{
    qDebug() << " values is " << v;
}



void NewList::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(this->pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        mWindow->posWidget->updatePosition(this->pos());
        this->blockSignals(true);
    }
}

void NewList::mouseReleaseEvent(QMouseEvent *event)
{
    /* 放开鼠标时检查它的是否出了边界要 */
    QWidget *p = this->parentWidget();
    QPoint pos = this->pos();

    if(this->x() < 0)
    {
        pos.setX(0 );
        this->move(pos);

    }
    if(this->y() < 0 )
    {
        pos.setY(0 );
        this->move(pos);
    }

    QSize ms = p->size();


    //左出界检查
    if((this->x()  + this->size().width()) > ms.width())
    {
        pos.setX( ms.width() - this->size().width()  );
        this->move(pos);
    }

    //上出界检查
    if((this->y() + this->size().height()) > ms.height())
    {
        pos.setY(ms.height() - this->size().height() );
        this->move(pos);
    }

//    mainListWidget->resize(m_frame->size());

    // 把　QScrollArea　的大小调成与父控件一样大.
    mainScroll->resize(this->size());
    foreach (QWidget *w, childlist) {
        ((NewLayout*)w)->setMaximumSize(this->size());
         ((NewLayout*)w)->setFixedWidth(this->size().width());
      //  mWindow->posWidget->updateMaxSize(this->size());
    }

}

void NewList::writeToJson(QJsonObject &json)
{
    QJsonArray layoutarr;

    foreach (QWidget *w, childlist) {
        QJsonObject outjson;
        outjson[NAME] = w->objectName();
        qDebug() << " childlist  " << w->metaObject()->className();
        // 这里只有两种对像类型,NewLayout , NewFrame;
        QString clsname = w->metaObject()->className();

        if(!clsname.compare(CN_NEWLAYOUT))
        {
            ((NewLayout*)w)->writeToJson(outjson);
        }else{
            ((NewFrame*)w)->writeToJson(outjson);
        }

        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LISTWIDGET] = layoutarr;
    json[CLASS] = this->metaObject()->className();
   // json[CAPTION] = this->property(DKEY_LOCALSEQ).toString();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    QJsonArray projson;
    projson.append(Compoent::getRectJson(this));
    json[PROPERTY] = projson;

}


void NewList::readFromJson(const QJsonObject &valobj)
{

    QRect oldrect = getRectFromStruct(valobj[PROPERTY].toArray());
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = valobj[CLASS].toString();
    if(!clsName.compare(CN_NEWLAYOUT) || !clsName.compare(LAYOUT))
    {
        onAddOneLine();
        NewLayout *nl = (NewLayout*)(childlist.last());
        nl->setGeometry(oldrect);

        // 这里一定是Layout 嵌套了.
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            nl->readFromJson(item.toObject());
        }
        m_frame->adjustSize();
    }
//    else if(!clsName.compare(CN_NEWFRAME) || !clsName.compare(QFRAME))
//    {
//        mWindow->ComCtrl->createLayoutFromJson(valobj,this);
//    }

}

void NewList::onTextChanged(QString str)
{
    QLineEdit *txt = (QLineEdit *)(QObject::sender());

    changeJsonValue(txt->objectName(),str);
  //  dynValues[txt->objectName()] = str;
}


void NewList::addChildrenToTree()
{
    foreach (QWidget *w, childlist) {
        mWindow->tree->addObjectToCurrentItem(w);
        ((NewLayout*)w)->addChildrenToTree();
    }
}

NewLayout::NewLayout(QSize nsize, MainWindow *w, QWidget *parent):
   // FormResizer(parent)
   BaseForm(parent)
{
    this->mWindow = w;
    //setMinimumSize(nsize ); // 最小尺寸
    resize(nsize);
    if(parent)
        setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.
    //this->setObjectName(this->metaObject()->className());
    setStyleSheet("NewLayout#%s{border: 0.5px solid #929292;}" % LAYOUT);
    setFocusPolicy(Qt::ClickFocus);
    m_frame->setStyleSheet("background-color: #DAE4A7;");
    show();


    QString uname = QString("%1_%2").arg("布局",
                                         QString::number(mWindow->ComCtrl->ProMap.size()));
    setProperty(DKEY_LOCALSEQ,uname );
    setObjectName(uname.toUtf8());
    setToolTip(uname);
}

void NewLayout::onSelectMe()
{
   // setStyleSheet("NewFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
   // clearOtherObjectStyleSheet();

    if(!parentWidget()->objectName().compare(LISTWIDGET))
    {
        // 滑动到适当的位置.
        int n = 0;
        foreach(NewLayout *w,parentWidget()->findChildren<NewLayout*>())
        {
            if(this == w) break;
            n+= w->height();
        }
      ((NewList*)parentList)->mainScroll->verticalScrollBar()->setValue(n);
    }

    this->BaseForm::onSelectMe();
//    mWindow->cManager->activeSS()->setSelectObject(this);
//    mWindow->posWidget->setConnectNewQWidget(this);
//    mWindow->propertyWidget->createPropertyBox(this);
//    mWindow->imgPropertyWidget->delPropertyBox();

//    this->blockSignals(true);
}

void NewLayout::DeleteMe()
{

    // 它是否是列表控件的一员.
    if(!parentWidget()->objectName().compare(LISTWIDGET))
    {
        ((NewList*)parentList)->childlist.removeOne(this);
    }
    this->BaseForm::DeleteMe();

}

void NewLayout::addChildrenToTree()
{
    foreach (QWidget *w, childlist) {
        QString clsname = w->metaObject()->className();
        mWindow->tree->addObjectToCurrentItem(w);
        if(!clsname.compare(CN_NEWLIST)) {
            ((NewList*)w)->addChildrenToTree();

        }else if(!clsname.compare(CN_NEWLAYOUT))
        {
            ((NewLayout*)w)->addChildrenToTree();
        }
    }
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
       // this->setFixedWidth(v);
    }else if(!sender->objectName().compare(H))
    {
        if((this->pos().y() + v )> this->parentWidget()->size().height())
            return;
        QSize n(this->size());
        n.setHeight(v);
        this->resize(n);
       // this->setFixedHeight(v);
    }
    this->blockSignals(true);
}


void NewLayout::mousePressEvent(QMouseEvent *event)
{
  //  emit sizeChanged();
   // setMaximumSize(this->parentWidget()->size());

    if (event->button() == Qt::LeftButton)
    {
        mOffset = event->pos();
        onSelectMe();
    }else if(event->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        QAction delme("删除当前布局",this);
        connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));
        QAction saveTemp("保存成控件",this);
        connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));

        contextMenu->addAction(&delme);
        contextMenu->addAction(&saveTemp);
        contextMenu->exec(mapToGlobal(event->pos()));
    }

}
void NewLayout::mouseMoveEvent(QMouseEvent *event)
{

    QString clsname = parentWidget()->parentWidget()->metaObject()->className();
   //  qDebug() << " parent class name " <<  clsname;
     // 这里主要防止它在列表的拖动事件.
    if(CN_NEWLAYOUT.compare(clsname) && CN_NEWLAYER.compare(clsname) ) return;
    if (event->buttons() & Qt::LeftButton)
    {
        move(this->pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        mWindow->posWidget->updatePosition(this->pos());
        this->blockSignals(true);
    }
}

void NewLayout::mouseReleaseEvent(QMouseEvent *event)
{
    /* 放开鼠标时检查它的是否出了边界要 */
    QWidget *p = this->parentWidget();
    QPoint pos = this->pos();
    if(this->x() < 0)
    {
        pos.setX(0 );
        this->move(pos);
    }
    if(this->y() < 0 )
    {
        pos.setY(0 );
        this->move(pos);
    }

    QSize ms = p->size();
    //左出界检查
    if((this->x()  + this->size().width()) > ms.width())
    {
        pos.setX( ms.width() - this->size().width()  );
        this->move(pos);

    }

    //上出界检查
    if((this->y() + this->size().height()) > ms.height())
    {
        pos.setY(ms.height() - this->size().height() );
        this->move(pos);
    }
    // 这里只能在释放鼠标时改变左边的控件值
    mWindow->posWidget->updateSize(this->size());
}

void NewLayout::onDeleteMe()
{
    //这里不能绕过它的父控件.
    //delMySelf();
   // mWindow->cManager->activeSS()->delSelectedLayout();


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

        DeleteMe();
    }


   // DeleteMe();
}

void NewLayout::onBeComeTemplateWidget()
{

   QString fpath =  QFileDialog::getSaveFileName(this,"保存成自定义控件,不要保存到目录,否则无法加载它.",QDir::currentPath() + "/widgets",
            "Json files (*.json);;All files (*.*)",
                new QString("Json files (*.json)"),QFileDialog::ShowDirsOnly|QFileDialog::ReadOnly);
   qDebug() << " save file " << fpath <<" has json "  << fpath.lastIndexOf("json");
   if(fpath.isEmpty())
        return;
   if(fpath.lastIndexOf("json") == -1)
       fpath.append(".json");

    QJsonObject json ;

     QJsonObject wid;
    writeToJson(wid);

    QJsonArray property = wid[PROPERTY].toArray();
    for (int i =0 ;i < property.size();i++) {
        QJsonObject tt = property.at(i).toObject();
        if(tt.contains(KEY_RECT))
        {
            QJsonObject rect = tt[KEY_RECT].toObject();
            QVariantMap vmap ;
            vmap["x"] = QString::number(0);
            vmap["y"] = QString::number(0);
            vmap[WIDTH] =rect[WIDTH].toString();
            vmap[HEIGHT] = rect[HEIGHT].toString();
            tt[KEY_RECT] = QJsonObject::fromVariantMap(vmap);

            property[i] = tt;
            break;
        }


    }
    QJsonArray arr ;

    wid[PROPERTY] = property;
    arr.append(wid);
    json[COMPOENTS] = arr;
   // arr.append(json);

  //  json[COMPOENTS] = arr;

    QFile saveFile;
    saveFile.setFileName(fpath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    }
    QJsonDocument jsonDoc(json);
    saveFile.write(jsonDoc.toJson());
    // 加载到当前工程里面去.
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
    QWidget *w = childlist.at(index);
    childlist.removeAt(index);
     ((NewFrame*)w)->delMySelf();
}

void NewLayout::deleteObject(QWidget *w)
{
    int i = childlist.indexOf((BaseForm*)w);
    childlist.removeAt(i);
    QString clsname = w->metaObject()->className();
    if(!clsname.compare(CN_NEWLAYOUT))
    {
        ((NewLayout*)w)->DeleteMe();
    }else {
        ((NewFrame*)w)->delMySelf();
    }
}


void NewLayout::writeToJson(QJsonObject &json)
{

    QJsonArray layoutarr;
    //  qDebug() << "NewLayout  parent object " << &json;

    foreach (QWidget *w, childlist) {
        QJsonObject outjson;
        outjson[NAME] = w->objectName();
        qDebug() << " childlist  " << w->metaObject()->className();
        // 这里只有两种对像类型,NewLayout , NewFrame;
        QString clsname = w->metaObject()->className();

        if(!clsname.compare(CN_NEWLAYOUT))
        {
            ((NewLayout*)w)->writeToJson(outjson);
        }else if(!clsname.compare(CN_NEWLIST))
        {
            ((NewList*)w)->writeToJson(outjson);
        }
        else{
            ((NewFrame*)w)->writeToJson(outjson);
        }

        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LAYOUT] = layoutarr;
    json[CLASS] = this->metaObject()->className();
   // json[CAPTION] = this->property(DKEY_LOCALSEQ).toString();
    json[CAPTION] = this->property(DKEY_TXT).toString();
   // json[NAME] = this->objectName();
    QJsonArray projson;
    projson.append(Compoent::getRectJson(this));
    json[PROPERTY] = projson;
}


void NewLayout::readFromJson(const QJsonObject &valobj)
{
     QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray());
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = valobj[CLASS].toString();
    QString caption = valobj[CAPTION].toString();
    qDebug() << " read from json caption is " << caption;
    QVariant variant = valobj[PROPERTY].toVariant();
    if(!clsName.compare(CN_NEWLAYOUT) || !clsName.compare(CN_LAYOUT))
    {
        NewLayout *newlayout = new NewLayout(oldrect.size(),mWindow,m_frame);
        newlayout->setProperty(DKEY_TXT,caption);
        if(valobj.contains(PROPERTY))
        {
            // QJsonObject vp =QJsonObject::fromVariantMap(qvm[PROPERTY]);

            newlayout->copyProperty(variant);
            newlayout->setProperty(DKEY_DYN,variant);
        }

     //   newlayout->setProperty(DKEY_LOCALSEQ,caption);
        newlayout->setGeometry(oldrect);
        newlayout->addMainWindow(mWindow);
        childlist.append(newlayout);
        mWindow->tree->addObjectToCurrentItem(newlayout);
        //   LayoutList.append(nl);
        newlayout->onSelectMe();

        // 这里一定是Layout 嵌套了.
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            newlayout->readFromJson(item.toObject());
        }
    }else if(!clsName.compare(CN_NEWLIST))
    {
      NewList *nlist = new NewList(oldrect.size(),m_frame);
      nlist->setProperty(DKEY_TXT,caption);
   //   nlist->setProperty(DKEY_LOCALSEQ,caption);
      if(valobj.contains(PROPERTY))
      {
          nlist->copyProperty(variant);
          nlist->setProperty(DKEY_DYN,variant);
      }
      nlist->setGeometry(oldrect);
      mWindow->tree->addObjectToCurrentItem(nlist);
      childlist.append(nlist);
      nlist->onSelectMe();
      nlist->show();
      foreach (QJsonValue item, valobj[LISTWIDGET].toArray()) {
          nlist->readFromJson(item.toObject());
      }
    }
    else if(!clsName.compare(CN_NEWFRAME)
             || !clsName.compare(QFRAME))

    {
      //  mWindow->ComCtrl->createLayoutFromJson(valobj,this);
      //  this->createNewFrameObject(valobj);
        this->createObjectFromJson(valobj);
    }

}


//按照JSON创建一个控件.
//void NewLayout::createNewFrameObject(const QJsonObject &json)
//{

//    NewFrame* ww = (NewFrame *)createObjectFromJson(json,m_frame);
//    //QString objname = json[NAME].toString();
//   // QString txt = json[CAPTION].toString();
//   // ww->setObjectName(objname);
//  //  qDebug() << "create object on " << this->property(DKEY_LOCALSEQ).toString();

//    //  这里要树上的所有控件顺序全局编号.
////    int n = mWindow->ComCtrl->ProMap.size();
////    ww->setProperty(DKEY_LOCALSEQ,
////        QString("%1_%2").arg(txt,QString::number(n)));
//    childlist.append(ww);

//    // 找出图层,把新建的控件添加进去.
//    mWindow->tree->addObjectToCurrentItem(ww);
//    ww->show();
//    ww->onSelectMe();
//}

QWidget* NewLayout::createObjectFromJson(const QJsonObject &json)
{
    NewFrame *newFrame = new NewFrame(m_frame);
    QVariant variant = json.value(PROPERTY).toVariant();
    if(json.contains(PROPERTY))
    {
        // QJsonObject vp =QJsonObject::fromVariantMap(qvm[PROPERTY]);

        newFrame->copyProperty(variant);
        newFrame->setProperty(DKEY_DYN,variant);
    }
    QRect rect = Compoent::getRectFromStruct(json[PROPERTY].toArray());

 //   newlayout->setProperty(DKEY_LOCALSEQ,caption);
    newFrame->setProperty(DKEY_TXT,json[CAPTION].toString());
    newFrame->setGeometry(rect);
    //newFrame->addMainWindow(mWindow);
    childlist.append(newFrame);
    mWindow->tree->addObjectToCurrentItem(newFrame);

    foreach (QJsonValue item, json[WIDGET].toArray()) {
        newFrame->readFromJson(item.toObject());
    }
    newFrame->show();
    newFrame->onSelectMe();

}


//QWidget* NewLayout::createObjectFromJson(const QJsonObject &json,QWidget *pobj)
//{
//    QWidget *nobj;
//    QVariant property;
//    for(QJsonObject::const_iterator it = json.constBegin();it != json.constEnd();++it)
//    {
//        QString key = it.key();
//        switch (it.value().type()) {
//        case QJsonValue::String:
//            if(!key.compare(CLASS))
//            {
//                QString cval = it.value().toString();

//                if(!cval.compare(QFRAME) || !cval.compare(CN_NEWFRAME))
//                {
//                    //创建父控件
//                    NewFrame  *n = new NewFrame(pobj);
//                    //  n->addMainWindow(pobj->parent());
//                    if(json.contains(PROPERTY))
//                    {
//                        n->copyProperty(json[PROPERTY]);
//                    }
//                    nobj =(QWidget*)(n);
//                  //  nobj->setProperty(DKEY_CLSNAME,cval);
//                }
//                else if(!cval.compare(QLABEL) || !cval.compare(CN_NEWLABEL))
//                {
//                    NewFrame *np = (NewFrame*)pobj;
//                    //nobj =(QWidget*)(new NewLabel(pobj));
//                    nobj =(QWidget*)(new NewLabel(np->m_frame));
//                    if(json.contains(PROPERTY))
//                    {

//                        ((NewLabel*)nobj)->copyProperty(json[PROPERTY]);
//                        // qDebug() << " NewLabel dyn property " << ((NewLabel*)nobj)->dynValues;
//                    }

//                }
//                 else if(!cval.compare(CN_NEWLIST))
//                {
//                    // 尝试创建列表控件.
//                  //  qDebug() << " create new list parent class name" << pobj->metaObject()->className();
//                    NewList *n = new NewList(pobj);
//                    n->resize(pobj->sizeHint());
//                    n->mWindow = mWindow;
//                    if(json.contains(PROPERTY))
//                    {
//                        n->copyProperty(json[PROPERTY]);
//                    }
//                    nobj = (QWidget*)n;
//                }
//            }
//            else if(!key.compare(NAME))
//            {
//                nobj->setObjectName(it.value().toString());
//            }
//            else if(!key.compare(CAPTION)) /* 界面显示的名称 */
//            {
//                nobj->setProperty(DKEY_CAPTION,it.value().toString());
//            }
//            break;
//        case QJsonValue::Array:
//            if(!key.compare(PROPERTY))
//            {
//                // nobj->setProperty("dynProperty",it.value());
//                property = it.value().toVariant();
//            }
//            else if(!key.compare(WIDGET)) // 处理它的子控件.
//            {
//                foreach(QJsonValue val, it.value().toArray())
//                {
//                    // qDebug() << qv.type();
//                    if(val.type() == QJsonValue::Object)
//                    {
//                        createObjectFromJson(val.toObject(),nobj);
//                       // chlist.append(cobj->objectName());
//                    }

//                }
//            }
//            break;
//        default:
//            break;
//        }

//    }

//    // 下面只处理控件的坐标与控件中的图片.

//    nobj->setProperty(DKEY_DYN,property);
//    QJsonArray array ;
//    QString clsName = nobj->metaObject()->className();
//    bool isFrame = !clsName.compare(CN_NEWFRAME) ;// 二元判断,不是控件就是图片.
//    if(!clsName.compare(CN_NEWFRAME))
//    {
//        array = ((NewFrame*)nobj)->dynValues[DKEY_DYN].toArray();
//    }else if(!clsName.compare(CN_NEWLABEL))
//    {
//        array = ((NewLabel*)nobj)->dynValues[DKEY_DYN].toArray();
//    }else if(!clsName.compare(CN_NEWLIST))
//    {
//        array = ((NewList*)nobj)->dynValues[DKEY_DYN].toArray();
//    }
//    parseJsonProperty(nobj,array); // 读取坐标与图片属性.
//    return nobj;
//}


//void NewLayout::parseJsonProperty(QWidget *nobj, const QJsonArray &array)
//{
//    QString clsName = nobj->metaObject()->className();
//    bool isFrame = !clsName.compare(CN_NEWFRAME) ;// 二元判断,不是控件就是图片.

//    foreach(QJsonValue val, array)
//    {
////         qDebug()  << " Property Val List :" << val.type()
////                   << " json value " << val;
//        switch (val.type()) {
//        case QJsonValue::String:
//            break;
//        case QJsonValue::Array:

//            break;
//        case QJsonValue::Object:
//        {
//            QJsonObject obj = val.toObject();
//            if(obj.contains(IMAGE))
//            {
//                QPixmap p;
//                QString path = obj[IMAGE].toString();
//                if(path.contains("\\"))
//                {
//                    path.replace("\\","/");
//                }
//                p.load(path);
//                ((NewLabel *)nobj)->setPixmap(p);
//              //  ((NewLabel *)nobj)->setFixedSize(p.size());
//            }
//            else if(obj.contains(STRUCT))
//            {
//                // 处理结构体里面的坐标.
//                parseJsonProperty(nobj,obj[STRUCT].toArray());
//            }
//            else if(obj.contains(KEY_RECT))
//            {
//                //  QString clsName = nobj->property(DKEY_CLSNAME).toString();
//                QVariantMap rect = obj[KEY_RECT].toObject().toVariantMap();
//                QRect r = QRect(rect[LX].toString().toInt(),
//                        rect[LY].toString().toInt(),
//                        rect[WIDTH].toString().toInt(),
//                        rect[HEIGHT].toString().toInt());
//               // QRect r = Compoent::getRectFromStruct(obj[PROPERTY].toArray());
//                if(!clsName.compare(CN_NEWFRAME))
//                {
//                    qobject_cast<NewFrame *>(nobj)->setGeometry(r);
//                    ((NewFrame*)nobj)->m_frame->setGeometry(r);
//                    ((NewFrame*)nobj)->updateGeometry();

//                    // 外面８个点的实际位置.
//                   // ((NewFrame*)nobj)->setFixedSize(r.size()/*+MARGIN_SIZE*/);
//                    qDebug() << "after update geometry NewFrame size " << nobj->size();
//                    // ((NewFrame*)nobj)->setFixedSize(r.size());
//                }else if(!clsName.compare(CN_NEWLIST))
//                {
//                    ((NewList*)nobj)->setGeometry(r);
//                }
//                else if(!clsName.compare(CN_NEWLABEL))
//                {
//                    ((NewLabel *)nobj)->setGeometry(r);
//                    qDebug() << "after update geometry NewLabel size " << nobj->size();
//                }

//            }
//            else if(obj.contains(LIST))
//            {

//            }
//        }
//            break;
//        default:
//            break;
//        }
//    }

//}



NewLayer::NewLayer(QSize nsize, QWidget *parent)
    :BaseForm(parent)
    // mActiveIdx(-1)

{

    mWindow = ((ScenesScreen*)parent)->mWindow;
    setMinimumSize(nsize ); // 最小尺寸
    resize(nsize);
    setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.
    //this->setObjectName(this->metaObject()->className());
   // setStyleSheet("%s{border: 0.5px solid #929292;}" % LAYER);
    setFocusPolicy(Qt::ClickFocus);
     m_frame->setStyleSheet("background-color: #C0DCC0;");
    show();
    QString key =QString("%1_%2").arg("图层",QString::number( mWindow->ComCtrl->ProMap.size()));
    setProperty(DKEY_LOCALSEQ,key);
    QString n = QString("%1_%2").arg(metaObject()->className(),
                                     QString::number(mWindow->ComCtrl->ProMap.size()));
    setObjectName(n);
    setToolTip(key);
}

//void NewLayer::createNewLayout(QSize size)
//{
//    // 在些新建一个布局为当前的子控件.
//    createNewLayout(m_frame,size);
//}

void NewLayer::readFromJson(const QJsonObject &valobj)
{
    QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray());
   // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
   QString clsName = valobj[CLASS].toString();
   QString caption = valobj[CAPTION].toString();
   qDebug() << " read from json caption is " << caption;
   QVariant variant = valobj.value(PROPERTY).toVariant();
   if(!clsName.compare(CN_NEWLAYOUT) || !clsName.compare(CN_LAYOUT))
   {
       NewLayout *newlayout = new NewLayout(oldrect.size(),mWindow,m_frame);
       newlayout->setProperty(DKEY_TXT,caption);
       if(valobj.contains(PROPERTY))
       {
           // QJsonObject vp =QJsonObject::fromVariantMap(qvm[PROPERTY]);

           newlayout->copyProperty(variant);
           newlayout->setProperty(DKEY_DYN,variant);
       }

    //   newlayout->setProperty(DKEY_LOCALSEQ,caption);
       newlayout->setGeometry(oldrect);
    //   newlayout->addMainWindow(mWindow);
     //  childlist.append(newlayout);
       mWindow->tree->addObjectToCurrentItem(newlayout);
       childlist.append(newlayout);
       newlayout->onSelectMe();

       // 这里一定是Layout 嵌套了.
       foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
           newlayout->readFromJson(item.toObject());
       }
   }


}

//void NewLayer::onSelectMe()
//{

//    //clearOtherObjectStyleSheet();
//    mWindow->cManager->activeSS()->setSelectObject(this);
//    mWindow->posWidget->setConnectNewQWidget(this);
//    mWindow->propertyWidget->createPropertyBox(this);
//    mWindow->imgPropertyWidget->delPropertyBox();
//    this->blockSignals(true);
//}



//void NewLayer::DeleteMe()
//{
//     mWindow->cManager->activeSS()->delSelectedLayer();
//}

void NewLayer::onDeleteMe()
{


    //删除布局控件
    QMessageBox msgBox;
    msgBox.setWindowTitle("删除提示");
    QString msg = QString("你真的要删除当前%1吗?删除之后不可以撤消,请选择<删除>删除.").arg(this->property(DKEY_TXT).toString());
    msgBox.setText(msg);
    // msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Yes,"删除");
    msgBox.setButtonText(QMessageBox::Cancel,"取消");
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    //qDebug() << " QMessageBox result " << ret;
    if(ret == QMessageBox::Yes)
    {

        DeleteMe();
    }



}


void NewLayer::addChildrenToTree()
{
    foreach (QWidget *w, childlist) {
        mWindow->tree->addObjectToCurrentItem(w);
        ((NewLayout*)w)->addChildrenToTree();
    }
}

void NewLayer::writeToJson(QJsonObject &json)
{
    QJsonArray layoutarr;
    foreach (QWidget *w, childlist) {
        QJsonObject layoutObj;
       // qDebug() << "ScenesScreen  sub object " << &layoutObj;
        layoutObj[NAME] = w->objectName();
        ((NewLayout*)w)->writeToJson(layoutObj);
       // qDebug() << " LayoutObj " << layoutObj;
        layoutarr.append(layoutObj);
    }
    //json[CAPTION] = this->property(DKEY_LOCALSEQ).toString();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[CLASS] = this->metaObject()->className();
    //json[NAME] = this->objectName();
    json[LAYOUT] = layoutarr;
    QJsonArray proterty;
    proterty.append(Compoent::getRectJson(this));
    json[PROPERTY] = proterty;
}

void NewLayer::resizeEvent(QResizeEvent *event)
{
    // 这里要重写它的基类事件.
    updateGeometry();
    QWidget::resizeEvent(event);
    foreach (QWidget* nl, childlist) {
        nl->setMaximumSize(this->size());
    }
}


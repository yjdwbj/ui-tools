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
#include <QDialogButtonBox>

#include "core_def.h"
#include "scenesscreen.h"
#include "canvasmanager.h"

QJsonValue Compoent::changeJsonValue(const QJsonArray &arg,QString key,
                               const QVariant &val)
{
    static bool isChanged = false;

    QJsonArray parr = arg;
    int asize = parr.size();
    int i = 0;
    //for(int i = 0;i < asize;i++)
    foreach (QJsonValue item, parr)
    {
        QJsonObject obj = item.toObject();
        if(item.isArray())
        {
             QJsonArray tarray = item.toArray();
            //  return
             parr.replace(i,changeJsonValue(tarray,key,val));
//             if(isChanged)
//                 return parr;
//             else
//                 continue;

             //return parr;
             //break;
        }else if(obj.contains(STRUCT))
        {
            QJsonArray arr = obj[STRUCT].toArray();
           // changeJsonValue(arr,key,val);
            obj[STRUCT] = changeJsonValue(arr,key,val);
            parr[i] = obj;
//            if(isChanged)
//                return parr;
//            else
//                continue;
            //return parr;
            //break;
        } else
        // 这里必需要有-type 这个属性名.
        if(obj[WTYPE].toString().contains(key) )
        {
            if(obj.contains(LIST) && obj.contains(DEFAULT))
            {

                QJsonValue v =  QJsonValue::fromVariant(val);
                if(v.isArray())
                {
                    obj[LIST] = v;
                  //  isChanged = true;
                }else
                {
                    obj[DEFAULT] = v;
                 //   isChanged = true;

                }
            }else if(obj.contains(DEFAULT))
            {
                obj[DEFAULT] = QJsonValue::fromVariant(val);
                //isChanged = true;

            }

            else if(obj.contains(UID))
            {
                 obj[ENAME] =  QJsonValue::fromVariant(val);
              //   isChanged = true;

            }

            /*else if(obj.contains(ENAME))
            {
                obj[ENAME] = QJsonValue::fromVariant(val);
            }*/
            else if(obj.contains(IMAGE))
            {
                obj[IMAGE] = QJsonValue::fromVariant(val);
                //isChanged = true;

//                parr[i] = obj;
//                break;
            }
            else if(obj.contains(GRAYCOLOR)){

                obj[GRAYCOLOR] = QJsonValue::fromVariant(val);
               // isChanged = true;
            }else if(obj.contains(COLOR))
            {
               obj[COLOR] = QJsonValue::fromVariant(val);
               //isChanged = true;
            }else if(obj.contains(BAKCOLOR))
            {
               obj[BAKCOLOR] = QJsonValue::fromVariant(val);
              // isChanged = true;
            }
            else if(obj.contains(BAKIMAGE))
            {
                obj[BAKIMAGE] = QJsonValue::fromVariant(val);
               // isChanged = true;
            }
            else if(obj.contains(BORDER))
            {
               obj[BORDER] = QJsonValue::fromVariant(val);
              // isChanged = true;
            }else if(obj.contains(KEY_RECT))
            {

            }
            parr.replace(i,obj);
           // isChanged = true;
            break;
        }
        i++;
    }
    return parr;
}

void Compoent::changeJsonValue(QString key, QVariant val)
{
   QJsonArray parr = dynValues[DKEY_DYN].toArray();
  // changeJsonValue(parr,key,val);
   // int asize = parr.size();

    // 这个QJsonObject 都是只读的,修改之后必需重新赋值.
   // dynValues[DKEY_DYN] = parr;

    dynValues[DKEY_DYN] = changeJsonValue(parr,key,val);
}

QJsonArray Compoent::updateRBJsonValue(const QJsonArray &arr, QWidget *w)
{
    QJsonArray projson = arr;

    int psize =projson.size();
    for(int i =0; i < psize;i++)
    {
        QJsonValue item = projson.at(i);
        if(item.isArray())
        {
             projson.replace(i,updateRBJsonValue(item.toArray(),w));
             return projson;
            continue;
        }

//        QJsonObject ov = projson.at(i).toObject();
        QJsonObject ov = item.toObject();

        if(ov.contains(STRUCT))
        {
            QJsonArray array =  ov[STRUCT].toArray();
            ov[STRUCT] = updateRBJsonValue( array,w);

        }else if(ov.contains(KEY_RECT))
        {
            //ov = getRectJson(this);
            ov[KEY_RECT] = getRectJson(w)[KEY_RECT];

        }else if(ov.contains(UID))
        {
          //  ov[UID] = w->property(DKEY_UID).toString();
             ov[UID] = w->property(DKEY_UID).toInt();
        }
        else if(ov.contains(BORDER))
        {
            ov[BORDER] = getBorderJson(w)[BORDER];
        }
        projson[i] = ov;

    }
     return projson;
}

QVariant Compoent::getJsonValue(const QJsonArray &parr,QString key)
{
    int asize = parr.size();
   // for(int i = 0;i < asize;i++)
    foreach (QJsonValue val, parr)
    {
        if(val.isArray())
        {
            QVariant ret = getJsonValue(val.toArray(),key);
            if(ret.isValid())
                return ret;
            else
                continue;
        }


        QJsonObject obj = val.toObject();
        if(obj.contains(STRUCT))
        {
            QVariant ret = getJsonValue(obj[STRUCT].toArray(),key);
            if(ret.isValid())
                return ret;
            else
                continue;
        }else if(obj[WTYPE].toString().contains(key))
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
    return QVariant();
}

QVariant Compoent::getJsonValue(QString key) const
{
    return getJsonValue(dynValues[DKEY_DYN].toArray(),key);
}


void Compoent::onBindValue(QWidget *w,const QVariantMap &map)
{
 //   qDebug() << " dyn size " << dynValues.size();
     // 通过关键字来设置控件的默认值.
    if(!w)
        return;
     QString n = w->metaObject()->className();
     QString uname = w->objectName();
     if(!n.compare("QLineEdit"))
     {
         QLineEdit *txt = (QLineEdit *)w;

         txt->setText(getJsonValue(uname).toString());
     }else if(!n.compare("QComboBox"))
     {
         QComboBox *cb = (QComboBox *)w;
         if(!uname.compare(PIC_TEXT)) return;
         QVariant nlv =  cb->property(DKEY_IMAGELST);
         if(nlv.isValid() )
         {
             // 处理图片列表与它的默认值.
             cb->clear();
             QVariantList nlist = nlv.toList();
             QString defimg = cb->property(DKEY_IMGIDX).toString();
             for(QVariantList::const_iterator it = nlist.begin();
                     it != nlist.end();++it)
             {
                 // example for key  is  "config/images/string/alarm_pol.bmp"
                 QString key = (*it).toString().replace('\\',QDir::separator());

                 cb->addItem(QIcon(key.section(SECTION_CHAR,1,1)),key.section(SECTION_CHAR,0,0));
             }
             cb->setCurrentText(defimg);
             cb->setProperty(DKEY_IMGIDX,defimg);
           }
            else
         {
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
               c = QColor(getJsonValue(BAKCOLOR).toString()) ;
          }
          else if(!uname.compare(BORDER))
          {
              //unsigned int n = getJsonValue(BORDER).toUInt();
              //灰度转成ＲＧＢ
             // QColor c(n & 0xfff00000,n &0xfff00,n & 0xff);
              //QColor c = QColor::fromRgb(n);
              // 这里暂时有RGB颜色测试
              c = QColor(getJsonValue(BORDER).toString());

          }else if(!uname.compare(BAKIMAGE))
          {
              QPixmap p(12,12);
              QString img = getJsonValue(BAKIMAGE).toString();
              p.load(img);
              ((QPushButton*)w)->setIcon(p);
              return;

          }else if(!uname.compare(PIC_TEXT))
          {


              return;
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



QJsonObject Compoent::getValueFromProperty(const QJsonArray &arr, const QString &key)
{
    QJsonObject ret;
    foreach (QJsonValue pval, arr) {
        if(pval.isArray())
        {
            return getValueFromProperty(pval.toArray(),key);
        }
        QJsonObject pobj = pval.toObject();
        if(pobj.contains(key))
        {
            return pobj;
        }
        else if(pobj.contains(STRUCT)) {
            return getValueFromProperty(pobj[STRUCT].toArray(),key);
        }
    }
    return  ret;
}

QRect Compoent::getRectFromStruct(const QJsonArray &arr,QString key)
{
    QJsonObject rectobj  = getValueFromProperty(arr,key);
    QRect rect(0,0,0,0);
    if(!rectobj.isEmpty())
    {
        QVariantMap obj =  rectobj[key].toObject().toVariantMap();
        if(!key.compare(BORDER))
        {
            rect.setLeft(obj[LEFT].toString().toInt());
            rect.setBottom(obj[BOTTOM].toString().toInt());
            rect.setRight(obj[RIGHT].toString().toInt());
            rect.setTop(obj[TOP].toString().toInt());
        }else if(!key.compare(KEY_RECT)) {
            rect.setX(obj[LX].toString().toInt());
            rect.setY(obj[LY].toString().toInt());
            rect.setWidth(obj[WIDTH].toString().toInt());
            rect.setHeight(obj[HEIGHT].toString().toInt());
        }

    }
    return rect;
}

QJsonObject Compoent::getBorderJson(QWidget *w)
{
    QJsonObject border;
    QVariantMap vmap ;
    BaseForm *fr = (BaseForm*)w;
    vmap[LEFT] = fr->mBorder.left();
    vmap[TOP] = fr->mBorder.top();
    vmap[RIGHT] = fr->mBorder.right();
    vmap[BOTTOM] = fr->mBorder.bottom();
    border[BORDER] = QJsonObject::fromVariantMap(vmap);
    return border;
}


void Compoent::copyProperty(const QVariant &va)
{
    QJsonObject oo = QJsonValue::fromVariant(va).toObject();
    dynValues[DKEY_DYN] = QJsonValue::fromVariant(va);
}


//void NewLabel::onEnumItemChanged(QString txt)
//{
//   QComboBox *cb =(QComboBox *)(QObject::sender());
//   changeJsonValue(cb->objectName(),txt);
//}

//void NewLabel::onNumberChanged(int num)
//{

//    QSpinBox *sp = (QSpinBox *)(QObject::sender());
//   // dynValues[sp->objectName()] = num;
//    changeJsonValue(sp->objectName(),num);
//}

//void NewLabel::onTextChanged(QString str)
//{
//    QLineEdit *txt = (QLineEdit *)(QObject::sender());
//  //  dynValues[txt->objectName()] = str;
//    changeJsonValue(txt->objectName(),str);
//}

///*------------------------------------------------------------------------*/



//NewLabel::NewLabel(QWidget *parent)
//    :QLabel(parent),
//     selIndex(0),disDefaultList(false)
//{
//    mWindow = ((NewFrame*)(parent->parentWidget()))->mWindow;
//   // mWindow = ((NewFrame*)parent)->mWindow;
//    this->setLineWidth(0);
//    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
//}


//void NewLabel::addPropertyBoxSignal(QSpinBox *b)
//{
//    connect(b,SIGNAL(valueChanged(int)),this,SLOT(onXYWHChangedValue(int)));
//}

//void NewLabel::onXYWHChangedValue(int v)
//{
//    /* 绑定坐标控件的更新 */
//    QWidget *sender =(QWidget *)(QObject::sender());

//    QWidget *p = this->parentWidget();

//  //  p->move(p->parentWidget()->mapFromGlobal(QCursor::pos()-mOffset));
//    if(!sender->objectName().compare(X))
//    {
//        //o.setX(v);
//        QPoint pos = p->pos();
//        pos.setX(v);
//        p->move(pos);

//    }else if(!sender->objectName().compare(Y))
//    {
//        QPoint pos = p->pos();
//        pos.setY(v);
//        p->move(pos );

//    }
//}


//void NewLabel::removeWidFromLayout(QLayout* layout)
//{

//    if(!layout)
//        return;
//    QLayoutItem* child;
//    while(layout->count()!=0)
//    {
//        child = layout->takeAt(0);
//        if(child->layout() != 0)
//        {
//            removeWidFromLayout(child->layout());
//        }
//        else if(child->widget() != 0)
//        {
//            delete child->widget();
//        }

//        delete child;
//    }
//}


//void NewLabel::clearOtherObjectStyleSheet()
//{
//    /* 清除控件的红线框 */
//    QList<NewLabel*> nflist =  this->parentWidget()->findChildren<NewLabel*>();
//    foreach(NewLabel *x,nflist)
//    {
//           x->setStyleSheet("");

//    }
//}


//QWidget *NewLabel::getQWidgetByName(QString name) const
//{
//    QWidgetList tlist = qApp->topLevelWidgets();

//    for(QWidgetList::iterator wit = tlist.begin();wit != tlist.end();++wit)
//    {
//       // qDebug() << "find object by name :" << (*wit)->objectName();
//        if((*wit)->objectName() == name)
//        {
//            return *wit;
//            break;
//        }
//    }
//    return (QWidget*)0;
//}

//void NewLabel::mousePressEvent(QMouseEvent *ev)
//{

//    /* 单击选中它的父对像 */

//    //NewFrame *p =(NewFrame*) (this->parentWidget());
//    // 下面如果它的父控件是NewFrame->m_frame
//    NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
//    if(ev->button() == Qt::RightButton)
//    {
//        QMenu *contextMenu = new QMenu(this);
//        QAction delme(QString("删除当前-%1").arg(p->property(DKEY_TXT).toString()),this);
//        connect(&delme,SIGNAL(triggered(bool)),p,SLOT(onDeleteMe()));
//        contextMenu->addAction(&delme);
//        contextMenu->exec(mapToGlobal(ev->pos()));
//    }else
//    {
//        // p->setState(SelectionHandleActive);
//         mWindow->cManager->activeSS()->setSelectObject(p);

//       //  p->setStyleSheet("NewFrame{border: 0.5px solid red;}");

//         clearOtherObjectStyleSheet();
//         mWindow->posWidget->setConnectNewQWidget(p);
//         mWindow->propertyWidget->createPropertyBox(p);
//      //   mWindow->imgPropertyWidget->delPropertyBox();
//    }
//    mOffset = ev->pos();
//    setCursor(Qt::ClosedHandCursor);
//}


//void NewLabel::mouseReleaseEvent(QMouseEvent *ev)
//{

//    /* 放开鼠标时检查它的是否出了边界要 */
//    NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
//    //NewFrame *p =(NewFrame*) (this->parentWidget());
//    QPoint pos = p->pos();
//    if(p->x() < 0)
//    {
//        pos.setX(0);
//        p->move(pos);

//    }
//    if(p->y() < 0 )
//    {
//        pos.setY(0);
//        p->move(pos);

//    }

//    QSize ms = p->parentWidget()->parentWidget()->size();
//    if((p->x() + p->size().width()) > ms.width())
//    {
//        pos.setX( ms.width() - p->size().width() /*- MARGIN_SIZE.width() /2*/ );
//        p->move(pos);
//        qDebug() << " move pos " << pos;
//    }

//    if((p->y() + p->size().height()) > ms.height())
//    {
//        pos.setY(ms.height() - p->size().height() /*- MARGIN_SIZE.height() /2*/);
//        p->move(pos);
//    }
//}

//void NewLabel::mouseMoveEvent(QMouseEvent *event)
//{

//    if (event->buttons() & Qt::LeftButton)
//    {
//        //NewFrame *p =(NewFrame*)(this->parentWidget());
//        NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());
//        p->move( p->pos() + (event->pos() - mOffset));
//        /* 把新的位置更新到右边属性框 */
//        mWindow->posWidget->updatePosition(p->pos());
//        p->blockSignals(true);
//    }

//}

//void NewLabel::mouseDoubleClickEvent(QMouseEvent *event)
//{

//   // NewFrame *p = (NewFrame *)this->parentWidget();
//    NewFrame *p = (NewFrame *)this->parentWidget()->parentWidget(); // 如果父控件是NewFrame->m_frame
//    p->setState(SelectionHandleOff);


//    clearOtherObjectStyleSheet();
//   // p->setStyleSheet("");
//    QList<NewLabel*> nlist =  p->findChildren<NewLabel*>();
//    foreach (NewLabel *n, nlist) {
//        if(n != this)
//            n->setStyleSheet("");
//    }

//    setStyleSheet("QLabel{border: 1px solid red;border-style: outset;}");
//    mWindow->propertyWidget->createPropertyBox(p);
//   // mWindow->imgPropertyWidget->createPropertyBox(this);
//}


//void NewLabel::onListImageChanged(QString img)
//{
//   foreach (QString s, myImageList) {
//       QString k = s.section(":",0,0);
//       if(!k.compare(img))
//       {
//           QString fpath = s.section(":",1,1);
//           this->setPixmap(QPixmap(fpath));/* 更新图片 */
//           int idx = QDir::currentPath().length() + 1;
//           this->changeJsonValue(IMAGE,fpath.mid(idx));
//           selIndex = myImageList.indexOf(s);
//           break;
//       }
//   }

//}

//void NewLabel::updatePixmap(QString imgpath)
//{
//    this->setPixmap(QPixmap(imgpath));
//}




BaseForm::BaseForm(QWidget *parent)
    :FormResizer(parent), mBorderColor("#FFFFFF"),
      mBorder(0,0,0,0)
{
    setStyleSheet("");
    mbkImage = "";

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
    onSelectMe();
    if (event->button() == Qt::LeftButton)
    {
        mOffset = event->pos();

    }else if(event->button() == Qt::RightButton)
    {
        QString clsname = metaObject()->className();
        QMenu *contextMenu = new QMenu(this);
        QAction delme(QIcon(":/icon/icons/removesubmitfield.png")  ,
                      QString("删除当前-%1").arg(this->property(DKEY_TXT).toString()),this);
        connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));
        contextMenu->addAction(&delme);
         QAction saveTemp(QIcon(":/icon/icons/build.png"),"保存成控件",this);
        if(!CN_NEWLAYOUT.compare(clsname))
        {
            contextMenu->addAction(&saveTemp);
            connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));
        }
        QAction hideobj(QIcon(":/icon/icons/hide-30x30.png"),"隐藏",this);
        if(!CN_NEWFRAME.compare(clsname))
        {

        }else
        {
            connect(&hideobj,SIGNAL(triggered(bool)),SLOT(onSwapViewObject(bool)));
            contextMenu->addAction(&hideobj);
        }

        QWidget *parentWidget = this->parentWidget();
        QString pobjname = parentWidget->objectName();

        if(!pobjname.compare(LISTWIDGET) || !clsname.compare(CN_NEWLIST) )
        {
            NewList *nl;
            if(!clsname.compare(CN_NEWLIST))
                nl = (NewList*)this;
            else
                nl =(NewList*)(((NewLayout*)this)->parentList);
            contextMenu->addAction(nl->menuAddLine);
            contextMenu->addAction(nl->menuSetHeight);
            contextMenu->addAction(nl->menuSetSpace);
        }

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
    moveNewPos(pos);
//    if(this->x() < 0)
//    {
//        pos.setX(0 /*- MARGIN_SIZE.width() /2*/);
//        this->move(pos);

//    }
//    if(this->y() < 0 )
//    {
//        pos.setY(0 /*-MARGIN_SIZE.width() /2*/);
//        this->move(pos);
//    }

//    QSize ms = p->size();
//    //左出界检查
//    if((this->x()  + this->size().width()) > ms.width())
//    {
//        pos.setX( ms.width() - this->size().width() /*+ MARGIN_SIZE.width() /2*/ );
//        this->move(pos);

//    }

//    //上出界检查
//    if((this->y() + this->size().height()) > ms.height())
//    {
//        pos.setY(ms.height() - this->size().height() /*+ MARGIN_SIZE.height() /2*/);
//        this->move(pos);
//    }

    // 这里只能在释放鼠标时改变左边的控件值
    mWindow->posWidget->updateSize(this->size());
}

void BaseForm::moveNewPos(int x,int y)
{
    moveNewPos(QPoint(x,y));
}

void BaseForm::moveNewPos(QPoint pos)
{
    QWidget *p = this->parentWidget();
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
    qDebug() << " postwidget has changed " << sender->objectName() << " value " << v;

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


void BaseForm::onTextSelected()
{
    QObject *obj  = QObject::sender();
    QPushButton *btn =  ((QPushButton*)obj);
    MenuItemDialog *md = new MenuItemDialog(btn->text(),mWindow);
    md->exec();
    changeJsonValue(obj->objectName(),md->text);
    btn->setText(md->text);
    md->deleteLater();
}


void BaseForm::addChildrenToTree()
{

}

//void BaseForm::onDeleteMe()
//{
//    DeleteMe();
//}

void BaseForm::onBorderChangedValue(int v)
{
    QString name = QObject::sender()->objectName();
    if(!name.compare(LEFT))
    {

        mBorder.setLeft(v);
    }else if(!name.compare(TOP))
    {

        mBorder.setTop(v);
    }else if(!name.compare(RIGHT))
    {

        mBorder.setRight(v);
    }else if(!name.compare(BOTTOM))
    {

        mBorder.setBottom(v);
    }

    updateStyleSheets();

//    QJsonValue qv = getBorderJson(this).value(BORDER);
//    changeJsonValue(BORDER,qv.toVariant());
  //  update();
    this->blockSignals(true);
}


void BaseForm::updateStyleSheets()
{
    QString str = QString("background-color: %6;" \
                          "border-style: outset; "\
                          "border-color: %5; "\
                          "border-left-width: %1px; "\
                          "border-top-width: %2px; " \
                          "border-right-width: %3px; "\
                          "border-bottom-width: %4px;").arg(QString::number(mBorder.left()),
                                                               QString::number(mBorder.top()),
                                                               QString::number(mBorder.right()),
                                                               QString::number(mBorder.bottom()),
                                                               mBorderColor,
                                                               mbkColor);
   // 没有背景图片这个参数时就显示背景色.
   if(!mbkImage.isEmpty())
   {
      str =  QString("background-image: url(%1); %2").arg(mbkImage,str);
   }

   setStyleSheet(QString("BaseForm#%1 { %2 }").arg(this->objectName(),str)); 
   update();
   repaint();
//   qDebug() << " object name " << objectName()
//             << " stylesheet " << this->styleSheet();
}

void BaseForm::paintEvent(QPaintEvent *ev)
{
    QStyleOption option;
    option.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

}

void BaseForm::resizeEvent(QResizeEvent *event)
{
    // 这里要重写它的基类事件.
    updateGeometry();
    QWidget::resizeEvent(event);
    foreach (QWidget* nl, childlist) {
        nl->setMaximumSize(this->size());
    }
}

void BaseForm::onEnumItemChanged(QString txt)
{
   QComboBox *cb =(QComboBox *)(QObject::sender());
 //  qDebug() << " Frame enum value changed " << txt << cb->currentText();
 //  dynValues[cb->objectName()] = txt;
   if(!cb->objectName().compare(PIC_TEXT))
   {
       changeJsonValue(cb->objectName(),mWindow->mOrderlist[cb->currentIndex()]);
   }else
       changeJsonValue(cb->objectName(),txt);


    this->signalsBlocked();
}

void BaseForm::onColorButtonClicked()
{
    QPushButton *btn = (QPushButton*)QObject::sender();
    QColor c = QColor::fromRgb(btn->property(DKEY_COLOR).toUInt());
    QColorDialog *color =new  QColorDialog(c);
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
            this->mbkColor = c.name(QColor::HexArgb);
           // setStyleSheet(QString("background-color: %1;").arg(c.name(QColor::HexRgb)));
            changeJsonValue(BAKCOLOR,c);

        }
        else if(!objname.compare(BORDER))
        {

            this->changeJsonValue(BORDER,qGray(c.rgb()));
            this->mBorderColor = c.name(QColor::HexArgb);
            changeJsonValue(BORDER,c);
        }
        this->updateStyleSheets();

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

//    qDebug() << " our object name " << objectName()
//           << " clss name " << metaObject()->className();


    mWindow->cManager->activeSS()->setSelectObject(this);
    mWindow->posWidget->setConnectNewQWidget(this);
    mWindow->propertyWidget->createPropertyBox(this);
  //  mWindow->imgPropertyWidget->delPropertyBox();
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
        ((BaseForm*)w)->DeleteMe();
//        if(!CN_NEWLABEL.compare(w->metaObject()->className()))
//        {
//            childlist.removeOne(w);
//            w->deleteLater();
//        }else{
//           ((BaseForm*)w)->DeleteMe();
//        }

    }

    mWindow->tree->setMyParentNode();  //选中它的父控件.
    mWindow->tree->deleteItem(this);
    mWindow->posWidget->resetValues();
    mWindow->ComCtrl->ProMap.remove(property(DKEY_LOCALSEQ).toString());
    mWindow->propertyWidget->delPropertyBox();
  //  mWindow->imgPropertyWidget->delPropertyBox();
    // 它是否是列表控件的一员.

    deleteLater();
}

void BaseForm::initJsonValue()
{
    mbkColor = Compoent::getJsonValue(BAKCOLOR).toString();
    mBorderColor = Compoent::getJsonValue(BORDER).toString();
    mBorder = Compoent::getRectFromStruct(dynValues[DKEY_DYN].toArray(),BORDER);
    setProperty(DKEY_UID,mWindow->ComCtrl->ProMap.size());
    //changeJsonValue(UID,mWindow->ComCtrl->ProMap.size());
}

void BaseForm::onBackgroundImageDialog()
{

    ImageListView *imgview = new ImageListView(QDir::currentPath(),this);
    imgview->setFixedSize(mWindow->size() * 0.6);
    imgview->exec();
    int ret = imgview->result() ;
}


void BaseForm::onSelectedBackgroundImage(QListWidgetItem *item)
{
    QWidget *sender = (QWidget*)(QObject::sender());
    QVariantMap vmap =  sender->property(DKEY_IMGMAP).toMap();
//    qDebug() << " image text is " << item->text()
//             << " objectname " << objectName();

    if(!vmap.isEmpty())
    {
        QString fpath = vmap[item->text()].toString();
        mbkImage = fpath;

        updateStyleSheets();

        changeJsonValue(property(DKEY_CURVAL).toString(),fpath);

    }

}

void BaseForm::onListImageChanged(QString img)
{
   QString objname = QObject::sender()->objectName();
   QVariantList imglist = this->property(DKEY_IMAGELST).toList();
   foreach (QVariant v, imglist) {
       QString s = v.toString();
       QString k = s.section(SECTION_CHAR,0,0);
       if(!k.compare(img))
       {
         //  QString fpath = s.section(":",1,1);
         //  this->setPixmap(QPixmap(fpath));/* 更新图片 */
        //   int idx = QDir::currentPath().length() + 1;
           //this->changeJsonValue(BAKIMAGE,fpath.mid(idx));
          // selIndex = imglist.indexOf(s);
           changeJsonValue(objname,s.section(SECTION_CHAR,1,1)); // 修改JSON里的值
           break;
       }
   }

}



void BaseForm::onPictureDialog(bool )
{
    QWidget *sparent =((QWidget*)QObject::sender())->parentWidget();

    QString key = QObject::sender()->objectName();
    QComboBox *cb = sparent->findChild<QComboBox*>(key);

    // QMessageBox::warning(this,"test","your clicked me: ");
    QVariantList imglist = this->property(DKEY_IMAGELST).toList();
    ImageFileDialog *ifd = new ImageFileDialog(imglist,this);

    ifd->exec();
    //qDebug() << " ImageFileDialog ";
    //selectedMap sMap  = ifd->getSelectedMap();
    imglist = ifd->getSelectedList();

  //  disDefaultList = imglist.size() ? true : false;
    ifd->deleteLater();
   // QJsonObject json;
    int rootlen  = QDir::currentPath().length()+1;
    QJsonArray qa;

    cb->clear();
    setProperty(DKEY_IMAGELST,imglist);
    foreach (QVariant v, imglist) {
        QString s = v.toString();
        // example for s   "alarm_du.bmp:/home/yjdwbj/build-ut-tools-Desktop_Qt_5_6_0_GCC_64bit-Debug/images/string/alarm_du.bmp
        // example for s   ""m104.bmp:config/images/string/m104.bmp"
        QString lastsection = s.section(SECTION_CHAR,1,1);
         QString substr;
        if(lastsection.indexOf(QDir::separator()) == 0)
        {
           substr = s.section(SECTION_CHAR,1,1).mid(rootlen).replace("\\",QDir::separator());
        }else
        {
            substr = s.section(SECTION_CHAR,1,1).replace("\\",QDir::separator());
        }
        qa.append(substr);
        cb->addItem(QIcon(substr), s.section(SECTION_CHAR,0,0));
    }
    cb->setCurrentText(cb->property(DKEY_IMGIDX).toString());
    changeJsonValue(key,qa);
    //json[LIST] = qa;
    // 把新的列表更新的json中.

}


NewFrame::NewFrame(QString caption, QWidget *parent)
  //  :FormResizer(parent),Compoent()
    :BaseForm(parent)

{

    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    //setObjectName(CN_NEWFRAME);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setFocusPolicy(Qt::ClickFocus);

    QString uname = QString("%1_%2").arg(caption,QString::number(mWindow->ComCtrl->ProMap.size()));

    setProperty(DKEY_LOCALSEQ,uname);
    setObjectName(uname);
    mbkColor = "#4285F4";
    updateStyleSheets();
    setToolTip(uname);
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

void NewFrame::readFromJson(const QJsonValue &qv)
{
    QJsonObject json = qv.toObject();
    QRect oldrect = Compoent::getRectFromStruct(json[PROPERTY].toArray(),KEY_RECT);
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = json[CLASS].toString();
    QString caption = json[CAPTION].toString();
    qDebug() << " read from json caption is " << caption;
    QVariant variant = json[PROPERTY].toVariant();
//    if(!clsName.compare(CN_NEWLABEL) || !clsName.compare(QLABEL))
//    {
//        NewLabel *newLabel = new NewLabel(m_frame);
//        if(json.contains(PROPERTY))
//        {
//            newLabel->copyProperty(variant);
//            newLabel->setProperty(DKEY_DYN,variant);
//        }
//        newLabel->setGeometry(oldrect);
//        //    newLabel->addMainWindow(mWindow);
//        childlist.append(newLabel);

//        //   LayoutList.append(nl);
//        // newLabel->onSelectMe();
//        foreach (QJsonValue val, json.value(PROPERTY).toArray()) {
//           QJsonObject obj = val.toObject();
//           if(obj.contains(IMAGE))
//           {
//               QPixmap p;
//               QString path = obj[IMAGE].toString();
//               if(path.contains("\\"))
//               {
//                   path.replace("\\","/");
//               }
//               p.load(path);
//               newLabel->setPixmap(p);
//               break;
//           }
//        }
//    }
}


QJsonObject NewFrame::writeToJson()
{

    QJsonObject json =  QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    QJsonArray projson;// 属性

    projson = dynValues[DKEY_DYN].toArray();

    json[NAME] = objectName();
    json[PROPERTY] = updateRBJsonValue(projson,(QWidget*)this);
    dynValues[DKEY_DYN] = projson;
    json[CAPTION] = this->property(DKEY_TXT).toString();
    return json;
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

NewList::NewList(QJsonValue json, const QSize size, QWidget *parent):
    //FormResizer(parent)
    BaseForm(parent),listOrient(Qt::Vertical)
{
  //  setMinimumSize(nsize ); // 最小尺寸
   // setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.

    QJsonObject obj = json.toObject();
    QString caption = obj[CAPTION].toString();

    QString Orientation = obj[QRIENTATION].toString();
   // itemHeight = this->height() * 0.3;
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    setFocusPolicy(Qt::ClickFocus);
    mainScroll =  new QScrollArea(m_frame);

    mainScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setAlignment(Qt::AlignCenter);
   // mainScroll->setWidgetResizable(true);

    mainWidget = new QWidget();
    mainWidget->setObjectName(LISTWIDGET);  // 用来识别布局是否在列表控件下面.
    mainScroll->setWidget(mainWidget);

    menuSetSpace = new QAction(QIcon(":/icon/icons/horizontal-scale-icon.png"),"设置间隔",this);
    if(!Orientation.compare(HORIZONTAL,Qt::CaseInsensitive))
    {
        listOrient = Qt::Horizontal;
        listLayout = new QHBoxLayout(mainWidget);
        menuSetHeight =  new QAction(QIcon(":/icon/icons/fileoverlay_ui@2x.png"),"设置列宽",this);
        menuAddLine = new QAction(QIcon(":/icon/icons/plus.png"),"添加列",this);
        mainScroll->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    }else
    {
        menuAddLine = new QAction(QIcon(":/icon/icons/plus.png"),"添加行",this);
        menuSetHeight =  new QAction(QIcon(":/icon/icons/fileoverlay_ui@2x.png"),"设置行高",this);
        listLayout = new QVBoxLayout(mainWidget);
        mainScroll->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        listOrient = Qt::Vertical;
    }

    listLayout->setSpacing(obj[ITEMSPACE].toInt());
    itemHeight = obj[ITEMSIZE].toInt();

    connect(menuAddLine,SIGNAL(triggered(bool)),SLOT(onAddManyLine()));
    connect(menuSetHeight,SIGNAL(triggered(bool)),SLOT(onSetFixedHeight()));
    connect(menuSetSpace,&QAction::triggered,[=]{
        listLayout->setSpacing( tinySpinBoxDialog(menuSetSpace->text(),listLayout->spacing(),0,99));
        listLayout->update();
    });

    listLayout->setMargin(0);

    listLayout->setContentsMargins(0,0,0,0);
    listLayout->setSizeConstraint(QLayout::SetFixedSize);
   // mainWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
  //  m_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);


    //m_frame->setStyleSheet("background-color: #79AEC8;");
    int n = mWindow->ComCtrl->ProMap.size();
    QString str = QString("%1_%2").arg(caption,QString::number(n));
    setProperty(DKEY_LOCALSEQ,str);
    setProperty(DKEY_TXT,caption);
    setObjectName(str);
    mainWidget->setMaximumSize(999,999);

    setToolTip(str);

    show();

    resize(size);
    repaint();
    mbkColor = "#00BCD4";
    updateStyleSheets();
   // mainScroll->resize(this->size());
   // mainWidget->resize(this->size());


}

void NewList::updateAllItemsSize()
{
    foreach (QWidget *w, childlist) {

        ((NewLayout*)w)->setMaximumSize(this->size());
         ((NewLayout*)w)->setFixedSize(this->size());
        if(listOrient == Qt::Horizontal)
            ((NewLayout*)w)->setFixedWidth(itemHeight);
        else
            ((NewLayout*)w)->setFixedHeight(itemHeight);
    }
}

void NewList::onSetFixedHeight()
{
    QString txt =((QAction*)(QObject::sender()))->text();
    int num = tinySpinBoxDialog(/*"设置每行的高度:"*/txt,itemHeight,2,999);
    if(num)
    {
        itemHeight = num;
        updateAllItemsSize();
    }

}


int  NewList::tinySpinBoxDialog(QString  str,int val ,int min ,int max)
{
    int num = 0;

    BaseDialog *nWindow = new BaseDialog(this);
    nWindow->setObjectName(metaObject()->className());
    nWindow->UpdateStyle();
    QVBoxLayout *vb = new QVBoxLayout();

    QHBoxLayout *hb = new QHBoxLayout();

    QLabel *title = new QLabel(str);
    QSpinBox *spinbox = new QSpinBox();
    spinbox->setToolTip(QString("请输入%1~%2的整数").arg(QString::number(min),QString::number(max)));
    spinbox->setValue(val);

    hb->addWidget(title);
    hb->addWidget(spinbox);
    vb->addLayout(hb);
    QDialogButtonBox *dbb = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                                                 Qt::Horizontal,
                                                 nWindow);
    dbb->button(QDialogButtonBox::Ok)->setText("确定");
   dbb->button(QDialogButtonBox::Cancel)->setText("取消");
   connect(dbb,SIGNAL(accepted()),nWindow,SLOT(accept()));
   connect(dbb,SIGNAL(rejected()),nWindow,SLOT(reject()));
   vb->addWidget(dbb);
    nWindow->setLayout(vb);
    nWindow->exec();

    int ret = nWindow->result();
    if(ret)
         num = spinbox->value();

    nWindow->deleteLater();

    return num;

}

void NewList::onAddManyLine()
{
         QString txt =((QAction*)(QObject::sender()))->text();
         int num = tinySpinBoxDialog(txt,1,1,99);
        QString one = "#CCFFCC";
        QString two = "#99FFCC";
        for(int i = 0; i < num;i++)
        {
            NewLayout *n =   onAddOneLine();
            n->mbkColor = i % 2 ? one : two;
            n->updateStyleSheets();
        }
        if(num) updateAllItemsSize();
}

NewLayout * NewList::onAddOneLine()
{
    onSelectMe();
    QJsonObject obj = QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout).toObject();
    NewLayout *newlayout = new NewLayout(obj[CAPTION].toString(),m_frame->size()-QSize(15,15),mWindow,mainWidget);

    newlayout->parentList = this;

  //  nl->addMainWindow(mWindow);
    childlist.append(newlayout);
   // int n = mWindow->ComCtrl->ProMap.size();
    newlayout->copyProperty(mWindow->ComCtrl->mVariantLayout);
    newlayout->setProperty(DKEY_DYN,mWindow->ComCtrl->mVariantLayout);
    newlayout->initJsonValue();

    newlayout->setProperty(DKEY_INTOLIST,true);
    mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),newlayout);
    newlayout->onSelectMe();
    newlayout->updateStyleSheets();
    newlayout->show();;
    listLayout->addWidget(newlayout);
  //  updateAllItemsSize();

   return newlayout;
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
    }else
    {
        DeleteMe();
    }
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
    updateAllItemsSize();

}


void NewList::wheelEvent(QWheelEvent *event)
{

    QString lname =  listLayout->metaObject()->className() ;
    if(listOrient == Qt::Horizontal)
    {
        if(event->orientation() != Qt::Horizontal)
        {
            QWheelEvent  evt(event->pos(),event->delta(),event->buttons(),event->modifiers(),Qt::Horizontal);
           // wheelEvent(&evt);
            QApplication::sendEvent(mainScroll->horizontalScrollBar(),&evt);
        }
    }
   // event->accept();;
}

QJsonObject NewList::writeToJson()
{
    QJsonArray layoutarr;
    QJsonObject json = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();

    foreach (QWidget *w, childlist) {
        QJsonObject outjson;
        outjson[NAME] = w->objectName();
       // qDebug() << " childlist  " << w->metaObject()->className();
        // 这里只有两种对像类型,NewLayout , NewFrame;
        QString clsname = w->metaObject()->className();

        if(!clsname.compare(CN_NEWLAYOUT))
        {
           outjson = ((NewLayout*)w)->writeToJson();
        }else{
           outjson = ((NewFrame*)w)->writeToJson();
        }

        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LISTWIDGET] = layoutarr;
    json[ITEMSPACE] = listLayout->spacing();
    json[ITEMSIZE] = itemHeight;
   // json[CLASS] = this->metaObject()->className();
   // json[CAPTION] = this->property(DKEY_LOCALSEQ).toString();
    //json[CAPTION] = this->property(DKEY_TXT).toString();
    QJsonArray projson = dynValues[DKEY_DYN].toArray();
    updateRBJsonValue(projson,this);
    projson.append(Compoent::getRectJson(this));
    json[PROPERTY] = projson;
    return json;

}


void NewList::readFromJson(const QJsonObject &valobj)
{

    QRect oldrect = getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
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
        mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),w);
        ((NewLayout*)w)->addChildrenToTree();
    }
}

NewLayout::NewLayout(QString caption, QSize nsize, MainWindow *w, QWidget *parent):
   // FormResizer(parent)
   BaseForm(parent),
   StyleStr("background-color: #DAE4A7;")
{
    this->mWindow = w;

    resize(nsize);
    if(parent)
        setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.
    setFocusPolicy(Qt::ClickFocus);
    setProperty(DKEY_INTOLIST,false);
    show();
  //  mbkColor = "#DAE4A7";


    QString uname = QString("%1_%2").arg(caption,
                                         QString::number(mWindow->ComCtrl->ProMap.size()));
    setProperty(DKEY_LOCALSEQ,uname );
    setProperty(DKEY_TXT,caption);
    setObjectName(uname);
   // updateStyleSheets();
    setToolTip(uname);
}

void NewLayout::onSelectMe()
{

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
        mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),w);
        if(!clsname.compare(CN_NEWLIST)) {
            ((NewList*)w)->addChildrenToTree();

        }else if(!clsname.compare(CN_NEWLAYOUT))
        {
            ((NewLayout*)w)->addChildrenToTree();
        }
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
    mWindow->posWidget->updatePosition(this->pos());
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

}

void NewLayout::onBeComeTemplateWidget()
{

//   QString fpath =  QFileDialog::getSaveFileName(this,"保存成自定义控件,不要保存到其它目录,否则无法加载它.",QDir::currentPath() + "/widgets",
//            "Json files (*.json);;All files (*.*)",
//                new QString("Json files (*.json)"),QFileDialog::ShowDirsOnly|QFileDialog::ReadOnly);
//   qDebug() << " save file " << fpath <<" has json "  << fpath.lastIndexOf("json");
//   if(fpath.isEmpty())
//        return;
//   if(fpath.lastIndexOf("json") == -1)
//       fpath.append(".json");

    QDir wdir(mWindow->cManager->mProjectWidgetDir );
    if(!wdir.exists(mWindow->cManager->mProjectWidgetDir ))
        wdir.mkdir(mWindow->cManager->mProjectWidgetDir );
    QString fpath;
    QString name;
    while(1)
    {
        BaseDialog *nWindow = new BaseDialog(this);
        nWindow->setObjectName("fdialog");
        nWindow->UpdateStyle();
        QVBoxLayout *vb = new QVBoxLayout();
        QLineEdit *nametxt =  new QLineEdit();
        QString deftxt = "请输入控件名称";
        nametxt->setText(deftxt);
        nametxt->selectAll();
        vb->addWidget(nametxt);

        QDialogButtonBox *dbb = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                                                     Qt::Horizontal,
                                                     nWindow);
        dbb->button(QDialogButtonBox::Ok)->setText("确定");
       dbb->button(QDialogButtonBox::Cancel)->setText("取消");
       connect(dbb,SIGNAL(accepted()),nWindow,SLOT(accept()));
       connect(dbb,SIGNAL(rejected()),nWindow,SLOT(reject()));
       vb->addWidget(dbb);
        nWindow->setLayout(vb);
        nWindow->exec();

        int ret = nWindow->result();
        name = nametxt->text();
        fpath =mWindow->cManager->mProjectWidgetDir + QDir::separator() + name;

        nWindow->deleteLater();
        if(ret)
        {
            if(nametxt->text().isEmpty() || !deftxt.compare(nametxt->text()))
            {
                continue;
            }
            break;
        }

        else
            return;
    }


    // 创建一个为当前控件的30%大小的ICON图片.
    QSize iconsize(size());
    QPixmap pixmap(iconsize);
    render(&pixmap,QPoint(),QRegion(rect()));
    pixmap.scaled(size() * 0.3).save(fpath+".png");
 //   pixmap.save(fpath + ".png");

    QJsonObject json ;

     QJsonObject wid = writeToJson();

    wid[ICON] = fpath + ".png";

    QJsonArray property = wid[PROPERTY].toArray();
    for (int i =0 ;i < property.size();i++) {
        QJsonObject tt = property.at(i).toObject();
        if(tt.contains(KEY_RECT))
        {
            QJsonObject rect = tt[KEY_RECT].toObject();
            QVariantMap vmap ;
            vmap[LX] = QString::number(0);
            vmap[LY] = QString::number(0);
            vmap[WIDTH] =rect[WIDTH].toString();
            vmap[HEIGHT] = rect[HEIGHT].toString();
            tt[KEY_RECT] = QJsonObject::fromVariantMap(vmap);

            property[i] = tt;
            break;
        }


    }
    wid[CAPTION] = name;
    QJsonArray arr ;

    wid[PROPERTY] = property;
    arr.append(wid);
    json[COMPOENTS] = arr;
   // arr.append(json);

  //  json[COMPOENTS] = arr;



    QFile saveFile;
    saveFile.setFileName(fpath + ".json");
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
     ((NewFrame*)w)->DeleteMe();
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
        ((NewFrame*)w)->DeleteMe();
    }
}


QJsonObject NewLayout::writeToJson()
{

    QJsonArray layoutarr;
    //  qDebug() << "NewLayout  parent object " << &json;
    QJsonObject json = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    foreach (QWidget *w, childlist) {
        QJsonObject outjson;
       // outjson[NAME] = w->objectName();
        qDebug() << " childlist  " << w->metaObject()->className();
        // 这里只有两种对像类型,NewLayout , NewFrame;
        QString clsname = w->metaObject()->className();

        if(!clsname.compare(CN_NEWLAYOUT))
        {
          outjson =  ((NewLayout*)w)->writeToJson();
        }else if(!clsname.compare(CN_NEWLIST))
        {
           outjson=  ((NewList*)w)->writeToJson();
        }
        else{
           outjson =  ((NewFrame*)w)->writeToJson();
        }

        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LAYOUT] = layoutarr;
    json[NAME] = objectName();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    QJsonArray projson = dynValues[DKEY_DYN].toArray();
    json[PROPERTY] =  updateRBJsonValue(projson,this);
    return json;
}


void NewLayout::readFromJson(const QJsonValue &qv)
{
    QJsonObject valobj = qv.toObject();
     QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = valobj[CLASS].toString();
    QString caption = valobj[CAPTION].toString();
    qDebug() << " read from json caption is " << caption;
    QVariant variant = valobj[PROPERTY].toVariant();
    if(!clsName.compare(CN_NEWLAYOUT)/* || !clsName.compare(CN_LAYOUT)*/)
    {
        NewLayout *newlayout = new NewLayout(caption,oldrect.size(),mWindow,m_frame);
         newlayout->setProperty(DKEY_TYPE, valobj[WTYPE].toString());
         newlayout->setProperty(DKEY_JSONSTR,qv );
        if(valobj.contains(PROPERTY))
        {
            newlayout->copyProperty(variant);
            newlayout->setProperty(DKEY_DYN,variant);
            newlayout->initJsonValue();
        }
        newlayout->setGeometry(oldrect);
        newlayout->addMainWindow(mWindow);
        childlist.append(newlayout);
        mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),newlayout);
        newlayout->onSelectMe();
        newlayout->updateStyleSheets();

        // 这里一定是Layout 嵌套了.
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            newlayout->readFromJson(item);
        }
    }else if(!clsName.compare(CN_NEWLIST))
    {
      NewList *nlist = new NewList(qv,oldrect.size(),m_frame);
      nlist->setProperty(DKEY_JSONSTR,qv );
     // nlist->setProperty(DKEY_TXT,caption);
   //   nlist->setProperty(DKEY_LOCALSEQ,caption);
      if(valobj.contains(PROPERTY))
      {
          nlist->copyProperty(variant);
          nlist->setProperty(DKEY_DYN,variant);
          nlist->initJsonValue();
      }
      nlist->setGeometry(oldrect);
      mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),nlist);
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
        this->createObjectFromJson(qv);
    }

}


QWidget* NewLayout::createObjectFromJson(const QJsonValue &qv)
{
    QJsonObject json = qv.toObject();
    QString caption = json[CAPTION].toString();
    NewFrame *newFrame = new NewFrame(caption,m_frame);
    newFrame->setProperty(DKEY_JSONSTR,qv);
    QVariant variant = json.value(PROPERTY).toVariant();
    newFrame->setProperty(DKEY_TYPE,json[WTYPE].toString());
    if(json.contains(PROPERTY))
    {
        // QJsonObject vp =QJsonObject::fromVariantMap(qvm[PROPERTY]);

        newFrame->copyProperty(variant);
        newFrame->setProperty(DKEY_DYN,variant);
        newFrame->initJsonValue();
    }
    QRect rect = Compoent::getRectFromStruct(json[PROPERTY].toArray(),KEY_RECT);

 //   newlayout->setProperty(DKEY_LOCALSEQ,caption);
    newFrame->setProperty(DKEY_TXT,caption);
    newFrame->setGeometry(rect);
    //newFrame->addMainWindow(mWindow);
    childlist.append(newFrame);
    mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),newFrame);

    QJsonObject obj = getValueFromProperty(json[PROPERTY].toArray(),BAKIMAGE);
    if(!obj.isEmpty() && obj.contains(BAKIMAGE))
    {
       // QPixmap p;
        QString path = obj[BAKIMAGE].toString();
        if(path.contains("\\"))
        {
            path.replace("\\","/");
        }
        newFrame->mbkImage = path;
        newFrame->updateStyleSheets();
    }

    // 这里不再需要添加图片了.
//    foreach (QJsonValue item, json[WIDGET].toArray()) {
//        newFrame->readFromJson(item.toObject());
//    }
    newFrame->show();
    newFrame->onSelectMe();

}


NewLayer::NewLayer(QString caption,QSize nsize, QWidget *parent)
    :BaseForm(parent)
    // mActiveIdx(-1)

{
    mWindow = ((ScenesScreen*)parent)->mWindow;
    resize(nsize);
    setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.

    setFocusPolicy(Qt::ClickFocus);
    show();
    QString key =QString("%1_%2").arg(caption,QString::number( mWindow->ComCtrl->ProMap.size()));
    setProperty(DKEY_LOCALSEQ,key);
    setProperty(DKEY_TXT,caption);
    setObjectName(key);
   // updateStyleSheets();
    setToolTip(key);
}



void NewLayer::readFromJson(const QJsonValue &qv)
{

   QJsonObject  valobj = qv.toObject();
    QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
   // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
   QString clsName = valobj[CLASS].toString();
   QString caption = valobj[CAPTION].toString();
   qDebug() << " read from json caption is " << caption;
   QVariant variant = valobj.value(PROPERTY).toVariant();
   if(!clsName.compare(CN_NEWLAYOUT) /*|| !clsName.compare(CN_LAYOUT)*/)
   {
       NewLayout *newlayout = new NewLayout(caption,oldrect.size(),mWindow,m_frame);
       newlayout->setProperty(DKEY_TYPE, valobj[WTYPE].toString());
       newlayout->setProperty(DKEY_JSONSTR,qv);
       if(valobj.contains(PROPERTY))
       {
           newlayout->copyProperty(variant);
           newlayout->setProperty(DKEY_DYN,variant);
           newlayout->initJsonValue();
       }

       newlayout->setGeometry(oldrect);
       mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),newlayout);
       childlist.append(newlayout);
       newlayout->onSelectMe();
       newlayout->updateStyleSheets();

       // 这里一定是Layout 嵌套了.
       foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
           newlayout->readFromJson(item);
       }
   }


}


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
        mWindow->tree->addObjectToCurrentItem(property(DKEY_LOCALSEQ).toString(),w);
        ((NewLayout*)w)->addChildrenToTree();
    }
}

QJsonObject  NewLayer::writeToJson()
{
    QJsonArray layoutarr;
    QJsonObject json =QJsonValue::fromVariant(this->property(DKEY_JSONSTR)).toObject();
    foreach (QWidget *w, childlist) {
        layoutarr.append(((NewLayout*)w)->writeToJson());
    }
    json[NAME] = objectName();
    //json[CAPTION] = this->property(DKEY_LOCALSEQ).toString();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[LAYOUT] = layoutarr;
    QJsonArray proterty = dynValues[DKEY_DYN].toArray();
    json[PROPERTY] = updateRBJsonValue(proterty,this);
    return json;
}






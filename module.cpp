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



static   QStringList KeyList = QStringList() << DEFAULT << UID
                                             << IMAGE << GRAYCOLOR
                                             << COLOR << BAKCOLOR
                                             << BAKIMAGE ;



QJsonValue Compoent::changeJsonValue(const QJsonArray &arg,QString key,
                               const QVariant &val)
{
    QJsonArray parr = arg;
    int asize = parr.size();
    int i = 0;
    for(int i = 0;i < asize;i++)
    {
        QJsonValue item = parr.at(i);
        if(item.isArray())
        {
            QJsonArray tarray = item.toArray();
            parr.replace(i,changeJsonValue(tarray,key,val));
        }else if(item.isObject())
        {
            QJsonObject obj = item.toObject();
            if(obj.contains(STRUCT))
            {
                QJsonArray arr = obj[STRUCT].toArray();
                obj[STRUCT] = changeJsonValue(arr,key,val);
                parr[i] = obj;

            }else if(obj.contains(key) && !key.compare(KEY_RECT))
            {
                QString str = val.toString();
                QString section1 = str.section(":",0,0);
                int num = str.section(":",1,1).toInt();

                foreach (QString str, QStringList() << LX << LY
                         << WIDTH << HEIGHT) {
                    if(!str.compare(section1))
                    {
                         QJsonObject rect = obj[KEY_RECT].toObject();
                        rect[section1] = num;
                        obj[KEY_RECT] = rect;
                        break;
                    }
                }
                parr.replace(i,obj);
                break;
            }else if(obj.contains(key) && !key.compare(BORDER))
            {
                QString str = val.toString();
                QString section1 = str.section(":",0,0);
                int num = str.section(":",1,1).toInt();
                if(!section1.compare(GRAYCOLOR))
                {
                    obj[GRAYCOLOR] = str.section(":",1,1);
                }else{
                    foreach (QString kstr, QStringList() << TOP << LEFT
                             << RIGHT << BOTTOM) {
                        if(!kstr.compare(section1))
                        {
                            QJsonObject bobj = obj[BORDER].toObject();
                            bobj[kstr] = num;
                            obj[BORDER] = bobj;
                            break;
                        }
                    }
                }
                parr.replace(i,obj);
                break;
            } else if(obj[WTYPE].toString().contains(key) )
            {
                // 这里必需要有-type 这个属性名.
                if(obj.contains(LIST) && obj.contains(DEFAULT))
                {

                    QJsonValue v =  QJsonValue::fromVariant(val);
                    if(v.isArray())
                    {
                        obj[LIST] = v;
                    }else
                    {
                        obj[DEFAULT] = v;
                    }
                }else {
                    foreach (QString kstr, KeyList) {
                        if(obj.contains(kstr))
                        {
                            obj[kstr] = QJsonValue::fromVariant(val);
                            break;
                        }
                    }
                }
                parr.replace(i,obj);
                break;
            }
        }
    }
    return parr;
}

void Compoent::changeJsonValue(QString key, const QVariant &val)
{
   QJsonArray parr =  mOwerJson[PROPERTY].toArray();
  // changeJsonValue(parr,key,val);
   // int asize = parr.size();

    // 这个QJsonObject 都是只读的,修改之后必需重新赋值.
   // dynValues[DKEY_DYN] = parr;

   // dynValues[DKEY_DYN] = changeJsonValue(parr,key,val);
    mOwerJson[PROPERTY] = changeJsonValue(parr,key,val);
}

void Compoent::changeJsonValue(QWidget *w,QString key, const QVariant &val)
{

//    BaseForm *bfthis = (BaseForm *)this;
//    foreach(QByteArray ba, bfthis->dynamicPropertyNames())
//    {
//        qDebug()   << "objname " << bfthis->objectName() << " dynamic Name " << ba;
//    }
//    qDebug() << " QWidget property "
//             << w->property(DKEY_ARRIDX)
//             << w->property(DKEY_PARRIDX)
//             << " objname " << w->objectName();

    QString owercls = w->property(DKEY_OWERJSON).toString();

    if(!owercls.compare("ComProperty"))
    {
        // 默就解析到 JSON PROPERTY 里的属性,不递归到CSS属性里去.
//        int index = w->property(DKEY_ARRIDX).toInt();
//        QJsonArray parray = mOwerJson[PROPERTY].toArray();
//        QJsonValue val = parray.at(index);
//        parray[index] = changeJsonValue(val.toArray(),key,val);
//        mOwerJson[PROPERTY] = parray;
        changeJsonValue(key,val);

    }else if(!owercls.compare("CssProperty"))
    {
        // 如果是QTabWidget 里的控件,就用JSON属性里的CSS属性数组.

        int index = w->property(DKEY_PARRIDX).toInt();
        QJsonArray parray = mOwerJson[PROPERTY].toArray();
        QJsonObject pobj = parray.at(index).toObject();
        QJsonArray structarray = pobj[STRUCT].toArray();
        int  cssindex = w->property(DKEY_ARRIDX).toInt();
        QJsonArray cssArray = structarray.at(cssindex).toArray();
        structarray[cssindex] = changeJsonValue(cssArray,key,val);
        pobj[STRUCT] = structarray;

        parray[index] = pobj;
        mOwerJson[PROPERTY] = parray;
    }
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
            if(w->property(DKEY_INTOCONTAINER).toBool())
            {

            }

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

QVariant Compoent::getJsonValue(const QJsonValue &val, QString key)
{
    if(val.isArray())
    {
        return getJsonValue(val.toArray(),key);
    }else if(val.isObject())
    {
        QJsonObject obj = val.toObject();
        foreach (QString str, KeyList) {
            if(obj.contains(str))
                return obj[str].toVariant();
        }
    }
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
        foreach (QString str, KeyList) {
            if(obj.contains(str))
                return obj[str].toVariant();
        }


    }
    return QVariant();
}

QVariant Compoent::getJsonValue(QString key) const
{
//    return getJsonValue(dynValues[DKEY_DYN].toArray(),key);
    return getJsonValue(mOwerJson[PROPERTY].toArray(),key);
}



void Compoent::onBindValue(QWidget *w,const QVariantMap &map)
{
 //   qDebug() << " dyn size " << dynValues.size();
     // 通过关键字来设置控件的默认值.
    if(!w)
        return;
     BaseForm *myself = (BaseForm*)this;
     QJsonValue  item = QJsonValue::fromVariant(w->property(DKEY_JSONSTR));

     // 是否是第一个TAB;
     bool isFirst = !w->property(DKEY_ARRIDX).toInt();
     QString n = w->metaObject()->className();
     QString uname = w->objectName();
     if(!n.compare("QLineEdit"))
     {
         QLineEdit *txt = (QLineEdit *)w;

         txt->setText(getJsonValue(item, uname).toString());
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
                 QString key = (*it).toString();
                 //QString key = (*it).toString().replace('\\',QDir::separator());

                 cb->addItem(QIcon(key.section(SECTION_CHAR,1,1)),key.section(SECTION_CHAR,0,0));
             }
             cb->setCurrentText(defimg);
             cb->setProperty(DKEY_IMGIDX,defimg);
           }
            else
         {
            cb->setCurrentText(getJsonValue(item,uname).toString());
         }

     }else if(!n.compare("QSpinBox"))
     {
         QSpinBox *sp = (QSpinBox *)w;
         sp->setValue(getJsonValue(item,uname).toDouble());
     }
     else if(!n.compare("QPushButton"))
     {
           QColor c;
          if(!uname.compare(BAKCOLOR))
          {
               c = QColor(getJsonValue(item,BAKCOLOR).toString()) ;
               if(isFirst) myself->mbkColor =c.name();
          }
          else if(!uname.compare(BORDER))
          {
              //unsigned int n = getJsonValue(BORDER).toUInt();
              //灰度转成ＲＧＢ
             // QColor c(n & 0xfff00000,n &0xfff00,n & 0xff);
              //QColor c = QColor::fromRgb(n);
              // 这里暂时有RGB颜色测试
              c = QColor(getJsonValue(item,GRAYCOLOR).toString());
              if(isFirst) myself->mBorderColor = c.name();

          }else if(!uname.compare(BAKIMAGE))
          {
              QPixmap p(12,12);
              QString img = getJsonValue(item,BAKIMAGE).toString();
              img = myself->mWindow->mGlobalSet->value(INI_PRJIMAGEDIR).toString() + BACKSLASH + img;
              p.load(img);
              ((QPushButton*)w)->setIcon(p);
              if(isFirst) myself->mbkImage = img;
              return;

          }else if(!uname.compare(PIC_TEXT))
          {
              return;
          }
          if(isFirst) myself->updateStyleSheets();
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
    if(w->property(DKEY_INTOCONTAINER).toBool())
    {
        // 在容器控件里要把它的坐标,转换成相对于父控件的绝对坐标.
        QPoint pos = w->mapToParent(w->parentWidget()->pos());
        vmap[LX] = QString::number(pos.x());
        vmap[LY] = QString::number(pos.y());
    }else{
        vmap[LX] = QString::number(w->x());
        vmap[LY] = QString::number(w->y());
    }

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


BaseForm::BaseForm(QWidget *parent)
    :FormResizer(parent), mBorderColor("#FFFFFF"),
      mBorder(0,0,0,0),
      mCreateFlag(false)
{
    setStyleSheet("");
    mbkImage = "";
    setContentsMargins(0,0,0,0);

}


void BaseForm::mouseMoveEvent(QMouseEvent *event)
{
    QString clsname = parentWidget()->parentWidget()->metaObject()->className();
   //  qDebug() << " parent class name " <<  clsname;
     // 这里主要防止它在列表的拖动事件.
    QString thisname = metaObject()->className();
    if(thisname.compare(CN_NEWLAYER))
        if(CN_NEWLAYOUT.compare(clsname) &&
                CN_NEWLAYER.compare(clsname) ) return;
    if (event->buttons() & Qt::LeftButton)
    {
        move(this->pos() + (event->pos() - mOffset));
        /* 把新的位置更新到右边属性框 */
        if(!posWidget->property(DKEY_ARRIDX).toInt())
        {
            posWidget->updatePosition(this);
        }
        changeJsonValue(posWidget,
                        KEY_RECT,
                        QString("%1:%2").arg(LX,
                        QString::number(this->x())));
        changeJsonValue(posWidget,
                        KEY_RECT,
                        QString("%1:%2").arg(LY,
                        QString::number(this->y())));

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
        contextMenu->addSeparator();
         QAction saveTemp(QIcon(":/icon/icons/build.png"),"保存成控件",this);
        if(!CN_NEWLAYOUT.compare(clsname))
        {
            contextMenu->addAction(&saveTemp);
            connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));
        }
        QAction hideobj(QIcon(":/icon/icons/eye_closed@2x.png"),"隐藏",this);
        if(!CN_NEWFRAME.compare(clsname))
        {

        }else
        {
            connect(&hideobj,SIGNAL(triggered(bool)),SLOT(onSwapViewObject(bool)));
            contextMenu->addAction(&hideobj);
        }

        contextMenu->addSeparator();
        QAction copy(QIcon(":/icon/icons/editcopy.png"),"复制",this);
        connect(&copy,&QAction::triggered,[=](){
           mWindow->mCopyItem = QJsonValue(writeToJson());
        });

        QAction paste(QIcon(":/icon/icons/editpaste.png"),"粘贴",this);
        connect(&paste,&QAction::triggered,[=](){
           mWindow->cManager->activeSS()->pasteItem(this);
        });

        paste.setEnabled(!mWindow->mCopyItem.isNull());
        contextMenu->addAction(&copy);
        contextMenu->addAction(&paste);




       // QWidget *parentWidget = this->parentWidget();
       // QString pobjname = parentWidget->objectName();
        bool inContainer = property(DKEY_INTOCONTAINER).toBool();

        if(inContainer)
        {
            clsname = ((NewLayout*)this)->container->metaObject()->className();
            if( !clsname.compare(CN_NEWLIST))
            {
                NewList*  nl =(NewList*)(((NewLayout*)this)->container);
                contextMenu->addAction(nl->menuAddLine);
                contextMenu->addAction(nl->menuSetHeight);
                contextMenu->addAction(nl->menuSetSpace);
            }else if(!clsname.compare(CN_NEWGRID))
            {
                NewGrid*   ng = (NewGrid*)(((NewLayout*)this)->container);
                contextMenu->addAction(ng->menuAddRow);
                contextMenu->addAction(ng->menuAddCol);
                contextMenu->addAction(ng->menuSpace);
                contextMenu->addAction(ng->menuSize);
                contextMenu->addAction(ng->menuV);
                contextMenu->addAction(ng->menuH);
            }


        }else if(!clsname.compare(CN_NEWGRID))
        {
            NewGrid *ng = (NewGrid*)this;
            contextMenu->addAction(ng->menuAddRow);
            contextMenu->addAction(ng->menuAddCol);
            contextMenu->addAction(ng->menuSpace);
            contextMenu->addAction(ng->menuSize);
            contextMenu->addAction(ng->menuV);
            contextMenu->addAction(ng->menuH);

        }else if(!clsname.compare(CN_NEWLIST))
        {
            NewList*  nl =(NewList*)this;
            contextMenu->addAction(nl->menuAddLine);
            contextMenu->addAction(nl->menuSetHeight);
            contextMenu->addAction(nl->menuSetSpace);
        }

        contextMenu->exec(mapToGlobal(event->pos()));
        contextMenu->deleteLater();
    }

}

void BaseForm::onSwapViewObject(bool b)
{
    mWindow->tree->onSwapShowHideObject(false);
}

void BaseForm::mouseReleaseEvent(QMouseEvent *event)
{

    /* 放开鼠标时检查它的是否出了边界要 */
   // QWidget *p = this->parentWidget();
    QPoint pos = this->pos();
    moveNewPos(pos);
    // 这里只能在释放鼠标时改变左边的控件值

    //mWindow->posWidget->updateSize(this->size());

    if(!posWidget->property(DKEY_ARRIDX).toInt())
    {
        posWidget->updateSize(this);
        posWidget->updatePosition(this);
    }

    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(LX,QString::number(this->x())));
    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(LY,QString::number(this->y())));

    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(WIDTH,QString::number(this->width())));
    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(HEIGHT,QString::number(this->height())));
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
    bool isFirst = !sender->property(DKEY_ARRIDX).toInt();
    if(!sender->objectName().compare(X))
    {

        if(isFirst) {
            QPoint pos = this->pos();
            pos.setX(v);
            this->move(pos);}
        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(LX,QString::number(v)));

    }else if(!sender->objectName().compare(Y))
    {
        if(isFirst) {
            QPoint pos = this->pos();
            pos.setY(v);
            this->move(pos );}
        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(LY,QString::number(v)));

    }else if(!sender->objectName().compare(W))
    {
        if(isFirst) {
            if((this->pos().x() + v )> this->parentWidget()->size().width())
                return;
            QSize n(this->size());
            n.setWidth(v);
            this->resize(n);}
        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(WIDTH,QString::number(v)));
    }else if(!sender->objectName().compare(H))
    {
        if(isFirst) {
            if((this->pos().y() + v )> this->parentWidget()->size().height())
                return;
            QSize n(this->size());
            n.setHeight(v);
            this->resize(n);}
        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(HEIGHT,QString::number(v)));
    }
}

void BaseForm::onNumberChanged(int num)
{

    QSpinBox *sp = (QSpinBox *)(QObject::sender());
    changeJsonValue(sp,sp->objectName(),num);
  //  dynValues[sp->objectName()] = num;
}

void BaseForm::onTextChanged(QString str)
{
    QLineEdit *txt = (QLineEdit *)(QObject::sender());

    changeJsonValue(txt,txt->objectName(),str);
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


void BaseForm::onBorderChangedValue(int v)
{
    QWidget *w = (QWidget*)QObject::sender();
    QString name = w->objectName();
     changeJsonValue(w,BORDER,
               QString("%1:%2").arg(name,QString::number(v)));
     if(!w->property(DKEY_ARRIDX).toInt())  // 只有第一全CSS属性可以与界面互动.
     {
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
     }
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
      if(!QFileInfo(mbkImage).exists())
      {
          mbkImage = QFileInfo(mbkImage).absoluteFilePath();
      }
      str =  QString("background-image: url(%1); %2").arg(mbkImage,str);
   }

   QString clsname = metaObject()->className();

   if(!clsname.compare(CN_NEWGRID))
   {
       ((NewGrid*)this)->mainScroll->setStyleSheet(QString("QScrollArea#%1 { %2 }").arg(
                                                 this->objectName(),
                                                 str));
   }
   else if(!clsname.compare(CN_NEWLIST))
   {
       ((NewList*)this)->mainScroll->setStyleSheet(QString("%1#%2 { %3 }").arg(clsname,
                                                 this->objectName(),
                                                 str));
   }else{
       setStyleSheet(QString("BaseForm#%1 { %2 }").arg(this->objectName(),str));
   }

   update();
   repaint();

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
       changeJsonValue(cb,cb->objectName(),mWindow->mOrderlist[cb->currentIndex()]);
   }else
       changeJsonValue(cb,cb->objectName(),txt);


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
        bool isFirst = !btn->property(DKEY_ARRIDX).toInt();
        if(!objname.compare(BAKCOLOR))
        {
            changeJsonValue(btn,BAKCOLOR,c.name());
            if(isFirst)
                mbkColor = c.name(QColor::HexArgb);
        }
        else if(!objname.compare(BORDER))
        {

            changeJsonValue(btn,BORDER,
                            QString("%1:%2").arg( GRAYCOLOR,c.name(QColor::HexArgb)));
                                                 // QString::number(c.value())));
                                                //  QString::number(qGray(c.rgb()))));
            if(isFirst)
                mBorderColor = c.name(QColor::HexArgb);
            //  changeJsonValue(btn,BORDER,c);
        }
        if(isFirst) updateStyleSheets();

    }

}


void BaseForm::removeChild(QWidget *w)
{
    childlist.removeOne(w);
}

void BaseForm::onSelectMe()
{
    mWindow->cManager->activeSS()->setSelectObject(this);

    mWindow->propertyWidget->createPropertyBox(this);
    posWidget->setConnectNewQWidget(this);
    posWidget->updatePosition(this);
    posWidget->updateSize(this);
}

void BaseForm::DeleteMe()
{
    QListIterator<QWidget*> item(childlist);
    while(item.hasNext())
    {
        QWidget *w = item.next();
        ((BaseForm*)w)->DeleteMe();
    }

    mWindow->tree->setMyParentNode();  //选中它的父控件.
    mWindow->tree->deleteItem(this);
    mWindow->ComCtrl->ProMap.remove(mUniqueStr);
    mWindow->propertyWidget->delPropertyBox();
    // 它是否是列表控件的一员.
    deleteLater();
}

void BaseForm::initJsonValue()
{
    mbkColor = Compoent::getJsonValue(BAKCOLOR).toString();
    mBorderColor = Compoent::getJsonValue(BORDER).toString();
    mBorder = Compoent::getRectFromStruct(mOwerJson[PROPERTY].toArray(),BORDER);
    if(this->mCreateFlag)
    {
        setProperty(DKEY_UID,mWindow->ComCtrl->ProMap.size());
    }else{
        setProperty(DKEY_UID,Compoent::getJsonValue(UID).toInt());
    }

    //changeJsonValue(UID,mWindow->ComCtrl->ProMap.size());
}

void BaseForm::onBackgroundImageDialog()
{
    QWidget *w  = (QWidget*)(QObject::sender());
    QString imgdir = mWindow->mGlobalSet->value(INI_PRJIMAGEDIR).toString();
    ImageListView *imgview = new ImageListView(imgdir,this);
    // 把一个动态属性传递给另一个事件发送对像,用来确定要修改JSON里的那一段值.
    imgview->imglist->setProperty(DKEY_ARRIDX,w->property(DKEY_ARRIDX));
    imgview->imglist->setProperty(DKEY_PARRIDX,w->property(DKEY_PARRIDX));
    imgview->imglist->setProperty(DKEY_OWERJSON,w->property(DKEY_OWERJSON));


    QObject::connect(imgview->imglist,&QListWidget::itemDoubleClicked,
            [=](QListWidgetItem *item){

       // QWidget *sender = imgview->imglist;
        QVariantMap vmap =  imgview->imglist->property(DKEY_IMGMAP).toMap();
        if(!vmap.isEmpty())
        {
            QString fpath = vmap[item->text()].toString();
            if(!imgview->imglist->property(DKEY_ARRIDX).toInt())
            {
                mbkImage = fpath;
                updateStyleSheets();
            }

            int n = imgdir.length() +1;

            changeJsonValue(imgview->imglist,property(DKEY_CURVAL).toString(),fpath.mid(n));

            QPixmap p(12,12);
            p.load(fpath);
            ((QPushButton*)w)->setIcon(p);
        }

    });

    imgview->setFixedSize(mWindow->size() * 0.6);
    imgview->exec();
    //int ret = imgview->result() ;
    imgview->deleteLater();
}


void BaseForm::onSelectedBackgroundImage(QListWidgetItem *item)
{
    QWidget *sender = (QWidget*)(QObject::sender());
    QVariantMap vmap =  sender->property(DKEY_IMGMAP).toMap();
    if(!vmap.isEmpty())
    {
        QString fpath = vmap[item->text()].toString();
        if(!sender->property(DKEY_ARRIDX).toInt())
        {
            mbkImage = fpath;
            updateStyleSheets();
        }
        changeJsonValue(sender,property(DKEY_CURVAL).toString(),fpath);
    }

}

void BaseForm::onListImageChanged(QString img)
{

   QWidget *w = (QWidget*)(QObject::sender());
   QString objname =w->objectName();
   QVariantList imglist = this->property(DKEY_IMAGELST).toList();
   foreach (QVariant v, imglist) {
       QString s = v.toString();
       QString k = s.section(SECTION_CHAR,0,0);
       if(!k.compare(img))
       {
           QString fpath = s.section(SECTION_CHAR,1,1);
           int len = mWindow->mGlobalSet->value(INI_PRJIMAGEDIR).toString().length()+1;
           changeJsonValue(w,objname,fpath.mid(len)); // 修改JSON里的值
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
    ImageFileDialog *ifd = new ImageFileDialog(imglist,
                                               mWindow->mGlobalSet->value(INI_PRJIMAGEDIR).toString(),
                                               this);

    ifd->exec();
     imglist = ifd->getSelectedList();
    ifd->deleteLater();
    QString imgdir = mWindow->mGlobalSet->value(INI_PRJIMAGEDIR).toString();
    if(imgdir.isEmpty())
        imgdir = QDir::currentPath();

    int rootlen  = /*QDir::currentPath().length()+1;*/imgdir.length() +1;
    QJsonArray qa;

    cb->clear();
    setProperty(DKEY_IMAGELST,imglist);
    foreach (QVariant v, imglist) {
        QString s = v.toString();
        // example for s   "alarm_du.bmp:/home/yjdwbj/build-ut-tools-Desktop_Qt_5_6_0_GCC_64bit-Debug/images/string/alarm_du.bmp
        // example for s   ""m104.bmp:config/images/string/m104.bmp"
        QString lastsection = s.section(SECTION_CHAR,1,1);
         QString substr;
        if(lastsection.indexOf(BACKSLASH) == 0)
        {
           substr = s.section(SECTION_CHAR,1,1).mid(rootlen);
        }else
        {
            substr = s.section(SECTION_CHAR,1,1);
        }
        qa.append(substr);
        cb->addItem(QIcon(substr), s.section(SECTION_CHAR,0,0));
    }
    cb->setCurrentText(cb->property(DKEY_IMGIDX).toString());
    changeJsonValue((QWidget*)QObject::sender(),key,qa);
    //json[LIST] = qa;
    // 把新的列表更新的json中.

}


int  BaseForm::tinySpinBoxDialog(QString  str,int val ,int min ,int max)
{
    int num = 0;

    BaseDialog *nWindow = new BaseDialog(this);
    nWindow->setObjectName(metaObject()->className());
    nWindow->UpdateStyle();
    QVBoxLayout *vb = new QVBoxLayout();

    QHBoxLayout *hb = new QHBoxLayout();

    QLabel *title = new QLabel(str);
    QSpinBox *spinbox = new QSpinBox();
    spinbox->setMaximum(max);
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

QSize  BaseForm::ChangeSizeDialog(QSize size)
{
    int w,h = 0;
    while(1)
    {

        BaseDialog *nWindow = new BaseDialog(this);
        nWindow->setObjectName(metaObject()->className());
        nWindow->UpdateStyle();
        QGridLayout *glayout = new QGridLayout();

        QLabel *wt = new QLabel("宽:");
        QLabel *ht= new QLabel("高:");

        QString tooltip = "请输入 1000 < 0 内的整数";
        QSpinBox *wbox = new QSpinBox();
        QSpinBox *hbox = new QSpinBox();
        wbox->setMaximum(999);
        wbox->setToolTip(tooltip);
        wbox->setValue(size.width());

        hbox->setMaximum(999);
        hbox->setToolTip(tooltip);
        hbox->setValue(size.height());

        glayout->addWidget(wt,0,0);
        glayout->addWidget(wbox,0,1);
        glayout->addWidget(ht,1,0);
        glayout->addWidget(hbox,1,1);
        QDialogButtonBox *dbb = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                                                     Qt::Horizontal,
                                                     nWindow);
        dbb->button(QDialogButtonBox::Ok)->setText("确定");
        dbb->button(QDialogButtonBox::Cancel)->setText("取消");
        connect(dbb,SIGNAL(accepted()),nWindow,SLOT(accept()));
        connect(dbb,SIGNAL(rejected()),nWindow,SLOT(reject()));
        glayout->addWidget(dbb,2,0,1,0);
        nWindow->setLayout(glayout);
        nWindow->exec();


        int ret = nWindow->result();
        w = wbox->value();
        h = hbox->value();
        nWindow->deleteLater();
        if(ret)
        {
            if(!w || !h)
            {
                continue;
            }
            break;
        }
    }

    return QSize(w,h);
}

void BaseForm::wheelEvent(QWheelEvent *event)
{
   event->ignore();
}

NewLayout *BaseForm::CreateNewLayout(const QJsonValue &qv,
                                     QWidget *parent,bool isCreate)
{

    QJsonObject  valobj = qv.toObject();
    QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // QVariant variant = valobj.value(PROPERTY).toVariant();
    QString caption = valobj[CAPTION].toString();
    NewLayout *newlayout = new NewLayout(caption,oldrect.size(),mWindow,parent);
    newlayout->mCreateFlag = isCreate;
    newlayout->setProperty(DKEY_TYPE, valobj[WTYPE].toString());
    newlayout->mOwerJson = qv.toObject();
    mWindow->tree->addObjectToCurrentItem(mUniqueStr,newlayout);
    newlayout->onSelectMe();
    newlayout->updateStyleSheets();
    newlayout->show();
    return newlayout;
}



void BaseForm::addChildrenToTree()
{
    foreach (QWidget *w, childlist) {
        mWindow->tree->addObjectToCurrentItem(mUniqueStr,w);
        ((BaseForm*)w)->addChildrenToTree();
    }
}

NewFrame::NewFrame(QString caption, QWidget *parent)
  //  :FormResizer(parent),Compoent()
    :BaseForm(parent)
{

    mType =TYPEFRAME;
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    //setObjectName(CN_NEWFRAME);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setFocusPolicy(Qt::ClickFocus);

    QString uname = QString("%1_%2").arg(caption,QString::number(mWindow->ComCtrl->ProMap.size()));
    mUniqueStr = uname;
    //setProperty(DKEY_LOCALSEQ,uname);
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
}


QJsonObject NewFrame::writeToJson()
{
    QJsonObject json =  mOwerJson;
   // QJsonArray projson;// 属性
    json[NAME] = objectName();
    json[PROPERTY] = mOwerJson[PROPERTY];
    json[CAPTION] = this->property(DKEY_TXT).toString();
    return json;
}



NewGrid::NewGrid(const QJsonValue &qv,
                  const QList<int> *arglist, QWidget *parent)
    :BaseForm(parent),
     mainWidget(new QWidget()),
     sliderOrientation(Qt::Vertical),
     rows(arglist->at(0)),
     mainScroll(new QScrollArea(m_frame)),
     cols(arglist->at(1)),itemSize(arglist->at(2),arglist->at(3))
{
    mType=TYPEGRID;
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    QJsonObject obj = qv.toObject();
    QString caption = obj[CAPTION].toString();
    setFocusPolicy(Qt::ClickFocus);

    mainScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setAlignment(Qt::AlignCenter);
   // mainScroll->setWidgetResizable(true);


  //  mainScroll->setStyleSheet("background-color: #A9A9A9;");
    mainScroll->setWidget(mainWidget);
    mainScroll->setContentsMargins(0,0,0,0);

    gridLayout = new QGridLayout(mainWidget);
    gridLayout->setSpacing(obj[ITEMSPACE].toInt());
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSizeConstraint(QLayout::SetFixedSize);

    int n = mWindow->ComCtrl->ProMap.size();
    QString str = QString("%1_%2").arg(caption,QString::number(n));
    mUniqueStr = str;
   // setProperty(DKEY_LOCALSEQ,str);
    setProperty(DKEY_TXT,caption);
    setObjectName(str);
    mainScroll->setObjectName(str);
    mainWidget->setObjectName(str);  // 用来识别布局是否在列表控件下面.
    mainWidget->setMaximumSize(999,999);
    mainWidget->setContentsMargins(0,0,0,0);
   // mainWidget->setStyleSheet("background-color: #A9A9A9;");

    setToolTip(str);
    QRect oldrect = getRectFromStruct(obj[PROPERTY].toArray(),KEY_RECT);
    gridLayout->setGeometry(oldrect);
    setGeometry(oldrect);
    resize(oldrect.size());
    mainWidget->resize(oldrect.size());
    show();
   // repaint();

    mbkColor = "#FFE4C4";
    updateStyleSheets();

   //setProperty(DKEY_JSONSTR,qv );
    mOwerJson = qv.toObject();

    menuAddRow =  new QAction(QIcon(":/icon/icons/addtab.png"),"添加行",this);
    connect(menuAddRow,SIGNAL(triggered(bool)),SLOT(onAddOneRow()));

    menuAddCol =  new QAction(QIcon(":/icon/icons/addtab.png"),"添加列",this);
    connect(menuAddCol,SIGNAL(triggered(bool)),SLOT(onAddOneCol()));

    menuSpace = new QAction(QIcon(":/icon/icons/same-width.png"),"单元间距",this);

    connect(menuSpace,&QAction::triggered,[=](){
        gridLayout->setSpacing(tinySpinBoxDialog(menuSpace->text(),gridLayout->spacing(),1,99));
    });

    menuSize = new QAction(QIcon(":/icon/icons/scale-icon.png"),"单元尺寸",this);
    connect(menuSize,&QAction::triggered,[=](){
        itemSize.scale(ChangeSizeDialog(itemSize),Qt::IgnoreAspectRatio);
        updateAllItemsSize();
    });


    menuOrientation = new QActionGroup(this);
    menuV = new QAction(/*QIcon(":/icon/icons/same-height.png"),*/"垂直滚动",this);
    menuH = new QAction(/*QIcon(":/icon/icons/same-width.png"),*/"水平滚动",this);
    menuOrientation->addAction(menuV);
    menuOrientation->addAction(menuH);
    menuV->setCheckable(true);
    menuH->setCheckable(true);
    menuV->setChecked(true);

    connect(menuOrientation,&QActionGroup::triggered,[=](QAction *a)
    {
        sliderOrientation = a == menuV ? Qt::Vertical : Qt::Horizontal;
    });

     QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonRelease,QCursor::pos(),
                       Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
     QApplication::postEvent(this,event);
}


void NewGrid::addRowsCols()
{

    for(int x = 0 ; x < rows ;x++)
        for(int y = 0;y  < cols ; y++)
            initRowsCols(x,y,
                         QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));

    updateAllItemsSize();
}

void NewGrid::initRowsCols(int row,int col,const QJsonValue &value)
{
    QString odd = "#CDC0B0";
    QString nodd = "#EED5B7";
    QString nnn = "#98F5FF";
    QString tt = "#FF7F50";
    onSelectMe();
    NewLayout *nl = CreateNewLayout(value,mainWidget,mCreateFlag);
    nl->container = this;
    nl->setProperty(DKEY_INTOCONTAINER,true);
    childlist.append(nl);
    // 自已的行列坐标.
    nl->setProperty(DKEY_ROW,row);
    nl->setProperty(DKEY_COL,col);
    if(mCreateFlag) // 第一次创建有四个不同的色隔开.
    {
        if(col % 2)
        {
            if(row % 2)
                nl->mbkColor = tt;
            else
                nl->mbkColor = odd;
        }
        else{
            if(row % 2)
                nl->mbkColor = nnn;
            else
                nl->mbkColor = nodd;
        }
        nl->changeJsonValue(BAKCOLOR,QColor(nl->mbkColor));
        nl->updateStyleSheets();
    }
    gridLayout->addWidget(nl,row,col);

}

void NewGrid::onAddOneRow()
{
    mCreateFlag = true;
    QAction *a = (QAction*)(QObject::sender());
    int num = tinySpinBoxDialog(a->text(),1,0,99);
    for(int x = 0 ; x < num ;
        x++,rows++)
    {
        for(int i =0;i<cols ;i++)
        {
            initRowsCols(rows,i,QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));
        }
    }

    updateAllItemsSize();

}

void NewGrid::onAddOneCol()
{
    mCreateFlag = true;
    QAction *a = (QAction*)(QObject::sender());
    int num = tinySpinBoxDialog(a->text(),1,0,99);
    for(int x = 0 ; x < num;
        x++,cols++)
    {
        for(int i =0;i<rows ;i++)
        {
            initRowsCols(i,cols,QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));
        }
    }
    updateAllItemsSize();

}

void NewGrid::updateAllItemsSize()
{
    foreach (QWidget *w, childlist) {
        ((NewLayout*)w)->setMaximumSize(this->size());
        ((NewLayout*)w)->setFixedSize(itemSize);
    }
    onSelectMe();

}

void NewGrid::onDeleteMe()
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

void NewGrid::mouseReleaseEvent(QMouseEvent *event)
{

     mainScroll->resize(this->size());
     mainWidget->resize(this->size());

}

void NewGrid::wheelEvent(QWheelEvent *event)
{

    if(sliderOrientation == Qt::Horizontal)
    {
        if(event->orientation() != Qt::Horizontal)
        {
            QWheelEvent  *evt = new QWheelEvent(event->pos(),event->globalPos(),
                             event->delta(),
                             event->buttons(),
                             event->modifiers(),Qt::Horizontal);
          //  QApplication::sendEvent(mainScroll->horizontalScrollBar(),&evt);
            QApplication::postEvent(mainScroll->horizontalScrollBar(),evt);
        }

       // wheelEvent(&evt);
    }

}


//bool NewGrid::eventFilter(QObject *obj, QEvent *event)
//{
//    if(!CN_NEWLAYOUT.compare(obj->metaObject()->className()) &&
//            event->type() == QEvent::Wheel)
//    {
//        this->wheelEvent((QWheelEvent*)event);
//        return true;
//    }
//    return BaseForm::eventFilter(obj,event);
//}


void NewGrid::readFromJson(const QJsonValue &value)
{

    QJsonObject obj = value.toObject();
    int row = obj[JROW].toInt();
    int col = obj[JCOL].toInt();

    initRowsCols(row,col,value);
    NewLayout *nl = (NewLayout*)(childlist.last());


    // 这里一定是Layout 嵌套了.
    foreach (QJsonValue item, obj[LAYOUT].toArray()) {
        nl->readFromJson(item,mCreateFlag);
    }
}

QJsonObject NewGrid::writeToJson()
{
    QJsonArray layoutarr;
    //QJsonObject json = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    QJsonObject json = mOwerJson;

    foreach (QWidget *w, childlist) {
        QJsonObject outjson =  QJsonValue::fromVariant(w->property(DKEY_JSONSTR)).toObject();
        outjson[NAME] = w->objectName();
       // qDebug() << " childlist  " << w->metaObject()->className();
        // 这里只有两种对像类型,NewLayout , NewFrame;
       // QString clsname = w->metaObject()->className();
        outjson = ((BaseForm*)w)->writeToJson();
        outjson[JCOL] = w->property(DKEY_COL).toInt();
        outjson[JROW] = w->property(DKEY_ROW).toInt();
        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LISTWIDGET] = layoutarr;
    json[ITEMSPACE] = gridLayout->spacing();
    json[GCOLS] = cols;
    json[GROWS] = rows;
    json[WIDTH] = itemSize.width();
    json[HEIGHT] = itemSize.height();


   // QJsonArray projson = dynValues[DKEY_DYN].toArray();

   // json[PROPERTY] = updateRBJsonValue(json[PROPERTY].toArray(),this);;
     json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}

NewList::NewList(QJsonValue json, const QSize size, QWidget *parent):
    //FormResizer(parent)
    BaseForm(parent),

    sliderOrientation(Qt::Vertical),
    mainScroll(new QScrollArea(m_frame)),
 //   mainScroll(new ContainerScroll(m_frame)),
    mainWidget(new QWidget())

{
    mType =TYPELIST;
    setProperty(DKEY_JSONSTR,json.toVariant());
    QJsonObject obj = json.toObject();
    QString caption = obj[CAPTION].toString();

    QString Orientation = obj[QRIENTATION].toString();
   // itemHeight = this->height() * 0.3;
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    //setFocusPolicy(Qt::ClickFocus);

    mainScroll->setFocusPolicy(Qt::WheelFocus);
    mainScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setAlignment(Qt::AlignCenter);
    mainScroll->setContentsMargins(0,0,0,0);


    mainWidget->setObjectName(LISTWIDGET);  // 用来识别布局是否在列表控件下面.
    mainWidget->setContentsMargins(0,0,0,0);
    mainScroll->setWidget(mainWidget);

    menuSetSpace = new QAction(QIcon(":/icon/icons/horizontal-scale-icon.png"),"设置间隔",this);
    if(!Orientation.compare(HORIZONTAL,Qt::CaseInsensitive))
    {
        sliderOrientation = Qt::Horizontal;
        listLayout = new QHBoxLayout(mainWidget);
        menuSetHeight =  new QAction(QIcon(":/icon/icons/fileoverlay_ui@2x.png"),"设置列宽",this);
        menuAddLine = new QAction(QIcon(":/icon/icons/addtab.png"),"添加列",this);
        mainScroll->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    }else
    {
        menuAddLine = new QAction(QIcon(":/icon/icons/addtab.png"),"添加行",this);
        menuSetHeight =  new QAction(QIcon(":/icon/icons/fileoverlay_ui@2x.png"),"设置行高",this);
        listLayout = new QVBoxLayout(mainWidget);
        mainScroll->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        sliderOrientation = Qt::Vertical;
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

    int n = mWindow->ComCtrl->ProMap.size();
    QString str = QString("%1_%2").arg(caption,QString::number(n));
    mUniqueStr = str;
  //  setProperty(DKEY_LOCALSEQ,str);
    setProperty(DKEY_TXT,caption);
    setObjectName(str);
    mainScroll->setObjectName(str);
    mainWidget->setMaximumSize(999,999);
    setToolTip(str);

    resize(size);
    //show();
   // repaint();
    mbkColor = "#00BCD4";
    updateStyleSheets();

    QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonRelease,QCursor::pos(),
                      Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);

//    QMouseEvent event(QMouseEvent::MouseButtonRelease,QCursor::pos(),
//                      Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
  //  QApplication::sendEvent(this,&event);
    QApplication::postEvent(this,event);
 //   qApp->notify(this,&event);
    show();
}

void NewList::updateAllItemsSize()
{
    foreach (QWidget *w, childlist) {

        ((NewLayout*)w)->setMaximumSize(this->size());
         ((NewLayout*)w)->setFixedSize(this->size());
        if(sliderOrientation == Qt::Horizontal)
            ((NewLayout*)w)->setFixedWidth(itemHeight);
        else
            ((NewLayout*)w)->setFixedHeight(itemHeight);
    }
    onSelectMe();
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


void NewList::onAddManyLine()
{
         QString txt =((QAction*)(QObject::sender()))->text();
         int num = tinySpinBoxDialog(txt,1,1,99);
        QString one = "#CCFFCC";
        QString two = "#99FFCC";
        for(int i = 0; i < num;i++)
        {
            NewLayout *n =   AddOneLine(QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));
            n->mbkColor = i % 2 ? one : two;
            n->changeJsonValue(BAKCOLOR,QColor(n->mbkColor));

            n->updateStyleSheets();
        }
        if(num) updateAllItemsSize();
}

NewLayout * NewList::AddOneLine(QJsonValue value)
{
    onSelectMe();
    NewLayout *newlayout = CreateNewLayout(value,mainWidget,mCreateFlag);
    onSelectMe();
    newlayout->onSelectMe();
    newlayout->setProperty(DKEY_INTOCONTAINER,true);
    childlist.append(newlayout);
    newlayout->container = this;
    listLayout->addWidget(newlayout);
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


    if(sliderOrientation == Qt::Horizontal)
    {
        if(event->orientation() != Qt::Horizontal)
        {
            QWheelEvent  *evt = new QWheelEvent(event->pos(),event->globalPos(),
                             event->delta(),
                             event->buttons(),
                             event->modifiers(),Qt::Horizontal);
          //  QApplication::sendEvent(mainScroll->horizontalScrollBar(),&evt);
            QApplication::postEvent(mainScroll->horizontalScrollBar(),evt);
        }

       // wheelEvent(&evt);
    }
  //  event->accept();;
}


bool NewList::eventFilter(QObject *obj, QEvent *event)
{
    if(!CN_NEWLAYOUT.compare(obj->metaObject()->className()) &&
            event->type() == QEvent::Wheel)
    {

       // wheelEvent(&evt);
        QWheelEvent* w = static_cast<QWheelEvent*>(event);

        if(sliderOrientation == Qt::Horizontal)
        {
            QWheelEvent  evt(w->pos(),w->globalPos(),this->width()/4,
                             w->buttons(),
                             w->modifiers(),Qt::Horizontal);

            obj->removeEventFilter(this);
            QApplication::sendEvent(mainScroll,&evt);
            obj->installEventFilter(this);
        }else
        {
            QWheelEvent  evt(w->pos(),w->globalPos(),this->width()/4,
                             w->buttons(),
                             w->modifiers(),Qt::Vertical);
//            qDebug() << " pos " << w->pos() << "gpos " << w->globalPos()
//                     << w->orientation() << w->buttons() << w->delta();
            obj->removeEventFilter(this);
            QApplication::sendEvent(mainScroll,&evt);
            obj->installEventFilter(this);
        }

        return true;
    }
    return BaseForm::eventFilter(obj,event);
}

QJsonObject NewList::writeToJson()
{
    QJsonArray layoutarr;
  //  QJsonObject json = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    QJsonObject json = mOwerJson;

    foreach (QWidget *w, childlist) {
        QJsonObject outjson =  QJsonValue::fromVariant(w->property(DKEY_JSONSTR)).toObject();
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
  //  QJsonArray projson = dynValues[DKEY_DYN].toArray();
   // json[PROPERTY] = updateRBJsonValue(json[PROPERTY].toArray(),this);;
     json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}


void NewList::readFromJson(const QJsonValue &qv)
{
    QJsonObject valobj = qv.toObject();
    //QRect oldrect = getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = valobj[CLASS].toString();
    if(!clsName.compare(CN_NEWLAYOUT) || !clsName.compare(LAYOUT))
    {

        NewLayout *nl = AddOneLine(valobj);

       // nl->setGeometry(oldrect);
        // 这里一定是Layout 嵌套了.
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            nl->readFromJson(item,this->mCreateFlag);
        }
        m_frame->adjustSize();
    }

}


NewLayout::NewLayout(QString caption, QSize nsize,
                     MainWindow *w, QWidget *parent):
   BaseForm(parent)
{
    mType = TYPELAYOUT;
    this->mWindow = w;

    resize(nsize);
//    qDebug() << "this layout size " << size()
//             <<" parent size " << parent->size();
    if(parent)
        setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.

    setFocusPolicy(Qt::ClickFocus);
    setProperty(DKEY_INTOCONTAINER,false);
    show();
    update();

    QString uname = QString("%1_%2").arg(caption,
                                         QString::number(mWindow->ComCtrl->ProMap.size()));
   // setProperty(DKEY_LOCALSEQ,uname );
    mUniqueStr = uname ;
    setProperty(DKEY_TXT,caption);
    setObjectName(uname);
   // updateStyleSheets();
    setToolTip(uname);
}



void NewLayout::DeleteMe()
{

    // 它是否是列表控件的一员.

    if(property(DKEY_INTOCONTAINER).toBool())
    {

        if(!CN_NEWLIST.compare(metaObject()->className()))
            ((NewList*)container)->childlist.removeOne(this);
        else{
           ((NewGrid*)container)->childlist.removeOne(this);
        }
    }
    this->BaseForm::DeleteMe();

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
    QString widgetdir = mWindow->mGlobalSet->value(INI_PRJCUSTOM).toString();
    if(widgetdir.isEmpty()) widgetdir = QDir::currentPath() + BACKSLASH + "widgets";

    QDir wdir(widgetdir );
    if(!wdir.exists(widgetdir))
        wdir.mkdir(widgetdir);
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
        fpath = widgetdir + BACKSLASH + name;

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
    QJsonObject json = mOwerJson;
    foreach (QWidget *w, childlist) {
        QJsonObject outjson = QJsonValue::fromVariant( w->property(DKEY_JSONSTR)).toObject();
       // outjson[NAME] = w->objectName();
       // qDebug() << " childlist  " << w->metaObject()->className();
        // 这里只有两种对像类型,NewLayout , NewFrame;
        QString clsname = w->metaObject()->className();
        outjson = ((BaseForm*)w)->writeToJson();
        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LAYOUT] = layoutarr;
    json[NAME] = objectName();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}


void NewLayout::readFromJson(const QJsonValue &qv,bool flag)
{
    QJsonObject valobj = qv.toObject();
    // QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    // 在NewLayout　类上面可以创建包括它自已在内的三种类型的对像.
    QString clsName = valobj[CLASS].toString();
    if(!clsName.compare(CN_NEWLAYOUT)/* || !clsName.compare(CN_LAYOUT)*/)
    {

        NewLayout *newlayout = CreateNewLayout(qv,m_frame,flag);
        childlist.append(newlayout);
        // 这里一定是Layout 嵌套了.
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            newlayout->readFromJson(item,flag);
        }
    }else if(!clsName.compare(CN_NEWLIST))
    {
      NewList *nlist = new NewList(qv,QSize(10,10),m_frame);
      nlist->mCreateFlag = flag;
      nlist->mOwerJson = qv.toObject();
      mWindow->tree->addObjectToCurrentItem(mUniqueStr,nlist);
      childlist.append(nlist);
      nlist->onSelectMe();
      foreach (QJsonValue item, valobj[LISTWIDGET].toArray()) {
          nlist->readFromJson(item);
      }
    }else if(!clsName.compare(CN_NEWGRID))
    {
        NewGrid *ngrid;
         if(flag)
         {
            QList<int> arglist =   rowcoldialog();
            ngrid = new NewGrid(qv,&arglist,
                                      m_frame);
         }else{
            QList<int> arglist ;
            arglist << 0 << 0 << 0 <<0;
            ngrid = new NewGrid(qv,&arglist,
                                      m_frame);
            int cols = valobj[GCOLS].toInt();
            int rows = valobj[GROWS].toInt();
            ngrid->rows = rows;
            ngrid->cols = cols;
            ngrid->itemSize.scale(QSize(valobj[WIDTH].toInt(),
                                        valobj[HEIGHT].toInt()),
                                        Qt::IgnoreAspectRatio);
         }
         ngrid->mCreateFlag = flag;
         mWindow->tree->addObjectToCurrentItem(mUniqueStr,ngrid);
         childlist.append(ngrid);
         if(flag)
            ngrid->addRowsCols();
         else{
             foreach (QJsonValue item, valobj[LISTWIDGET].toArray()) {
                 ngrid->readFromJson(item);
             }
             ngrid->updateAllItemsSize();
         }
    }
    else if(!clsName.compare(CN_NEWFRAME)
             /*|| !clsName.compare(QFRAME)*/)
    {
        this->createObjectFromJson(qv);
    }

}


QList<int> NewLayout::rowcoldialog()
{
    QList<int> arglist;
    int r,c,w,h = 0;
    while(1)
    {

        BaseDialog *nWindow = new BaseDialog();
        nWindow->setObjectName(metaObject()->className());
        nWindow->UpdateStyle();
        QVBoxLayout *vb = new QVBoxLayout();

        QHBoxLayout *rowlayout = new QHBoxLayout();

        QLabel *rowtitle = new QLabel("行:");
        QSpinBox *rowbox = new QSpinBox();
        //rowbox->setMaximum(max);
        QString tooltip = "请输入行列整数,必需满足 (行*列>0)";
        rowbox->setToolTip(tooltip);
        rowbox->setValue(2);

        rowlayout->addWidget(rowtitle);
        rowlayout->addWidget(rowbox);

        QLabel *coltitle = new QLabel("列:");
        QSpinBox *colbox = new QSpinBox();
        colbox->setToolTip(tooltip);
        colbox->setValue(2);

        QHBoxLayout *collayout = new QHBoxLayout();
        collayout->addWidget(coltitle);
        collayout->addWidget(colbox);

        vb->addLayout(rowlayout);
        vb->addLayout(collayout);


        QLabel *itemwd = new QLabel("单元宽度:");
        QLabel *itemht = new QLabel("单元高度:");
        QString whstr = "每个单元的大小";
        QSpinBox *itemwBox = new QSpinBox();
        itemwBox->setValue(30);
        QSpinBox *itemhBox = new QSpinBox();
        itemhBox->setValue(30);
        itemwBox->setToolTip(whstr);
        itemhBox->setToolTip(whstr);
        QHBoxLayout *wdlayout = new QHBoxLayout();
        wdlayout->addWidget(itemwd);
        wdlayout->addWidget(itemwBox);

        QHBoxLayout *htlayout = new QHBoxLayout();
        htlayout->addWidget(itemht);
        htlayout->addWidget(itemhBox);

        vb->addLayout(wdlayout);
        vb->addLayout(htlayout);

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
        {
            r = rowbox->value();
            c = colbox->value();
            w = itemwBox->value();
            h = itemhBox->value();
            nWindow->deleteLater();
            if(!r || !c || !w || !h )
            {
                continue;
            }
            arglist << r << c << w <<h;
            break;
        }
    }
   return arglist;
}

QWidget* NewLayout::createObjectFromJson(const QJsonValue &qv)
{
    QJsonObject json = qv.toObject();
    QString caption = json[CAPTION].toString();
    NewFrame *newFrame = new NewFrame(caption,m_frame);
   // newFrame->setProperty(DKEY_JSONSTR,qv);
    newFrame->mOwerJson = qv.toObject();
    newFrame->setProperty(DKEY_TYPE,json[WTYPE].toString());
    newFrame->setProperty(DKEY_TXT,caption);
    childlist.append(newFrame);
    mWindow->tree->addObjectToCurrentItem(mUniqueStr,newFrame);

    QJsonObject obj = getValueFromProperty(json[PROPERTY].toArray(),BAKIMAGE);
    if(!obj.isEmpty() && obj.contains(BAKIMAGE))
    {
       // QPixmap p;
        QString path = obj[BAKIMAGE].toString();
//        if(path.contains("\\"))
//        {
//            path.replace("\\","/");
//        }
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
    mType = TYPELAYER;
    mWindow = ((ScenesScreen*)parent)->mWindow;
    resize(nsize);
    setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.

    setFocusPolicy(Qt::ClickFocus);
    show();
    QString key =QString("%1_%2").arg(caption,QString::number( mWindow->ComCtrl->ProMap.size()));
    mUniqueStr = key;
    //setProperty(DKEY_LOCALSEQ,key);
    setProperty(DKEY_TXT,caption);
    setObjectName(key);
   // updateStyleSheets();
    setToolTip(key);
}



void NewLayer::readLayoutFromJson(const QJsonValue &qv,bool flag)
{

   QJsonObject  valobj = qv.toObject();
   // QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
   // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
   QString clsName = valobj[CLASS].toString();
   if(!clsName.compare(CN_NEWLAYOUT) /*|| !clsName.compare(CN_LAYOUT)*/)
   {
        NewLayout *newlayout = CreateNewLayout(qv,m_frame,flag);
       // 这里一定是Layout 嵌套了.
        childlist.append(newlayout);
       foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
           //这里只有读取工程与读取自定义控件才会到这里,要做区分.
           newlayout->readFromJson(item,flag);
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


QJsonObject  NewLayer::writeToJson()
{
    QJsonArray layoutarr;
    QJsonObject json = mOwerJson;
    foreach (QWidget *w, childlist) {
        layoutarr.append(((NewLayout*)w)->writeToJson());
    }
    json[NAME] = objectName();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[LAYOUT] = layoutarr;
    json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}






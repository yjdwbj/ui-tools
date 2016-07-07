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




BaseForm::BaseForm(QWidget *parent)
    :FormResizer(parent)
{


}

void BaseForm::onSelectMe()
{
//    clearOtherObjectStyleSheet();
//    mWindow->cManager->activeSS()->setSelectObject(this);
//    mWindow->posWidget->setConnectNewQWidget(this);
//    mWindow->propertyWidget->createPropertyBox(this);
//    mWindow->imgPropertyWidget->delPropertyBox();
//    this->blockSignals(true);

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
        QAction delme(QString("删除当前-%1").arg(this->property(DKEY_CAPTION).toString()),this);
        connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));
        QAction saveTemp("保存成控件",this);
        connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));
        contextMenu->addAction(&delme);
        contextMenu->addAction(&saveTemp);
        contextMenu->exec(mapToGlobal(event->pos()));
    }

}

void BaseForm::mouseReleaseEvent(QMouseEvent *event)
{

}

void BaseForm::paintEvent(QPaintEvent *)
{

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


NewLayout::NewLayout(QSize nsize,QWidget *parent):
    FormResizer(parent)
{
    setMinimumSize(nsize ); // 最小尺寸
    setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.
    this->setObjectName(this->metaObject()->className());
    setStyleSheet("NewLayout#%s{border: 0.5px solid #929292;}" % LAYOUT);
    setFocusPolicy(Qt::ClickFocus);

//    // 使用调色板来改顔色
//    QPalette Pal(m_frame->palette());

//    // set black background
//    Pal.setColor(QPalette::Background, "#DDE4A7");
//    m_frame->setAutoFillBackground(true);
//    m_frame->setPalette(Pal);
//    m_frame->update();
      m_frame->setStyleSheet("background-color: #DAE4A7;");

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
    QListIterator<QWidget*> nf(NewFrameList);
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
        QAction saveTemp("保存成控件",this);
        connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));
        contextMenu->addAction(&delme);
        contextMenu->addAction(&saveTemp);
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

void NewLayout::onBeComeTemplateWidget()
{

//    QDialog namedig(this);
//    QHBoxLayout *hb = new QHBoxLayout();
//    hb->addWidget(new QLabel("请输入控件名称:"));
//    QLineEdit *txt = new QLineEdit();
//    txt->setToolTip("最大四个中文字符");
//    txt->setMaxLength(4);
//    txt->setText(QString("Wid%1").arg(QString::number(mWindow->ComCtrl->mCWidgetCount)));
//    hb->addWidget(txt);
//    namedig.setLayout(hb);
//    namedig.exec();

   QString fpath =  QFileDialog::getSaveFileName(this,"保存成自定义控件,不要保存到目录,否则无法加载它.",QDir::currentPath() + "/widgets",
            "Json files (*.json);;All files (*.*)",
                new QString("Json files (*.json)"),QFileDialog::ShowDirsOnly|QFileDialog::ReadOnly);
   qDebug() << " save file " << fpath <<" has json "  << fpath.lastIndexOf("json");
   if(fpath.isEmpty())
        return;
   if(fpath.lastIndexOf("json") == -1)
       fpath.append(".json");

//   qDebug() << " save path " << fpath;


    QJsonObject json ;
    QJsonArray arr ;
    QJsonObject wid;


    writeToJson(wid);
    arr.append(wid);
    json[COMPOENTS] = arr;

    QFile saveFile;
    saveFile.setFileName(fpath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");

    }
    QJsonDocument jsonDoc(json);
    saveFile.write(jsonDoc.toJson());

    // 加载到当前工程里面去.




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
    QWidget *w = NewFrameList.at(index);
    NewFrameList.removeAt(index);
     ((NewFrame*)w)->delMySelf();
}

void NewLayout::deleteObject(QWidget *w)
{
    int i = NewFrameList.indexOf(w);
    NewFrameList.removeAt(i);
    ((NewFrame*)w)->delMySelf();

}


void NewLayout::writeToJson(QJsonObject &json)
{

        QJsonArray layoutarr;
      //  qDebug() << "NewLayout  parent object " << &json;

        foreach (QWidget *w, NewFrameList) {
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


//按照JSON创建一个控件.
void NewLayout::createNewFrameObject(const QJsonObject &json)
{

//    for(QJsonObject::const_iterator it = json.constBegin();it != json.constEnd();++it)
//    {
//        QString key = it.key();
//        qDebug() << " QJsonObject iterator key " << key << " value " << it.value();
//        qDebug() << " value type " << it.value().type();
//    }
    NewFrame* ww = (NewFrame *)CreateObjectFromJson(json,m_frame);

   // NewFrame* ww = (NewFrame *)CreateObjectFromJson(json.toVariantMap(),m_frame);
    QString objname = json[NAME].toString();
    QString txt = json[CAPTION].toString();
    //mWindow->Scenes->activeLayer()->m_frame);

    ww->setObjectName(objname);
    //ww->setObjectName(QString("%1_%2").arg(btn->text(),QString::number(comList.size())));
    ww->setProperty(DKEY_LOCALSEQ,QString("%1_%2").arg(txt,QString::number(NewFrameList.size())));

  //  ProMap[ww->property(DKEY_LOCALSEQ).toString()] = ww;
    NewFrameList.append(ww);

    // 找出图层,把新建的控件添加进去.
    mWindow->tree->addObjectToLayout(ww);
    ww->show();
}
QWidget* NewLayout::CreateObjectFromJson(const QJsonObject &json,QWidget *pobj)
{
    QWidget *nobj;
    QVariant property;
    for(QJsonObject::const_iterator it = json.constBegin();it != json.constEnd();++it)
    {
        QString key = it.key();
        switch (it.value().type()) {
        case QJsonValue::String:
            if(!key.compare(CLASS))
            {
                QString cval = it.value().toString();

                if(!cval.compare(QFRAME) || !cval.compare(CN_NEWFRAME))
                {

                    //创建父控件
                    NewFrame  *n = new NewFrame((QWidget*)pobj);
                    //  n->addMainWindow(pobj->parent());
                    if(json.contains(PROPERTY))
                    {
                        // QJsonObject vp =QJsonObject::fromVariantMap(qvm[PROPERTY]);
                        n->copyProperty(json[PROPERTY]);
                    }
                    nobj =(QWidget*)(n);
                  //  nobj->setProperty(DKEY_CLSNAME,cval);
                }
                else if(!cval.compare(QLABEL) || !cval.compare(CN_NEWLABEL))
                {
                    NewFrame *np = (NewFrame*)pobj;
                    nobj =(QWidget*)(new NewLabel(np->m_frame));
                    if(json.contains(PROPERTY))
                    {

                        ((NewLabel*)nobj)->copyProperty(json[PROPERTY]);
                        // qDebug() << " NewLabel dyn property " << ((NewLabel*)nobj)->dynValues;
                    }

                }
            }
            else if(!key.compare(NAME))
            {
                nobj->setObjectName(it.value().toString());
            }
            else if(!key.compare(CAPTION)) /* 界面显示的名称 */
            {
                nobj->setProperty(DKEY_CAPTION,it.value().toString());
            }
            break;
        case QJsonValue::Array:
            if(!key.compare(PROPERTY))
            {
                // nobj->setProperty("dynProperty",it.value());
                property = it.value().toVariant();
            }
            else if(!key.compare(WIDGET)) // 处理它的子控件.
            {
                foreach(QJsonValue val, it.value().toArray())
                {
                    // qDebug() << qv.type();
                    if(val.type() == QJsonValue::Object)
                    {
                        qDebug() << " why running here. ";
                        QWidget *cobj = CreateObjectFromJson(val.toObject(),nobj);
                       // chlist.append(cobj->objectName());
                    }

                }
            }
            break;
        default:
            break;
        }

    }

    // 下面只处理控件的坐标与控件中的图片.
    QJsonArray array ;
    QString clsName = nobj->metaObject()->className();
    bool isFrame = !clsName.compare(CN_NEWFRAME) ;// 二元判断,不是控件就是图片.
    if(isFrame)
    {
        array = ((NewFrame*)nobj)->dynValues[DKEY_DYN].toArray();
    }else
    {

        array = ((NewLabel*)nobj)->dynValues[DKEY_DYN].toArray();
    }
    foreach(QJsonValue val, array)
    {
         qDebug()  << " Property Val List :" << val.type()
                   << " json value " << val;
        switch (val.type()) {
        case QJsonValue::String:
            break;
        case QJsonValue::Array:

            break;
        case QJsonValue::Object:
        {
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
                ((NewLabel *)nobj)->setPixmap(p);
                ((NewLabel *)nobj)->setFixedSize(p.size());
            }
            else if(obj.contains(KEY_RECT))
            {
                //  QString clsName = nobj->property(DKEY_CLSNAME).toString();
                QVariantMap rect = obj[KEY_RECT].toObject().toVariantMap();
                QRect r = QRect(rect["x"].toString().toInt(),
                        rect["y"].toString().toInt(),
                        rect["width"].toString().toInt(),
                        rect["height"].toString().toInt());

                if(isFrame)
                {

                    qobject_cast<NewFrame *>(nobj)->setGeometry(r);
                    ((NewFrame*)nobj)->m_frame->setGeometry(r);
                    ((NewFrame*)nobj)->updateGeometry();

                    // 外面８个点的实际位置.
                    ((NewFrame*)nobj)->setFixedSize(r.size()+MARGIN_SIZE);
                    // ((NewFrame*)nobj)->setFixedSize(r.size());
                }else
                {
                    ((NewLabel *)nobj)->setGeometry(r);
                }

            }
            else if(obj.contains(LIST))
            {

            }
        }
            break;
        default:
            break;
        }
    }
//        if(val.type() == QJsonValue::Array)
//        {
//            QVariantMap qvm = qv.toMap();

//            for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
//            {
//                QString key = it.key();
//                if(!key.compare(KEY_RECT)) /* 这里直接处理json "rect" 对像字段 */
//                {
//                    //  QString clsName = nobj->property(DKEY_CLSNAME).toString();
//                    QVariantMap rect = it.value().toMap();
//                    QRect r = QRect(rect["x"].toString().toInt(),
//                            rect["y"].toString().toInt(),
//                            rect["width"].toString().toInt(),
//                            rect["height"].toString().toInt());

//                    //if(!clsName.compare(QFRAME))
//                    if(!CN_NEWFRAME.compare(nobj->metaObject()->className()))
//                    {

//                        qobject_cast<NewFrame *>(nobj)->setGeometry(r);
//                        ((NewFrame*)nobj)->m_frame->setGeometry(r);
//                        ((NewFrame*)nobj)->updateGeometry();

//                        // 外面８个点的实际位置.
//                        ((NewFrame*)nobj)->setFixedSize(r.size()+MARGIN_SIZE);
//                        // ((NewFrame*)nobj)->setFixedSize(r.size());

//                    }else{
//                        qobject_cast<NewLabel *>(nobj)->setGeometry(r);
//                        //  qDebug() << " label geometry " << qobject_cast<NewLabel *>(nobj)->geometry();
//                    }
//                }
//                else if(!key.compare(IMAGE)) // 这里中处理了这一个图片属性.
//                {
//                    QPixmap p;
//                    QString path = it.value().toString();
//                    if(path.contains("\\"))
//                    {
//                        path.replace("\\","/");
//                    }
//                    p.load(path);
//                    qobject_cast<NewLabel *>(nobj)->setPixmap(p);
//                    qobject_cast<NewLabel *>(nobj)->setFixedSize(p.size());
//                }
//                else {

//                    //  qDebug() << "other property Key : " << key  << " Val : " << it.value();
//                }

//            }
//        }





    return nobj;
}

QWidget* NewLayout::CreateObjectFromJson(const QVariantMap &qvm,QWidget *pobj)
{
    QWidget *nobj;
    QVariant property;

    for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
    {
        QString key = it.key();
        // qDebug() << "json key is " << key;
        QVariant::Type qvt = it.value().type();


        switch (qvt) {
        case QVariant::String:
            if(!key.compare(CLASS))
            {
                QString cval = it.value().toString();

                if(!cval.compare(QFRAME) || !cval.compare(CN_NEWFRAME))
                {

                    //创建父控件
                    NewFrame  *n = new NewFrame((QWidget*)pobj);
                    //  n->addMainWindow(pobj->parent());
                    if(qvm.contains(PROPERTY))
                    {
                        // QJsonObject vp =QJsonObject::fromVariantMap(qvm[PROPERTY]);
                        n->copyProperty(qvm[PROPERTY]);
                    }
                    nobj =(QWidget*)(n);
                  //  nobj->setProperty(DKEY_CLSNAME,cval);
                }
                else if(!cval.compare(QLABEL) || !cval.compare(CN_NEWLABEL))
                {
                    NewFrame *np = (NewFrame*)pobj;
                    nobj =(QWidget*)(new NewLabel(np->m_frame));
                    if(qvm.contains(PROPERTY))
                    {

                        ((NewLabel*)nobj)->copyProperty(qvm[PROPERTY]);
                        // qDebug() << " NewLabel dyn property " << ((NewLabel*)nobj)->dynValues;
                    }

                }
            }
            else if(!key.compare(NAME))
            {
                nobj->setObjectName(it.value().toString());
            }
            else if(!key.compare(CAPTION)) /* 界面显示的名称 */
            {
                nobj->setProperty(DKEY_CAPTION,it.value().toString());
            }
            break;
        case QVariant::List:
        {
            if(!key.compare(PROPERTY))
            {
                // nobj->setProperty("dynProperty",it.value());
                property = it.value();
            }
            else if(!key.compare(WIDGET)) // 处理它的子控件.
            {


            }else {
                QVariantList qvl = it.value().toList();
               // QStringList chlist;
                foreach(QVariant qv, qvl)
                {
                    // qDebug() << qv.type();
                    if(qv.type() == QVariant::Map)
                    {
                        qDebug() << " why running here. ";
                        QWidget *cobj = CreateObjectFromJson(qv.toMap(),nobj);
                       // chlist.append(cobj->objectName());
                    }

                }

                //pobj->setProperty("chlist",chlist);

            }
        }
            break;
        case QVariant::Map:
            //  CreateObjectFromJson(it.value().toMap());
            // qDebug() << " type is Map " << it.value().toMap();
            break;
        case QVariant::Double:
            //qDebug()  << " Value is Int : " << it.value().toInt();
            break;
        default:
            break;
        }
    }
    nobj->setProperty(DKEY_DYN,property);
   // nobj->setProperty(DKEY_UID,QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));

    /*  处理每一个json对像的property部分 */
    // qDebug() << "Dynamic Property Count " << nobj->dynamicPropertyNames().count();
    foreach(QByteArray qba,nobj->dynamicPropertyNames())
    {
        QVariantList qvl = nobj->property(qba).toList();
        foreach(QVariant qv, qvl)
        {
            // qDebug()  << " Property Val List :" << qv.type();
            if(qv.type() == QVariant::Map)
            {
                QVariantMap qvm = qv.toMap();

                for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
                {
                    QString key = it.key();
                    if(!key.compare(KEY_RECT)) /* 这里直接处理json "rect" 对像字段 */
                    {
                      //  QString clsName = nobj->property(DKEY_CLSNAME).toString();
                        QVariantMap rect = it.value().toMap();
                        QRect r = QRect(rect["x"].toString().toInt(),
                                rect["y"].toString().toInt(),
                                rect["width"].toString().toInt(),
                                rect["height"].toString().toInt());

                        //if(!clsName.compare(QFRAME))
                        if(!CN_NEWFRAME.compare(nobj->metaObject()->className()))
                        {

                            qobject_cast<NewFrame *>(nobj)->setGeometry(r);
                            ((NewFrame*)nobj)->m_frame->setGeometry(r);
                            ((NewFrame*)nobj)->updateGeometry();

                            // 外面８个点的实际位置.
                           ((NewFrame*)nobj)->setFixedSize(r.size()+MARGIN_SIZE);
                           // ((NewFrame*)nobj)->setFixedSize(r.size());

                        }else{
                            qobject_cast<NewLabel *>(nobj)->setGeometry(r);
                            //  qDebug() << " label geometry " << qobject_cast<NewLabel *>(nobj)->geometry();
                        }
                    }
                    else if(!key.compare(IMAGE)) // 这里中处理了这一个图片属性.
                    {
                        QPixmap p;
                        QString path = it.value().toString();
                        if(path.contains("\\"))
                        {
                            path.replace("\\","/");
                        }
                        p.load(path);
                        qobject_cast<NewLabel *>(nobj)->setPixmap(p);
                        qobject_cast<NewLabel *>(nobj)->setFixedSize(p.size());
                    }
                    else {

                        //  qDebug() << "other property Key : " << key  << " Val : " << it.value();
                    }

                }
            }
        }

    }

    return nobj;
}


NewLayer::NewLayer(QSize nsize, QWidget *parent)
    :BaseForm(parent),
      mActiveIdx(-1)

{

    mWindow = ((ScenesScreen*)parent)->mWindow;
    setMinimumSize(nsize ); // 最小尺寸
    setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.
    this->setObjectName(this->metaObject()->className());
    setStyleSheet("NewLayout#%s{border: 0.5px solid #929292;}" % LAYER);
    setFocusPolicy(Qt::ClickFocus);
     m_frame->setStyleSheet("background-color: #C0DCC0;");
    show();


}

void NewLayer::createNewLayout()
{
    // 在些新建一个布局为当前的子控件.
    NewLayout *nl = new NewLayout(QSize(150,200)+MARGIN_SIZE,this);
    nl->addMainWindow(mWindow);
    LayoutList.append(nl);
    mActiveIdx = LayoutList.size() - 1;
    //nl->setObjectName(QString("%1_%2").arg(nl->metaObject()->className(),QString::number(mActiveIdx)));
    nl->setProperty(DKEY_LOCALSEQ,QString("%1_%2").arg("布局",
                    QString::number(mActiveIdx)));

    nl->onSelectMe();

}


void NewLayer::onSelectMe()
{

    //clearOtherObjectStyleSheet();
    mWindow->cManager->activeSS()->setSelectObject(this);
    mWindow->posWidget->setConnectNewQWidget(this);
    mWindow->propertyWidget->createPropertyBox(this);
    mWindow->imgPropertyWidget->delPropertyBox();
    this->blockSignals(true);
}

void NewLayer::clearOtherObjectStyleSheet()
{

}


void NewLayer::DeleteMe()
{
     mWindow->cManager->activeSS()->delSelectedLayer();
}

void NewLayer::onDeleteMe()
{
    DeleteMe();
}

//void NewLayer::mousePressEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::LeftButton)
//    {
//        mOffset = event->pos();
//        onSelectMe();
//    }else if(event->button() == Qt::RightButton)
//    {
//        QMenu *contextMenu = new QMenu(this);
//        QAction delme("删除当前图层",this);
//        connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));
//        contextMenu->addAction(&delme);

//        contextMenu->exec(mapToGlobal(event->pos()));
//    }

//}
//void NewLayer::mouseMoveEvent(QMouseEvent *event)
//{
//    if (event->buttons() & Qt::LeftButton)
//    {
//        move(this->pos() + (event->pos() - mOffset));
//        /* 把新的位置更新到右边属性框 */
//        mWindow->posWidget->updatePosition(this->pos());
//        this->blockSignals(true);
//    }
//}

//void NewLayer::mouseReleaseEvent(QMouseEvent *event)
//{
//    /* 放开鼠标时检查它的是否出了边界要 */
//    QWidget *p = this->parentWidget();
//    QPoint pos = this->pos();
//    if(this->x() < 0)
//    {
//        pos.setX(0 - MARGIN_SIZE.width() /2);
//        this->move(pos);

//    }
//    if(this->y() < 0 )
//    {
//        pos.setY(0 -MARGIN_SIZE.width() /2);
//        this->move(pos);
//    }

//    QSize ms = p->size();
//    //左出界检查
//    if((this->x()  + this->size().width()) > ms.width())
//    {
//        pos.setX( ms.width() - this->size().width() + MARGIN_SIZE.width() /2 );
//        this->move(pos);

//    }

//    //上出界检查
//    if((this->y() + this->size().height()) > ms.height())
//    {
//        pos.setY(ms.height() - this->size().height() + MARGIN_SIZE.height() /2);
//        this->move(pos);
//    }

//    // 这里只能在释放鼠标时改变左边的控件值
//    mWindow->posWidget->updateSize(this->size());

//}



//void NewLayer::clearOtherObjectStyleSheet()
//{
//    /* 清除控件的红线框 */
//    QList<NewLayout*> nflist =  m_frame->findChildren<NewLayout*>();
//    foreach(NewLayout *x,nflist)
//    {
//           x->setStyleSheet("");

//    }

//}

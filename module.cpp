#include "module.h"
#include <QtCore/QMetaProperty>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStackedLayout>

#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSpacerItem>

#include <QApplication>
#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSignalMapper>

#include "config.h"
#include "scenesscreen.h"
#include "canvasmanager.h"


 void Compoent::onBindValue(QWidget *w, const QVariant &val)
{
 //   qDebug() << " dyn size " << dynValues.size();
    QString n = w->metaObject()->className();
    QString uname = w->objectName();
    if(!n.compare("QTextEdit"))
    {
        if(dynValues.contains(uname))
        {
            QTextEdit *txt = (QTextEdit *)w;
            txt->setText(dynValues[uname].toString());
        }else
        {
            dynValues[uname] = val;
        }
    }else if(!n.compare("QComboBox"))
    {
       // qDebug() << " dynValues addr " << dynValues << &dynValues;
        if(dynValues.contains(uname))
        {
            QComboBox *cb = (QComboBox *)w;
            cb->setCurrentText(dynValues[uname].toString());

        }else
        {
            dynValues[uname] = val;
        }

    }else if(!n.compare("QSpinBox"))
    {

         if(dynValues.contains(uname))
         {
             QSpinBox *sp = (QSpinBox *)w;
             sp->setValue(dynValues[uname].toDouble());
         }else
         {
             dynValues[uname] = val;
         }
    }
}



void NewLabel::onEnumItemChanged(QString txt)
{
   QComboBox *cb =(QComboBox *)(QObject::sender());
  // qDebug() << " Label enum value changed " << txt << cb->currentText();
   dynValues[cb->objectName()] = txt;

}

void NewLabel::onNumberChanged(int num)
{

    QSpinBox *sp = (QSpinBox *)(QObject::sender());
    dynValues[sp->objectName()] = num;
}

void NewLabel::onTextChanged()
{
    QTextEdit *txt = (QTextEdit *)(QObject::sender());
    dynValues[txt->objectName()] = txt->toPlainText();
}

/*------------------------------------------------------------------------*/

void NewFrame::onEnumItemChanged(QString txt)
{
   QComboBox *cb =(QComboBox *)(QObject::sender());
 //  qDebug() << " Frame enum value changed " << txt << cb->currentText();
   dynValues[cb->objectName()] = txt;
  // qDebug() << " dynValues addr " << dynValues << &dynValues;

    this->signalsBlocked();
}

void NewFrame::onNumberChanged(int num)
{

    QSpinBox *sp = (QSpinBox *)(QObject::sender());
    dynValues[sp->objectName()] = num;
}

void NewFrame::onTextChanged()
{
    QTextEdit *txt = (QTextEdit *)(QObject::sender());
  //  qDebug() << " txt changed" << txt->toPlainText();
    dynValues[txt->objectName()] = txt->toPlainText();
}



//void NewLabel::onBindValue(QWidget *w, const QVariant &val)
//{
//    Compoent::onBindValue(w,val);
//}

//void NewFrame::onBindValue(QWidget *w, const QVariant &val)
//{
//    Compoent::onBindValue(w,val);
//}

NewLabel::NewLabel(QWidget *parent)
    :QLabel(parent),
     selIndex(0),disDefaultList(false)
{

    // MainWindow *m ;
    QWidgetList tlist = qApp->topLevelWidgets();
    for(QWidgetList::iterator wit = tlist.begin();wit != tlist.end();++wit)
    {
        if((*wit)->objectName() == "MainWindow")
        {
            mWindow = (MainWindow*)(*wit);
            break;
        }
    }
    this->setLineWidth(0);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    // connect(this,SIGNAL(Clicked()),SLOT(onClieck()));
}

//QGroupBox* NewLabel::CreateXYWHGBox(QWidget *p)
//{
//    QGridLayout *xywh = new QGridLayout();
//    xywh->setObjectName("xywhGrid");

//    QMap<QString,int> xymap;
//    xymap[X] = p->geometry().x();
//    xymap[Y] = p->geometry().y();
//    xymap[W] = p->geometry().width();
//    xymap[H] = p->geometry().height();

//    int index = 0;


//    for(QMap<QString,int>::iterator it = xymap.begin();it != xymap.end();++it)
//    {
//        QLabel *s = new QLabel(it.key());
//        s->setFixedWidth(35);
//        xywh->addWidget(s,index,0);
//        QSpinBox *xedit = new QSpinBox();
//        xedit->setFixedWidth(40);

//        xedit->setObjectName(it.key());
//        xedit->setMaximum(1000);
//        xedit->setValue(it.value());
//        xywh->addWidget(xedit,index,1);
//        connect(xedit,SIGNAL(valueChanged(int)),p,SLOT(onXYWHChangedValue(int)));
//        index++;
//    }

//    QGroupBox *xygb = new QGroupBox(tr("坐标位置"));
//    //xywh->setSizeConstraint(QLayout::SetFixedSize);
//    xywh->setContentsMargins(2,10,2,20);
//    xygb->setObjectName("xygb");
//    xygb->setLayout(xywh);
//    return xygb;
//}


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
       // o.setY(v);
     //   QPoint p = sender->pos();
      //  p.setY(v);
       // sender->move(p);
        QPoint pos = p->pos();
        pos.setY(v);
        p->move(pos );

    }
//    else if(!sender->objectName().compare(W))
//    {

//    }else if(!sender->objectName().compare(H))
//    {

//    }


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

//            QList<NewLabel*> list =  x->findChildren<NewLabel*>();
//            QListIterator<NewLabel*> it(list);
//            while(it.hasNext())
//            {
//                NewLabel *nl = it.next();
//                nl->setStyleSheet("");
//            }
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
    mWindow->propertyWidget->createPropertyBox(p);


     QTextEdit *et = (QTextEdit *)(getQWidgetByName("debugEdit"));
     if(et != 0 )
     {
         et->setText(QString("mouse x:%1 , y:%2 ")
                     .arg(QString::number(ev->pos().rx()))
                     .arg(QString::number(ev->pos().ry())));
     }
  //  ev->accept();
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
        pos.setX( ms.width() - p->size().width() );
        p->move(pos);
    }

    if((p->y() + p->size().height()) > ms.height())
    {
        pos.setY(ms.height() - p->size().height());
        p->move(pos);
    }
    //qDebug() << " parent Widget pos " << p->pos() << "size : " << p->size() << " geomerty " << p->geometry();

    //qDebug() << " new pos from Frame : " << p->pos();
}

void NewLabel::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {
        NewFrame *p =(NewFrame*) (this->parentWidget()->parentWidget());

        p->move( p->pos() + (event->pos() - mOffset));


        /* 把新的位置更新到右边属性框 */

        QPoint nr = p->pos();
        foreach (QWidget *w, mWindow->propertyWidget->findChildren<QWidget*>()) {
           if(!w->objectName().compare(X))
           {
               QSpinBox *s = (QSpinBox*)w;

               s->setValue(nr.x());
           }
           else if(!w->objectName().compare(Y))
           {
               QSpinBox *s = (QSpinBox*)w;

               s->setValue(nr.y());
           }

       }


    }
    // event->accept();
    //  this->parentWidget()->update();
   // qDebug() << " new pos from Layer : "  << pos() ;
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
    qDebug() << " Frame pos is " << p->pos();

}


void NewLabel::onListImageChanged(QString img)
{
   //  selectedMap sMap = this->property(IMAGELST).toMap();
  // QStringList selList = this->property(DKEY_IMAGELST).toStringList();
   foreach (QString s, myImageList) {
       QString k = s.section(":",0,0);
       if(!k.compare(img))
       {
           this->setPixmap(QPixmap(s.section(":",1,1)));/* 更新图片 */
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
    // QMessageBox::warning(this,"test","your clicked me: ");
    ImageFileDialog *ifd = new ImageFileDialog(myImageList,this);

    ifd->exec();
    //selectedMap sMap  = ifd->getSelectedMap();
    myImageList = ifd->getSelectedList();

    disDefaultList = myImageList.size() ? true : false;
    ifd->deleteLater();
    QComboBox *cb=0;
    foreach (QWidget *w, mWindow->imgPropertyWidget->findChildren<QWidget*>())
    {
        if(!w->objectName().compare(LISTIMAGE))
        {
          //  qDebug() << " found QComobox " << w->objectName();
            cb = (QComboBox *)w;
            cb->clear();
            break;
        }
    }
    updateComboItems(cb);

}

void NewLabel::updateComboItems(QComboBox *cb)
{
    foreach (QString s, myImageList) {
       cb->addItem(s.section(":",0,0));
    }
    cb->setCurrentIndex(this->property(DKEY_IMGIDX).toInt());
}




NewFrame::NewFrame(QWidget *parent)
    :FormResizer(parent)
{

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
}

void NewFrame::onSelectMe()
{
  //  setStyleSheet("NewFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
    clearOtherObjectStyleSheet();
    mWindow->cManager->activeSS()->setSelectObject(this);
   // ((NewLayout*)this->parentWidget())->clearOtherSelectHandler();
    this->setState(SelectionHandleActive);
    mWindow->propertyWidget->createPropertyBox(this);
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

    }else if(!sender->objectName().compare(W))
    {

      //  sender->setFixedWidth(v);
    }else if(!sender->objectName().compare(H))
    {

      //  sender->setFixedHeight(v);
    }
    //sender->setGeometry(o);

}




NewLayout::NewLayout(QSize nsize,QWidget *parent):
    FormResizer(parent)
{
   // setFixedSize(nsize);
    setMinimumSize(nsize );
    setMaximumSize(parent->size());
    this->setObjectName(this->metaObject()->className());
    setFocusPolicy(Qt::ClickFocus);
    show();
}


void NewLayout::onSelectMe()
{
   // setStyleSheet("NewFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
    clearOtherObjectStyleSheet();
    mWindow->cManager->activeSS()->setSelectObject(this);
    mWindow->propertyWidget->createPropertyBox(this);
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
}


void NewLayout::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    mOffset = event->pos();
   // m_curSize = m_startSize = this->size();
   // setCursor(Qt::ClosedHandCursor);
    onSelectMe();
    //qDebug() << " Press the layout " << this->m_frame->pos();
}
void NewLayout::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(this->pos() + (event->pos() - mOffset));

        /* 把新的位置更新到右边属性框 */

        QPoint nr = this->pos();

      //  QSize ns = this->size();
        foreach (QWidget *w, mWindow->propertyWidget->findChildren<QWidget*>()) {
           if(!w->objectName().compare(X))
           {
               QSpinBox *s = (QSpinBox*)w;

               s->setValue(nr.x());
           }
           else if(!w->objectName().compare(Y))
           {
               QSpinBox *s = (QSpinBox*)w;

               s->setValue(nr.y());
           }
       }

    }

}

void NewLayout::mouseReleaseEvent(QMouseEvent *event)
{
    /* 放开鼠标时检查它的是否出了边界要 */
    QWidget *p = this->parentWidget();
    QPoint pos = this->pos();
    if(this->x() < 0)
    {
        pos.setX(0);
        this->move(pos);

    }
    if(this->y() < 0 )
    {
        pos.setY(0);
        this->move(pos);
    }

    QSize ms = p->size();
    //左出界检查
    if((this->x()  + this->size().width()) > ms.width())
    {
        pos.setX( ms.width() - this->size().width() );
        this->move(pos);
    }

    //上出界检查
    if((this->y() + this->size().height()) > ms.height())
    {
        pos.setY(ms.height() - this->size().height());
        this->move(pos);
    }

    // 这里只能在释放鼠标时改变左边的控件值
    foreach (QWidget *wid, mWindow->propertyWidget->findChildren<QWidget*>()) {
       if(!wid->objectName().compare(H))
       {
           QSpinBox *s = (QSpinBox*)wid;

           s->setValue(this->height());
       }
       else if(!wid->objectName().compare(W))
       {
           QSpinBox *s = (QSpinBox*)wid;

           s->setValue(this->width());
       }

   }

}

void NewLayout::clearOtherObjectStyleSheet()
{
    /* 清除控件的红线框 */
    QList<NewFrame*> nflist =  m_frame->findChildren<NewFrame*>();
    foreach(NewFrame *x,nflist)
    {

//            QList<NewLabel*> list =  x->findChildren<NewLabel*>();
//            QListIterator<NewLabel*> it(list);
//            while(it.hasNext())
//            {
//                NewLabel *nl = it.next();
//                nl->setStyleSheet("");
//            }
           x->setStyleSheet("");

    }

//    QWidgetList laylist = this->parentWidget()->findChildren<QWidget*>();
//    foreach (QWidget *wid, laylist) {

//        if (wid == this)
//            continue;
//        if(!CN_NEWFRAME.compare(wid->metaObject()->className())
//            || !CN_NewLayout.compare(wid->metaObject()->className()))
//        {
//            ((NewLayout*)wid)->setState(SelectionHandleOff);
//        }
//    }
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

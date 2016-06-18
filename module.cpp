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

    // connect(this,SIGNAL(Clicked()),SLOT(onClieck()));
}

QGroupBox* NewLabel::CreateXYWHGBox(QWidget *p)
{
    QGridLayout *xywh = new QGridLayout();
    xywh->setObjectName("xywhGrid");

    QMap<QString,int> xymap;
    xymap[X] = p->geometry().x();
    xymap[Y] = p->geometry().y();
    xymap[W] = p->geometry().width();
    xymap[H] = p->geometry().height();

    int index = 0;


    for(QMap<QString,int>::iterator it = xymap.begin();it != xymap.end();++it)
    {
        QLabel *s = new QLabel(it.key());
        s->setFixedWidth(35);
        xywh->addWidget(s,index,0);
        QSpinBox *xedit = new QSpinBox();
        xedit->setFixedWidth(40);

        xedit->setObjectName(it.key());
        xedit->setMaximum(1000);
        xedit->setValue(it.value());
        xywh->addWidget(xedit,index,1);
        connect(xedit,SIGNAL(valueChanged(int)),SLOT(onXYWHChangedValue(int)));
        index++;
    }

    QGroupBox *xygb = new QGroupBox(tr("坐标位置"));
    //xywh->setSizeConstraint(QLayout::SetFixedSize);
    xywh->setContentsMargins(2,10,2,20);
    xygb->setObjectName("xygb");
    xygb->setLayout(xywh);
    return xygb;
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
       // o.setY(v);
     //   QPoint p = sender->pos();
      //  p.setY(v);
       // sender->move(p);
        QPoint pos = p->pos();
        pos.setY(v);
        p->move(pos );

    }else if(!sender->objectName().compare(W))
    {
        //o.setWidth(v);
        //sender->setFixedWidth(v);
    }else if(!sender->objectName().compare(H))
    {
       // o.setHeight(v);
       // sender->setFixedHeight(v);
    }
    //sender->setGeometry(o);

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


void NewLabel::clearOtherObjectStyleSheet(QWidget *p)
{
    /* 清除控件的红线框 */
    QList<NewFrame*> nflist =  mWindow->mCanvas->findChildren<NewFrame*>();
    foreach(NewFrame *x,nflist)
    {
        if(x != p)
        {
            QList<NewLabel*> list =  x->findChildren<NewLabel*>();
            QListIterator<NewLabel*> it(list);
            while(it.hasNext())
            {
                NewLabel *nl = it.next();
                nl->setStyleSheet("");
            }

           x->setStyleSheet("");
        }
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

    NewFrame *p =(NewFrame*) (this->parentWidget());

    p->setStyleSheet("QFrame#NewFrame{border: 0.5px solid red;}");
//    p->setStyleSheet("QFrame#NewFrame{border: 2.5px;"\
//                     "border-style: outset;"\
//                     "border-color: red;"\
//                     "border-width: 10px;"\
//"subcontrol-position: top left;"\
//"padding:2 13px;"\
//                     "padding: 10px;}"); // 把本图片的父控件设置的红框
    clearOtherObjectStyleSheet(p);
    mWindow->propertyWidget->createPropertyBox(p,false);


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
    NewFrame *p =(NewFrame*) (this->parentWidget());
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

    QSize ms = mWindow->mCanvas->size();
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
    qDebug() << " parent Widget pos " << p->pos() << "size : " << p->size() << " geomerty " << p->geometry();


}

void NewLabel::mouseMoveEvent(QMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {
        NewFrame *p =(NewFrame*) (this->parentWidget());
       // QSize psize = p->parentWidget()->size();
        p->move( p->pos() + (event->pos() - mOffset));
      //  p->update();

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
}




void NewLabel::mouseDoubleClickEvent(QMouseEvent *event)
{

    NewFrame *p = (NewFrame *)this->parentWidget();

    clearOtherObjectStyleSheet(p);
    p->setStyleSheet("");
    QList<NewLabel*> nlist =  p->findChildren<NewLabel*>();
    foreach (NewLabel *n, nlist) {
        if(n != this)
            n->setStyleSheet("");

    }

    setStyleSheet("QLabel{border: 1px solid red;border-style: outset;}");
    mWindow->imgPropertyWidget->createPropertyBox(this,true);

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

void NewLabel::onPictureDialog(bool b)
{
    // QMessageBox::warning(this,"test","your clicked me: ");
    ImageFileDialog *ifd = new ImageFileDialog(myImageList,this);
    ifd->show();
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
 //   this->setProperty(DKEY_IMAGELST,selList); /* 保存它的图片列表在它的动态属性中 */
    /*
    foreach (QString s, myImageList) {
       cb->addItem(s.section(":",0,0));
    }
   // cb->addItems(sMap.keys());
    cb->setCurrentIndex(this->property(DKEY_IMGIDX).toInt());
    */
}

void NewLabel::updateComboItems(QComboBox *cb)
{
    foreach (QString s, myImageList) {
       cb->addItem(s.section(":",0,0));
    }
    cb->setCurrentIndex(this->property(DKEY_IMGIDX).toInt());
}



NewFrame::NewFrame(QWidget *parent)
    :QWidget(parent)
{
    qDebug() << " property " ;
    setObjectName("NewFrame");
    //connect(this,SIGNAL(Clicked()),SLOT(onSelectMe()));
   // this->setLineWidth(0);
   // setFrameShape(QFrame::NoFrame);
}

void NewFrame::mousePressEvent(QMouseEvent *event)
{

}

void NewFrame::mouseMoveEvent(QMouseEvent *event)
{
   // event->accept();
}


void NewFrame::onSelectMe()
{



    setStyleSheet("QFrame#NewFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
    clearOtherObjectStyleSheet(this);
    mWindow->propertyWidget->createPropertyBox(this);
}

void NewFrame::addMainWindow(QObject *mw)
{
    mWindow = (MainWindow*)mw;
}

void NewFrame::clearOtherObjectStyleSheet(QWidget *p)
{
    /* 清除控件的红线框 */
    QList<NewFrame*> nflist =  mWindow->mCanvas->findChildren<NewFrame*>();
    foreach(NewFrame *x,nflist)
    {
        if(x != p)
        {
            QList<NewLabel*> list =  x->findChildren<NewLabel*>();
            QListIterator<NewLabel*> it(list);
            while(it.hasNext())
            {
                NewLabel *nl = it.next();
                nl->setStyleSheet("");
            }

           x->setStyleSheet("");
        }
    }
}

void NewFrame::onXYWHChangedValue(int v)
{
    /* 绑定坐标控件的更新 */
    QWidget *sender =(QWidget *)(QObject::sender());

   // QWidget *p = this->parentWidget();

  //  p->move(p->parentWidget()->mapFromGlobal(QCursor::pos()-mOffset));
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

bool NewFrame::eventFilter(QObject *obj, QEvent *event)
{

    return false;
}

void NewFrame::enterEvent(QEvent *event)
{


}


void NewFrame::onClick()
{
    qDebug() << "frame clicked";
}

NewLayer::NewLayer(QSize nsize,QWidget *parent):QFrame(parent)
{
    setFixedSize(nsize);
    setObjectName("NewLayer");
    setStyleSheet("QWidget#NewLayer{border: 0.5px solid red;}");
    setFrameShape(QFrame::NoFrame);
    setLineWidth(0);
    show();
}

void NewLayer::SelectLayer()
{




        setStyleSheet("*{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
       // clearOtherObjectStyleSheet(this);
      // mWindow->propertyWidget->createPropertyBox(this);


}

void NewLayer::onXYWHChangedValue(int v)
{
    /* 绑定坐标控件的更新 */
    QWidget *sender =(QWidget *)(QObject::sender());

   // QWidget *p = this->parentWidget();

  //  p->move(p->parentWidget()->mapFromGlobal(QCursor::pos()-mOffset));
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
        if((this->pos().x() + v )> this->parentWidget()->size().width())
            return;
        sender->setFixedWidth(v);
    }else if(!sender->objectName().compare(H))
    {
        if((this->pos().y() + v )> this->parentWidget()->size().height())
            return;

        sender->setFixedHeight(v);
    }
}

void NewLayer::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->pos();
    setCursor(Qt::ClosedHandCursor);
}
void NewLayer::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {

       // QSize psize = p->parentWidget()->size();
        move(pos() + (event->pos() - mOffset));

        /* 把新的位置更新到右边属性框 */

        QPoint nr = pos();
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

void NewLayer::mouseReleaseEvent(QMouseEvent *event)
{
    /* 放开鼠标时检查它的是否出了边界要 */
    QWidget *p = this->parentWidget();
    QPoint pos = this->pos();
    if(this->x() < 0)
    {
        pos.setX(0);
        move(pos);

    }
    if(this->y() < 0 )
    {
        pos.setY(0);
        move(pos);

    }

    QSize ms = p->size();
    if((this->x()  + this->size().width()) > ms.width())
    {
        pos.setX( ms.width() - this->size().width() );
        move(pos);
    }

    if((this->y() + this->size().height()) > ms.height())
    {
        pos.setY(ms.height() - this->size().height());
        move(pos);
    }
}


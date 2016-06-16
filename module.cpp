#include "module.h"
#include <QtCore/QMetaProperty>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSpacerItem>

#include <QApplication>
#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSignalMapper>

#include "config.h"



NewLabel::NewLabel(QWidget *parent)
    :QLabel(parent)
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
        sender->setFixedWidth(v);
    }else if(!sender->objectName().compare(H))
    {
       // o.setHeight(v);
        sender->setFixedHeight(v);
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
    p->setStyleSheet("QFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
    clearOtherObjectStyleSheet(p);
    mWindow->propertyWidget->createPropertyBox(p);


     QTextEdit *et = (QTextEdit *)(getQWidgetByName("debugEdit"));
     if(et != 0 )
     {
         et->setText(QString("mouse x:%1 , y:%2 ")
                     .arg(QString::number(ev->pos().rx()))
                     .arg(QString::number(ev->pos().ry())));
     }
    ev->accept();
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
        QSize psize = p->parentWidget()->size();
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
    /*
    QList<NewLabel*> list =  p->findChildren<NewLabel*>();
    QListIterator<NewLabel*> it(list);
    while(it.hasNext())
    {
        NewLabel *nl = it.next();
        nl->setStyleSheet("");
    }
    p->setStyleSheet("");
    */
    clearOtherObjectStyleSheet(p);
    p->setStyleSheet("");
    QList<NewLabel*> list =  p->findChildren<NewLabel*>();
    QListIterator<NewLabel*> it(list);
    while(it.hasNext())
    {
        NewLabel *nl = it.next();
        qDebug() << "  label style sheet "  << nl->styleSheet();
        nl->setStyleSheet("");
    }

    this->setStyleSheet("QLabel{border: 1px solid red;}");
    /* here testing dyanmicProperty */


    /* 这里要与主界面的一些控件做通信 */
    /*
    foreach (QWidget *w, mWindow->imgPropertyWidget->layout()->findChildren<QWidget*>()) {
        delete w;  // 释放原来的控件
    }*/

    removeWidFromLayout(mWindow->imgPropertyWidget->layout());
    delete mWindow->imgPropertyWidget->layout();
    QVBoxLayout *v = new QVBoxLayout();
    v->setObjectName(IMGGRPLYT);


    mWindow->imgPropertyWidget->setLayout(v);
    //mWindow->imgPropertyWidget->setTitle(this->objectName());

    QVariantList qvl = this->property(DKEY_DYN).toList();
    /* 根据JSON格式动态生成下面的控件 */
    foreach(QVariant qv, qvl)
    {
        if(qv.type() == QVariant::Map)
        {
            QVariantMap qvm = qv.toMap();
            QString uname = qvm["-name"].toString();
            if(qvm.contains("enum"))
            {


                QComboBox *cb = new QComboBox();
                QVariantList qvlist = qvm["enum"].toList();
                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {

                    cb->addItem((*it).toMap().firstKey());
                }
                v->addWidget(new QLabel(uname));
                v->addWidget(cb);

            }else if(qvm.contains("list"))
            {

                QComboBox *cb = new QComboBox();
                cb->setObjectName("ListImage");
                this->setProperty(DKEY_IMGIDX,0); /* 当前选择的行号*/
                //QString uname =  qvm["-name"].toString();
                QVariantList qvlist = qvm["list"].toList();
                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {
                    cb->addItem((*it).toString());
                }


                v->addWidget(new QLabel(uname));
                //v->addLayout(h);
                QPushButton *b = new QPushButton(tr("添加图片"));
                connect(b,SIGNAL(clicked(bool)),SLOT(onPictureDialog(bool)));
                v->addWidget(b);
                v->addWidget(cb);
                /* 绑定QComoBox的更改信号,更改它的值就要在相应的画版控件更新图片 */
                connect(cb,SIGNAL(currentTextChanged(QString)),SLOT(onListImageChanged(QString)));


            }
            else{
                if(uname.compare("geometry") &&
                        uname.compare("image"))
                {
                    if(qvm.contains("id"))
                    {
                        v->addWidget(new QLabel(uname));
                        v->addWidget(new QLabel(this->property("uid").toString()));

                    } /* 这里是一个特殊属性,唯一序号 */

                    else if(qvm["default"].type() == QVariant::Double)
                    {
                       // QDateTime t;

                        // QTextEdit *id = new QTextEdit(t.toLocalTime().toString());
                        // id->setEnabled(false);
                        /* 这里要区分ID 还是数字属性 */
                        v->addWidget(new QLabel(uname));
                        QSpinBox *s = new QSpinBox();
                        v->addWidget(s);
                        s->setValue(qvm["default"].toInt());
                      //  v->addWidget(new QLabel(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())));
                    }
                    else{
                        QTextEdit *txt = new QTextEdit(qvm["default"].toString());
                        v->addWidget(new QLabel(uname));
                        v->addWidget(txt);
                        txt->setFixedHeight(25);
                        //  v->addSpacerItem(new QSpacerItem(10,50));

                    }
                }
            }


        }
    }
    //v->addSpacerItem(new QSpacerItem(100,300));
    v->setContentsMargins(propertyMarg);

}


void NewLabel::onListImageChanged(QString img)
{
   //  selectedMap sMap = this->property(IMAGELST).toMap();
   QStringList selList = this->property(DKEY_IMAGELST).toStringList();
   foreach (QString s, selList) {
       QString k = s.section(":",0,0);
       if(!k.compare(img))
       {
           this->setPixmap(QPixmap(s.section(":",1,1)));/* 更新图片 */
           break;
       }
   }

}


void NewLabel::onPictureDialog(bool b)
{
    // QMessageBox::warning(this,"test","your clicked me: ");
    ImageFileDialog *ifd = new ImageFileDialog(this);
    ifd->show();
    ifd->exec();
    //selectedMap sMap  = ifd->getSelectedMap();
    QStringList selList = ifd->getSelectedList();
    ifd->deleteLater();
    QComboBox *cb=0;
    foreach (QWidget *w, mWindow->imgPropertyWidget->findChildren<QWidget*>())
    {
        if(!w->objectName().compare("ListImage"))
        {
          //  qDebug() << " found QComobox " << w->objectName();
            cb = (QComboBox *)w;
            break;
        }
    }
    this->setProperty(DKEY_IMAGELST,selList); /* 保存它的图片列表在它的动态属性中 */
    foreach (QString s, selList) {
       cb->addItem(s.section(":",0,0));
    }
   // cb->addItems(sMap.keys());
    cb->setCurrentIndex(0);
}


NewFrame::NewFrame(QWidget *parent)
    :QWidget(parent)
{
    //connect(this,SIGNAL(Clicked()),SLOT(onSelectMe()));
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



    setStyleSheet("QFrame{border: 0.5px solid red;}"); // 把本图片的父控件设置的红框
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

        sender->setFixedWidth(v);
    }else if(!sender->objectName().compare(H))
    {

        sender->setFixedHeight(v);
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




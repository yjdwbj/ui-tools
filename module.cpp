#include "module.h"
#include <QtCore/QMetaProperty>
#include <QtWidgets/QStatusBar>

NewLabel::NewLabel(QWidget *parent)
    :QLabel(parent)
{

    // connect(this,SIGNAL(Clicked()),SLOT(onClieck()));
}



void NewLabel::mousePressEvent(QMouseEvent *ev)
{

    NewFrame *p =(NewFrame*) (this->parentWidget());
    p->setStyleSheet("QFrame{border: 0.5px solid red;}");

    mOffset = ev->pos();

    QWidget *mCanvas  = p->parentWidget();
    QStatusBar *sb = mCanvas->parentWidget()->findChild<QStatusBar*>();
    if(sb)
    {
        sb->showMessage(QString("mouse x:%1 , y:%2 ")
                        .arg(QString::number(ev->pos().rx()))
                        .arg(QString::number(ev->pos().ry()))  );

    }
    ev->accept();

    emit Clicked();
}

void NewLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    NewFrame *p =(NewFrame*) (this->parentWidget());
    qDebug() << " drop pos x: " << ev->pos().x() << " y:" << ev->pos().y();
    qDebug() << " drop Global  pos x: " << ev->globalPos().x() << " y:" << ev->globalPos().y();


}

void NewLabel::mouseMoveEvent(QMouseEvent *event)
{



    if (event->buttons() & Qt::LeftButton)
    {
        NewFrame *p =(NewFrame*) (this->parentWidget());
        //  qDebug() << "checked move event " << p->objectName() ;
        // emit p->mouseMoveEvent(event);
        //emit this->parentWidget()->mouseMoveEvent(event);
        // p->move(p->parentWidget()->mapFromGlobal(event->pos()));
        QSize psize = p->parentWidget()->size();
        qDebug() << " parent size w: " << psize.width() << " h : " << psize.height();
        qDebug() << " move pos x: " << event->pos().x() << " y: " << event->pos().y();
        qDebug() << " move Global  pos x: " << event->globalPos().x() << " y: " << event->globalPos().y();

        p->move(p->parentWidget()->mapFromGlobal(QCursor::pos()-mOffset));
        p->update();




        // qDebug() << "offset is " << offset.rx() << "  " << offset.ry() << " event pos " << event->pos().rx() << " " << event->pos().ry() ;
    }
    // event->accept();
    //  this->parentWidget()->update();
}

void NewFrame::mouseMoveEvent(QMouseEvent *event)
{


    event->accept();

}


void NewLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "label double clicked";
    NewFrame *p = (NewFrame *)this->parentWidget();
    QList<NewLabel*> list =  p->findChildren<NewLabel*>();
    QListIterator<NewLabel*> it(list);
    while(it.hasNext())
    {
        NewLabel *nl = it.next();
        nl->setStyleSheet("");
    }
    p->setStyleSheet("");

    this->setStyleSheet("QLabel{border: 1px solid red;}");
    /* here testing dyanmicProperty */

    foreach(QByteArray qba, this->dynamicPropertyNames())
    {
        qDebug() << QString::fromLocal8Bit(qba) << this->property(qba);
    }

    qDebug() << "test index of property \n";
    static const QMetaObject *dm = this->metaObject();
    for(int i = 0 ; i < dm->propertyCount();i++)
    {
        const char *dname = dm->property(i).name();
        qDebug() << QString::fromLocal8Bit(dname) << this->property(dname);
    }


}

void NewLabel::onClieck()
{
    qDebug() << " On Clicked";
}

NewFrame::NewFrame(QWidget *parent)
    :QWidget(parent)
{

}

void NewFrame::mousePressEvent(QMouseEvent *event)
{

    QWidget *mCanvas  = this->parentWidget();
    QStatusBar *sb = mCanvas->parentWidget()->findChild<QStatusBar*>();
    if(sb)
    {





        sb->showMessage(QString("mouse x:%1 , y:%2 ").arg(QString::number(event->pos().rx())).arg(QString::number(event->pos().ry()))  );



    }
    event->accept();
    // emit Clicked();
}



bool NewFrame::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *mCanvas  = this->parentWidget();
    QStatusBar *sb = mCanvas->parentWidget()->findChild<QStatusBar*>();
    if(sb)
    {
        if ( event->type() != QEvent::MouseMove )
            return false;

        const QMouseEvent* const me = static_cast<const QMouseEvent*>( event );
        sb->showMessage(QString("mouse x:%1 , y:%2 ").arg(QString::number(me->pos().rx())).arg(QString::number(me->pos().ry()))  );
    }
    // event->accept();
    return false;
}

void NewFrame::enterEvent(QEvent *event)
{
    QWidget *mCanvas  = this->parentWidget();
    QStatusBar *sb = mCanvas->parentWidget()->findChild<QStatusBar*>();
    if(sb)
    {

        if ( event->type() == QEvent::MouseButtonPress )
        {
            const QMouseEvent* const me = static_cast<const QMouseEvent*>( event );
            sb->showMessage(QString("mouse x:%1 , y:%2 ").arg(QString::number(me->pos().rx())).arg(QString::number(me->pos().ry()))  );
        }

    }
    event->accept();

}


void NewFrame::onClick()
{
    qDebug() << "frame clicked";
}




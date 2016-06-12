#include "module.h"
#include <QtCore/QMetaProperty>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSpacerItem>
#include <QDateTime>
#include <QApplication>
#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>

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



void NewLabel::mousePressEvent(QMouseEvent *ev)
{

    /* 单击选中它的父对像 */

    NewFrame *p =(NewFrame*) (this->parentWidget());
    p->setStyleSheet("QFrame{border: 0.5px solid red;}");

    mWindow->propertyWidget->layout()->deleteLater();
    QVBoxLayout *v = new QVBoxLayout();
    mWindow->propertyWidget->setLayout(v);
    mWindow->propertyWidget->setTitle(p->objectName());


    QVariantList qvl = p->property("dynProperty").toList();
    foreach(QVariant qv, qvl)
    {

        if(qv.type() == QVariant::Map)
        {
            QVariantMap qvm = qv.toMap();
            QString uname =  qvm["-name"].toString();
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
                //QString uname =  qvm["-name"].toString();
                QVariantList qvlist = qvm["list"].toList();
                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {
                    cb->addItem((*it).toString());
                }
                v->addWidget(new QLabel(uname));
                v->addWidget(cb);

            }
            else{
                if(uname.compare("geometry"))
                {
                    if(qvm["default"].type() == QVariant::Double)
                    {
                        QDateTime t;

                        // QTextEdit *id = new QTextEdit(t.toLocalTime().toString());
                        // id->setEnabled(false);

                        v->addWidget(new QLabel(uname));
                        v->addWidget(new QLabel(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())));
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
        qDebug() << "next qv is " << qv;
    }







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

    //  emit Clicked();
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
    /* 这里测试打一下对像的所有动态性 */
    foreach(QByteArray qba, this->dynamicPropertyNames())
    {
        // qDebug() << QString::fromLocal8Bit(qba) << this->property(qba);
    }




    /* 这里要与主界面的一些控件做通信 */
    mWindow->imgPropertyWidget->layout()->deleteLater();
    QVBoxLayout *v = new QVBoxLayout();
    mWindow->imgPropertyWidget->setLayout(v);
    mWindow->imgPropertyWidget->setTitle(this->objectName());





    QVariantList qvl = this->property("dynProperty").toList();
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
                //QString uname =  qvm["-name"].toString();
                QVariantList qvlist = qvm["list"].toList();
                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {
                    cb->addItem((*it).toString());
                }
                // QHBoxLayout *h = new QHBoxLayout();

                v->addWidget(new QLabel(uname));
                //v->addLayout(h);
                QPushButton *b = new QPushButton(tr("添加图片"));
                connect(b,SIGNAL(clicked(bool)),SLOT(onPictureDialog(bool)));
                v->addWidget(b);
                v->addWidget(cb);


            }
            else{
                if(uname.compare("geometry") &&
                        uname.compare("image"))
                {
                    if(qvm["default"].type() == QVariant::Double)
                    {
                        QDateTime t;

                        // QTextEdit *id = new QTextEdit(t.toLocalTime().toString());
                        // id->setEnabled(false);

                        v->addWidget(new QLabel(uname));
                        v->addWidget(new QLabel(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())));
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
    /* QObjectList ob = m->propertyStack->children();
    for(QObjectList::const_iterator it = ob.begin();
        it != ob.end(); ++it)
    {
        QWidget *qw = (QWidget* )(*it);
        qDebug() << " current name " << qw->objectName() << " clicked name " << this->objectName();

        if(!qw->objectName().compare(this->objectName()))
        {
            qDebug() << "clicked Object Name " << this->objectName();
            m->propertyStack->setCurrentWidget(qw);
            break;
        }
    }*/


}



void NewLabel::onPictureDialog(bool b)
{
    // QMessageBox::warning(this,"test","your clicked me: ");
    ImageFileDialog *ifd = new ImageFileDialog(this);
    ifd->show();
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




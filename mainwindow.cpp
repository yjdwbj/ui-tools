#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dragwidget.h"
#include "module.h"

static QObject* mParent;
static bool isWidget = true;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    out(stdout, QIODevice::WriteOnly),

    ui(new Ui::MainWindow)
{
    QDesktopWidget *pDwgt = QApplication::desktop();







    ui->setupUi(this);

  //  mCanvas->setStyleSheet("background-color: rgb(238, 238, 236);");
  /*  ui->centralWidget->hide();
    ui->centralWidget->deleteLater();

    QMdiArea *qma = new QMdiArea();
  // qma->setFixedSize(QSize(40,20));
//    qma->setGeometry(QRect(100,100,300,300));
 //   ui->centralWidget->hide();
 //   ui->centralWidget->deleteLater();
    setCentralWidget(qma);
  //  ui->centralWidget->setGeometry(QRect(100,100,300,300));
  //  setContentsMargins(100,100,0,0);
    qma->setFixedSize(QSize(480,600));

    QMdiSubWindow *midwin = new QMdiSubWindow;
  //  midwin->setAttribute(Qt::WA_QuitOnClose);
    midwin->setWindowFlags(Qt::FramelessWindowHint);
    qma->addSubWindow(midwin);*/



    QRect desk_rect = pDwgt->screenGeometry(pDwgt->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
   // ui->centralWidget->setFixedHeight(desk_y);
   // ui->centralWidget->setFixedWidth(desk_x-400);
    mCanvas = new QFrame(ui->centralWidget);
    mCanvas->setFrameShadow(QFrame::Raised);
    mCanvas->setFrameShape(QFrame::StyledPanel);
    mCanvas->installEventFilter(this);
    int ch = ui->centralWidget->height()+200;
    int cw = ui->centralWidget->width()+300;
    mCanvas->move(cw/2,ch/2);
    mCanvas->setFixedSize(320,240);

    QString css = "*{  border: 2px solid gray;}";
    // ui->centralWidget->setStyleSheet(css);
   // QHBoxLayout *mainLayout = new QHBoxLayout();
  //  ui->centralWidget->setLayout(mainLayout);
    ui->mainToolBar->addWidget(new QPushButton("test"));


    lDock = new QDockWidget(tr("left"));
    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
    lList = new QListWidget(lDock);
    lDock->setWidget(lList);

    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/krfb.png")),tr("test")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/kwin.png")),tr("kwin")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/kuser.png")),tr("kuser")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/calc.png")),tr("calc")));
    addDockWidget(Qt::LeftDockWidgetArea, lDock);
    lList->setFixedWidth(120);


    rDock = new QDockWidget(tr("right"));
    rDock->setAllowedAreas(Qt::RightDockWidgetArea);

    rList = new QListWidget(rDock);
    addDockWidget(Qt::RightDockWidgetArea, rDock);
    rDock->setWidget(rList);
   // rList->addItems(QStringList() << "111" << "222" << "333");




    rList->setFixedWidth(120);




    QJsonParseError json_error;
    mParent = ui->centralWidget;

    //QFile data("/home/yjdwbj/pximap_frame.json");
    QFile data("/home/yjdwbj/menu_strip.json");
    if (data.open(QFile::ReadOnly|QIODevice::Text)) {
        QByteArray qba = data.readAll();
        QTextStream in(&data);
        QString str;
        int ans = 0;
        in >> str >> ans;

        // QJsonDocument qd = QJsonDocument::fromRawData(str,&ans);

        QJsonDocument qd = QJsonDocument::fromJson(qba,&json_error);
        if(json_error.error == QJsonParseError::NoError)
        {
            QWidget *mp = 0;
            QPoint mpos;
            if(qd.isObject())
            {

             //   HandleObject( qd.object());
                HandleFrameObject(qd.object(),ui->centralWidget->objectName());

                QListIterator<ObjComt> iterator(ComList);
                while (iterator.hasNext()) {


                    ObjComt oc = iterator.next();
                    QString pName = oc.parentName;

                    if(!oc.clsName.compare("QFrame"))
                    //if(!oc.clsName.compare("QWidget"))
                    {
                        NewFrame *qf = new NewFrame(mCanvas);
                        mp = (QWidget*)qf;
                        qf->setObjectName(oc.objName);
                        qf->setEnabled(true);
                        qf->setVisible(true);
                        qf->setGeometry(oc.rect);
                        QPoint cpos = qf->pos();
                        QPoint ppos = mCanvas->pos();


                        QPoint ncpos = qf->mapFromParent(ppos);

                        QPoint nppos = qf->mapToParent(ppos);
                        QPoint nncpos = qf->mapFromGlobal(ppos);


                        int x =mCanvas->size().width()/2-20;
                        int y = mCanvas->size().height()/2-20;
                        qf->move(QPoint(x,y));
                        qf->installEventFilter(this);


                     //   qf->move(mapToParent(mCanvas->pos()-qf->pos()));

                      //  qf->setFrameShadow(QFrame::Raised);
                     // qf->setFrameShape(QFrame::StyledPanel);

                    }
                   //   qDebug() << oc.pixmap;

                }


                QListIterator<ObjComt> it(ComList);
                while (it.hasNext()) {


                    ObjComt oc = it.next();
              //      QString pName = oc.parentName;
                //    qDebug() << "current Name " << oc.objName << "paraent Name " << pName;
                    if(!oc.clsName.compare("QLabel"))
                    {

                        NewLabel *lab  = new NewLabel(mp);
                       // lab->setParent(ui->centralWidget->findChild<QFrame*>(oc.parentName));
                        lab->setObjectName(oc.objName);
                        lab->setGeometry(oc.rect);
                        lab->setProperty("myname",oc.objName);
                       // lab->move(mapToGlobal(lab->pos()-mpos));
                        QPoint cpos = lab->pos();
                        QPoint ppos = mp->pos();
                        lab->setProperty("px",oc.rect.x());
                        lab->setProperty("py",oc.rect.y());
                        lab->setProperty("pw",oc.rect.width());
                        lab->setProperty("ph",oc.rect.height());




                        QPoint ncpos = lab->mapFromParent(ppos);

                        QPoint nppos = lab->mapToParent(ppos);
                        lab->installEventFilter(this);
                      //  lab->move(lab->mapToParent(mpos-lab->pos()));

                    //    qDebug() << oc.pixmap;
                        QPixmap img ;
                        img.load(oc.pixmap);
                        lab->setPixmap(img);
                        oc.obj = lab;
                        lab->setProperty("imgpath",oc.pixmap);
                        connect(lab,SIGNAL(Clicked()),lab,SLOT(onClieck()));

                      //  connect(lab,SIGNAL(mouse),SLOT(slot_mousepressed()));

                    //    lab->update();



                    }




                }
            //    mp->update();
           //     mp->adjustSize();


                // leftList->addItem(result.value(obj.toString()).toString());

            }

        }

    }



}

void MainWindow::slot_framepressed(QMouseEvent *ev)
{
    qDebug() << "frame mose event";
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{


        if(obj == mCanvas)
        {
            if(event->type() == QEvent::MouseButtonPress)
            {
                QList<QFrame*> lst = mCanvas->findChildren<QFrame*>();
                QListIterator<QFrame*> it(lst);
                while(it.hasNext())
                {
                    QFrame *qf = it.next();
                    qf->setStyleSheet("");
                }

            }
        }

        if ( event->type() != QEvent::MouseButtonPress )
            return false;


            const QMouseEvent* const me = static_cast<const QMouseEvent*>( event );
            int x = me->pos().rx();
            int y = me->pos().ry();
            QPoint xy = me->pos();
            QPoint pxy = mapFromParent(xy);
            QPoint gxy = mapFromGlobal(xy);


            QString msg =  QString("mouse x:%1 , y:%2 ----- px : %3 , py : %4 ---- gx : %5, gy : %6")
                    .arg(QString::number(xy.rx()))
                    .arg(QString::number(xy.ry()))
                    .arg(QString::number(pxy.rx()))
                    .arg(QString::number(pxy.ry()))
                    .arg(QString::number(gxy.rx()))
                    .arg(QString::number(gxy.rx()));
            ui->statusBar->showMessage( msg );


            qDebug() << " mapto global or parent";

            pxy = this->mapToParent(xy);
            gxy = this->mapToGlobal(xy);

            qDebug() << " px : " << pxy.x() << ",px : " << pxy.y()
                     << " , gx : " << gxy.x() << ", gy : " << gxy.y() ;




   // event->accept();
    return false;
}

void MainWindow::setWidget(QObject &oob)
{
   /* if(isWidget)
    {
        qobject_cast<QWidget*>(oob)->setParent(qobject_cast<QWidget*>(mParent));
    }else
    {
       qobject_cast<QBoxLayout*>(mParent)->addWidget(qobject_cast<QWidget*>(oob));
    }*/
}

void MainWindow::HandleFrameObject(QJsonObject qjo,QString ParentName)
{

    ObjComt obj;
    obj.parentName = ParentName;
    QObject *child;
    for(QJsonObject::iterator it = qjo.begin();it != qjo.end();++it)
    {
        QString key = it.key();
        if(!key.compare(NAME))
        {
            obj.objName = it.value().toString();
        }else if(!key.compare(CLASS))
        {
            obj.clsName = it.value().toString();
        }else if(!key.compare("widget"))
        {
            if(it.value().isObject())
            {
               HandleFrameObject(it.value().toObject(),obj.objName);
            }
            else if(it.value().isArray())
            {
                QJsonArray qja = it.value().toArray();

                for(int idx = 0;idx < qja.size();idx++)
                {
                    if(qja[idx].isObject())
                    {
                        HandleFrameObject(qja[idx].toObject(),obj.objName);
                    }
                }
            }

        }else if(!key.compare(PROPERTY))
        {
            if(it.value().isArray())
            {
                QJsonArray qja = it.value().toArray();

                for(int idx = 0; idx < qja.size();idx++)
                {
                    QJsonValue qjv = qja[idx];
                    if(qjv.isObject())
                    {
                       QJsonObject ooj = qjv.toObject();
                       if(ooj.contains(RECT))
                       {

                           QJsonObject rect = ooj[RECT].toObject();
                           int x,y,w,h;
                           x = rect["x"].toString().toInt();
                           y = rect["y"].toString().toInt();
                           w = rect["width"].toString().toInt();
                           h = rect["height"].toString().toInt();

                           obj.rect.setRect(x,y,w,h);
                       }else if(ooj.contains("pixmap"))
                       {
                           int p = ooj["pixmap"].type();
                           obj.pixmap = QDir::currentPath()+"/"+ ooj["pixmap"].toString();
                       }

                    }

                }
            }
        }

    }


   /* if(!obj.clsName.compare("QFrame"))
    {
        v
        obj.obj = qf;

    }else if(!obj.clsName.compare("QWidget"))
    {
        QWidget *wid = new QWidget();
        wid->setObjectName(obj.objName);
        obj.obj = wid;

    }else if(!obj.clsName.compare("QLabel"))
    {
        QLabel *lab  = new QLabel();
        lab->setParent(QObject::findChild<QFrame*>(obj.parentName));
        lab->setObjectName(obj.objName);
        lab->setGeometry(obj.rect);
        lab->setPixmap( QPixmap(obj.pixmap));
        lab->setVisible(true);

        obj.obj = lab;
    }*/
    ComList.append(obj);

}

void MainWindow::HandleObject(QJsonObject qjo)
{
    if(qjo.contains(WIDGET))
    {
        QJsonValue qjv = qjo[WIDGET].toObject();
        if(qjv.isObject())
        {
             HandleObject(qjv.toObject());
        }
        else if(qjv.isArray())
        {
           // HandleArrayObject(qjv.isArray());
        }

    }
    else if(qjo.contains(CLASS))
    {
        QString cval = qjo[CLASS].toString();
        if(!cval.compare("QWidget"))
        {

            QWidget *o = new QWidget();
            if(isWidget)
            {
                o->setParent(qobject_cast<QWidget*>(mParent));
            }else
            {
               qobject_cast<QBoxLayout*>(mParent)->addWidget(o);
            }
            //setWidget(o);
            mParent = o;
            isWidget = true;
            if(qjo.contains(NAME))
            {
                o->setObjectName(qjo[NAME].toString());
            }
            if(qjo.contains(PROPERTY))
            {
                QJsonObject proj = qjo[PROPERTY].toObject();
                if (proj.contains(RECT))
                {
                    o->setGeometry(QRect(proj["x"].toInt(),
                                   proj["y"].toInt(),
                                   proj["width"].toInt(),
                                   proj["height"].toInt()));
                }


            }
            if(qjo.contains(LAYOUT))
            {
                QJsonObject layoutobj = qjo[LAYOUT].toObject();
                HandleObject(layoutobj);
                mParent = o;
                isWidget = true;

            }
        }else if(!cval.compare("QVBoxLayout"))
        {
            QVBoxLayout *vlay = new QVBoxLayout();
            if(isWidget)
            {
                qobject_cast<QWidget*>(mParent)->setLayout(vlay);
            }else
            {
               qobject_cast<QBoxLayout*>(mParent)->addLayout(vlay);
            }

            isWidget = false;
            mParent = vlay;
            if(qjo.contains(NAME))
            {
               vlay->setObjectName(qjo[NAME].toString());
            }
            if(qjo.contains(ITEM))
            {
                QJsonArray qja = qjo[ITEM].toArray();
                HandleArrayObject(qja);
               /* for(int x = 0; x < qja.size();x++)
                {
                        QJsonValue qija = qja[x];
                        if(qija.isObject())
                        {
                            HandleObject(qija.toObject());
                        }else if(qija.isString())
                        {
                            out << qija.toString() << "\n";
                        }
                }*/
            }
        }
        else if(!cval.compare("QHBoxLayout"))
        {
                 QHBoxLayout *vlay = new QHBoxLayout();
                 if(isWidget)
                 {
                     qobject_cast<QWidget*>(mParent)->setLayout(vlay);
                 }else
                 {
                    qobject_cast<QBoxLayout*>(mParent)->addLayout(vlay);
                 }
                  isWidget = false;
                  if(qjo.contains(NAME))
                  {
                     vlay->setObjectName(qjo[NAME].toString());
                  }
                  if(qjo.contains(ITEM))
                  {
                     HandleArrayObject(qjo[ITEM].toArray());
                  }
        }
    }

}

void MainWindow::HandleArrayObject(QJsonArray array)
{
    int asize = array.size();
    for(int x = 0; x < asize;x++)
    {
            QJsonValue qija = array[x];
            if(qija.isObject())
            {
                HandleObject(qija.toObject());
            }else if(qija.isString())
            {
                out << qija.toString() << "\n";
            }
    }
}

void MainWindow::getJsonValue(QJsonValue qjv)

{

    /*switch (qjv.type()) {
    case QJsonValue::Array:
    {//out << "\nkey " << key << " value  Array\n" ;
        QJsonArray qja = qjv.toArray();
        int qjas = qja.size();
        for(int i = 0; i < qjas; i++)
        {
            QJsonValue sqjv = qja[i];

            getJsonValue(sqjv);

        }}
        break;
    case QJsonValue::Bool:
        out << "key " << qjv.toBool() << " value  Bool\n" ;
        break;
    case QJsonValue::String:
        out << "key " << qjv.toString() << " value  String\n" ;
        break;
    case QJsonValue::Object:
    {
        out << "key     value  Object\n" ;
        // result =  qv.toVariant().toMap();
        QJsonObject qjo = qjv.toObject();
       if(qjo.contains(CLASS))
       {
           QString cval = qjo[CLASS].toString();
           if(!cval.compare("QWidget"))
           {
               QWidget *o = new QWidget();
               mParent = o;
               if(qjo.contains(NAME))
               {
                   o->setObjectName(qjo[NAME].toString());
               }
               if(qjo.contains(PROPERTY))
               {
                   QJsonObject proj = qjo[PROPERTY].toObject();
                   if (proj.contains(RECT))
                   {
                       o->setGeometry(QRect(proj["x"].toInt(),
                                      proj["y"].toInt(),
                                      proj["width"].toInt(),
                                      proj["height"].toInt()));
                   }


               }
               if(qjo.contains(LAYOUT))
               {
                   QJsonObject layoutobj = qjo[LAYOUT].toObject();

               }
           }else if(!cval.compare("QVBoxLayout"))
           {
               QVBoxLayout *vlay = new QVBoxLayout(mParent);
               mParent->setLayout(vlay);
               if(qjo.contains(NAME))
               {
                  vlay->setObjectName(qjo[NAME].toString());
               }
               if(qjo.contains(ITEM))
               {
                   QJsonArray qja = qjo[ITEM].toArray();
                   for(int x = 0; x < qja.size();x++)
                   {

                   }
               }
           }
       }


        foreach (QVariant obj, result.keys()) {
            QString key = obj.toString();
            out << "\n key " << key;
            //  QJsonValue qjv = qd.object().value(key);
            getJsonValue(qjv.toObject().value(key)) ;
        }

    }
        break;
    case QJsonValue::Double:
        out << "key " << qjv.toDouble() << " value  Double\n" ;
        break;
    case QJsonValue::Null:
        out << "key null  value  Null\n" ;
        break;
    default:
        break;


    }
    out << "\n";
    */
}





MainWindow::~MainWindow()
{
    delete ui;
}

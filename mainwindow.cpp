#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dragwidget.h"

#include "handlejson.h"
#include <QStandardPaths>



static bool isWidget = true;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    out(stdout, QIODevice::WriteOnly),

    ui(new Ui::MainWindow),
    propertyWidget(new QGroupBox()),
    imgPropertyWidget(new QGroupBox())

{
    QDesktopWidget *pDwgt = QApplication::desktop();
    ui->setupUi(this);
    imgPropertyWidget->setTitle(tr("元素属性"));
    imgPropertyWidget->setLayout(new QVBoxLayout());
    propertyWidget->setTitle(tr("控件属性"));
    propertyWidget->setLayout(new QVBoxLayout());

    propertyWidget->setFlat(true);



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


    ui->mainToolBar->addWidget(new QPushButton("test"));


    lDock = new QDockWidget(tr("left"));
    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
    lDock->setFixedWidth(180);
    lDock->setObjectName("LeftDock");
    lList = new QListWidget();
   // lDock->setWidget(lList);

    QWidget *lDockWidget = new QWidget(lDock);
    lDockWidget->setObjectName("lDockWidget");

    lDock->setWidget(lDockWidget);


    QVBoxLayout *leftLayout = new QVBoxLayout();
   // lDock->setLayout(leftLayout);
    leftLayout->setObjectName("leftLayout");
    lDockWidget->setLayout(leftLayout);

    leftLayout->addWidget(lList);



    //QStackedLayout *propertyStack = new QStackedLayout();
  //  this->rightStackedLayout()->setObjectName("ObjProperty");
    //propertyStack->setObjectName("ObjProperty");

    leftLayout->addWidget(propertyWidget);
    leftLayout->addWidget(imgPropertyWidget); /* 单个图片的属性组 */


    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/krfb.png")),tr("test")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/kwin.png")),tr("kwin")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/kuser.png")),tr("kuser")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/calc.png")),tr("calc")));
    addDockWidget(Qt::LeftDockWidgetArea, lDock);
    lList->setFixedWidth(160);


    rDock = new QDockWidget(tr("right"));
    rDock->setObjectName("RightDock");
    rDock->setAllowedAreas(Qt::RightDockWidgetArea);

    rList = new QListWidget(rDock);
    rList->setObjectName("rDockListWidget");
    addDockWidget(Qt::RightDockWidgetArea, rDock);
    rDock->setWidget(rList);
    rList->setFixedWidth(160);
    QString filename =  QDir::currentPath() + "/menu_strip.json";
    //qDebug() << " json file name " << filename;
    QFileInfo qfi(filename);
    if(!qfi.exists())
    {
        QMessageBox::warning(this,tr("错误"),tr("找不到控件文件"));
        return;
    }
    //qDebug() << " json file name is : " << filename;
    HandleJson *hj = new HandleJson(filename);
  //  mCanvas->setProperty("PropertyStack",QVariant::fromValue(propertyStack));
    QWidgetList qwlist = qApp->allWidgets();

    /*for(QWidgetList::const_iterator it = qwlist.begin(); it != qwlist.end();++it )
    {
        qDebug() << " All Qwidget list object : " << (*it)->objectName();
    }*/
    QWidget *ww = (QWidget*)(hj->CreateObjectFromJson(hj->mJsonMap,mCanvas));
    qDebug() << "New object Rect " << ww->geometry()
             << " Pos " <<  ww->mapToParent(ww->pos());
    delete hj;
   // ww->move(ww->parentWidget()->mapFromGlobal(QPoint(200,200)));


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





MainWindow::~MainWindow()
{
    delete ui;
}

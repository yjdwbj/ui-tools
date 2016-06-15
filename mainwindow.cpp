#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "handlejson.h"
#include "compoentcontrols.h"

#include <QStandardPaths>



static bool isWidget = true;

static int CanvasW  = 320;
static int CanvasH  = 240;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    out(stdout, QIODevice::WriteOnly),

    ui(new Ui::MainWindow),
    propertyWidget(new QGroupBox()),
    imgPropertyWidget(new QGroupBox())

{

    ui->setupUi(this);


    //this->showFullScreen();

    imgPropertyWidget->setTitle(tr("元素属性"));
    imgPropertyWidget->setLayout(new QVBoxLayout());
    propertyWidget->setTitle(tr("控件属性"));
    propertyWidget->setLayout(new QVBoxLayout());
    propertyWidget->setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");
    imgPropertyWidget->setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");






    //propertyWidget->setFlat(true);


    /*QDesktopWidget *pDwgt = QApplication::desktop();
    QRect desk_rect = pDwgt->screenGeometry(pDwgt->screenNumber(QCursor::pos()));
    qDebug() << "desk rect is " << desk_rect;
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();

    this->setMaximumSize(desk_rect.size());

    ui->centralWidget->setMaximumSize(desk_rect.width(),desk_rect.height() -35);
    this->showMaximized();
    this->centralWidget()->showMaximized();
    */
    //ui->centralWidget->setFixedHeight(desk_y);
   // ui->centralWidget->setFixedWidth(desk_x-400);
    mCanvas = new QFrame(ui->centralWidget);
    mCanvas->setFrameShadow(QFrame::Raised);
    mCanvas->setFrameShape(QFrame::StyledPanel);
    mCanvas->installEventFilter(this);

    //mCanvas->move((this->size().width()/2) - CanvasW,(this->size().height()/2)-CanvasH);
    mCanvas->move((this->size().width()/2) - CanvasW /2  ,(this->size().height()/2)-200 / 2);
    //mCanvas->move(QPoint(100,100));
    //mCanvas->move(ch/2,cw/2);
   // mCanvas->move(this->mapFromGlobal(QPoint(ch/2,cw/2)));

    mCanvas->setFixedSize(CanvasW,CanvasH);
    qDebug() << " this size " << this->geometry();
    qDebug() << " this centralWidget size " << ui->centralWidget->geometry();
    qDebug() << "mCanvas pos " << mCanvas->pos();


    ui->mainToolBar->addWidget(new QPushButton("test"));

    /* 左边属性框 */
    lDock = new QDockWidget();
    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
    lDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
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
    lDockWidget->setStyleSheet("QGroupBox{border: 1px solid gray;top: 18px; padding: 6px}");

   // leftLayout->addWidget(lList);
    CompoentControls *cc = new  CompoentControls(lDockWidget);
    leftLayout->addWidget(cc);


    lList->setFixedHeight((lDock->size().height() -50) / 2);



    //QStackedLayout *propertyStack = new QStackedLayout();
  //  this->rightStackedLayout()->setObjectName("ObjProperty");
    //propertyStack->setObjectName("ObjProperty");
    QFrame *qcl = new QFrame();

    QVBoxLayout *propertyLayout = new QVBoxLayout(qcl);
    propertyLayout->addWidget(propertyWidget);
    propertyLayout->addWidget(imgPropertyWidget);


    leftLayout->addWidget(qcl);
   // leftLayout->addWidget(propertyWidget);
   // propertyWidget->setStyleSheet("{top: 12px; padding: 1px;}");

   // leftLayout->addSpacerItem(new QSpacerItem(10,10));
    //propertyWidget->setStyleSheet("QGroupBox{border: 1px solid gray;}");
    //leftLayout->addWidget(imgPropertyWidget); /* 单个图片的属性组 */


    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/krfb.png")),tr("test")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/kwin.png")),tr("kwin")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/kuser.png")),tr("kuser")));
    lList->addItem(new QListWidgetItem(QIcon(tr("/usr/share/icons/mate/48x48/apps/calc.png")),tr("calc")));
    addDockWidget(Qt::LeftDockWidgetArea, lDock);
    tree = new TreeDock(this,this);

   // addDockWidget(Qt::LeftDockWidgetArea,new TreeDock(this));
    splitDockWidget(lDock,tree,Qt::Horizontal);
    lList->setFixedWidth(160);
    QScrollArea *defcom = new QScrollArea();




    /*rDock = new QDockWidget(tr("right"));
    rDock->setObjectName("RightDock");
    rDock->setAllowedAreas(Qt::RightDockWidgetArea);

    rList = new QListWidget(rDock);
    rList->setObjectName("rDockListWidget");
    addDockWidget(Qt::RightDockWidgetArea, rDock);
    rDock->setWidget(rList);
    rList->setFixedWidth(160);*/

    /*
    QString filename =  QDir::currentPath() + "/menu_strip.json";
    //qDebug() << " json file name " << filename;
    QFileInfo qfi(filename);
    if(!qfi.exists())
    {
        QMessageBox::warning(this,tr("错误"),tr("找不到控件文件"));
        return;
    }

    HandleJson *hj = new HandleJson(filename);
  //  mCanvas->setProperty("PropertyStack",QVariant::fromValue(propertyStack));
    QWidgetList qwlist = qApp->allWidgets();


    QWidget *ww = (QWidget*)(hj->CreateObjectFromJson(hj->mJsonMap,mCanvas));
    qDebug() << "New object Rect " << ww->geometry()
             << " Pos " <<  ww->mapToParent(ww->pos());
    delete hj;
            */
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
            ui->debugEdit->setText( msg );


            qDebug() << " mapto global or parent";

            pxy = this->mapToParent(xy);
            gxy = this->mapToGlobal(xy);

            qDebug() << " px : " << pxy.x() << ",px : " << pxy.y()
                     << " , gx : " << gxy.x() << ", gy : " << gxy.y() ;




   // event->accept();
    return false;
}


void MainWindow::mousePressEvent(QMouseEvent *ev)
{
     ui->statusBar->showMessage(QString("mouse x:%1 , y:%2 ")
                    .arg(QString::number(ev->pos().rx()))
                    .arg(QString::number(ev->pos().ry()))  );
}




MainWindow::~MainWindow()
{
    delete ui;
}

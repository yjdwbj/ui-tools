#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "handlejson.h"
#include "compoentcontrols.h"
#include "propertybox.h"
#include "scenesscreen.h"
#include "canvasmanager.h"
#include "pageview.h"


#include <QStandardPaths>
#include <QStyleFactory>
#include <QRegion>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    out(stdout, QIODevice::WriteOnly),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    cManager = new CanvasManager(this);
    propertyWidget = new ComProperty(this) ;
    imgPropertyWidget = new ImgProperty(this);
  //  QStringList sflist = QStyleFactory::keys();
    setWindowTitle(tr("图片编辑工具"));
    //qDebug() << " list " << sflist;
    // 　  this->setStyle(QStyleFactory::create("GTK+"));

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    //    Scenes = new ScenesScreen(QSize(Width,Height),ui->centralWidget);
    //    Scenes->addMainWindow(this);
    //    Scenes->move(this->width() * 0.12,this->height()* 0.3);  // 按屏幕比例调整

    //    ssList->append(Scenes);

//    QPushButton *newPrj = new QPushButton(tr("新建工程"));
//    ui->mainToolBar->addWidget(newPrj);

//    connect(newPrj,SIGNAL(clicked(bool)),SLOT(onCreateNewProject()));

//    QPushButton *newPage = new QPushButton(tr("新建页面"));
//    connect(newPage,SIGNAL(clicked(bool)),SLOT(onCreateNewScenesScreen()));
//    ui->mainToolBar->addWidget(newPage);

//    QPushButton *delPage = new QPushButton(tr("删除当前页"));
//    connect(delPage,SIGNAL(clicked(bool)),SLOT(onDelCurrentScenesScreen()));
//    ui->mainToolBar->addWidget(delPage);

    // 左边属性框
    lDock = new QDockWidget();
    lDock->setEnabled(false);
    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
    lDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
   // lDock->setFixedWidth(this->size().width() * 0.15);
    lDock->setObjectName("LeftDock");
   // lList = new QListWidget();
    // lDock->setWidget(lList);

    QWidget *lDockWidget = new QWidget(lDock);
    lDockWidget->setObjectName("lDockWidget");
    lDockWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    lDockWidget->setFixedWidth(this->width() * 0.15);

    lDock->setWidget(lDockWidget);



    QVBoxLayout *leftLayout = new QVBoxLayout();

    // lDock->setLayout(leftLayout);
    leftLayout->setObjectName("leftLayout");
    lDockWidget->setLayout(leftLayout);
   // lDockWidget->setStyleSheet("QGroupBox{border: 1px solid gray;top: 18px; padding: 6px}");

    // leftLayout->addWidget(lList);
    ComCtrl = new  CompoentControls(this,lDockWidget);

    leftLayout->addWidget(ComCtrl);




    QFrame *qcl = new QFrame();

    QVBoxLayout *propertyLayout = new QVBoxLayout(qcl);
    // 控件属性框
    //*pb = new PropertyBox();
    //propertyLayout->addWidget(pb);
    leftLayout->addWidget(propertyWidget);
    leftLayout->addWidget(imgPropertyWidget);



   // leftLayout->addWidget(qcl);


    addDockWidget(Qt::LeftDockWidgetArea, lDock);
    tree = new TreeDock(this);
//    tree->addCompoentControls(cc);
//    tree->addPropBox(propertyWidget);


    splitDockWidget(lDock,tree,Qt::Horizontal);


    //　右边的截图页面.
    pageView = new PageView(this);
    pageView->addMainWindow(this);
    pageView->setAllowedAreas( Qt::RightDockWidgetArea);
    pageView->setFeatures(QDockWidget::NoDockWidgetFeatures);

    addDockWidget(Qt::RightDockWidgetArea,pageView);



}

void MainWindow::addWidgetToToolBar(QWidget *w)
{
    ui->mainToolBar->addWidget(w);
}

//void MainWindow::onCreateNewScenesScreen()
//{
//    cManager->createNewCanvas();
//}


//void MainWindow::onDelCurrentScenesScreen()
//{

//    //if(QMessageBox::warning(this,"删除提示","你真的要删除当前页面吗?删除之后不可以撤消,请选择<确认>删除.") )
//        QMessageBox msgBox;
//        msgBox.setWindowTitle("删除提示");
//        msgBox.setText("你真的要删除当前页面吗?删除之后不可以撤消,请选择<删除>删除.");
//       // msgBox.setInformativeText("Do you want to save your changes?");
//        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
//        msgBox.setButtonText(0x4000,"删除");
//        msgBox.setButtonText(0x400000,"取消");
//        msgBox.setDefaultButton(QMessageBox::Cancel);
//        int ret = msgBox.exec();
//        qDebug() << " QMessageBox result " << ret;
//        if(ret == QMessageBox::Yes)
//        {
//            cManager->deleteCurrentPage();
//        }

//}

//void MainWindow::onCreateNewProject()
//{
//    ProjectDialog *pd = new ProjectDialog(this);
//    pd->exec();
//    pd->deleteLater();
//   // this->cManager->setDefaultPageSize(pd->getDefaultSize());
//   // qDebug() << " default page size " << pd->getDefaultSize();
//}

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

        }else if(event->type() == QEvent::MouseMove)
        {

        }
    }

    if ( event->type() != QEvent::MouseButtonPress )
        return false;


    const QMouseEvent* const me = static_cast<const QMouseEvent*>( event );
    //            int x = me->pos().rx();
    //            int y = me->pos().ry();
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
    //ui->debugEdit->setText( msg );


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

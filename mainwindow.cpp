#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "handlejson.h"
#include "compoentcontrols.h"
//#include "propertybox.h"
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
    lDock = ui->dockWidget;
    cManager = new CanvasManager(this);
     posWidget = new Position(this);
    propertyWidget = new ComProperty("控件属性",this) ;
    imgPropertyWidget = new ComProperty("图片属性",this);
  //  QStringList sflist = QStyleFactory::keys();
    setWindowTitle(tr("图片编辑工具"));
    //qDebug() << " list " << sflist;
    // 　  this->setStyle(QStyleFactory::create("GTK+"));

  //  QApplication::setStyle(QStyleFactory::create("Fusion"));

    // 左边属性框
//    lDock = new QDockWidget();
//    lDock->setStyleSheet("QWidget{border: 0.5px solid red;}");
//    lDock->setEnabled(false);
//    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
//    lDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
//    lDock->setFixedWidth(this->size().width() * 0.20);
//    lDock->setObjectName("LeftDock");



   // QWidget *lDockWidget = new QWidget(lDock);
    ui->dockWidget->setFixedWidth(this->width()*0.12);
    ui->dockWidget->setEnabled(false);

    //　qDockWidget 下面必需要发放一个QWidget ,　才能显示控件.
    QWidget *lDockWidget = new QWidget(ui->dockWidget);

    ui->dockWidget->setWidget(lDockWidget);
    lDockWidget->setObjectName("lDockWidget");
    lDockWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QVBoxLayout *leftLayout = new QVBoxLayout(lDockWidget);

    leftLayout->setObjectName("leftLayout");
   // leftLayout->setSizeConstraint(QLayout::SetFixedSize);
    lDockWidget->setLayout(leftLayout);

    ComCtrl = new  CompoentControls(this,ui->dockWidget);
    leftLayout->addWidget(ComCtrl);
    leftLayout->addWidget(posWidget);


//    leftLayout->addWidget(ComCtrl);

//    // 控件属性框

    leftLayout->addWidget(propertyWidget);
    leftLayout->addWidget(imgPropertyWidget);


   // addDockWidget(Qt::LeftDockWidgetArea, lDock);
    tree = new TreeDock(this);
    // 左边两个并排的QDockWidget
    splitDockWidget(ui->dockWidget,tree,Qt::Horizontal);


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

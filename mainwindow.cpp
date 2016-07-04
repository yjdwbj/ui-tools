#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "compoentcontrols.h"
#include "scenesscreen.h"
#include "canvasmanager.h"
#include "pageview.h"


#include <QStandardPaths>
#include <QStyleFactory>
#include <QRegion>
#include <QMessageBox>
#include <QStandardPaths>


void Backgroud::paintEvent(QPaintEvent *)
{
      // 这里必需要得新定义一个类,重写它的paintEvent才能画出它的背景.
      QPainter p(this);
      p.drawPixmap(this->rect(),QPixmap(backImage));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    //out(stdout, QIODevice::WriteOnly),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    QString iniFile  =  QStandardPaths::displayName(QStandardPaths::DataLocation) + "/ui-config";
    globalSet= new QSettings(iniFile,QSettings::IniFormat);
    bk = new Backgroud();
    QString stylestr = "QPushButton::hover{"\
                        "background: #F48024}";
                       //"background: #5EBA7D}";

    setStyleSheet(stylestr);
    setCentralWidget(bk);
   // lDock = ui->dockWidget;
    cManager = new CanvasManager(this);
     posWidget = new Position(this);
    propertyWidget = new ComProperty("控件属性",this) ;
    imgPropertyWidget = new ComProperty("图片属性",this);

    setWindowTitle(WIN_TITLE);

    // 左边属性框
    lDock = new QDockWidget();
   // lDock->setStyleSheet("QWidget{border: 0.5px solid red;}");
    lDock->setEnabled(false);
    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
    lDock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
    lDock->setFixedWidth(this->size().width() * 0.14);
    lDock->setObjectName("LeftDock");


    //　qDockWidget 下面必需要发放一个QWidget ,　才能显示控件.
    QWidget *lDockWidget = new QWidget(lDock);

    lDock->setWidget(lDockWidget);
    lDockWidget->setObjectName("lDockWidget");
    lDockWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QVBoxLayout *leftLayout = new QVBoxLayout(lDockWidget);

    leftLayout->setObjectName("leftLayout");
   // leftLayout->setSizeConstraint(QLayout::SetFixedSize);
    lDockWidget->setLayout(leftLayout);

    ComCtrl = new  CompoentControls(this,lDock);
    leftLayout->addWidget(ComCtrl);
    leftLayout->addWidget(posWidget);


//    leftLayout->addWidget(ComCtrl);

//    // 控件属性框

    leftLayout->addWidget(propertyWidget);
    leftLayout->addWidget(imgPropertyWidget);

    tree = new TreeDock(this);
    // 左边两个并排的QDockWidget
    addDockWidget(Qt::LeftDockWidgetArea, tree);
    splitDockWidget(tree,lDock,Qt::Horizontal);


    //　右边的截图页面.
    pageView = new PageView(this);
   // pageView->setFloating(true);
    pageView->addMainWindow(this);
    pageView->setAllowedAreas( Qt::RightDockWidgetArea);
    pageView->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);

    addDockWidget(Qt::RightDockWidgetArea,pageView);

    // 缓存一些背景图片.
    QString dir = QDir::currentPath() + "/backgrounds";
    QDirIterator it(dir, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString fpath = it.next();
        bool b = fpath.contains('/');
        int idx = fpath.lastIndexOf(b ? '/' : '\\')+1;
        bakimageMap[fpath.mid(idx)] = QPixmap(fpath);
    }
    QVariant bkvar = globalSet->value(INI_PRJBAKIMG);
    if(bkvar.isValid())
    {
        bk->backImage = bakimageMap[bkvar.toString()];
    }
    else
    {
      bk->backImage = bakimageMap.first();
    }

    this->centralWidget()->update();

    // 如果可以自动打开上次的工程
    QVariant prjvar = globalSet->value(INI_PRJLAST);
    QFile PrjFile;
    if(prjvar.isValid())
    {

        PrjFile.setFileName(prjvar.toString());
    }
    else
    {
        PrjFile.setFileName("save.json");
    }



    if (PrjFile.open(QFile::ReadOnly|QIODevice::Text)) {
        QByteArray qba = PrjFile.readAll();
        QTextStream in(&PrjFile);
        QString str;
        int ans = 0;
        in >> str >> ans;
        QJsonParseError json_error;
        QJsonDocument qd = QJsonDocument::fromJson(qba,&json_error);
        if(json_error.error == QJsonParseError::NoError)
        {

            QPoint mpos;
            if(qd.isObject())
            {
                QJsonObject  qdobj = qd.object();
                setWindowTitle(qdobj[NAME].toString());

                cManager->readProjectJson(qdobj[PAGES].toArray());
                cManager->setActiveSS(qdobj[ACTPAGE].toInt());

            }
        }else{
            // qDebug() << " read Json file error";
            qDebug() << json_error.errorString();
        }


    }

//    QJsonArray CanvasArray;
//   // QJsonObject root;
//    foreach (QWidget *w, mCanvasList) {
//        QJsonObject CanvasObj;
//       CanvasObj[NAME] = w->objectName();
//       ((ScenesScreen*)w)->writeToJson(CanvasObj);
//       CanvasArray.append(CanvasObj);
//      // qDebug() << "CanvasObj json  " << CanvasObj;

//    }
//    QJsonDocument jsonDoc(CanvasArray);
//    saveFile.write(jsonDoc.toJson());


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
    // event->accept();
    return false;
}


void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    ui->statusBar->showMessage(QString("mouse x:%1 , y:%2 ")
                               .arg(QString::number(ev->pos().rx()))
                               .arg(QString::number(ev->pos().ry()))  );

    if(ev->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        QAction chBakimg("修改背景",this);
        contextMenu->addAction(&chBakimg);
        connect(&chBakimg,SIGNAL(triggered(bool)),this,SLOT(onChangeBackgroud()));

        contextMenu->exec(ev->pos());
    }
}


void MainWindow::onChangeBackgroud()
{

    QDialog dig(this->centralWidget());
    dig.setFixedSize(this->centralWidget()->size() * 0.5);
    dig.setWindowTitle(tr("修改背景"));

    QVBoxLayout *v = new QVBoxLayout();
    dig.setLayout( v);
    QListWidget *imglist = new QListWidget();
    imglist->setSelectionMode(QAbstractItemView::SingleSelection);
    imglist->setViewMode(QListWidget::IconMode);
    imglist->setIconSize(QSize(160,140));
    //connect(imglist,SIGNAL(itemDoubleClicked(QListWidgetItem*)),SLOT(onDobuleClickedImage(QListWidgetItem*)));
    connect(imglist,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(onDobuleClickedImage(QListWidgetItem*)));
    v->addWidget(imglist);
    QMapIterator<QString,QPixmap> it(bakimageMap);
    while(it.hasNext())
    {
        it.next();
        imglist->addItem(new QListWidgetItem(QIcon(it.value()),it.key()));
    }

    dig.setModal(true);
    dig.exec();

}

void MainWindow::onDobuleClickedImage(QListWidgetItem *item)
{
    bk->backImage = bakimageMap[item->text()];
    globalSet->setValue(INI_PRJBAKIMG,item->text());
    this->centralWidget()->update();
    //update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

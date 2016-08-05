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

#include "mydialog.h"


#include "libxls/xls.h"

#include <QSysInfo>

#include <QProcess>

using namespace xls;


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
    ui->mainToolBar->setAllowedAreas(Qt::TopToolBarArea);

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
    //imgPropertyWidget = new ComProperty("图片属性",this);

    setWindowTitle(WIN_TITLE);

    // 左边属性框
    lDock = new QDockWidget();
   // lDock->setStyleSheet("QWidget{border: 0.5px solid red;}");
    lDock->setEnabled(false);
    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
    lDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    lDock->setFixedWidth(this->size().width() * 0.14);
    lDock->setObjectName("LeftDock");


    //　qDockWidget 下面必需要发放一个QWidget ,　才能显示控件.
    QWidget *lDockWidget = new QWidget(lDock);

    lDock->setWidget(lDockWidget);
    lDockWidget->setObjectName("lDockWidget");
    lDockWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QVBoxLayout *leftLayout = new QVBoxLayout(lDockWidget);
    leftLayout->setSpacing(2);
    leftLayout->setMargin(0);
    leftLayout->setContentsMargins(0,0,0,0);


    leftLayout->setObjectName("leftLayout");
   // leftLayout->setSizeConstraint(QLayout::SetFixedSize);
    lDockWidget->setLayout(leftLayout);

    ComCtrl = new  CompoentControls(this,lDock);
    leftLayout->addWidget(ComCtrl);
    leftLayout->addWidget(posWidget);


//    leftLayout->addWidget(ComCtrl);

//    // 控件属性框

    leftLayout->addWidget(propertyWidget);
    //leftLayout->addWidget(imgPropertyWidget);

    tree = new TreeDock(this);
    // 左边两个并排的QDockWidget
    addDockWidget(Qt::LeftDockWidgetArea, tree);
    tree->setFeatures(QDockWidget::NoDockWidgetFeatures);
    splitDockWidget(tree,lDock,Qt::Horizontal);


    //　右边的截图页面.
    pageView = new PageView(this);
   // pageView->setFloating(true);
    pageView->addMainWindow(this);
    pageView->setAllowedAreas( Qt::RightDockWidgetArea);
    pageView->setFeatures(QDockWidget::NoDockWidgetFeatures);

    addDockWidget(Qt::RightDockWidgetArea,pageView);

    // 缓存一些背景图片.
    QString dir = QDir::currentPath() + QDir::separator() +"backgrounds";
    QDirIterator it(dir, QStringList() << "*.jpg", QDir::Files/*, QDirIterator::Subdirectories*/);
    while (it.hasNext())
    {
        QString fpath = it.next();
        bool b = fpath.contains('/');
        int idx = fpath.lastIndexOf(b ? '/' : '\\')+1;
        bakimageMap[fpath.mid(idx)] = QPixmap(fpath);
        bimgPath[fpath.mid(idx)] = fpath;
    }
    QVariant bkvar = globalSet->value(INI_PRJBAKIMG);
    if(bkvar.isValid())
    {
        bk->backImage = bakimageMap[bkvar.toString()];
    }
    else
    {
        if(!bakimageMap.isEmpty())
            bk->backImage = bakimageMap[0];
    }

    this->centralWidget()->update();
    show();  // 这里不能少.


    createCSVFile(QDir::currentPath()+"/行车记录仪.xls");
//    I18nLanguage *pd = new I18nLanguage(this);
//    pd->exec();
//      MenuItemDialog *md = new MenuItemDialog("m1",this);
//      md->exec();
//      qDebug() << " select text is " << md->text;








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

}

void MainWindow::readCSVFile(QString csvfile)
{

    QFile csv(csvfile);
    if(!csv.open(QIODevice::ReadOnly|QIODevice::Text))
        return;

    QVariantMap idCn;
    QStringList langList;
    QByteArray firstline=csv.readLine();
    foreach (QByteArray v, firstline.split(';')) {
       langList.append(QString::fromLocal8Bit(v.data()).trimmed());
    }

    langList.removeAt(0);
    while(!csv.atEnd())
    {
        QByteArray ba =  csv.readLine();
        QList<QByteArray> lba= ba.split(';');
        QListIterator<QByteArray> it(ba.split(';'));
        QString key = QString::fromLocal8Bit(it.next().data());
        QString val = QString::fromLocal8Bit( it.next().data());
        idCn[key] = val;
    }

}

void MainWindow::createCSVFile(QString xlsfile)
{
    QString sysname = QSysInfo::prettyProductName();

    QFileInfo info(xlsfile);


    QString outfile = info.absolutePath() + QDir::separator() +info.completeBaseName() + ".csv";
    if(QFileInfo::exists(outfile))
    {
        QFile::copy(outfile,outfile+".bak");
    }

    if(sysname.contains("Windows"))
    {

         /* 先用xls2csv.exe */
        QString cmd = QDir::currentPath() + QDir::separator() +  "xls2csv.exe";
        /* 先用xls2csv.exe */
        QProcess xlsprocess;
        xlsprocess.start(cmd,QStringList() << xlsfile);

        QTextStream rsyncStdoutStream(xlsprocess.readAllStandardOutput());
        bool once = true;
        while(!xlsprocess.waitForFinished())
        {
           QString line =  rsyncStdoutStream.readLine();
           if(line.isNull())
               break;

           if(once)
           {

               LanguageList =   line.split(';');
               if(LanguageList.size())
                   LanguageList.removeAt(0);
               once = false;
           }else
           {
               QStringList tmp = line.split(';');
               QString key = tmp[0].toLower().trimmed();
               itemMap[key]=tmp[1].trimmed() ;
               orderlist << key;
           }
          // qDebug() << line;
        }
        qApp->processEvents();


    }else
    {
        // 这里是非微软系统
        QString cmd = QDir::currentPath() + QDir::separator() +"xls2csv";

        QProcess xlsprocess;
        if(!QFileInfo::exists(cmd))
        {
            qDebug() << "  tools not exists ";
            return;
        }
        xlsprocess.start(cmd,QStringList() << xlsfile);
        xlsprocess.waitForFinished();
        QTextStream rsyncStdoutStream(xlsprocess.readAllStandardOutput());
        //while(!xlsprocess.waitForFinished())
        bool once = true;

        while(1)
        {
           QString line =  rsyncStdoutStream.readLine();
           if(line.isNull())
               break;

           if(once)
           {
               LanguageList =   line.split(';');
               if(LanguageList.size())
                   LanguageList.removeAt(0);
               once = false;
           }else
           {
               QStringList tmp = line.split(';');
               QString key = tmp[0].toLower().trimmed();
               itemMap[key]=tmp[1].trimmed() ;
               orderlist << key;

           }
          // qDebug() << line;
        }
        qApp->processEvents();
    }
}



void MainWindow::addWidgetToToolBar(QWidget *w)
{
    ui->mainToolBar->addWidget(w);
}



void MainWindow::mousePressEvent(QMouseEvent *ev)
{
//    ui->statusBar->showMessage(QString("mouse x:%1 , y:%2 ")
//                               .arg(QString::number(ev->pos().rx()))
//                               .arg(QString::number(ev->pos().ry()))  );

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

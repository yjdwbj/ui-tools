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
#include <stdio.h>
#include <unistd.h>



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


   // createCSVFile(QDir::currentPath()+"/行车记录仪.xls");
    QString path = QDir::currentPath()+"/行车记录仪.xls";
    QString csv = QDir::currentPath()+"/行车记录仪.csv";

   // gmain(path.toLocal8Bit().data());
     MreadExcelFile(path.toLocal8Bit().data());
    // readCSVFile(csv);
    // createCSVFile(path);
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



void MainWindow::MreadExcelFile(char *xlsfile)
{

   // static char  stringSeparator = 0;
  //  static char *lineSeparator = "\n";
    static char *fieldSeparator = ";";
    static char *encoding = "UTF-8";

    struct st_row_data* row;
    WORD cellRow, cellCol;
    xlsWorkBook* pWB;
    xlsWorkSheet* pWS;
    unsigned int i;
 //   int justList = 0;
    char *sheetName = "";
    int isFirstLine =0;

    // open workbook, choose standard conversion
    pWB = xls_open(xlsfile, encoding);
    if (!pWB) {

        QMessageBox msgBox;
        msgBox.setWindowTitle("XLS文件找不到");
        msgBox.setText("XLS文件找不到.");
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"确定");
        return ;
    }

    QFile cvsfile("qtread.csv");
    cvsfile.open(QIODevice::WriteOnly|QIODevice::Text);

    // check if the requested sheet (if any) exists
    if (sheetName[0]) {
        for (i = 0; i < pWB->sheets.count; i++) {
            if (strcmp(sheetName, (char *)pWB->sheets.sheet[i].name) == 0) {
                break;
            }
        }

        if (i == pWB->sheets.count) {

            QMessageBox msgBox;
            msgBox.setWindowTitle("XLS文件工作薄找不到");
            msgBox.setText("XLS文件工作薄找不到,正常的语言表列只在第一个工作薄.");
            // msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
            msgBox.setButtonText(QMessageBox::Yes,"确定");
            return ;
        }
    }

    // open and parse the sheet
    pWS = xls_getWorkSheet(pWB, 0);
    xls_parseWorkSheet(pWS);

    // process all rows of the sheet
    for (cellRow = 0; cellRow <= pWS->rows.lastrow; cellRow++) {
        int isFirstCol = 1;
        row = (struct st_row_data*)(xls_row(pWS, cellRow));
        // process cells

//        if (!isFirstLine) {
//            cvsfile.write("\n");
//        } else {
//            isFirstLine = 0;
//        }

        QStringList collist;
        for (cellCol = 0; cellCol <= pWS->rows.lastcol; cellCol++) {
            //printf("Processing row=%d col=%d\n", cellRow+1, cellCol+1);
            QString cellstr ;

            xlsCell *cell = xls_cell(pWS, cellRow, cellCol);

            if ((!cell) || (cell->isHidden)) {
                continue;
            }

            if (!isFirstCol) {
                printf("%s", fieldSeparator);
                cvsfile.write(";");
            } else {
                isFirstCol = 0;
            }

            // display the colspan as only one cell, but reject rowspans (they can't be converted to CSV)
            if (cell->rowspan > 1) {

                QMessageBox msgBox;
                msgBox.setWindowTitle("XLS文件内容不正确");
                msgBox.setText("XLS文件内容不正确,不能有单元合并的单元,请选择一个正解的文件.");
                // msgBox.setInformativeText("Do you want to save your changes?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
                msgBox.setButtonText(QMessageBox::Yes,"确定");

                //  fprintf(stderr, "Warning: %d rows spanned at col=%d row=%d: output will not match the Excel file.\n", cell->rowspan, cellCol+1, cellRow+1);
            }

            // display the value of the cell (either numeric or string)
            if (cell->id == 0x27e || cell->id == 0x0BD || cell->id == 0x203) {
                // OutputNumber(cell->d);
                cellstr = QString::number(cell->d).toUtf8().data();

            } else if (cell->id == 0x06) {
                // formula
                if (cell->l == 0) // its a number
                {

                    //OutputNumber(cell->d);
                    cellstr = QString::number(cell->d).toUtf8().data();

                } else {
                    if (!strcmp((char *)cell->str, "bool")) // its boolean, and test cell->d
                    {
                        // OutputString((int) cell->d ? "true" : "false");

                        cellstr =(int) cell->d ? "true" : "false";
                    } else if (!strcmp((char *)cell->str, "error")) // formula is in error
                    {
                        cellstr = "*error*";
                    } else // ... cell->str is valid as the result of a string formula.
                    {
                        cellstr = tr((char *)(cell->str));
                    }
                }
            } else if (cell->str != NULL) {
                cellstr = tr((char *)(cell->str));
            } else {
                //  OutputString("");
                // cvsfile.write("");
            }
            QString s = tr((char *)(cell->str));
            for(int i=0;i<s.size();i++)
            {
                if(s.at(i) < 32)
                {
                    s = s.mid(0,i);
                    break;
                }
            }
            collist <<  s;
            if(!cellRow)
            {
                foreach (QString v , collist) {
                    LanguageList << v;
                }
                LanguageList.removeFirst();

            }
            cvsfile.write(s.toUtf8().data());
        }
        if(collist.size())
        {
            itemMap[collist.first()] = collist.at(1);
            orderlist << collist.first();
        }
         cvsfile.write("\n");

    }
    orderlist.removeFirst();
    xls_close_WS(pWS);

    xls_close(pWB);

    cvsfile.close();

}





void MainWindow::readCSVFile(QString csvfile)
{

    QFile csv(csvfile);
    if(!csv.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::warning(this,"open xls file error","xls 打不开");
        return;
    }



    QStringList langList;
    QByteArray firstline=csv.readLine();
    QByteArrayList balist ;
    char comma = ',';
    char celcomma = ';';

    if(firstline.count(comma) > 20)
    {
        balist = firstline.split(comma);
    }else
    {
        balist = firstline.split(celcomma);
    }
    foreach (QByteArray v, balist) {
       LanguageList.append(QString::fromUtf8(v.data()).trimmed());
    }

    if(LanguageList.size())
        LanguageList.removeFirst();
    while(!csv.atEnd())
    {
        QByteArray ba =  csv.readLine();
        QList<QByteArray> lba;
        if(firstline.count(comma) > 20)
        {
            lba = ba.split(comma);
        }else
        {
            lba = ba.split(celcomma);
        }
        QListIterator<QByteArray> it(lba);
        QString key = QString::fromUtf8(it.next().data());
        QString val = QString::fromUtf8( it.next().data());
        itemMap[key] = val;
        orderlist << key;
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
        xlsprocess.waitForFinished();
        QTextStream rsyncStdoutStream(xlsprocess.readAllStandardOutput());
        bool once = true;

        QFile csvfile("debug.csv");
        csvfile.open(QIODevice::WriteOnly);
        QTextStream out(&csvfile);
        while(1)
        {
           QString line =  rsyncStdoutStream.readLine();
           if(line.isNull())
           {
               out << "empty line";
                  break;
           }
            out << line;

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

        csvfile.close();
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

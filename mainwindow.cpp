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
#include <QtConcurrent/QtConcurrent>
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
LoadImgTask::LoadImgTask(QWidget *parent)
{
    rotate = new BusyIndicator(parent);
}

void LoadImgTask::setDone()
{
    rotate->onStop();
}

void LoadImgTask::run()
{
    rotate->exec();
    rotate->deleteLater();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    //out(stdout, QIODevice::WriteOnly),
    ui(new Ui::MainWindow),
    mGlobalIniFile(QStandardPaths::displayName(QStandardPaths::DataLocation) + "/ui-config")
{

    ui->setupUi(this);
    mRootPathLen = QDir::currentPath().length()+1;
    ui->mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    mGlobalIniFile = mGlobalIniFile.toUtf8();
    mGlobalSet= new QSettings(mGlobalIniFile,QSettings::IniFormat);

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    bk = new Backgroud();
    QString stylestr = "QPushButton::hover{"\
                       "background: #F48024}"\
                       "QDialog {background-color: #FFFFBF};";
    //"background: #5EBA7D}";


    setStyleSheet(stylestr);
    setCentralWidget(bk);

    cManager = new CanvasManager(this);
    posWidget = 0;

    setWindowTitle(VERSION);


    // 左边属性框
    lDock = new QDockWidget(this);
    lDock->setEnabled(false);
    lDock->setAllowedAreas( Qt::LeftDockWidgetArea);
    lDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    lDock->setFixedWidth(this->size().width() * 0.14);
    lDock->setObjectName("LeftDock");

    //　qDockWidget 下面必需要发放一个QWidget ,　才能显示控件.
    QWidget *lDockWidget = new QWidget(lDock);

    lDock->setWidget(lDockWidget);
    lDockWidget->setObjectName("lDockWidget");
    lDockWidget->setSizePolicy(QSizePolicy::Preferred,
                               QSizePolicy::Preferred);
    QVBoxLayout *leftLayout = new QVBoxLayout(lDockWidget);
    leftLayout->setSpacing(2);
    leftLayout->setMargin(0);
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setObjectName("leftLayout");
    lDockWidget->setLayout(leftLayout);

    ComCtrl = new  CompoentControls(this);
    leftLayout->addWidget(ComCtrl);

    propertyWidget = new ComProperty(this) ;
    leftLayout->addWidget(propertyWidget);
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
    show();
    if(!QFileInfo(mGlobalIniFile).exists())
    {
        while (1){
            GlobalSettings gs(this);
            QSize ps = size();
            int w = ( ps.width() - gs.width() )/ 2;
            int h = (ps.height()-gs.height()) / 2;
            gs.move(gs.mapFromParent(QPoint(w,h)));
            gs.exec();
            if (gs.isSetFine()) break;
        }
        mGlobalSet->deleteLater();
        mGlobalSet= new QSettings(mGlobalIniFile,QSettings::IniFormat);
    }

    ComCtrl->ReadJsonWidgets();
    cManager->mProjectImageDir = mGlobalSet->value(INI_PRJIMAGEDIR).toString();
    if(!cManager->mProjectImageDir.compare("."))
    {
        cManager->mProjectImageDir = QDir::currentPath();
    }else
    {
        cManager->mProjectImageDir.prepend(BACKSLASH);
        cManager->mProjectImageDir.prepend(QDir::currentPath());
    }

    // 缓存一些背景图片.
    QString dir = QDir::currentPath().replace(SLASH,BACKSLASH) +BACKSLASH +"backgrounds";
    if(!QFileInfo::exists(dir))
    {
        QDir d(dir);
        d.mkdir(dir);
    }
    QDirIterator it(dir, QStringList() << "*.jpg", QDir::Files/*, QDirIterator::Subdirectories*/);
    while (it.hasNext())
    {
        QString fpath = it.next().toUtf8();
        int idx = fpath.lastIndexOf(BACKSLASH)+1;
        mImgMap[fpath] = QPixmap(fpath);
        bakimageList << fpath;
        bimgPath[fpath.mid(idx)] = fpath;
    }

    QVariant bkvar = mGlobalSet->value(INI_PRJBAKIMG);
    if(bkvar.isValid())
    {
        // bk->backImage = bakimageMap[bkvar.toString()];
        bk->backImage = mImgMap[bkvar.toString()];
    }
    else
    {
        bk->backImage = bakimageList.count() > 0 ? mImgMap[bakimageList.first()] :
                QPixmap() ;
    }

    this->centralWidget()->update();
    show();  // 这里不能少.


    QVariant langfile = mGlobalSet->value(INI_PRJMLANG);
    if(langfile.isValid())
    {
        readMultiLanguage(langfile.toString());
    }


    QVariant lastprj = mGlobalSet->value(INI_PRJLAST);
    if(lastprj.isValid())
        cManager->OpenProject(lastprj.toString());
}



void MainWindow::readMultiLanguage(QString file)
{
    QFileInfo finfo(file);
    if(finfo.exists())
    {
        QString ext = finfo.completeSuffix();
        if(!ext.compare("xls"))
        {
            readExcelFile(file.toLocal8Bit().data());
        }else if(!ext.compare("csv"))
        {
            readCSVFile(file);
        }
    }
}

void MainWindow::readExcelFile(char *xlsfile)
{
    static char *fieldSeparator = ";";
    static char *encoding = "UTF-8";

//    struct st_row_data* row;
    WORD cellRow =0, cellCol=0;
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

    // int n = QString(xlsfile).lastIndexOf(QDir::separator());
    QFile cvsfile(/*QString(xlsfile).mid(n) + QDir::separator() +*/ "qtread.csv");
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
    mOrderlist.clear();

    // process all rows of the sheet
    for (cellRow = 0; cellRow <= pWS->rows.lastrow; cellRow++) {
        int isFirstCol = 1;
//        row = (struct st_row_data*)(xls_row(pWS, cellRow));
        // process cells

        QStringList collist;
        for (cellCol = 0; cellCol <= pWS->rows.lastcol; cellCol++) {
            //printf("Processing row=%d col=%d\n", cellRow+1, cellCol+1);
            QString cellstr ;

            xlsCell *cell = xls_cell(pWS, cellRow, cellCol);

            if ((!cell) || (cell->isHidden)) {
                continue;
            }

            if (!isFirstCol) {
                //  printf("%s", fieldSeparator);
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
            collist <<  s.trimmed();

            cvsfile.write(s.toUtf8().data());
        }
        if(!cellRow)
        {

            mLanguageList.clear();
            foreach (QString v , collist) {
                mLanguageList << v;
            }
            mLanguageList.removeFirst();

        }
        if(collist.size())
        {
            mItemMap[collist.first()] = collist.at(1);
            mOrderlist << collist.first();
        }
        cvsfile.write("\n");

    }
    mOrderlist.removeFirst();
    xls_close_WS(pWS);

    xls_close(pWB);

    cvsfile.close();

}





void MainWindow::readCSVFile(QString csvfile)
{

    QFile csv(csvfile);
    if(!csv.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::warning(this,"open xls file error","xls 打不开,请查看[全局设置]里的路径目录是否正确");
        return;
    }


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
    mLanguageList.clear();
    foreach (QByteArray v, balist) {
        mLanguageList.append(QString::fromUtf8(v.data()).trimmed());
    }

    if(mLanguageList.size())
        mLanguageList.removeFirst();
    mOrderlist.clear();
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
        mItemMap[key] = val;
        mOrderlist << key;
    }

}

void MainWindow::addWidgetToToolBar(QWidget *w)
{
    if(!w)
    {
        ui->mainToolBar->addSeparator();
    }else
        ui->mainToolBar->addWidget(w);
}



void MainWindow::mousePressEvent(QMouseEvent *ev)
{

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
    QString tooltip = "<b><p>背景图片目录名是 'backgrounds'　</p>"\
                      "<p>把背景图片放在该目录下就可以显示了,只支持JPG格式</p></b>";
    v->addWidget(new QLabel(tooltip));
    imglist->setSelectionMode(QAbstractItemView::SingleSelection);
    imglist->setViewMode(QListWidget::IconMode);
    imglist->setIconSize(QSize(160,140));
    imglist->setToolTip(tooltip);

    connect(imglist,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(onDobuleClickedImage(QListWidgetItem*)));
    v->addWidget(imglist);
    foreach (QString path, bakimageList) {
        // int len = path.lastIndexOf(BACKSLASH)+1;
        imglist->addItem(new QListWidgetItem(QIcon(mImgMap[path]),
                                             bimgPath.key(path)));
    }
    dig.setModal(true);
    dig.exec();

}

void MainWindow::onDobuleClickedImage(QListWidgetItem *item)
{
    bk->backImage = mImgMap[bimgPath.value(item->text())];
    QString imgpath = bimgPath.value(item->text()).toUtf8().mid(mRootPathLen);
    if(imgpath.isEmpty()) imgpath=".";
    mGlobalSet->setValue(INI_PRJBAKIMG,imgpath);
    this->centralWidget()->update();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(cManager->mIsOpenProject)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("打开工程提示");
        msgBox.setText("当前编辑的工程有新的修改没有保存,选请择<保存>进行保存.");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"保存");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Yes)
        {
            //　需要保存
            cManager->onSaveProject();
        }
        cManager->closeCurrentProject();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

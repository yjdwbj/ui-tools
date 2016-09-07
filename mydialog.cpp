#include "mydialog.h"
#include "ui_dialog.h"
#include "ui_language.h"
#include "ui_confproject.h"
#include "ui_globalset.h"
#include <QMessageBox>
#include "mainwindow.h"
#include "canvasmanager.h"
//#include <QQmlApplicationEngine>
//#include <QQmlContext>
//#include <QQuickView>
#include <QDrag>
#include <QMimeData>
#include <QDataStream>
#include <QBitmap>
#include <QRadioButton>
#include <QFileDialog>
#include <QStyleFactory>
#include <QThread>


const static char DnditemData[] = "application/x-dnditemdata";


BaseDialog::BaseDialog(QWidget *parent):
    QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setModal(true);
   // setStyleSheet( QString("BaseDialog#%1 {background-color: #FFFFBF;}").arg(objectName()));
}


void BaseDialog::UpdateStyle()
{
    setStyleSheet( QString("BaseDialog#%1 {background-color: #FFFFBF;}").arg(objectName()));
    qDebug() << " basedialog stylesheet " << styleSheet();
}

ImageFileDialog::ImageFileDialog(QVariantList old, QString imgpath, QWidget *parent)
    :QDialog(parent),
//      sellist(new CustomListWidget()),
//      flistview(new CustomListWidget()),
      sellist(new QListWidget()),
      flistview(new QListWidget()),
      treefile(new QTreeView()),
   //   selstrList(old),
      statusBar(new QLabel(this)),
      okbtn(new QPushButton("确定",this))
{

    mWindow = (MainWindow*)parent;
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setModal(true);
    treefile->header()->setHidden(true);
     flistview->setProperty(DKEY_EXTMAP,extMap);
     /* 填弃上一次的数据 */
    dirModel = new QFileSystemModel(this);
    fileModel = new QFileSystemModel(this);
    this->setFixedSize(1000,600);

    sellist->setSelectionMode(QAbstractItemView::ExtendedSelection);
    sellist->setAlternatingRowColors(true);
    flistview->setSelectionMode(QAbstractItemView::ExtendedSelection);
    flistview->setAlternatingRowColors(true);

  //  flistview->setViewMode(QListWidget::IconMode);
  //  sellist->setViewMode(QListWidget::IconMode);
    flistview->setIconSize(QSize(80,60));
    sellist->setIconSize(QSize(80,60));
    // flistview->setIconSize(QSize(200,180));
    //  dirModel->setReadOnly(true);
    //  fileModel->setReadOnly(true);

    QMap<QString,QString> navigator;
    navigator[UP] = ":/icon/icons/buildstepmoveup@2x.png";
    navigator[DOWN] = ":/icon/icons/buildstepmovedown@2x.png";
    navigator[LEFT] = ":/icon/icons/prev@2x.png";
    navigator[RIGHT] = ":/icon/icons/next@2x.png";


    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *mh = new QHBoxLayout();
    this->setLayout(mainLayout);
    mainLayout->addLayout(mh);
    this->setWindowTitle(tr("图片编辑"));
    QColor white(255,255,255);
    QVBoxLayout *v = new QVBoxLayout();
    v->setSizeConstraint(QLayout::SetFixedSize);


    QMapIterator<QString,QString> itmap(navigator);
    while(itmap.hasNext())
    {
        itmap.next();
        QPushButton *btn = new QPushButton();
        QPixmap p(itmap.value());
        btn->setIconSize(p.size());
        p.setMask(p.createMaskFromColor(Qt::white));
        btn->setIcon(QIcon(p));



        btn->setObjectName(itmap.key());
        btn->setFixedSize(btn->iconSize());
        v->addWidget(btn);
        if(!LEFT.compare(itmap.key()))
        {
            connect(btn,SIGNAL(clicked(bool)),SLOT(onAddSelectedItems()));
        }else if(!UP.compare(itmap.key()))
        {
            connect(btn,SIGNAL(clicked(bool)),SLOT(onUp()));
        }else if(!DOWN.compare(itmap.key()))
        {
            connect(btn,SIGNAL(clicked(bool)),SLOT(onDown()));
        }else if(!RIGHT.compare(itmap.key()))
        {
            connect(btn,SIGNAL(clicked(bool)),SLOT(onDelSelectedItems()));
        }
    }

    dirModel->setRootPath(imgpath);
    dirModel->removeColumn(3);
    dirModel->removeColumn(2);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);


    filters << "*.bmp" << "*.png" << "*.jpg";
    dirModel->setNameFilters(filters);


    treefile->setModel(dirModel);
    treefile->setRootIndex(dirModel->index(imgpath));
    treefile->setAlternatingRowColors(true);

    treefile->hideColumn(3);
    treefile->hideColumn(2);
    treefile->hideColumn(1);


    fileModel->setRootPath(imgpath);
    fileModel->setNameFilters(filters);
    fileModel->setFilter(QDir::Files);
//    flistview->setModel(fileModel);
//    flistview->setRootIndex(fileModel->index(imgpath));

//    connect(sellist,SIGNAL(doubleClicked(QModelIndex)),
//            SLOT(onSelListViewDoubleClicked(QModelIndex)));
    connect(sellist,&QListWidget::clicked,
            [=](QModelIndex index){
        sellist->clearSelection();
        sellist->setCurrentIndex(index);
        findChild<QPushButton*>(UP)->setEnabled(index.row() == 0 ? false : true);
        findChild<QPushButton*>(DOWN)->setEnabled(index.row()
                                                  == sellist->count() -1 ? false : true);
    });
    connect(flistview,SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onListViewDoubleClicked(QModelIndex)));

    connect(treefile,SIGNAL(clicked(QModelIndex)),
            SLOT(onTreeViewClicked(QModelIndex)));

    /* 主布局是水平布局,左(QListWidget),中(垂直布局),右(文件系统) */
    // sellist->setModel(strListMode);
    mh->addWidget(sellist);
    mh->addLayout(v);
    mh->addWidget(flistview);
    mh->addWidget(treefile);


    QHBoxLayout *mlayout = new QHBoxLayout();
    mlayout->setSizeConstraint(QLayout::SetMaximumSize);
    QLabel *msg  = new QLabel("已经添加的图片数:");
    //statusBar->setStyleSheet("background-color: #aa1100;");
    statusBar->setText("0");
    mlayout->addWidget(msg);
    mlayout->addWidget(statusBar);
    mlayout->addWidget(okbtn);
    statusBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    mainLayout->addLayout(mlayout);

    foreach (QVariant v, old) {
        // example for v "config/images/digital-0.png"
        QString str = v.toString();
        QString fpath = QDir::currentPath() + BACKSLASH + str;
        QPixmap pic = mWindow->mImgMap[fpath];
        if(pic.isNull())
        {
            pic = mWindow->mImgMap[fpath] = QPixmap(fpath);
        }
        QString sname = str.mid(str.lastIndexOf(BACKSLASH)+1);
        QListWidgetItem *nitem = new QListWidgetItem(pic,sname);
        sellist->addItem(nitem);
        selectedMap[sname] = fpath;
    }
    statusBar->setText(QString::number(sellist->count()));

    updateListImages(imgpath);
    this->setModal(true);
//    UpdateStyle();
    connect(okbtn,SIGNAL(clicked(bool)),SLOT(accept()));

}


void ImageFileDialog::updateListImages(QString path)
{
  //  imgMap.clear();
    flistview->clear();
    extMap.clear();
    QDirIterator it(path,filters, QDir::Files /*,QDirIterator::Subdirectories*/);
    while (it.hasNext())
    {
        QString fpath = it.next().toUtf8();
        int idx = fpath.lastIndexOf(BACKSLASH)+1;
        QString basename = fpath.mid(idx);
        extMap[basename] = fpath;
        bool isFind = false;
        for(int i =0; i < sellist->count();i++)
        {
           QListWidgetItem *item = sellist->item(i);
           if(!item->text().compare(basename))
           {
               // 这里有相同文件名了,所以在待选列表不再添加它,这里没有处理不同目录下的同名文件.都视为同名.
               isFind = true;
               break;
           }
        }


       // flistview->addItem(new QListWidgetItem(QPixmap(fpath),fpath.mid(idx)));
        QPixmap pic =  mWindow->mImgMap[fpath];
        if(pic.isNull())
        {
           pic = mWindow->mImgMap[fpath] = QPixmap(fpath);

        }
        flistview->addItem(new QListWidgetItem(pic,basename));
        if(isFind)
            flistview->setRowHidden(flistview->count()-1,true);
        qApp->processEvents();
    }

}

void ImageFileDialog::onListViewDoubleClicked(QModelIndex index)
{
    /* 双击添加到右框*/
    appendSelectedItem(index);
}
void ImageFileDialog::updateListWidget()
{
//    QWidget *first;
//    QWidget *last;
    for(int i = 0; i < sellist->count();i++)
    {
        QListWidgetItem *item = sellist->item(i);
         QWidget *w =  sellist->itemWidget(item);
         w->setProperty(DKEY_ROW,i);
         if(i == 0)
         {
           // first = w;
            w->findChild<QPushButton*>("up")->setEnabled(false);
         }else if(i == sellist->count() -1){
             //last = w;
             w->findChild<QPushButton*>("down")->setEnabled(false);
         }else
         {
             w->findChild<QPushButton*>("up")->setEnabled(true);
             w->findChild<QPushButton*>("down")->setEnabled(true);
         }

    }


}


QWidget* ImageFileDialog::createUpAndDownButton(int row)
{
    QPushButton *up =new  QPushButton();
    up->setProperty(DKEY_ROW,row);
    connect(up,SIGNAL(clicked(bool)),SLOT(onUp()));
    up->setObjectName(tr("up"));
    QPixmap uppix(":/icon/icons/arrowup.png");
    // 隐除PNG图片中的白色像素.
     QBitmap umask= uppix.createMaskFromColor(QColor(255,255,255),Qt::MaskInColor);
    uppix.setMask(umask);
    up->setIcon(uppix);


    QPushButton *down =new QPushButton();
    down->setProperty(DKEY_ROW,row);
    connect(down,SIGNAL(clicked(bool)),SLOT(onDown()));
    down->setObjectName(tr("down"));
    QPixmap downpix(":/icon/icons/arrowdown.png");
    QBitmap dmask= downpix.createMaskFromColor(QColor(255,255,255),Qt::MaskInColor);
    downpix.setMask(dmask);
    down->setIcon(downpix);

    up->setFixedSize(up->iconSize());
    down->setFixedSize(down->iconSize());


    QHBoxLayout *hb = new QHBoxLayout();

    QVBoxLayout *vb = new QVBoxLayout();
    vb->setSizeConstraint(QLayout::SetFixedSize);
    vb->setSpacing(1);
    //vb->addStretch();

    vb->addWidget(up);
    vb->addWidget(down);
    QWidget *w = new QWidget();
    hb->setSpacing(1);
    hb->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    hb->addLayout(vb);
    //hb->addStretch();
    w->setLayout(hb);
    return w;
}

void ImageFileDialog::appendSelectedItem(QModelIndex index)
{
    QString s = flistview->item(index.row())->text();
    QPixmap pic = mWindow->mImgMap[extMap[s].toString()];
    if(pic.isNull())
    {
        pic = mWindow->mImgMap[extMap[s].toString()] = QPixmap(extMap[s].toString());
    }
    QListWidgetItem *nitem = new QListWidgetItem(pic,s);
    sellist->addItem(nitem);
    selectedMap[s] = extMap[s].toString();
    flistview->setRowHidden(index.row(),true);
//    QListWidgetItem *item = flistview->takeItem(index.row());
//    delete item;
    hRows[s] = index;
   // selMap[s] = fileModel->fileInfo(index).absoluteFilePath();
    /* 这里不能使用MAP , QComobox 需要排序 */
   // selstrList.append(QString("%1:%2").arg(s,fileModel->fileInfo(index).absoluteFilePath()));
   // 这里的每一条数据必需是下面格式:　　文件名:文件名的绝对完全路径
   // selstrList.append(QString("%1|%2").arg(s,extMap[s].toString()));
    statusBar->setText(QString::number(sellist->count()));
}

QVariantList ImageFileDialog::getSelectedList()
{
    QVariantList lst;
    for(int i = 0 ; i < sellist->count();i++)
    {
        QListWidgetItem *item = sellist->item(i);
        QString str = item->text();
        lst << QString("%1|%2").arg(str,selectedMap[str].toString());
    }
    return lst;

}

void ImageFileDialog::onUp()
{
    QPushButton *btn = (QPushButton*)(QObject::sender());
//    int row = btn->property(DKEY_ROW).toInt();
    int row = sellist->currentRow();


     QListWidgetItem *item =  sellist->takeItem(row);
     sellist->insertItem(row -1,item);
     sellist->setCurrentItem(item);
     btn->setEnabled(row -1 == 0 ? false : true);
}

void ImageFileDialog::onDown()
{
    QPushButton *btn = (QPushButton*)(QObject::sender());
    int row = sellist->currentRow();
     QListWidgetItem *item =  sellist->takeItem(row);
     sellist->insertItem(row +1,item);
     sellist->setCurrentItem(item);
     btn->setEnabled(row + 1 == sellist->count() -1 ? false : true);
}

void ImageFileDialog::onSelListViewDoubleClicked(QModelIndex index)
{
    /* 双击从右框删除 */
    // sellist->takeItem(index.row())->text();
    QString selstr = sellist->takeItem(index.row())->text();
    //sellist->setRowHidden(index.row(),true);
    delete sellist->takeItem(index.row());
    foreach(QListWidgetItem *fitem,flistview->findItems(selstr,
                                              Qt::MatchExactly))
    {
        flistview->setRowHidden(flistview->row(fitem),false);
        break;
    }
    selectedMap.remove(selstr);
    /* 从列表删除 */

   statusBar->setText(QString::number(sellist->count()));
   sellist->clearSelection();
//   statusBar->repaint();
}

void ImageFileDialog::onDelSelectedItems()
{
    QList<QListWidgetItem*> items = sellist->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        QString selstr = item->text();
        foreach(QListWidgetItem *fitem,flistview->findItems(selstr,
                                                  Qt::MatchExactly))
        {
            flistview->setRowHidden(flistview->row(fitem),false);
            break;
        }

        selectedMap.remove(selstr);

        delete sellist->takeItem(sellist->row(item));
    }
   // updateListWidget();
    sellist->clearSelection();
}


void ImageFileDialog::onAddSelectedItems()
{
    QModelIndexList qil = flistview->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, qil)
    {
        appendSelectedItem(index);
    }
    statusBar->setText(QString::number(sellist->count()));
    flistview->clearSelection();
}

ImageFileDialog::~ImageFileDialog()
{
    delete dirModel;
    delete fileModel;
}

void ImageFileDialog::onTreeViewClicked(QModelIndex index)
{
    QString mPath = dirModel->fileInfo(index).absoluteFilePath();
    updateListImages(mPath);

    // dirModel->fetchMore(index);
    treefile->setExpanded(index,true);
    treefile->expand(index);
    treefile->setCurrentIndex(dirModel->index(mPath));
}



ProjectDialog::ProjectDialog(QWidget *parent)
    :QDialog(parent),
    //:QDialog(parent),
    ui(new Ui::ProjectDialog),
    defaultXLS(QDir::currentPath().replace(SLASH,BACKSLASH) + BACKSLASH + "行车记录仪.xls")
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setModal(true);

    setObjectName(this->metaObject()->className());
//    this->UpdateStyle();
    //setStyleSheet( QString("ProjectDialog#%1 {background-color: #FFFFBF;}").arg(metaObject()->className()));

    mWindow = (MainWindow*)(parent);
    setWindowTitle("新建工程");
    setModal(true);
    connect(this,SIGNAL(accepted()),SLOT(onAccepted()));
    connect(this,SIGNAL(rejected()),SLOT(onRejected()));
    connect(this,SIGNAL(finished(int)),SLOT(onFinished(int)));
    QVariant langfile = mWindow->mGlobalSet->value(INI_PRJMLANG);
    if(langfile.isValid())
    {
        defaultXLS = langfile.toString();
    }

    ui->view_lang->setText(defaultXLS);
    CheckLangFile(defaultXLS);

    QVariant sizeVar =  mWindow->mGlobalSet->value(INI_PRJSIZE);
     int w,h;
    if(sizeVar.isValid())
    {
        QString prjsize = sizeVar.toString();
        w = prjsize.section("*",0,0).toInt();
        h = prjsize.section("*",1,1).toInt();

    }else{
        w = 128;
        h = 128;
    }

    QSize pdsize(w,h);
    ui->spinBox->setMinimum(64);
    ui->spinBox_2->setMinimum(64);
    ui->spinBox->setMaximum(parent->width()*0.8);
    ui->spinBox_2->setMaximum(parent->height()*0.8);
    ui->spinBox->setValue(w);
    ui->spinBox_2->setValue(h);


    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("创建");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");

    //setLayout();
}

bool ProjectDialog::CheckLangFile(QString path)
{
    bool ret  = QFileInfo::exists(path);
    if(ret)
    {
        ui->filestatus->setPixmap(QPixmap(":/icon/icons/xfail.png"));
    }else
    {

        ui->filestatus->setPixmap(QPixmap(":/icon/icons/xpass.png"));
        ui->filestatus->setText("多国语言文件不存在");
    }

    return ret;
}

void ProjectDialog::on_pushButton_clicked()
{


   QFileInfo definfo(defaultXLS);
   QString xlsfile = QFileDialog::getOpenFileName(Q_NULLPTR,
                                                  tr("打开多国语言文件"),
                                                  definfo.absolutePath(),
                                                  //tr("xls files (*.xls);;CSV UTF-8 (*.csv)"));
                                                   tr("xls files , CSV UTF-8 file (*.xls *.csv )"));
//                                                  Q_NULLPTR,
//                                                  QFileDialog::ExistingFile
//                                                  );
   if(!xlsfile.isEmpty() || CheckLangFile(xlsfile))
   {
      mWindow->mGlobalSet->setValue(INI_PRJMLANG, xlsfile.toUtf8().mid(mWindow->mRootPathLen) );
      ui->view_lang->setText(xlsfile);

   }

}

ProjectDialog::~ProjectDialog()
{
    delete ui;
}

QSize ProjectDialog::getDefaultSize()
{
    return QSize(ui->spinBox->value(),ui->spinBox_2->value());
}

void ProjectDialog::closeEvent(QCloseEvent *)
{
   // qDebug() << " close result is " << this->result();
}

void ProjectDialog::onAccepted()
{

     //  qDebug() << " trigger accepted event ";
       if(ui->spinBox->value() == 0 ||
               ui->spinBox_2->value() == 0)
       {
           QMessageBox::warning(this,"提示","宽高不能设置为零.");
       }


       mWindow->cManager->setDefaultPageSize(QSize(ui->spinBox->value(),ui->spinBox_2->value()));
       mWindow->mGlobalSet->setValue(INI_PRJSIZE,
                                   QString("%1*%2").arg(QString::number(ui->spinBox->value()),
                                                       QString::number(ui->spinBox_2->value())));


       mWindow->setWindowTitle(VERSION + ui->prjname->text());
       mWindow->cManager->mProjectName = ui->prjname->text();
}

void ProjectDialog::onRejected()
{
    qDebug() << " trigger reject event ";
}

void ProjectDialog::onFinished(int result)
{
    qDebug() << " trigger finished " << result;
}


void ProjectDialog::onSpinBoxVChanged(int v)
{
    qDebug() << " spin box value changed " << v << " get " << ui->spinBox->value();
}

ImageListView::ImageListView(QString path, QWidget *parent)
    :QDialog(parent),
     fileModel(new QFileSystemModel),
     treeModel(new QFileSystemModel),
     treefile(new QTreeView()),
     imglist(new QListWidget)
{
    mWindow = (MainWindow*)parent;
    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setModal(true);
    QVBoxLayout *mvb = new QVBoxLayout();
    QHBoxLayout *mh = new QHBoxLayout();
    mvb->addLayout(mh);
    this->setLayout(mvb);
    this->setWindowTitle(tr("图片编辑"));
    filters << "*.bmp" << "*.png" << "*.jpg";
    treeModel->setRootPath(path);
    //treeModel->removeColumn(3);
    //treeModel->removeColumn(2);
    treeModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    //  QPushButton *okbtn = new QPushButton("确定",this);
    // okbtn->setFixedWidth(60);
    // connect(okbtn,SIGNAL(clicked(bool)),SLOT(accept()));
    setToolTip("双击选中图片并更新到控件显示.");
    treeModel->setNameFilters(filters);
    treeModel->setHeaderData(1,
                             Qt::Horizontal,
                             "目录",Qt::DisplayRole);
    treefile->setModel(treeModel);
    treefile->header()->model()->setHeaderData(0, Qt::Horizontal,
                                               "目录");
    treefile->header()->setHidden(true);


    treefile->setRootIndex(treeModel->index(path));
    treefile->setAlternatingRowColors(true);

    treefile->hideColumn(3);
    treefile->hideColumn(2);
    treefile->hideColumn(1);


    fileModel->setRootPath(path);
    fileModel->setNameFilters(filters);
    fileModel->setFilter(QDir::Files);
    //    imglist->setModel(fileModel);
    //    imglist->setRootIndex(fileModel->index(path));



    imglist->setSelectionMode(QAbstractItemView::SingleSelection);
    imglist->setViewMode(QListWidget::IconMode);
    imglist->setIconSize(QSize(160,140));
    imglist->setAlternatingRowColors(true);

    mh->addWidget(treefile);
    mh->addWidget(imglist);
    // mvb->addWidget(okbtn);



    treefile->setFixedWidth(160);
    connect(treefile,SIGNAL(clicked(QModelIndex)),SLOT(onTreeViewClicked(QModelIndex)));
    //    connect(imglist,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
    //            parent,SLOT(onSelectedBackgroundImage(QListWidgetItem*)));

    updateListImages(path);
}

void ImageListView::updateListImages(QString path)
{
    imgMap.clear();
    imglist->clear();

     QDirIterator it(path,filters, QDir::Files
                     /*, QDirIterator::Subdirectories*/);
    while (it.hasNext())
    {
        QString fpath = it.next().toUtf8();
//        bool b = fpath.contains('/');
//        int idx = fpath.lastIndexOf(b ? '/' : '\\')+1;
        int idx = fpath.lastIndexOf(BACKSLASH)+1;
        QString shortname = fpath.mid(idx);
        imgMap[shortname] = fpath;
        //imglist->addItem(new QListWidgetItem(QIcon(QPixmap(fpath)),fpath.mid(idx)));
        QPixmap pic = mWindow->mImgMap[fpath];
        if(pic.isNull())
        {
            pic = mWindow->mImgMap[fpath] = QPixmap(fpath);
        }
        imglist->addItem(new QListWidgetItem(pic,shortname));
        qApp->processEvents();
    }
    imglist->setProperty(DKEY_IMGMAP,imgMap);
}

void ImageListView::onTreeViewClicked(QModelIndex index)
{
    QString mPath = treeModel->fileInfo(index).absoluteFilePath();
  //  qDebug() << " DirModel AbsoluteFile Path " << mPath;
   // imglist->setRootIndex(fileModel->setRootPath(mPath));
    updateListImages(mPath);

    // dirModel->fetchMore(index);
    treefile->setExpanded(index,true);
    treefile->expand(index);
    treefile->setCurrentIndex(treeModel->index(mPath));
}


MenuItemDialog::MenuItemDialog( QString old, QWidget *parent)
    :QDialog(parent),
      listWidget(new QListWidget),
      text(old)
{
    mWindow = (MainWindow*)(parent);
    setWindowTitle("菜单条目列表");


    foreach (QString key , mWindow->mOrderlist) {
        QListWidgetItem* item = new QListWidgetItem(listWidget);
        QRadioButton *rb = new QRadioButton(mWindow->mItemMap[key]);
        connect(rb,&QRadioButton::toggled,[=](bool f){
           if(f)
               text = rb->text();
        });

        listWidget->setItemWidget(item,rb);
        rb->setChecked(!key.trimmed().compare(old));
    }



    connect(listWidget,&QListWidget::itemPressed,this,
            [=](QListWidgetItem *item)
            {
             listWidget->clearSelection();
             text = mWindow->mOrderlist[listWidget->row(item)]; });

    QVBoxLayout *layout= new QVBoxLayout();
    layout->addWidget(listWidget);
    setLayout(layout);
    setFixedSize(180,360);
    setModal(true);
}

I18nLanguage::I18nLanguage(QVariantList oldvar, QWidget *parent):
    QDialog(parent),
    ui(new Ui::I18nLanguage)
{

    ui->setupUi(this);
    mWindow = (MainWindow*)(parent);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setModal(true);

    //ui->langWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
    ui->itemWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
    ui->itemWidget->setAlternatingRowColors(true);
    foreach(QString v,mWindow->mOrderlist)
    {
        QListWidgetItem* item = new QListWidgetItem(mWindow->mItemMap.value(v),
                                                    ui->itemWidget);
        item->setData(Qt::CheckStateRole,Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    foreach(QVariant v,oldvar)
    {
        int n = mWindow->mOrderlist.indexOf(v.toString());
        QListWidgetItem* item = ui->itemWidget->item(n);
        item->setCheckState(Qt::Checked);
    }

    connect(ui->btn_ok,SIGNAL(clicked(bool)),SLOT(accept()));
    setModal(true);
//    UpdateStyle();
}

QStringList I18nLanguage::getSelectedItems()
{
    QStringList lst;
    for(int i = 0;i < ui->itemWidget->count();i++)
    {
        QListWidgetItem *item = ui->itemWidget->item(i);
        if(item->checkState() == Qt::Checked)
            lst.append(item->text());
    }
    return lst;
}

void I18nLanguage::on_item_selectall_clicked()
{
 //   ui->itemWidget->selectAll();
    for(int i = 0;i < ui->itemWidget->count();i++)
    {
        QListWidgetItem *item = ui->itemWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
}



void I18nLanguage::on_item_dselectall_clicked()
{
    //ui->itemWidget->clearSelection();
    for(int i = 0;i < ui->itemWidget->count();i++)
    {
        QListWidgetItem *item = ui->itemWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}

void I18nLanguage::on_item_re_clicked()
{
    for(int i = 0;i < ui->itemWidget->count();i++)
    {
        QListWidgetItem *item = ui->itemWidget->item(i);
        item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
       // item->setSelected(!item->isSelected());
    }
}


ConfigProject::ConfigProject(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConfigProject),
    defaultXLS(QDir::currentPath().replace(SLASH,BACKSLASH) + BACKSLASH + "行车记录仪.xls")

{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint| Qt::Dialog);
    mWindow = (MainWindow*)(parent);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");


    CheckLangFile(defaultXLS);

     ui->view_lang->setText(defaultXLS);


    QVariant qv = mWindow->mGlobalSet->value(INI_PRJMLANG);
    if(qv.isValid())
    {

        QString oldfile = qv.toString();
         defaultXLS = oldfile;
         if(CheckLangFile(oldfile))
         {
             ui->view_lang->setText(oldfile);
         }
    }


    ui->langWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);

    updateListWidget();
    setModal(true);

}


void ConfigProject::updateListWidget()
{

    int count =  ui->langWidget->count();
//    ui->langWidget->selectAll();
//    qDeleteAll(ui->langWidget->selectedItems());
    for (int i =0 ; i < count;i++ ) {
       delete ui->langWidget->takeItem(i);
    }
    ui->langWidget->clear();
    QStringList tlist = mWindow->cManager->mPrjSelectlang;
    foreach (QString v, mWindow->mLanguageList) {
        QListWidgetItem* item = new QListWidgetItem(v,ui->langWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable |
                       Qt::ItemIsEditable| Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      //  item->setCheckState(Qt::Unchecked); // AND initialize check state
        bool b = false;
        foreach (QString o,tlist ) {
            if(!v.compare(o))
            {
               // item->setCheckState(Qt::Checked);
                b = true;
                tlist.removeOne(o);
                break;
            }
        }
        item->setCheckState(b ? Qt::Checked : Qt::Unchecked);
    }
    count =  ui->langWidget->count();
    ui->lang_dselectall->setEnabled(count);
    ui->lang_re->setEnabled(count);
    ui->lang_selectall->setEnabled(count);
}

bool ConfigProject::CheckLangFile(QString path)
{
    bool ret  = QFileInfo::exists(path);
    if(ret)
    {
        ui->filestatus->setPixmap(QPixmap(":/icon/icons/xfail.png"));
    }else
    {

        ui->filestatus->setPixmap(QPixmap(":/icon/icons/xpass.png"));
        ui->filestatus->setText("多国语言文件不存在");
    }

    return ret;
}



void ConfigProject::on_openfile_clicked()
{
    QFileInfo definfo(defaultXLS);
    QString xlsfile = QFileDialog::getOpenFileName(Q_NULLPTR,
                                                   tr("打开多国语言文件"),
                                                   definfo.absolutePath(),
                                                   tr("xls files , CSV UTF-8 file (*.xls *.csv )"));
 //                                                  Q_NULLPTR,
 //                                                  QFileDialog::ExistingFile
 //                                                  );
    if(!xlsfile.isEmpty() || CheckLangFile(xlsfile))
    {

           mWindow->mGlobalSet->setValue(INI_PRJMLANG,xlsfile.toUtf8().mid(mWindow->mRootPathLen));
           ui->view_lang->setText(xlsfile);
           mWindow->readMultiLanguage(xlsfile);
           updateListWidget();
    }
}

void ConfigProject::on_lang_selectall_clicked()
{
    for(int i = 0;i < ui->langWidget->count();i++)
    {
        QListWidgetItem *item = ui->langWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
}

void ConfigProject::on_lang_dselectall_clicked()
{
    for(int i = 0;i < ui->langWidget->count();i++)
    {
        QListWidgetItem *item = ui->langWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}

void ConfigProject::on_lang_re_clicked()
{
    for(int i = 0;i < ui->langWidget->count();i++)
    {
        QListWidgetItem *item = ui->langWidget->item(i);
        item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
       // item->setSelected(!item->isSelected());
    }
}

QStringList ConfigProject::getSelectLang()
{
    QStringList lst;
    for(int i = 0;i < ui->langWidget->count();i++)
    {
        QListWidgetItem *item = ui->langWidget->item(i);
        if(item->checkState() == Qt::Checked)
            lst.append(item->text());
    }
    return lst;
}


GlobalSettings::GlobalSettings(QWidget *parent):
    QDialog(parent),
    ui(new Ui::GlobalSettings),
    isFine(true)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setModal(true);
    setObjectName(this->metaObject()->className());
//    this->UpdateStyle();
    mWindow = (MainWindow*)parent;


    FileEdit *mlangfile= new FileEdit("多国语言文件:");
    setMap["多国语言文件:"]  = mlangfile;
    mlangfile->setToolTip("工程控件要用的语言文,可选office2003版本的xls文件,或者utf8格式,分号(;)间隔的csv文件.");
    mlangfile->setFilter(tr("xls 文件 , CSV UTF-8 文件 (*.xls *.csv )"));
    mlangfile->setFileOrDir(false);
    mlangfile->filters << "*.xls" << "*.csv";
    IniMap[INI_PRJMLANG] = mlangfile;


    FileEdit *comfile = new FileEdit("控件文件:");
    setMap["控件文件:"] = comfile;
    comfile->setToolTip("原始的模版控件文件,json格式.");
    comfile->setFilter(tr("json 文件 (*.json)"));
    comfile->setFileOrDir(false);
    comfile->filters << "*.json";
    IniMap[INI_PRJJSON] = comfile;

    FileEdit *imagedir = new FileEdit("图片资源目录:");
    setMap["图片资源目录:"] = imagedir;
     imagedir->setToolTip("工程中要用到的图片资源目录,默认是 images");
    // projectdir->setFilter("");
     imagedir->setFileOrDir(true);
     IniMap[INI_PRJIMAGEDIR] = imagedir;
     imagedir->setFilePath(QDir::currentPath());

    FileEdit *projectdir = new FileEdit("工程目录:");
     setMap["工程目录:"] = projectdir;
     projectdir->setToolTip("工程保存的目录,默认是程序运行目录.");
     projectdir->setFileOrDir(true);
     IniMap[INI_PRJDIR] = projectdir;
     projectdir->setFilePath(QDir::currentPath());


     FileEdit *comdir = new FileEdit("自定义控件目录:");
     setMap["自定义控件目录:"] = comdir;
      comdir->setToolTip("自定义的模版控件目录,默认是widgets目录.");
    //  projectdir->setFilter("");
      comdir->setFileOrDir(true);
      IniMap[INI_PRJCUSTOM] = comdir;
      comdir->setFilePath(QDir::currentPath()+ BACKSLASH + "widgets") ;



      QSpinBox *width = new QSpinBox();
      width->setFixedWidth(60);
      width->setMaximum(1000);
      setMap[W]  = width;

      QSpinBox *height = new QSpinBox();
      height->setFixedWidth(60);
      height->setMaximum(1000);
      setMap[H] = height;

      QMapIterator<QString,QWidget*> iter(setMap);
      ui->treeWidget->setColumnCount(2);
      QTreeWidgetItem *headeritem = new QTreeWidgetItem();
      headeritem->setText(0,"全局配置项");
      headeritem->setText(1,"内容");
      ui->treeWidget->setHeaderItem(headeritem);
      ui->treeWidget->setAlternatingRowColors(true);
      QTreeWidgetItem *vsize =  new QTreeWidgetItem(QStringList() << "界面尺寸");
      ui->treeWidget->addTopLevelItem(vsize);

      QVariant sizeVar =  mWindow->mGlobalSet->value(INI_PRJSIZE);
       int w,h;
      if(sizeVar.isValid())
      {
          QString prjsize = sizeVar.toString();
          w = prjsize.section("*",0,0).toInt();
          h = prjsize.section("*",1,1).toInt();

      }else{
          w = 128;
          h = 128;
      }
      width->setValue(w); height->setValue(h);

      while(iter.hasNext())
      {
          iter.next();
          QTreeWidgetItem *t;
          if(!H.compare(iter.key()))
          {
              //ui->treeWidget_2->setItemWidget(t,1,iter.value());
              t = new QTreeWidgetItem(QStringList() << H);
              vsize->addChild(t);

          }else if(!W.compare(iter.key()))
          {
              t = new QTreeWidgetItem(QStringList() << W);
              vsize->addChild(t);
          }else{
              t = new QTreeWidgetItem(ui->treeWidget,QStringList() <<iter.key());
              t->setToolTip(0,iter.key());
              t->setToolTip(1,((FileEdit*)iter.value())->filePath());
          }
          ui->treeWidget->setItemWidget(t,1,iter.value());
      }

     QMapIterator<QString,QWidget*> it(IniMap);
     while(it.hasNext())
     {
         // 更新来自INI文件的值.
         it.next();
         QVariant va= mWindow->mGlobalSet->value(it.key());
         if(va.isValid())
         {
             QString pf = va.toString();
             if(pf.startsWith("."))
                 pf.remove(0,1).prepend(QDir::currentPath());
             else
                 pf.prepend(QDir::currentPath() +BACKSLASH );
             ((FileEdit*)it.value())->setFilePath(pf);
         }

     }


     ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
     ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");

    connect(this,SIGNAL(accepted()),SLOT(onAccepted()));
    setModal(true);
//    UpdateStyle();
}


QTreeWidgetItem* GlobalSettings::getItemByString(QString name)
{

    QList<QTreeWidgetItem*> qwilist =
            ui->treeWidget->findItems(name,Qt::MatchFixedString |
                                  Qt::MatchRecursive);
    QTreeWidgetItem *item = 0;
    if(qwilist.size())
    {
        return qwilist.first();
    }
    return item;
}

void GlobalSettings::onAccepted()
{
    int w,h;
     QWidget *hwidget = ui->treeWidget->itemWidget(getItemByString(H),1);
     QWidget *wwidget =  ui->treeWidget->itemWidget(getItemByString(W),1);
     h = ((QSpinBox*)hwidget)->value();
     w = ((QSpinBox*)wwidget)->value();
    if(w == 0 ||  h == 0)
    {
        QMessageBox::warning(this,"提示","宽高不能设置为零.");
        h = 64;
        w = 64;
    }

    mWindow->cManager->setDefaultPageSize(QSize(w,h));
    mWindow->mGlobalSet->setValue(INI_PRJSIZE,
                                QString("%1*%2").arg(QString::number(w),
                                                    QString::number(h)));
    QMapIterator<QString,QWidget*> it(IniMap);
    while(it.hasNext())
    {
        // 更新来自INI文件的值.
        it.next();
        QString f = ((FileEdit*)it.value())->filePath().toUtf8();
        if(f.startsWith(QDir::currentPath()))
            f = f.mid(mWindow->mRootPathLen);

        if(f.isEmpty()) f= ".";
        mWindow->mGlobalSet->setValue(it.key(),f);
        if(f.isEmpty())
            isFine = false;
    }
}

FileEdit::FileEdit(QString txt, QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    theLineEdit = new QLabel(this);
   // theLineEdit->setEnabled(false);
   // theLineEdit->setStyleSheet("background-color: red;");
    theLineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    QToolButton *button = new QToolButton(this);
    button->setObjectName(txt);
    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    button->setText(QLatin1String("..."));
    layout->addWidget(theLineEdit);
    layout->addWidget(button);
    setFocusProxy(theLineEdit);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
    connect(button,&QToolButton::clicked,[=](){
        QDialog *bd = new QDialog(this);
        bd->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        bd->setModal(true);
        bd->setWindowTitle(txt);
        QTreeView *fileTree = new QTreeView(this);
        fileTree->setAlternatingRowColors(true);
        QFileSystemModel *dirModel = new QFileSystemModel;
        //qDebug() << " path " << theLineEdit->text();
        dirModel->setRootPath(theLineEdit->text());
        dirModel->removeColumn(3);
        dirModel->removeColumn(2);
        dirModel->removeColumn(1);
        dirModel->setNameFilters(filters);

        fileTree->setModel(dirModel);
        fileTree->header()->setHidden(true);
        fileTree->hideColumn(3);
        fileTree->hideColumn(2);
        fileTree->hideColumn(1);

        fileTree->setRootIndex(dirModel->index(QDir::currentPath()));
        if(isDir){
            dirModel->setFilter(QDir::NoDotDot | QDir::AllDirs);
        }else{
            dirModel->setFilter(QDir::NoDotDot| QDir::Files | QDir::Dirs);
       }


        QVBoxLayout *vb = new QVBoxLayout();

        QPushButton *okbtn = new QPushButton("确认",this);
        vb->addWidget(fileTree);
        vb->addWidget(okbtn);
        connect(okbtn,&QPushButton::clicked,[=](){
            QString abpath = dirModel->fileInfo(fileTree->currentIndex()).absoluteFilePath();
            theLineEdit->setText(abpath);
            emit bd->accept();
        });

        // QTreeView 双击事件处理的例子.
        connect(fileTree,&QTreeView::doubleClicked,[=](QModelIndex index){
            theLineEdit->setText(dirModel->fileInfo(index).absoluteFilePath());
            emit bd->accept();
        });


        bd->setLayout(vb);
//        bd->UpdateStyle();
        bd->exec();
        bd->deleteLater();
        fileTree->deleteLater();
        dirModel->deleteLater();
    });

}





#include "mydialog.h"
#include "ui_dialog.h"
#include "ui_language.h"
#include <QMessageBox>
#include "mainwindow.h"
#include "canvasmanager.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QDrag>
#include <QMimeData>
#include <QDataStream>
#include <QBitmap>
#include <QRadioButton>



const static char DnditemData[] = "application/x-dnditemdata";







CustomListWidget::CustomListWidget(QWidget *parent)
    :QListWidget(parent)
{
    setAcceptDrops(true);
}

void CustomListWidget::dragLeaveEvent(QDragLeaveEvent *e)
{

}
void CustomListWidget::dropEvent(QDropEvent *event)
{

    CustomListWidget *source =(CustomListWidget *)(event->source());
      if (source && source != this) {
          addItem(event->mimeData()->text());
          event->setDropAction(Qt::MoveAction);
          event->accept();
      }
}

void CustomListWidget::checkMimeData(QDragMoveEvent *event)
{

    CustomListWidget *source =(CustomListWidget *)(event->source());
       if (source && source != this) {
           event->setDropAction(Qt::MoveAction);
           event->accept();
       }

    qDebug() << " event source   " << event->source()
             << " event " << event;
//    if(event->mimeData()->hasFormat(DnditemData))
//    {
//        if(event->source() == this)
//        {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        }else {
//            event->acceptProposedAction();
//        }
//    }else{
//        event->ignore();
//    }
}

void CustomListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    checkMimeData(event);
//    if(event->mimeData()->hasFormat(DnditemData))
//    {
//        if(event->source() == this)
//        {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        }else {
//            event->acceptProposedAction();
//        }
//    }else{
//        event->ignore();
//    }
}

void CustomListWidget::dragEnterEvent(QDragEnterEvent *event)
{
//    if(event->mimeData()->hasFormat(DnditemData))
//    {
//        if(event->source() == this)
//        {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        }else{
//            event->acceptProposedAction();
//        }
//    }else{
//        event->ignore();
//    }
    checkMimeData(event);

}

void CustomListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QListWidget::mouseMoveEvent(event);
}

void CustomListWidget::performDrag()
{
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        QString fname = item->text();
        mimeData->setText(fname);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        QVariantMap extMap = property(DKEY_EXTMAP).toMap();
        drag->setPixmap(QPixmap(extMap[fname].toString()));
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
            delete item;
    }
}

void CustomListWidget::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton)
           startPos = event->pos();

     QListWidget::mousePressEvent(event);
//    QListWidgetItem *item  = this->itemAt(event->pos());
//    if(!item)
//        return;


//    QByteArray itemData;
//    QDataStream dataStream(&itemData,QIODevice::WriteOnly);

//    QString fname = item->text();
//    dataStream << fname << event->pos() ;

//    QMimeData *mimeData = new QMimeData;

//    mimeData->setData(DnditemData,itemData);

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//    drag->setPixmap(QPixmap(fname));
//    drag->setHotSpot(event->pos());


}


ImageFileDialog::ImageFileDialog(QVariantList old, QWidget *parent)
    :QDialog(parent),
//      sellist(new CustomListWidget()),
//      flistview(new CustomListWidget()),
      sellist(new QListWidget()),
      flistview(new QListWidget()),
      treefile(new QTreeView()),
      selstrList(old),
      statusBar(new QLabel)
{


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
    navigator[UP] = ":/icon/icons/arrowup@2x.png";
    navigator[DOWN] = ":/icon/icons/arrowdown@2x.png";
    navigator[LEFT] = ":/icon/icons/arrowleft@2x.png";
    navigator[RIGHT] = ":/icon/icons/arrowright@2x.png";


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
        btn->setIcon(QIcon(itmap.value()));
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


    QString imgpath = QDir::currentPath() + "/config/images";
    QFileInfo qf(imgpath);
    qDebug() << " images path " << imgpath;
    dirModel->setRootPath(imgpath);
    dirModel->removeColumn(3);
    dirModel->removeColumn(2);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);


    filters << "*.bmp" << "*.png" << "*.jpg";
    dirModel->setNameFilters(filters);


    treefile->setModel(dirModel);
    treefile->setRootIndex(dirModel->index(imgpath));

    treefile->hideColumn(3);
    treefile->hideColumn(2);
    treefile->hideColumn(1);


    fileModel->setRootPath(imgpath);
    fileModel->setNameFilters(filters);
    fileModel->setFilter(QDir::Files);
//    flistview->setModel(fileModel);
//    flistview->setRootIndex(fileModel->index(imgpath));

    connect(sellist,SIGNAL(doubleClicked(QModelIndex)),SLOT(onSelListViewDoubleClicked(QModelIndex)));
    connect(sellist,SIGNAL(clicked(QModelIndex)),SLOT(onSelectCurrentItem(QModelIndex)));
    connect(flistview,SIGNAL(doubleClicked(QModelIndex)),SLOT(onListViewDoubleClicked(QModelIndex)));

    connect(treefile,SIGNAL(clicked(QModelIndex)),SLOT(onTreeViewClicked(QModelIndex)));

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
    statusBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    mainLayout->addLayout(mlayout);
    setOldList();
    this->setModal(true);
   // exec();
}

void ImageFileDialog::setOldList()
{
    foreach (QVariant v, selstrList) {
        QString str = v.toString();
        QListWidgetItem *nitem = new QListWidgetItem(QPixmap(str.section(":",1,1)),
                                                     str.section(":",0,0));

        sellist->addItem(nitem);
    }
}


void ImageFileDialog::updateListImages(QString path)
{
  //  imgMap.clear();
    flistview->clear();
    extMap.clear();
    QDirIterator it(path,filters, QDir::Files /*,QDirIterator::Subdirectories*/);

    while (it.hasNext())
    {
        QString fpath = it.next();
        bool b = fpath.contains('/');
        int idx = fpath.lastIndexOf(b ? '/' : '\\')+1;
        //bakimageMap[fpath.mid(idx)] = QPixmap(fpath);
      //  imgMap[fpath.mid(idx)] = fpath;
        extMap[fpath.mid(idx)] = fpath;
        bool isFind = false;
        for(int i =0; i < sellist->count();i++)
        {
           QListWidgetItem *item =    sellist->item(i);
           if(!item->text().compare(fpath.mid(idx)))
           {
               // 这里有相同文件名了,所以在待选列表不再添加它,这里没有处理不同目录下的同名文件.都视为同名.
               isFind = true;
               break;
           }
        }
        if(isFind)
            continue;

        flistview->addItem(new QListWidgetItem(QPixmap(fpath),fpath.mid(idx)));
    }
  //  flistview->setProperty(DKEY_IMGMAP,imgMap);
}

void ImageFileDialog::onSelectCurrentItem(QModelIndex index)
{

  findChild<QPushButton*>(UP)->setEnabled(index.row() == 0 ? false : true);
  findChild<QPushButton*>(DOWN)->setEnabled(index.row() == sellist->count() -1 ? false : true);

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
    QListWidgetItem *nitem = new QListWidgetItem(QPixmap(extMap[s].toString()),s);
    sellist->addItem(nitem);
    QListWidgetItem *item = flistview->takeItem(index.row());
    delete item;
    hRows[s] = index;
   // selMap[s] = fileModel->fileInfo(index).absoluteFilePath();
    /* 这里不能使用MAP , QComobox 需要排序 */
   // selstrList.append(QString("%1:%2").arg(s,fileModel->fileInfo(index).absoluteFilePath()));
   // 这里的每一条数据必需是下面格式:　　文件名:文件名的绝对完全路径
    selstrList.append(QString("%1:%2").arg(s,extMap[s].toString()));
    statusBar->setText(QString::number(sellist->count()));
}

void ImageFileDialog::onUp()
{
    QPushButton *btn = (QPushButton*)(QObject::sender());
//    int row = btn->property(DKEY_ROW).toInt();
    int row = sellist->currentRow();
//    if(row == 0)
//    {
//        btn->setEnabled(false);
//        return;
//    }
     QListWidgetItem *item =  sellist->takeItem(row);
     sellist->insertItem(row -1,item);
     sellist->setCurrentItem(item);
     btn->setEnabled(row -1 == 0 ? false : true);
}

void ImageFileDialog::onDown()
{
    QPushButton *btn = (QPushButton*)(QObject::sender());
    int row = sellist->currentRow();
//    if(row == sellist->count() -1)
//    {
//        btn->setEnabled(false);
//        return;
//    }
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

  //  QModelIndex qmi = fileModel->index(selstr);
    QString fpath = fileModel->fileInfo(fileModel->index(selstr)).absoluteFilePath();
    qDebug() << " will show the " << fpath << " selected str: " << selstr;

    //sellist->setRowHidden(index.row(),true);
    delete sellist->takeItem(index.row());

    flistview->setRowHidden(hRows[selstr].row(),false);
   // selMap.remove(selstr);

    /* 从列表删除 */
   int i = selstrList.indexOf( QString("%1:%2").arg(selstr,fpath));
   if( -1 != i )
       selstrList.removeAt(i);
   statusBar->setText(QString::number(sellist->count()));
//   statusBar->repaint();
}

void ImageFileDialog::onDelSelectedItems()
{
    QList<QListWidgetItem*> items = sellist->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        QString selstr = item->text();

        flistview->setRowHidden(hRows[selstr].row(),false);

        delete sellist->takeItem(sellist->row(item));
       // selMap.remove(item->text());
        QString fpath = fileModel->fileInfo(hRows[selstr]).absoluteFilePath();
        int i = selstrList.indexOf( QString("%1:%2").arg(selstr,fpath));
        if( -1 != i )
            selstrList.removeAt(i);
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
    qDebug() << " DirModel AbsoluteFile Path " << mPath;
   // flistview->setRootIndex(fileModel->setRootPath(mPath));
    updateListImages(mPath);

    // dirModel->fetchMore(index);
    treefile->setExpanded(index,true);
    treefile->expand(index);
    treefile->setCurrentIndex(dirModel->index(mPath));
}



ProjectDialog::ProjectDialog(QWidget *parent):QDialog(parent),ui(new Ui::ProjectDialog)
{
    ui->setupUi(this);
    mWindow = (MainWindow*)(parent);
    setWindowTitle("新建工程");
    setModal(true);
    connect(this,SIGNAL(accepted()),SLOT(onAccepted()));
    connect(this,SIGNAL(rejected()),SLOT(onRejected()));
    connect(this,SIGNAL(finished(int)),SLOT(onFinished(int)));
    QVariant sizeVar =  mWindow->globalSet->value(INI_PRJSIZE);
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
           QMessageBox::warning(this,"提示","宽高没能设置为零.");
       }


       mWindow->cManager->setDefaultPageSize(QSize(ui->spinBox->value(),ui->spinBox_2->value()));
       mWindow->globalSet->setValue(INI_PRJSIZE,
                                   QString("%1*%2").arg(QString::number(ui->spinBox->value()),
                                                       QString::number(ui->spinBox_2->value())));
       mWindow->setWindowTitle(ui->lineEdit->text());
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

    QHBoxLayout *mh = new QHBoxLayout();
    this->setLayout(mh);
    this->setWindowTitle(tr("图片编辑"));
    filters << "*.bmp" << "*.png" << "*.jpg";


    treeModel->setRootPath(path);
    treeModel->removeColumn(3);
    treeModel->removeColumn(2);
    treeModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);


    treeModel->setNameFilters(filters);


    treefile->setModel(treeModel);
    treefile->setRootIndex(treeModel->index(path));

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

    mh->addWidget(treefile);
    mh->addWidget(imglist);

    updateListImages(path);
    treefile->setFixedWidth(160);
    connect(treefile,SIGNAL(clicked(QModelIndex)),SLOT(onTreeViewClicked(QModelIndex)));
    connect(imglist,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            parent,SLOT(onSelectedBackgroundImage(QListWidgetItem*)));

}

void ImageListView::updateListImages(QString path)
{
    imgMap.clear();
    imglist->clear();

    QDirIterator it(path,filters, QDir::Files, QDirIterator::Subdirectories);

//    QQuickView qview;

//    qview.setResizeMode(QQuickView::SizeRootObjectToView);
//    qview.setSource(QUrl("qrc:/icon/busyindicator.qml"));
//    qview.show();




    while (it.hasNext())
    {
        QString fpath = it.next();
        bool b = fpath.contains('/');
        int idx = fpath.lastIndexOf(b ? '/' : '\\')+1;
        //bakimageMap[fpath.mid(idx)] = QPixmap(fpath);
        imgMap[fpath.mid(idx)] = fpath;
        imglist->addItem(new QListWidgetItem(QIcon(QPixmap(fpath)),fpath.mid(idx)));
    }
    imglist->setProperty(DKEY_IMGMAP,imgMap);
}

void ImageListView::onTreeViewClicked(QModelIndex index)
{
    QString mPath = treeModel->fileInfo(index).absoluteFilePath();
    qDebug() << " DirModel AbsoluteFile Path " << mPath;
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


    foreach (QString key , mWindow->orderlist) {
//        QListWidgetItem* item = new QListWidgetItem(QString("    %1").arg(key),listWidget);
//        QRadioButton *rb = new QRadioButton(QString("            %1").arg(map[key]));
        QListWidgetItem* item = new QListWidgetItem(listWidget);
        QRadioButton *rb = new QRadioButton(mWindow->itemMap[key]);
        connect(rb,&QRadioButton::toggled,[=](bool f){
           if(f)
               text = rb->text();
        });

        listWidget->setItemWidget(item,rb);
        rb->setChecked(!key.trimmed().compare(old));
    }

//    foreach (QString s, map) {
//        QListWidgetItem* item = new QListWidgetItem(listWidget);
//        QRadioButton *rb = new QRadioButton(s);
//        listWidget->setItemWidget(item,rb);
//        rb->setChecked(!s.compare(old));
//    }

    connect(listWidget,&QListWidget::itemPressed,this,
            [=](QListWidgetItem *item)
            {
             listWidget->clearSelection();
             text = mWindow->orderlist[listWidget->row(item)]; });

    QVBoxLayout *layout= new QVBoxLayout();
    layout->addWidget(listWidget);
    setLayout(layout);
    setFixedSize(180,360);
    setModal(true);
}


//QString MenuItemDialog::getSelectText()
//{
//    for(int i =0 ; i < listWidget->count();i++)
//    {
//        QListWidgetItem *item = listWidget->item(i);
//        QRadioButton *rb = (QRadioButton *)(listWidget->itemWidget(item));
//        if(rb->isChecked())
//        {
//           // return rb->text();
//           return item->text().trimmed();
//        }
//    }
//}

I18nLanguage::I18nLanguage( QWidget *parent):
    QDialog(parent),
    ui(new Ui::I18nLanguage)
{

    ui->setupUi(this);
    mWindow = (MainWindow*)(parent);

    ui->langWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
    ui->itemWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
    foreach (QString s, mWindow->LanguageList) {

        QListWidgetItem* item = new QListWidgetItem(s, ui->langWidget);
       // item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable); // set checkable flag

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable |
                       Qt::ItemIsEditable| Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(Qt::Unchecked); // AND initialize check state

    }
   // ui->langWidget->addItems(mWindow->LanguageList);

    QMapIterator<QString,QString> iter(mWindow->itemMap);
    while(iter.hasNext())
    {
        iter.next();
        QListWidgetItem* item = new QListWidgetItem(ui->itemWidget);
        ui->itemWidget->setItemWidget(item,new QRadioButton(iter.value()));
       // item->setData(Qt::CheckStateRole,Qt::Unchecked);
      //  item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
     //   item->setCheckState(Qt::Unchecked); // AND initialize check state
    }

    setModal(true);
}

QStringList I18nLanguage::getSelectedItems(bool isLang)
{
    QStringList lst;
    if(isLang)
    {
        for(int i = 0;i < ui->langWidget->count();i++)
        {
            QListWidgetItem *item = ui->langWidget->item(i);
            if(item->checkState() == Qt::Checked)
            lst.append(item->text());
        }
    }else{
        for(int i = 0;i < ui->itemWidget->count();i++)
        {
            QListWidgetItem *item = ui->itemWidget->item(i);
            if(item->checkState() == Qt::Checked)
            lst.append(item->text());
        }
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

void I18nLanguage::on_lang_selectall_clicked()
{
    for(int i = 0;i < ui->langWidget->count();i++)
    {
        QListWidgetItem *item = ui->langWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
}

void I18nLanguage::on_lang_dselectall_clicked()
{
    for(int i = 0;i < ui->langWidget->count();i++)
    {
        QListWidgetItem *item = ui->langWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}

void I18nLanguage::on_lang_re_clicked()
{
    for(int i = 0;i < ui->langWidget->count();i++)
    {
        QListWidgetItem *item = ui->langWidget->item(i);
        item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
       // item->setSelected(!item->isSelected());
    }
}

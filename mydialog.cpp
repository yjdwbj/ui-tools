#include "mydialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include "mainwindow.h"
#include "canvasmanager.h"

ImageFileDialog::ImageFileDialog(QStringList old, QWidget *parent)
    :QDialog(parent),
      sellist(new QListWidget()),
      treefile(new QTreeView()),
      flistview(new QListView()),
      selstrList(old)
{

     /* 填弃上一次的数据 */
    dirModel = new QFileSystemModel(this);
    fileModel = new QFileSystemModel(this);
    this->setFixedSize(1000,600);
    sellist->setSelectionMode(QAbstractItemView::MultiSelection);
    flistview->setSelectionMode(QAbstractItemView::MultiSelection);
    //  dirModel->setReadOnly(true);
    //  fileModel->setReadOnly(true);


    QHBoxLayout *mh = new QHBoxLayout();
    this->setLayout(mh);
    this->setWindowTitle(tr("图片编辑"));


    add = new QPushButton(tr("<--"));
    del =new QPushButton(tr("-->"));
    QVBoxLayout *v = new QVBoxLayout();
    v->addWidget(add);
    v->addWidget(del);

    connect(del,SIGNAL(clicked(bool)),SLOT(onDelSelectedItems()));
    connect(add,SIGNAL(clicked(bool)),SLOT(onAddSelectedItems()));


    QString imgpath = QDir::currentPath() + "/images";
    QFileInfo qf(imgpath);
    qDebug() << " images path " << imgpath;
    dirModel->setRootPath(imgpath);
    dirModel->removeColumn(3);
    dirModel->removeColumn(2);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    QStringList mlist;
    mlist << "*.bmp" << "*.png" << "*.jpg";
    dirModel->setNameFilters(mlist);


    treefile->setModel(dirModel);
    treefile->setRootIndex(dirModel->index(imgpath));

    treefile->hideColumn(3);
    treefile->hideColumn(2);
    treefile->hideColumn(1);


    fileModel->setRootPath(imgpath);
    fileModel->setNameFilters(mlist);
    flistview->setModel(fileModel);
    flistview->setRootIndex(fileModel->index(imgpath));

    connect(sellist,SIGNAL(doubleClicked(QModelIndex)),SLOT(onSelListViewDoubleClicked(QModelIndex)));
    connect(flistview,SIGNAL(doubleClicked(QModelIndex)),SLOT(onListViewDoubleClicked(QModelIndex)));
    connect(treefile,SIGNAL(clicked(QModelIndex)),SLOT(onTreeViewClicked(QModelIndex)));

    /* 主布局是水平布局,左(QListWidget),中(垂直布局),右(文件系统) */
    // sellist->setModel(strListMode);
    mh->addWidget(sellist);
    mh->addLayout(v);
    mh->addWidget(flistview);
    mh->addWidget(treefile);

    this->setModal(true);
    exec();
}

void ImageFileDialog::onListViewDoubleClicked(QModelIndex index)
{
    /* 双击添加到右框*/
    //  QString sstr = fileModel->data(index).toString();
    //qDebug() << " here is dir or file " << sstr;
    // fileModel->fileInfo(index).absolutePath();
    QFileInfo qf = dirModel->fileInfo(index);
    if(qf.isDir())
    {
        flistview->setRootIndex(fileModel->setRootPath(qf.absoluteFilePath()));

        // treefile->setExpanded(index,true);
        // treefile->expand(index);
        QModelIndex mi = dirModel->index(qf.absoluteFilePath());
        treefile->setCurrentIndex(mi);
        treefile->expand(mi);
        return;
    }

    QString s = fileModel->data(index).toString();
    sellist->addItem(s);
    //selMap[s] = fileModel->fileInfo(index).absoluteFilePath();
    // 这里的每一条数据必需是下面格式:　　文件名:文件名的绝对完全路径
    selstrList << QString("%1:%2").arg(s,fileModel->fileInfo(index).absoluteFilePath());
    flistview->setRowHidden(index.row(),true);
    hRows[s] = index;


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
    sellist->clearSelection();
}


void ImageFileDialog::onAddSelectedItems()
{
    QModelIndexList qil = flistview->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, qil)
    {

        QString s = fileModel->data(index).toString();
        if(dirModel->fileInfo(index).isDir())
            continue;
        if(fileModel->fileInfo(index).isDir())
            continue;
        sellist->addItem(s);
        flistview->setRowHidden(index.row(),true);
        hRows[s] = index;
       // selMap[s] = fileModel->fileInfo(index).absoluteFilePath();
        /* 这里不能使用MAP , QComobox 需要排序 */
        selstrList.append(QString("%1:%2").arg(s,fileModel->fileInfo(index).absoluteFilePath()));

    }
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
    flistview->setRootIndex(fileModel->setRootPath(mPath));

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
    QSize pdsize(430,360);
    ui->spinBox->setMinimum(64);
    ui->spinBox_2->setMinimum(64);
    ui->spinBox->setMaximum(parent->width()*0.8);
    ui->spinBox_2->setMaximum(parent->height()*0.8);
    QSize ds = mWindow->cManager->getDefaultPageSize();

    if(ds.width() > pdsize.width() && ds.height() > pdsize.height())
    {
        // 记住上次的输入
        ui->spinBox->setValue(ds.width());
        ui->spinBox_2->setValue(ds.height());
    }else{
        ui->spinBox->setValue(pdsize.width());
        ui->spinBox_2->setValue(pdsize.height());
    }



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

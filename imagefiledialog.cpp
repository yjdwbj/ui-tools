#include "imagefiledialog.h"

ImageFileDialog::ImageFileDialog(QWidget *parent)
    :QDialog(parent),
      sellist(new QListWidget()),
      treefile(new QTreeView()),
      flistview(new QListView())
{

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
    selstrList << QString("%1:%2").arg(s,fileModel->fileInfo(index).absoluteFilePath());
    flistview->setRowHidden(index.row(),true);
    hRows[s] = index;


}

void ImageFileDialog::onSelListViewDoubleClicked(QModelIndex index)
{
    /* 双击从右框删除 */
    // sellist->takeItem(index.row())->text();
    QString selstr = sellist->takeItem(index.row())->text();
    QModelIndex qmi = fileModel->index(selstr);
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


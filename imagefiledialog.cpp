#include "imagefiledialog.h"

ImageFileDialog::ImageFileDialog(QWidget *parent)
    :QDialog(parent),
      sellist(new QListWidget()),
      treefile(new QTreeView()),
      flistview(new QListView())
{
    this->setFixedSize(1000,600);


    QHBoxLayout *mh = new QHBoxLayout();
    this->setLayout(mh);
    this->setWindowTitle(tr("图片编辑"));




    add = new QPushButton(tr("<--"));
    del =new QPushButton(tr("-->"));
    QVBoxLayout *v = new QVBoxLayout();
    v->addWidget(add);
    v->addWidget(del);

    dirModel = new QFileSystemModel(this);
    QString imgpath = QDir::currentPath() + "/images";
    qDebug() << " images path " << imgpath;
    dirModel->setRootPath(imgpath);

    dirModel->removeColumn(3);
    dirModel->removeColumn(2);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    // model->setNameFilters();
    QStringList mlist;
    mlist << "*.bmp" << "*.png" << "*.jpg";
    dirModel->setNameFilters(mlist);


    qDebug() << "model root " << dirModel->rootPath();
    treefile->setModel(dirModel);
    treefile->hideColumn(3);
    treefile->hideColumn(2);
    treefile->hideColumn(1);





    fileModel  = new QFileSystemModel(this);
   // QString imgpath = QDir::currentPath() + "/images";
    qDebug() << " images path " << imgpath;
    fileModel->setRootPath(imgpath);
  //  fileModel->removeColumn(3);
    fileModel->setNameFilters(mlist);




    flistview->setModel(fileModel);

    connect(treefile,SIGNAL(clicked(QModelIndex)),SLOT(onTreeViewClicked(QModelIndex)));

    /* 主布局是水平布局,左(QListWidget),中(垂直布局),右(文件系统) */
    mh->addWidget(sellist);
    mh->addLayout(v);
    mh->addWidget(flistview);
    mh->addWidget(treefile);

   this->setModal(true);
}

ImageFileDialog::~ImageFileDialog()
{
    this->deleteLater();
}

void ImageFileDialog::onTreeViewClicked(QModelIndex index)
{
    QString mPath = dirModel->fileInfo(index).absoluteFilePath();
    flistview->setRootIndex(fileModel->setRootPath(mPath));
}


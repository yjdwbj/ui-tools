#include "pageview.h"
#include "mainwindow.h"
#include "canvasmanager.h"

#include <QVBoxLayout>
#include <QDebug>

#include <QGraphicsGridLayout>
#include <QGraphicsLayoutItem>



PageView::PageView(MainWindow *mw)
    :mWindow(mw),mImgList(new QListWidget)
{
    QWidget *main = new QWidget(this);
    QVBoxLayout *vb = new QVBoxLayout(main);
    main->setLayout(vb);
    setWidget(main);
    main->setStyleSheet("QListWidget{background-color: #C0DCC0;}");

   mImgList->setViewMode(QListWidget::IconMode);
   mImgList->setIconSize(QSize(200,180));
   mImgList->setDragEnabled(false);
   mImgList->setSelectionMode(QAbstractItemView::SingleSelection);
  // mImgList->resizeMode(QListWidget::Adjust);
//   mImgList->addItem(new QListWidgetItem(QIcon("test.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test1.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test2.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test3.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test4.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test5.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test6.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test7.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test.png"),""));
//   mImgList->addItem(new QListWidgetItem(QIcon("test.png"),""));


    vb->addWidget(mImgList);
    setMinimumHeight(mWindow->size().height()-60);
    setMaximumHeight(mWindow->size().height());
   // qDebug() << "  items size " <<  mScene->items().size();
   // connect(mImgList,SIGNAL(clicked(QModelIndex)),SLOT(onClickedItem(QModelIndex)));

    connect(mImgList,SIGNAL(itemPressed(QListWidgetItem*)),SLOT(onClickedItem(QListWidgetItem*)));
  //  connect(mImgList,SIGNAL(itemChanged(QListWidgetItem*)),SLOT(onItemChanged(QListWidgetItem*)));
 //   connect(mImgList,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(onClickedItem(QListWidgetItem*)));
   // connect(mImgList,SIGNAL(itemClicked(QListWidgetItem*))
    show();
}

void PageView::addNewPage(QPixmap &p)
{
    QListWidgetItem * nit = new QListWidgetItem(QIcon(p),"");

    mImgList->addItem(nit);
   // qDebug() << " Page count " << mImgList->count();

}

void PageView::addNewPage(QPixmap &p, QString txt)
{
    QListWidgetItem * nit = new QListWidgetItem(QIcon(p),txt);

    mImgList->addItem(nit);
}

void PageView::delPage(int index)
{

    mImgList->removeItemWidget(mImgList->takeItem(index));
}

void PageView::InsertPage(int index, QPixmap &p)
{
    mImgList->insertItem(index,new QListWidgetItem(QIcon(p),""));
}

void PageView::InsertPage(int index, QPixmap &p, QString txt)
{
    mImgList->insertItem(index,new QListWidgetItem(QIcon(p),txt));
}

void PageView::onClickedItem(QListWidgetItem *item)
{
    //qDebug() << " clicked index is " << item->text() ;
    int row = mImgList->row(item);
    mWindow->cManager->setActiveSS(row);
}


void PageView::onItemChanged(QListWidgetItem *it)
{
    qDebug() << " now change to index " << it->text() << " row is " << mImgList->row(it);
}

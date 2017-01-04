#include "pageview.h"
#include "mainwindow.h"
#include "canvasmanager.h"

#include <QVBoxLayout>
#include <QDebug>
PageView::PageView(MainWindow *mw)
    :mWindow(mw),mImgList(new QListWidget)
{
   mImgList->setViewMode(QListWidget::IconMode);
   mImgList->setIconSize(QSize(200,180));
   mImgList->setDragEnabled(false);
   mImgList->setDragDropMode(QAbstractItemView::NoDragDrop);
   mImgList->setSelectionMode(QAbstractItemView::SingleSelection);
   mImgList->setEditTriggers(QAbstractItemView::NoEditTriggers);


   // vb->addWidget(mImgList);
    setStyleSheet("QListWidget{background-color: #C0DCC0;}");
    setWidget(mImgList);


    connect(mImgList,SIGNAL(itemPressed(QListWidgetItem*)),SLOT(onClickedItem(QListWidgetItem*)));
    //connect(mImgList,SIGNAL(itemDoubleClicked(QListWidgetItem*)),SLOT(onClickedItem(QListWidgetItem*)));
    show();
}

void PageView::addNewPage(QPixmap &p)
{
    QListWidgetItem * twi = new QListWidgetItem(QIcon(p),"");

    mImgList->addItem(twi);
   // qDebug() << " Page count " << mImgList->count();

}

void PageView::addNewPage(QPixmap &p, QString txt)
{
    QListWidgetItem * nit = new QListWidgetItem(QIcon(p),txt);

    mImgList->addItem(nit);
    mImgList->setCurrentItem(nit);
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
    QListWidgetItem *twi = new QListWidgetItem(QIcon(p),txt);
    mImgList->insertItem(index,twi);
    mImgList->clearSelection();
    mImgList->setCurrentItem(twi,QItemSelectionModel::ClearAndSelect);
//    mImgList->setItemSelected(twi,true);
//    onClickedItem(twi);

}

void PageView::onClickedItem(QListWidgetItem *item)
{
    //qDebug() << " clicked index is " << item->text() ;
    int row = mImgList->row(item);
    mWindow->cManager->setActiveSS(row);
    mImgList->clearSelection();
   // mImgList->setCurrentItem(item);
    mImgList->setCurrentItem(item,QItemSelectionModel::ClearAndSelect);
    mImgList->setItemSelected(item,true);

}

void PageView::ReloadListView()
{
   for(int i = 0 ; i < mImgList->count();i++)
   {
       emit onClickedItem(mImgList->item(i));
   }
}

void PageView::PressItem(int index)
{
   QListWidgetItem *item =  mImgList->item(index);
   emit mImgList->itemPressed(item);
}

void PageView::onItemChanged(QListWidgetItem *it)
{
    qDebug() << " now change to index " << it->text() << " row is " << mImgList->row(it);
}

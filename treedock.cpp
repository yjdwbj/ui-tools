#include "treedock.h"

#include "mainwindow.h"
#include <QMetaObject>
#include <QMetaProperty>
#include "formresizer.h"
#include "scenesscreen.h"
#include "canvasmanager.h"
#include "module.h"


static QString HeadCol = "结点,属性";

TreeDock::TreeDock(QWidget *parent)
    :QDockWidget(parent),
      mWindow((MainWindow*)parent), treeWidget(new QTreeWidget())
{

    /*
    QWidgetList tlist = qApp->topLevelWidgets();
    for(QWidgetList::iterator wit = tlist.begin();wit != tlist.end();++wit)
    {
        if((*wit)->objectName() == "MainWindow")
        {
            mWindow = (MainWindow*)(*wit);
            break;
        }
    }*/


    QString style = "QTreeView {\
            show-decoration-selected: 1;\
}\
\
QTreeView::item {\
border: 1px solid #d9d9d9;\
    border-top-color: transparent;\
    border-bottom-color: transparent;\
}\
\
QTreeView::item:hover {\
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);\
border: 1px solid #bfcde4;\
}\
\
QTreeView::item:selected {\
                    border: 1px solid #567dbc;\
}\
\
QTreeView::item:selected:active{\
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);\
}\
\
QTreeView::item:selected:!active {\
                    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);\
}\
QTreeView {\
    alternate-background-color: yellow;\
}\
QTreeView::branch {\
background: palette(base);\
}\
\
QTreeView::branch:has-siblings:!adjoins-item {\
                      background: cyan;\
}\
\
QTreeView::branch:has-siblings:adjoins-item {\
                      background: red;\
}\
\
QTreeView::branch:!has-children:!has-siblings:adjoins-item {\
                       background: blue;\
}\
\
QTreeView::branch:closed:has-children:has-siblings {\
                      background: pink;\
}\
\
QTreeView::branch:has-children:!has-siblings:closed {\
                      background: gray;\
}\
\
QTreeView::branch:open:has-children:has-siblings {\
                      background: magenta;\
}\
\
QTreeView::branch:open:has-children:!has-siblings {\
                      background: green;\
}\
";

//QTreeWidget *tree = new QTreeWidget();
treeWidget->setColumnCount(2);

treeWidget->setHeaderLabels(HeadCol.split(","));
setStyleSheet("background-color: #C0DCC0;");
connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),SLOT(onItemPressed(QTreeWidgetItem*,int)));
//root = new QTreeWidgetItem(treeWidget);
//root->setText(0,"根结点");
//root->setText(1,"画布");

this->setWidget(treeWidget);

//this->setFeatures(DockWidgetMovable|DockWidgetFloatable);
//tree->setStyleSheet(style);

setFixedWidth(200);
//treeWidget->setFixedHeight(mWindow->size().height()-80);
treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
//treeWidget->header()->setStretchLastSection(false);
treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//setFeatures(QDockWidget::DockWidgetVerticalTitleBar);

connect(treeWidget,SIGNAL(customContextMenuRequested(QPoint)),SLOT(onCustomContextMenu(QPoint)));


}

//void TreeDock::addCompoentControls(CompoentControls *cc)
//{
//    comC = cc;
//}

//void TreeDock::addPropBox(PropertyBox *p)
//{
//    pb = p;
//}

void TreeDock::setSelectTreeItem(QWidget *obj)
{
    QString key  = obj->property(DKEY_LOCALSEQ).toString();
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(key,Qt::MatchFixedString | Qt::MatchRecursive);
    foreach (QTreeWidgetItem *qwi, qwilist) {
        //qDebug() << " this text " << qwi->text(0);
       //if(!qwi->text(0).compare(obj->objectName()))
       if(!qwi->text(0).compare(key)) // caption 加序号.
       {
           treeWidget->setCurrentItem(qwi);
           break;
       }
    }
      //  treeWidget->setCurrentItem(it);
}


void TreeDock::swapIconForItem(QString txt)
{

    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(txt,
                                                            Qt::MatchFixedString | Qt::MatchRecursive);

    if(qwilist.count())
    {
        QTreeWidgetItem *item = qwilist.at(0);
        QWidget *w = mWindow->ComCtrl->ProMap[txt];
        item->setIcon(0,w->isHidden() ? QIcon(HIDE_ICON) : QIcon(SHOW_ICON));
    }

}

void TreeDock::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = treeWidget->indexAt(point);

    QTreeWidgetItem *item = treeWidget->itemAt(point);
    if(item)
    {
        qDebug() << " item is " << item->text(0) << " pos " << point;
       // contextMenu->exec(treeWidget->mapToGlobal(point));

//       if(!item->parent()) // 顶级
       if(!item->text(1).compare(CN_NEWLAYOUT) ||
               !item->text(1).compare(CN_NEWLAYER))
       {

            QWidget *w = mWindow->ComCtrl->ProMap[item->text(0)];
            QMenu *menu = new QMenu(treeWidget);
            QAction hideit("隐藏",treeWidget);
          //  hideit.setMenu(menu);
            QAction viewit("显示",treeWidget);
         //   viewit.setMenu(menu);
            if(w->isHidden())
            {
                viewit.setIcon(QIcon(SHOW_ICON));
                connect(&viewit,SIGNAL(triggered(bool)),SLOT(onSwapShowHideObject(bool)));
                menu->addAction(&viewit);
            }else
            {
                hideit.setIcon(QIcon(HIDE_ICON));
                menu->addAction(&hideit);
                connect(&hideit,SIGNAL(triggered(bool)),SLOT(onSwapShowHideObject(bool)));
            }
            menu->exec(treeWidget->viewport()->mapToGlobal(point));
            delete menu;
       }
    }
}


void TreeDock::onSwapShowHideObject(bool)
{
    QTreeWidgetItem *item = treeWidget->currentItem();
    if(item)
    {
        QString text = item->text(0);
        QWidget *ww = mWindow->ComCtrl->ProMap[text];
        //qDebug() << " item " << text << " is hide " << ww->isHidden();
        item->setExpanded(ww->isHidden());
        for(int i = 0; i < item->childCount();i++)
        {
            item->child(i)->setHidden(!ww->isHidden());
        }
        ww->setHidden(!ww->isHidden());
       // ww->update();
        swapIconForItem(text);
    }
}

void TreeDock::setMyParentNode()
{
    QTreeWidgetItem *item= treeWidget->currentItem()->parent();
    if(item)
    {
        QWidget *w = mWindow->ComCtrl->ProMap[item->text(0)];
        mWindow->cManager->activeSS()->setSelectObject((FormResizer*)w);

    }
}

void TreeDock::onItemPressed(QTreeWidgetItem *item,int col)
{

   // if (root  == item)
    //    return;

    qDebug() << " clicked tree : " << item->text(0);
    if(mWindow->ComCtrl->ProMap.contains(item->text(0)))
    {
        ((FormResizer*)mWindow->ComCtrl->ProMap[item->text(0)])->onSelectMe();
    }
    treeWidget->setCurrentItem(item);
}

void TreeDock::addChildObject(QString root, QString node, QString property)
{
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(root,
                              Qt::MatchFixedString | Qt::MatchRecursive);

    if(qwilist.count())
    {
        QTreeWidgetItem *nqwi =  new QTreeWidgetItem(qwilist.at(0), QStringList() << node << property);
        treeWidget->setCurrentItem(nqwi);
    }
}

void TreeDock::addItemToRoot(QString node, QString property)
{
     QTreeWidgetItem *nroot = new QTreeWidgetItem(treeWidget,QStringList() << node << property);
     nroot->setIcon(0,QIcon(SHOW_ICON));
     treeWidget->setCurrentItem(nroot);
}

void TreeDock::addItemToRoot(QWidget *ww)
{

    QString key = ww->property(DKEY_LOCALSEQ).toString();
  //  tlist << key << ww->metaObject()->className();
     QTreeWidgetItem *nroot = new QTreeWidgetItem(treeWidget,
                          QStringList()  << key << ww->metaObject()->className());
     nroot->setIcon(0,QIcon(SHOW_ICON));

     treeWidget->setCurrentItem(nroot);
     mWindow->ComCtrl->ProMap[key] = ww;
}

void TreeDock::addObjectToCurrentItem(QString root,QWidget *ww)
{
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(root,
                              Qt::MatchFixedString | Qt::MatchRecursive);

    if(qwilist.count())
    {

        QString key = ww->property(DKEY_LOCALSEQ).toString();
        QString clsname = ww->metaObject()->className();
        QTreeWidgetItem *nqwi =  new QTreeWidgetItem(qwilist.at(0),
                                                     QStringList() << key << clsname);

        if(!CN_NEWLAYOUT.compare(clsname))
        {
            nqwi->setIcon(0,QIcon(SHOW_ICON));
        }
        mWindow->ComCtrl->ProMap[key] = ww;
        treeWidget->setCurrentItem(nqwi);

    }
}

void TreeDock::deleteItem(QWidget *obj)
{
    QString key = obj->property(DKEY_LOCALSEQ).toString();
   // QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(obj->objectName(),Qt::MatchFixedString | Qt::MatchRecursive);
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(key,Qt::MatchFixedString | Qt::MatchRecursive);

    foreach (QTreeWidgetItem *qwi, qwilist) {
        //qDebug() << " this text " << qwi->text(0);
       if(!qwi->text(0).compare(key))
       {
           // 在它的父控件里的列表里找到它,移除它.
           if(qwi->parent())
           {
               QWidget *parentControl =mWindow->ComCtrl->ProMap[qwi->parent()->text(0)];
               ((BaseForm*)parentControl)->removeChild(obj);
           }else
           {
               // 这里是顶层了.
               mWindow->cManager->activeSS()->LayerList.removeOne(obj);
           }

           treeWidget->removeItemWidget(qwi,0);
           mWindow->ComCtrl->ProMap.remove(key);
           delete qwi;
           break;
       }
    }

   // treeWidget->removeItemWidget(treeWidget->currentItem(),1);
}

void TreeDock::deleteAllitem()
{

    treeWidget->clear();
}

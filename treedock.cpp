#include "treedock.h"

#include "mainwindow.h"
#include <QMetaObject>
#include <QMetaProperty>
#include "formresizer.h"

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

setFixedHeight(mWindow->size().height()-50);
setFixedWidth(200);
treeWidget->setFixedHeight(mWindow->size().height()-80);
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
        // 这里只能隐
        if(!item->parent()) // 顶级
        {
            QIcon icon = item->icon(0);
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
     mWindow->ComCtrl->ProMap[key] = (FormResizer*)ww;
}

void TreeDock::addObjectToCurrentItem(QWidget *ww)
{

   //     qDebug() << "Tree  row Size " << mWindow->tree->treeWidget->children().size();
        QTreeWidgetItem *qwi = treeWidget->currentItem();
        if(qwi)
        {
           QStringList tlist;
           QString key = ww->property(DKEY_LOCALSEQ).toString();
           tlist << key << ww->metaObject()->className();
            //qDebug() << " add child to " << qwi->text(0) << qwi->text(1);
          // 如果是NEWFRAME 或者NEWLIST 就选择它的父节点.
           bool pbool = !qwi->text(1).compare(CN_NEWFRAME)/* || !qwi->text(1).compare(CN_NEWLIST)*/;
           QTreeWidgetItem *nqwi =  new QTreeWidgetItem(pbool ? qwi->parent() : qwi, tlist);

           mWindow->ComCtrl->ProMap[key] = (FormResizer*)ww;
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

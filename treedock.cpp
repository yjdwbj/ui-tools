#include "treedock.h"

#include "mainwindow.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QApplication>
#include "formresizer.h"
#include "scenesscreen.h"
#include "canvasmanager.h"
#include "module.h"


static QString HeadCol = "结点,属性";

class PageView;

TreeDock::TreeDock(QWidget *parent)
    :QDockWidget(parent),
      mWindow((MainWindow*)parent), treeWidget(new QTreeWidget())
{
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

this->setWidget(treeWidget);
//QTreeWidget *tree = new QTreeWidget();
treeWidget->setColumnCount(2);

treeWidget->setHeaderLabels(HeadCol.split(","));
setStyleSheet("background-color: #C0DCC0;");
connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
        SLOT(onItemPressed(QTreeWidgetItem*,int)));


setFixedWidth(200);

treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
connect(treeWidget,SIGNAL(customContextMenuRequested(QPoint)),
        SLOT(onCustomContextMenu(QPoint)));




}


void TreeDock::setSelectTreeItem(QWidget *obj)
{
    QString key  = ((BaseForm*)obj)->mUniqueStr;
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(key,
                                                            Qt::MatchFixedString |
                                                            Qt::MatchRecursive);
    foreach (QTreeWidgetItem *qwi, qwilist) {
        if(!qwi->text(0).compare(key)) // caption 加序号.
        {
            treeWidget->setCurrentItem(qwi);
            break;
        }
    }
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
    // QModelIndex index = treeWidget->indexAt(point);

    QTreeWidgetItem *item = treeWidget->itemAt(point);
    if(item)
    {

        QWidget *w = mWindow->ComCtrl->ProMap[item->text(0)];
        ((BaseForm*)w)->createContextMenu(treeWidget,
                                          treeWidget->viewport()->mapToGlobal(point));
    }
}


void TreeDock::onSwapShowHideObject(bool)
{
    QTreeWidgetItem *item = treeWidget->currentItem();
    if(item)
    {
        QString text = item->text(0);
        QWidget *ww = mWindow->ComCtrl->ProMap[text];
        for(int i = 0; i < item->childCount();i++)
        {
            item->child(i)->setHidden(!ww->isHidden());

        }
        ww->setHidden(!ww->isHidden());
        if(!ww->isHidden())
            mWindow->pageView->PressItem(mWindow->cManager->activeIndex());
        swapIconForItem(text);
    }
}

void TreeDock::setMyParentNode()
{
    QTreeWidgetItem *citem = treeWidget->currentItem();
    //  qDebug() << " current item text" << citem->text(0);

    if(citem)
    {

        QTreeWidgetItem *item= citem->parent();
        if(item)
        {
            QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonPress,QCursor::pos(),
                                                 Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
            QWidget *w = mWindow->ComCtrl->ProMap[item->text(0)];
            QApplication::postEvent(w,event);
        }
        else if(citem->text(1).compare(CN_NEWLAYER))
        {
            QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(CN_NEWLAYER,
                                                                    Qt::MatchFixedString | Qt::MatchRecursive,1);
            qwilist.removeOne(citem);
            if(qwilist.count())
            {
                QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonPress,QCursor::pos(),
                                                     Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
                QWidget *w = mWindow->ComCtrl->ProMap[qwilist.last()->text(0)];
                QApplication::postEvent(w,event);
            }
        }
    }

}

void TreeDock::onItemPressed(QTreeWidgetItem *item,int col)
{
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
        QTreeWidgetItem *nqwi =  new QTreeWidgetItem(qwilist.at(0),
                                                     QStringList() << node << property);
        treeWidget->setCurrentItem(nqwi);
    }
}

void TreeDock::addItemToRoot(QString node, QString property)
{



    QTreeWidgetItem *nroot = new QTreeWidgetItem(treeWidget,QStringList() << node << property);
    nroot->setIcon(0,QIcon(SHOW_ICON));
    treeWidget->setCurrentItem(nroot);

}

void TreeDock::addItemToRoot(QWidget *obj)
{

    QString key = /*ww->property(DKEY_LOCALSEQ).toString()*/((BaseForm*)obj)->mUniqueStr;;
    //    treeWidget->setCurrentItem(root);
    QTreeWidgetItem *nroot = new QTreeWidgetItem(treeWidget,
                                                 QStringList()  << key << obj->metaObject()->className());
    nroot->setIcon(0,obj->isHidden() ? QIcon(HIDE_ICON) : QIcon(SHOW_ICON));
    nroot->setExpanded(obj->isHidden());

    treeWidget->setCurrentItem(nroot);

    mWindow->ComCtrl->ProMap[key] = obj;

}

void TreeDock::addObjectToCurrentItem(QString root,QWidget *obj)
{
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(root,
                                                            Qt::MatchFixedString | Qt::MatchRecursive);

    if(qwilist.count())
    {

        QString key = /*ww->property(DKEY_LOCALSEQ).toString()*/((BaseForm*)obj)->mUniqueStr;;
        QString clsname = obj->metaObject()->className();
        QTreeWidgetItem *nqwi =  new QTreeWidgetItem(qwilist.first(),
                                                     QStringList() << key << clsname);

        if(obj->inherits(CN_NEWLAYOUT))
        {
            nqwi->setIcon(0,obj->isHidden() ? QIcon(HIDE_ICON) : QIcon(SHOW_ICON));
            //            nqwi->setExpanded(obj->isHidden());
        }
        mWindow->ComCtrl->ProMap[key] = obj;
        treeWidget->blockSignals(true);
        treeWidget->setCurrentItem(nqwi);
        ((BaseForm*)obj)->onSelectMe();
        treeWidget->blockSignals(false);
    }
}

void TreeDock::deleteItem(QWidget *obj)
{
    QString key = ((BaseForm*)obj)->mUniqueStr;
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
                mWindow->cManager->activeSS()->childlist.removeOne(obj);
            }

            treeWidget->removeItemWidget(qwi,0);
            mWindow->ComCtrl->ProMap.remove(key);
            delete qwi;
            break;
        }
    }
}

void TreeDock::deleteAllitem()
{

    treeWidget->clear();
}

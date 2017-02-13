#include "treedock.h"

#include "mainwindow.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QApplication>
#include "formresizer.h"
#include "scenesscreen.h"
#include "canvasmanager.h"
#include "module.h"


static QString HeadCol = "结点,属性,ID号";

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


setFixedWidth(parent->width() * 0.2);

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

    if(!qwilist.count())
        return;
    QTreeWidgetItem *item = qwilist.at(0);
    QWidget *w = BaseForm::mObjectMap[txt];
    item->setIcon(0,w->isHidden() ? QIcon(HIDE_ICON) : QIcon(SHOW_ICON));

}

void TreeDock::onCustomContextMenu(const QPoint &point)
{
    // QModelIndex index = treeWidget->indexAt(point);

    QTreeWidgetItem *item = treeWidget->itemAt(point);
    if(!item)
        return;

    QWidget *w = BaseForm::mObjectMap[item->text(0)];
    ((BaseForm*)w)->createContextMenu(treeWidget,
                                      treeWidget->viewport()->mapToGlobal(point));
}

void TreeDock::onSwapShowHideSubObject()
{
    QTreeWidgetItem *item =  treeWidget->currentItem();
    if(!item)
        return;

    QTreeWidgetItem *pitem = item->parent();
    if(!pitem)
        return;

    QString ctext = item->text(1);
    QWidget *cw = BaseForm::mObjectMap[item->text(0)];
    bool flag = !cw->isHidden();
    for(int i = 0 ; i < pitem->childCount();i++)
    {
        QTreeWidgetItem *citem = pitem->child(i);
        qDebug() << " child text  " << citem->text(0);

        if(flag == BaseForm::mObjectMap[citem->text(0)]->isHidden())
            continue;
        if(ctext.compare(citem->text(1)))
            continue;
        SwapItem(citem);
    }

}



void TreeDock::SwapShowHideObject_BF(BaseForm *bf)
{
    for(int i = 0; i < bf->childlist.count();i++)
    {
        BaseForm *cwid =((BaseForm *)bf->childlist.at(i));
        cwid->setHidden(!cwid->isHidden());
        addObjectToCurrentItem(bf->objectName() ,cwid);
        if(cwid->childlist.count())
        {
            SwapShowHideObject_BF(cwid);
        }
    }
}

void TreeDock::SwapShowHideObject(QTreeWidgetItem* item)
{
    if(!item)
        return;


    for(int i = 0; i < item->childCount();i++)
    {
        QTreeWidgetItem *it = item->child(i);
        if(!it)
            continue;
        QString t = it->text(0);
        SwapShowHideObject(it);

        QWidget *w = BaseForm::mObjectMap[t];
        w->setHidden(!w->isHidden());
        it->setHidden(!it->isHidden());
        if(it->text(1).compare(CN_NEWFRAME))
            it->setIcon(0,it->childCount() > 0 && it->child(0)->isHidden() ?
                            QIcon(HIDE_ICON) :QIcon(SHOW_ICON)  );

    }

}

void TreeDock::SwapItem(QTreeWidgetItem *item)
{
    QWidget *w = BaseForm::mObjectMap[item->text(0)];
    BaseForm *bf = (BaseForm*)w;
    w->setHidden(!w->isHidden());
    if( (bf->mType != BaseForm::T_NewFrame)  &&
            (bf->childlist.count() != item->childCount()))
    {
        SwapShowHideObject_BF(bf);
    }else
        SwapShowHideObject(item);
    if(item->text(1).compare(CN_NEWFRAME))
        item->setIcon(0,w->isHidden() ? QIcon(HIDE_ICON) : QIcon(SHOW_ICON));

}

void TreeDock::onSwapShowHideObject()
{
    QTreeWidgetItem *item = treeWidget->currentItem();
    if(!item)
        return;
    SwapItem(item);

}

void TreeDock::moveListItemOrder(QString pname, int src, int dst)
{

    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(pname,
                                                            Qt::MatchFixedString |
                                                            Qt::MatchRecursive);
    if(qwilist.count() > 0)
    {
        QTreeWidgetItem *item = qwilist.first();
        item->insertChild(dst,item->takeChild(src));
    }
}

void TreeDock::moveItemOrder(QTreeWidgetItem *item,int src,int dst)
{
    item->insertChild(dst,item->takeChild(src));
}


void TreeDock::moveTopItemOrder(int src,int dst)
{
    treeWidget->insertTopLevelItem(dst,treeWidget->takeTopLevelItem(src));
}

void TreeDock::setMyParentNode()
{
    QTreeWidgetItem *citem = treeWidget->currentItem();
    if(!citem)
        return;

    QTreeWidgetItem *item= citem->parent();
    if(item)
    {
        QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonPress,QCursor::pos(),
                                             Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
        QWidget *w = BaseForm::mObjectMap[item->text(0)];
        QApplication::postEvent(w,event);
    }
    else if(citem->text(1).compare(CN_NEWLAYER))
    {
        QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(CN_NEWLAYER,
                                                                Qt::MatchFixedString | Qt::MatchRecursive,1);
        qwilist.removeOne(citem);
        if(!qwilist.count())
            return;
        QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonPress,QCursor::pos(),
                                             Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
        QWidget *w = BaseForm::mObjectMap[qwilist.last()->text(0)];
        QApplication::postEvent(w,event);

    }


}

void TreeDock::onItemPressed(QTreeWidgetItem *item,int col)
{
    if(BaseForm::mObjectMap.contains(item->text(0)))
    {
        ((FormResizer*)BaseForm::mObjectMap[item->text(0)])->onSelectMe();
    }
    treeWidget->setCurrentItem(item);
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
    QString ename = ((BaseForm*)obj)->mEnameStr;
    QTreeWidgetItem *nroot = new QTreeWidgetItem(treeWidget,
                                                 QStringList()  << key
                                                 << obj->metaObject()->className()
                                                 << ename);
    nroot->setIcon(0,obj->isHidden() ? QIcon(HIDE_ICON) : QIcon(SHOW_ICON));
    nroot->setExpanded(obj->isHidden());

    treeWidget->setCurrentItem(nroot);

    BaseForm::mObjectMap[key] = obj;

}

void TreeDock::addObjectToCurrentItem(QString root,QWidget *obj)
{
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(root,
                                                            Qt::MatchFixedString | Qt::MatchRecursive);

    if(!qwilist.count())
        return;

    QString key = /*ww->property(DKEY_LOCALSEQ).toString()*/((BaseForm*)obj)->mUniqueStr;
    QString ename = ((BaseForm*)obj)->mEnameStr;
    QString clsname = obj->metaObject()->className();
    QTreeWidgetItem *nqwi =  new QTreeWidgetItem(qwilist.first(),
                                                 QStringList() << key << clsname << ename);

    if(obj->inherits(CN_NEWLAYOUT))
    {
        nqwi->setIcon(0,obj->isHidden() ? QIcon(HIDE_ICON) : QIcon(SHOW_ICON));
        //            nqwi->setExpanded(obj->isHidden());
    }
    BaseForm::mObjectMap[key] = obj;
    treeWidget->blockSignals(true);
    treeWidget->setCurrentItem(nqwi);
    ((BaseForm*)obj)->onSelectMe();
    treeWidget->blockSignals(false);

}

void TreeDock::deleteItem(QWidget *obj)
{
    QString key = ((BaseForm*)obj)->mUniqueStr;
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(key,Qt::MatchFixedString | Qt::MatchRecursive);

    foreach (QTreeWidgetItem *qwi, qwilist) {
        //qDebug() << " this text " << qwi->text(0);
        if(qwi->text(0).compare(key))
            continue;

        // 在它的父控件里的列表里找到它,移除它.
        if(qwi->parent())
        {
            QWidget *parentControl =BaseForm::mObjectMap[qwi->parent()->text(0)];
            ((BaseForm*)parentControl)->removeChild(obj);
        }else
        {
            // 这里是顶层了.
            mWindow->cManager->activeSS()->childlist.removeOne(obj);
        }

        treeWidget->removeItemWidget(qwi,0);
        BaseForm::mObjectMap.remove(key);
        delete qwi;
        break;
    }
}

void TreeDock::deleteAllitem()
{
    treeWidget->clear();
}

void TreeDock::updateSeq(QString val)
{
    QTreeWidgetItem *item = treeWidget->currentItem();
    if(!item)
        return;
    item->setText(2,val);
}

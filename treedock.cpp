#include "treedock.h"

#include "mainwindow.h"
#include <QMetaObject>
#include <QMetaProperty>

static QString HeadCol = "结点,属性";

TreeDock::TreeDock(MainWindow *mw, QWidget *parent)
    :QDockWidget(parent),
      mWindow(mw), treeWidget(new QTreeWidget())
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
connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),SLOT(onItemPressed(QTreeWidgetItem*,int)));
//root = new QTreeWidgetItem(treeWidget);
//root->setText(0,"根结点");
//root->setText(1,"画布");

this->setWidget(treeWidget);

this->setFeatures(DockWidgetMovable|DockWidgetFloatable);
//tree->setStyleSheet(style);

setFixedHeight(mWindow->size().height()-50);
setFixedWidth(200);
treeWidget->setFixedHeight(mWindow->size().height()-80);


}

void TreeDock::addCompoentControls(CompoentControls *cc)
{
    comC = cc;
}

void TreeDock::addPropBox(PropertyBox *p)
{
    pb = p;
}

void TreeDock::setSelectTreeItem(QWidget *obj)
{
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(obj->objectName(),Qt::MatchFixedString | Qt::MatchRecursive);
    foreach (QTreeWidgetItem *qwi, qwilist) {
        //qDebug() << " this text " << qwi->text(0);
       if(!qwi->text(0).compare(obj->objectName()))
       {
           treeWidget->setCurrentItem(qwi);
           break;
       }
    }
      //  treeWidget->setCurrentItem(it);
}

void TreeDock::onItemPressed(QTreeWidgetItem *item,int col)
{

   // if (root  == item)
    //    return;

    qDebug() << " clicked tree : " << item->text(0);
    foreach (QWidget *w, comC->comList) {
        if(!w->objectName().compare(item->text(0)))
        {
              ((FormResizer*)w)->onSelectMe();
                 break;
        }

    }
    treeWidget->setCurrentItem(item);
}

void TreeDock::addItemToRoot(QString node, QString property)
{
     QTreeWidgetItem *nroot = new QTreeWidgetItem(treeWidget,QStringList() << node << property);
     treeWidget->setCurrentItem(nroot);
}

void TreeDock::addObjectToLayout(QWidget *ww)
{

   //     qDebug() << "Tree  row Size " << mWindow->tree->treeWidget->children().size();
        QTreeWidgetItem *qwi = treeWidget->currentItem();
        if(qwi)
        {
            //qDebug() << " add child to " << qwi->text(0) << qwi->text(1);
           QTreeWidgetItem *nqwi =  new QTreeWidgetItem(!qwi->text(1).compare(CN_NEWFRAME) ? qwi->parent()
                                                                    : qwi, QStringList()  << ww->objectName() << ww->metaObject()->className());
           treeWidget->setCurrentItem(nqwi);
        }
}

void TreeDock::deleteItem(QWidget *obj)
{
    QList<QTreeWidgetItem*> qwilist = treeWidget->findItems(obj->objectName(),Qt::MatchFixedString | Qt::MatchRecursive);
    foreach (QTreeWidgetItem *qwi, qwilist) {
        //qDebug() << " this text " << qwi->text(0);
       if(!qwi->text(0).compare(obj->objectName()))
       {
           treeWidget->removeItemWidget(qwi,0);
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

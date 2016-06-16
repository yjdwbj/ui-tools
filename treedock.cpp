#include "treedock.h"

#include "mainwindow.h"
#include <QMetaObject>
#include <QMetaProperty>

static QString HeadCol = "结点,属性";

TreeDock::TreeDock(MainWindow *mw, QWidget *parent)
    :QDockWidget(parent),
      mWindow(mw), tree(new QTreeWidget())
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
tree->setColumnCount(2);





tree->setHeaderLabels(HeadCol.split(","));
connect(tree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),SLOT(onItemPressed(QTreeWidgetItem*,int)));




QTreeWidgetItem *root = new QTreeWidgetItem(tree);
root->setText(0,"根结点");
root->setText(1,"画布");



this->setWidget(tree);


this->setFeatures(DockWidgetMovable|DockWidgetFloatable);
tree->setStyleSheet(style);
// tree->adjustSize();

//  qDebug() << "  mWindow Size: "  << mWindow->size() ;
setFixedHeight(mWindow->size().height()-50);
setFixedWidth(200);
tree->setFixedHeight(mWindow->size().height()-80);

// tree->setFixedHeight(size().height()-50);
//  setStyleSheet("QTreeView{}");
// show();
}

void TreeDock::addCompoentControls(CompoentControls *cc)
{
    comC = cc;
}

void TreeDock::addPropBox(PropertyBox *p)
{
    pb = p;
}

void TreeDock::onItemPressed(QTreeWidgetItem *item,int col)
{

    if (tree->topLevelItem(0)  == item)
        return;

    qDebug() << " clicked tree : " << item->text(0);
    foreach (QWidget *w, comC->comList) {
        if(!w->objectName().compare(item->text(0)))
        {
            NewFrame *f = (NewFrame*)w;
            f->onSelectMe();
           // pb->createPropertyBox(w);

            break;
        }

    }


}

void TreeDock::addItemToRoot(QString node, QString property)
{
    QTreeWidgetItem *root =  tree->topLevelItem(0);
    QTreeWidgetItem *child = new QTreeWidgetItem(QStringList() << node << property);

    root->addChild(child);
}

/*
QVariant CompoentTreeModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole && role != Qt::DecorationRole)
        return QVariant();


}

void CompoentTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
*/

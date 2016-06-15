#include "treedock.h"

#include "mainwindow.h"
#include <QMetaObject>
#include <QMetaProperty>

static QString HeadCol = "结点,属性";

TreeDock::TreeDock(MainWindow *mw, QWidget *parent)
    :QDockWidget(parent),
      mWindow(mw)
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

    QTreeWidget *tree = new QTreeWidget();
    tree->setColumnCount(2);





    tree->setHeaderLabels(HeadCol.split(","));




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

#ifndef TREEDOCK_H
#define TREEDOCK_H
#include <QDockWidget>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include "compoentcontrols.h"
#include "propertybox.h"

class MainWindow;
class PropertyBox;
class CompoentControls;


class TreeDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit TreeDock(MainWindow *mw, QWidget *parent=Q_NULLPTR);
    void addItemToRoot(QString node,QString property);
    void addCompoentControls(CompoentControls *cc);
    void addPropBox(PropertyBox *p);

    void addObjectToLayout(QWidget *ww);

    void deleteAllitem();

    QTreeWidgetItem *getRoot()const { return root;}

    void setSelectTreeItem(QWidget *);
    QTreeWidget *treeWidget;



public slots:
    void onItemPressed(QTreeWidgetItem *item, int col);




private:
    MainWindow *mWindow;

    CompoentControls *comC;
    PropertyBox *pb;
    QTreeWidgetItem *root;
};


#endif // TREEDOCK_H

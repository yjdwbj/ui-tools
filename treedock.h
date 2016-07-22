#ifndef TREEDOCK_H
#define TREEDOCK_H
#include <QDockWidget>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include "compoentcontrols.h"
//#include "propertybox.h"

class MainWindow;
//class PropertyBox;
class CompoentControls;
class CanvasManager;
class ScenesScreen;


class TreeDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit TreeDock(QWidget *parent=Q_NULLPTR);
    void addItemToRoot(QString node,QString property);
    void addItemToRoot(QWidget *ww);
//    void addCompoentControls(CompoentControls *cc);
//    void addPropBox(PropertyBox *p);

    void addObjectToCurrentItem(QWidget *ww);
    void addChildObject(QString root,QString node,QString property);

    void deleteAllitem();
    void deleteItem(QWidget *obj);
    void swapIconForItem(QString txt);



    QTreeWidgetItem *getRoot()const { return root;}

    void setSelectTreeItem(QWidget *);

    void setMyParentNode();
    QTreeWidget *treeWidget;


public slots:
    void onItemPressed(QTreeWidgetItem *item, int col);
    void onCustomContextMenu(const QPoint &point);
    void onSwapShowHideObject(bool);




private:
    MainWindow *mWindow;

  //  CompoentControls *comC;
   // PropertyBox *pb;
    QTreeWidgetItem *root;
    QMenu *contextMenu ;

};


#endif // TREEDOCK_H

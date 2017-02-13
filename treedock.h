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
class BaseForm;



class TreeDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit TreeDock(QWidget *parent=Q_NULLPTR);
    void addItemToRoot(QString node,QString property);
    void addItemToRoot(QWidget *obj);

    void addObjectToCurrentItem(QString root, QWidget *obj);
    void addChildObject(QString root,QString node,QString property);

    void deleteAllitem();
    void deleteItem(QWidget *obj);
    void swapIconForItem(QString txt);

    void setSelectTreeItem(QWidget *);

    void setMyParentNode();
    void SwapShowHideObject_BF(BaseForm *bf);
    void SwapShowHideObject(QTreeWidgetItem* item);
    void updateSeq(QString val);
    void moveListItemOrder(QString pname, int src, int dst);
    void moveItemOrder(QTreeWidgetItem *item,int src,int dst);
    void moveTopItemOrder(int src, int dst);
    QTreeWidget *treeWidget;


public slots:
    void onItemPressed(QTreeWidgetItem *item, int col);
    void onCustomContextMenu(const QPoint &point);
    void onSwapShowHideObject();
    void onSwapShowHideSubObject();



private:
    MainWindow *mWindow;
    QMenu *contextMenu ;

    void SwapItem(QTreeWidgetItem *item);

};


#endif // TREEDOCK_H

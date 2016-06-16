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



public slots:
    void onItemPressed(QTreeWidgetItem *item, int col);



private:
    MainWindow *mWindow;
    QTreeWidget *tree;
    CompoentControls *comC;
    PropertyBox *pb;
};

/*
class CompoentTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CompoentTreeModel(QWidget *parent=Q_NULLPTR);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);

private:

};
*/
#endif // TREEDOCK_H

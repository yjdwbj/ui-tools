#ifndef TREEDOCK_H
#define TREEDOCK_H
#include <QDockWidget>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>

class MainWindow;


class TreeDock : public QDockWidget
{
    friend class MainWindow;
    Q_OBJECT
public:
    explicit TreeDock(MainWindow *mw, QWidget *parent=Q_NULLPTR);


private:
    MainWindow *mWindow;
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

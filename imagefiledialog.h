#ifndef IMAGEFILEDIALOG_H
#define IMAGEFILEDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListWidget>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QStringListModel>

typedef QMap<QString,QVariant> selectedMap;

class ImageFileDialog: public QDialog
{
    Q_OBJECT
public:
    explicit ImageFileDialog(QStringList old,QWidget *parent = 0);
    ~ImageFileDialog();

    //const selectedMap getSelectedMap() const { return selMap;}
    const QStringList getSelectedList() const { return selstrList;}
   // void setSelectedList(QStringList b) const { selstrList = b;}

private slots:
    void onTreeViewClicked(QModelIndex index);
    void onListViewDoubleClicked(QModelIndex index);
    void onSelListViewDoubleClicked(QModelIndex index);
    void onDelSelectedItems();
    void onAddSelectedItems();


private:
    QFileSystemModel *dirModel;
    QFileSystemModel *fileModel;
    QListView *flistview;
    QListWidget *sellist;
    QTreeView *treefile;
    QPushButton *add;
    QPushButton *del;


    QMap<QString,QModelIndex> hRows;
    //selectedMap selMap;
    QStringList selstrList;


};



#endif // IMAGEFILEDIALOG_H

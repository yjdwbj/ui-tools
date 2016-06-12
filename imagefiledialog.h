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

class ImageFileDialog: public QDialog
{
    Q_OBJECT
public:
    explicit ImageFileDialog(QWidget *parent = 0);
    ~ImageFileDialog();

private slots:
    void onTreeViewClicked(QModelIndex index);

private:
    QFileSystemModel *dirModel;
    QFileSystemModel *fileModel;
    QListView *flistview;
    QListWidget *sellist;
    QTreeView *treefile;
    QPushButton *add;
    QPushButton *del;


};



#endif // IMAGEFILEDIALOG_H

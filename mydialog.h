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

class MainWindow;

typedef QMap<QString,QVariant> selectedMap;

class ImageFileDialog: public QDialog
{
    Q_OBJECT
public:
    explicit ImageFileDialog(QVariantList old,QWidget *parent = 0);
    ~ImageFileDialog();

    //const selectedMap getSelectedMap() const { return selMap;}
    const QVariantList getSelectedList() const { return selstrList;}
    void updateListImages(QString path);
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
    QListWidget *flistview;
    QListWidget *sellist;
    QTreeView *treefile;
    QPushButton *add;
    QPushButton *del;

    QStringList filters;

    QMap<QString,QModelIndex> hRows;
    //selectedMap selMap;
    QVariantList selstrList;

};





namespace Ui {
    class ProjectDialog;
}

class ProjectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProjectDialog(QWidget *parent=0);
    ~ProjectDialog();

    QSize pageSize;
public slots:
    void onAccepted();
    void onRejected();
    void onFinished(int result);
    QSize getDefaultSize();

    void onSpinBoxVChanged(int);
private:
    Ui::ProjectDialog  *ui;
    MainWindow *mWindow;

protected:
    void closeEvent(QCloseEvent *);

};

class ImageListView : public QDialog
{
    Q_OBJECT
public:
    explicit ImageListView(QString path = QDir::currentPath(),QWidget *parent = 0);
private:

    void updateListImages(QString path);
    QFileSystemModel *fileModel;
    QFileSystemModel *treeModel;
//    QMap<QString,QString> imgMap;
    QVariantMap imgMap;
    QTreeView *treefile;
    QListWidget *imglist;
    QStringList filters;
private slots:
    void onTreeViewClicked(QModelIndex );

};


#endif // IMAGEFILEDIALOG_H

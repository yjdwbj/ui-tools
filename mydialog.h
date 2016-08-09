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
#include <QLabel>

class MainWindow;

//typedef QMap<QString,QVariant> selectedMap;

class CustomListWidget : public QListWidget
{
public:
    CustomListWidget(QWidget *parent=0);
    void performDrag();

protected:
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *e);




private:
    void checkMimeData(QDragMoveEvent *event);
    QPoint startPos;
};

class ImageFileDialog: public QDialog
{
    Q_OBJECT
public:
    explicit ImageFileDialog(QVariantList old,QWidget *parent = 0);
    ~ImageFileDialog();

    //const selectedMap getSelectedMap() const { return selMap;}
    const QVariantList getSelectedList() const { return selstrList;}
    void updateListImages(QString path);

    void setOldList();
   // void setSelectedList(QStringList b) const { selstrList = b;}

private slots:
    void onTreeViewClicked(QModelIndex index);
    void onListViewDoubleClicked(QModelIndex index);
    void onSelListViewDoubleClicked(QModelIndex index);
    void onDelSelectedItems();
    void onAddSelectedItems();
    void onUp();
    void onDown();
    void onSelectCurrentItem(QModelIndex index);



private:
    QWidget *createUpAndDownButton(int row);
    void updateListWidget();
    void appendSelectedItem(QModelIndex index);

    QFileSystemModel *dirModel;
    QFileSystemModel *fileModel;
    QListWidget *flistview;
    QListWidget *sellist;
//    CustomListWidget *flistview;
//    CustomListWidget *sellist;
    QTreeView *treefile;
    QPushButton *add;
    QPushButton *up;
    QPushButton *down;
    QPushButton *del;
    QLabel *statusBar;

    QStringList filters;

    QMap<QString,QModelIndex> hRows;
   // QMap<QString,QString> extMap;
    QVariantMap extMap;
    //selectedMap selMap;
    QVariantList selstrList;
protected:
    void dragEnterEvent();

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

    bool CheckLangFile(QString path);

    Ui::ProjectDialog  *ui;
    MainWindow *mWindow;
    QString defaultXLS;

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_pushButton_clicked();
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

namespace Ui {
    class I18nLanguage;
}

class I18nLanguage: public QDialog
{
    Q_OBJECT
public:
    explicit I18nLanguage(QWidget *parent=0);

    QStringList getSelectedItems(bool isLang=true);


    QListWidget *languageSel;
    QListWidget *menuSel;

private slots:
    void on_item_selectall_clicked();

    void on_item_dselectall_clicked();

    void on_item_re_clicked();

    void on_lang_selectall_clicked();

    void on_lang_dselectall_clicked();

    void on_lang_re_clicked();

private:
    bool CheckLangFile(QString path);

    MainWindow *mWindow;
    Ui::I18nLanguage *ui;
};



class MenuItemDialog: public QDialog
{
    Q_OBJECT
public:
    explicit MenuItemDialog(QString old,QWidget *parent=0);
    QString text;
private:
    QListWidget *listWidget;
    MainWindow *mWindow;


};

namespace Ui {
    class ConfigProject;
}


class ConfigProject: public QDialog
{
    Q_OBJECT
public:
    explicit ConfigProject(QWidget *parent = 0);

    QStringList getSelectLang();

private slots:
    void on_openfile_clicked();

    void on_lang_selectall_clicked();

    void on_lang_dselectall_clicked();

    void on_lang_re_clicked();



private:

    void updateListWidget();
     bool CheckLangFile(QString path);


    MainWindow *mWindow;
    Ui::ConfigProject *ui;
    QString defaultXLS;
};




#endif // IMAGEFILEDIALOG_H

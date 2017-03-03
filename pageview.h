#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include <QWidget>
#include <QDockWidget>
#include <QListWidget>
#include <QPixmap>
#include <QListWidget>

class MainWindow;
class PageView : public QDockWidget
{
    Q_OBJECT
public:
    explicit PageView(MainWindow *mw);
    void addMainWindow(MainWindow *w) { mWindow = w;}

    void addNewPage(QPixmap &);
    void addNewPage(QPixmap &,QString txt);
    void delPage(int index);
    void InsertPage(int index, QPixmap &p);
    void InsertPage(int index, QPixmap &p,QString txt);
    void PressItem(int index);
    void ReloadListView();

    MainWindow* mWindow;

public slots:
    //  void onClickedItem(QModelIndex);
    void onClickedItem(QListWidgetItem * );
//    void onItemChanged(QListWidgetItem*);

private:
    QListWidget *mImgList;
};

#endif // PAGEVIEW_H

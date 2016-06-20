#ifndef MODULE_H
#define MODULE_H
#include <QtWidgets/QFrame>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <QComboBox>
#include <QPainter>
#include <QPaintEvent>:q
#include "mainwindow.h"
#include "imagefiledialog.h"

#include "propertybox.h"


class MainWindow;
class ScenesScreen;
class NewLabel :public QLabel
{
    friend class PropertyBox;

    Q_OBJECT

signals:
    void Clicked();
public:
    NewLabel(QWidget *parent=0);
    enum widType {
        Number =0x0,
        Text = 0x1,
        List = 0x2,
        Enum = 0x3
    };


    void createPropertyBox();
    void addPropertyBoxSignal(QSpinBox *b);
    void updatePixmap(QString imgpath);
    void updateComboItems(QComboBox *cb);
    QString *defaultImg;
    MainWindow *mWindow;



protected:
    void mousePressEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QWidget* getQWidgetByName(QString name) const;
    QGroupBox* CreateXYWHGBox(QWidget *p);
    void removeWidFromLayout(QLayout* layout);
    void clearOtherObjectStyleSheet(QWidget *);



private slots:

    void onPictureDialog(bool );
    void onXYWHChangedValue(int);
    void onListImageChanged(QString);



private:


    void UpdateXYWHPos();


    QPoint mOffset;

    QStringList myImageList;
    int selIndex; /* 选中的当前的图片号 */
    bool disDefaultList; /* 屏闭默认的图片列表　*/

};

class NewFrame :public QWidget
{
    Q_OBJECT
public:
     QPoint mOffset;
    NewFrame(QWidget *parent=0);
    void mouseMoveEvent(QMouseEvent *event);
    void addMainWindow(QObject *mw);
    void onSelectMe();
    MainWindow *mWindow;

private:


signals:
   void Clicked();
private slots:

    void onXYWHChangedValue(int v);

protected:
    void mousePressEvent(QMouseEvent *event) ;
    void clearOtherObjectStyleSheet(QWidget *p);

   // void mouseDoubleClickEvent(QMouseEvent *event);

};

class NewLayer :public QFrame
{
    Q_OBJECT
public:
    explicit NewLayer(QSize nsize, QWidget *parent=0);
    NewLayer (int width,int height,QWidget *parent=0);
    void SelectLayer();
    void addMainWindow(MainWindow *m) { mWindow = m;}
    MainWindow *mWindow;


private:
    QPoint mOffset;
    QWidgetList mNewFrameList;


public slots:
    void onXYWHChangedValue(int v);


protected:

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);


};


#endif // MODULE_H

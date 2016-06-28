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
#include <QPaintEvent>
#include <QGraphicsItem>
#include "mainwindow.h"
#include "mydialog.h"

#include "propertybox.h"
#include "formresizer.h"


class MainWindow;
class ScenesScreen;

class Compoent
{

public:
//    Compoent(){}
//    ~Compoent(){}
    virtual void onBindValue(QWidget *w,const QVariant &val);
    QMap<QString,QVariant> dynValues;

};



class NewLabel :public QLabel ,public Compoent
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


    //void createPropertyBox();
    void addPropertyBoxSignal(QSpinBox *b);
    void updatePixmap(QString imgpath);
    void updateComboItems(QComboBox *cb);

  //  QMap<QString,QVariant> dynValues;
 //   void onBindValue(QWidget *w, const QVariant &val);

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
    void clearOtherObjectStyleSheet();
  //  void onBindValue(QWidget *w,const QVariant &val);

private slots:

    void onPictureDialog(bool );
    void onXYWHChangedValue(int);
    void onListImageChanged(QString);
    void onTextChanged();
    void onNumberChanged(int num);
    void onEnumItemChanged(QString txt);

private:
    void UpdateXYWHPos();
    QPoint mOffset;

    QStringList myImageList;
    int selIndex; /* 选中的当前的图片号 */
    bool disDefaultList; /* 屏闭默认的图片列表　*/

};

class NewFrame :public FormResizer,public Compoent
{
    Q_OBJECT
public:
     QPoint mOffset;
    NewFrame(QWidget *parent=0);

    void addMainWindow(QObject *mw);
    void onSelectMe();
    void delMySelf();
  //  void onBindValue(QWidget *w,const QVariant &val);

   // void onBindValue(QWidget *w, const QVariant &val);
  //  QMap<QString,QVariant> dynValues; //本控件的所有属性值
    MainWindow *mWindow;
private slots:

    void onXYWHChangedValue(int v);
    void onTextChanged();
    void onNumberChanged(int num);
    void onEnumItemChanged(QString txt);

protected:

//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event) ;
    void clearOtherObjectStyleSheet();

   // void mouseDoubleClickEvent(QMouseEvent *event);

};

//class NewLayout :public QFrame
class NewLayout :public FormResizer
{
    Q_OBJECT
public:
    explicit NewLayout(QSize nsize, QWidget *parent=0);
    NewLayout (int width,int height,QWidget *parent=0);

    void onSelectMe();
    void addMainWindow(MainWindow *m) { mWindow = m;}
    MainWindow *mWindow;
    void clearOtherSelectHandler();
    void appendChildObj(QWidget *w) { mChList.append(w);}
    void deleteObject(int index);
    void deleteObject(QWidget *w);
    const QWidgetList &getChildrenList() { return mChList;}

    void delMySelf();


private:
    QPoint mOffset;
   // QSize m_startSize;
    //QSize m_curSize;
    QWidgetList mChList;


    void clearOtherObjectStyleSheet();


public slots:
    void onXYWHChangedValue(int v);



protected:

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
 //   void paintEvent(QPaintEvent *);


};


#endif // MODULE_H

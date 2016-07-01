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
#include <QJsonArray>

//#include "propertybox.h"
#include "formresizer.h"


class MainWindow;
class ScenesScreen;

class Compoent
{

public:
    virtual void onBindValue(QWidget *w, const QVariantMap &map);
    virtual QJsonObject getRectJson(QWidget* w);
    virtual void copyProperty(const QVariant &va);
    virtual void changeJsonValue(QString key, QVariant val);
    QVariant getJsonValue(QString key) const;
    void updateJsonArray(QString key,const QJsonArray &arr);
    //QVariantMap dynValues;
    //QJsonValue dynValues;
    QJsonObject dynValues;

};



class NewLabel :public QLabel ,public Compoent
{
   // friend class PropertyBox;
    friend class ComProperty;
    friend class ImgProperty;
    Q_OBJECT
signals:
    void Clicked();
public:
    NewLabel(QWidget *parent=0);
    //void createPropertyBox();
    void addPropertyBoxSignal(QSpinBox *b);
    void updatePixmap(QString imgpath);
    void updateComboItems(QComboBox *cb);
    void writeToJson(QJsonObject &json);

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
    void onTextChanged(QString);
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
    void writeToJson(QJsonObject &json);

    MainWindow *mWindow;
private slots:

    void onXYWHChangedValue(int v);
    void onTextChanged(QString str);
    void onNumberChanged(int num);
    void onEnumItemChanged(QString txt);

protected:
    void clearOtherObjectStyleSheet();

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

    void clearOtherSelectHandler();
    void appendChildObj(QWidget *w) { mChList.append(w);}
    void deleteObject(int index);
    void deleteObject(QWidget *w);
    const QWidgetList &getChildrenList() { return mChList;}

    void delMySelf();

    void writeToJson(QJsonObject &json);

    MainWindow *mWindow;


private:
    QPoint mOffset;
    QWidgetList mChList;


    void clearOtherObjectStyleSheet();


public slots:
    void onXYWHChangedValue(int v);



protected:

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};


#endif // MODULE_H

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
#include <QMenu>
#include <QAction>

//#include "propertybox.h"
#include "formresizer.h"


class MainWindow;
class ScenesScreen;


class Compoent
{

public:
     Compoent(){}
     void onBindValue(QWidget *w, const QVariantMap &map);
     QJsonObject getRectJson(QWidget* w);
     QJsonObject getBorderJson(QWidget *w);
     void copyProperty(const QVariant &va);
     QVariant getJsonValue(const QJsonArray &arr,QString key) const;
     QVariant getJsonValue(QString key) const;
     void changeJsonValue(QString key, QVariant val);
     void changeJsonValue( QJsonArray &json,QString key,
                                    const QVariant &val);

     void updateRBJsonValue(QJsonArray &json,QWidget *w); // 更新UID,RECT,BORDER三个属性

    void updateJsonArray(QString key,const QJsonArray &arr);
     QJsonObject getRectFromStruct(const QJsonArray &arr);
    //QVariantMap dynValues;
    //QJsonValue dynValues;
    QJsonObject dynValues;
};

class JsonHandle
{

};



class NewLabel :public QLabel,public Compoent
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

    NewFrame(QWidget *parent=0);

    void addMainWindow(QObject *mw);
    void onSelectMe();
    void delMySelf();
    void writeToJson(QJsonObject &json);

    MainWindow *mWindow;
    QPoint mOffset;
private slots:

    void onXYWHChangedValue(int v);
    void onTextChanged(QString str);
    void onNumberChanged(int num);
    void onEnumItemChanged(QString txt);
    void onColorButtonClicked();
    void onColorButtonChanged();

protected:
    void clearOtherObjectStyleSheet();
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *event);
   // void paintEvent(QPaintEvent *);

};

class NewList :public FormResizer,public Compoent
{
    Q_OBJECT
public:
    NewList(QWidget *parent=0);
    void onSelectMe();
    void writeToJson(QJsonObject &json);
    void readFromJson(const QJsonObject &valobj);


     MainWindow *mWindow;
     QListWidget *lwid; // 列表控件.

private:
      QPoint mOffset;
      QWidget *mainWidget;
      QScrollArea *mainScroll;
      QListWidget *mainListWidget;
      QVBoxLayout *listLayout;
      QWidgetList listwidget;

public slots:
      void onAddOneLine();
      void onDeleteMe();
      void onColorButtonClicked();
      void onAdjustSize();
      void onListItemSizeChanged();
      void onXYWHChangedValue(int v);
      void onTextChanged(QString str);



protected:

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

//class NewLayout :public QFrame
class NewLayout :public FormResizer , public Compoent
{
    friend class ScenesScreen;
    friend class CompoentControls;
    Q_OBJECT
public:
    explicit NewLayout(QSize nsize, QWidget *parent=0);
//    NewLayout (const QJsonObject &json,QWidget *parent=0);
    NewLayout (int width,int height,QWidget *parent=0);

    void onSelectMe();
    void addMainWindow(MainWindow *m) { mWindow = m;}

    void clearOtherSelectHandler();
  //  void appendChildObj(QWidget *w) { mChList.append(w);}
    void deleteObject(int index);
    void deleteObject(QWidget *w);
    const QWidgetList &getChildrenList() { return childlist;}

    void delMySelf();

    void writeToJson(QJsonObject &json);
    void readFromJson(const QJsonObject &array);

    void createNewFrameObject(const QJsonObject &json);
   // QWidget *CreateObjectFromJson(const QVariantMap &qvm, QWidget *pobj);
    QWidget* createObjectFromJson(const QJsonObject &json,QWidget *pobj);


    void parseJsonProperty(QWidget *nobj, const QJsonArray &array);
    void addNewObject(QWidget *w) { childlist.append(w);}


    MainWindow *mWindow;


private:
    void clearOtherObjectStyleSheet();

    QPoint mOffset;
    QWidgetList childlist;

public slots:
    void onXYWHChangedValue(int v);
    void onDeleteMe();
    void onBeComeTemplateWidget();

signals:
      void sizeChanged();

protected:

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};




// 图层与布局的基础控件.
class BaseForm: public FormResizer
{
    Q_OBJECT
public:
    explicit BaseForm(QWidget *parent=0);


    MainWindow *mWindow;



    virtual void DeleteMe() = 0;

    void HideMe();

public slots:
    void onXYWHChangedValue(int v);
    void onSwapViewObject(bool b);

private:

    QPoint mOffset;



protected:
    virtual  void clearOtherObjectStyleSheet() = 0;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);




};


// 场景下面放图层,图层下面放布局,布局下面放控件.
class NewLayer : public BaseForm
{
    Q_OBJECT
public:
    explicit NewLayer(QSize nsize, QWidget *parent=0);
    void onSelectMe();

   // MainWindow *mWindow;
    void DeleteMe();
    void createNewLayout();
    void readFromJson(const QJsonArray &array);
    void createNewLayout(QWidget *parent);
    NewLayout *activeLayout() {
            return mActiveIdx == -1 ? (NewLayout*)0 :
                                      (NewLayout*)(LayoutList.at(mActiveIdx));}
    void setActiveLayout(QWidget *w) { mActiveIdx = LayoutList.indexOf(w);}
    void writeToJson(QJsonObject &json);

public slots:
    void onDeleteMe();

private:
    void clearOtherObjectStyleSheet();
    QPoint mOffset;
    int mActiveIdx;
    QWidgetList LayoutList; // 它下面的布局子控件.
protected:
    void resizeEvent(QResizeEvent *event);

//protected:
//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MODULE_H

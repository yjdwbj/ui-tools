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
     static QJsonObject getRectJson(QWidget* w);
     QJsonObject getBorderJson(QWidget *w);
     void copyProperty(const QVariant &va);
     static QVariant getJsonValue(const QJsonArray &arr,QString key);
     QVariant getJsonValue(QString key) const;
     void changeJsonValue(QString key, QVariant val);
     QJsonValue changeJsonValue( const QJsonArray &json,QString key,
                                    const QVariant &val);

     QJsonArray updateRBJsonValue(const QJsonArray &json,QWidget *w); // 更新UID,RECT,BORDER三个属性

    void updateJsonArray(QString key,const QJsonArray &arr);
    static QJsonObject getValueFromProperty(const QJsonArray &arr,const QString &key);

    static QRect getRectFromStruct(const QJsonArray &arr, QString key) ;


     QRect getRectFromJson(const QJsonObject &json) const;

    QJsonObject dynValues;
};




// 图层与布局的基础控件.
class BaseForm: public FormResizer,public Compoent
{
    Q_OBJECT
public:
    explicit BaseForm(QWidget *parent=0);
    MainWindow *mWindow;
    QRect mBorder;
    QPoint mOffset;
    QList<QWidget*> childlist;
    QWidget *parentControl;
    QString mBorderColor;
    QString mbkColor;
    QString mbkImage;


    void onSelectMe();
    virtual void DeleteMe();
    void HideMe();
    void addChildrenToTree();
    void updateStyleSheets() ;
    void removeChild(QWidget *w);
    void initJsonValue();

    void moveNewPos(QPoint pos);
    void moveNewPos(int x,int y);



public slots:
    void onXYWHChangedValue(int v);
    void onSwapViewObject(bool b);

    void onTextChanged(QString str);
    void onTextSelected();
    void onNumberChanged(int num);
    void onEnumItemChanged(QString txt);
    void onColorButtonClicked();
    void onBorderChangedValue(int v);
    void onBackgroundImageDialog();
    void onSelectedBackgroundImage(QListWidgetItem *item);

    virtual void onDeleteMe() =0;
    void onPictureDialog(bool );
    void onListImageChanged(QString);

protected:
    void clearOtherObjectStyleSheet();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);

};



//class NewLabel :public QLabel,public Compoent
//{
//   // friend class PropertyBox;
//    friend class ComProperty;
//    friend class ImgProperty;
//    Q_OBJECT
//signals:
//    void Clicked();
//public:
//    NewLabel(QWidget *parent=0);
//    //void createPropertyBox();
//    void addPropertyBoxSignal(QSpinBox *b);
//    void updatePixmap(QString imgpath);
//    void updateComboItems(QComboBox *cb);
//    void writeToJson(QJsonObject &json);

//    QString *defaultImg;
//    MainWindow *mWindow;



//protected:
//    void mousePressEvent(QMouseEvent *event) ;
//    void mouseReleaseEvent(QMouseEvent *ev);
//    void mouseDoubleClickEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    QWidget* getQWidgetByName(QString name) const;
//    QGroupBox* CreateXYWHGBox(QWidget *p);
//    void removeWidFromLayout(QLayout* layout);
//    void clearOtherObjectStyleSheet();
//  //  void onBindValue(QWidget *w,const QVariant &val);

//private slots:

//   // void onPictureDialog(bool );
//    void onXYWHChangedValue(int);
//    void onListImageChanged(QString);
//    void onTextChanged(QString);
//    void onNumberChanged(int num);
//    void onEnumItemChanged(QString txt);

//private:
//    void UpdateXYWHPos();
//    QPoint mOffset;

//    QStringList myImageList;
//    int selIndex; /* 选中的当前的图片号 */
//    bool disDefaultList; /* 屏闭默认的图片列表　*/

//};



//class NewFrame :public FormResizer,public Compoent
class NewFrame :public BaseForm
{
    Q_OBJECT
public:

    NewFrame(QString caption,QWidget *parent=0);

    void addMainWindow(QObject *mw);
  //  void onSelectMe();
  //  void DeleteMe();
    QJsonObject  writeToJson();
    void readFromJson(const QJsonValue &json);



public slots:
    void onDeleteMe();


protected:
    void clearOtherObjectStyleSheet();
   // void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *event);
  //  void updateBorderColor();
   // void paintEvent(QPaintEvent *event);

};

//class NewList :public FormResizer,public Compoent
class NewList :public BaseForm
{
    Q_OBJECT
public:
    NewList(QString caption,const QSize size,QWidget *parent=0);
    //void onSelectMe();
    QJsonObject writeToJson();
    void readFromJson(const QJsonObject &valobj);
    void addChildrenToTree();

    // MainWindow *mWindow;
     QScrollArea *mainScroll;
    // QWidgetList listwidget;

private:
     // QPoint mOffset;
      QWidget *mainWidget;
      QVBoxLayout *listLayout;

public slots:
      void onAddOneLine();
      void onDeleteMe();

     // void onXYWHChangedValue(int v);
      void onTextChanged(QString str);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
   // void updateBorderColor();
};

//class NewLayout :public QFrame
//class NewLayout :public FormResizer , public Compoent
class NewLayout :public BaseForm
{
    friend class ScenesScreen;
    friend class CompoentControls;
    Q_OBJECT
public:
    explicit NewLayout(QString caption,QSize nsize,MainWindow *w, QWidget *parent=0);
//    NewLayout (const QJsonObject &json,QWidget *parent=0);
    NewLayout (int width,int height,QWidget *parent=0);

    void onSelectMe();
    void DeleteMe();
    void addMainWindow(MainWindow *m) { mWindow = m;}
    void clearOtherSelectHandler();

    void deleteObject(int index);
    void deleteObject(QWidget *w);
    //void delMySelf();

    void writeToJson(QJsonObject &json);
    QJsonObject writeToJson();
    void readFromJson(const QJsonValue &qv);
    void addChildrenToTree();

    void createNewFrameObject(const QJsonObject &json);
    QWidget* createObjectFromJson(const QJsonValue &qv);

    QWidget *parentList; // 特意用来存放的
    QString StyleStr;


private:
    void clearOtherObjectStyleSheet();

public slots:
   // void onXYWHChangedValue(int v);
    void onDeleteMe();
    void onBeComeTemplateWidget();


protected:

    void mouseMoveEvent(QMouseEvent *event) override;
    //void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
   // void updateBorderColor();
    //void paintEvent(QPaintEvent *ev);

};


// 场景下面放图层,图层下面放布局,布局下面放控件.
class NewLayer : public BaseForm
{
    Q_OBJECT
public:
    explicit NewLayer(QString caption, QSize nsize, QWidget *parent=0);
    void readFromJson(const QJsonValue &qv);

    QJsonObject  writeToJson() ;
   // const QWidgetList &getChidren() const { return LayoutList;}
    void addChildrenToTree();
   // void updateBorderColor();

public slots:
    void onDeleteMe();

private:
    void clearOtherObjectStyleSheet();

//protected:
//    void resizeEvent(QResizeEvent *event);
  //  void paintEvent(QPaintEvent *);

//protected:
//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MODULE_H

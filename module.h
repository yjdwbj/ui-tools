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
class NewLayout;

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

    int tinySpinBoxDialog(QString  str,int val ,int min ,int max);
    QSize ChangeSizeDialog(QSize size);



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
    void wheelEvent(QWheelEvent *);

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



class ContainerScroll : public QScrollArea
{
    Q_OBJECT
public:
    ContainerScroll(QWidget *parent=0);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void  wheelEvent(QWheelEvent *);
    QWidget *container;
};


//class NewFrame :public FormResizer,public Compoent
class NewFrame :public BaseForm
{
    Q_OBJECT
public:

    NewFrame(QString caption,QWidget *parent=0);

    void addMainWindow(QObject *mw);
    QJsonObject  writeToJson();
    void readFromJson(const QJsonValue &json);

public slots:
    void onDeleteMe();
protected:
    void clearOtherObjectStyleSheet();
    void mouseMoveEvent(QMouseEvent *event);
};


class NewGrid: public BaseForm
{
    Q_OBJECT
public:
    NewGrid(const QJsonValue &qv, const QSize size,  const QList<int> *arglist, QWidget *parent=0);
    bool rowcoldialog();
    void initRowsCols(int row, int col);

    void addRowsCols();

    void addOneCol();
    void updateAllItemsSize();

    QAction *menuAddRow;
    QAction *menuAddCol;
    QAction *menuSpace;
    QAction *menuSize;
    QActionGroup *menuOrientation;
    QAction *menuV,*menuH;



public slots:
    void onDeleteMe();
    void onAddOneRow();
    void onAddOneCol();


private:


    int rows,cols;  //行列数
    int rowH,rowW;  //行高行宽
    int colH,colW;  //列高列宽

    QGridLayout *gridLayout;
    QScrollArea *mainScroll;
    QWidget *mainWidget;
    Qt::Orientation sliderOrientation;
    QSize itemSize;

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
};

//class NewList :public FormResizer,public Compoent
class NewList :public BaseForm
{
    Q_OBJECT
public:
    NewList(QJsonValue json,const QSize size,QWidget *parent=0);
    QJsonObject writeToJson();
    void readFromJson(const QJsonObject &valobj);
    void addChildrenToTree();
   // QScrollArea *mainScroll;
    ContainerScroll *mainScroll;
    QWidget *mainWidget;
     QBoxLayout *listLayout;
     Qt::Orientation  sliderOrientation;


     // 菜单项.
     QAction *menuAddLine;
     QAction *menuSetHeight;
     QAction *menuSetSpace;

private:
      int tinySpinBoxDialog(QString str, int val, int min, int max);
      NewLayout *onAddOneLine();
      void updateAllItemsSize();
      int itemHeight; // 垂直是宽随父控件,水平是高随父控件.

public slots:

      void onAddManyLine();
      void onSetFixedHeight();
      void onDeleteMe();
      void onTextChanged(QString str);

protected:
    //void mouseMoveEvent(QMouseEvent *event);
   // void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *);
    bool eventFilter(QObject *obj, QEvent *event);
   // void updateBorderColor();
};

class NewLayout :public BaseForm
{
    friend class ScenesScreen;
    friend class CompoentControls;
    Q_OBJECT
public:
    explicit NewLayout(QString caption,QSize nsize,MainWindow *w, QWidget *parent=0);
//    NewLayout (const QJsonObject &json,QWidget *parent=0);
    NewLayout (int width,int height,QWidget *parent=0);

   // void onSelectMe();
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

    QWidget *container; // 特意用来存放的
    QString StyleStr;
private:
    void clearOtherObjectStyleSheet();
    QList<int> rowcoldialog();

public slots:
   // void onXYWHChangedValue(int v);
    void onDeleteMe();
    void onBeComeTemplateWidget();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};


// 场景下面放图层,图层下面放布局,布局下面放控件.
class NewLayer : public BaseForm
{
    Q_OBJECT
public:
    explicit NewLayer(QString caption, QSize nsize, QWidget *parent=0);
    void readFromJson(const QJsonValue &qv);
    QJsonObject  writeToJson() ;
    void addChildrenToTree();

public slots:
    void onDeleteMe();

private:
    void clearOtherObjectStyleSheet();
};

#endif // MODULE_H

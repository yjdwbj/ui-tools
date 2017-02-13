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

#include "formresizer.h"



class MainWindow;
class ScenesScreen;
class NewLayout;
class NewLayer;
class NewGrid;
class NewList;
class Position;





class Compoent
{
public:
    Compoent(){}
    void onBindValue(QWidget *w);
    static QJsonObject getRectJson(QWidget* w);
    QJsonObject getBorderJson(QWidget *w);
    void copyProperty(const QVariant &va);
    static QVariant getJsonValue(const QJsonArray &arr,QString key);
    QVariant getJsonValue(QString key) const;
    QVariant getJsonValue(const QJsonArray &arr, QString key,int index);
    void changeJsonValue(int idex, const QVariant &val);
    void changeJsonValue(QString key, const QVariant &val);
    QJsonValue changeJsonValue( const QJsonArray &json,QString key,
                                const QVariant &val);
    void changeJsonValue(QWidget *w, QString key, const QVariant &val);
    QJsonValue changeJsonValue(const QJsonArray &json, int index, const QVariant &val);

    QJsonArray updateRBJsonValue(const QJsonArray &json,QWidget *w); // 更新UID,RECT,BORDER三个属性
    void updateJsonArray(QString key,const QJsonArray &arr);
    static QJsonObject getValueFromProperty(const QJsonArray &arr,const QString &key);
    static QVariant getJsonValue(const QJsonValue &val, QString key);
    static QRect getRectFromStruct(const QJsonArray &arr, QString key) ;
    QJsonArray getActionListJson();
    QRect getRectFromJson(const QJsonObject &json) const;
    QString getEnameFromJson(const QJsonArray &arr);

    // QJsonObject dynValues;
    QJsonObject mOwerJson;
};




class BaseScrollArea: public QScrollArea
{
    Q_OBJECT
public:
    BaseScrollArea(QWidget *parent=0);
private:
    void paintEvent(QPaintEvent *);
};



// 图层与布局的基础控件.
class BaseForm: public FormResizer,public Compoent
{
    Q_OBJECT
    Q_ENUMS(ObjTypes)
public:

    enum ObjTypes {
        T_NewLayer =  0x0,
        T_NewLayout = 0x1,
        T_NewFrame = 0x2,
        T_NewList = 0x3,
        T_NewGrid = 0x4,
        TYPESS = 0x5,
        Object = 0x6
    //        TYPECONTAINER = TYPEGRID | TYELIST
    };

//    Q_DECLARE_FLAGS(ObjFlags, ObjType)
    Q_ENUM(ObjTypes)


    enum SwapType {
        UpOne = 0x0,
        DownOne = 0x1,
        DownBottom = 0x3,
        UpTop = 0x4
    };

    explicit BaseForm(QWidget *parent=0);
    MainWindow *mWindow;
    BaseForm *mParent; // 父对像

    QPoint mOffset;
    QRect mOldRect;
    QPoint mOldPos;
    QSize mOldSize;
    QList<QWidget*> childlist;
//    QWidget *parentControl;

    //　界面显示的一些变量.
    QString mBorderColor;
    QString mbkColor;
    QString mbkImage;
    Position *posWidget;
    QRect mBorder;

    bool mCreateFlag; // 区分这是从原始模版读取的,还是工程读取

    QString mUniqueStr;
    QString mEnameStr;

    void initialEname();
    void onSelectMe();
    virtual void DeleteMe();
    void HideMe();
    void addChildrenToTree();
    void updateStyleSheets() ;
    void removeChild(QWidget *w);
    void initJsonValue();
    void createContextMenu(QWidget *parent, QPoint pos);
    void SwapLayerOrder(SwapType st);
    bool isContainer(){
        return  (mType == T_NewList) || (mType == T_NewGrid);
    }

    QString updateEname(int index);

    QJsonObject ContainerWriteToJson(QWidget *w);
    QWidget* getPairWidgetFromPLayout(QWidget *sender);

    void moveNewPos(QPoint pos);
    void moveNewPos(int x,int y);

    int tinySpinBoxDialog(QString  str,int val ,int min ,int max);
    QSize ChangeSizeDialog(QSize size);

    void mouseMoveToPos(const QPoint& p);

    virtual QJsonObject writeToJson() = 0;

    NewLayout *CreateNewLayout(const QJsonValue &qv,
                               QWidget *parent, bool isCreate, bool incontainer);

    //    QJsonValue mPropertyJson;


    ObjTypes mType;
    int mPageIndex;

    static ObjTypes mCopyFromType ;
    static QJsonValue mCopyItem;
    static bool mPrjIsChanged;


    static QString getSequence(const QString &key);
    static QString getEnameSeq(const QString &key, QWidget *obj);

    static QMap<QString,QWidget*> mObjectMap; // 新生成的控件.
    static QMap<QString,QWidget*> mSeqEnameMap; // 对应到小机里的唯一名称.




public slots:
    void onXYWHChangedValue(int v);
    void onSwapViewObject();
    void onClearJsonValue();

    void onTextChanged(QString str);
    void onTextSelected();
    void onNumberChanged(int num);
    void onEnumItemChanged(QString txt);
    void onColorButtonClicked();
    void onBorderChangedValue(int v);
    void onBackgroundImageDialog();
    void onActionDialog();


    virtual void onDeleteMe() =0;

    void onListImageChanged(QString);

protected:
    void clearOtherObjectStyleSheet();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *);
    void initObject(const QJsonObject &json);
    void objectMoveSwapMenu(QMenu *contextMenu);
    void listObjectMoveMenu(QMenu *contextMenu, BaseForm *container);
    void dropEvent(QDropEvent *e);
    void dragEnterEvent(QDragEnterEvent *);

};

class NewFrame :public BaseForm
{
    Q_OBJECT
public:

    NewFrame(const QJsonObject &json,QWidget *parent=0);

    void addMainWindow(QObject *mw);
    QJsonObject  writeToJson();
//    void readFromJson(const QJsonValue &json);

public slots:
    void onDeleteMe();
protected:
    void clearOtherObjectStyleSheet();

};


class NewGrid: public BaseForm
{
    Q_OBJECT
public:
    NewGrid(const QJsonValue &qv,  const QList<int> *arglist, QWidget *parent=0);

    bool rowcoldialog();
    void initRowsCols(int row, int col, const QJsonValue &value);

    void addRowsCols();

    void addOneCol();
    void updateAllItemsSize();
    QJsonObject writeToJson();
    void readFromJson(const QJsonValue &value);



    BaseScrollArea *mainScroll;
    QWidget *mainWidget;
    QAction *menuAddRow;
    QAction *menuAddCol;
    QAction *menuSpace;
    QAction *menuSize;
    QActionGroup *menuOrientation;
    QAction *menuV,*menuH;
    int rows =0,cols=0;  //行列数
    QSize itemSize;



public slots:
    void onDeleteMe();
    void onAddOneRow();
    void onAddOneCol();


private:
    int rowH,rowW;  //行高行宽
    int colH,colW;  //列高列宽

    QGridLayout *gridLayout;
    Qt::Orientation sliderOrientation;


protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

class NewList :public BaseForm
{
    Q_OBJECT
public:
    NewList(const QJsonValue &json,const QSize size,QWidget *parent=0);
    QJsonObject writeToJson();
    void readFromJson(const QJsonValue &valobj);
    void updateAllItemsSize();
    // void addChildrenToTree();
    QScrollArea *mainScroll;
    // ContainerScroll *mainScroll;
    QWidget *mainWidget;
    QBoxLayout *listLayout;
    Qt::Orientation  sliderOrientation;
//    void pasteAddOneLine(const QJsonValue &value);
    void updateOneItem(QWidget *w,int width,int height);
    NewLayout *AddOneLine(const QJsonValue &value);
    void pasteOneLine(const QJsonValue &value);


    // 菜单项.
    QAction *menuAddLine;
    QAction *menuSetHeight;
    QAction *menuSetSpace;

private:


    int itemHeight; // 垂直是宽随父控件,水平是高随父控件.

public slots:

    void onAddManyLine();
    void onSetFixedHeight();
    void onDeleteMe();


protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *);
    bool eventFilter(QObject *obj, QEvent *event);
};

class NewLayout :public BaseForm
{
    friend class ScenesScreen;
    friend class CompoentControls;
    Q_OBJECT
public:
    explicit NewLayout(const QJsonObject &json, QRect rect, MainWindow *w, QWidget *parent=0);
    NewLayout (int width,int height,QWidget *parent=0);

    void DeleteMe();
    void addMainWindow(MainWindow *m) { mWindow = m;}
    void clearOtherSelectHandler();

    void deleteObject(int index);
    void deleteObject(QWidget *w);


    void writeToJson(QJsonObject &json);
    QJsonObject writeToJson();
    void readFromJson(const QJsonValue &qv, bool flag);


    void createNewFrameObject(const QJsonObject &json);
    QWidget* createObjectFromJson(const QJsonValue &qv);

//    QWidget *container; // 特意用来存放的

private:
    void clearOtherObjectStyleSheet();
    QList<int> rowcoldialog();

public slots:
    void onDeleteMe();
    void onBeComeTemplateWidget();
};


// 场景下面放图层,图层下面放布局,布局下面放控件.
class NewLayer : public BaseForm
{
    Q_OBJECT
public:
    explicit NewLayer(const QJsonObject &json, QRect rect, QWidget *parent=0);
    void readLayoutFromJson(const QJsonValue &qv, bool flag);

    QJsonObject  writeToJson() ;

public slots:
    void onDeleteMe();

private:
    void clearOtherObjectStyleSheet();
};


// 场景下面放页面,页面下面放图层,图层下面放布局,布局下面放控件.
//class NewPage : public BaseForm
//{
//    Q_OBJECT
//public:
//    explicit NewPage(QString caption, QSize nsize, QWidget *parent=0);
//    void readLayoutFromJson(const QJsonValue &qv, bool flag);
//    QJsonObject  writeToJson() ;

//public slots:
//    void onDeleteMe();
//};

#endif // MODULE_H

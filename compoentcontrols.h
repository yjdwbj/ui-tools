#ifndef COMPOENTCONTROLS_H
#define COMPOENTCONTROLS_H

#include <QWidget>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QDateTime>
#include <QSpinBox>
#include <QLineEdit>
#include <QSignalMapper>
#include <QComboBox>
#include "mainwindow.h"

#include "treedock.h"


class TreeDock;
class MainWindow;
class ScenesScreen;
class BaseForm;
class NewFrame;


void removeWidFromLayout(QLayout *layout);

//坐标类
class Position: public QGroupBox
{
    Q_OBJECT
public:
    explicit Position(QWidget *parent=0);
    ~Position();
    //QGroupBox *CreateXYWHGBox(QWidget *p);

    void setConnectNewQWidget(QWidget *com);
    void updatePosition(QPoint pos);
    void updateSize(QSize size);
    void updateMaxSize(QSize size);
    void resetValues();
private:
    QSpinBox *Xpos,*Ypos,*Wpos,*Hpos;
    QWidget *old;
    QList<QMetaObject::Connection> connections;
};

// 内边框显示的类
class Border: public QGroupBox
{
    Q_OBJECT
public:
    explicit Border(QWidget *parent=0);

    //QGroupBox *CreateXYWHGBox(QWidget *p);

    void setConnectNewQWidget(QWidget *com);
//    void updateBorder(QPoint pos);
//    void updateSize(QSize size);
    void resetValues();
private:
    QSpinBox *Left,*Top,*Right,*Bottom; // left,top ,right,bottom
    QWidget *old;
    QList<QMetaObject::Connection> connections;
};

class ComProperty: public QGroupBox
{
    Q_OBJECT
public:
    explicit ComProperty(QString title, QWidget *parent=0);
    ~ComProperty(){}

    void createPropertyBox(QWidget *p);
   // void parseJsonToWidget(QWidget *p, const QVariantList &qvl, QLayout *layout);

    void parseJsonToWidget(QWidget *p, const QJsonArray &array);
    void parseJsonToWidget(QWidget *p, const QJsonObject &array, QLayout *layout);
    void delPropertyBox();
    void updateImageComboBox(QString key, int index, const QStringList &list);
    QWidget* getPropertyObject(QString key) const { return widgetMap.value(key);}

private:
    // QGroupBox *CreateXYWHGBox(QWidget *p);
     QVBoxLayout* mainLayout;
     QWidget *mainWidget;
     QWidget *oldobject;
     QMap<QString,QWidget*> widgetMap;  // 预备着多个同类型的控件区分.

};


class CompoentControls : public QGroupBox
{
    friend class PropertyBox;
    friend class TreeDock;
    friend class NewLayer;
    friend class NewLayout;
    friend class NewList;
    friend class NewFrame;
    friend class ScenesScreen;
    friend class CanvasManager;
    Q_OBJECT
public:
    explicit CompoentControls(MainWindow *mw,QWidget *parent = 0);
  //  QObject* CreateObjectFromJson(QVariantMap qvm, QObject *pobj);

   // QWidget *CcWidgetList,ProWidgetList,ImgWidgetList;

     int mCWidgetCount; // 自定义控件的个数.
     QMap<QString,QWidget*> ProMap; // 新生成的控件.

private:
     MainWindow *mWindow;
     QVariantMap mJsonMap;
     //QGridLayout* mainLayout;
     QVBoxLayout* mainLayout;
     QWidget *mainWidget;
     QString mJsonFile;
     QWidgetList comList;
     //QJsonArray comJsonArr;
     QMap<QString,QVariantMap> comMap;

     //QSizePolicy mSizePolicy;

     void CreateButtonList(const QJsonArray &comJsonArr);
     QWidget *createCustomObject(const QJsonArray &comJsonArr);
    // QWidget *createLayoutFromJson(const QJsonObject &json,QWidget *parent);
    // QWidget *createCustomWidget(const QJsonValue &json, QWidget *parent);

     QWidget *getQWidgetByName(QString name) const;
     QJsonArray ReadTemplateWidgetFile(QString file) const;
    // void ReadCustomWidgetFiles(QString file);

signals:

public slots:
     void onCreateCompoentToCanvas();
   //  void onCreateListWidget();
     void onCreateCustomWidget();
     void onCreateNewLayout();
     void onCreateNewLayer();
};

#endif // COMPOENTCONTROLS_H

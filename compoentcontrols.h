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
class FormResizer;
class NewFrame;


void removeWidFromLayout(QLayout *layout);

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
    void resetValues();
private:
    QSpinBox *Xpos,*Ypos,*Wpos,*Hpos;
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
    Q_OBJECT
public:
    explicit CompoentControls(MainWindow *mw,QWidget *parent = 0);
    QObject* CreateObjectFromJson(QVariantMap qvm, QObject *pobj);
    NewFrame *ReadObjectFromJson(QVariantMap qvm, QObject *, QString txt, QString objname);
   // QWidget *CcWidgetList,ProWidgetList,ImgWidgetList;

private:
     MainWindow *mWindow;
     QVariantMap mJsonMap;
     //QGridLayout* mainLayout;
     QVBoxLayout* mainLayout;
     QString mJsonFile;
     QWidgetList comList;
     QJsonArray comJsonArr;
     QMap<QString,QVariantMap> comMap;
     QMap<QString,FormResizer*> ProMap; // 新生成的控件.
     //QSizePolicy mSizePolicy;

     void CreateButtonList();
     QWidget *getQWidgetByName(QString name) const;
     void ReadJsonFile();

signals:

public slots:
     void onCreateCompoentToCanvas();
     void onCreateNewLayout();
};

#endif // COMPOENTCONTROLS_H

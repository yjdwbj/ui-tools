#ifndef HANDLEJSON_H
#define HANDLEJSON_H
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileDevice>
#include <QTextStream>
#include <QtCore/QRect>
#include <QMapData>
#include <QMapIterator>
#include <QDebug>
#include <QApplication>
#include <QStackedLayout>
#include <QtWidgets/QGroupBox>
#include "module.h"



class HandleJson{

public:
     HandleJson(QString name);
     void HandleFrameObject(QJsonObject qjo,QString ParentName);
     void ReadJsonFile();
     void HandleJsonMap(QVariantMap qvm);
     QObject* CreateObjectFromJson(QVariantMap qvm, QObject *obj);
     QRect readRect(QVariantMap qvm);
     QWidget *findQWidget(QObjectList objlist , QString widName);
     QWidget *findQWidget(QWidgetList objlist , QString widName);

     QVariantMap mJsonMap;

private:
    QString NAME = "-name";
    QString WIDGET ="widget";
    QString CLASS = "-class";
    QString LAYOUT ="layout";
    QString PROPERTY = "property";
    QString ITEM="item";
    QString RECT ="rect";
    QString QFRAME = "QFrame";
    QString QLABEL = "QLabel";
    typedef struct  ObjComt{
        QString objName;
        QString clsName;
        QVariantMap  property;
        QVariantMap  child;
        QObject *obj;
        QString parentName;
    };
    QList<ObjComt>  ComList;
    QString jsonName ;
    NewFrame *mParentObj;


};






#endif // HANDLEJSON_H

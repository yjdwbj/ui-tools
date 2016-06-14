#ifndef COMPOENTCONTROLS_H
#define COMPOENTCONTROLS_H

#include <QWidget>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QDateTime>
#include "mainwindow.h"

static QString NAME = "-name";
static QString WIDGET ="widget";
static QString CLASS = "-class";
static QString LAYOUT ="layout";
static QString PROPERTY = "property";
static QString ITEM="item";
static QString RECT ="rect";
static QString QFRAME = "QFrame";
static QString QLABEL = "QLabel";
static QString CAPTION = "caption";

static const char* DKEY_CAPTION = "caption";

class CompoentControls : public QWidget
{
    Q_OBJECT
public:
    explicit CompoentControls(QWidget *parent = 0);

private:
     MainWindow *mWindow;
     QVariantMap mJsonMap;
     QObject* CreateObjectFromJson(QVariantMap qvm, QObject *pobj);
     QWidget *getQWidgetByName(QString name) const;
     void ReadJsonFile();
     QString mJsonFile;
     QWidgetList comList;

signals:

public slots:
     void onCreateCompoentToCanvas();
};

#endif // COMPOENTCONTROLS_H

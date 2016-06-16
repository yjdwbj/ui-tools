#ifndef COMPOENTCONTROLS_H
#define COMPOENTCONTROLS_H

#include <QWidget>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QDateTime>
#include <QSignalMapper>
#include "mainwindow.h"
#include "propertybox.h"
#include "treedock.h"


class TreeDock;
class MainWindow;
class CompoentControls : public QWidget
{
    friend class PropertyBox;
    friend class TreeDock;
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
     QJsonArray comJsonArr;
     QMap<QString,QVariantMap> comMap;
     void CreateButtonList();

signals:

public slots:
     void onCreateCompoentToCanvas();

};

#endif // COMPOENTCONTROLS_H

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
class ScenesScreen;


void removeWidFromLayout(QLayout *layout);



class ComProperty: public QGroupBox
{
    Q_OBJECT
public:
    explicit ComProperty(QWidget *parent=0);
    ~ComProperty(){}
  //  QGroupBox *CreateXYWHGBox(QWidget *p);
    void createPropertyBox(QWidget *p);
private:
     QGroupBox *CreateXYWHGBox(QWidget *p);
     QVBoxLayout* mainLayout;

};

class ImgProperty: public QGroupBox
{
    Q_OBJECT
public:
    explicit ImgProperty(QWidget *parent=0);
    ~ImgProperty(){}
    void createPropertyBox(QWidget *p);

private:
    QVBoxLayout* mainLayout;
};


class CompoentControls : public QGroupBox
{
    friend class PropertyBox;
    friend class TreeDock;
    Q_OBJECT
public:
    explicit CompoentControls(MainWindow *mw,QWidget *parent = 0);
    QObject* CreateObjectFromJson(QVariantMap qvm, QObject *pobj);
   // QWidget *CcWidgetList,ProWidgetList,ImgWidgetList;

private:
     MainWindow *mWindow;
     QVariantMap mJsonMap;
     //QGridLayout* mainLayout;
     QHBoxLayout* mainLayout;
     QString mJsonFile;
     QWidgetList comList;
     QJsonArray comJsonArr;
     QMap<QString,QVariantMap> comMap;
     QSizePolicy mSizePolicy;

     void CreateButtonList();
     QWidget *getQWidgetByName(QString name) const;
     void ReadJsonFile();

signals:

public slots:
     void onCreateCompoentToCanvas();
     void onCreateNewLayout();
};

#endif // COMPOENTCONTROLS_H

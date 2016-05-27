#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFrame>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QtCore/QFileDevice>
#include <QTextStream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void getJsonValue(QJsonValue qjv);

    void HandleObject(QJsonObject qjo);
    void HandleArrayObject(QJsonArray array);
    void setWidget(QObject &oob);

private:
    Ui::MainWindow *ui;
    QDockWidget* rDock;
    QDockWidget* lDock;
    QListWidget* rList;
    QListWidget* lList;
    QTextStream out;


    enum KType {
        Null =  0x0,
        Bool = 0x1,
        Double = 0x2,
        String = 0x3,
        Array = 0x4,
        Object = 0x5,
        Undefined = 0x80
    };
    QString NAME = "-name";
    QString WIDGET ="widget";
    QString CLASS = "-class";
    QString LAYOUT ="layout";
    QString PROPERTY = "property";
    QString ITEM="item";
    QString RECT ="rect";
    typedef struct {
        QString objName;
        QString clsName;
        QString pixmap;
        QRect rect;
        QObject *obj;
        QObject *parent;
    }ObjComt;
    QObject *HandleFrameObject(QJsonObject qjo);

};

#endif // MAINWINDOW_H

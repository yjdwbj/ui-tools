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

#include <QDebug>
#include <QLabel>
#include <QDir>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMdiArea>
#include <QtCore/QMargins>
#include <QMouseEvent>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMessageBox>






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
    bool eventFilter(QObject *obj, QEvent *event);



    QGroupBox *propertyWidget;
    QGroupBox *imgPropertyWidget;

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

    void HandleFrameObject(QJsonObject qjo, QString ParentName);
    QWidget *pWin;
    QFrame *mCanvas;



};

#endif // MAINWINDOW_H

#ifndef MODULE_H
#define MODULE_H
#include <QtWidgets/QFrame>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include "mainwindow.h"
#include "imagefiledialog.h"





class NewLabel :public QLabel
{
    Q_OBJECT

signals:
    void Clicked();
public:
    NewLabel(QWidget *parent=0);
    enum widType {
        Number =0x0,
        Text = 0x1,
        List = 0x2,
        Enum = 0x3
    };

    Ui::MainWindow *ui;

protected:
    void mousePressEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QWidget* getQWidgetByName(QString name) const;
    QGroupBox* CreateXYWHGBox(QWidget *p);
    void removeWidFromLayout(QLayout* layout);

private slots:
    void onClieck();
    void onPictureDialog(bool );
    void onXYWHChangedValue(int);
    void onListImageChanged(QString );


private:


    void UpdateXYWHPos();


    QPoint mOffset;
    MainWindow *mWindow;

};

class NewFrame :public QWidget
{
    Q_OBJECT
public:
     QPoint mOffset;
    NewFrame(QWidget *parent=0);
    void mouseMoveEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

signals:
   void Clicked();
private slots:
    void onClick();




protected:
    void mousePressEvent(QMouseEvent *event) ;

   // void mouseDoubleClickEvent(QMouseEvent *event);

};


#endif // MODULE_H

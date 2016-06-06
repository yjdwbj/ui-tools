#ifndef MODULE_H
#define MODULE_H
#include <QtWidgets/QFrame>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QDebug>
#include <QtWidgets/QWidget>
#include <QMouseEvent>

class NewLabel :public QLabel
{
    Q_OBJECT

signals:
    void Clicked();
public:
    NewLabel(QWidget *parent=0);

protected:
    void mousePressEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void onClieck();

private:
    QPoint mOffset;

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

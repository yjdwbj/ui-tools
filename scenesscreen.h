#ifndef SCENESSCREEN_H
#define SCENESSCREEN_H

#include <QObject>
#include <QWidget>
#include <QFrame>

//#include "mainwindow.h"
class MainWindow;
class ScenesScreen : public QFrame
{
    Q_OBJECT
public:
    explicit ScenesScreen(QWidget *parent = 0);

signals:

public slots:

private:
   MainWindow* mWindow;

};

#endif // SCENESSCREEN_H

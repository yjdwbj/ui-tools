#ifndef PROPERTYBOX_H
#define PROPERTYBOX_H

#include <QWidget>
#include <QGroupBox>
#include <QSpinBox>
#include "mainwindow.h"

#include "module.h"
#include <QComboBox>
#include <QTextEdit>





class NewLabel;

class MainWindow;


class PropertyBox : public QGroupBox
{
 //   friend class NewLabel;
    Q_OBJECT
public:


    explicit PropertyBox(QWidget *parent = 0);
    void createPropertyBox(QWidget *p);

signals:

public slots:


private:
    void removeWidFromLayout(QLayout *layout);
    QGroupBox *CreateXYWHGBox(QWidget *p);
    QVBoxLayout *mainLayout;
    MainWindow *mw;
    NewLabel *comT;
};

#endif // PROPERTYBOX_H

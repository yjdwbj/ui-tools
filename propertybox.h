#ifndef PROPERTYBOX_H
#define PROPERTYBOX_H

#include <QWidget>
#include <QGroupBox>
#include <QSpinBox>
#include <QScrollArea>
#include "mainwindow.h"

#include "module.h"
#include <QComboBox>
#include <QLineEdit>





class NewLabel;

class MainWindow;


class PropertyBox : public QScrollArea
{
 //   friend class NewLabel;
    Q_OBJECT
public:


    explicit PropertyBox(QString title,QWidget *parent = 0);
    void createPropertyBox(QWidget *p);
    //void onListImageChanged(QString img);
    //void onPictureDialog(bool b);


signals:

public slots:


private:
    void removeWidFromLayout(QLayout *layout);
    QGroupBox *CreateXYWHGBox(QWidget *p);
    QVBoxLayout *mainLayout;
    MainWindow *mw;
    NewLabel *comT;
    QSizePolicy mSizePolicy;
    QWidget *mainWidget;
};

#endif // PROPERTYBOX_H

#include "propertybox.h"
#include <QApplication>
#include "config.h"

// 控件属性的框

PropertyBox::PropertyBox(QString title, QWidget *parent) : QGroupBox(parent),
    mainLayout(new QVBoxLayout())
{

    // MainWindow *m ;
    QWidgetList tlist = qApp->topLevelWidgets();
    for(QWidgetList::iterator wit = tlist.begin();wit != tlist.end();++wit)
    {
        if((*wit)->objectName() == "MainWindow")
        {
            mw = (MainWindow*)(*wit);
            break;
        }
    }

   // setLayout(mainLayout);
   // setTitle(tr("控件属性"));
    setTitle(title);
    mainLayout->setObjectName(COMGRPLYT);
   // mainLayout->addSpacing(1);
    mainLayout->setContentsMargins(0,50,0,0);
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

}


QGroupBox* PropertyBox::CreateXYWHGBox(QWidget *p)
{
    QGridLayout *xywh = new QGridLayout();
    xywh->setObjectName("xywhGrid");

    QMap<QString,int> xymap;
    xymap[X] = p->geometry().x();
    xymap[Y] = p->geometry().y();
    xymap[W] = p->geometry().width();
    xymap[H] = p->geometry().height();

    int index = 0;


    for(QMap<QString,int>::iterator it = xymap.begin();it != xymap.end();++it)
    {
        QLabel *s = new QLabel(it.key());
        s->setFixedWidth(35);
        xywh->addWidget(s,index,0);
        QSpinBox *xedit = new QSpinBox();
        xedit->setFixedWidth(40);

        xedit->setObjectName(it.key());
        xedit->setMaximum(1000);
        xedit->setValue(it.value());
        xywh->addWidget(xedit,index,1);
        //connect(xedit,SIGNAL(valueChanged(int)),SLOT(onXYWHChangedValue(int)));
       // comT->addPropertyBoxSignal(xedit);
        connect(xedit,SIGNAL(valueChanged(int)),p,SLOT(onXYWHChangedValue(int)));

        index++;
    }

    QGroupBox *xygb = new QGroupBox(tr("坐标位置"));
    xygb->setSizePolicy(mSizePolicy);
    //xywh->setSizeConstraint(QLayout::SetFixedSize);
    xywh->setContentsMargins(0,15,0,0);

    xygb->setObjectName("xygb");
    xygb->setLayout(xywh);
    return xygb;
}

void PropertyBox::createPropertyBox(QWidget *p, bool isImage)
{

    // 删除之前的重新画一个新的.
  //  qDebug()  << " object name " << mainLayout->objectName()
    //          << "object uid  " << p->property(DKEY_UID).toString();

    if(!mainLayout->property(DKEY_UID).toString().compare(p->property(DKEY_UID).toString()))
    {
        //qDebug() << " press at same return ";
        return;
    }

    removeWidFromLayout(mainLayout);
    delete mainLayout;
    mainLayout = new QVBoxLayout();

    mainLayout->setProperty(DKEY_UID,p->property(DKEY_UID).toString());
    setLayout( mainLayout);
    mainLayout->setObjectName(COMGRPLYT);
    mainLayout->addSpacing(1);
    setTitle(p->objectName());

    if(!isImage)
    {
        mainLayout->addWidget(CreateXYWHGBox(p));
    }



    QVariantList qvl = p->property(DKEY_DYN).toList();
    foreach(QVariant qv, qvl)
    {

        if(qv.type() == QVariant::Map)
        {
            QVariantMap qvm = qv.toMap();
            QString uname =  qvm[NAME].toString();
            if(qvm.contains(ENUM))
            {
                QComboBox *cb = new QComboBox();
                QVariantList qvlist = qvm[ENUM].toList();
                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {

                    cb->addItem((*it).toMap().firstKey());
                }
                mainLayout->addWidget(new QLabel(uname));
                mainLayout->addWidget(cb);

            }else if(qvm.contains(LIST))
            {

                //qDebug() << " class name " << p->metaObject()->className();
                QComboBox *cb = new QComboBox();
                cb->setObjectName(LISTIMAGE);  // 这里假设一个NewLabel只有这样一个QComoBox
                p->setProperty(DKEY_IMGIDX,0); // 当前选择的行号
                //QString uname =  qvm["-name"].toString();
                QString className = p->metaObject()->className();
                if(!className.compare("NewLabel"))
                {
                    NewLabel *nl = (NewLabel*)p;
                    if(!nl->disDefaultList)
                    {
                        QVariantList qvlist = qvm[LIST].toList();
                        for(QVariantList::const_iterator it = qvlist.begin();
                            it != qvlist.end();++it)
                        {
                            cb->addItem((*it).toString());//添加默认的列表到Combobox

                        }
                    }else{
                        nl->updateComboItems(cb);
                    }
                }

                mainLayout->addWidget(new QLabel(uname));
                //v->addLayout(h);
                QPushButton *b = new QPushButton(tr("添加图片"));
                connect(b,SIGNAL(clicked(bool)),p,SLOT(onPictureDialog(bool)));
                mainLayout->addWidget(b);
                mainLayout->addWidget(cb);
                // 绑定QComoBox的更改信号,更改它的值就要在相应的画版控件更新图片
                connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onListImageChanged(QString)));

            }else if(qvm.contains(IMAGE)) /* 跳过这一行.*/
            {

            }
            else{
                if(uname.compare(GEOMETRY))
                {

                    if(qvm.contains("id"))
                    {
                        QLabel *title = new QLabel(uname);
                        title->setFixedHeight(labHeight);
                        mainLayout->addWidget(title);
                        QLabel *id = new QLabel(p->property(DKEY_UID).toString());
                        id->setStyleSheet("*{border: 0.5px solid gray;}");
                        id->setFixedHeight(labHeight);
                        mainLayout->addWidget(id);
                         // 这里是一个特殊属性,唯一序号
                    }

                    else if(qvm[DEFAULT].type() == QVariant::Double)
                    {
                        // QTextEdit *id = new QTextEdit(t.toLocalTime().toString());
                        // id->setEnabled(false);
                        mainLayout->addWidget(new QLabel(uname));
                        QSpinBox *s = new QSpinBox();
                        mainLayout->addWidget(s);
                        s->setValue(qvm[DEFAULT].toInt());

                        if(qvm.contains(MAX))
                        {
                            s->setMaximum(qvm[MAX].toInt());
                        }
                        if(qvm.contains(MIN))
                        {
                            s->setMinimum(qvm[MIN].toInt());
                        }
                    }
                    else{
                        QTextEdit *txt = new QTextEdit(qvm[DEFAULT].toString());
                        mainLayout->addWidget(new QLabel(uname));
                        mainLayout->addWidget(txt);
                        txt->setFixedHeight(25);
                        //  v->addSpacerItem(new QSpacerItem(10,50));

                    }
                }
            }


        }

    }

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(verticalSpacer);
    mainLayout->setContentsMargins(0,20,0,0);


}

void PropertyBox::removeWidFromLayout(QLayout *layout)
{
    QLayoutItem* child;
    while(layout->count()!=0)
    {
        child = layout->takeAt(0);
        if(child->layout() != 0)
        {
            removeWidFromLayout(child->layout());
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }
}


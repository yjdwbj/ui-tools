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
    QSize psize = p->parentWidget()->size();

    int index = 0;
    for(QMap<QString,int>::iterator it = xymap.begin();it != xymap.end();++it)
    {
        QLabel *s = new QLabel(it.key());
        s->setFixedWidth(35);
        xywh->addWidget(s,index,0);
        QSpinBox *xedit = new QSpinBox();

        if( (it.key() == H || it.key() == W))
        {
            xedit->setEnabled(!CN_NEWLAYOUT.compare(p->metaObject()->className()) ); // 非NewLayout控件大小不能调整
        }
        xedit->setFixedWidth(40);
        if(it.key() == H)
            xedit->setMaximum(psize.height());

        if(it.key() == W)
            xedit->setMaximum(psize.width());

        xedit->setObjectName(it.key());
        //  检查它的值不能超上层控件边界.
      //  xedit->setMaximum(1000);
        xedit->setValue(it.value());
        xywh->addWidget(xedit,index,1);
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

void PropertyBox::createPropertyBox(QWidget *p)
{

    // 删除之前的重新画一个新的.

    QString className = p->metaObject()->className();
    QString nkeyuid ;
    if(!CN_NEWLAYOUT.compare(className))
    {
        nkeyuid = p->objectName();
        if(!mainLayout->property(DKEY_UID).toString().compare(nkeyuid))
        {
            return;
        }

    }else if(!CN_NEWFRAME.compare(className))
    {
        nkeyuid = p->property(DKEY_UID).toString();
        if(!mainLayout->property(DKEY_UID).toString().compare(nkeyuid))
        {
            return;
        }

    }else if(!CN_NEWLABEL.compare(className))
    {

    }
    removeWidFromLayout(mainLayout);
    delete mainLayout;
    mainLayout = new QVBoxLayout();
    mainLayout->setProperty(DKEY_UID,nkeyuid);


    setLayout( mainLayout);
    mainLayout->setObjectName(COMGRPLYT);
    mainLayout->addSpacing(1);
  // setTitle(p->objectName());

    if(CN_NEWLABEL.compare(className))
    {
        //只要不是图片元素都是要重画坐标属性的.
        mainLayout->addWidget(CreateXYWHGBox(p));
    }

    QVariantList qvl = p->property(DKEY_DYN).toList();
    foreach(QVariant qv, qvl)
    {

        NewFrame *nf = (NewFrame*)p;
        if(qv.type() == QVariant::Map)
        {
            QVariantMap qvm = qv.toMap();
            QString uname =  qvm[CAPTION].toString();
            if(qvm.contains(ENUM))
            {
                QComboBox *cb = new QComboBox();
                cb->setObjectName(uname);
                cb->setProperty(DKEY_VALTYPE,ENUM);


                QVariantList qvlist = qvm[ENUM].toList();

                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {

                    cb->addItem((*it).toMap().firstKey());
                }
                mainLayout->addWidget(new QLabel(uname));
                mainLayout->addWidget(cb);
                QString fk ="";
                if(qvlist.size())
                {
                    fk = qvlist.at(0).toString();
                }
                if(!className.compare(CN_NEWFRAME))
                {
                    ((NewFrame*)p)->onBindValue(cb,fk);
                }else{
                    ((NewLabel*)p)->onBindValue(cb,fk);
                }
                connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onEnumItemChanged(QString)));

            }else if(qvm.contains(LIST))
            {

                //qDebug() << " class name " << p->metaObject()->className();
                QComboBox *cb = new QComboBox();
                cb->setObjectName(uname);
                cb->setProperty(DKEY_VALTYPE,LIST);
                cb->setObjectName(LISTIMAGE);  // 这里假设一个NewLabel只有这样一个QComoBox
                p->setProperty(DKEY_IMGIDX,0); // 当前选择的行号
                //QString uname =  qvm["-name"].toString();
                QString className = p->metaObject()->className();
                QString fk = "";
                if(!className.compare(CN_NEWLABEL))
                {
                    NewLabel *nl = (NewLabel*)p;
                    if(!nl->disDefaultList)
                    {
                        QVariantList qvlist = qvm[LIST].toList();
                        if(qvlist.size())
                        {
                            fk = qvlist.at(0).toString();
                        }
                        for(QVariantList::const_iterator it = qvlist.begin();
                            it != qvlist.end();++it)
                        {

                            cb->addItem((*it).toString());//添加默认的列表到Combobox
                        }
                    }else{
                        nl->updateComboItems(cb);
                    }
                }

                 if(!className.compare(CN_NEWFRAME))
                 {
                     ((NewFrame*)p)->onBindValue(cb,fk);
                 }else{
                     ((NewLabel*)p)->onBindValue(cb,fk);
                 }

                mainLayout->addWidget(new QLabel(uname));
                //v->addLayout(h);
                QPushButton *b = new QPushButton(tr("添加图片"));
                connect(b,SIGNAL(clicked(bool)),p,SLOT(onPictureDialog(bool)));
                mainLayout->addWidget(b);
                mainLayout->addWidget(cb);
                // 绑定QComoBox的更改信号,更改它的值就要在相应的画版控件更新图片
                connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onListImageChanged(QString)));

            }else if(qvm.contains(IMAGE) ) /* 跳过这一行.*/
            {

            }
            else{
                if(uname.compare(GEOMETRY))
                {

                    QVariant::Type t = qvm[DEFAULT].type();
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

                    else if(t == QVariant::Double)
                    {
                        // QTextEdit *id = new QTextEdit(t.toLocalTime().toString());
                        // id->setEnabled(false);
                        mainLayout->addWidget(new QLabel(uname));
                        QSpinBox *s = new QSpinBox();

                        s->setObjectName(uname);
                        s->setProperty(DKEY_VALTYPE,NUMBER);
                        //要保存每一次修改过的值.
                        mainLayout->addWidget(s);
                        //s->setValue(qvm[DEFAULT].toInt());
                      //  ((Compoent*)p)->onBindValue(s,qvm[DEFAULT].toInt());
                        if(!className.compare(CN_NEWFRAME))
                        {
                            ((NewFrame*)p)->onBindValue(s,qvm[DEFAULT].toInt());
                        }else{
                            ((NewLabel*)p)->onBindValue(s,qvm[DEFAULT].toInt());
                        }

                        if(qvm.contains(MAX))
                        {
                            s->setMaximum(qvm[MAX].toInt());
                        }
                        if(qvm.contains(MIN))
                        {
                            s->setMinimum(qvm[MIN].toInt());
                        }
                        connect(s,SIGNAL(valueChanged(int)),p,SLOT(onNumberChanged(int)));
                    }
                    else if(t == QVariant::String ){
                        QTextEdit *txt = new QTextEdit(qvm[DEFAULT].toString());
                        txt->setObjectName(uname);
                        txt->setProperty(DKEY_VALTYPE,TEXT);


                        if(!className.compare(CN_NEWFRAME))
                        {
                            ((NewFrame*)p)->onBindValue(txt,qvm[DEFAULT].toString());
                        }else{
                            ((NewLabel*)p)->onBindValue(txt,qvm[DEFAULT].toString());
                        }
                        mainLayout->addWidget(new QLabel(uname));
                        mainLayout->addWidget(txt);
                        txt->setFixedHeight(25);
                         connect(txt,SIGNAL(textChanged()),p,SLOT(onTextChanged()));
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


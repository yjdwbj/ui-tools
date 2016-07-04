#include "compoentcontrols.h"
//#include "handlejson.h"
#include "config.h"
#include "scenesscreen.h"
#include "canvasmanager.h"
#include "module.h"
#include <QHash>
#include <QComboBox>
#include <QApplication>

//static int pwidth = 20;


Position::Position( QWidget *parent)
    :QGroupBox(parent),
      Xpos(new QSpinBox()),
      Ypos(new QSpinBox()),
      Wpos(new QSpinBox()),
      Hpos(new QSpinBox()),
      old(0)
{
    setTitle(tr("坐标位置"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");
   // setFixedHeight((parent->width()-50)*0.1);
    QGridLayout *xywh = new QGridLayout();
    xywh->setSpacing(2);
    setLayout(xywh);
    xywh->addWidget(new QLabel(X),0,0);
    xywh->addWidget(Xpos,0,1);


    xywh->addWidget(new QLabel(Y),1,0);
    xywh->addWidget(Ypos,1,1);

    xywh->addWidget(new QLabel(W),2,0);
    xywh->addWidget(Wpos,2,1);

    xywh->addWidget(new QLabel(H),3,0);
    xywh->addWidget(Hpos,3,1);
//    QSpacerItem *verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    xywh->addItem(verticalSpacer,4,0,0,1);

    Xpos->setObjectName(X);
    Ypos->setObjectName(Y);
    Hpos->setObjectName(H);
    Wpos->setObjectName(W);

    xywh->setContentsMargins(0,15,0,0);
}

void Position::setConnectNewQWidget(QWidget *com)
{
    setEnabled(true);
    if(old == com)
        return;

    foreach(auto var , connections)
    {
        QObject::disconnect(var);
    }

    connections.clear();

    Xpos->disconnect();
    Ypos->disconnect();
    Wpos->disconnect();
    Hpos->disconnect();


    Xpos->setValue(com->geometry().x());
    Ypos->setValue(com->geometry().y());
    Wpos->setValue(com->geometry().width());
    Hpos->setValue(com->geometry().height());

    // 非NewLayout控件大小不能调整
    bool isLayout = !CN_NEWLAYOUT.compare(com->metaObject()->className());
    Wpos->setEnabled(isLayout);
    Hpos->setEnabled(isLayout);

    QSize psize = com->parentWidget()->size();
    Hpos->setMaximum(psize.height());
    Wpos->setMaximum(psize.width());
    connections << QObject::connect(Xpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
    connections << QObject::connect(Ypos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
    connections << QObject::connect(Wpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
    connections << QObject::connect(Hpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));

    old = com;
}

void Position::updatePosition(QPoint pos)
{
    Xpos->setValue(pos.x());
    Ypos->setValue(pos.y());
}

void Position::updateSize(QSize size)
{
    Wpos->setValue(size.width());
    Hpos->setValue(size.height());
}

void Position::resetValues()
{
    foreach(auto var , connections)
    {
        QObject::disconnect(var);
    }

    connections.clear();

    Xpos->setValue(0);
    Ypos->setValue(0);
    Wpos->setValue(0);
    Hpos->setValue(0);
    this->setEnabled(false);

}

Position::~Position()
{
//    QLayoutItem* child;
//    QLayout *layout = this->layout();
//    while(layout->count()!=0)
//    {
//        child = layout->takeAt(0);
//        if(child->layout() != 0)
//        {
//            removeWidFromLayout(child->layout());
//        }
//        else if(child->widget() != 0)
//        {
//            delete child->widget();
//        }

//        delete child;
//    }
}

ComProperty::ComProperty(QString title,QWidget *parent):
    QGroupBox(parent),
    mainLayout(new QVBoxLayout()),
    oldobject(0)

{
    setTitle(title);
    setStyleSheet("QGroupBox,QLabel,QScrollArea,QWidget{background-color: #C0DCC0;}");
    this->setFixedHeight((parent->height()-50) * 0.3);
    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    QHBoxLayout *hb = new QHBoxLayout(this);
    hb->setSpacing(0);
    this->setLayout(hb);
    mainWidget = new QWidget();

    QScrollArea *scroll =  new QScrollArea();
    scroll->setParent(this);
    hb->addWidget(scroll);
    scroll->setWidget(mainWidget);
    mainWidget->setLayout(mainLayout);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   // setTitle(tr("控件属性"));


}



void ComProperty::delPropertyBox()
{
    removeWidFromLayout(mainLayout);
    delete mainLayout;
    mainLayout = 0;
    oldobject = 0;
}

void ComProperty::createPropertyBox(QWidget *p)
{

    if(oldobject==p)
        return;

    // 删除之前的重新画一个新的.
    QString className = p->metaObject()->className();
   // QString nkeyuid ;
    if(mainLayout)
    {
        removeWidFromLayout(mainLayout);
        delete mainLayout;
        mainLayout = 0;
    }


    // mainWidget->layout()->destroyed();

    // QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout = new QVBoxLayout(mainWidget);
   // mainLayout->setProperty(DKEY_UID,nkeyuid);

    mainWidget->setLayout(mainLayout);

    mainLayout->setObjectName(COMGRPLYT);
    mainLayout->addSpacing(0);

    QVariantList qvl = p->property(DKEY_DYN).toList();


    foreach(QVariant qv, qvl)
    {

        QWidget *wid = 0;
        QVariant val;
        if(qv.type() == QVariant::Map)
        {
            QVariantMap qvm = qv.toMap();
            QString uname =  qvm[NAME].toString();
            QString caption =  qvm[CAPTION].toString();
            if(qvm.contains(ENUM))
            {
                QComboBox *cb = new QComboBox();
                cb->setObjectName(uname);
                cb->setProperty(DKEY_CAPTION,caption);
                cb->setProperty(DKEY_VALTYPE,ENUM);
                QVariantList qvlist = qvm[ENUM].toList();
                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {
                    cb->addItem((*it).toMap().firstKey());
                }
                QLabel * l = new QLabel(uname);
                mainLayout->addWidget(l);
                mainLayout->addWidget(cb);
                wid = cb;
                connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onEnumItemChanged(QString)));

            }else if(qvm.contains(LIST))
            {
                QComboBox *cb = new QComboBox();
                cb->setObjectName(uname);
                cb->setProperty(DKEY_VALTYPE,LIST);
                // 这里通过它的JSON组数的位置去找它.
              //  cb->setObjectName(LISTIMAGE);
                //p->setProperty(DKEY_IMGIDX,0); // 当前选择的行号
                QString cbkey =QString("%1_%2").arg(uname,QString::number(widgetMap.size()));
                widgetMap[cbkey] = cb;
              //  QString className = p->metaObject()->className();
                wid = cb;
                QLabel * l = new QLabel(uname);
                mainLayout->addWidget(l);
                QPushButton *b = new QPushButton(tr("添加图片"));
                b->setObjectName(cbkey);
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
                    if(qvm.contains(UID))
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

                        mainLayout->addWidget(new QLabel(uname));
                        QSpinBox *s = new QSpinBox();

                        s->setObjectName(uname);

                        s->setProperty(DKEY_VALTYPE,NUMBER);
                        //要保存每一次修改过的值.
                        mainLayout->addWidget(s);
                        wid = s;

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
                        QLineEdit *txt = new QLineEdit(qvm[DEFAULT].toString());
                        txt->setObjectName(uname);
                        txt->setProperty(DKEY_VALTYPE,TEXT);

                        if(qvm.contains(MAXLEN))
                        {
                            txt->setMaxLength(qvm[MAXLEN].toDouble());
                        }

                        wid = txt;
                        QLabel * l = new QLabel(uname);
                        mainLayout->addWidget(l);
                        mainLayout->addWidget(txt);
                        txt->setFixedHeight(25);
                        connect(txt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));

                    }
                }
            }

            if(!wid)
                continue;
            if(!className.compare(CN_NEWFRAME))
            {
                ((NewFrame*)p)->onBindValue(wid,qvm);
            }else{
                ((NewLabel*)p)->onBindValue(wid,qvm);
            }

        }

    }
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    oldobject = p;

}

void ComProperty::updateImageComboBox(QString key,int index , const QStringList &list)
{
   QComboBox *cb = (QComboBox*)(widgetMap.value(key,0));
   if(!cb)
       return;

   cb->clear();
 //  cb->addItems(list);
   foreach (QString s, list) {
      cb->addItem(s.section(":",0,0));
   }
   cb->setCurrentIndex(index);
}

void removeWidFromLayout(QLayout *layout)
{
    if(!layout)
        return;
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


CompoentControls::CompoentControls(MainWindow *mw, QWidget *parent)
    : QGroupBox(parent),
      mainLayout(new QVBoxLayout()),
      mWindow(mw)
{

    // MainWindow *m ;
  //  this->setFixedHeight((parent->height()-50) * 0.15);
    setStyleSheet("{border: 1px solid gray;}");

    setTitle(tr("控件列表"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");
    this->setLayout(mainLayout);

//    QHBoxLayout *hb = new QHBoxLayout(this);
//    hb->setSpacing(0);
//    this->setLayout(hb);
//    QWidget *mainWidget = new QWidget();


//    QScrollArea *scroll = new QScrollArea();
//    hb->addWidget(scroll);
//    scroll->setWidget(mainWidget);
//    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    //QGridLayout *mainLayout = new QGridLayout();
   // mainWidget->setLayout(mainLayout);
   // mainLayout->setContentsMargins(0,50,0,0);
    mainLayout->setSpacing(1);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    this->setFixedHeight(200);

   // mSizePolicy.setVerticalPolicy(QSizePolicy::Preferred);



   // mJsonFile =  QDir::currentPath() + "/menu_strip.json";
    mJsonFile = QDir::currentPath() + "/control.json";
    qDebug() << " json file name " << mJsonFile;
    QFileInfo qfi(mJsonFile);
    if(!qfi.exists())
    {
        QMessageBox::warning(this,tr("错误"),tr("找不到控件文件"));
        return;
    }


    ReadJsonFile();
    CreateButtonList();

}

QWidget *CompoentControls::getQWidgetByName(QString name) const
{
    QWidgetList tlist = qApp->topLevelWidgets();

    for(QWidgetList::iterator wit = tlist.begin();wit != tlist.end();++wit)
    {
        if((*wit)->objectName() == name)
        {
            return *wit;
            break;
        }
    }
    return (QWidget*)0;
}

void CompoentControls::ReadJsonFile()
{

    QFile data(mJsonFile);
    if (data.open(QFile::ReadOnly|QIODevice::Text)) {
        QByteArray qba = data.readAll();
        QTextStream in(&data);
        QString str;
        int ans = 0;
        in >> str >> ans;
        QJsonParseError json_error;
        QJsonDocument qd = QJsonDocument::fromJson(qba,&json_error);
        if(json_error.error == QJsonParseError::NoError)
        {

            QPoint mpos;
            if(qd.isObject())
            {
                if(qd.object().contains("compoents"))
                {
                    comJsonArr =  qd.object()["compoents"].toArray();
                }

            }
        }else{
            // qDebug() << " read Json file error";
            qDebug() << json_error.errorString();
        }


    }

}

void CompoentControls::CreateButtonList()
{
    int row,col = 0;

    //    this->setStyleSheet("QPushButton:hover:!pressed\
    //    {\
    //                            border: 1px solid red;\
    //                            radious: 1px;\
    //                            background-color: #729FCF;\
    //                        }\
    //                        QPushButton:open {  \
    //                            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
    //                            stop: 0 #dadbde, stop: 1 #f6f7fa);\
    //                        }\
    //                        QPushButton::menu-indicator {\
    //                            image: url(menu_indicator.png);\
    //                            subcontrol-origin: padding;\
    //                            subcontrol-position: bottom right;\
    //                        }\
    //                        QPushButton::menu-indicator:pressed, QPushButton::menu-indicator:open {\
    //                            position: relative;\
    //                            top: 2px; left: 2px; \
    //                        }"\
    //                        );




    QGridLayout *comLayout = new QGridLayout();

    // mainLayout->addWidget(new QPushButton());
    QVBoxLayout *v = new QVBoxLayout();
    mainLayout->addLayout(v);
    mainLayout->addLayout(comLayout);
    comLayout->setSizeConstraint(QLayout::SetFixedSize);

    comLayout->setVerticalSpacing(1);
    comLayout->setHorizontalSpacing(1);

    QPushButton *l = new QPushButton(tr("布局"));

    //l->setSizePolicy(mSizePolicy);
    connect(l,SIGNAL(clicked(bool)),SLOT(onCreateNewLayout()));
    v->addWidget(l);

    foreach (QJsonValue qjv, comJsonArr)
    {
        QVariantMap  qjm = qjv.toObject().toVariantMap();
        QString caption = qjv.toObject()[CAPTION].toString();
        QString objname = qjv.toObject()[NAME].toString();
        //  qDebug() << " json key is " << uname;
        comMap[caption] = qjm;
        QPushButton *btnTest = new QPushButton(caption);
        btnTest->setProperty(DKEY_CATEGORY,objname);

       // btnTest->setSizePolicy(mSizePolicy);
        if(qjm.contains(ICON))
            btnTest->setIcon(QIcon(qjv.toObject()[ICON].toString()));

       // btnTest->setFixedSize(40,40);
        if(col == 2)
        {
            col = 0;
            row++;
        }
        comLayout->addWidget(btnTest,row,col++);
        connect(btnTest,SIGNAL(clicked(bool)),this,SLOT(onCreateCompoentToCanvas()));

    }
    //  comLayout->setRowStretch(row,0);
//    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    comLayout->addItem(verticalSpacer,++row,0,1,1);
}

void CompoentControls::onCreateCompoentToCanvas()
{
    // QObject *sender = QObject::sender(); /* 确定的那一个按钮被点击了 */

    NewLayout *activeLayout = mWindow->cManager->activeSS()->activeLayout();
    if(!activeLayout)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }

    QPushButton *btn = (QPushButton*)(QObject::sender());
//    NewFrame* ww = (NewFrame *)CreateObjectFromJson(comMap[btn->text()],activeLayout->m_frame);
//    //mWindow->Scenes->activeLayer()->m_frame);
//    ww->setObjectName(btn->property(DKEY_CATEGORY).toString());
//    //ww->setObjectName(QString("%1_%2").arg(btn->text(),QString::number(comList.size())));
//    ww->setProperty(DKEY_LOCALSEQ,QString("%1_%2").arg(btn->text(),QString::number(comList.size())));
//    activeLayout->appendChildObj(ww);
//    // 做treeWidget区分的名字.
//    ProMap[ww->property(DKEY_LOCALSEQ).toString()] = ww;
//    comList.append(ww);
//    ww->onSelectMe();
//    // 找出图层,把新建的控件添加进去.
//    mWindow->tree->addObjectToLayout(ww);
//    ww->show();
   NewFrame* ww =  ReadObjectFromJson(comMap[btn->text()],activeLayout->m_frame,btn->text(),
            btn->property(DKEY_CATEGORY).toString());
    uint number = qHash(QDateTime::currentDateTime().toMSecsSinceEpoch());
    ww->setProperty(DKEY_UID,QString::number(number));
    ww->onSelectMe();
}

NewFrame* CompoentControls::ReadObjectFromJson(QVariantMap qvm, QObject *pobj,QString txt,QString objname)
{
    NewLayout *activeLayout = mWindow->cManager->activeSS()->activeLayout();
    NewFrame* ww = (NewFrame *)CreateObjectFromJson(qvm,pobj);
    //mWindow->Scenes->activeLayer()->m_frame);
    ww->setObjectName(objname);
    //ww->setObjectName(QString("%1_%2").arg(btn->text(),QString::number(comList.size())));
    ww->setProperty(DKEY_LOCALSEQ,QString("%1_%2").arg(txt,QString::number(comList.size())));
    activeLayout->appendChildObj(ww);
    // 做treeWidget区分的名字.
    ProMap[ww->property(DKEY_LOCALSEQ).toString()] = ww;
    comList.append(ww);

    // 找出图层,把新建的控件添加进去.
    mWindow->tree->addObjectToLayout(ww);
    ww->show();
    return ww;
}


QObject* CompoentControls::CreateObjectFromJson(QVariantMap qvm, QObject *pobj)
{
    QObject *nobj;
    QVariant property;
    for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
    {
        QString key = it.key();
        // qDebug() << "json key is " << key;
        QVariant::Type qvt = it.value().type();


        switch (qvt) {
        case QVariant::String:
            if(!key.compare(CLASS))
            {
                QString cval = it.value().toString();

                if(!cval.compare(QFRAME) || !cval.compare(CN_NEWFRAME))
                {

                    //创建父控件
                    NewFrame  *n = new NewFrame((QWidget*)pobj);
                    //  n->addMainWindow(pobj->parent());
                    if(qvm.contains(PROPERTY))
                    {
                        // QJsonObject vp =QJsonObject::fromVariantMap(qvm[PROPERTY]);
                        n->copyProperty(qvm[PROPERTY]);
                        //   qDebug() << " NewFrame dyn property " << n->dynValues;
                    }
                    nobj =qobject_cast<QObject*>(n);
                  //  nobj->setProperty(DKEY_CLSNAME,cval);
                }
                else if(!cval.compare(QLABEL) || !cval.compare(CN_NEWLABEL))
                {
                    NewFrame *np = (NewFrame*)pobj;
                    nobj =qobject_cast<QObject*>(new NewLabel(np->m_frame));
                    if(qvm.contains(PROPERTY))
                    {

                        ((NewLabel*)nobj)->copyProperty(qvm[PROPERTY]);
                        // qDebug() << " NewLabel dyn property " << ((NewLabel*)nobj)->dynValues;
                    }

                }
            }
            else if(!key.compare(NAME))
            {
                nobj->setObjectName(it.value().toString());
            }
            else if(!key.compare(CAPTION)) /* 界面显示的名称 */
            {
                nobj->setProperty(DKEY_CAPTION,it.value().toString());
            }
            break;
        case QVariant::List:
        {
            if(!key.compare(PROPERTY))
            {
                // nobj->setProperty("dynProperty",it.value());
                property = it.value();
            }else {
                QVariantList qvl = it.value().toList();
               // QStringList chlist;
                foreach(QVariant qv, qvl)
                {
                    // qDebug() << qv.type();
                    if(qv.type() == QVariant::Map)
                    {
                        QObject *cobj = CreateObjectFromJson(qv.toMap(),nobj);
                       // chlist.append(cobj->objectName());
                    }

                }

                //pobj->setProperty("chlist",chlist);

            }
        }
            break;
        case QVariant::Map:
            //  CreateObjectFromJson(it.value().toMap());
            // qDebug() << " type is Map " << it.value().toMap();
            break;
        case QVariant::Double:
            //qDebug()  << " Value is Int : " << it.value().toInt();
            break;
        default:
            break;
        }
    }
    nobj->setProperty(DKEY_DYN,property);
   // nobj->setProperty(DKEY_UID,QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));

    /*  处理每一个json对像的property部分 */
    // qDebug() << "Dynamic Property Count " << nobj->dynamicPropertyNames().count();
    foreach(QByteArray qba,nobj->dynamicPropertyNames())
    {
        QVariantList qvl = nobj->property(qba).toList();
        foreach(QVariant qv, qvl)
        {
            // qDebug()  << " Property Val List :" << qv.type();
            if(qv.type() == QVariant::Map)
            {
                QVariantMap qvm = qv.toMap();

                for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
                {
                    QString key = it.key();
                    if(!key.compare(KEY_RECT)) /* 这里直接处理json "rect" 对像字段 */
                    {
                      //  QString clsName = nobj->property(DKEY_CLSNAME).toString();
                        QVariantMap rect = it.value().toMap();
                        QRect r = QRect(rect["x"].toString().toInt(),
                                rect["y"].toString().toInt(),
                                rect["width"].toString().toInt(),
                                rect["height"].toString().toInt());

                        //if(!clsName.compare(QFRAME))
                        if(!CN_NEWFRAME.compare(nobj->metaObject()->className()))
                        {

                            qobject_cast<NewFrame *>(nobj)->setGeometry(r);
                            ((NewFrame*)nobj)->m_frame->setGeometry(r);
                            ((NewFrame*)nobj)->updateGeometry();

                            // 外面８个点的实际位置.
                            ((NewFrame*)nobj)->setFixedSize(r.size()+MARGIN_SIZE);

                        }else{
                            qobject_cast<NewLabel *>(nobj)->setGeometry(r);
                            //  qDebug() << " label geometry " << qobject_cast<NewLabel *>(nobj)->geometry();
                        }
                    }
                    else if(!key.compare(IMAGE)) // 这里中处理了这一个图片属性.
                    {
                        QPixmap p;
                        QString path = it.value().toString();
                        if(path.contains("\\"))
                        {
                            path.replace("\\","/");
                        }
                        p.load(path);
                        qobject_cast<NewLabel *>(nobj)->setPixmap(p);
                        qobject_cast<NewLabel *>(nobj)->setFixedSize(p.size());
                    }
                    else {

                        //  qDebug() << "other property Key : " << key  << " Val : " << it.value();
                    }

                }
            }
        }

    }

    return nobj;
}


void CompoentControls::onCreateNewLayout()
{

    ScenesScreen *ss = mWindow->cManager->activeSS();
    if(ss)
    {
        ss->createNewLayout();
        QPushButton *btn = (QPushButton*)(QObject::sender());
        NewLayout *nl =  ss->activeLayout();
        ProMap[nl->property(DKEY_LOCALSEQ).toString()] = nl;
        //comList.append(nl);
        mWindow->tree->addItemToRoot(nl->property(DKEY_LOCALSEQ).toString(),btn->text());
    }
}

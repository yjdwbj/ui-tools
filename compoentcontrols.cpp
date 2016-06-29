#include "compoentcontrols.h"
#include "handlejson.h"
#include "config.h"
#include "scenesscreen.h"
#include "canvasmanager.h"


ComProperty::ComProperty(QWidget *parent):
    QGroupBox(parent),
    mainLayout(new QVBoxLayout())
{
   QScrollArea *scroll = new   QScrollArea();
   scroll->setWidget(this);
   scroll->setWidgetResizable(true);
    scroll->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
   scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // setTitle(tr("控件属性"));

}

QGroupBox* ComProperty::CreateXYWHGBox(QWidget *p)
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
   // xygb->setSizePolicy(mSizePolicy);
    //xywh->setSizeConstraint(QLayout::SetFixedSize);
   // xywh->setContentsMargins(0,15,0,0);

    xygb->setObjectName("xygb");
    xygb->setLayout(xywh);
    return xygb;
}


void ComProperty::createPropertyBox(QWidget *p)
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

//    foreach (QWidget *w, mainWidget->findChildren<QWidget*>()) {
//       delete w;
//    }
//    mainWidget->layout()->deleteLater();
    removeWidFromLayout(mainLayout);
    delete mainLayout;

   // QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout = new QVBoxLayout();
    mainLayout->setProperty(DKEY_UID,nkeyuid);


    setLayout(mainLayout);
    qDebug() << " create propertBox " << this->geometry();

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
                        QLineEdit *txt = new QLineEdit(qvm[DEFAULT].toString());
                        txt->setObjectName(uname);
                        txt->setProperty(DKEY_VALTYPE,TEXT);
                        if(qvm.contains(MAXLEN))
                        {
                            txt->setMaxLength(qvm[MAXLEN].toDouble());
                        }


                        if(!className.compare(CN_NEWFRAME))
                        {
                            ((NewFrame*)p)->onBindValue(txt,qvm[DEFAULT].toString());
                        }else{
                            ((NewLabel*)p)->onBindValue(txt,qvm[DEFAULT].toString());
                        }
                        mainLayout->addWidget(new QLabel(uname));
                        mainLayout->addWidget(txt);
                        txt->setFixedHeight(25);
                         connect(txt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));
                        //  v->addSpacerItem(new QSpacerItem(10,50));

                    }
                }
            }

        }

    }

//    QSpacerItem *verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    mainLayout->addItem(verticalSpacer);
//    mainLayout->setContentsMargins(0,20,0,0);


}

void removeWidFromLayout(QLayout *layout)
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


ImgProperty::ImgProperty(QWidget *parent)
    :QGroupBox(parent),
      mainLayout(new QVBoxLayout())
{
    QScrollArea *scroll = new   QScrollArea();
    scroll->setWidget(this);
    scroll->setWidgetResizable(true);
    scroll->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //  setTitle(tr("图片属性"));
}

void ImgProperty::createPropertyBox(QWidget *p)
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

//    foreach (QWidget *w, mainWidget->findChildren<QWidget*>()) {
//       delete w;
//    }
//    mainWidget->layout()->deleteLater();
    removeWidFromLayout(mainLayout);
    delete mainLayout;

   // QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout = new QVBoxLayout();
    mainLayout->setProperty(DKEY_UID,nkeyuid);


    setLayout(mainLayout);
    qDebug() << " create propertBox " << this->geometry();

    mainLayout->setObjectName(COMGRPLYT);
    mainLayout->addSpacing(1);
  // setTitle(p->objectName());



    QVariantList qvl = p->property(DKEY_DYN).toList();
    foreach(QVariant qv, qvl)
    {

        NewFrame *nf = (NewFrame*)p;
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
                cb->setFixedWidth(100);
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

                        mainLayout->addWidget(new QLabel(uname));
                        QSpinBox *s = new QSpinBox();
                        s->setFixedWidth(100);

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
                        QLineEdit *txt = new QLineEdit(qvm[DEFAULT].toString());
                        txt->setFixedWidth(100);
                        txt->setObjectName(uname);
                        txt->setProperty(DKEY_VALTYPE,TEXT);
                        if(qvm.contains(MAXLEN))
                        {
                            txt->setMaxLength(qvm[MAXLEN].toDouble());
                        }


                        if(!className.compare(CN_NEWFRAME))
                        {
                            ((NewFrame*)p)->onBindValue(txt,qvm[DEFAULT].toString());
                        }else{
                            ((NewLabel*)p)->onBindValue(txt,qvm[DEFAULT].toString());
                        }
                        mainLayout->addWidget(new QLabel(uname));
                        mainLayout->addWidget(txt);
                        txt->setFixedHeight(25);
                         connect(txt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));
                        //  v->addSpacerItem(new QSpacerItem(10,50));

                    }
                }
            }

        }

    }

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(verticalSpacer);
    mainLayout->setContentsMargins(0,0,0,0);


}



CompoentControls::CompoentControls(MainWindow *mw, QWidget *parent)
    : QGroupBox(parent),
      mainLayout(new QHBoxLayout()),
      mWindow(mw)
{

    // MainWindow *m ;
    setStyleSheet("{border: 1px solid gray;}");

    setTitle(tr("控件列表"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidget(this);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    //QGridLayout *mainLayout = new QGridLayout();
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(0,50,0,0);
    mainLayout->setSpacing(0);
    this->setFixedHeight(200);

    mSizePolicy.setVerticalPolicy(QSizePolicy::Preferred);



   mJsonFile =  QDir::currentPath() + "/menu_strip.json";
   // mJsonFile = QDir::currentPath() + "/control.json";
    qDebug() << " json file name " << mJsonFile;
    QFileInfo qfi(mJsonFile);
    if(!qfi.exists())
    {
        QMessageBox::warning(this,tr("错误"),tr("找不到控件文件"));
        return;
    }

    // HandleJson *hj = new HandleJson(filename);
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
    mainLayout->addLayout(comLayout);
   // mainLayout->addWidget(new QPushButton());
    QVBoxLayout *v = new QVBoxLayout();
    mainLayout->addLayout(v);

    comLayout->setVerticalSpacing(1);
    comLayout->setHorizontalSpacing(1);



    QPushButton *l = new QPushButton(tr("布局"));
    l->setSizePolicy(mSizePolicy);
    connect(l,SIGNAL(clicked(bool)),SLOT(onCreateNewLayout()));
    v->addWidget(l);


   // qDebug() << " compoents size " << comJsonArr.size();
    foreach (QJsonValue qjv, comJsonArr)
    {
        QVariantMap  qjm = qjv.toObject().toVariantMap();
        QString uname = qjv.toObject()[CAPTION].toString();
      //  qDebug() << " json key is " << uname;
        comMap[uname] = qjm;
        QPushButton *btnTest = new QPushButton(uname);
        btnTest->setSizePolicy(mSizePolicy);
        if(qjm.contains(ICON))
            btnTest->setIcon(QIcon(qjv.toObject()[ICON].toString()));

        btnTest->setFixedSize(40,40);
        if(col == 2)
        {
            col = 0;
            row++;
        }
        comLayout->addWidget(btnTest,row,col++,1,1);
        connect(btnTest,SIGNAL(clicked(bool)),this,SLOT(onCreateCompoentToCanvas()));

    }
  //  comLayout->setRowStretch(row,0);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    comLayout->addItem(verticalSpacer,++row,0,1,1);
}

void CompoentControls::onCreateCompoentToCanvas()
{
   // QObject *sender = QObject::sender(); /* 确定的那一个按钮被点击了 */

    NewLayout *activeLayer = mWindow->cManager->activeSS()->activeLayer();
    if(!activeLayer)
    {
        QMessageBox::warning(this,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }

    QPushButton *btn = (QPushButton*)(QObject::sender());
    NewFrame* ww = (NewFrame *)CreateObjectFromJson(comMap[btn->text()],
            mWindow->cManager->activeSS()->activeLayer()->m_frame);
            //mWindow->Scenes->activeLayer()->m_frame);
    ww->setObjectName(QString("%1_%2").arg(btn->text(),QString::number(comList.size())));
    activeLayer->appendChildObj(ww);
   // activeLayer->mNewFrameList.append(ww);
    comList.append(ww);

    this->parentWidget()->move(50,50);
    ww->addMainWindow(mWindow);
    ww->onSelectMe();

    // 找出图层,把新建的控件添加进去.
    mWindow->tree->addObjectToLayout(ww);
  //  mWindow->tree->addItemToRoot(ww->objectName(),btn->text());

    ww->show();
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

                if(!cval.compare(QFRAME))
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
                    nobj->setProperty(DKEY_CLSNAME,cval);
                }
                else if(!cval.compare(QLABEL))
                {
                    NewFrame *np = (NewFrame*)pobj;

                   // qDebug() << " NewLabel Property " << it.value().toMap();
                    nobj =qobject_cast<QObject*>(new NewLabel(np->m_frame));
                    if(qvm.contains(PROPERTY))
                    {
                    //    QVariantMap vp =qvm[PROPERTY].toMap();
                        ((NewLabel*)nobj)->copyProperty(qvm[PROPERTY]);
                       // qDebug() << " NewLabel dyn property " << ((NewLabel*)nobj)->dynValues;
                    }
                    nobj->setProperty(DKEY_CLSNAME,cval);

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
                QStringList chlist;
                foreach(QVariant qv, qvl)
                {
                    // qDebug() << qv.type();
                    if(qv.type() == QVariant::Map)
                    {
                        QObject *cobj = CreateObjectFromJson(qv.toMap(),nobj);
                        chlist.append(cobj->objectName());
                    }

                }

                pobj->setProperty("chlist",chlist);

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
    nobj->setProperty(DKEY_UID,QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
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
                        QString clsName = nobj->property(DKEY_CLSNAME).toString();
                        QVariantMap rect = it.value().toMap();
                        QRect r = QRect(rect["x"].toString().toInt(),
                                rect["y"].toString().toInt(),
                                rect["width"].toString().toInt(),
                                rect["height"].toString().toInt());

                        if(!clsName.compare(QFRAME))
                        {

                           qobject_cast<NewFrame *>(nobj)->setGeometry(r);
                           ((NewFrame*)nobj)->m_frame->setGeometry(r);
                           ((NewFrame*)nobj)->updateGeometry();
                            ((NewFrame*)nobj)->setFixedSize(r.size()+QSize(20,20));

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
        NewLayout *nl =  ss->activeLayer();
        //comList.append(nl);

        mWindow->tree->addItemToRoot(nl->objectName(),btn->text());
    }
  //  mWindow->cManager->activeSS()->createNewLayout();


}

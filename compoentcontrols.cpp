#include "compoentcontrols.h"
#include "handlejson.h"
#include "config.h"
#include "scenesscreen.h"
#include "canvasmanager.h"

CompoentControls::CompoentControls(QWidget *parent) : QGroupBox(parent),mainLayout(new QHBoxLayout())
{

    // MainWindow *m ;
    setStyleSheet("{border: 1px solid gray;}");
    QWidgetList tlist = qApp->topLevelWidgets();
    for(QWidgetList::iterator wit = tlist.begin();wit != tlist.end();++wit)
    {
        if((*wit)->objectName() == "MainWindow")
        {
            mWindow = (MainWindow*)(*wit);
            break;
        }
    }
    setTitle(tr("控件列表"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");

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
                    nobj =qobject_cast<QObject*>(n);
                    nobj->setProperty(DKEY_CLSNAME,cval);
                }
                else if(!cval.compare(QLABEL))
                {
                    NewFrame *np = (NewFrame*)pobj;
                    nobj =qobject_cast<QObject*>(new NewLabel(np->m_frame));
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
                          // qobject_cast<NewFrame *>(nobj)->move(QPoint(501,501));
                         // qDebug() << "create Frame geometry " << r << qobject_cast<NewFrame *>(nobj)->geometry();


                        }else{
                            qobject_cast<NewLabel *>(nobj)->setGeometry(r);
                          //  qDebug() << " label geometry " << qobject_cast<NewLabel *>(nobj)->geometry();
                        }
                    }
                    else if(!key.compare(IMAGE))
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

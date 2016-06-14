#include "compoentcontrols.h"
#include "handlejson.h"

CompoentControls::CompoentControls(QWidget *parent) : QWidget(parent)
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

    QGridLayout *mainLayout = new QGridLayout();
    this->setLayout(mainLayout);


    QPushButton *btnTest = new QPushButton(tr("test"));
    mainLayout->addWidget(btnTest,0,0);
    mainLayout->setContentsMargins(0,0,0,300);

    connect(btnTest,SIGNAL(clicked(bool)),SLOT(onCreateCompoentToCanvas()));

    mJsonFile =  QDir::currentPath() + "/menu_strip.json";
    //qDebug() << " json file name " << filename;
    QFileInfo qfi(mJsonFile);
    if(!qfi.exists())
    {
        QMessageBox::warning(this,tr("错误"),tr("找不到控件文件"));
        return;
    }

    // HandleJson *hj = new HandleJson(filename);
    ReadJsonFile();
    //  mJsonMap = hj->getCompoentMap();
    //  delete hj;

    /*
    QWidget *ww = (QWidget*)(hj->CreateObjectFromJson(hj->mJsonMap,mCanvas));
    qDebug() << "New object Rect " << ww->geometry()
             << " Pos " <<  ww->mapToParent(ww->pos());
    delete hj;
    */
}

QWidget *CompoentControls::getQWidgetByName(QString name) const
{
    QWidgetList tlist = qApp->topLevelWidgets();
    QWidget *w = 0;
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
                mJsonMap = qd.object().toVariantMap();

            }
        }else{
            qDebug() << " read Json file error";
            qDebug() << json_error.errorString();
        }


    }

}

void CompoentControls::onCreateCompoentToCanvas()
{

    QWidget* ww = (QWidget *)CreateObjectFromJson(mJsonMap,mWindow->mCanvas);
    QString caption = ww->property(DKEY_CAPTION).toString();
    ww->setObjectName(QString("%1_%2").arg(caption,QString::number(comList.size())));
    comList.append(ww);


    qDebug() << " clicked add QWidget " << ww->objectName() << ww->pos();
    qDebug() << " Com List size " << comList.size();
    ww->show();

}

QObject* CompoentControls::CreateObjectFromJson(QVariantMap qvm, QObject *pobj)
{
    QObject *nobj;
    QVariant property;
    for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
    {
        QString key = it.key();
        QVariant::Type qvt = it.value().type();


        switch (qvt) {
        case QVariant::String:
            if(!key.compare(CLASS))
            {
                QString cval = it.value().toString();

                if(!cval.compare(QFRAME))
                {
                    //mParentObj = new NewFrame();
                    nobj =qobject_cast<QObject*>(new NewFrame((QWidget *)pobj));
                    nobj->setProperty("clsName",cval);
                }
                else if(!cval.compare(QLABEL))
                {
                    nobj =qobject_cast<QObject*>(new NewLabel((QWidget *)pobj));
                    nobj->setProperty("clsName",cval);

                }
            }
            else if(!key.compare(NAME))
            {
                // mParentObj->setObjectName(it.value().toString());
                // qDebug() << "Create new Label " << it.value().toString() ;
                nobj->setObjectName(it.value().toString());
                //if(nobj)
                //   nobj->setObjectName(it.value().toString());

            }
            else if(!key.compare(CAPTION)) /* 界面显示的名称 */
            {
                nobj->setProperty(DKEY_CAPTION,it.value().toString());
            }

            // qDebug() << " Value is String : " << it.value().toString();
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
    nobj->setProperty("dynProperty",property);
    nobj->setProperty("uid",QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    /*  处理每一个json对像的property部分 */
    // qDebug() << "Dynamic Property Count " << nobj->dynamicPropertyNames().count();
    foreach(QByteArray qba,nobj->dynamicPropertyNames())
    {
        // qDebug() << " Property Key: "  << QString::fromLocal8Bit(qba);
        // qDebug() << " Property Val type :" << nobj->property(qba).type();
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
                    // qDebug() << "Property Val Map Key: " << key
                    //          << " Map Value:" << it.value();

                    if(!key.compare(RECT)) /* 这里直接处理json "rect" 对像字段 */
                    {
                        QString clsName = nobj->property("clsName").toString();
                        QVariantMap rect = it.value().toMap();
                        QRect r = QRect(rect["x"].toString().toInt(),
                                rect["y"].toString().toInt(),
                                rect["width"].toString().toInt(),
                                rect["height"].toString().toInt());

                        if(!clsName.compare(QFRAME))
                        {

                            QFrame *m = qobject_cast<QFrame*>(pobj);
                            int x =m->size().width()/2;
                            int y = m->size().height()/2;


                            qobject_cast<NewFrame *>(nobj)->setGeometry(r);
                             qobject_cast<NewFrame *>(nobj)->move(QPoint(x,y));


                        }else{
                            qobject_cast<NewLabel *>(nobj)->setGeometry(r);
                        }
                    }
                    else if(!key.compare("image"))
                    {
                        qobject_cast<NewLabel *>(nobj)->setPixmap(it.value().toString());
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

#include "handlejson.h"
#include <QtWidgets/QDesktopWidget>
#include "mainwindow.h"



HandleJson::HandleJson(QString name)
    :jsonName(name),
      mParentObj(0)

{
    ReadJsonFile();
}


void HandleJson::ReadJsonFile()
{

    QFile data(jsonName);
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
            QWidget *mp = 0;
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

QObject* HandleJson::CreateObjectFromJson(QVariantMap qvm, QObject *pobj)
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
                qDebug() << "Create new Label " << it.value().toString() ;
                nobj->setObjectName(it.value().toString());
                //if(nobj)
                //   nobj->setObjectName(it.value().toString());

            }

            qDebug() << " Value is String : " << it.value().toString();
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
    /*  处理每一个json对像的property部分 */
    qDebug() << "Dynamic Property Count " << nobj->dynamicPropertyNames().count();
    foreach(QByteArray qba,nobj->dynamicPropertyNames())
    {
         qDebug() << " Property Key: "  << QString::fromLocal8Bit(qba);
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
                            int x =m->size().width()/2-20;
                            int y = m->size().height()/2-20;


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

                        qDebug() << "other property Key : " << key  << " Val : " << it.value();
                    }

                }
            }
        }

    }

    return nobj;
}

QWidget *HandleJson::findQWidget(QObjectList objlist , QString widName)

{

    for(QObjectList::const_iterator oit=objlist.begin();oit != objlist.end() ; ++oit)
    {

          qDebug() << " child of Name " << widName << (*oit)->objectName();

        if(!(*oit)->objectName().compare(widName))

        {
            qDebug() << " Find object Name  " << (*oit)->objectName() << (*oit)->isWidgetType();

            return   qobject_cast<QWidget*>(*oit);

        }
    }
    return (QWidget*)0;
}

QWidget *HandleJson::findQWidget(QWidgetList objlist , QString widName)
{
    for(QWidgetList::const_iterator wit = objlist.begin();
        wit != objlist.end();++wit)
    {

        if(!(*wit)->objectName().compare(widName))
        {
             qDebug() << " Find object Name  " << (*wit)->objectName();
            return (*wit);

        }
    }
    return (QWidget*)0;
}



QRect HandleJson::readRect(QVariantMap qvm)
{
    int x,y,w,h;
    QVariantMap::const_iterator it = qvm.begin();
    if(qvm.size() != 4)
        return QRect();
    h = it.value().toString().toInt();
    ++it;
    w = it.value().toString().toInt();
    ++it;
    x = it.value().toString().toInt();
    ++it;
    y = it.value().toString().toInt();
    /* for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
    {
        QString key(it.key());
        if(!key.compare("x"))
        {
            x = it.value().toString().number();
        }else {
    }*/





    return QRect(x,y,w,h);
}

void HandleJson::HandleJsonMap(QVariantMap qvm)
{
    // QVariantMap qvm = qjo.toVariantMap();

    QList<QVariantList> objlist;
    for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
    {
        qDebug() << " Key : " << it.key();
        QVariant::Type qvt = it.value().type();
        qDebug() << " values type is : "  << qvt;
        if(qvt == QVariant::String)
        {
            qDebug() << " Value is : " << it.value().toString() ;
        }else if(qvt == QVariant::List)
        {
            QVariantList qvl = it.value().toList();
            foreach(QVariant qv, qvl)
            {
                // qDebug() << qv.type();
                if(qv.type() == QVariant::Map)
                {
                    HandleJsonMap(qv.toMap());
                }
            }

        }else if(qvt == QVariant::Double)
        {
            qDebug() << " number is " << it.value().toInt();
        }
        else if(qvt == QVariant::Map)
        {
            HandleJsonMap(it.value().toMap());
        }
    }

}


void HandleJson::HandleFrameObject(QJsonObject qjo,QString ParentName)
{

    ObjComt obj;
    obj.parentName = ParentName;
    QObject *child;

    for(QJsonObject::iterator it = qjo.begin();it != qjo.end();++it)
    {



        QString key = it.key();
        qDebug() << it.value().type();
        if(!key.compare(NAME))
        {
            obj.objName = it.value().toString();
        }else if(!key.compare(CLASS))
        {
            obj.clsName = it.value().toString();
        }else if(!key.compare("widget"))
        {
            if(it.value().isObject())
            {
                HandleFrameObject(it.value().toObject(),obj.objName);
            }
            else if(it.value().isArray())
            {
                QJsonArray qja = it.value().toArray();

                for(int idx = 0;idx < qja.size();idx++)
                {
                    if(qja[idx].isObject())
                    {
                        HandleFrameObject(qja[idx].toObject(),obj.objName);
                    }
                }
            }

        }else if(!key.compare(PROPERTY))
        {
            if(it.value().isArray())
            {
                QJsonArray qja = it.value().toArray();

                for(int idx = 0; idx < qja.size();idx++)
                {
                    QJsonValue qjv = qja[idx];
                    if(qjv.isObject())
                    {
                        QJsonObject ooj = qjv.toObject();
                        if(ooj.contains(RECT))
                        {

                            QJsonObject rect = ooj[RECT].toObject();
                            int x,y,w,h;
                            x = rect["x"].toString().toInt();
                            y = rect["y"].toString().toInt();
                            w = rect["width"].toString().toInt();
                            h = rect["height"].toString().toInt();

                            // obj.rect.setRect(x,y,w,h);
                        }else if(ooj.contains("image"))
                        {
                            int p = ooj["image"].type();
                            //  obj.pixmap = QDir::currentPath()+"/"+ ooj["image"].toString();
                        }

                    }

                }
            }
        }

    }



    ComList.append(obj);

}

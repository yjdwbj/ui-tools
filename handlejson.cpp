#include "handlejson.h"
#include <QtWidgets/QDesktopWidget>
#include "config.h"



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
                    nobj->setProperty(DKEY_CLSNAME,cval);
                }
                else if(!cval.compare(QLABEL))
                {
                    nobj =qobject_cast<QObject*>(new NewLabel((QWidget *)pobj));
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
    /*  处理每一个json对像的property部分 */

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
                    // qDebug() << "Property Val Map Key: " << key
                    //          << " Map Value:" << it.value();

                    if(!key.compare(RECT)) /* 这里直接处理json "rect" 对像字段 */
                    {
                        QString clsName = nobj->property(DKEY_CLSNAME).toString();
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
                    else if(!key.compare(IMAGE))
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

    return QRect(x,y,w,h);
}

void HandleJson::HandleJsonMap(QVariantMap qvm)
{
    // QVariantMap qvm = qjo.toVariantMap();

    QList<QVariantList> objlist;
    for(QVariantMap::const_iterator it = qvm.begin();it != qvm.end();++it)
    {
      //  qDebug() << " Key : " << it.key();
        QVariant::Type qvt = it.value().type();
       /// qDebug() << " values type is : "  << qvt;
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
         //   qDebug() << " number is " << it.value().toInt();
        }
        else if(qvt == QVariant::Map)
        {
            HandleJsonMap(it.value().toMap());
        }
    }

}


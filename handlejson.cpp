#include "handlejson.h"


void HandleFrameObject(QJsonObject qjo,QString ParentName)
{

    ObjComt obj;
    obj.parentName = ParentName;
    QObject *child;
    for(QJsonObject::iterator it = qjo.begin();it != qjo.end();++it)
    {
        QString key = it.key();
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

                           obj.rect.setRect(x,y,w,h);
                       }else if(ooj.contains("image"))
                       {
                           int p = ooj["image"].type();
                           obj.pixmap = QDir::currentPath()+"/"+ ooj["image"].toString();
                       }

                    }

                }
            }
        }

    }


   /* if(!obj.clsName.compare("QFrame"))
    {
        v
        obj.obj = qf;

    }else if(!obj.clsName.compare("QWidget"))
    {
        QWidget *wid = new QWidget();
        wid->setObjectName(obj.objName);
        obj.obj = wid;

    }else if(!obj.clsName.compare("QLabel"))
    {
        QLabel *lab  = new QLabel();
        lab->setParent(QObject::findChild<QFrame*>(obj.parentName));
        lab->setObjectName(obj.objName);
        lab->setGeometry(obj.rect);
        lab->setPixmap( QPixmap(obj.pixmap));
        lab->setVisible(true);

        obj.obj = lab;
    }*/
    ComList.append(obj);

}

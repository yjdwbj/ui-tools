#include "module.h"
#include <QtCore/QMetaProperty>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStackedLayout>

#include <QtWidgets/QSpinBox>

#include <QtWidgets/QSpacerItem>

#include <QApplication>
#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSignalMapper>
#include <QLineEdit>
#include <QFileDialog>
#include <QColorDialog>
#include <QRgb>
#include <QScrollBar>
#include <QDialogButtonBox>
#include <QFont>
#include <QTextEdit>
#include <QBitmap>
#include <QDateTime>

#include "core_def.h"
#include "scenesscreen.h"
#include "canvasmanager.h"



static   QStringList KeyList = QStringList() << DEFAULT << ENAME
                                             << IMAGE << GRAYCOLOR
                                             << COLOR << BAKCOLOR
                                             << BAKIMAGE ;


QJsonValue BaseForm::mCopyItem;
BaseForm::ObjTypes BaseForm::mCopyFromType;
QMap<QString,QWidget*> BaseForm::mObjectMap; // 新生成的控件.
QMap<QString,QWidget*> BaseForm::mSeqEnameMap; // 对应到小机里的唯一名称.
bool BaseForm::mPrjIsChanged = false;
bool BaseForm::mReadJson = false;


QWidgetList BaseForm::mObjectTemplte;
//QWidget *BaseForm::mLayer;
QWidget *BaseForm::mLayout;


QJsonValue Compoent::changeJsonValue(const QJsonArray &arg,QString key,
                                     const QVariant &val)
{
    QJsonArray parr = arg;
    int asize = parr.size();
    for(int i = 0;i < asize;i++)
    {
        QJsonValue item = parr.at(i);
        if(item.isArray())
        {
            QJsonArray tarray = item.toArray();
            parr.replace(i,changeJsonValue(tarray,key,val));
        }else if(item.isObject())
        {
            QJsonObject obj = item.toObject();
            if(obj.contains(STRUCT))
            {
                QJsonArray arr = obj[STRUCT].toArray();
                obj[STRUCT] = changeJsonValue(arr,key,val);
                parr[i] = obj;

            }else if(obj.contains(key) && !key.compare(KEY_RECT))
            {
                QString str = val.toString();
                QString section1 = str.section(":",0,0);
                int num = str.section(":",1,1).toInt();

                foreach (QString str, QStringList() << LX << LY
                         << WIDTH << HEIGHT) {
                    if(!str.compare(section1))
                    {
                        QJsonObject rect = obj[KEY_RECT].toObject();
                        rect[section1] = num;
                        obj[KEY_RECT] = rect;
                        break;
                    }
                }
                parr.replace(i,obj);
                break;
            }else if(obj.contains(key) && !key.compare(BORDER))
            {
                QString str = val.toString();
                QString section1 = str.section(":",0,0);
                int num = str.section(":",1,1).toInt();
                if(!section1.compare(GRAYCOLOR))
                {
                    obj[GRAYCOLOR] = str.section(":",1,1);
                }else{
                    foreach (QString kstr, QStringList() << TOP << LEFT
                             << RIGHT << BOTTOM) {
                        if(!kstr.compare(section1))
                        {
                            QJsonObject bobj = obj[BORDER].toObject();
                            bobj[kstr] = num;
                            obj[BORDER] = bobj;
                            break;
                        }
                    }
                }
                parr.replace(i,obj);
                break;
            } else if(obj[WTYPE].toString().contains(key) )
            {
                // 这里必需要有-type 这个属性名.
                if(obj.contains(LIST) && obj.contains(DEFAULT))
                {

                    QJsonValue v =  QJsonValue::fromVariant(val);
                    if(v.isArray())
                    {
                        obj[LIST] = v;
                    }else
                    {
                        obj[DEFAULT] = v;
                    }
                }else {
                    foreach (QString kstr, KeyList) {
                        if(obj.contains(kstr))
                        {
                            obj[kstr] = QJsonValue::fromVariant(val);
                            break;
                        }
                    }
                }
                parr.replace(i,obj);
                break;
            }
        }
    }
    return parr;
}

void Compoent::changeJsonValue(int idex, const QVariant &val)
{
    QJsonArray parr =  mOwerJson[PROPERTY].toArray();
    mOwerJson[PROPERTY] = changeJsonValue(parr,idex,val);
}

void Compoent::changeJsonValue(QString key, const QVariant &val)
{
    QJsonArray parr =  mOwerJson[PROPERTY].toArray();
    mOwerJson[PROPERTY] = changeJsonValue(parr,key,val);
}

QJsonArray Compoent::getActionListJson()
{
    QJsonArray parr =  mOwerJson[PROPERTY].toArray();
    for(int i = 0 ; i < parr.size();i++)
    {
        QJsonObject qo = parr.at(i).toObject();
        if(qo.contains(ACTION))
        {
            return qo[ACTION].toArray();
        }
    }
    return QJsonArray();
}


QJsonValue Compoent::changeJsonValue(const QJsonArray &json,int index, const QVariant &val)
{

    QJsonObject obj = json.at(index).toObject();
    QString wtype  = obj[WTYPE].toString();

    if(!wtype.compare(KEY_RECT))
    {

        QString str = val.toString();
        QString section1 = str.section(":",0,0);
        int num = str.section(":",1,1).toInt();

        foreach (QString str, QStringList() << LX << LY
                 << WIDTH << HEIGHT) {
            if(!str.compare(section1))
            {
                QJsonObject rect = obj[KEY_RECT].toObject();
                rect[section1] = num;
                obj[KEY_RECT] = rect;
                break;
            }
        }
    }else if(!wtype.compare(BAKCOLOR))
    {
        obj[BAKCOLOR] = QJsonValue::fromVariant(val);
    }else if(!wtype.compare(COLOR))
    {
        obj[COLOR] = QJsonValue::fromVariant(val);
    }
    else if(!wtype.compare(UID)){
        obj[ENAME] = QJsonValue::fromVariant(val);
    }else if(!wtype.compare(BAKIMAGE))
    {
        obj[BAKIMAGE] = QJsonValue::fromVariant(val);
    }else if(!wtype.compare(ENUM))
    {
        obj[DEFAULT] = QJsonValue::fromVariant(val);
    }
    else if(!wtype.compare(ALPHA))
    {
        obj[DEFAULT] = QJsonValue::fromVariant(val);
    }else if(!wtype.compare(BORDER))
    {
        QString str = val.toString();
        if(str.isEmpty())   // 为空表示这边框色被清除..
        {
            obj[GRAYCOLOR]=str;
        }else{
            QString section1 = str.section(":",0,0);
            int num = str.section(":",1,1).toInt();

            if(!section1.compare(GRAYCOLOR))
            {
                obj[GRAYCOLOR] = str.section(":",1,1);
            }else{
                foreach (QString kstr, QStringList() << TOP << LEFT
                         << RIGHT << BOTTOM) {
                    if(!kstr.compare(section1))
                    {
                        QJsonObject bobj = obj[BORDER].toObject();
                        bobj[kstr] = num;
                        obj[BORDER] = bobj;
                        break;
                    }
                }
            }
        }
    }
    else if(obj.contains(LIST))
    {

        QJsonValue v =  QJsonValue::fromVariant(val);
        if(v.isArray())
        {
            obj[LIST] = v;
        }else
        {
            obj[DEFAULT] = v;
        }
    }else if(obj.contains(DEFAULT))
    {
        obj[DEFAULT] = QJsonValue::fromVariant(val);
    }

    QJsonArray ret(json);
    ret.replace(index,obj);
    return ret;
}




void Compoent::changeJsonValue(QWidget *w,QString key, const QVariant &val)
{
    QVariant sval(val);
    //  qDebug() << "QWidget name " << key << val;
    QString owercls = w->property(DKEY_OWERJSON).toString();
    int jsonindex = w->property(DKEY_JSONIDX).toInt();

    int index = w->property(DKEY_PARRIDX).toInt();
    QJsonArray parray = mOwerJson[PROPERTY].toArray();
    QJsonObject pobj = parray.at(index).toObject();

    if(!owercls.compare("ComProperty"))
    {
        // 默就解析到 JSON PROPERTY 里的属性,不递归到CSS属性里去.
        // changeJsonValue(jsonindex,sval);
        parray = changeJsonValue(parray,jsonindex,sval).toArray();


    }else if(!owercls.compare("CssProperty"))
    {
        // 如果是QTabWidget 里的控件,就用JSON属性里的CSS属性数组.


        QJsonArray structarray = pobj[STRUCT].toArray();
        int  cssindex = w->property(DKEY_ARRIDX).toInt();

        QJsonArray cssArray = structarray.at(cssindex).toArray();


        structarray[cssindex] = changeJsonValue(cssArray,jsonindex,sval);
        //structarray[cssindex] = changeJsonValue(cssArray,key,val);
        pobj[STRUCT] = structarray;
        parray[index] = pobj;

    }
    mOwerJson[PROPERTY] = parray;
}



QJsonArray Compoent::updateRBJsonValue(const QJsonArray &arr, QWidget *w)
{
    QJsonArray projson = arr;

    int psize =projson.size();
    for(int i =0; i < psize;i++)
    {
        QJsonValue item = projson.at(i);
        if(item.isArray())
        {
            projson.replace(i,updateRBJsonValue(item.toArray(),w));
            return projson;
            continue;
        }

        //        QJsonObject ov = projson.at(i).toObject();
        QJsonObject ov = item.toObject();

        if(ov.contains(STRUCT))
        {
            QJsonArray array =  ov[STRUCT].toArray();
            ov[STRUCT] = updateRBJsonValue( array,w);

        }else if(ov.contains(KEY_RECT))
        {
            //ov = getRectJson(this);
            ov[KEY_RECT] = getRectJson(w)[KEY_RECT];
            //            if(w->property(DKEY_INTOCONTAINER).toBool())
            //            {

            //            }

        }else if(ov.contains(UID))
        {
            //  ov[UID] = w->property(DKEY_UID).toString();
            ov[UID] = w->property(DKEY_UID).toInt();
        }
        else if(ov.contains(BORDER))
        {
            ov[BORDER] = getBorderJson(w)[BORDER];
        }
        projson[i] = ov;

    }
    return projson;
}


QVariant Compoent::getJsonValue(const QJsonValue &val, QString key)
{
    if(val.isArray())
    {
        return getJsonValue(val.toArray(),key);
    }else if(val.isObject())
    {
        QJsonObject obj = val.toObject();
        foreach (QString str, KeyList) {
            if(obj.contains(str))
                return obj[str].toVariant();
        }
    }
}

QString Compoent::getEnameFromJson(const QJsonArray &arr)
{
    foreach (QJsonValue val, arr)
    {
        if(val.isObject())
        {
            QJsonObject obj = val.toObject();
            if(obj.contains(ENAME))
                return obj[ENAME].toString();
        }
    }
    return "";
}

QVariant Compoent::getJsonValue(const QJsonArray &parr,QString key)
{
    // for(int i = 0;i < asize;i++)
    QVariant ret;
    foreach (QJsonValue val, parr)
    {
        if(val.isArray())
        {
            ret = getJsonValue(val.toArray(),key);
            if(ret.isValid())
                return ret;
            else
                continue;
        }
        QJsonObject obj = val.toObject();
        QString  wtype = obj[WTYPE].toString();
        if(!wtype.compare(key) || obj.contains(key))
        {
            foreach (QString str, KeyList) {
                if(obj.contains(str))
                    return obj[str].toVariant();
            }

        }else if(!wtype.compare(STRUCT))
        {
            ret = getJsonValue(obj[STRUCT].toArray(),key);
            if(ret.isValid())
                return ret;
            else
                continue;
        }

    }
    return QVariant();
}

QVariant Compoent::getJsonValue(QString key) const
{

    return getJsonValue(mOwerJson[PROPERTY].toArray(),key);
}



void Compoent::onBindValue(QWidget *w)
{
    //   qDebug() << " dyn size " << dynValues.size();
    // 通过关键字来设置控件的默认值.
    if(!w)
        return;
    BaseForm *myself = (BaseForm*)this;
    QJsonValue  item = QJsonValue::fromVariant(w->property(DKEY_JSONSTR));

    // 是否是第一个TAB;
    // bool isFirst = !w->property(DKEY_ARRIDX).toInt();
    //     QString n = w->metaObject()->className();
    QString uname = w->objectName();
    if(w->inherits("QLineEdit"))
    {
        QLineEdit *txt = (QLineEdit *)w;

        QString jstr = getJsonValue(item, uname).toString();
        if(!jstr.isEmpty())
            txt->setText(jstr);
    }else if(w->inherits("QComboBox"))
    {
        QComboBox *cb = (QComboBox *)w;
        if(!uname.compare(PIC_TEXT)) return;
        QVariant nlv =  cb->property(DKEY_IMAGELST);
        if(nlv.isValid() )
        {
            // 处理图片列表与它的默认值.
            cb->clear();
            QVariantList nlist = nlv.toList();
            QString defimg = cb->property(DKEY_IMGIDX).toString();
            for(QVariantList::const_iterator it = nlist.begin();
                it != nlist.end();++it)
            {
                // example for key  is  "config/images/string/alarm_pol.bmp"
                QString key = (*it).toString();
                //QString key = (*it).toString().replace('\\',QDir::separator());

                cb->addItem(QIcon(key.section(SECTION_CHAR,1,1)),key.section(SECTION_CHAR,0,0));
            }
            cb->setCurrentText(defimg);
            cb->setProperty(DKEY_IMGIDX,defimg);
        }
        else
        {
            cb->setCurrentText(getJsonValue(item,uname).toString());
        }

    }else if(w->inherits("QSpinBox"))
    {
        QSpinBox *sp = (QSpinBox *)w;
        sp->setValue(getJsonValue(item,uname).toDouble());
    }
    else if(w->inherits("QPushButton"))
    {
        QColor c;
        if(!uname.compare(BAKCOLOR) ||
                !uname.compare(COLOR))
        {
            QString cname = getJsonValue(item,uname).toString();
            c = QColor(cname) ;
            // if(isFirst)
            if(c.isValid())
            {

                if(item.toObject().contains(BAKCOLOR))
                    myself->mbkColor =c.name(QColor::HexArgb);
            }
            else
                myself->getPairWidgetFromPLayout(w)->setEnabled(false);
        }
        else if(!uname.compare(BORDER))
        {
            //unsigned int n = getJsonValue(BORDER).toUInt();
            //灰度转成ＲＧＢ
            // QColor c(n & 0xfff00000,n &0xfff00,n & 0xff);
            //QColor c = QColor::fromRgb(n);
            // 这里暂时有RGB颜色测试
            c = QColor(getJsonValue(item,GRAYCOLOR).toString());
            // if(isFirst)
            if(c.isValid())
                myself->mBorderColor = c.name(QColor::HexArgb);
            else
                myself->getPairWidgetFromPLayout(w)->setEnabled(false);

        }else if(!uname.compare(BAKIMAGE))
        {
            QPixmap p(12,12);
            QString img = getJsonValue(item,BAKIMAGE).toString();
            if(!img.isEmpty())
            {

                img = QDir::currentPath() + BACKSLASH + img;
                p.load(img);
                ((QPushButton*)w)->setIcon(p);
                // if(isFirst)
                myself->mbkImage = img;
            } else
                myself->getPairWidgetFromPLayout(w)->setEnabled(false);
            return;
        }else if(!uname.compare(PIC_TEXT))
        {


            return;
        }
        //if(isFirst)
        //    myself->updateStyleSheets();
        if(c.isValid())
        {
            QPixmap p(12,12);
            p.fill(c);
            ((QPushButton*)w)->setIcon(p);
        }

        ((QPushButton*)w)->setProperty(DKEY_COLOR,c.name(QColor::HexArgb));
    }
}


QJsonObject Compoent::getRectJson(QWidget *w)
{
    QJsonObject rect;
    QVariantMap vmap;
    //    if(w->property(DKEY_INTOCONTAINER).toBool())
    if(w->inherits("BaseForm") &&
            ((BaseForm*)w)->mParent->isContainer())
    {
        // 在容器控件里要把它的坐标,转换成相对于父控件的绝对坐标.
        //        QPoint pos = w->mapToParent(w->parentWidget()->pos());
        QPoint pos = w->mapToParent(((BaseForm*)w)->mParent->pos());
        vmap[LX] = pos.x();
        vmap[LY] = pos.y();
    }else{
        vmap[LX] = w->x();
        vmap[LY] = w->y();
    }

    vmap[WIDTH] = w->width();
    vmap[HEIGHT] = w->height();
    rect[KEY_RECT] = QJsonObject::fromVariantMap(vmap);
    return rect;
}



QJsonObject Compoent::getValueFromProperty(const QJsonArray &arr, const QString &key)
{
    QJsonObject ret;
    foreach (QJsonValue pval, arr) {
        if(pval.isArray())
        {
            return getValueFromProperty(pval.toArray(),key);
        }
        QJsonObject pobj = pval.toObject();
        if(pobj.contains(key))
        {
            return pobj;
        }
        else if(pobj.contains(STRUCT)) {
            return getValueFromProperty(pobj[STRUCT].toArray(),key);
        }
    }
    return  ret;
}

QRect Compoent::getRectFromStruct(const QJsonArray &arr,QString key)
{
    QJsonObject rectobj  = getValueFromProperty(arr,key);
    QRect rect(0,0,0,0);
    if(!rectobj.isEmpty())
    {
        QVariantMap obj =  rectobj[key].toObject().toVariantMap();
        if(!key.compare(BORDER))
        {
            rect.setLeft(obj[LEFT].toInt());
            rect.setBottom(obj[BOTTOM].toInt());
            rect.setRight(obj[RIGHT].toInt());
            rect.setTop(obj[TOP].toInt());
        }else if(!key.compare(KEY_RECT)) {
            rect.setX(obj[LX].toInt());
            rect.setY(obj[LY].toInt());
            rect.setWidth(obj[WIDTH].toInt());
            rect.setHeight(obj[HEIGHT].toInt());
        }

    }
    return rect;
}

QJsonObject Compoent::getBorderJson(QWidget *w)
{
    QJsonObject border;
    QVariantMap vmap ;
    BaseForm *fr = (BaseForm*)w;
    vmap[LEFT] = fr->mBorder.left();
    vmap[TOP] = fr->mBorder.top();
    vmap[RIGHT] = fr->mBorder.right();
    vmap[BOTTOM] = fr->mBorder.bottom();
    border[BORDER] = QJsonObject::fromVariantMap(vmap);
    return border;
}


BaseForm::BaseForm(QWidget *parent)
    :FormResizer(parent), mBorderColor(""),
      mBorder(0,0,0,0),
      mCreateFlag(false),
      posWidget(0)
{
    setStyleSheet("");
    mbkImage = "";
    setContentsMargins(0,0,0,0);
    setAcceptDrops(true);
    this->m_frame->setAcceptDrops(true);
    mPrjIsChanged = true;
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);

}

void BaseForm::mouseMoveToPos(const QPoint &p)
{
    onSelectMe();
    move(this->pos() + p);
    changeJsonValue( posWidget,
                     KEY_RECT,
                     QString("%1:%2").arg(LX,
                                          QString::number(x())));
    changeJsonValue( posWidget,
                     KEY_RECT,
                     QString("%1:%2").arg(LY,
                                          QString::number(y())));
}


void BaseForm::mouseMoveEvent(QMouseEvent *event)
{
    //    QString clsname = parentWidget()->parentWidget()->metaObject()->className();
    //   //  qDebug() << " parent class name " <<  clsname;
    //     // 这里主要防止它在列表的拖动事件.
    //    QString thisname = metaObject()->className();
    //    if(thisname.compare(CN_NEWLAYER))
    //        if(CN_NEWLAYOUT.compare(clsname) &&
    //                CN_NEWLAYER.compare(clsname) ) return;
    if (event->buttons() & Qt::LeftButton)
    {

        QPoint np = mapTo(CanvasManager::mActiveSS,event->pos()) - event->pos() ;

        if(event->pos().x() > mOffset.x())
            np.setX(np.x() + SELECTION_MARGIN/2);
        if(event->pos().x() < mOffset.x())
            np.setX(np.x() - SELECTION_MARGIN/2);

        if(event->pos().y() > mOffset.y())
            np.setY(np.y() + SELECTION_MARGIN/2);
        if(event->pos().y() < mOffset.y())
            np.setY(np.y() - SELECTION_MARGIN/2);


        CanvasManager::mActiveSS->mXYLine->setPos(np);



        //        if(property(DKEY_INTOCONTAINER).toBool())
        if(mParent->mType == T_NewGrid ||
                mParent->mType == T_NewList)
        {

            mParent->mouseMoveToPos(event->pos() - mOffset);

        }else{

            mouseMoveToPos(event->pos() - mOffset);
            /* 把新的位置更新到右边属性框 */
            posWidget->updatePosition(this);
        }
        event->accept();
        this->blockSignals(true);
        mPrjIsChanged = true;
    }
}

void BaseForm::mousePressEvent(QMouseEvent *event)
{
    //    qDebug() << " my name " << this->objectName() << "class " << metaObject()->className();
    onSelectMe();
    mPrjIsChanged = true;
    if (event->button() == Qt::LeftButton)
    {
        mOffset = event->pos();
        mOldRect = this->geometry();
        this->setProperty(DKEY_OLDPOS,this->pos());
        mOldSize = this->size();
        //        mWindow->mXYLine->raise();
        CanvasManager::mActiveSS->mXYLine->setHidden(false);

    }else if(event->button() == Qt::RightButton)
    {

        createContextMenu(this,mapToGlobal(event->pos()));

    }
}


void BaseForm::SwapLayerOrder(SwapType st)
{
    //    QWidget *p =  this->parentWidget();
    //    qDebug() << "this meta class name " << this->metaObject()->className();
    QList<QWidget*> *mlist;
    if(mParent == this)
    {
        mlist = &((ScenesScreen*)this->parentWidget())->childlist;
    }else
    {
        mlist = &mParent->childlist;
    }
    //    qDebug() << " parent widget objectname " << p->objectName() << p->metaObject()->className();
    if(mlist->size())
    {
        int index = mlist->indexOf(this);
        mlist->removeAt(index);
        switch (st) {
        case UpOne:
            mlist->insert(index+1,this);
            break;
        case DownOne:
            mlist->insert(index-1,this);
            break;
        case DownBottom:
            mlist->insert(0,this);
            break;
        case UpTop:
            mlist->insert(mlist->count(),this);
            break;
        default:
            break;
        }
    }

}


void BaseForm::objectMoveSwapMenu(QMenu *contextMenu)
{
    //    QWidget *pwid = this->parentWidget();
    QList<QWidget*> plist ;
    if(this == mParent)
    {
        plist = ((ScenesScreen*)this->parentWidget())->childlist;
    }else{
        plist = mParent->childlist;
    }
    QTreeWidget *treeWidget = mWindow->tree->treeWidget;
    if(plist.count() > 1)
    {
        auto lambda_downone = [=](){
            QWidget *last = this;
            foreach( QWidget *o,plist)
            {
                // qDebug() << " object order " << o->objectName();
                if(o == this)
                {
                    if(o == last)
                        break;
                    else
                    {
                        this->stackUnder(last);
                        int myindex = 0;

                        QTreeWidgetItem *item = treeWidget->currentItem();
                        int row = mWindow->tree->treeWidget->currentIndex().row();
                        if(item && row > 0)
                        {
                            QTreeWidgetItem* parent = item->parent();
                            if(!parent)
                            {
                                myindex = treeWidget->indexOfTopLevelItem(item);
                                mWindow->tree->moveTopItemOrder(myindex,myindex-1);
                            }else{
                                myindex = parent->indexOfChild(item);
                                mWindow->tree->moveItemOrder(parent,myindex,myindex-1);
                            }
                        }
                        SwapLayerOrder(DownOne);
                        break;
                    }

                }
                last=(QWidget*)o;
            }
        };


        auto lambda_upone = [=](){
            int n= 0;
            foreach( QWidget *o,plist)
            {
                //  qDebug() << " object order " << o->objectName();
                if(o == this)
                {
                    n = 1;
                }else if(n)
                {
                    ((QWidget*)o)->stackUnder(this);

                    QTreeWidgetItem *item =  treeWidget->currentItem();
                    int myindex = 0;
                    if(item)
                    {
                        QTreeWidgetItem* parent = item->parent();
                        if(!parent)
                        {
                            myindex = treeWidget->indexOfTopLevelItem(item);
                            mWindow->tree->moveTopItemOrder(myindex,myindex-1);
                        }else{
                            myindex = parent->indexOfChild(item);
                            mWindow->tree->moveItemOrder(parent,myindex,myindex+1);
                        }
                    }
                    SwapLayerOrder(UpOne);
                    break;
                }
            }
        };


        auto lambda_downbottom = [=](){

            this->lower();

            QTreeWidgetItem *item =  treeWidget->currentItem();
            if(item)
            {
                QTreeWidgetItem* parent = item->parent();
                if(!parent)
                {
                    mWindow->tree->moveTopItemOrder(treeWidget->indexOfTopLevelItem(item),0);
                }else{
                    mWindow->tree->moveItemOrder(parent,parent->indexOfChild(item),0);
                }
            }
            SwapLayerOrder(DownBottom);
        };


        auto lambda_uptop = [=](){
            this->raise();
            QTreeWidgetItem *item =  treeWidget->currentItem();
            if(item)
            {
                QTreeWidgetItem* parent = item->parent();
                if(!parent)
                {
                    mWindow->tree->moveTopItemOrder(
                                treeWidget->indexOfTopLevelItem(item),
                                treeWidget->topLevelItemCount()-1);
                }else{
                    mWindow->tree->moveItemOrder(parent,
                                                 parent->indexOfChild(item),
                                                 parent->childCount()-1);
                }

                SwapLayerOrder(UpTop);
            }
        };


        if(plist.last() == this)
        {
            QAction *downone = new QAction(QIcon(":/icon/icons/go-down.png"),"移下一层",this);
            connect(downone,&QAction::triggered,lambda_downone);
            QAction *downbottom= new QAction(QIcon(":/icon/icons/go-bottom.png"),"移到底层",this);
            connect(downbottom,&QAction::triggered,lambda_downbottom);
            contextMenu->addAction(downone);
            contextMenu->addAction(downbottom);
        }else if(plist.first() == this)
        {
            QAction *upone = new QAction(QIcon(":/icon/icons/go-up.png"),"移上一层",this);
            connect(upone,&QAction::triggered,lambda_upone);
            QAction *uptop = new QAction(QIcon(":/icon/icons/go-top.png"),"移到顶层",this);
            connect(uptop,&QAction::triggered,lambda_uptop);
            contextMenu->addAction(upone);
            contextMenu->addAction(uptop);

        }else{

            QAction *downone = new QAction(QIcon(":/icon/icons/go-down.png"),"移下一层",this);
            connect(downone,&QAction::triggered,lambda_downone);
            QAction *downbottom= new QAction(QIcon(":/icon/icons/go-bottom.png"),"移到底层",this);
            connect(downbottom,&QAction::triggered,lambda_downbottom);
            QAction *upone = new QAction(QIcon(":/icon/icons/go-up.png"),"移上一层",this);
            connect(upone,&QAction::triggered,lambda_upone);
            QAction *uptop = new QAction(QIcon(":/icon/icons/go-top.png"),"移到顶层",this);
            connect(uptop,&QAction::triggered,lambda_uptop);
            contextMenu->addAction(downone);
            contextMenu->addAction(downbottom);
            contextMenu->addAction(upone);
            contextMenu->addAction(uptop);
        }


    }
    contextMenu->addSeparator();
}

void BaseForm::listObjectMoveMenu(QMenu *contextMenu,BaseForm *container)
{
    if(container->inherits(CN_NEWLIST))
    {
        QBoxLayout *boxlayout = ((NewList*)container)->listLayout;
        int first =  boxlayout->indexOf(this);
        int last = boxlayout->count() -1;

        //        qDebug() << " this widget index of  " << first
        //                 << " count is " << boxlayout->count();
        if(boxlayout->count() > 1)
        {

            auto lambda_downbottom =        [=](){
                boxlayout->insertItem(boxlayout->count(),
                                      boxlayout->takeAt(first));
                mWindow->tree->moveListItemOrder(container->mUniqueStr,
                                                 first,
                                                 boxlayout->count());
                SwapLayerOrder(DownBottom);
            };

            auto lambda_uptop =         [=](){
                boxlayout->insertItem(0,boxlayout->takeAt(first));
                mWindow->tree->moveListItemOrder(container->mUniqueStr,
                                                 first,
                                                 0);
                SwapLayerOrder(UpTop);
            };

            auto lambda_upone = [=](){
                boxlayout->insertItem(first - 1,boxlayout->takeAt(first));
                mWindow->tree->moveListItemOrder(container->mUniqueStr,
                                                 first,
                                                 first - 1);
                SwapLayerOrder(UpOne);
            };
            auto lambda_downone = [=](){
                boxlayout->insertItem(first + 1,boxlayout->takeAt(first));
                mWindow->tree->moveListItemOrder(container->mUniqueStr,
                                                 first,
                                                 first + 1);
                SwapLayerOrder(DownOne);
            };

            if(first == 0 )
            {

                QAction *downbottom = new QAction(QIcon(":/icon/icons/go-bottom.png"),"移到底层",this);
                connect(downbottom,&QAction::triggered,lambda_downbottom);
                QAction *downone = new QAction(QIcon(":/icon/icons/go-down.png"),"移下一层",this);
                connect(downone,&QAction::triggered,lambda_downone);
                contextMenu->addAction(downone);
                contextMenu->addAction(downbottom);

            }else if(first == last )
            {
                QAction *uptop = new QAction(QIcon(":/icon/icons/go-top.png"),"移到顶层",this);
                connect(uptop,&QAction::triggered,lambda_uptop);
                QAction *upone = new QAction(QIcon(":/icon/icons/go-up.png"),"移上一层",this);
                connect(upone,&QAction::triggered,lambda_upone);
                contextMenu->addAction(upone);
                contextMenu->addAction(uptop);
            }else{
                QAction *downbottom = new QAction(QIcon(":/icon/icons/go-bottom.png"),"移到底层",this);
                connect(downbottom,&QAction::triggered,lambda_downbottom);
                QAction *downone = new QAction(QIcon(":/icon/icons/go-down.png"),"移下一层",this);
                connect(downone,&QAction::triggered,lambda_downone);
                QAction *uptop = new QAction(QIcon(":/icon/icons/go-top.png"),"移到顶层",this);
                connect(uptop,&QAction::triggered,lambda_uptop);
                QAction *upone = new QAction(QIcon(":/icon/icons/go-up.png"),"移上一层",this);
                connect(upone,&QAction::triggered,lambda_upone);
                contextMenu->addAction(upone);
                contextMenu->addAction(uptop);
                contextMenu->addAction(downone);
                contextMenu->addAction(downbottom);
            }
        }
        contextMenu->addSeparator();
    }
}

void BaseForm::createContextMenu(QWidget *parent,QPoint pos)
{

    QMenu *contextMenu = new QMenu(parent);
    //    QString clsname = metaObject()->className();
    QAction delme(QIcon(":/icon/icons/removesubmitfield.png")  ,
                  QString("删除当前-%1").arg(this->property(DKEY_TXT).toString()),this);
    connect(&delme,SIGNAL(triggered(bool)),SLOT(onDeleteMe()));

    QAction saveTemp(QIcon(":/icon/icons/build.png"),"保存成控件",this);
    if(inherits(CN_NEWLAYOUT))
    {
        contextMenu->addAction(&saveTemp);
        connect(&saveTemp,SIGNAL(triggered(bool)),SLOT(onBeComeTemplateWidget()));
    }

    if(!inherits(CN_NEWFRAME))
    {
        QAction *qaobj = new QAction(this);
        if(parent->inherits("QTreeWidget"))
        {
            QTreeWidget *w = ((QTreeWidget*)parent);
            //            QTreeWidgetItem *i = w->itemAt(w->viewport()->mapFromGlobal(pos));
            //            qDebug() << "tree click  " << this->objectName() << this->metaObject()->className();
            QAction *subobj = new QAction(this);

            if(isHidden())
            {
                subobj->setIcon(QIcon(SHOW_ICON));
                subobj->setIconText("显示同类容器");
                qaobj->setIcon(QIcon(SHOW_ICON));
                qaobj->setIconText("显示");

            }else{
                subobj->setIcon(QIcon(HIDE_ICON));
                subobj->setIconText("隐藏同类容器");
                qaobj->setIcon(QIcon(HIDE_ICON));
                qaobj->setIconText("隐藏");
            }
            connect(subobj,SIGNAL(triggered(bool)),mWindow->tree, SLOT(onSwapShowHideSubObject()));
            contextMenu->addAction(subobj);
        }else{

            //        QAction *qaobj = new QAction(this);
            if(isHidden())
            {
                qaobj->setIcon(QIcon(SHOW_ICON));
                qaobj->setIconText("显示");
            }else{
                qaobj->setIcon(QIcon(HIDE_ICON));
                qaobj->setIconText("隐藏");
            }
        }
        //        QAction hideobj(QIcon(HIDE_ICON),"隐藏",this);
        connect(qaobj,SIGNAL(triggered(bool)),SLOT(onSwapViewObject()));
        contextMenu->addAction(qaobj);
    }

    contextMenu->addSeparator();
    QAction copy(QIcon(":/icon/icons/editcopy.png"),"复制",this);
    connect(&copy,&QAction::triggered,[=](){
        mCopyItem = QJsonValue(writeToJson());
        mCopyFromType = mType;
    });

    QAction paste(QIcon(":/icon/icons/editpaste.png"),"粘贴",this);
    connect(&paste,&QAction::triggered,[=](){
        CanvasManager::mActiveSS->pasteItem(this);
    });

    paste.setEnabled(!mCopyItem.isNull());
    contextMenu->addAction(&copy);
    contextMenu->addAction(&paste);
    contextMenu->addSeparator();

    if(mParent->isContainer())
    {
        listObjectMoveMenu(contextMenu,mParent);
    }
    else{
        objectMoveSwapMenu(contextMenu);
    }
    QAction findaction(QIcon(":/icon/icons/search.png"),"查找对像",this);
    connect(&findaction,&QAction::triggered,[=]{
        findDlg dlg(mWindow);

        dlg.exec();
    });
    contextMenu->addAction(&findaction);
    contextMenu->addSeparator();

    auto lambda_grid_menu = [=](NewGrid *ng){
        contextMenu->addAction(ng->menuAddRow);
        contextMenu->addAction(ng->menuAddCol);
        contextMenu->addAction(ng->menuSpace);
        contextMenu->addAction(ng->menuSize);
        contextMenu->addAction(ng->menuV);
        contextMenu->addAction(ng->menuH);

    };

    auto lambda_list_menu  = [=](NewList*  nl){
        contextMenu->addAction(nl->menuAddLine);
        contextMenu->addAction(nl->menuSetHeight);
        contextMenu->addAction(nl->menuSetSpace);
    };

    //    bool inContainer = property(DKEY_INTOCONTAINER).toBool();
    if(mParent->mType == T_NewList)
    {
        lambda_list_menu((NewList*)(mParent));
    }else if(mParent->mType == T_NewGrid)
    {
        lambda_grid_menu((NewGrid*)(mParent));
    }
    else if(inherits(CN_NEWGRID))
    {
        lambda_grid_menu((NewGrid*)(this));
    }else if(inherits(CN_NEWLIST))
    {
        lambda_list_menu((NewList*)(this));
    }

    contextMenu->addSeparator();
    contextMenu->addAction(&delme);
    contextMenu->exec(pos);
    contextMenu->deleteLater();
}


void BaseForm::onSwapViewObject()
{
    mWindow->tree->onSwapShowHideObject();
}

void BaseForm::mouseReleaseEvent(QMouseEvent *event)
{

    CanvasManager::mActiveSS->mXYLine->setHidden(true);
    /* 放开鼠标时检查它的是否出了边界要 */
    // QWidget *p = this->parentWidget();
    QPoint pos = this->pos();
    moveNewPos(pos);
    // 这里只能在释放鼠标时改变左边的控件值
    //    qDebug() << " object size " << this->size()
    //             << " object pos " << this->pos();
    //  if(!property(DKEY_INTOCONTAINER).toBool())
    if(mParent->isContainer())
        return;

    if(mType == T_NewList)
    {
        ((NewList*)this)->mainScroll->resize(this->size());
        ((NewList*)this)->updateAllItemsSize();
    }else if(mType == T_NewGrid){
        ((NewGrid*)this)->mainScroll->resize(this->size());
        ((NewGrid*)this)->mainWidget->resize(this->size());
    }

    if(!posWidget) return;

    posWidget->updateSize(this);
    posWidget->updatePosition(this);
    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(LX,QString::number(this->x())));
    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(LY,QString::number(this->y())));

    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(WIDTH,QString::number(this->width())));
    changeJsonValue(posWidget,
                    KEY_RECT,
                    QString("%1:%2").arg(HEIGHT,QString::number(this->height())));
    event->accept();
}

void BaseForm::moveNewPos(int x,int y)
{
    moveNewPos(QPoint(x,y));
}

void BaseForm::moveNewPos(QPoint pos)
{
    QWidget *p = this->parentWidget();


    QSize ms = p->size();
    QSize thsize=(this->size());
    QPoint oldpos = this->property(DKEY_OLDPOS).toPoint();
    if(this->pos() == oldpos)
    {
        if((this->width() + this->x()) > ms.width())
        {
            thsize.setWidth(ms.width() - this->x());
        }

        if((this->y() + this->height()) > ms.height())
        {
            thsize.setHeight(ms.height() - this->y());
        }


    }else {

        if(this->x() < 0)
        {
            pos.setX(0 /*- MARGIN_SIZE.width() /2*/);
            //this->move(pos);
        }

        if(this->x() > ms.width() )
        {
            pos.setX(ms.width() - this->width());
        }
        else if((this->x() + this->width()) > ms.width())
        {
            pos.setX(ms.width() - this->width() );
        }

        if(this->y() < 0 )
        {
            pos.setY(0 /*-MARGIN_SIZE.width() /2*/);
            //      this->move(pos);
        }

        if(this->y() > ms.height())
        {
            pos.setY(ms.height() - this->height());

        }else if((this->y() + this->height()) > ms.height())
        {
            pos.setY(ms.height() - this->height());
        }
    }

    this->setGeometry(QRect(pos,thsize));
    // 这里又要把侧栏里的坐标同步.这里如果不阻塞它的信号,就会是一个死循环.
    if(this->posWidget)
    {
        this->posWidget->blockSignals(true);
        this->posWidget->updateSize(this);
        this->posWidget->updatePosition(this);
        this->posWidget->blockSignals(false);
    }

    // }
}


void BaseForm::onXYWHChangedValue(int v)
{
    /* 绑定坐标控件的更新 */

    QWidget *sender =(QWidget *)(QObject::sender());
    // bool isFirst = !sender->property(DKEY_ARRIDX).toInt();
    QPoint pos = this->pos();
    if(!sender->objectName().compare(X))
    {

        pos.setX(v);

        moveNewPos(pos);

        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(LX,QString::number(v)));

    }else if(!sender->objectName().compare(Y))
    {

        pos.setY(v);

        moveNewPos(pos);

        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(LY,QString::number(v)));

    }else if(!sender->objectName().compare(W))
    {

        this->setProperty(DKEY_OLDPOS,pos);
        QSize n(this->size());
        n.setWidth(v);
        this->resize(n);
        moveNewPos(pos);

        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(WIDTH,QString::number(v)));
    }else if(!sender->objectName().compare(H))
    {

        this->setProperty(DKEY_OLDPOS,pos);
        QSize n(this->size());
        n.setHeight(v);
        this->resize(n);
        moveNewPos(pos);

        changeJsonValue(sender,
                        KEY_RECT,
                        QString("%1:%2").arg(HEIGHT,QString::number(v)));
    }

    if(this->mType == T_NewList ||
            this->mType == T_NewGrid)
    {
        QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonRelease,QCursor::pos(),
                                             Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
        QApplication::postEvent(this,event);
    }

}

void BaseForm::onNumberChanged(int num)
{

    QSpinBox *sp = (QSpinBox *)(QObject::sender());
    changeJsonValue(sp,sp->objectName(),num);
}

void BaseForm::onTextChanged(QString str)
{
    QLineEdit *txt = (QLineEdit *)(QObject::sender());
    QJsonObject obj = QJsonValue::fromVariant(txt->property(DKEY_JSONSTR)).toObject();



    if(obj.contains(ENAME))
    {
        QString old = obj[ENAME].toString();
        if (!old.compare(str))
            return;
        //        mWindow->ComCtrl->mEnameSeq.removeOne(old);
        mEnameStr = getEnameSeq(str,this);
        mWindow->tree->updateSeq(mEnameStr);
        //        mWindow->ComCtrl->mEnameSeq.append(nstr);
        changeJsonValue(txt,txt->objectName(),mEnameStr);

    }else{

        changeJsonValue(txt,txt->objectName(),str);

    }


    //  dynValues[txt->objectName()] = str;
}


void BaseForm::onTextSelected()
{
    QObject *obj  = QObject::sender();
    QPushButton *btn =  ((QPushButton*)obj);
    MenuItemDialog *md = new MenuItemDialog(btn->text(),mWindow);
    md->exec();
    changeJsonValue(obj->objectName(),md->text);
    btn->setText(md->text);
    md->deleteLater();
}


void BaseForm::onBorderChangedValue(int v)
{
    QWidget *w = (QWidget*)QObject::sender();
    QString name = w->objectName();
    changeJsonValue(w,BORDER,
                    QString("%1:%2").arg(name,QString::number(v)));
    //     if(!w->property(DKEY_ARRIDX).toInt())  // 只有第一全CSS属性可以与界面互动.
    //     {
    if(!name.compare(LEFT))
    {
        mBorder.setLeft(v);
    }else if(!name.compare(TOP))
    {

        mBorder.setTop(v);
    }else if(!name.compare(RIGHT))
    {

        mBorder.setRight(v);
    }else if(!name.compare(BOTTOM))
    {

        mBorder.setBottom(v);
    }
    updateStyleSheets();
    //  }
    this->blockSignals(true);
}


void BaseForm::updateStyleSheets()
{
    QString str = QString(
                "border-style: solid; "\
                "border-left-width: %1px; "\
                "border-top-width: %2px; " \
                "border-right-width: %3px; "\
                "border-bottom-width: %4px;").arg(QString::number(mBorder.left()),
                                                  QString::number(mBorder.top()),
                                                  QString::number(mBorder.right()),
                                                  QString::number(mBorder.bottom())
                                                  );


    if(!mBorderColor.isEmpty())
    {
        str = QString("border-color: %1; %2").arg(mBorderColor,str);
    }else{
        str = "";
    }

    // 没有背景图片这个参数时就显示背景色.
    if(!mbkImage.isEmpty())
    {
        str =  QString("background-image: url(%1); %2").arg(mbkImage,str);
    }else{
        //找图片列表属性.
        QJsonArray parray = mOwerJson[PROPERTY].toArray();
        QString imgpic ="";
        QString txtpic = "";
        for(int n = 0; n < parray.count() ; n++)
        {
            QJsonObject obj = parray.at(n).toObject();
            if(obj.contains(LIST))
            {
                QString Stype = obj[WTYPE].toString();
                if(!Stype.compare(PIC_LIST))
                {
                    imgpic = obj[LIST].toArray().at(0).toString();
                }else if(!Stype.compare(PIC_TEXT))
                {
                    txtpic = obj[LIST].toArray().at(0).toString();
                }
                if(!imgpic.isEmpty())
                {

                    break;
                }

            }
        }

        if(!imgpic.isEmpty())
            str =  QString("background-image: url(%1); %2").arg(imgpic,str);
        else if(!txtpic.isEmpty())
        {

            QString textname = mWindow->mItemMap[txtpic];

            QString fpath =  mWindow->cManager->mProjectImageDir + BACKSLASH + txtpic + ".png";
            QPixmap pic = mWindow->mImgMap[fpath];

            if(pic.isNull())
            {
                if(QFileInfo(fpath).exists())
                {
                    pic.load(fpath);
                    mWindow->mImgMap[fpath] = pic;

                }else{

                    QFont font;
                    QLineEdit *edit = new QLineEdit(this);
                    //  str = QString("font-family: %1 ;font-size: 16px;").arg(font.defaultFamily());
                    edit->setStyleSheet(QString("font-family: %1 ;font-size: 16px;").arg(font.defaultFamily()));

                    edit->setText(textname);
                    // edit->adjustSize();
                    edit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
                    edit->adjustSize();
                    edit->setFixedWidth(5 + textname.count() * 16);
                    QSize s(edit->width(),edit->height());
                    QPixmap pixmap(s);
                    edit->render(&pixmap,QPoint(),QRegion(QRect(QPoint(0,0),s)));
                    pixmap.setMask(pixmap.createMaskFromColor(Qt::white));
                    pixmap.save(fpath);
                    delete edit;
                    mWindow->mImgMap[fpath] = pixmap;
                }
            }
            str =  QString("background-image: url(%1); %2").arg(fpath,str);
        }

    }





    if(!mbkColor.isEmpty())
    {
        str = QString("background-color: %1; %2").arg(mbkColor,str);
    }

    QString clsname = metaObject()->className();

    if(inherits(CN_NEWGRID))
    {
        ((NewGrid*)this)->mainScroll->setStyleSheet(QString("BaseScrollArea#%1 { %2 }").arg(
                                                        this->objectName(),
                                                        str));

    }
    else if(inherits(CN_NEWLIST))
    {
        //       ((NewList*)this)->mainScroll->setStyleSheet(QString("%1#%2 { %3 }").arg(clsname,
        //                                                 this->objectName(),
        //                                                 str));
        ((NewList*)this)->mainScroll->setStyleSheet(QString("QScrollArea#%1 { %2 }").arg(
                                                        this->objectName(),
                                                        str));

    }else{
        setStyleSheet(QString("BaseForm#%1 { %2 }").arg(this->objectName(),str));
    }

    //    qDebug() << " base form stylesheet " << this->styleSheet();
    update();
    repaint();

}

void BaseForm::paintEvent(QPaintEvent *ev)
{
    QStyleOption option;
    option.init(this);
    QPainter painter(this);

    style()->drawPrimitive(QStyle::PE_Widget,
                           &option, &painter, this);

}

BaseScrollArea::BaseScrollArea(QWidget *parent):
    QScrollArea(parent)
{

}

void BaseScrollArea::paintEvent(QPaintEvent *ev)
{
    QStyleOption option;
    option.init(this);
    QPainter painter(this);

    //  painter.setPen(QColor(0,0,0,0));

    style()->drawPrimitive(QStyle::PE_Widget,
                           &option, &painter, this);
}


void BaseForm::resizeEvent(QResizeEvent *event)
{
    // 这里要重写它的基类事件.
    updateGeometry();
    QWidget::resizeEvent(event);
    foreach (QWidget* nl, childlist) {
        nl->setMaximumSize(this->size());
    }
}

void BaseForm::onEnumItemChanged(QString txt)
{
    QComboBox *cb =(QComboBox *)(QObject::sender());
    if(!cb->objectName().compare(PIC_TEXT))
    {
        changeJsonValue(cb,cb->objectName(),mWindow->mOrderlist[cb->currentIndex()]);
    }else
        changeJsonValue(cb,cb->objectName(),txt);
}

void BaseForm::onColorButtonClicked()
{
    QPushButton *btn = (QPushButton*)QObject::sender();
    QColor c = QColor(btn->property(DKEY_COLOR).toString());
    // qDebug() << "old color " << c.name(QColor::HexArgb);
    QColorDialog *color =new  QColorDialog(c,this);
    color->setCurrentColor(c);

    color->setOptions( QColorDialog::DontUseNativeDialog |
                       QColorDialog::ShowAlphaChannel);
    color->exec();
    c =  color->selectedColor();

    if(c.isValid())
    {
        // qDebug() << " you selected color is " << c.rgb();
        QPixmap p(12,12);
        p.fill(c);
        btn->setIcon(p);
        btn->update();
        QString objname = btn->objectName();
        //   bool isFirst = !btn->property(DKEY_ARRIDX).toInt();
        if(!objname.compare(BAKCOLOR) ||
                !objname.compare(COLOR))
        {

            QString curcolor= c.name(QColor::HexArgb);
            if(!objname.compare(BAKCOLOR))
                mbkColor = curcolor;
            btn->setProperty(DKEY_COLOR,curcolor);

            changeJsonValue(btn,objname,curcolor);
            //  if(isFirst)
        }
        else if(!objname.compare(BORDER))
        {

            changeJsonValue(btn,BORDER,
                            QString("%1:%2").arg( GRAYCOLOR,c.name(QColor::HexArgb)));

            mBorderColor = c.name(QColor::HexArgb);

        }

        updateStyleSheets();
        QPushButton *cbtn = (QPushButton*)(getPairWidgetFromPLayout(btn));
        cbtn->setEnabled(true);

    }

}


void BaseForm::removeChild(QWidget *w)
{
    childlist.removeOne(w);
}


void BaseForm::initialEname()
{
    mEnameStr = getEnameFromJson(this->mOwerJson[PROPERTY].toArray());
    mEnameStr = getEnameSeq(mEnameStr,this);
    mPageIndex = mWindow->cManager->activeIndex();

    if(!mEnameStr.isEmpty())
    {
        mSeqEnameMap[mEnameStr] = this;
    }else{
        qDebug() << "Ename is empty";
    }
    mWindow->statusBar()->showMessage(QString("控件数量: %1").arg(QString::number(mSeqEnameMap.size())));
    if(BaseForm::mReadJson)
        initJsonValue();
}

QString BaseForm::updateEname(int index)
{
    mEnameStr =  mSeqEnameMap.key(this);
    if(mEnameStr.isEmpty())
    {
        mEnameStr = metaObject()->className()+ tr("_") + mUniqueStr.section('_',1,1);
    }
    mEnameStr =  getEnameSeq(mEnameStr,this);
    changeJsonValue(index,mEnameStr);
    return mEnameStr;

}

void BaseForm::onSelectMe()
{
    CanvasManager::mActiveSS->setSelectObject(this);
    if(mReadJson) return;

    mWindow->propertyWidget->createPropertyBox(this);
    if(!mParent->isContainer())
    {
        posWidget->updatePosition(this);
        posWidget->updateSize(this);
    }
    if(mType == T_NewLayer)
    {
        mLayout->setEnabled(true);
        setObjectTempEnabled(false);
    }else if(mType == T_NewLayout)
    {
        mLayout->setEnabled(true);
        setObjectTempEnabled(true);
    }else{
        mLayout->setEnabled(false);
        setObjectTempEnabled(false);
    }

}

void BaseForm::DeleteMe()
{
    QListIterator<QWidget*> item(childlist);
    while(item.hasNext())
    {
        QWidget *w = item.next();
        ((BaseForm*)w)->DeleteMe();
    }

    mWindow->tree->setMyParentNode();  //选中它的父控件.
    mWindow->tree->deleteItem(this);
    mObjectMap.remove(mUniqueStr);
    mSeqEnameMap.remove(mSeqEnameMap.key(this));
    mWindow->propertyWidget->delPropertyBox();
    // 它是否是列表控件的一员.
    deleteLater();
}

void BaseForm::initJsonValue()
{
    QColor c;
    QString cname = getJsonValue(BAKCOLOR).toString();
    c = QColor(cname) ;
    // if(isFirst)
    if(c.isValid())
    {

        mbkColor =c.name(QColor::HexArgb);
    }
    c = QColor(getJsonValue(GRAYCOLOR).toString());
    if(c.isValid())
        mBorderColor = c.name(QColor::HexArgb);

    //    mBorder =  getRectFromStruct(mOwerJson[PROPERTY].toArray(),BORDER);

    QString img = getJsonValue(BAKIMAGE).toString();
    if(!img.isEmpty())
    {
        img = QDir::currentPath() + BACKSLASH + img;
        mbkImage = img;
    }

    setGeometry(getRectFromStruct(mOwerJson[PROPERTY].toArray(),KEY_RECT));
}


QWidget* BaseForm::getPairWidgetFromPLayout(QWidget *sender)
{
    QWidget *w = 0;
    foreach (QHBoxLayout* hbp,
             sender->parentWidget()->findChildren<QHBoxLayout*>(sender->objectName())){
        if(hbp->indexOf(sender) > -1)
        {
            for(int i  =0;i < hbp->count() ;i++)
            {
                w =hbp->itemAt(i)->widget();
                if(w && w != sender)
                {
                    break;
                }
            }
            break;
        }
    }
    return w;
}

void BaseForm::onClearJsonValue()
{
    QWidget *sender  = (QWidget*)(QObject::sender());
    sender->setEnabled(false);

    QPushButton *btn = (QPushButton*)(getPairWidgetFromPLayout(sender));
    btn->setIcon(QIcon());
    sender->setProperty(DKEY_JSONIDX,btn->property(DKEY_JSONIDX));
    QString objname = sender->objectName();
    this->changeJsonValue(sender,objname,"");
    if(!objname.compare(BAKCOLOR))
    {
        mbkColor = "";
    }else if(!objname.compare(BAKIMAGE))
    {
        mbkImage = "";
    }else if(!objname.compare(BORDER))
    {
        Border *border= btn->parentWidget()->parentWidget()->findChild<Border *>();
        if(border)
            border->resetValues();
        mBorderColor = "";
    }
    updateStyleSheets();

}

void BaseForm::onActionDialog()
{
    QWidget *w  = (QWidget*)(QObject::sender());
    QJsonObject val = w->property(DKEY_JSONSTR).toJsonValue().toObject();
    if(!val[ACTION].isArray())
    {
        QMessageBox::warning(this,"格式错误",((QPushButton*)w)->text() + "格式错误,必须是数组类型");
        return;
    }

    ActionList *actlist = new ActionList(this);

    actlist->setFixedSize(this->mWindow->size() * 0.5);
    actlist->exec();


}

void BaseForm::onBackgroundImageDialog()
{
    QWidget *w  = (QWidget*)(QObject::sender());
    QString imgdir = mWindow->cManager->mProjectImageDir;

    ImageListView *imgview = new ImageListView(imgdir,this->mWindow);
    // 把一个动态属性传递给另一个事件发送对像,用来确定要修改JSON里的那一段值.
    imgview->imglist->setProperty(DKEY_ARRIDX,w->property(DKEY_ARRIDX));
    imgview->imglist->setProperty(DKEY_PARRIDX,w->property(DKEY_PARRIDX));
    imgview->imglist->setProperty(DKEY_OWERJSON,w->property(DKEY_OWERJSON));
    imgview->imglist->setProperty(DKEY_JSONIDX,w->property(DKEY_JSONIDX));


    QObject::connect(imgview->imglist,&QListWidget::itemDoubleClicked,
                     [=](QListWidgetItem *item){
        // QWidget *sender = imgview->imglist;
        QVariantMap vmap =  imgview->imglist->property(DKEY_IMGMAP).toMap();
        if(!vmap.isEmpty())
        {
            QString fpath = vmap[item->text()].toString();
            mbkImage = fpath;
            updateStyleSheets();
            changeJsonValue(imgview->imglist,
                            property(DKEY_CURVAL).toString(),
                            fpath.mid(mWindow->mRootPathLen));
            QPixmap p(12,12);
            p.load(fpath);
            ((QPushButton*)w)->setIcon(p);
        }
        QPushButton *btn = (QPushButton*)(getPairWidgetFromPLayout(w));
        btn->setEnabled(true);
    });

    imgview->setFixedSize(mWindow->size() * 0.6);
    imgview->exec();
}


void BaseForm::onListImageChanged(QString img)
{

    QWidget *w = (QWidget*)(QObject::sender());
    QString objname =w->objectName();
    QVariantList imglist = this->property(DKEY_IMAGELST).toList();
    foreach (QVariant v, imglist) {
        QString s = v.toString();
        QString k = s.section(SECTION_CHAR,0,0);
        if(!k.compare(img))
        {
            QString fpath = s.section(SECTION_CHAR,1,1);
            changeJsonValue(w,objname,fpath.mid(mWindow->mRootPathLen)); // 修改JSON里的值
            break;
        }
    }
}


int  BaseForm::tinySpinBoxDialog(QString  str,int val ,int min ,int max)
{
    int num = 0;

    BaseDialog *nWindow = new BaseDialog(this);
    nWindow->setObjectName(metaObject()->className());
    nWindow->UpdateStyle();
    QVBoxLayout *vb = new QVBoxLayout();

    QHBoxLayout *hb = new QHBoxLayout();

    QLabel *title = new QLabel(str);
    QSpinBox *spinbox = new QSpinBox();
    spinbox->setMaximum(max);
    spinbox->setToolTip(QString("请输入%1~%2的整数").arg(QString::number(min),QString::number(max)));
    spinbox->setValue(val);

    hb->addWidget(title);
    hb->addWidget(spinbox);
    vb->addLayout(hb);
    QDialogButtonBox *dbb = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                                                 Qt::Horizontal,
                                                 nWindow);
    dbb->button(QDialogButtonBox::Ok)->setText("确定");
    dbb->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(dbb,SIGNAL(accepted()),nWindow,SLOT(accept()));
    connect(dbb,SIGNAL(rejected()),nWindow,SLOT(reject()));
    vb->addWidget(dbb);
    nWindow->setLayout(vb);
    nWindow->exec();

    int ret = nWindow->result();
    if(ret)
        num = spinbox->value();
    nWindow->deleteLater();
    return num;

}

QSize  BaseForm::ChangeSizeDialog(QSize size)
{
    int w,h = 0;
    while(1)
    {

        BaseDialog *nWindow = new BaseDialog(this);
        nWindow->setObjectName(metaObject()->className());
        nWindow->UpdateStyle();
        QGridLayout *glayout = new QGridLayout();

        QLabel *wt = new QLabel("宽:");
        QLabel *ht= new QLabel("高:");

        QString tooltip = "请输入 1000 < 0 内的整数";
        QSpinBox *wbox = new QSpinBox();
        QSpinBox *hbox = new QSpinBox();
        wbox->setMaximum(999);
        wbox->setToolTip(tooltip);
        wbox->setValue(size.width());

        hbox->setMaximum(999);
        hbox->setToolTip(tooltip);
        hbox->setValue(size.height());

        glayout->addWidget(wt,0,0);
        glayout->addWidget(wbox,0,1);
        glayout->addWidget(ht,1,0);
        glayout->addWidget(hbox,1,1);
        QDialogButtonBox *dbb = new QDialogButtonBox(QDialogButtonBox::Ok|
                                                     QDialogButtonBox::Cancel,
                                                     Qt::Horizontal,
                                                     nWindow);
        dbb->button(QDialogButtonBox::Ok)->setText("确定");
        dbb->button(QDialogButtonBox::Cancel)->setText("取消");
        connect(dbb,SIGNAL(accepted()),nWindow,SLOT(accept()));
        connect(dbb,SIGNAL(rejected()),nWindow,SLOT(reject()));
        glayout->addWidget(dbb,2,0,1,0);
        nWindow->setLayout(glayout);
        nWindow->exec();

        int ret = nWindow->result();
        w = wbox->value();
        h = hbox->value();
        nWindow->deleteLater();
        if(ret)
        {
            if(!w || !h)
            {
                continue;
            }
            break;
        }
    }

    return QSize(w,h);
}

void BaseForm::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

//void BaseForm::dragEnterEvent(QDragEnterEvent *e)
//{
//    qDebug() << " drag enter is " << e->pos() << this->objectName() << this->metaObject()->className();
//    e->accept();
//}

//void BaseForm::dropEvent(QDropEvent *e)
//{

//    switch (mType) {
//    case T_NewLayout:
//    {
//        if(mWindow->cManager->activeSS()->activeObject() == this)
//            e->acceptProposedAction();
//        else
//            return;

//        QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
//        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//        QPixmap pixmap;
//        QPoint offset;

//        QVariant qv;
//        int flag ;
//        dataStream >> flag  >> qv >> pixmap >> offset;
//        if(static_cast<int>(ObjTypes::T_NewLayer)  == flag){
//            ScenesScreen *ss = mWindow->cManager->activeSS();
//            if(!ss)
//                return;
//            ss->createNewLayer( QJsonValue::fromVariant(qv),true);
//        }else{
//            QJsonValue val = QJsonValue::fromVariant(qv);
//            ((NewLayout*)this)->readFromJson(val,true);
//        }
//    }
//        break;
//    case T_NewLayer:
//    {
//        QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
//        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//        QPixmap pixmap;
//        QPoint offset;

//        QVariant qv;
//        int flag ;
//        dataStream >> flag  >> qv >> pixmap >> offset;
//        if(mWindow->cManager->activeSS()->activeObject() == this)
//            e->acceptProposedAction();
//        else
//            return;
//        if(static_cast<int>(ObjTypes::T_NewLayout)  == flag)
//        {
//            ((NewLayer*)this)->readLayoutFromJson(QJsonValue::fromVariant(qv),true);
//        }else{
//            QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
//        }
//    }
//        break;
//    default:
//    {
//        if(mWindow->cManager->activeSS()->activeObject() == mParent)
//            e->acceptProposedAction();
//        else
//            return;
//        QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
//        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//        QPixmap pixmap;
//        QPoint offset;

//        QVariant qv;
//        int flag ;
//        dataStream >> flag  >> qv >> pixmap >> offset;
//        ((NewLayout*)mParent)->readFromJson(qv.toJsonValue(),true);
//     }
//        break;
//    }

//}

NewLayout *BaseForm::CreateNewLayout(const QJsonValue &qv,
                                     QWidget *parent,bool isCreate,bool incontainer)
{

    QJsonObject  valobj = qv.toObject();
    QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // QVariant variant = valobj.value(PROPERTY).toVariant();
    //    qDebug() << " whois call CreateNewLayout" << this->objectName() << this->metaObject()->className();
    NewLayout *newlayout = new NewLayout(valobj,oldrect,mWindow,parent);
    newlayout->mParent = this;
    //    if(incontainer)
    //        newlayout->setProperty(DKEY_INTOCONTAINER,incontainer);
    newlayout->mCreateFlag = isCreate;
    newlayout->setProperty(DKEY_TYPE, valobj[WTYPE].toString());
    newlayout->mOwerJson = qv.toObject();
    newlayout->initialEname();
    mWindow->tree->addObjectToCurrentItem(mUniqueStr,newlayout);

    //    newlayout->show();
    newlayout->onSelectMe();
    newlayout->updateStyleSheets();
    //    newlayout->show();
    return newlayout;
}



void BaseForm::addChildrenToTree()
{
    foreach (QWidget *w, childlist) {
        mWindow->tree->addObjectToCurrentItem(mUniqueStr,w);
        if(!w->isHidden())
            ((BaseForm*)w)->addChildrenToTree();
    }
}



QJsonObject BaseForm::ContainerWriteToJson(QWidget *w)
{
    QJsonObject outjson =  QJsonValue::fromVariant(w->property(DKEY_JSONSTR)).toObject();
    outjson[NAME] = w->objectName();

    if(w->inherits(CN_NEWLAYOUT))
    {
        NewLayout *layout =  (NewLayout*)w;
        QJsonArray parray = layout->mOwerJson[PROPERTY].toArray();

        for(int n = 0; n < parray.count() ; n++)
        {
            QJsonObject obj = parray.at(n).toObject();
            if(obj.contains(STRUCT))
            {
                // 容器的里的对像的每一个的CSS属性要改成一样的.测试之后再确定
                QJsonObject rectobj=  getRectJson(layout)[KEY_RECT].toObject();
                QJsonArray structArray = obj[STRUCT].toArray();
                for(int snum = 0; snum < structArray.count();snum++)
                {
                    QJsonArray cssarray = structArray.at(snum).toArray();
                    for( int num =0;num < cssarray.count();num++)
                    {
                        QJsonObject cobj = cssarray.at(num).toObject();
                        if(cobj.contains(KEY_RECT))
                        {
                            cobj[KEY_RECT]= rectobj;
                            cssarray.replace(num,cobj);
                            structArray.replace(snum,cssarray);
                            break;
                        }
                    }
                }
                obj[STRUCT] = structArray;
                parray[n] = obj;
                break;
            }
        }
        layout->mOwerJson[PROPERTY] = parray;
    }
    outjson = ((NewFrame*)w)->writeToJson();
    //     outjson = ((NewLayout*)w)->writeToJson();
    return outjson;
}


void BaseForm::initObject(const QJsonObject &json)
{
    QString caption = json[CAPTION].toString();
    setProperty(DKEY_TXT,caption);
    QString uname = QString("%1_%2").arg(caption,
                                         QString::number(mObjectMap.size()));
    mUniqueStr = getSequence(uname);
    setObjectName(mUniqueStr);
    setToolTip(mUniqueStr);
}


QString BaseForm::getSequence(const QString &key)
{
    QString t = key.section('_',0,0);
    int n = key.section('_',1,1).toInt();
    QString tkey = key;
    while(mObjectMap.contains(tkey))
    {
        tkey = QString("%1_%2").arg(t,
                                    QString::number(++n));
    }
    return tkey;
}
QString BaseForm::getEnameSeq(const QString &key,QWidget* obj)
{
    auto a_lambda_func = [=](const QString &k,QWidget *o) {
        QString tmp = mSeqEnameMap.key(o);
        if(!tmp.isEmpty())
        {
            // 找到这个对像已经有一个名字,
            if(tmp.compare(k))
            {
                mSeqEnameMap.remove(tmp);
                mSeqEnameMap[k] = o;
            }
        }
        else{
            mSeqEnameMap[k] = o;
        }
    };

    if(key.contains('_'))
    {
        QString t = key.section('_',0,0);
        int n = key.section('_',1,1).toInt();
        QString tkey = key;
        if(t.isEmpty())
        {
            t = obj->metaObject()->className();
        }
        while(mSeqEnameMap.contains(tkey) )
        {

            QWidget *exist = mSeqEnameMap.value(tkey);
            if(exist == obj)
            {
                mSeqEnameMap[tkey] = obj;
                break;
            }
            else
                tkey = QString("%1_%2").arg(t,
                                            QString::number(++n));
        }

        a_lambda_func(tkey,obj);

        return tkey;
    }else{
        int n = 0;
        QString tkey = "";
        QString t = "";
        if(key.isEmpty())
        {
            tkey = t = obj->metaObject()->className();
        }else
            tkey  =  t= key;


        while(mSeqEnameMap.contains(tkey) )
        {
            QWidget *exist = mSeqEnameMap.value(tkey);
            if(exist == obj){
                mSeqEnameMap[tkey] = obj;
                break;
            }
            else
                tkey = QString("%1_%2").arg(t,
                                            QString::number(++n));
        }

        a_lambda_func(tkey,obj);
        return tkey;
    }

}

void BaseForm::setObjectTempEnabled(bool f)
{
    foreach (QWidget *w, mObjectTemplte) {
        w->setEnabled(f);
    }
}

NewFrame::NewFrame(const QJsonObject &json, QWidget *parent)
//  :FormResizer(parent),Compoent()
    :BaseForm(parent)
{

    mType = T_NewFrame;
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    //setObjectName(CN_NEWFRAME);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setFocusPolicy(Qt::ClickFocus);
    initObject(json);
}

void NewFrame::clearOtherObjectStyleSheet()
{
    /* 清除控件的红线框 */
    QList<NewFrame*> nflist =  this->parentWidget()->findChildren<NewFrame*>();
    foreach(NewFrame *x,nflist)
    {
        x->setStyleSheet("");
    }
}


void NewFrame::onDeleteMe()
{
    DeleteMe();
}

QJsonObject NewFrame::writeToJson()
{
    QJsonObject json =  mOwerJson;
    // QJsonArray projson;// 属性
    json[NAME] = objectName();
    json[PROPERTY] = mOwerJson[PROPERTY];
    json[CAPTION] = this->property(DKEY_TXT).toString();
    return json;
}



NewGrid::NewGrid(const QJsonValue &qv,
                 const QList<int> *arglist, QWidget *parent)
    :BaseForm(parent),
      mainWidget(new QWidget()),
      sliderOrientation(Qt::Vertical),
      rows(arglist->at(0)),
      mainScroll(new BaseScrollArea(m_frame)),
      cols(arglist->at(1)),itemSize(arglist->at(2),arglist->at(3))
{
    mType= T_NewGrid;
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    QJsonObject obj = qv.toObject();
    initObject(obj);
    setFocusPolicy(Qt::ClickFocus);

    mainScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setAlignment(Qt::AlignCenter);
    // mainScroll->setWidgetResizable(true);


    mainScroll->setWidget(mainWidget);
    mainScroll->setContentsMargins(0,0,0,0);

    gridLayout = new QGridLayout(mainWidget);
    gridLayout->setSpacing(obj[ITEMSPACE].toInt());
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainScroll->setObjectName(mUniqueStr);
    mainWidget->setObjectName(mUniqueStr);  // 用来识别布局是否在列表控件下面.
    mainWidget->setMaximumSize(999,999);
    mainWidget->setContentsMargins(0,0,0,0);

    QRect oldrect = getRectFromStruct(obj[PROPERTY].toArray(),KEY_RECT);
    gridLayout->setGeometry(oldrect);
    setGeometry(oldrect);
    resize(oldrect.size());
    mainWidget->resize(oldrect.size());
    show();

    mOwerJson = qv.toObject();

    menuAddRow =  new QAction(QIcon(":/icon/icons/addtab.png"),"添加行",this);
    connect(menuAddRow,SIGNAL(triggered(bool)),SLOT(onAddOneRow()));

    menuAddCol =  new QAction(QIcon(":/icon/icons/addtab.png"),"添加列",this);
    connect(menuAddCol,SIGNAL(triggered(bool)),SLOT(onAddOneCol()));

    menuSpace = new QAction(QIcon(":/icon/icons/same-width.png"),"单元间距",this);

    connect(menuSpace,&QAction::triggered,[=](){
        gridLayout->setSpacing(tinySpinBoxDialog(menuSpace->text(),
                                                 gridLayout->spacing(),1,99));
    });

    menuSize = new QAction(QIcon(":/icon/icons/scale-icon.png"),"单元尺寸",this);
    connect(menuSize,&QAction::triggered,[=](){
        itemSize.scale(ChangeSizeDialog(itemSize),Qt::IgnoreAspectRatio);
        updateAllItemsSize();
    });


    menuOrientation = new QActionGroup(this);
    menuV = new QAction(/*QIcon(":/icon/icons/same-height.png"),*/"垂直滚动",this);
    menuH = new QAction(/*QIcon(":/icon/icons/same-width.png"),*/"水平滚动",this);
    menuOrientation->addAction(menuV);
    menuOrientation->addAction(menuH);
    menuV->setCheckable(true);
    menuH->setCheckable(true);
    menuV->setChecked(true);

    connect(menuOrientation,&QActionGroup::triggered,[=](QAction *a)
    {
        sliderOrientation = a == menuV ? Qt::Vertical : Qt::Horizontal;
    });

    QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonRelease,QCursor::pos(),
                                         Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
    QApplication::postEvent(this,event);
}


void NewGrid::addRowsCols()
{

    for(int x = 0 ; x < rows ;x++)
        for(int y = 0;y  < cols ; y++)
            initRowsCols(x,y,
                         QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));

    updateAllItemsSize();
}

void NewGrid::initRowsCols(int row,int col,const QJsonValue &value)
{

    onSelectMe();
    NewLayout *nl = CreateNewLayout(value,mainWidget,mCreateFlag,true);
    //    nl->container = this;

    childlist.append(nl);
    // 自已的行列坐标.
    nl->setProperty(DKEY_ROW,row);
    nl->setProperty(DKEY_COL,col);
    gridLayout->addWidget(nl,row,col);

}

void NewGrid::onAddOneRow()
{
    mCreateFlag = true;
    mReadJson = true;
    QAction *a = (QAction*)(QObject::sender());
    int num = tinySpinBoxDialog(a->text(),1,0,99);
    for(int x = 0 ; x < num ;
        x++,rows++)
    {
        for(int i =0;i<cols ;i++)
        {
            initRowsCols(rows,i,QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));
        }
    }

    updateAllItemsSize();
    mReadJson = false;
    ((NewGrid*)childlist.last())->onSelectMe();

}

void NewGrid::onAddOneCol()
{
    mCreateFlag = true;
    mReadJson = true;
    QAction *a = (QAction*)(QObject::sender());
    int num = tinySpinBoxDialog(a->text(),1,0,99);
    for(int x = 0 ; x < num;
        x++,cols++)
    {
        for(int i =0;i<rows ;i++)
        {
            initRowsCols(i,cols,QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));
        }
    }
    updateAllItemsSize();
    mReadJson = false;
    ((NewGrid*)childlist.last())->onSelectMe();

}

void NewGrid::updateAllItemsSize()
{
    foreach (QWidget *w, childlist) {
        ((NewLayout*)w)->setMaximumSize(this->size());
        ((NewLayout*)w)->setFixedSize(itemSize);
        ((BaseForm*)w)->onSelectMe();
        if(mReadJson)
            continue;
        ((BaseForm*)w)->changeJsonValue(((BaseForm*)w)->posWidget,
                                        KEY_RECT,
                                        QString("%1:%2").arg(WIDTH,
                                                             QString::number(itemSize.width())));
        ((BaseForm*)w)->changeJsonValue(((BaseForm*)w)->posWidget,
                                        KEY_RECT,
                                        QString("%1:%2").arg(HEIGHT,
                                                             QString::number(itemSize.height())));

    }
    onSelectMe();

}

void NewGrid::onDeleteMe()
{

    if(childlist.size())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("删除提示");
        QString msg = QString("你真的要删除当前%1吗?删除之后不可以撤消,请选择<删除>删除.").arg(this->property(DKEY_TXT).toString());
        msgBox.setText(msg);
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"删除");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //qDebug() << " QMessageBox result " << ret;
        if(ret == QMessageBox::Yes)
        {

            DeleteMe();
        }
    }else
    {
        DeleteMe();
    }

}

//void NewGrid::mouseReleaseEvent(QMouseEvent *event)
//{

//    mainScroll->resize(this->size());
//    mainWidget->resize(this->size());
//    BaseForm::mouseReleaseEvent(event);

//}

void NewGrid::wheelEvent(QWheelEvent *event)
{

    if(sliderOrientation == Qt::Horizontal)
    {
        if(event->orientation() != Qt::Horizontal)
        {
            QWheelEvent  *evt = new QWheelEvent(event->pos(),event->globalPos(),
                                                event->delta(),
                                                event->buttons(),
                                                event->modifiers(),Qt::Horizontal);
            QApplication::postEvent(mainScroll->horizontalScrollBar(),evt);
        }
    }

}

void NewGrid::readFromJson(const QJsonValue &value)
{

    QJsonObject obj = value.toObject();
    int row = obj[JROW].toInt();
    int col = obj[JCOL].toInt();

    initRowsCols(row,col,value);
    NewLayout *nl = (NewLayout*)(childlist.last());


    // 这里一定是Layout 嵌套了.
    foreach (QJsonValue item, obj[LAYOUT].toArray()) {
        nl->readFromJson(item,mCreateFlag);
    }
}

QJsonObject NewGrid::writeToJson()
{
    QJsonArray layoutarr;
    //QJsonObject json = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    QJsonObject json = mOwerJson;

    foreach (QWidget *w, childlist) {

        // 这里只有两种对像类型,NewLayout , NewFrame;

        // outjson = ((BaseForm*)w)->writeToJson();
        QJsonObject outjson = ContainerWriteToJson(w);
        outjson[JCOL] = w->property(DKEY_COL).toInt();
        outjson[JROW] = w->property(DKEY_ROW).toInt();
        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LISTWIDGET] = layoutarr;
    json[ITEMSPACE] = gridLayout->spacing();
    json[GCOLS] = cols;
    json[GROWS] = rows;
    json[WIDTH] = itemSize.width();
    json[HEIGHT] = itemSize.height();
    json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}

NewList::NewList(const QJsonValue &json, const QSize size, QWidget *parent):
    //FormResizer(parent)
    BaseForm(parent),

    sliderOrientation(Qt::Vertical),
    mainScroll(new QScrollArea(m_frame)),
    //   mainScroll(new ContainerScroll(m_frame)),
    mainWidget(new QWidget())

{
    mType = T_NewList;
    mWindow = ((NewLayout*)parent->parentWidget())->mWindow;
    setProperty(DKEY_JSONSTR,json.toVariant());
    QJsonObject obj = json.toObject();
    //    QString caption = obj[CAPTION].toString();
    initObject(obj);

    QString Orientation = obj[QRIENTATION].toString();
    // itemHeight = this->height() * 0.3;

    //setFocusPolicy(Qt::ClickFocus);

    mainScroll->setFocusPolicy(Qt::WheelFocus);
    mainScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScroll->setAlignment(Qt::AlignCenter);
    mainScroll->setContentsMargins(0,0,0,0);


    //    mainWidget->setObjectName(LISTWIDGET);  // 用来识别布局是否在列表控件下面.
    mainWidget->setContentsMargins(0,0,0,0);
    mainScroll->setWidget(mainWidget);


    menuSetSpace = new QAction(QIcon(":/icon/icons/horizontal-scale-icon.png"),"设置间隔",this);
    if(!Orientation.compare(HORIZONTAL,Qt::CaseInsensitive))
    {
        sliderOrientation = Qt::Horizontal;
        listLayout = new QHBoxLayout(mainWidget);
        menuSetHeight =  new QAction(QIcon(":/icon/icons/fileoverlay_ui@2x.png"),"设置列宽",this);
        menuAddLine = new QAction(QIcon(":/icon/icons/addtab.png"),"添加列",this);
        mainScroll->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    }else
    {
        menuAddLine = new QAction(QIcon(":/icon/icons/addtab.png"),"添加行",this);
        menuSetHeight =  new QAction(QIcon(":/icon/icons/fileoverlay_ui@2x.png"),"设置行高",this);
        listLayout = new QVBoxLayout(mainWidget);
        mainScroll->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        sliderOrientation = Qt::Vertical;
    }
    mainWidget->setSizePolicy(mainScroll->sizePolicy());

    listLayout->setSpacing(obj[ITEMSPACE].toInt());
    itemHeight = obj[ITEMSIZE].toInt();

    connect(menuAddLine,SIGNAL(triggered(bool)),SLOT(onAddManyLine()));
    connect(menuSetHeight,SIGNAL(triggered(bool)),SLOT(onSetFixedHeight()));
    connect(menuSetSpace,&QAction::triggered,[=]{
        listLayout->setSpacing( tinySpinBoxDialog(menuSetSpace->text(),
                                                  listLayout->spacing(),0,99));
        listLayout->update();
    });

    listLayout->setMargin(0);

    listLayout->setContentsMargins(0,0,0,0);
    listLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    mainWidget->setObjectName(mUniqueStr);
    mainScroll->setObjectName(mUniqueStr);
    mainWidget->setMaximumSize(999,999);
    resize(size);
    QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseButtonRelease,
                                         QCursor::pos(),
                                         Qt::LeftButton,
                                         Qt::LeftButton,
                                         Qt::NoModifier);
    QApplication::postEvent(this,event);
    show();
}


void NewList::updateOneItem(QWidget *w,int width,int height)
{
    ((BaseForm*)w)->onSelectMe();

    w->setFixedHeight(height);
    w->setFixedWidth(width);
    if(((BaseForm*)w)->posWidget)
    {
        ((BaseForm*)w)->changeJsonValue(((BaseForm*)w)->posWidget,
                                        KEY_RECT,
                                        QString("%1:%2").arg(HEIGHT,
                                                             QString::number(height)));
        ((BaseForm*)w)->changeJsonValue(((BaseForm*)w)->posWidget,
                                        KEY_RECT,
                                        QString("%1:%2").arg(WIDTH,
                                                             QString::number(width)));

    }
}

void NewList::updateAllItemsSize()
{

    foreach (QWidget *w, childlist) {
        ((NewLayout*)w)->setMaximumSize(this->size());
        if(sliderOrientation == Qt::Horizontal)
        {
            if(itemHeight == w->width() &&
                    w->height() == this->height())
            {
                //                 qDebug() << " widget it's size "  << w->size() << " name is "
                //                          << w->objectName() << w->metaObject()->className();
                break;
            }
            updateOneItem(w,itemHeight,this->height());
        }

        else
        {
            if(itemHeight == w->height() &&
                    w->width() == this->width())
            {
                //                 qDebug() << " widget it's size "  << w->size() << " name is "
                //                          << w->objectName() << w->metaObject()->className();
                break;
            }
            updateOneItem(w,this->width(),itemHeight);
        }

    }

    onSelectMe();
}

void NewList::onSetFixedHeight()
{
    QString txt =((QAction*)(QObject::sender()))->text();
    int num = tinySpinBoxDialog(/*"设置每行的高度:"*/txt,itemHeight,2,999);
    if(num)
    {
        itemHeight = num;
        updateAllItemsSize();
    }

}


void NewList::onAddManyLine()
{
    QString txt =((QAction*)(QObject::sender()))->text();
    int num = tinySpinBoxDialog(txt,1,1,99);
    mReadJson = true;
    for(int i = 0; i < num;i++)
    {
        AddOneLine(QJsonValue::fromVariant(mWindow->ComCtrl->mVariantLayout));
    }
    mReadJson = false;
    ((NewList*)childlist.last())->onSelectMe();
}

void NewList::pasteOneLine(const QJsonValue &value)
{
    NewLayout *nlayout = AddOneLine(value);
    QJsonObject  valobj = value.toObject();
    foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
        nlayout->readFromJson(item,true);
    }
}

NewLayout * NewList::AddOneLine(const QJsonValue &value)
{
    onSelectMe();

    NewLayout *newlayout = CreateNewLayout(value,mainWidget,mCreateFlag,true);
    onSelectMe();

    newlayout->onSelectMe();
    childlist.append(newlayout);
    //    newlayout->container = this;

    listLayout->addWidget(newlayout);
    if(sliderOrientation == Qt::Horizontal)
    {
        updateOneItem(newlayout,itemHeight,this->height());
    }else{
        updateOneItem(newlayout,this->width(),itemHeight);
    }
    return newlayout;
}


void NewList::onDeleteMe()
{
    if(childlist.size())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("删除提示");
        QString msg = QString("你真的要删除当前%1吗?删除之后不可以撤消,请选择<删除>删除.").arg(this->property(DKEY_TXT).toString());
        msgBox.setText(msg);
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"删除");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //qDebug() << " QMessageBox result " << ret;
        if(ret == QMessageBox::Yes)
        {

            DeleteMe();
        }
    }else
    {
        DeleteMe();
    }
}


//void NewList::mouseReleaseEvent(QMouseEvent *event)
//{

//    // 把　QScrollArea　的大小调成与父控件一样大.
//    mainScroll->resize(this->size());
//    updateAllItemsSize();
////    BaseForm::mouseReleaseEvent(event);
//    event->accept();

//}


void NewList::wheelEvent(QWheelEvent *event)
{


    if(sliderOrientation == Qt::Horizontal)
    {
        if(event->orientation() != Qt::Horizontal)
        {
            QWheelEvent  *evt = new QWheelEvent(event->pos(),event->globalPos(),
                                                event->delta(),
                                                event->buttons(),
                                                event->modifiers(),Qt::Horizontal);

            QApplication::postEvent(mainScroll->horizontalScrollBar(),evt);
        }


    }

}


bool NewList::eventFilter(QObject *obj, QEvent *event)
{

    if(obj->inherits(CN_NEWLAYOUT) && event->type() == QEvent::Wheel)
    {

        // wheelEvent(&evt);
        QWheelEvent* w = static_cast<QWheelEvent*>(event);

        if(sliderOrientation == Qt::Horizontal)
        {
            QWheelEvent  evt(w->pos(),w->globalPos(),this->width()/4,
                             w->buttons(),
                             w->modifiers(),Qt::Horizontal);

            obj->removeEventFilter(this);
            QApplication::sendEvent(mainScroll,&evt);
            obj->installEventFilter(this);
        }else
        {
            QWheelEvent  evt(w->pos(),w->globalPos(),this->width()/4,
                             w->buttons(),
                             w->modifiers(),Qt::Vertical);

            obj->removeEventFilter(this);
            QApplication::sendEvent(mainScroll,&evt);
            obj->installEventFilter(this);
        }

        return true;
    }
    return BaseForm::eventFilter(obj,event);
}



QJsonObject NewList::writeToJson()
{
    QJsonArray layoutarr;
    //  QJsonObject json = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    QJsonObject json = mOwerJson;

    foreach (QWidget *w, childlist) {

        // qDebug() << " childlist  " << w->metaObject()->className();
        // 这里只有两种对像类型,NewLayout , NewFrame;
        layoutarr.append(ContainerWriteToJson(w));
    }
    // 这一句必需要在这个偱环后面.
    json[LISTWIDGET] = layoutarr;
    json[ITEMSPACE] = listLayout->spacing();
    json[ITEMSIZE] = itemHeight;
    json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}


void NewList::readFromJson(const QJsonValue &qv)
{
    QJsonObject valobj = qv.toObject();
    //QRect oldrect = getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = valobj[CLASS].toString();
    if(!clsName.compare(CN_NEWLAYOUT) || !clsName.compare(LAYOUT))
    {
        NewLayout *nl = AddOneLine(valobj);
        // nl->setGeometry(oldrect);
        // 这里一定是Layout 嵌套了.
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            nl->readFromJson(item,this->mCreateFlag);
        }
        m_frame->adjustSize();
    }

}


NewLayout::NewLayout(const QJsonObject &json, QRect rect,
                     MainWindow *w, QWidget *parent):
    BaseForm(parent)
{
    mType = T_NewLayout;
    this->mWindow = w;


    //    qDebug() << "this layout size " << size()
    //             <<" parent size " << parent->size();
    if(parent)
        setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.

    setFocusPolicy(Qt::ClickFocus);
    //    setProperty(DKEY_INTOCONTAINER,false);
    show();
    setGeometry(rect);
    update();
    initObject(json);
}



void NewLayout::DeleteMe()
{
    // 它是否是列表控件的一员.
    if(mParent->mType == T_NewGrid ||
            mParent->mType == T_NewList)
    {
        mParent->childlist.removeOne(this);
    }
    this->BaseForm::DeleteMe();
}



void NewLayout::onDeleteMe()
{
    //这里不能绕过它的父控件.
    //delMySelf();
    // mWindow->cManager->activeSS()->delSelectedLayout();
    //删除布局控件
    QMessageBox msgBox;
    msgBox.setWindowTitle("删除提示");
    msgBox.setText("你真的要删除当前布局吗?删除之后不可以撤消,请选择<删除>删除.");
    // msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Yes,"删除");
    msgBox.setButtonText(QMessageBox::Cancel,"取消");
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    //qDebug() << " QMessageBox result " << ret;
    if(ret == QMessageBox::Yes)
    {
        DeleteMe();
    }

}

void NewLayout::onBeComeTemplateWidget()
{
    QString widgetdir = mWindow->mGlobalSet->value(INI_PRJCUSTOM).toString();
    if(widgetdir.isEmpty()) widgetdir = QDir::currentPath() + BACKSLASH + "widgets";

    QDir wdir(widgetdir );
    if(!wdir.exists(widgetdir))
        wdir.mkdir(widgetdir);
    QString fpath;
    QString name;
    while(1)
    {
        BaseDialog *nWindow = new BaseDialog(this);
        nWindow->setObjectName("fdialog");
        nWindow->UpdateStyle();
        QVBoxLayout *vb = new QVBoxLayout();
        QLineEdit *nametxt =  new QLineEdit();
        QString deftxt = "请输入控件名称";
        nametxt->setText(deftxt);
        nametxt->selectAll();
        vb->addWidget(nametxt);

        QDialogButtonBox *dbb = new QDialogButtonBox(QDialogButtonBox::Ok|
                                                     QDialogButtonBox::Cancel,
                                                     Qt::Horizontal,
                                                     nWindow);
        dbb->button(QDialogButtonBox::Ok)->setText("确定");
        dbb->button(QDialogButtonBox::Cancel)->setText("取消");
        connect(dbb,SIGNAL(accepted()),nWindow,SLOT(accept()));
        connect(dbb,SIGNAL(rejected()),nWindow,SLOT(reject()));
        vb->addWidget(dbb);
        nWindow->setLayout(vb);
        nWindow->exec();

        int ret = nWindow->result();
        name = nametxt->text();
        fpath = widgetdir + BACKSLASH + name;

        nWindow->deleteLater();
        if(ret)
        {
            if(nametxt->text().isEmpty() || !deftxt.compare(nametxt->text()))
            {
                continue;
            }
            break;
        }

        else
            return;
    }


    // 创建一个为当前控件的30%大小的ICON图片.
    QSize iconsize(size());
    QPixmap pixmap(iconsize);
    render(&pixmap,QPoint(),QRegion(rect()));
    pixmap.scaled(size() * 0.3).save(fpath+".png");
    //   pixmap.save(fpath + ".png");

    QJsonObject json ;

    QJsonObject wid = writeToJson();

    wid[ICON] = fpath + ".png";

    QJsonArray property = wid[PROPERTY].toArray();
    for (int i =0 ;i < property.size();i++) {
        QJsonObject tt = property.at(i).toObject();
        if(tt.contains(KEY_RECT))
        {
            QJsonObject rect = tt[KEY_RECT].toObject();
            QVariantMap vmap ;
            vmap[LX] = 0;
            vmap[LY] = 0;
            vmap[WIDTH] =rect[WIDTH].toInt();
            vmap[HEIGHT] = rect[HEIGHT].toInt();
            tt[KEY_RECT] = QJsonObject::fromVariantMap(vmap);

            property[i] = tt;
            break;
        }


    }
    wid[CAPTION] = name;
    QJsonArray arr ;

    wid[PROPERTY] = property;
    arr.append(wid);
    json[COMPOENTS] = arr;

    QFile saveFile;
    saveFile.setFileName(fpath + ".json");
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    }
    QJsonDocument jsonDoc(json);
    saveFile.write(jsonDoc.toJson());
    // 加载到当前工程里面去.
}

void NewLayout::clearOtherObjectStyleSheet()
{
    /* 清除控件的红线框 */
    QList<NewFrame*> nflist =  m_frame->findChildren<NewFrame*>();
    foreach(NewFrame *x,nflist)
    {
        x->setStyleSheet("");
    }
}

void NewLayout::clearOtherSelectHandler()
{

    QWidgetList laylist = this->parentWidget()->findChildren<QWidget*>();
    foreach (QWidget *wid, laylist) {

        if (wid == this)
            continue;
        if(wid->inherits(CN_NEWFRAME) || wid->inherits(CN_NEWLAYOUT))
        {
            ((FormResizer*)wid)->setState(SelectionHandleOff);
        }
    }
}

void NewLayout::deleteObject(int index)
{
    QWidget *w = childlist.at(index);
    childlist.removeAt(index);
    ((NewFrame*)w)->DeleteMe();
}

void NewLayout::deleteObject(QWidget *w)
{
    int i = childlist.indexOf((BaseForm*)w);
    childlist.removeAt(i);
    if(w->inherits(CN_NEWLAYOUT))
    {
        ((NewLayout*)w)->DeleteMe();
    }else {
        ((NewFrame*)w)->DeleteMe();
    }
}


QJsonObject NewLayout::writeToJson()
{

    QJsonArray layoutarr;
    QJsonObject json = mOwerJson;
    foreach (QWidget *w, childlist) {
        QJsonObject outjson = QJsonValue::fromVariant( w->property(DKEY_JSONSTR)).toObject();
        // 这里只有两种对像类型,NewLayout , NewFrame;
        //        QString clsname = w->metaObject()->className();
        outjson = ((BaseForm*)w)->writeToJson();
        layoutarr.append(outjson);
    }
    // 这一句必需要在这个偱环后面.
    json[LAYOUT] = layoutarr;
    json[NAME] = objectName();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}


void NewLayout::readFromJson(const QJsonValue &qv,bool flag)
{
    QJsonObject valobj = qv.toObject();
    // QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    // 在NewLayout　类上面可以创建包括它自已在内的三种类型的对像.
    QString clsName = valobj[CLASS].toString();
    if(!clsName.compare(CN_NEWLAYOUT)/* || !clsName.compare(CN_LAYOUT)*/)
    {

        NewLayout *newlayout = CreateNewLayout(qv,m_frame,flag,false);
        childlist.append(newlayout);
        // 这里一定是Layout 嵌套了.
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            newlayout->readFromJson(item,flag);
        }
    }else if(!clsName.compare(CN_NEWLIST))
    {
        NewList *nlist = new NewList(qv,QSize(10,10),m_frame);
        nlist->mParent = this;
        nlist->mCreateFlag = flag;
        nlist->mOwerJson = qv.toObject();
        nlist->initialEname();
        mWindow->tree->addObjectToCurrentItem(mUniqueStr,nlist);
        childlist.append(nlist);
        nlist->onSelectMe();

        foreach (QJsonValue item, valobj[LISTWIDGET].toArray()) {
            nlist->readFromJson(item);
        }

        nlist->updateAllItemsSize();
        nlist->updateStyleSheets();
        nlist->update();

    }else if(!clsName.compare(CN_NEWGRID))
    {
        NewGrid *ngrid;
        if(flag)
        {
            QList<int> arglist =   rowcoldialog();
            if(!arglist.size())
                return;
            ngrid = new NewGrid(qv,&arglist,
                                m_frame);
        }else{
            QList<int> arglist ;
            arglist << 0 << 0 << 0 <<0;
            ngrid = new NewGrid(qv,&arglist,
                                m_frame);
            int cols = valobj[GCOLS].toInt();
            int rows = valobj[GROWS].toInt();
            ngrid->rows = rows;
            ngrid->cols = cols;
            ngrid->itemSize.scale(QSize(valobj[WIDTH].toInt(),
                                        valobj[HEIGHT].toInt()),
                                  Qt::IgnoreAspectRatio);
        }
        ngrid->mParent = this;
        ngrid->mCreateFlag = flag;
        ngrid->initialEname();
        mWindow->tree->addObjectToCurrentItem(mUniqueStr,ngrid);
        childlist.append(ngrid);
        if(flag)
            ngrid->addRowsCols();
        else{
            foreach (QJsonValue item, valobj[LISTWIDGET].toArray()) {
                ngrid->readFromJson(item);
            }
            ngrid->updateAllItemsSize();
        }
        ngrid->onSelectMe();
        ngrid->updateStyleSheets();
    }
    else if(!clsName.compare(CN_NEWFRAME)
            /*|| !clsName.compare(QFRAME)*/)
    {
        this->createObjectFromJson(qv);
    }

}


QList<int> NewLayout::rowcoldialog()
{
    QList<int> arglist;
    int r,c,w,h = 0;
    while(1){

        BaseDialog *nWindow = new BaseDialog();
        nWindow->setObjectName(metaObject()->className());
        nWindow->UpdateStyle();
        QVBoxLayout *vb = new QVBoxLayout();

        QHBoxLayout *rowlayout = new QHBoxLayout();

        QLabel *rowtitle = new QLabel("行:");
        QSpinBox *rowbox = new QSpinBox();
        //rowbox->setMaximum(max);
        QString tooltip = "请输入行列整数,必需满足 (行*列>0)";
        rowbox->setToolTip(tooltip);
        rowbox->setValue(2);

        rowlayout->addWidget(rowtitle);
        rowlayout->addWidget(rowbox);

        QLabel *coltitle = new QLabel("列:");
        QSpinBox *colbox = new QSpinBox();
        colbox->setToolTip(tooltip);
        colbox->setValue(2);

        QHBoxLayout *collayout = new QHBoxLayout();
        collayout->addWidget(coltitle);
        collayout->addWidget(colbox);

        vb->addLayout(rowlayout);
        vb->addLayout(collayout);


        QLabel *itemwd = new QLabel("单元宽度:");
        QLabel *itemht = new QLabel("单元高度:");
        QString whstr = "每个单元的大小";
        QSpinBox *itemwBox = new QSpinBox();
        itemwBox->setValue(30);
        QSpinBox *itemhBox = new QSpinBox();
        itemhBox->setValue(30);
        itemwBox->setToolTip(whstr);
        itemhBox->setToolTip(whstr);
        QHBoxLayout *wdlayout = new QHBoxLayout();
        wdlayout->addWidget(itemwd);
        wdlayout->addWidget(itemwBox);

        QHBoxLayout *htlayout = new QHBoxLayout();
        htlayout->addWidget(itemht);
        htlayout->addWidget(itemhBox);

        vb->addLayout(wdlayout);
        vb->addLayout(htlayout);

        QDialogButtonBox *dbb = new QDialogButtonBox(QDialogButtonBox::Ok|
                                                     QDialogButtonBox::Cancel,
                                                     Qt::Horizontal,
                                                     nWindow);
        dbb->button(QDialogButtonBox::Ok)->setText("确定");
        dbb->button(QDialogButtonBox::Cancel)->setText("取消");
        connect(dbb,SIGNAL(accepted()),nWindow,SLOT(accept()));
        connect(dbb,SIGNAL(rejected()),nWindow,SLOT(reject()));

        vb->addWidget(dbb);
        nWindow->setLayout(vb);
        nWindow->exec();

        int ret = nWindow->result();
        if(ret)
        {
            r = rowbox->value();
            c = colbox->value();
            w = itemwBox->value();
            h = itemhBox->value();
            nWindow->deleteLater();
            if(!r || !c || !w || !h )
            {
                break;
            }
            arglist << r << c << w <<h;
            break;
        }else{
            break;
        }
    }
    return arglist;
}

QWidget* NewLayout::createObjectFromJson(const QJsonValue &qv)
{
    QJsonObject json = qv.toObject();
    QString caption = json[CAPTION].toString();
    NewFrame *newFrame = new NewFrame(json,m_frame);
    newFrame->mParent = this;
    // newFrame->setProperty(DKEY_JSONSTR,qv);
    newFrame->mOwerJson = qv.toObject();
    newFrame->initialEname();
    newFrame->setProperty(DKEY_TYPE,json[WTYPE].toString());
    newFrame->setProperty(DKEY_TXT,caption);
    childlist.append(newFrame);
    mWindow->tree->addObjectToCurrentItem(mUniqueStr,newFrame);
    QJsonObject obj = getValueFromProperty(json[PROPERTY].toArray(),BAKIMAGE);
    if(!obj.isEmpty() && obj.contains(BAKIMAGE))
    {
        // QPixmap p;
        QString path = obj[BAKIMAGE].toString();
        newFrame->mbkImage = path;
        newFrame->updateStyleSheets();
    }
    newFrame->show();
    newFrame->onSelectMe();
    newFrame->updateStyleSheets();

}

//void NewLayout::dragEnterEvent(QDragEnterEvent *e)
//{
//    CanvasManager::mActiveSS->mXYLine->setHidden(false);
//    e->accept();
//}

//void NewLayout::dragLeaveEvent(QDragLeaveEvent *e)
//{
//    CanvasManager::mActiveSS->mXYLine->setHidden(true);
//}

//void NewLayout::dropEvent(QDropEvent *e)
//{
//    CanvasManager::mActiveSS->mXYLine->setHidden(true);
//    e->acceptProposedAction();

//    QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
//    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//    QPixmap pixmap;
//    QPoint offset;

//    QVariant qv;
//    int flag ;
//    dataStream >> flag  >> qv >> pixmap >> offset;
//    if(static_cast<int>(ObjTypes::T_NewLayer)  == flag){
//        ScenesScreen *ss = mWindow->cManager->activeSS();
//        if(!ss)
//            return;
//        ss->createNewLayer( QJsonValue::fromVariant(qv),true);
//    }else{
//        readFromJson(QJsonValue::fromVariant(qv),true);
//    }
//    CanvasManager::mActiveSS->mXYLine->raise();
//}


//void NewLayout::dragMoveEvent(QDragMoveEvent *e)
//{
//    CanvasManager::mActiveSS->mXYLine->setPos(e->pos());
//    qDebug() << " NewLayout drag move " << e->pos();
//}

NewLayer::NewLayer(const QJsonObject &json, QRect rect, QWidget *parent)
    :BaseForm(parent)
{
    mType = T_NewLayer;
    mParent = this; // 最顶层了,它的父对像设置成它自已.
    mWindow = ((ScenesScreen*)parent)->mWindow;
    setGeometry(rect);
    setMaximumSize(parent->size()); //　最大尺寸不能超过它的父控件.

    setFocusPolicy(Qt::ClickFocus);
    show();
    initObject(json);

}



void NewLayer::readLayoutFromJson(const QJsonValue &qv,bool flag)
{

    QJsonObject  valobj = qv.toObject();
    // QRect oldrect = Compoent::getRectFromStruct(valobj[PROPERTY].toArray(),KEY_RECT);
    // 下面这种粗暴的取值方式是一定含有:　　"-class": "Classname" 的条目.
    QString clsName = valobj[CLASS].toString();
    if(!clsName.compare(CN_NEWLAYOUT) /*|| !clsName.compare(CN_LAYOUT)*/)
    {
        NewLayout *newlayout = CreateNewLayout(qv,m_frame,flag,false);
        // 这里一定是Layout 嵌套了.
        childlist.append(newlayout);
        foreach (QJsonValue item, valobj[LAYOUT].toArray()) {
            //这里只有读取工程与读取自定义控件才会到这里,要做区分.
            newlayout->readFromJson(item,flag);
        }
    }

}


void NewLayer::onDeleteMe()
{
    //删除布局控件
    QMessageBox msgBox;
    msgBox.setWindowTitle("删除提示");
    QString msg = QString("你真的要删除当前%1吗?删除之后不可以撤消,请选择<删除>删除.").arg(this->property(DKEY_TXT).toString());
    msgBox.setText(msg);
    // msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Yes,"删除");
    msgBox.setButtonText(QMessageBox::Cancel,"取消");
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    //qDebug() << " QMessageBox result " << ret;
    if(ret == QMessageBox::Yes)
    {
        DeleteMe();
    }
}

QJsonObject  NewLayer::writeToJson()
{
    QJsonArray layoutarr;
    QJsonObject json = mOwerJson;
    foreach (QWidget *w, childlist) {
        layoutarr.append(((NewLayout*)w)->writeToJson());
    }
    json[NAME] = objectName();
    json[CAPTION] = this->property(DKEY_TXT).toString();
    json[LAYOUT] = layoutarr;
    json[PROPERTY] = mOwerJson[PROPERTY];
    return json;
}


//void NewLayer::dragEnterEvent(QDragEnterEvent *e)
//{
//    CanvasManager::mActiveSS->mXYLine->setHidden(false);
//    e->accept();
//}

//void NewLayer::dragLeaveEvent(QDragLeaveEvent *e)
//{
//    CanvasManager::mActiveSS->mXYLine->setHidden(true);
//}

//void NewLayer::dragMoveEvent(QDragMoveEvent *e)
//{
//    qDebug() << "NewLayer drag move " << e->pos();
//    CanvasManager::mActiveSS->mXYLine->setPos(e->pos());
//}

//void NewLayer::dropEvent(QDropEvent *e)
//{
//    CanvasManager::mActiveSS->mXYLine->setHidden(true);
//    e->acceptProposedAction();

//    QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
//    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//    QPixmap pixmap;
//    QPoint offset;

//    QVariant qv;
//    int flag ;

//    dataStream >> flag  >> qv >> pixmap >> offset;
//    if(static_cast<int>(ObjTypes::T_NewLayer)  == flag){
//        ScenesScreen *ss = mWindow->cManager->activeSS();
//        if(!ss)
//            return;
//        ss->createNewLayer( QJsonValue::fromVariant(qv),true);
//    }else{
//        readLayoutFromJson(QJsonValue::fromVariant(qv),true);
//    }
//    CanvasManager::mActiveSS->mXYLine->raise();

//}

#include "compoentcontrols.h"
//#include "handlejson.h"
#include "core_def.h"
#include "scenesscreen.h"
#include "canvasmanager.h"
#include "module.h"

#include <QComboBox>
#include <QApplication>
#include <QThread>



#define DBGPRINT 0


Border::Border(QWidget *parent)
    :QGroupBox(parent),
      Left(new QSpinBox(this)),
      Top(new QSpinBox(this)),
      Right(new QSpinBox(this)),
      Bottom(new QSpinBox(this)),
      old(0)
{
    setTitle(tr("内边框线"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");
    // setFixedHeight((parent->width()-50)*0.1);
    QGridLayout *xywh = new QGridLayout(this);
    xywh->setSpacing(2);
    setLayout(xywh);
    xywh->addWidget(new QLabel("左:",this),0,0);
    xywh->addWidget(Left,0,1);


    xywh->addWidget(new QLabel("上:",this),1,0);
    xywh->addWidget(Top,1,1);

    xywh->addWidget(new QLabel("右:",this),2,0);
    xywh->addWidget(Right,2,1);

    xywh->addWidget(new QLabel("下:",this),3,0);
    xywh->addWidget(Bottom,3,1);

    Left->setObjectName(LEFT);
    Top->setObjectName(TOP);
    Right->setObjectName(RIGHT);
    Bottom->setObjectName(BOTTOM);

    QVariant arridx = parent->property(DKEY_ARRIDX);
    QVariant parridx = parent->property(DKEY_PARRIDX);
    QVariant jsonidx = parent->property(DKEY_JSONIDX);
    QVariant cname = parent->metaObject()->className();
    QList<QWidget*> lst;
    lst << Left << Top << Right << Bottom << this;
    foreach(QWidget *sp,lst)
    {
        sp->setProperty(DKEY_ARRIDX,arridx);
        sp->setProperty(DKEY_OWERJSON,cname);
        sp->setProperty(DKEY_PARRIDX,parridx);
        sp->setProperty(DKEY_JSONIDX,jsonidx);
    }


    xywh->setContentsMargins(0,15,0,0);
}

void Border::setConnectNewQWidget(QWidget *com)
{

    setEnabled(true);
    if(old == com)
        return;

    foreach(auto var , connections)
    {
        QObject::disconnect(var);
    }

    connections.clear();

    Left->disconnect();
    Right->disconnect();
    Top->disconnect();
    Bottom->disconnect();

    QJsonObject obj = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    QJsonObject bobj = obj[BORDER].toObject();
    QRect rect(0,0,0,0);
    rect.setBottom(bobj[BOTTOM].toInt());
    rect.setLeft(bobj[LEFT].toInt());
    rect.setTop(bobj[TOP].toInt());
    rect.setRight(bobj[RIGHT].toInt());

    ((BaseForm*)com)->mBorder = rect;

    Left->setValue(rect.left());
    Top->setValue(rect.top());
    Right->setValue(rect.right());
    Bottom->setValue(rect.bottom());


    connections << QObject::connect(Left,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
    connections << QObject::connect(Top,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
    connections << QObject::connect(Right,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
    connections << QObject::connect(Bottom,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
}


void Border::resetValues()
{

    Left->setValue(0);
    Top->setValue(0);
    Bottom->setValue(0);
    Right->setValue(0);
}


Position::Position(QWidget *parent)
    :QGroupBox(parent),
      Xpos(new QSpinBox(this)),
      Ypos(new QSpinBox(this)),
      Wpos(new QSpinBox(this)),
      Hpos(new QSpinBox(this)),
      old(0)
{
    setTitle(tr("位置坐标"));
    setObjectName(parent->objectName());
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");
    // setFixedHeight((parent->width()-50)*0.1);
    QGridLayout *xywh = new QGridLayout(this);
    xywh->setSpacing(2);
    setLayout(xywh);
    xywh->addWidget(new QLabel(X,this),0,0);
    xywh->addWidget(Xpos,0,1);


    xywh->addWidget(new QLabel(Y,this),1,0);
    xywh->addWidget(Ypos,1,1);

    xywh->addWidget(new QLabel(W,this),2,0);
    xywh->addWidget(Wpos,2,1);

    xywh->addWidget(new QLabel(H,this),3,0);
    xywh->addWidget(Hpos,3,1);


    Xpos->setObjectName(X);
    Ypos->setObjectName(Y);
    Wpos->setObjectName(W);
    Hpos->setObjectName(H);

    QVariant arridx = parent->property(DKEY_ARRIDX);
    QVariant parridx = parent->property(DKEY_PARRIDX);
    QVariant jsonidx = parent->property(DKEY_JSONIDX);
    QVariant cname = parent->metaObject()->className();


    QList<QWidget*> lst ;
    lst << Xpos << Ypos << Wpos << Hpos << this;
    foreach(QWidget *sp,lst )
    {
        sp->setProperty(DKEY_ARRIDX,arridx);
        sp->setProperty(DKEY_OWERJSON,cname);
        sp->setProperty(DKEY_PARRIDX,parridx);
        sp->setProperty(DKEY_JSONIDX,jsonidx);
    }

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


    // QVariant variant = com->property(DKEY_INTOCONTAINER);
    //    bool isLW = com->property(DKEY_INTOCONTAINER).toBool();
    //    BaseForm::ObjFlags flags = ((BaseForm*)com)->mParent->mType ;
    bool isLW = ((BaseForm*)com)->mParent->isContainer();

    Xpos->setEnabled(!isLW);
    Ypos->setEnabled(!isLW);
    Hpos->setEnabled(!isLW);
    Wpos->setEnabled(!isLW);

    //  qDebug() << " net QWidget pos " << com->pos() << " size " << com->size();

    QSize psize =   com->parentWidget()->size();


    Hpos->setMaximum(  psize.height());
    Wpos->setMaximum(  psize.width());

    Xpos->setMaximum(isLW ? 999 :psize.width()-com->width());
    Ypos->setMaximum(isLW ? 999 :psize.height()-com->height());
    Ypos->setMinimum(isLW ? -999 : 0);
    Xpos->setMinimum(isLW ? -999 : 0);

    // 一定要设置值再连接信号.
    QPoint pos(0,0) ;
    if(isLW)
    {
        // 转换容器里的控件坐标,在容器里的绝对坐标.
        pos =   com->mapToParent(com->parentWidget()->pos());
    }

    QJsonObject obj = QJsonValue::fromVariant(property(DKEY_JSONSTR)).toObject();
    QJsonObject bobj = obj[KEY_RECT].toObject();
    QRect rect(bobj[LX].toInt(),bobj[LY].toInt(),
               bobj[WIDTH].toInt(),bobj[HEIGHT].toInt());
    Xpos->setValue(rect.x());
    Ypos->setValue(rect.y());
    Hpos->setValue(rect.height());
    Wpos->setValue(rect.width());
    //if(!property(DKEY_ARRIDX).toInt())
    com->setGeometry(rect);

    if(!isLW)
    {
        connections << QObject::connect(Xpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
        connections << QObject::connect(Ypos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
        connections << QObject::connect(Wpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
        connections << QObject::connect(Hpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
    }
    old = com;
}

void Position::updatePosition(QWidget *w)
{

    //    bool isLW = w->property(DKEY_INTOCONTAINER).toBool();
    QPoint pos = w->pos();
    Xpos->blockSignals(true);
    Ypos->blockSignals(true);

    Xpos->setValue(pos.x());
    Ypos->setValue(pos.y());

    Xpos->blockSignals(false);
    Ypos->blockSignals(false);
}

void Position::updateSize(QWidget *w)
{
    QSize size = w->size();
    Wpos->blockSignals(true);
    Hpos->blockSignals(true);

    Wpos->setValue(size.width());
    Hpos->setValue(size.height());

    Wpos->blockSignals(false);
    Hpos->blockSignals(false);
}

void Position::updateMaxSize(QSize size)
{

    Wpos->setRange(10,size.width());
    Hpos->setRange(10,size.height());
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
            QWidget *w = child->widget();
            if(w)
            {
                if(w->layout() > 0)
                {
                    removeWidFromLayout(w->layout());
                }else {
                    w->setParent(0);
                    delete w;
                }
            }
        }

        delete child;
    }
}



CssProperty::CssProperty(QWidget *parent)
    :BaseProperty(parent)

{
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setMargin(0);
    show();
}


PropertyTab::PropertyTab(QWidget *parent)
    :QTabWidget(parent)
{

    show();
    connect(this,SIGNAL(currentChanged(int)),SLOT(onTabChanged(int)));
}


void PropertyTab::onTabChanged(int index){
    // 更改tab 项,刷新界面里的控件.
    //int index = currentIndex();
    int pindex = currentWidget()->property(DKEY_PARRIDX).toInt();
    BaseForm *bfobj = ((BaseForm *)mOwerObj);

    QJsonArray parry = bfobj->mOwerJson[PROPERTY].toArray();
    QJsonObject structobj = parry.at(pindex).toObject();
    QJsonArray structarry = structobj[STRUCT].toArray();

    QString objname= QString("%1_%2").arg(bfobj->mUniqueStr,
                                          QString::number(index));


    bfobj->posWidget = findChild<Position*>(objname);

    QJsonValue val =  structarry.at(index);
    QColor color = QColor(bfobj->getJsonValue(val.toArray(),
                                              BAKCOLOR).toString());
    if(color.isValid())
        bfobj->mbkColor = color.name(QColor::HexArgb);
    QString img =  bfobj->getJsonValue(val.toArray(),BAKIMAGE).toString();

    if(!img.isEmpty())
        bfobj->mbkImage =  QDir::currentPath() + BACKSLASH + img;

    bfobj->mBorder =
            bfobj->getRectFromStruct(val.toArray(),BORDER);

    color = QColor(bfobj->getJsonValue(val.toArray(),
                                       GRAYCOLOR).toString());
    if(color.isValid())
        bfobj->mBorderColor = color.name(QColor::HexArgb);

    //if(!bfobj->property(DKEY_INTOCONTAINER).toBool())
    if(!bfobj->mParent->isContainer())
    {
        //如果对像不在列表下,每一TAB属性里的位置坐标可以改变.
        //            qDebug() << tabrect << oldrect;
        QRect tabrect = bfobj->getRectFromStruct(val.toArray(),KEY_RECT);
        bfobj->setGeometry(tabrect);
    }


    bfobj->updateStyleSheets();

}

void PropertyTab::handleCSSProperty(TabHandle  handle)
{
    int index = currentIndex();
    int pindex = currentWidget()->property(DKEY_PARRIDX).toInt();
    QJsonArray parry = ((BaseForm *)mOwerObj)->mOwerJson[PROPERTY].toArray();
    QJsonObject structobj = parry.at(pindex).toObject();
    QJsonArray structarry = structobj[STRUCT].toArray();
    QJsonValue val =  structarry.at(index);

    switch (handle) {
    case Append:
    {
        CssProperty *cp = new CssProperty(this);
        cp->setProperty(DKEY_PARRIDX,
                        currentWidget()->property(DKEY_PARRIDX));   // STRUCT 属性在　PROPERTY属性数组里的位置.
        cp->setProperty(DKEY_ARRIDX,this->count());    //在STRUCT 属性数组的位置.


        structarry.append(val);
        structobj[STRUCT] = structarry;
        parry[pindex] = structobj;

        //        qDebug() << "add tab,json rect"  << ((BaseForm *)mOwerObj)->getRectFromStruct(parry,KEY_RECT)

        //                 << "object rect " <<  mOwerObj->geometry();

        ((BaseForm *)mOwerObj)->mOwerJson[PROPERTY] = parry;
        cp->parseJsonToWidget(mOwerObj,val.toArray());
        addTab(cp,QString("CSS属性_%1").
               arg(this->count()));

        //setCurrentWidget(cp);
    }
        break;
    case Insert:
    {
        CssProperty * cp = new CssProperty(this);
        cp->setProperty(DKEY_PARRIDX,
                        currentWidget()->property(DKEY_PARRIDX));   // STRUCT 属性在　PROPERTY属性数组里的位置.
        cp->setProperty(DKEY_ARRIDX,index);    //在STRUCT 属性数组的位置.


        structarry.insert(index,val);
        structobj[STRUCT] = structarry;
        parry[pindex] = structobj;
        cp->parseJsonToWidget(mOwerObj,val.toArray());

        ((BaseForm *)mOwerObj)->mOwerJson[PROPERTY] = parry;
        this->insertTab(index,cp,QString("CSS属性_%1").
                        arg(count()));


    }
        break;
    case Delete:
    {
        structarry.removeAt(index);
        this->removeTab(index);
        structobj[STRUCT] = structarry;
        parry[pindex] = structobj;
        ((BaseForm *)mOwerObj)->mOwerJson[PROPERTY] = parry;
    }
        break;
    default:
        break;
    }

    CssProperty *cp =(CssProperty*)(currentWidget());
    setCurrentWidget(cp);

}

void PropertyTab::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        QMenu *menu = new QMenu(this);
        QAction menuCopyAdd("复制添加",this);
        QObject::connect(&menuCopyAdd,
                         &QAction::triggered,[=](){
            handleCSSProperty(Append);
        });
        QAction menuCopyInsert("复制插入",this);
        QObject::connect(&menuCopyInsert,
                         &QAction::triggered,[=](){

            handleCSSProperty(Insert);
        });

        QAction menuDelete("删除活动项",this);
        QObject::connect(&menuDelete,
                         &QAction::triggered,[=](){
            handleCSSProperty(Delete);
        });

        menu->addAction(&menuCopyAdd);
        if(currentIndex())  menu->addAction(&menuCopyInsert);
        menu->addSeparator();

        if(currentIndex())     menu->addAction(&menuDelete);
        menu->exec(mapToGlobal(event->pos()));
    }
}

void PropertyTab::clearLayout()
{
    this->blockSignals(true);
    for(int i = 0; i < count() ; i++)
    {
        CssProperty *w = (CssProperty*)this->widget(i);

        removeTab(i);
        delete w;
    }
}

ComProperty::ComProperty(QWidget *parent):
    BaseProperty(parent),
    // mainLayout(new QVBoxLayout()),
    oldobject(0)

{
    //setTitle(title);
    setStyleSheet("QGroupBox,QLabel,QScrollArea,QWidget{background-color: #C0DCC0;}");
    this->setFixedHeight((parent->height()-50) * 0.7);
    this->setFixedWidth(((MainWindow*)parent)->lDock->width());
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    QHBoxLayout *hb = new QHBoxLayout(this);
    hb->setSpacing(0);
    hb->setMargin(0);
    hb->setContentsMargins(0,4,0,0);
    hb->setSpacing(0);
    this->setLayout(hb);
    mainWidget = new QWidget();
    mainWidget->setObjectName("mainWidget");

    QScrollArea *scroll =  new QScrollArea();
    scroll->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
    // scroll->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    scroll->setContentsMargins(0,0,0,0);
    scroll->setParent(this);
    scroll->setFixedSize(this->size());
    hb->addWidget(scroll);
    scroll->setWidget(mainWidget);
    mainWidget->setLayout(mainLayout);
    mainWidget->setFixedSize(this->size());
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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

    // QString nkeyuid ;
    if(mainLayout)
    {

        removeWidFromLayout(mainLayout);
        ((BaseForm*)p)->posWidget = 0;
        if(oldobject)
            ((BaseForm*)oldobject)->posWidget = 0;
        delete mainLayout;
        mainLayout = 0;
    }

    mainLayout = new QVBoxLayout(mainWidget);
    // mainLayout->setProperty(DKEY_UID,nkeyuid);

    mainWidget->setLayout(mainLayout);

    mainLayout->setObjectName(COMGRPLYT);
    mainLayout->addSpacing(0);

    parseJsonToWidget(p, ((BaseForm*)p)->mOwerJson[PROPERTY].toArray());

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    oldobject = p;
    show();
}




BaseProperty::BaseProperty(QWidget *parent)
    :QWidget(parent), mainLayout(new QVBoxLayout)
{

}



void BaseProperty::parseJsonToWidget(QWidget *p, const QJsonArray &array)
{
    for(int i = 0 ; i< array.size() ;i++)
    {
        QJsonValue item = array.at(i);
        QWidget *wid = 0;
        this->setProperty(DKEY_JSONIDX,i);

        switch (item.type()) {
        case QJsonValue::Array:
        {
            //   parseJsonToWidget(p,item.toArray());
        }
            break;
        case QJsonValue::Object:
        {
            QJsonObject object = item.toObject();
            QString uname = object[WTYPE].toString();
            QString caption = object[CAPTION].toString();
            if(object.contains(ISTRUE))
            {
                continue;
            }

            if(object.contains(STRUCT)) // 处理struct 关键字,QJsonArray
            {
                // parseJsonToWidget(p,object[STRUCT].toArray());
                PropertyTab *csstab = new PropertyTab(this);
                csstab->setObjectName(uname);
                csstab->setProperty(DKEY_ARRIDX,i);
                csstab->mOwerObj = p;

                bool manyCss = false;
                // 这个STRUCT属性,对应于Ｃ语言中的结构体的成员变量,这里可能有多份不同的值.
                QJsonArray structArray = object[STRUCT].toArray();
                csstab->blockSignals(true);
                for(int n = 0 ; n < structArray.size();n++)
                {
                    QJsonValue sval =structArray.at(n);
                    if(sval.isArray())
                    {

                        CssProperty *cp = new CssProperty(csstab);
                        cp->setObjectName(uname + QString::number(n));
                        cp->setProperty(DKEY_PARRIDX,i);   // STRUCT 属性在　PROPERTY属性数组里的位置.
                        cp->setProperty(DKEY_ARRIDX,n);    //在STRUCT 属性数组的位置.
                        csstab->addTab(cp,QString("CSS属性_%1").
                                       arg(QString::number(n)));
                        cp->parseJsonToWidget(p,sval.toArray());
                        // csstab->setCurrentWidget(cp);
                        manyCss = true;
                    }

                }
                csstab->blockSignals(false);

                if(!manyCss)
                {
                    csstab->deleteLater();
                    parseJsonToWidget(p,object[STRUCT].toArray());
                }else{
                    csstab->setCurrentIndex(csstab->count() -1);
                    mainLayout->addWidget(csstab);
                }


            }else if(object.contains(ENUM)){
                QLabel *title = new QLabel(caption,this);
                QComboBox *cb = new QComboBox(this);

                cb->setObjectName(uname);
                cb->setProperty(DKEY_CAPTION,caption);
                cb->setProperty(DKEY_VALTYPE,ENUM);
                QVariantList qvlist = object[ENUM].toArray().toVariantList();
                for(QVariantList::const_iterator it = qvlist.begin();
                    it != qvlist.end();++it)
                {
                    cb->addItem((*it).toMap().firstKey());
                }
                mainLayout->addWidget(title);
                mainLayout->addWidget(cb);
                wid = cb;
                connect(cb,SIGNAL(currentTextChanged(QString)),
                        p,SLOT(onEnumItemChanged(QString)));
            }else if(object.contains(LIST))
            {
                //QLabel *title = new QLabel(caption);
                QComboBox *cb = new QComboBox(this);
                cb->setObjectName(uname);
                cb->setProperty(DKEY_VALTYPE,LIST);
                // 这里通过它的JSON组数的位置去找它.
                //                wid = cb;
                QPushButton *btn = new QPushButton(caption,this);
                btn->setProperty(DKEY_JSONIDX,i);
                btn->setObjectName(uname);
                btn->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
                btn->setProperty(DKEY_ARRIDX,this->property(DKEY_ARRIDX));
                btn->setProperty(DKEY_OWERJSON,this->metaObject()->className());
                btn->setProperty(DKEY_PARRIDX,this->property(DKEY_PARRIDX));

                cb->setFixedWidth(this->width()-30);
                btn->setFixedWidth(this->width() -30);
                mainLayout->addWidget(btn);
                mainLayout->addWidget(cb);
                QJsonArray cbarray = object[LIST].toArray();
                btn->setProperty(DKEY_CBLIST,cbarray);
                QStringList lst ;
                BaseForm* baseform =   ((BaseForm*)p);
                if(!uname.compare(PIC_TEXT))
                {

                    foreach(QJsonValue v,cbarray) lst << baseform->mWindow->mItemMap.value(v.toString());
                    QString defstr = object[DEFAULT].toString().toLower();
                    cb->addItems(lst);
                    cb->setCurrentText(baseform->mWindow->mItemMap.value(defstr));
                    QObject::connect(btn,&QPushButton::clicked,[=](){
                        QJsonArray cblist =btn->property(DKEY_CBLIST).toJsonArray();
                        I18nLanguage lang(cblist.toVariantList(),baseform->mWindow);
                        QSize ps = lang.parentWidget()->size();
                        int w = ( ps.width() - lang.width())/ 2;
                        int h = (ps.height() - lang.height()) / 2;
                        lang.move(lang.mapFromParent(QPoint(w,h)));
                        lang.exec();

                        QStringList nlst = lang.getSelectedItems();
                        QString curstr = cb->currentText();
                        cb->clear();
                        cb->addItems(nlst);
                        cb->setCurrentText(curstr);
                        QStringList klist ;
                        foreach(QString v,nlst) klist << baseform->mWindow->mItemMap.key(v);
                        baseform->changeJsonValue(btn,uname,klist);
                        btn->setProperty(DKEY_CBLIST,QJsonArray::fromStringList(klist));
                        baseform->updateStyleSheets();

                    });

                    QObject::connect(cb,&QComboBox::currentTextChanged,[=](QString txt){
                        baseform->changeJsonValue(btn,uname,
                                                  baseform->mWindow->mItemMap.key(txt));
                    });
                    //                    continue;

                }else {
                    // 图片列表处理分支.这里不用Ｍap而用LIST来暂存一些数值,是因为map是无序的.
                    QJsonArray imgarray =  object[LIST].toArray();
                    QString defimg = object[DEFAULT].toString();
                    foreach (QVariant v, imgarray) {
                        QString key = v.toString();
                        int idx = key.lastIndexOf(BACKSLASH)+1;
                        lst << QString("%1|%2").arg(key.mid(idx),key);
                        //  QPixmap pic =  ((BaseForm*)p)->mWindow->mImgMap[imgdir+BACKSLASH+key];
                        cb->addItem(QIcon(key),
                                    key.mid(idx));
                    }

                    cb->setCurrentText(defimg);
                    cb->setProperty(DKEY_CBLIST,imgarray);
                    QObject::connect(btn,&QPushButton::clicked,[=](){
                        QJsonArray cblist =btn->property(DKEY_CBLIST).toJsonArray();
                        ImageFileDialog ifd(cblist.toVariantList(),
                                            baseform->mWindow->cManager->mProjectImageDir,
                                            baseform->mWindow);
                        QSize ps = baseform->mWindow->size();
                        int w = ( ps.width() - ifd.width() )/ 2;
                        int h = (ps.height()-ifd.height()) / 2;
                        ifd.move(ifd.mapFromParent(QPoint(w,h)));
                        ifd.exec();
                        QVariantList imglist = ifd.getSelectedList();
                        QJsonArray qa;
                        QString curstr = cb->currentText();
                        cb->clear();
#ifdef DBGPRINT
                        QFile fd("imagelist.txt");
                        fd.open(QIODevice::WriteOnly|QIODevice::Text);
#endif
                        foreach (QVariant v, imglist) {
                            QString s = v.toString();
                            // example for s   "alarm_du.bmp|/home/yjdwbj/build-ut-tools-Desktop_Qt_5_6_0_GCC_64bit-Debug/images/string/alarm_du.bmp
                            // example for s   ""m104.bmp|config/images/string/m104.bmp"
                            QString lastsection = s.section(SECTION_CHAR,1,1);
#ifdef DBGPRINT
                            fd.write(v.toString().toLocal8Bit().data());
#endif
                            QString substr;
                            if(lastsection.indexOf(BACKSLASH) == 0)
                            {
#ifdef DBGPRINT
                                fd.write(QString("baskslash equal zero").toLocal8Bit().data());
#endif
                                substr = s.section(SECTION_CHAR,1,1).mid(baseform->mWindow->mRootPathLen);
                            }else
                            {
#ifdef DBGPRINT
                                fd.write(QString("baskslash equal zero else").toLocal8Bit().data());
#endif
                                // 添加截取它的相对路径
                                substr = s.section(SECTION_CHAR,1,1).mid(baseform->mWindow->mRootPathLen);
                            }

                            qa.append(substr);
#ifdef DBGPRINT
                            fd.write("\n");
                            fd.write(substr.toLocal8Bit().data());
                            fd.write("\n");
#endif
                            cb->addItem(QIcon(s.section(SECTION_CHAR,1,1)),
                                        s.section(SECTION_CHAR,0,0));
                        }
#ifdef DBGPRINT
                        fd.close();
#endif
                        cb->setCurrentText(curstr);
                        baseform->changeJsonValue(btn,uname,qa.toVariantList());
                        btn->setProperty(DKEY_CBLIST,qa.toVariantList());
                        cb->setProperty(DKEY_CBLIST,imglist);
                        baseform->updateStyleSheets();
                    });

                    QObject::connect(cb,&QComboBox::currentTextChanged,[=](QString txt){

                        QVariantList imglist= cb->property(DKEY_CBLIST).toList();
                        foreach (QVariant v, imglist) {
                            QString s = v.toString();
                            QString k = s.section(SECTION_CHAR,0,0);
                            if(!k.compare(txt))
                            {
                                QString fpath = s.section(SECTION_CHAR,1,1);
                                baseform->changeJsonValue(btn,uname,
                                                          fpath.mid(baseform->mWindow->mRootPathLen)); // 修改JSON里的值
                                break;
                            }
                        }
                    });
                }

                continue;

            }else if(object.contains(KEY_RECT))
            {

                Position* posWidget =   new Position(this);
                posWidget->setProperty(DKEY_JSONIDX,i);
                posWidget->setObjectName(QString("%1_%2").arg(((BaseForm*)p)->mUniqueStr,
                                                              QString::number(property(DKEY_ARRIDX).toInt())));
                posWidget->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
                if(!p->property(DKEY_ARRIDX).toInt())
                    ((BaseForm*)p)->posWidget = posWidget;
                //  if(p->property(DKEY_INTOCONTAINER).toBool())
                if(((BaseForm*)p)->mParent &&
                        ((BaseForm*)p)->mParent->isContainer())
                {
                    posWidget->setHidden(true);

                }else{
                    posWidget->setConnectNewQWidget(p);
                }

                mainLayout->addWidget(posWidget);

            }else if(object.contains(BORDER))
            {

                Border *b = new Border(this);
                b->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
                b->setProperty(DKEY_JSONIDX,i);

                b->setConnectNewQWidget(p);
                mainLayout->addWidget(b);
                QPushButton *btn = new QPushButton(caption,this);

                btn->setObjectName(uname);
                //                b->setObjectName(uname);

                QHBoxLayout *hb = new QHBoxLayout;
                hb->setObjectName(uname);


                QPushButton *clearBtn = new QPushButton(QIcon(":/icon/icons/undo.png"),"",this);
                clearBtn->setToolTip("清除" + caption);
                clearBtn->setObjectName(uname);
                clearBtn->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
                clearBtn->setProperty(DKEY_ARRIDX,this->property(DKEY_ARRIDX));
                clearBtn->setProperty(DKEY_OWERJSON,this->metaObject()->className());
                clearBtn->setProperty(DKEY_PARRIDX,this->property(DKEY_PARRIDX));
                hb->addWidget(btn);
                hb->addWidget(clearBtn);
                connect(clearBtn,SIGNAL(clicked(bool)),p,SLOT(onClearJsonValue()));

                mainLayout->addLayout(hb);

                connect(btn,SIGNAL(clicked(bool)),p,SLOT(onColorButtonClicked()));
                wid = btn;
            }else if(object.contains(UID))
            {
                QLabel *title = new QLabel(caption,this);
                mainLayout->addWidget(title);

                QLineEdit *nameEdt = new QLineEdit(this);
                nameEdt->setObjectName(uname);

                //  nameEdt->setMaxLength(8);
                //   nameEdt->setInputMask("nnnnnnnn;"); // or NNNNNNNN;_
                nameEdt->setCursorPosition(0);
                nameEdt->setText(((BaseForm*)p)->updateEname(i));
                mainLayout->addWidget(nameEdt);
                connect(nameEdt,SIGNAL(textChanged(QString)),
                        p,SLOT(onTextChanged(QString)));
                wid = nameEdt;
                // 这里是一个特殊属性,唯一序号
            }
            else if(object.contains(BAKIMAGE))
            {
                QHBoxLayout *hb = new QHBoxLayout;
                hb->setObjectName(uname);
                hb->setSpacing(0);

                QPushButton *bkimage = new QPushButton(caption,this);
                bkimage->setObjectName(uname);

                QPushButton *clearBtn = new QPushButton(QIcon(":/icon/icons/undo.png"),"",this);
                clearBtn->setToolTip("清除" + caption);
                clearBtn->setObjectName(uname);
                clearBtn->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
                clearBtn->setProperty(DKEY_ARRIDX,this->property(DKEY_ARRIDX));
                clearBtn->setProperty(DKEY_OWERJSON,this->metaObject()->className());
                clearBtn->setProperty(DKEY_PARRIDX,this->property(DKEY_PARRIDX));
                hb->addWidget(bkimage);
                hb->addWidget(clearBtn);
                connect(clearBtn,SIGNAL(clicked(bool)),p,SLOT(onClearJsonValue()));

                mainLayout->addLayout(hb);
                p->setProperty(DKEY_CURVAL,BAKIMAGE);
                wid = bkimage;
                connect(bkimage,SIGNAL(clicked(bool)),p,SLOT(onBackgroundImageDialog()));
            }
            else if(object.contains(BAKCOLOR) ||
                    object.contains(COLOR))
            {
                QHBoxLayout *hb = new QHBoxLayout;
                hb->setSpacing(0);
                hb->setObjectName(uname);
                QPushButton *bkcolor = new QPushButton(caption,this);
                bkcolor->setObjectName(uname);

                QPushButton *clearBtn = new QPushButton(QIcon(":/icon/icons/undo.png"),"",this);
                clearBtn->setToolTip("清除" + caption);
                clearBtn->setObjectName(uname);
                hb->addWidget(bkcolor);
                hb->addWidget(clearBtn);

                clearBtn->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
                clearBtn->setProperty(DKEY_ARRIDX,this->property(DKEY_ARRIDX));
                clearBtn->setProperty(DKEY_OWERJSON,this->metaObject()->className());
                clearBtn->setProperty(DKEY_PARRIDX,this->property(DKEY_PARRIDX));

                connect(clearBtn,SIGNAL(clicked(bool)),p,SLOT(onClearJsonValue()));

                mainLayout->addLayout(hb);
                //mainLayout->addWidget(bkcolor);
                p->setProperty(DKEY_CURVAL,BAKCOLOR);

                connect(bkcolor,SIGNAL(clicked(bool)),p,SLOT(onColorButtonClicked()));
                wid = bkcolor;
            } else if(object.contains(ACTION))
            {
                // 这个按钮控件不作初始化了.
                QPushButton *actBtn = new QPushButton(object[CAPTION].toString(),this);
                //                qDebug() << " handle action is " << object[CAPTION].toString();
                actBtn->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
                actBtn->setProperty(DKEY_ARRIDX,i);
                mainLayout->addWidget(actBtn);
                connect(actBtn,SIGNAL(clicked(bool)),p,SLOT(onActionDialog()));

            }
            else{
                //下面是通JSON值类型来区分来创建不同类型的控件.
                if(object[DEFAULT].isString())
                {
                    if(!uname.compare(PIC_TEXT))
                    {

                        QLabel *title = new QLabel(caption,this);
                        QComboBox *cb = new QComboBox(this);
                        cb->setObjectName(uname);
                        QString defstr = object[DEFAULT].toString().toLower();
                        foreach (QString key, ((BaseForm*)p)->mWindow->mOrderlist) {
                            cb->addItem( ((BaseForm*)p)->mWindow->mItemMap[key] );
                        }
                        cb->setCurrentIndex(((BaseForm*)p)->mWindow->mOrderlist.indexOf(defstr));
                        mainLayout->addWidget(title);
                        mainLayout->addWidget(cb);
                        wid = cb;
                        connect(cb,SIGNAL(currentTextChanged(QString)),
                                p,SLOT(onEnumItemChanged(QString)));
                    }else{

                        QLineEdit *txt = new QLineEdit(object[DEFAULT].toString(),this);
                        txt->setObjectName(uname);
                        txt->setProperty(DKEY_VALTYPE,TEXT);
                        if(object.contains(MAXLEN))
                        {
                            txt->setMaxLength(object[MAXLEN].toDouble());
                        }
                        wid = txt;
                        // QLabel * l = new QLabel(uname);
                        QLabel *title = new QLabel(caption,this);
                        mainLayout->addWidget(title);
                        mainLayout->addWidget(txt);
                        txt->setFixedHeight(25);
                        connect(txt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));
                    }


                }else if(object[DEFAULT].isDouble())
                {
                    QLabel *title = new QLabel(caption,this);
                    //int val = object[DEFAULT].toDouble();
                    mainLayout->addWidget(title);
                    QSpinBox *s = new QSpinBox(this);
                    s->setObjectName(uname);
                    s->setProperty(DKEY_VALTYPE,NUMBER);
                    //要保存每一次修改过的值.
                    mainLayout->addWidget(s);
                    wid = s;
                    if(object.contains(MAX))
                    {
                        s->setMaximum(object[MAX].toInt());
                    }
                    if(object.contains(MIN))
                    {
                        s->setMinimum(object[MIN].toInt());
                    }
                    connect(s,SIGNAL(valueChanged(int)),p,SLOT(onNumberChanged(int)));
                }
            }
        }

        default:
            break;
        }
        if(!wid)
            continue;

        wid->setProperty(DKEY_JSONSTR,item); // 用来提取JSON里的值,不用在大范围查找.
        wid->setProperty(DKEY_ARRIDX,this->property(DKEY_ARRIDX));
        wid->setProperty(DKEY_OWERJSON,this->metaObject()->className());
        wid->setProperty(DKEY_PARRIDX,this->property(DKEY_PARRIDX));
        wid->setProperty(DKEY_JSONIDX,i);
        ((BaseForm*)p)->onBindValue(wid);

    }
}

CompoentControls::CompoentControls(QWidget *parent)
    : QGroupBox(parent),
      //:QWidget(parent),
      mainLayout(new QVBoxLayout()),
      mainWidget(new QWidget()),
      mCWidgetCount(0),
      //      mSequence(0),
      mWindow((MainWindow*)parent)
{

    setTitle(tr("控件列表"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");\
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,2,2,2);
    mainLayout->setMargin(1);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QVBoxLayout *hb = new QVBoxLayout();
    // hb->addWidget(new QLabel("控件列表"));
    hb->setSpacing(0);
    hb->setMargin(0);
    hb->setContentsMargins(0,4,0,0);
    this->setLayout(hb);

    // mainWidget = new QWidget();

    QScrollArea *scroll =  new QScrollArea();
    hb->addWidget(scroll);
    scroll->setWidget(mainWidget);
    mainWidget->setLayout(mainLayout);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}



void CompoentControls::ReadJsonWidgets()
{
    // 读取控件目录下的所有控件文件.
    // mJsonFile =  QDir::currentPath() + "/menu_strip.json";
    // QString file = QDir::currentPath() + "/control.json";
    QString file = mWindow->mGlobalSet->value(INI_PRJJSON).toString();

    while (1){
        if(!file.isEmpty()) break;
        GlobalSettings gs(mWindow);
        QSize ps = mWindow->size();
        int w = ( ps.width() - gs.width() )/ 2;
        int h = (ps.height()-gs.height()) / 2;
        gs.move(gs.mapFromParent(QPoint(w,h)));
        gs.exec();
        if (gs.isSetFine()) {
            mWindow->mGlobalSet->deleteLater();
            mWindow->mGlobalSet = new QSettings(mWindow->mGlobalIniFile,QSettings::IniFormat);
            file = QFileInfo(mWindow->mGlobalSet->value(INI_PRJJSON).toString()).absoluteFilePath();

            break;
        }
    }

    //  qDebug() << " json file name " << file;
    QFileInfo qfi(file);
    if(!qfi.exists())
    {
        QMessageBox::warning(this,tr("错误"),tr("找不到控件文件,请查看[全局设置]里的路径目录是否正确."));
        return;
    }

    QJsonArray qjv = ReadTemplateWidgetFile(file);

    if(!qjv.isEmpty())
        CreateButtonList(qjv);
    //   QString customdir = QDir::currentPath() + "/widgets";

    // 读取一些自定义的控件.
    // QDirIterator it(QDir::currentPath().replace(SLASH,BACKSLASH)+ BACKSLASH + "widgets", QStringList() << "*.json", QDir::Files, QDirIterator::Subdirectories);
    QVariant witdir = mWindow->mGlobalSet->value(INI_PRJCUSTOM);

    if(witdir.isValid() && QFileInfo(witdir.toString()).exists())
    {
        QDirIterator it( witdir.toString() , QStringList() << "*.json",
                         QDir::Files, QDirIterator::NoIteratorFlags);
        QJsonArray array;
        while (it.hasNext())
        {
            // 读取每一个自定义控件的JSON文件,静态编绎要防止的这个变量被优化了.
            QJsonArray  tarr =  ReadTemplateWidgetFile(it.next());
            foreach (QJsonValue val,tarr ) {
                array.append(val);
            }
            mCWidgetCount++;
        }
        if(!array.isEmpty())
        {
            QWidget *wid = createCustomObject(array);
            mainLayout->addWidget(wid);
        }
    }

}



QJsonArray CompoentControls::ReadTemplateWidgetFile(QString file) const
{
    QJsonArray array;
    QFile data(file);
    if (data.open(QFile::ReadOnly|QIODevice::Text)) {
        QByteArray qba = data.readAll().replace(SLASH,BACKSLASH);
        QString dd(qba);
        dd.replace("//","/");
        QJsonParseError json_error;
        QJsonDocument qd = QJsonDocument::fromJson(dd.toUtf8(),&json_error);
        if(json_error.error == QJsonParseError::NoError)
        {
            if(qd.isObject())
            {
                if(qd.object().contains(COMPOENTS))
                {
                    QJsonObject retobj = qd.object();
                    return retobj[COMPOENTS].toArray();
                }
            }
        }else{
            QMessageBox::warning(0,tr("错误"),
                                 QString("读取控件文件遇到错误<%1> 偏移<%2>,请查看检查文件<%3> 格式.").arg(json_error.errorString(),
                                                                                      QString::number(json_error.offset),
                                                                                      file));
        }
    }
    return array;
}


QWidget* CompoentControls::createCustomObject(const QJsonArray &comJsonArr)
{
    QGroupBox *gb = new QGroupBox(tr("自定义控件"));
    QGridLayout *comLayout = new QGridLayout();
    comLayout->setMargin(0);
    comLayout->setSpacing(2);
    comLayout->setContentsMargins(1,5,1,0);
    gb->setLayout(comLayout);
    int row =0;
    int col = 0;
    //    QByteArray winba= QJsonDocument(comJsonArr).toJson();

    foreach (QJsonValue qjv, comJsonArr)
    {
        QJsonObject jobj = qjv.toObject();
        //        QByteArray ba = QJsonDocument(jobj).toJson();

//        QString caption = jobj[CAPTION].toString();
//        QString objname = jobj[NAME].toString();
        DragButton *btnTest = new DragButton(qjv,this);
//        btnTest->setProperty(DKEY_CATEGORY,objname);
//        btnTest->setProperty(DKEY_JSONSTR,qjv.toVariant()); // 这个按钮绑定这一个JSON控件.

        if(jobj.contains(ICON))
            btnTest->setIcon(QIcon(jobj[ICON].toString()));

        btnTest->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        if(col == 2)
        {
            col = 0;
            row++;
        }

        // 这样能把tooltip显示成一张图片.
        btnTest->setToolTip( QString("<img src='%1'>").arg(jobj[ICON].toString()));
        BaseForm::mObjectTemplte.append(btnTest);
        comLayout->addWidget(btnTest,row,col++);
        // 这里要创建单独处理自定义控件的函数.
        // connect(btnTest,SIGNAL(clicked(bool)),this,SLOT(onCreateCustomWidget()));
    }
    return gb;
}


void CompoentControls::onCreateCustomWidget()
{
    // 被拖放替代,废弃了
    QWidget *wid = ScenesScreen::mActiveObj;
    if(!wid)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }
    //   QString clsname = wid->metaObject()->className();
    BaseForm::ObjTypes wtype = ((BaseForm*)wid)->mType;
    // if(!clsname.compare(CN_NEWLAYER) /*|| !clsname.compare(CN_LAYER)*/)
    if(wtype == BaseForm::T_NewLayer)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }
    DragButton *btn = (DragButton*)(QObject::sender());
    QJsonValue value = QJsonValue::fromVariant(btn->property(DKEY_JSONSTR));



    if(wtype == BaseForm::T_NewLayout)
    {
        ((NewLayout*)wid)->readFromJson(value,true);
    }
    else if(wtype == BaseForm::T_NewFrame)
    {
        // 选择它的父控件.
        ((NewLayout*)(wid->parentWidget()))->readFromJson(value,true);
    }
    else
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }
}

void CompoentControls::CreateButtonList(const QJsonArray &comJsonArr)
{

    int row = 0;
    int col = 0;
    QGridLayout *comLayout = new QGridLayout();

    // mainLayout->addWidget(new QPushButton());
    QVBoxLayout *v = new QVBoxLayout();
    v->setMargin(1);
    v->setSpacing(1);
    v->setContentsMargins(1,1,1,1);
    mainLayout->addLayout(v);
    mainLayout->addLayout(comLayout);
    //  comLayout->setSizeConstraint(QLayout::SetFixedSize);

    comLayout->setVerticalSpacing(1);
    comLayout->setHorizontalSpacing(1);
    comLayout->setMargin(1);
    comLayout->setContentsMargins(1,1,1,1);

    // 不写下面这句,Linux下mingw交叉编译的程序在winxp会出内存错误崩溃,
    //　winxp下的mingw编译没有测试.我服了它.
    foreach (QJsonValue qjv, comJsonArr)
    {
        QJsonObject jobj = qjv.toObject();
        //        QString caption = jobj[CAPTION].toString();
//        QString objname = jobj[NAME].toString();
        QString clsname = jobj[CLASS].toString();
        if(!clsname.compare(CN_NEWLAYOUT) /*||!CN_LAYOUT.compare(clsname)*/)
        {
            mVariantLayout = qjv.toVariant();
        }
        //  qDebug() << " json key is " << uname;

        DragButton *ObjBtn = new DragButton(qjv,this);

//        ObjBtn->setProperty(DKEY_CATEGORY,objname);
//        ObjBtn->setProperty(DKEY_JSONSTR,qjv.toVariant()); // 这个按钮绑定这一个JSON控件.

        if(jobj.contains(ICON))
            ObjBtn->setIcon(QIcon(jobj[ICON].toString()));

        //QString wtype = qjv.toObject()[WTYPE].toString();
        QString wtype = jobj[CLASS].toString();
        if(!wtype.compare(CN_NEWLAYER) /* || !wtype.compare(CN_LAYER)*/)
        {
            v->addWidget(ObjBtn);
            ObjBtn->setFixedHeight(50);
            //            connect(ObjBtn,SIGNAL(clicked(bool)),SLOT(onCreateNewLayer()));
        }
        else if(!wtype.compare(CN_NEWLAYOUT) /*|| !wtype.compare(CN_LAYOUT)*/ )
        {
            v->addWidget(ObjBtn);
            BaseForm::mLayout = ObjBtn;
            ObjBtn->setFixedHeight(50);
            //            connect(ObjBtn,SIGNAL(clicked(bool)),SLOT(onCreateNewLayout()));
        }
        else{

            ObjBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
            if(col == 2)
            {
                col = 0;
                row++;
            }
            BaseForm::mObjectTemplte.append(ObjBtn);
            comLayout->addWidget(ObjBtn,row,col++);
            //            connect(ObjBtn,SIGNAL(clicked(bool)),SLOT(onCreateCompoentToCanvas()));
        }
    }
}


void CompoentControls::onCreateCompoentToCanvas()
{
    // 被拖放替代,废弃了
    // QObject *sender = QObject::sender(); /* 确定的那一个按钮被点击了 */

    //这里要区分一下控件的类型,在没有图层的情况下,要先建图层,再在图层下面新建布局.
    QWidget *wid = ScenesScreen::mActiveObj;
    if(!wid)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }


    if(wid->inherits(CN_NEWLAYER) /*|| !clsname.compare(CN_LAYER)*/)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }

    DragButton *btn = (DragButton*)(QObject::sender());
    QJsonValue val =QJsonValue::fromVariant(btn->property(DKEY_JSONSTR));
    if(wid->inherits(CN_NEWFRAME)
            || wid->inherits(CN_NEWLIST)
            || wid->inherits(CN_NEWGRID))
    {
        //        ((NewLayout*)wid->parentWidget()->parentWidget())->readFromJson(val,true);
        ((NewLayout*)((BaseForm*)wid)->mParent)->readFromJson(val,true);
    }
    else {
        ((NewLayout*)wid)->readFromJson(val,true);
    }

}


void CompoentControls::onCreateNewLayout()
{
    // 被拖放替代,废弃了
    //布局只能创建在图层上.
    QWidget *w = ScenesScreen::mActiveObj;
    if(!w)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个图层或者新建一个图层,并选中它."));
        return;
    }

    //    QString clsname = w->metaObject()->className();
    DragButton *btn = (DragButton*)(QObject::sender());
    QVariant variant = btn->property(DKEY_JSONSTR);

    QJsonValue value = QJsonValue::fromVariant(variant);
    QRect oldrect = Compoent::getRectFromStruct(value.toObject()[PROPERTY].toArray(),KEY_RECT);
    if(oldrect.isEmpty())
    {
        oldrect.setWidth(200);
        oldrect.setHeight(200);
    }

    if(w->inherits(CN_NEWLAYOUT))
    {
        // 这里是在布局上面创建布局,嵌套.
        ((NewLayout*)w)->readFromJson(value,true);
    }
    else if(w->inherits(CN_NEWLAYER)/* || !CN_LAYER.compare(clsname)*/)
    {
        ((NewLayer*)w)->readLayoutFromJson(value,true);
    }else if(w->inherits(CN_NEWFRAME) || w->inherits(CN_NEWLIST))
    {
        //在当前控件的父控件上添加布局.
        NewLayout* layout = ((NewLayout*)((BaseForm*)w)->mParent);
        layout->readFromJson(value,true);
        //        ((NewLayout*)w->parentWidget()->parentWidget())->readFromJson(value,true);
    }
}

void CompoentControls::onCreateNewLayer()
{
    // 被拖放替代,废弃了
//    ScenesScreen *ss = CanvasManager::mActiveSS;
    if(!CanvasManager::mActiveSS)
        return;
    DragButton *btn = (DragButton*)(QObject::sender());
    QVariant variant = btn->property(DKEY_JSONSTR);
    CanvasManager::mActiveSS->createNewLayer( QJsonValue::fromVariant(variant),true);
}



DragButton::DragButton(const QJsonValue &value, QWidget *parent)
    :QPushButton(parent),
      mValue(value)
{
    QJsonObject jobj = value.toObject();
    QString caption = jobj[CAPTION].toString();
    QString objname = jobj[NAME].toString();
    QString clsname = jobj[CLASS].toString().prepend("T_");
    setObjectName(objname);
    QMetaEnum metaEnum = QMetaEnum::fromType<BaseForm::ObjTypes>();
    mFlag = metaEnum.keyToValue(clsname.toLocal8Bit().data());
    setText(caption);
}


void DragButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    QDrag *drag = new QDrag(this);
    QPixmap pixmap(size());
    render(&pixmap,QPoint(),QRegion(rect()));

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << mFlag << mValue.toVariant() << pixmap <<  event->pos() ;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(0,0));

    drag->exec();
}






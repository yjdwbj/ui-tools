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




Border::Border(QWidget *parent)
    :QGroupBox(parent),
      Left(new QSpinBox()),
      Top(new QSpinBox()),
      Right(new QSpinBox()),
      Bottom(new QSpinBox()),
      old(0)
{
    setTitle(tr("内边框线"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");
   // setFixedHeight((parent->width()-50)*0.1);
    QGridLayout *xywh = new QGridLayout();
    xywh->setSpacing(2);
    setLayout(xywh);
    xywh->addWidget(new QLabel("左:"),0,0);
    xywh->addWidget(Left,0,1);


    xywh->addWidget(new QLabel("上:"),1,0);
    xywh->addWidget(Top,1,1);

    xywh->addWidget(new QLabel("右:"),2,0);
    xywh->addWidget(Right,2,1);

    xywh->addWidget(new QLabel("下:"),3,0);
    xywh->addWidget(Bottom,3,1);
//    QSpacerItem *verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    xywh->addItem(verticalSpacer,4,0,0,1);

    Left->setObjectName(X);
    Top->setObjectName(Y);
    Right->setObjectName(H);
    Bottom->setObjectName(W);

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


    Left->setValue(((FormResizer*)com)->mBorder.x());
    Top->setValue(((FormResizer*)com)->mBorder.y());
    Right->setValue(((FormResizer*)com)->mBorder.width());
    Bottom->setValue(((FormResizer*)com)->mBorder.height());


    connections << QObject::connect(Left,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
    connections << QObject::connect(Top,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
    connections << QObject::connect(Right,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
    connections << QObject::connect(Bottom,SIGNAL(valueChanged(int)),com,SLOT(onBorderChangedValue(int)));
}


Position::Position(QWidget *parent)
    :QGroupBox(parent),
      Xpos(new QSpinBox()),
      Ypos(new QSpinBox()),
      Wpos(new QSpinBox()),
      Hpos(new QSpinBox()),
      old(0)
{
    setTitle(tr("位置坐标"));
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
    Wpos->setObjectName(W);
    Hpos->setObjectName(H);

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
    this->blockSignals(true);
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
    hb->setMargin(0);
    hb->setContentsMargins(0,4,0,0);
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
   // parseJsonToWidget(p,qvl,mainLayout);
    parseJsonToWidget(p, QJsonArray::fromVariantList(qvl));

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    oldobject = p;
}

void ComProperty::parseJsonToWidget(QWidget *p, const QJsonArray &array)
{


    QString className = p->metaObject()->className();
    foreach (QJsonValue item, array) {
        QWidget *wid = 0;
        //qDebug() << " item type " << item;
        switch (item.type()) {
        case QJsonValue::Array:
        {

        }
            break;
        case QJsonValue::Object:
        {
           QJsonObject object = item.toObject();
           QString uname = object[NAME].toString();
           QString caption = object[CAPTION].toString();
           QLabel *title = new QLabel(caption);

        //   parseJsonToWidget(p,object,layout);
           if(object.contains(STRUCT)) // 处理struct 关键字,QJsonArray
           {
               parseJsonToWidget(p,object[STRUCT].toArray());
           }else if(object.contains(ENUM)){

               QComboBox *cb = new QComboBox();
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
               connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onEnumItemChanged(QString)));
           }else if(object.contains(LIST))
           {
               QComboBox *cb = new QComboBox();
               cb->setObjectName(uname);
               cb->setProperty(DKEY_VALTYPE,LIST);
               // 这里通过它的JSON组数的位置去找它.
               QString cbkey =QString("%1_%2").arg(uname,QString::number(widgetMap.size()));
               widgetMap[cbkey] = cb;
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

           }else if(object.contains(KEY_RECT))
           {
           }else if(object.contains(BORDER))
           {
              QJsonObject border = object[BORDER].toObject();
              Border *b = new Border();
              b->setConnectNewQWidget(p);
              mainLayout->addWidget(b);
              QPushButton *btn = new QPushButton(tr("边框顔色"));
              mainLayout->addWidget(btn);
              btn->setObjectName(uname);
              connect(btn,SIGNAL(clicked(bool)),p,SLOT(onColorButtonClicked()));
              wid = btn;
           }else if(object.contains(UID))
           {
               mainLayout->addWidget(title);

               QLineEdit *nameEdt = new QLineEdit();
               nameEdt->setMaxLength(8);
               nameEdt->setInputMask("nnnnnnnn;"); // or NNNNNNNN;_
               nameEdt->setCursorPosition(0);
               mainLayout->addWidget(nameEdt);
//               id->setStyleSheet("*{border: 0.5px solid gray;}");
//               id->setFixedHeight(labHeight);
//               mainLayout->addWidget(id);
               // 这里是一个特殊属性,唯一序号
           }
           else if(object.contains(BAKIMAGE))
           {
               QPushButton *bkimage = new QPushButton("背景图片");
               mainLayout->addWidget(bkimage);
           }
           else if(object.contains(BAKCOLOR))
           {
               QPushButton *bkcolor = new QPushButton(tr("背景顔色"));
               bkcolor->setObjectName(uname);
               mainLayout->addWidget(bkcolor);
               connect(bkcolor,SIGNAL(clicked(bool)),p,SLOT(onColorButtonClicked()));
               wid = bkcolor;
           }else{
              //下面是通JSON值类型来区分来创建不同类型的控件.
               if(object[DEFAULT].isString())
               {

                   QLineEdit *txt = new QLineEdit(object[DEFAULT].toString());
                   txt->setObjectName(uname);
                   txt->setProperty(DKEY_VALTYPE,TEXT);

                   if(object.contains(MAXLEN))
                   {
                       txt->setMaxLength(object[MAXLEN].toDouble());
                   }

                   wid = txt;
                   // QLabel * l = new QLabel(uname);
                   mainLayout->addWidget(title);
                   mainLayout->addWidget(txt);
                   txt->setFixedHeight(25);
                   connect(txt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));


               }else if(object[DEFAULT].isDouble())
               {
                   int val = object[DEFAULT].toDouble();
                   mainLayout->addWidget(title);
                   QSpinBox *s = new QSpinBox();

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
            break;
        case QJsonValue::String:
            break;
        case QJsonValue::Double:
            break;
        default:
            break;
        }

        if(!wid)
            continue;
        if(!className.compare(CN_NEWFRAME))
        {
            ((NewFrame*)p)->onBindValue(wid,item.toObject().toVariantMap());
        }else{
            ((NewLabel*)p)->onBindValue(wid,item.toObject().toVariantMap());
        }

    }
}

void ComProperty::parseJsonToWidget(QWidget *p, const QJsonObject &json, QLayout *layout)
{
//    for(QJsonObject::const_iterator it = json.begin();it != json.end();++it)
//    {
//        QString key = it.key();
//        switch (it.value().type()) {
//        case QJsonValue::String:


//            break;
//        case QJsonValue::Object:
//            parseJsonToWidget(p,it.value().toObject());
//            break;
//        case QJsonValue::Array:
//            parseJsonToWidget(p,it.value().toArray());
//            break;
//        default:
//            break;
//        }
//    }
}
//void ComProperty::parseJsonToWidget(QWidget *p,const QVariantList &qvl,QLayout *layout)
//{

//     QString className = p->metaObject()->className();
//    foreach(QVariant qv, qvl)
//    {

//        QWidget *wid = 0;
//        QVariant val;

//        QVariantMap qvm = qv.toMap();
//        QString uname =  qvm[NAME].toString();
//        QString caption =  qvm[CAPTION].toString();
//        if(qvm.contains(ENUM))
//        {
//            QComboBox *cb = new QComboBox();
//            cb->setObjectName(uname);
//            cb->setProperty(DKEY_CAPTION,caption);
//            cb->setProperty(DKEY_VALTYPE,ENUM);
//            QVariantList qvlist = qvm[ENUM].toList();
//            for(QVariantList::const_iterator it = qvlist.begin();
//                it != qvlist.end();++it)
//            {
//                cb->addItem((*it).toMap().firstKey());
//            }
//            QLabel * l = new QLabel(uname);
//            mainLayout->addWidget(l);
//            mainLayout->addWidget(cb);
//            wid = cb;
//            connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onEnumItemChanged(QString)));

//        }else if(qvm.contains(LIST))
//        {
//            QComboBox *cb = new QComboBox();
//            cb->setObjectName(uname);
//            cb->setProperty(DKEY_VALTYPE,LIST);
//            // 这里通过它的JSON组数的位置去找它.
//            //  cb->setObjectName(LISTIMAGE);
//            //p->setProperty(DKEY_IMGIDX,0); // 当前选择的行号
//            QString cbkey =QString("%1_%2").arg(uname,QString::number(widgetMap.size()));
//            widgetMap[cbkey] = cb;
//            //  QString className = p->metaObject()->className();
//            wid = cb;
//            QLabel * l = new QLabel(uname);
//            mainLayout->addWidget(l);
//            QPushButton *b = new QPushButton(tr("添加图片"));
//            b->setObjectName(cbkey);
//            connect(b,SIGNAL(clicked(bool)),p,SLOT(onPictureDialog(bool)));
//            mainLayout->addWidget(b);
//            mainLayout->addWidget(cb);
//            // 绑定QComoBox的更改信号,更改它的值就要在相应的画版控件更新图片
//            connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onListImageChanged(QString)));
//        }else if(qvm.contains(IMAGE) ) /* 跳过这一行.*/
//        {

//        }else if(qvm.contains(STRUCT))
//        {
//            // 这里要做成一组属对应于一个结构体.
//            parseJsonToWidget(p,qvm[STRUCT].toList(),layout);

//        }
//        else{
//            if(uname.compare(GEOMETRY))
//            {

//                QVariant::Type t = qvm[DEFAULT].type();
//                QLabel *title = new QLabel(uname);
//                if(qvm.contains(UID))
//                {

//                    title->setFixedHeight(labHeight);

//                    mainLayout->addWidget(title);
//                    QLabel *id = new QLabel(p->property(DKEY_UID).toString());
//                    id->setStyleSheet("*{border: 0.5px solid gray;}");
//                    id->setFixedHeight(labHeight);
//                    mainLayout->addWidget(id);
//                    // 这里是一个特殊属性,唯一序号
//                }else if(qvm.contains(COLOR))
//                {
//                    mainLayout->addWidget(title);

//                }
//                else if(qvm.contains(BkIMAGE))
//                {

//                }
//                else if(t == QVariant::Int)
//                {

//                    mainLayout->addWidget(title);
//                    QSpinBox *s = new QSpinBox();

//                    s->setObjectName(uname);

//                    s->setProperty(DKEY_VALTYPE,NUMBER);
//                    //要保存每一次修改过的值.
//                    mainLayout->addWidget(s);
//                    wid = s;

//                    if(qvm.contains(MAX))
//                    {
//                        s->setMaximum(qvm[MAX].toInt());
//                    }
//                    if(qvm.contains(MIN))
//                    {
//                        s->setMinimum(qvm[MIN].toInt());
//                    }
//                    connect(s,SIGNAL(valueChanged(int)),p,SLOT(onNumberChanged(int)));
//                }
//                else if(t == QVariant::String ){
//                    QLineEdit *txt = new QLineEdit(qvm[DEFAULT].toString());
//                    txt->setObjectName(uname);
//                    txt->setProperty(DKEY_VALTYPE,TEXT);

//                    if(qvm.contains(MAXLEN))
//                    {
//                        txt->setMaxLength(qvm[MAXLEN].toDouble());
//                    }

//                    wid = txt;
//                    // QLabel * l = new QLabel(uname);
//                    mainLayout->addWidget(title);
//                    mainLayout->addWidget(txt);
//                    txt->setFixedHeight(25);
//                    connect(txt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));

//                }
//            }
//        }

//        if(!wid)
//            continue;
//        if(!className.compare(CN_NEWFRAME))
//        {
//            ((NewFrame*)p)->onBindValue(wid,qvm);
//        }else{
//            ((NewLabel*)p)->onBindValue(wid,qvm);
//        }

//    }


//    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
//    oldobject = p;

//}

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
    //:QWidget(parent),
      mainLayout(new QVBoxLayout()),
      mainWidget(new QWidget()),
      mCWidgetCount(0),
      mWindow(mw)
{

    // MainWindow *m ;
  //  this->setFixedHeight((parent->height()-50) * 0.15);
  //  setStyleSheet("{border: 1px solid gray;}");

    setTitle(tr("控件列表"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}"\
                  "QPushButton::hover{"\
                  "background: #F48024;}");
    //this->setLayout(mainLayout);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,2,2,2);
    mainLayout->setMargin(1);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
   // this->setFixedHeight(200);

   // mSizePolicy.setVerticalPolicy(QSizePolicy::Preferred);

    QVBoxLayout *hb = new QVBoxLayout(this);
   // hb->addWidget(new QLabel("控件列表"));
    hb->setSpacing(0);
    hb->setMargin(0);
    hb->setContentsMargins(0,4,0,0);
    this->setLayout(hb);

   // mainWidget = new QWidget();

    QScrollArea *scroll =  new QScrollArea();
   // scroll->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
   // scroll->setAlignment(Qt::AlignCenter);
   // scroll->setParent(this);

    hb->addWidget(scroll);
    scroll->setWidget(mainWidget);
    mainWidget->setLayout(mainLayout);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    // 读取控件目录下的所有控件文件.

   // mJsonFile =  QDir::currentPath() + "/menu_strip.json";
    QString file = QDir::currentPath() + "/control.json";
    qDebug() << " json file name " << file;
    QFileInfo qfi(file);
    if(!qfi.exists())
    {
        QMessageBox::warning(this,tr("错误"),tr("找不到控件文件"));
        return;
    }
    ReadTemplateWidgetFile(file);
    CreateButtonList();
 //   QString customdir = QDir::currentPath() + "/widgets";
    QDirIterator it(QDir::currentPath() + "/widgets", QStringList() << "*.json", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        // 读取每一个自定义控件的JSON文件
        ReadTemplateWidgetFile(it.next());
        mCWidgetCount++;
    }

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

void CompoentControls::ReadTemplateWidgetFile(QString file)
{
    QFile data(file);
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
//    QPushButton *layer = new QPushButton(tr("图层"));
//    connect(layer,SIGNAL(clicked(bool)),SLOT(onCreateNewLayer()));
//    layer->setFixedHeight(50);
//   // layer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    v->addWidget(layer);


//    QPushButton *l = new QPushButton(tr("布局"));

//    l->setFixedHeight(50);
//    //l->setSizePolicy(mSizePolicy);
//   // l->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    connect(l,SIGNAL(clicked(bool)),SLOT(onCreateNewLayout()));
//    v->addWidget(l);



    foreach (QJsonValue qjv, comJsonArr)
    {
        QVariantMap  qjm = qjv.toObject().toVariantMap();
        QString caption = qjv.toObject()[CAPTION].toString();
        QString objname = qjv.toObject()[NAME].toString();
        //  qDebug() << " json key is " << uname;
        comMap[caption] = qjm;
        QPushButton *btnTest = new QPushButton(caption);
        btnTest->setProperty(DKEY_CATEGORY,objname);
        btnTest->setProperty(DKEY_JSONSTR,qjv.toVariant()); // 这个按钮绑定这一个JSON控件.

       // btnTest->setSizePolicy(mSizePolicy);

        if(qjm.contains(ICON))
            btnTest->setIcon(QIcon(qjv.toObject()[ICON].toString()));

        QString wtype = qjv.toObject()[WTYPE].toString();
        if(!wtype.compare(CN_NEWLAYER)  || !wtype.compare(CN_LAYER))
        {
            v->addWidget(btnTest);
            btnTest->setFixedHeight(50);
            connect(btnTest,SIGNAL(clicked(bool)),SLOT(onCreateNewLayer()));
        }
        else if(!wtype.compare(CN_NEWLAYOUT) || !wtype.compare(CN_LAYOUT) )
        {
            v->addWidget(btnTest);
            btnTest->setFixedHeight(50);
            connect(btnTest,SIGNAL(clicked(bool)),SLOT(onCreateNewLayout()));

        }else{
             btnTest->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
            if(col == 2)
            {
                col = 0;
                row++;
            }
            comLayout->addWidget(btnTest,row,col++);
            connect(btnTest,SIGNAL(clicked(bool)),this,SLOT(onCreateCompoentToCanvas()));
        }
    }

}



void CompoentControls::onCreateCompoentToCanvas()
{
    // QObject *sender = QObject::sender(); /* 确定的那一个按钮被点击了 */

    //这里要区分一下控件的类型,在没有图层的情况下,要先建图层,再在图层下面新建布局.
   // NewLayout *activeLayout = mWindow->cManager->activeSS()->activeLayout();
    NewLayer  *activeLayer = mWindow->cManager->activeSS()->activeLayer();
    if(!activeLayer)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个图层或者新建一个并选中它."));
        return;
    }
    NewLayout *activeLayout = activeLayer->activeLayout();
    if(!activeLayout)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }

    QPushButton *btn = (QPushButton*)(QObject::sender());
    QJsonValue val =QJsonValue::fromVariant(btn->property(DKEY_JSONSTR));
    activeLayout->createNewFrameObject(val.toObject());
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
//    mWindow->tree->addObjectToCurrentItem(ww);
//    ww->show();

//   NewFrame* ww =  ReadObjectFromJson(comMap[btn->text()],activeLayout->m_frame,btn->text(),
//            btn->property(DKEY_CATEGORY).toString());
//    uint number = qHash(QDateTime::currentDateTime().toMSecsSinceEpoch());
//    ww->setProperty(DKEY_UID,QString::number(number));
//    ww->onSelectMe();
}

NewFrame* CompoentControls::ReadObjectFromJson(QVariantMap qvm, QObject *pobj,QString txt,QString objname)
{
//    NewLayout *activeLayout = mWindow->cManager->activeSS()->activeLayer()->activeLayout();
//    NewFrame* ww = (NewFrame *)CreateObjectFromJson(qvm,pobj);
//    //mWindow->Scenes->activeLayer()->m_frame);
//    ww->setObjectName(objname);
//    //ww->setObjectName(QString("%1_%2").arg(btn->text(),QString::number(comList.size())));
//    ww->setProperty(DKEY_LOCALSEQ,QString("%1_%2").arg(txt,QString::number(comList.size())));
//    //  activeLayout->appendChildObj(ww);
//    // 做treeWidget区分的名字.
//    ProMap[ww->property(DKEY_LOCALSEQ).toString()] = ww;
//    comList.append(ww);

//    // 找出图层,把新建的控件添加进去.
//    mWindow->tree->addObjectToCurrentItem(ww);
//    ww->show();
//    return ww;
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
                           // ((NewFrame*)nobj)->setFixedSize(r.size());

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
      //布局只能创建在图层上.
     NewLayer *activelayer = mWindow->cManager->activeSS()->activeLayer();
     if(!activelayer)
     {
         QMessageBox::warning(0,tr("提示"),tr("请选择一个图层或者新建一个图层,并选中它."));
         return;
     }
     activelayer->createNewLayout();





//    ScenesScreen *ss = mWindow->cManager->activeSS();
//    if(ss)
//    {
//        ss->createNewLayout();
//        QPushButton *btn = (QPushButton*)(QObject::sender());
//        NewLayout *nl =  ss->activeLayout();
//        ProMap[nl->property(DKEY_LOCALSEQ).toString()] = nl;
//        //comList.append(nl);
//        mWindow->tree->addItemToRoot(nl->property(DKEY_LOCALSEQ).toString(),btn->text());
//    }
}

void CompoentControls::onCreateNewLayer()
{
    ScenesScreen *ss = mWindow->cManager->activeSS();
    if(ss)
    {
        ss->createNewLayer();
        QPushButton *btn = (QPushButton*)(QObject::sender());

        NewLayer *nlayer =  ss->activeLayer();
        ProMap[nlayer->property(DKEY_LOCALSEQ).toString()] = nlayer;
        //comList.append(nl);
        mWindow->tree->addItemToRoot(nlayer->property(DKEY_LOCALSEQ).toString(),btn->text());
    }
}

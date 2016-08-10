#include "compoentcontrols.h"
//#include "handlejson.h"
#include "core_def.h"
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

    Left->setObjectName(LEFT);
    Top->setObjectName(TOP);
    Right->setObjectName(RIGHT);
    Bottom->setObjectName(BOTTOM);

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

    Left->setValue(((BaseForm*)com)->mBorder.left());
    Top->setValue(((BaseForm*)com)->mBorder.top());
    Right->setValue(((BaseForm*)com)->mBorder.right());
    Bottom->setValue(((BaseForm*)com)->mBorder.bottom());


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
    QString clsname = com->parentWidget()->metaObject()->className();

    QVariant variant = com->property(DKEY_INTOLIST);
    bool isLW = false;
    if(variant.isValid())
    {
        isLW = variant.toBool();
    }
    Xpos->setEnabled(!isLW);
    Ypos->setEnabled(!isLW);


  //  qDebug() << " net QWidget pos " << com->pos() << " size " << com->size();

    QSize psize = com->parentWidget()->size();
    Hpos->setMaximum(psize.height());
    Wpos->setMaximum(psize.width());
//    qDebug() << " xpos max " << Xpos->maximum()
//             << " set new xpos max " << psize.width() - com->width();
    Xpos->setMaximum(psize.width()-com->width());
    Ypos->setMaximum(psize.height()-com->height());

    // 一定要设置值再连接信号.
    Xpos->setValue(isLW ? 0 : com->pos().x());
    Ypos->setValue(isLW ? 0 : com->pos().y());
    if(!isLW)
    {
        connections << QObject::connect(Xpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
        connections << QObject::connect(Ypos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
    }

    Wpos->setValue(com->geometry().width());
    Hpos->setValue(com->geometry().height());

    // 非NewLayout控件大小不能调整
//    bool isLayout = !CN_NEWLAYOUT.compare(com->metaObject()->className());
//    Wpos->setEnabled(isLayout);
//    Hpos->setEnabled(isLayout);

    connections << QObject::connect(Wpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));
    connections << QObject::connect(Hpos,SIGNAL(valueChanged(int)),com,SLOT(onXYWHChangedValue(int)));

    old = com;
}

void Position::updatePosition(QPoint pos)
{
    Xpos->blockSignals(true);
    Ypos->blockSignals(true);
    Xpos->setValue(pos.x());
    Ypos->setValue(pos.y());

    Xpos->blockSignals(false);
    Ypos->blockSignals(false);
}

void Position::updateSize(QSize size)
{
    Wpos->blockSignals(true);
    Hpos->blockSignals(true);
    Wpos->setValue(size.width());
    Hpos->setValue(size.height());
    Wpos->blockSignals(false);
    Hpos->blockSignals(false);
//    this->blockSignals(false);
//    update();

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

ComProperty::ComProperty(QString title,QWidget *parent):
    QGroupBox(parent),
    mainLayout(new QVBoxLayout()),
    oldobject(0)

{
    setTitle(title);
    setStyleSheet("QGroupBox,QLabel,QScrollArea,QWidget{background-color: #C0DCC0;}");
    this->setFixedHeight((parent->height()-50) * 0.5);
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
             parseJsonToWidget(p,item.toArray());
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
        //   parseJsonToWidget(p,object,layout);
           if(object.contains(STRUCT)) // 处理struct 关键字,QJsonArray
           {
               parseJsonToWidget(p,object[STRUCT].toArray());
           }else if(object.contains(ENUM)){
               QLabel *title = new QLabel(caption);

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
               //QLabel *title = new QLabel(caption);
               QComboBox *cb = new QComboBox();
               cb->setObjectName(uname);
               cb->setProperty(DKEY_VALTYPE,LIST);
               // 这里通过它的JSON组数的位置去找它.
               QString cbkey =QString("%1_%2").arg(uname,QString::number(widgetMap.size()));
               widgetMap[cbkey] = cb;
               wid = cb;
               QPushButton *b = new QPushButton(tr("添加图片"));
               b->setObjectName(uname);
               connect(b,SIGNAL(clicked(bool)),p,SLOT(onPictureDialog(bool)));
               mainLayout->addWidget(b);
               mainLayout->addWidget(cb);
               QVariant nlv =  item.toObject().value(LIST);
               QVariantList imglist ;
               if(nlv.isValid() )
               {
                   // 处理图片列表与它的默认值
                   QVariantList nlist = nlv.toList();
                   QString defimg = item.toObject().value(DEFAULT).toString();
                //   int rootlen  = QDir::currentPath().length()+1;

                   defimg = defimg.replace("\\","/");
                   int sep = defimg.lastIndexOf('/') + 1;
                   defimg = defimg.mid(sep);

                   for(QVariantList::const_iterator it = nlist.begin();
                           it != nlist.end();++it)
                   {
                       // example for key  is  "config/images/string/alarm_pol.bmp"
                       QString key = (*it).toString();
                       bool b = key.contains('/');
                       int idx = key.lastIndexOf(b ? '/' : '\\')+1;
                     //  cb->addItem(QIcon(key),key.mid(idx));
                       imglist << QString("%1:%2").arg(key.mid(idx),key);
                   }

                   cb->setProperty(DKEY_IMGIDX,defimg);
                   p->setProperty(DKEY_IMAGELST,imglist);
                   cb->setProperty(DKEY_IMAGELST,imglist);

                  // changeJsonValue(LIST,nlv);
               }


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
              QPushButton *btn = new QPushButton(caption);
              mainLayout->addWidget(btn);
              btn->setObjectName(uname);

              connect(btn,SIGNAL(clicked(bool)),p,SLOT(onColorButtonClicked()));
              wid = btn;
           }else if(object.contains(UID))
           {
               QLabel *title = new QLabel(caption);
               mainLayout->addWidget(title);

               QLineEdit *nameEdt = new QLineEdit();
               nameEdt->setObjectName(uname);

               nameEdt->setMaxLength(8);
               nameEdt->setInputMask("nnnnnnnn;"); // or NNNNNNNN;_
               nameEdt->setCursorPosition(0);
               mainLayout->addWidget(nameEdt);
               connect(nameEdt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));
               wid = nameEdt;

               // 这里是一个特殊属性,唯一序号
           }
           else if(object.contains(BAKIMAGE))
           {
               QPushButton *bkimage = new QPushButton(caption);
               bkimage->setObjectName(uname);
               mainLayout->addWidget(bkimage);
                p->setProperty(DKEY_CURVAL,BAKIMAGE);
               connect(bkimage,SIGNAL(clicked(bool)),p,SLOT(onBackgroundImageDialog()));
           }
           else if(object.contains(BAKCOLOR))
           {
               QPushButton *bkcolor = new QPushButton(caption);
               bkcolor->setObjectName(uname);
               mainLayout->addWidget(bkcolor);
               p->setProperty(DKEY_CURVAL,BAKCOLOR);

               connect(bkcolor,SIGNAL(clicked(bool)),p,SLOT(onColorButtonClicked()));
               wid = bkcolor;
           }else{
              //下面是通JSON值类型来区分来创建不同类型的控件.
               if(object[DEFAULT].isString())
               {
                   if(!uname.compare(PIC_TEXT))
                   {
//                        QPushButton *imgbtn = new QPushButton(caption);
//                        imgbtn->setObjectName(uname);
//                        mainLayout->addWidget(imgbtn);
//                        connect(imgbtn,SIGNAL(clicked(bool)),p,SLOT(onTextSelected()));
//                        QString key = ((BaseForm*)p)->getJsonValue(PIC_TEXT).toString();
//                        QString val =   ((BaseForm*)p)->mWindow->itemMap[key];
//                        imgbtn->setText(val);
//                        wid = imgbtn;

                       QLabel *title = new QLabel(caption);

                       QComboBox *cb = new QComboBox();
                       cb->setObjectName(uname);
                     //  cb->setProperty(DKEY_CAPTION,caption);
                     //  cb->setProperty(DKEY_VALTYPE,ENUM);
                      // QVariantList qvlist = object[ENUM].toArray().toVariantList();
                       QString defstr = object[DEFAULT].toString().toLower();


                       foreach (QString key, ((BaseForm*)p)->mWindow->mOrderlist) {
                            cb->addItem( ((BaseForm*)p)->mWindow->mItemMap[key] );
                       }
                       cb->setCurrentIndex(((BaseForm*)p)->mWindow->mOrderlist.indexOf(defstr));
//                       for(QVariantList::const_iterator it = qvlist.begin();
//                           it != qvlist.end();++it)
//                       {
//                           cb->addItem((*it).toMap().firstKey());
//                       }

                       mainLayout->addWidget(title);
                       mainLayout->addWidget(cb);
                       wid = cb;
                       connect(cb,SIGNAL(currentTextChanged(QString)),p,SLOT(onEnumItemChanged(QString)));
                   }else{

                       QLineEdit *txt = new QLineEdit(object[DEFAULT].toString());
                       txt->setObjectName(uname);
                       txt->setProperty(DKEY_VALTYPE,TEXT);

                       if(object.contains(MAXLEN))
                       {
                           txt->setMaxLength(object[MAXLEN].toDouble());
                       }

                       wid = txt;
                       // QLabel * l = new QLabel(uname);
                       QLabel *title = new QLabel(caption);
                       mainLayout->addWidget(title);
                       mainLayout->addWidget(txt);
                       txt->setFixedHeight(25);
                       connect(txt,SIGNAL(textChanged(QString)),p,SLOT(onTextChanged(QString)));
                   }


               }else if(object[DEFAULT].isDouble())
               {
                   QLabel *title = new QLabel(caption);
                   //int val = object[DEFAULT].toDouble();
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
         ((BaseForm*)p)->onBindValue(wid,item.toObject().toVariantMap());

    }
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
    //:QWidget(parent),
      mainLayout(new QVBoxLayout()),
      mainWidget(new QWidget()),
      mCWidgetCount(0),
      mWindow(mw)
{

    setTitle(tr("控件列表"));
    setStyleSheet("QGroupBox,QLabel{background-color: #C0DCC0;}");\

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

    CreateButtonList(ReadTemplateWidgetFile(file));
 //   QString customdir = QDir::currentPath() + "/widgets";

    // 读取一些自定义的控件.
    QDirIterator it(QDir::currentPath() + "/widgets", QStringList() << "*.json", QDir::Files, QDirIterator::Subdirectories);

    QJsonArray array;
    while (it.hasNext())
    {
        // 读取每一个自定义控件的JSON文件
        foreach (QJsonValue val, ReadTemplateWidgetFile(it.next())) {
            array.append(val);
        }
        mCWidgetCount++;
    }
    mainLayout->addWidget(createCustomObject(array));
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

QJsonArray CompoentControls::ReadTemplateWidgetFile(QString file) const
{
    QJsonArray obj;
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
                if(qd.object().contains(COMPOENTS))
                {
                   obj = qd.object()[COMPOENTS].toArray();
                }
            }
        }else{
            // qDebug() << " read Json file error";
            qDebug() << json_error.errorString();
        }
    }
    return obj;
}


QWidget* CompoentControls::createCustomObject(const QJsonArray &comJsonArr)
{

    QGroupBox *gb = new QGroupBox(tr("自定义控件"));
    QGridLayout *comLayout = new QGridLayout();
    comLayout->setMargin(0);
    comLayout->setSpacing(2);
    comLayout->setContentsMargins(1,5,1,0);
    gb->setLayout(comLayout);
    int row,col = 0;

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
        btnTest->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        if(col == 2)
        {
            col = 0;
            row++;
        }

        // 这样能把tooltip显示成一张图片.
        btnTest->setToolTip( QString("<img src='%1'>").arg(qjv.toObject()[ICON].toString()));
//        btnTest->setStyleSheet("QPushButton::hover{"\
//        "background: #F48024;}");
        comLayout->addWidget(btnTest,row,col++);
        // 这里要创建单独处理自定义控件的函数.
        connect(btnTest,SIGNAL(clicked(bool)),this,SLOT(onCreateCustomWidget()));
    }
    return gb;
}


void CompoentControls::onCreateCustomWidget()
{


    QWidget *wid = mWindow->cManager->activeSS()->activeObject();
    if(!wid)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }
    QString clsname = wid->metaObject()->className();
    if(!clsname.compare(CN_NEWLAYER) /*|| !clsname.compare(CN_LAYER)*/)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }
    QPushButton *btn = (QPushButton*)(QObject::sender());
    QJsonValue value = QJsonValue::fromVariant(btn->property(DKEY_JSONSTR));

   // QString clsname = wid->metaObject()->className();
    if(!clsname.compare(CN_NEWLAYOUT))
    {
       ((NewLayout*)wid)->readFromJson(value);
       // ((NewLayout*)wid)->readFromJson(value.toArray());
    }
    else if(!clsname.compare(CN_NEWFRAME))
    {
        // 选择它的父控件.
       ((NewLayout*)wid->parentWidget())->readFromJson(value);
    }
    else
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }


}




void CompoentControls::CreateButtonList(const QJsonArray &comJsonArr)
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

    foreach (QJsonValue qjv, comJsonArr)
    {
        QVariantMap  qjm = qjv.toObject().toVariantMap();
        QString caption = qjv.toObject()[CAPTION].toString();
        QString objname = qjv.toObject()[NAME].toString();

        QString clsname = qjv.toObject()[CLASS].toString();
        if(!CN_NEWLAYOUT.compare(clsname) /*||!CN_LAYOUT.compare(clsname)*/)
        {
            Layout = qjv.toObject().value(PROPERTY).toVariant();
        }
        //  qDebug() << " json key is " << uname;
        comMap[caption] = qjm;
        QPushButton *btnTest = new QPushButton(caption);
        btnTest->setProperty(DKEY_CATEGORY,objname);
        btnTest->setProperty(DKEY_JSONSTR,qjv.toVariant()); // 这个按钮绑定这一个JSON控件.

       // btnTest->setSizePolicy(mSizePolicy);

        if(qjm.contains(ICON))
            btnTest->setIcon(QIcon(qjv.toObject()[ICON].toString()));

        //QString wtype = qjv.toObject()[WTYPE].toString();
        QString wtype = qjv.toObject()[CLASS].toString();
        if(!wtype.compare(CN_NEWLAYER) /* || !wtype.compare(CN_LAYER)*/)
        {
            v->addWidget(btnTest);
            btnTest->setFixedHeight(50);
            connect(btnTest,SIGNAL(clicked(bool)),SLOT(onCreateNewLayer()));
        }
        else if(!wtype.compare(CN_NEWLAYOUT) /*|| !wtype.compare(CN_LAYOUT)*/ )
        {
            v->addWidget(btnTest);
            btnTest->setFixedHeight(50);
            connect(btnTest,SIGNAL(clicked(bool)),SLOT(onCreateNewLayout()));
        }
        else{
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
    QWidget *wid = mWindow->cManager->activeSS()->activeObject();

//    if(!wid || clsname.compare(CN_NEWLAYOUT))
    //QString clsname;
    if(!wid)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }

    QString clsname = wid->metaObject()->className();
    if(!clsname.compare(CN_NEWLAYER) /*|| !clsname.compare(CN_LAYER)*/)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个布局或者新建一个并选中它."));
        return;
    }

    QPushButton *btn = (QPushButton*)(QObject::sender());
    QJsonValue val =QJsonValue::fromVariant(btn->property(DKEY_JSONSTR));
   // QString clsname = wid->metaObject()->className();
    if(!clsname.compare(CN_NEWFRAME) || !clsname.compare(CN_NEWLIST))
    {
        ((NewLayout*)wid->parentWidget()->parentWidget())->readFromJson(val);
    }
    else {
    //    ((NewLayout*)wid)->createNewFrameObject(val.toObject());
        ((NewLayout*)wid)->readFromJson(val);
    }

}


void CompoentControls::onCreateNewLayout()
{
      //布局只能创建在图层上.
    QWidget *w = mWindow->cManager->activeSS()->activeObject();
    if(!w)
    {
        QMessageBox::warning(0,tr("提示"),tr("请选择一个图层或者新建一个图层,并选中它."));
        return;
    }


    QString clsname = w->metaObject()->className();

    QPushButton *btn = (QPushButton*)(QObject::sender());
    QVariant variant = btn->property(DKEY_JSONSTR);

    QJsonValue value = QJsonValue::fromVariant(variant);
    QRect oldrect = Compoent::getRectFromStruct(value.toObject()[PROPERTY].toArray(),KEY_RECT);
    if(oldrect.isEmpty())
    {
        oldrect.setWidth(200);
        oldrect.setHeight(200);
    }

    if(/*!CN_LAYOUT.compare(clsname)|| */!CN_NEWLAYOUT.compare(clsname))
    {
        // 这里是在布局上面创建布局,嵌套.
        ((NewLayout*)w)->readFromJson(value);
    }
    else if(!CN_NEWLAYER.compare(clsname)/* || !CN_LAYER.compare(clsname)*/)
    {
       ((NewLayer*)w)->readFromJson(value);
    }else if(!CN_NEWFRAME.compare(clsname) || !CN_NEWLIST.compare(clsname))
    {
        ((NewLayout*)w->parentWidget()->parentWidget())->readFromJson(value);
    }

}

void CompoentControls::onCreateNewLayer()
{
    ScenesScreen *ss = mWindow->cManager->activeSS();
    if(ss)
    {
        QPushButton *btn = (QPushButton*)(QObject::sender());
        QVariant variant = btn->property(DKEY_JSONSTR);
        QJsonObject json = QJsonValue::fromVariant(variant).toObject();
        NewLayer *nlayer  = ss->createNewLayer(json);       
       // mWindow->tree->addItemToRoot(nlayer);
    }
}

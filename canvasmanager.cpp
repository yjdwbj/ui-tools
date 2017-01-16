#include "canvasmanager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scenesscreen.h"
#include "pageview.h"
#include "mydialog.h"
#include "module.h"
#include <QStackedLayout>
#include <QString>
#include <QStyleFactory>
#include <QComboBox>
#include <QTimer>
#include <QFileDialog>
#include <QTextBrowser>
#include <functional>
#include <QThreadPool>

extern "C"  {
int ffmpeg(int argc, char **argv);
}




CanvasManager::CanvasManager(MainWindow *w):
    mWindow(w),stack(new QStackedLayout()),
    mPageSize(0,0),
    newPrj(new QPushButton(tr("新建工程"))),
    newPage(new QPushButton(tr("新建页面"))),
    delPage(new QPushButton(tr("删除当前页"))),
    savePrj(new QPushButton(tr("保存工程"))),
    confPrj(new QPushButton(tr("工程设置"))),
    saveas(new QPushButton(QIcon(":/icon/icons/document-save-as.png"),"另存为")),
    mProjectWidgetDir(QDir::currentPath().replace(SLASH,BACKSLASH) + BACKSLASH + "widgets"),
    mPrjIsChanged(false),
    mIsOpenProject(false),
    autoSaveTimer(new QTimer(this))
{
    // w->ui->centralWidget;
    newPage->setEnabled(false);
    delPage->setEnabled(false);
    savePrj->setEnabled(false);
    saveas->setEnabled(false);
    confPrj->setEnabled(false);

    QComboBox *cb = new QComboBox();

    QPushButton *openPrj = new QPushButton("打开工程");
    QPushButton *globalbtn = new QPushButton("全局设置");

    QPushButton *recordbtn = new QPushButton(QIcon(":/icon/icons/player_play.png"),"录屏");
    QPushButton *aboutbtn = new QPushButton(QIcon(":/icon/icons/mode_help@2x.png"),"关于");
    cb->addItems(QStyleFactory::keys());

    mWindow->addWidgetToToolBar(newPrj);
    mWindow->addWidgetToToolBar(openPrj);
    mWindow->addWidgetToToolBar(Q_NULLPTR);
    mWindow->addWidgetToToolBar(savePrj);
    mWindow->addWidgetToToolBar(saveas);
    mWindow->addWidgetToToolBar(Q_NULLPTR);
    mWindow->addWidgetToToolBar(newPage);
    mWindow->addWidgetToToolBar(delPage);
    mWindow->addWidgetToToolBar(Q_NULLPTR);

    mWindow->addWidgetToToolBar(recordbtn);

    mWindow->addWidgetToToolBar(Q_NULLPTR);
    //  mWindow->addWidgetToToolBar(cb);
    mWindow->centralWidget()->setLayout(stack);
    // 按屏幕的大小比例调整.

    connect(newPrj,SIGNAL(clicked(bool)),SLOT(onCreateNewProject()));
    connect(newPage,SIGNAL(clicked(bool)),SLOT(onCreateNewScenesScreen()));
    connect(delPage,SIGNAL(clicked(bool)),SLOT(onDelCurrentScenesScreen()));
    connect(savePrj,SIGNAL(clicked(bool)),SLOT(onSaveProject()));
    connect(confPrj,SIGNAL(clicked(bool)),SLOT(onConfProject()));
    newPage->setIcon(QIcon(":/icon/icons/canvas-diagram.png"));
    newPrj->setIcon(QIcon(":/icon/icons/category_vcs.png"));
    savePrj->setIcon(QIcon(":/icon/icons/document-save-as.png"));
    delPage->setIcon(QIcon(":/icon/icons/removesubmitfield.png"));


    globalbtn->setIcon(QIcon(":/icon/icons/preferences-system.png"));
    connect(globalbtn,&QPushButton::clicked,[=](){

        while (1){
            GlobalSettings gs(mWindow);
            gs.setWindowTitle(globalbtn->text());
            gs.exec();
            if (gs.isSetFine()) break;
        }
    });
    mWindow->addWidgetToToolBar(globalbtn);


    //将定时器超时信号与槽(功能函数)联系起来

    connect( autoSaveTimer,&QTimer::timeout,[=](){
        saveProject("autosave.json");
    } );

    connect(openPrj,SIGNAL(clicked(bool)),SLOT(onOpenProject()));

    connect(saveas,SIGNAL(clicked(bool)),SLOT(onSaveAsProject()));


    mWindow->addWidgetToToolBar(Q_NULLPTR);
    mWindow->addWidgetToToolBar(aboutbtn);
    connect(aboutbtn,&QPushButton::clicked,[=](){
        QDialog *aboutdlg  = new QDialog(mWindow);
        aboutdlg->setWindowTitle(WIN_TITLE);
        aboutdlg->setFixedSize(430,360);
        aboutdlg->setModal(true);
        QLabel *label = new QLabel(aboutbtn);
        QVBoxLayout *vbox = new QVBoxLayout();
        //vbox->setContentsMargins(0,0,0,0);
        // vbox->setMargin(0);
        vbox->setSpacing(0);
        aboutdlg->setLayout(vbox);
        vbox->addWidget(label);
        QString txt = QString("<b><img src=':/icon/icons/QtBareMetal.png'></b>"\
                              "<b><p>版本:</p><p> %1 </p></b>").arg(VERSION);
        label->setText(txt);
        aboutdlg->exec();
        aboutdlg->deleteLater();
    });

    //开始运行定时器，定时时间间隔为6000ms

    char  outputvideo[] = "video.avi";
    char palettepng[] = "palettepng";
    connect(recordbtn,&QPushButton::clicked,[=]{


        if(mRecordFuture.isRunning())
        {
            mRecordFuture.cancel();
            mRecordFuture.waitForFinished();

            recordbtn->setEnabled(false);
            recordbtn->repaint();
            QList<QString> tmp;
            tmp << "ffmpeg" << "-y" << "-i" << outputvideo
                << "-vf" << "fps=10,palettegen=stats_mode=diff" << palettepng;

            int asize = tmp.size();
            char **pngargv = new char*[asize];
            for(int i = 0 ; i < asize;i++)
            {
                pngargv[i] = tmp.at(i).toLocal8Bit().data();
            }

            int argc = sizeof(pngargv)/sizeof(char *);
            QFuture<void> genpalette =QtConcurrent::run(ffmpeg,argc,pngargv);
            genpalette.waitForFinished();


            QList<QString> t;
            t     << "ffmpeg" << "-y" << "-i"
                  << outputvideo << "-i" << palettepng << "-r" << "5"
                  << "-lavfi" << "paletteuse=dither=floyd_steinberg"
                  <<"output.gif";

            int s = t.size();
            char **gifargv = new char*[s];
            for(int i =0 ; i < s;i++)
            {
                gifargv[i] = t[i].toLocal8Bit().data();
            }



            QFuture<void> gifpalette = QtConcurrent::run(ffmpeg,s,gifargv);

            gifpalette.waitForFinished();
            recordbtn->setEnabled(true);
            recordbtn->setText("录屏");
            recordbtn->setIcon(QIcon(":/icon/icons/player_play.png"));
            recordbtn->repaint();
        }else{

            recordbtn->setText("停止");
            recordbtn->setIcon(QIcon(":/icon/icons/player_stop.png"));
            recordbtn->repaint();
            int x = mWindow->rect().size().width();
            int y = mWindow->rect().size().height();
            QString wsize;
            wsize.sprintf("%dx%d",x,y);


#ifdef __WIN32
            QList<char *> tmp;
            tmp << "ffmpeg" << "-y" << "-f" << "gdigrab"
                << "-video_size" << wsize.toLocal8Bit().data() << "-framerate"
                << "25" << "-i" << "desktop" << "-vcodec" << "huffyuv" << outputvideo;
#else

            QList<char *> tmp;
            tmp << "ffmpeg" << "-v" << "error" << "-y" << "-f" << "x11grab" << "-t" << "10"
                << "-video_size" << wsize.toLocal8Bit().data() << "-framerate"
                << "25" << "-i" << QString(":0.0").toLocal8Bit().data() << "-vcodec"  << "huffyuv"
               << "-pix_fmt" << "yuv422p"  << QString(outputvideo).toLocal8Bit().data();


#endif
            int asize = tmp.size();
            char **argv = new char*[asize];
            for(int i = 0;i < asize;i++)
            {
                argv[i] = tmp[i];
            }




            mRecordFuture =  QtConcurrent::run(ffmpeg,asize,argv);


        }



    });


}


void CanvasManager::screenshot()
{
    // 对原来的那个页面进行截屏
    if(mCanvasList.size())
    {
        QWidget *wd = stack->currentWidget();
        if(wd)
        {
            // 匿名函数测试对像的子对像.
            //            std::function<void(QWidget*)> test_func =  [&](QWidget* obj){
            //                foreach( QObject *t, obj->children())
            //                {
            //                    qDebug() << " class name " << t->metaObject()->className()
            //                             << " widget name  "  << ((QWidget*)t)->objectName();
            //                    test_func((QWidget*)t);
            //                }
            //            };
            //            test_func(stack->currentWidget());

            QPixmap pixmap(stack->currentWidget()->size());
            stack->currentWidget()->render(&pixmap,QPoint(),QRegion(stack->currentWidget()->rect()));
            QVariant vp = stack->currentWidget()->property(DKEY_SHOT);
            if( vp.isValid() && vp.toBool() == false)
            {
                mWindow->pageView->addNewPage(pixmap,
                                              stack->currentWidget()->property(DKEY_TXT).toString());

                //pixmap.save("test.png");　//　这里如果有需要可以保存成文件.
                stack->currentWidget()->setProperty(DKEY_SHOT,true);
            }else{

                int index = stack->currentIndex();
                mWindow->pageView->delPage(index); // 删除当前的,更新每新的.
                mWindow->pageView->InsertPage(index,pixmap,stack->currentWidget()->property(DKEY_TXT).toString());
            }
        }

    }

}

ScenesScreen * CanvasManager::createNewCanvas()
{
    screenshot();
    ScenesScreen *Scenes = new ScenesScreen(mPageSize,(QWidget*)mWindow);

    Scenes->addMainWindow(mWindow);
    Scenes->move(mWindow->width() * 0.12,mWindow->height()* 0.3);  // 按屏幕比例调整
    Scenes->setProperty(DKEY_SHOT,false);  // 检查该页面是否创建过截图.


    // 这里不能改变它的对像名,用一个动态属
    //    // Scenes->setObjectName(QString("Page_%1").arg(QString::number(ssList.size()-1)));
    Scenes->setProperty(DKEY_TXT,QString("页面_%1").arg(QString::number(mCanvasList.size())));
    Scenes->setToolTip(Scenes->property(DKEY_TXT).toString());
    mCanvasList.append(Scenes);
    currentSS = Scenes;
    mWindow->lDock->setEnabled(true);
    stack->addWidget(Scenes);
    stack->setCurrentWidget(Scenes);
    // stack->setGeometry(stackRect);
    // 清理treeWidget 的行
    mWindow->tree->deleteAllitem();
    screenshot();
    return Scenes;
}

ScenesScreen *CanvasManager::activeSS()
{
    return (ScenesScreen*)(stack->currentWidget());
}

int CanvasManager::activeIndex()
{


    return stack->currentIndex();
}

void CanvasManager::setActiveSS(int index)
{
    if(index == -1) return;
    if(index < mCanvasList.size())
    {

        stack->setCurrentIndex(index);
        // 清理treeWidget 的行
        mWindow->tree->deleteAllitem();
        ScenesScreen *Scenes = (ScenesScreen*)(stack->currentWidget());
        // 把当前页的布局重新添加到treeWidget上
        foreach (QWidget *w, Scenes->childlist) {
            // QString key = w->property(DKEY_LOCALSEQ).toString();

            mWindow->tree->addItemToRoot(w);
            if(!w->isHidden())
                ((BaseForm*)w)->addChildrenToTree();


        }

        screenshot();
        //  stack->setGeometry(stackRect);
    }

}


void CanvasManager::deleteCurrentPage()
{
    ScenesScreen *ss = this->activeSS();
    if(ss)
    {
        int index = stack->currentIndex();
        stack->removeWidget(ss);
        mWindow->pageView->delPage(index);
        mWindow->tree->deleteAllitem();
        mCanvasList.removeAt(index);

        ss->delAllObjects();
        delPage->setEnabled(stack->count() == 0 ? false : true);
        mWindow->lDock->setEnabled(stack->count() == 0 ? false : true);
        setActiveSS(stack->currentIndex());
    }
}

void CanvasManager::onDelCurrentScenesScreen()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("删除提示");
    msgBox.setText("你真的要删除当前页面吗?删除之后不可以撤消,请选择<删除>删除.");
    // msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Yes,"删除");
    msgBox.setButtonText(QMessageBox::Cancel,"取消");
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    //  qDebug() << " QMessageBox result " << ret;
    if(ret == QMessageBox::Yes)
    {

        deleteCurrentPage();
    }

}

void CanvasManager::onCreateNewProject()
{


    if(mPrjIsChanged)
    {
        // 当前工程有修改还没有保存.
        QMessageBox msgBox;
        msgBox.setWindowTitle("新建工程提示");
        msgBox.setText("当前编辑的工程有新的修改没有保存,选请择<保存>进行保存.");
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"保存");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //qDebug() << " QMessageBox result " << ret;
        if(ret == QMessageBox::Yes)
        {
            //　需要保存
            onSaveProject();
        }
    }


    closeCurrentProject(); // 关闭当前工程.
    ProjectDialog *pd = new ProjectDialog(mWindow);
    pd->exec();
    // qDebug() << " ProjectDialog result " << pd->result();
    newPage->setEnabled(pd->result());
    savePrj->setEnabled(pd->result());
    saveas->setEnabled(pd->result());
    confPrj->setEnabled(pd->result());
    pd->deleteLater();
    if(pd->result())
    {

        onCreateNewScenesScreen();
        autoSaveTimer->start(60000);
    }

}


void CanvasManager::OpenProject(QString file)
{
    QFile PrjFile(file);
    if (PrjFile.open(QFile::ReadOnly|QIODevice::Text)) {
        QByteArray qba = PrjFile.readAll();
        QTextStream in(&PrjFile);
        QString str;
        int ans = 0;
        in >> str >> ans;
        QJsonParseError json_error;
        QJsonDocument qd = QJsonDocument::fromJson(qba,&json_error);

        if(json_error.error == QJsonParseError::NoError)
        {
            if(qd.isObject())
            {
                QJsonObject  qdobj = qd.object();
                mProjectName = qdobj[NAME].toString();
                mWindow->setWindowTitle( VERSION + mProjectName);
                foreach (QJsonValue val,qdobj[MLANG].toArray() ) {
                    mPrjSelectlang.append(val.toString());
                }
                readProjectJson(qdobj[PAGES].toArray());
                setActiveSS(qdobj[ACTPAGE].toInt());
                mPrjIsChanged=true;
                mIsOpenProject = true;
            }
        }else{
            // qDebug() << " read Json file error";
            qDebug() << json_error.errorString();
        }


    }
}

void CanvasManager::onOpenProject()
{
    if(mIsOpenProject)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("打开工程提示");
        msgBox.setText("当前编辑的工程有新的修改没有保存,选请择<保存>进行保存.");
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"保存");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        //qDebug() << " QMessageBox result " << ret;
        if(ret == QMessageBox::Yes)
        {
            //　需要保存
            onSaveProject();
        }

    }
    closeCurrentProject();
    QString pfile = QFileDialog::getOpenFileName(mWindow,
                                                 tr("打开工程文件"),
                                                 QDir::currentPath(),
                                                 tr("json files, json file (*.json)"));

    if(!pfile.isEmpty())
    {
        OpenProject(pfile);
        autoSaveTimer->start(60000);
        //        screenshot();
    }
}


void CanvasManager::closeCurrentProject()
{
    foreach (QWidget *w, mCanvasList) {
        deleteCurrentPage();
    }
    mWindow->ComCtrl->ProMap.clear();
    mIsOpenProject = false;
    autoSaveTimer->stop();
}

void CanvasManager::onCreateNewScenesScreen()
{
    mIsOpenProject = true;
    mPrjIsChanged = true;
    createNewCanvas();
    delPage->setEnabled(true);
    mWindow->propertyWidget->delPropertyBox();
}


void CanvasManager::onConfProject()
{
    ConfigProject cp(mWindow);
    cp.exec();
    // int n = cp.result();
    if(cp.result())  // accpet 就保存语言.
        mPrjSelectlang = cp.getSelectLang();

}


void CanvasManager::saveProject(QString fname)
{
    QFile saveFile(fname);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");

    }

    QJsonArray CanvasArray;
    // QJsonObject root;
    foreach (QWidget *w, mCanvasList) {
        CanvasArray.append(((ScenesScreen*)w)->writeToJson());
    }
    QJsonObject obj ;
    obj[NAME] = mProjectName;
    obj[ACTPAGE] = stack->currentIndex();
    obj[PAGES] = CanvasArray;
    obj[WTYPE] = "project";
    QJsonArray lang;
    foreach (QString v ,mPrjSelectlang) {
        QJsonValue val = v;
        lang.append(val);
    }
    obj[MLANG] = lang;
    QJsonDocument jsonDoc(obj);
    saveFile.write(jsonDoc.toJson());
    mIsOpenProject = false;
}

void CanvasManager::onSaveAsProject()
{
    QVariant prjvar = mWindow->mGlobalSet->value(INI_PRJDIR);
    QString fname = QFileDialog::getSaveFileName(mWindow,
                                                 tr("保存工程文件"),
                                                 prjvar.toString(),
                                                 tr("json files, json file (*.json)"));

    if(!fname.isEmpty())
    {
        if(!fname.endsWith(".json"))
        {
            fname = fname+".json";
        }
        mWindow->mGlobalSet->setValue(INI_PRJLAST,fname.toUtf8());
        saveProject(fname);
    }
}

void CanvasManager::onSaveProject()
{

    QVariant prjvar = mWindow->mGlobalSet->value(INI_PRJDIR);

    QString fname;
    if(prjvar.isValid())
    {
        fname = prjvar.toString() + BACKSLASH + mProjectName + ".json";
    }
    else
    {
        fname = QDir::currentPath() + BACKSLASH+ "save.json" ;
    }

    mProjectFullPath = fname;
    mWindow->mGlobalSet->setValue(INI_PRJLAST,fname.toUtf8());
    saveProject(fname);


}



void CanvasManager::readProjectJson(const QJsonArray &array)
{
    bool readflag = false;
    foreach (QJsonValue val, array) {
        readflag = true;
        // QVariantMap  qjm = val.toObject().toVariantMap();
        // 创建一个页面.
        switch (val.type()) {
        case QJsonValue::Object:
        {
            int w,h;
            QJsonObject valobj = val.toObject();
            foreach (QJsonValue pval, valobj[PROPERTY].toArray()) {
                QJsonObject pobj = pval.toObject();
                if(pobj.contains(KEY_RECT))
                {
                    w = pobj[KEY_RECT].toObject()[WIDTH].toInt();
                    h = pobj[KEY_RECT].toObject()[HEIGHT].toInt();
                    break;
                }
            }
            setDefaultPageSize(QSize(w,h));
            ScenesScreen *Scenes = createNewCanvas();
            // 递归读取它的页面.
            Scenes->readLayer(valobj[LAYER].toArray());
        }
            break;
        default:
            break;
        }
    }
    newPage->setEnabled(readflag);
    savePrj->setEnabled(readflag);
    saveas->setEnabled(readflag);
    delPage->setEnabled(readflag);
    confPrj->setEnabled(readflag);
}

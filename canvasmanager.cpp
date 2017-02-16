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
#include <QTimer>
#include <QMetaObject>
#include <QKeyEvent>
#include <signal.h>
#include <QtConcurrent>



extern "C"  {
int ffmpeg(int ,char **);
int sigterm_handler(int sig);
//需要把DEBUG打印到文件就用它来设置回调.
void av_log_set_callback(void (*callback)(void*, int, const char*, va_list));
}

static QByteArray outputvideo = "video.avi";
static QByteArray palettepng = "palette.png";
static QString outputgif = "output.gif";
static QString screenshootpng = "screenshoot.png";
static QString gifFileName = outputgif;




//QSlider*  CanvasManager::mHSlier = NULL;
//QSlider*  CanvasManager::mVSlier = NULL;

ScenesScreen* CanvasManager::mActiveSS = NULL;

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
//    mPrjIsChanged(false),
    mIsOpenProject(false),
    autoSaveTimer(new QTimer(this)),
    mFFmpegRuning(false)
{
    // w->ui->centralWidget;
    newPage->setEnabled(false);
    delPage->setEnabled(false);
    savePrj->setEnabled(false);
    saveas->setEnabled(false);
    confPrj->setEnabled(false);

//    if(!mVSlier)
//        mVSlier = new QSlider(Qt::Vertical,w->centralWidget());
//    if(!mHSlier)
//        mHSlier = new QSlider(Qt::Horizontal,w->centralWidget());


    QComboBox *cb = new QComboBox();

    QPushButton *openPrj = new QPushButton("打开工程");
    QPushButton *globalbtn = new QPushButton("全局设置");

    QPushButton *recordbtn = new QPushButton(QIcon(":/icon/icons/player_play.png"),"录屏");
    recordbtn->setToolTip("录取当前程序大小的窗口区域,最终保存成GIF图片");
    QPushButton *sshoot = new QPushButton(QIcon(":/icon/icons/Screenshot.png"),"截屏");
    sshoot->setToolTip("截取程序的界面,并保存成PNG图片");
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
    mWindow->addWidgetToToolBar(sshoot);
    mWindow->addWidgetToToolBar(recordbtn);

    mWindow->addWidgetToToolBar(Q_NULLPTR);
    //  mWindow->addWidgetToToolBar(cb);
//    QGridLayout *glayout = new QGridLayout();
//    mHSlier->setTickPosition(QSlider::TicksBelow);
//    mVSlier->setTickPosition(QSlider::TicksLeft);
//    mVSlier->setTracking(true);
//    mVSlier->setInvertedAppearance(true);

//    setXYHidden(true);


//    glayout->setSpacing(0);
//    glayout->setContentsMargins(0,0,0,0);
//    glayout->setSizeConstraint(QGridLayout::SetFixedSize);
//    glayout->addLayout(stack,0,1);
//    glayout->addWidget(mVSlier,0,0);
//    glayout->addWidget(mHSlier,1,1);

//    mWindow->centralWidget()->setLayout(glayout);

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
            QSize ps = mWindow->size();
            int w = ( ps.width() - gs.width() )/ 2;
            int h = (ps.height()-gs.height()) / 2;
            gs.move(gs.mapFromParent(QPoint(w,h)));
            gs.exec();
            if (gs.isSetFine()) break;
        }
    });
    mWindow->addWidgetToToolBar(globalbtn);


    connect(sshoot,&QPushButton::clicked,[=]{

        QString defstr  = QDir::currentPath() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-") + screenshootpng;
        QString fileName = QFileDialog::getSaveFileName(mWindow, tr("保存PNG截图"),
                                                        defstr,
                                                        tr("Images (*.png)"));
        if(fileName.isEmpty())
            return;


        QPixmap pixmap(mWindow->size());
        mWindow->render(&pixmap,QPoint(),QRegion(mWindow->rect()));
        pixmap.save(fileName);
    });

    //将定时器超时信号与槽(功能函数)联系起来

    connect( autoSaveTimer,&QTimer::timeout,[=](){
        if(BaseForm::mPrjIsChanged)
            saveProject("autosave.json");
    } );

    connect(openPrj,SIGNAL(clicked(bool)),SLOT(onOpenProject()));

    connect(saveas,SIGNAL(clicked(bool)),SLOT(onSaveAsProject()));


    mWindow->addWidgetToToolBar(Q_NULLPTR);
    mWindow->addWidgetToToolBar(aboutbtn);
    connect(aboutbtn,&QPushButton::clicked,[=](){
        QDialog *aboutdlg  = new QDialog(mWindow);
        aboutdlg->setWindowTitle(WIN_TITLE);
        aboutdlg->setFixedSize(400,420);
        aboutdlg->setModal(true);
        QLabel *label = new QLabel(aboutbtn);
        QVBoxLayout *vbox = new QVBoxLayout();
        //vbox->setContentsMargins(0,0,0,0);
        // vbox->setMargin(0);
        vbox->setSpacing(0);
        aboutdlg->setLayout(vbox);
        vbox->addWidget(label);
        QString txt = QString("<b><img src=':/icon/icons/smallpt.png'></b>"\
                              "<b>"\
                              "<p>名称: %1 </p>"\
                              "<p>版本: %2 </p>"\
                              "<p>编译时间: %3</p>"\
                              "<p>开发者: 刘春阳</p>"\
                              "<p>邮箱: yjdwbj@gmail.com</p>"\
                              "</b>").arg(WIN_TITLE,GITVER,BUILDTIME);
        label->setText(txt);
        aboutdlg->exec();
        aboutdlg->deleteLater();
    });

    //开始运行定时器，定时时间间隔为6000ms
    connect(recordbtn,SIGNAL(clicked(bool)),this,SLOT(onRecordClick(bool)));
}


static void my_logoutput(void* ptr, int level, const char* fmt,va_list vl){
    FILE *fp = fopen("my_log.txt","a+");
    if(!fp)
        return;
    vfprintf(fp,fmt,vl);
    fflush(fp);
    fclose(fp);

}


//void CanvasManager::setXYHidden(bool b)
//{
//    mVSlier->setHidden(b);
//    mHSlier->setHidden(b);
//}

//void CanvasManager::setXYPosition(const QPoint &p)
//{
//    mVSlier->setValue(p.y());
//    mVSlier->setToolTip(QString::number(p.y()));
//    mHSlier->setValue(p.x());

//}

//void CanvasManager::setXYRange(const QSize &size)
//{
//    mVSlier->setRange(0,size.height());
//    mVSlier->setTickInterval(size.height()/500);
//    mHSlier->setRange(0,size.width());
//    mHSlier->setTickInterval(size.width()/500);

//}

void CanvasManager::onRecordClick(bool b)
{
    QPushButton *recordbtn =  (QPushButton *)(QObject::sender());

    //    static FFMPEG_Thread thread(mWindow);
    if(mFFmpegRuning)
    {
        mFFmpegRuning = false;

        sigterm_handler(SIGTERM);
        QThread::msleep(500);
        if(!QFileInfo(outputvideo).exists())
        {
            QMessageBox::warning(mWindow,"提示",
                                 "没有找视频文件");
            recordbtn->setText("录屏");
            recordbtn->setIcon(QIcon(":/icon/icons/player_play.png"));
            recordbtn->repaint();
            return;
        }



        recordbtn->setEnabled(false);

        recordbtn->setText("保存中....");
        recordbtn->repaint();
        QThread *pngthread = new QThread();
        //        moveToThread(pngthread);

        QString filters = "fps=10,scale=flags=lanczos";
        //        filters.sprintf("fps=10,scale=flags=lanczos",mWindow->size().width());

        connect(pngthread,&QThread::started,[=]{
            QList<QByteArray> pngarray ;
            pngarray << "ffmpeg" << "-y"  << "-i"
                     << outputvideo << "-vf" << QString(filters+",palettegen=stats_mode=diff").toLocal8Bit()
                     << palettepng;

            int argc = pngarray.size();
            char **pngargv = new char*[argc+1]{NULL};
            for(int i = 0 ; i < argc;i++)
            {
                pngargv[i] = pngarray[i].data();
            }
            ffmpeg(argc,pngargv);
            delete[] pngargv;
            pngthread->exit();
        });




        QThread *gifthread = new QThread();

        connect(gifthread,&QThread::started,[=]{
            //            qDebug() << "start to gif";

            if(!QFileInfo(outputvideo).exists() ||
                    !QFileInfo(palettepng).exists())
            {
                QMessageBox::warning(mWindow,"提示",
                                     "没有找视频文件,或者PNG文件.");
                return;
            }

            QList<QByteArray> gifarray ;
            gifarray << "ffmpeg" << "-y"
                     << "-v" << "quiet"
                     << "-i" << outputvideo
                     << "-i" << palettepng
                     << "-r" << "10"
                        //                     << "-lavfi" << "fps=10,paletteuse=dither=bayer:bayer_scale=2"
                        //                     << "-lavfi" << "fps=10 [x]; [x][1:v] paletteuse:dither=sierra2"
                     << "-lavfi" << QString(filters+"[x];[x][1:v]paletteuse").toLocal8Bit()
                     << gifFileName.toUtf8();

            int  argc = gifarray.size();
            char **gifargv = new char*[argc+1]{NULL};
            for(int i = 0 ; i < argc;i++)
            {
                gifargv[i] = gifarray[i].data();
            }
            ffmpeg(argc,gifargv);
            QString rootdir =  QDir::currentPath();
            QFile::remove( rootdir + "/" + outputvideo);
            QFile::remove(rootdir + "/" + palettepng);
            delete[] gifargv;
            gifthread->exit();
        });

        connect(gifthread,&QThread::finished,[=]{
            recordbtn->setEnabled(true);
            recordbtn->setText("录屏");
            recordbtn->setIcon(QIcon(":/icon/icons/player_play.png"));
            //            gifthread->disconnect();
        });
        connect(gifthread,SIGNAL(finished()),recordbtn,SLOT(repaint()));
        QObject::connect(gifthread,SIGNAL(finished()),gifthread,SLOT(deleteLater()));
        QObject::connect(pngthread,&QThread::finished,gifthread,[=]{
            gifthread->start();
            //            pngthread->disconnect();
        });

        connect(pngthread,SIGNAL(finished()),pngthread,SLOT(deleteLater()));
        pngthread->start();

    }else{

        QString defstr  = QDir::currentPath() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-") + outputgif;
        gifFileName = QFileDialog::getSaveFileName(mWindow, tr("保存GIF"),
                                                   defstr,
                                                   tr("Images (*.gif)"));
        if(gifFileName.isEmpty())
            return;

        recordbtn->setText("停止");
        recordbtn->setIcon(QIcon(":/icon/icons/player_stop.png"));
        recordbtn->repaint();
        QString wsize;
        wsize.sprintf("%dx%d",mWindow->width(),mWindow->height());
        QString pos;
        pos.sprintf(":0.0+%d,%d",mWindow->geometry().x(),mWindow->geometry().y());


        QThread *ft = new QThread();
        connect(ft,&QThread::started,[=](){

            QList<QByteArray> tmp;
            tmp << "ffmpeg"
                << "-v" << "quiet"  << "-y"
                   // Video Size 要在-f 的前面.
                << "-video_size" << wsize.toLocal8Bit()
                << "-framerate" << "15"
       #if _WIN32
                << "-f" << "gdigrab"
                << "-i" << "desktop"
                << "-offset_x" << QString("%1").arg(QString::number(mWindow->pos().x())).toLocal8Bit()
                << "-offset_y"  << QString("%1").arg(QString::number(mWindow->pos().x())).toLocal8Bit()
       #else
                << "-f" << "x11grab"
                << "-i" << pos.toLocal8Bit()

       #endif
                << "-vcodec" << "huffyuv"
                << "-pix_fmt" << "yuv422p"
                << outputvideo;
            //            int count = tmp.size()+1;
            char **argv = new char*[tmp.size()+1]{NULL};
            for(int i =0 ;i < tmp.size() ;i++)
            {
                argv[i] = tmp[i].data();
            }
            ffmpeg(tmp.size(),argv);
            ft->exit();
            QThread::msleep(500);
            delete[] argv;

        });

        connect(ft,SIGNAL(finished()),ft,SLOT(quit()));
        connect(ft,SIGNAL(finished()),ft,SLOT(deleteLater()));
        ft->start();
        mFFmpegRuning = true;


    }

}

void CanvasManager::screenshot()
{
    // 对原来的那个页面进行截屏
    if(!mCanvasList.size())
        return;

    QWidget *wd = stack->currentWidget();
    if(!wd)
        return;

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

ScenesScreen * CanvasManager::createNewCanvas()
{
    screenshot();
    mActiveSS = new ScenesScreen(mPageSize,(QWidget*)mWindow);
    mActiveSS->setProperty(DKEY_SHOT,false);  // 检查该页面是否创建过截图.

    // 这里不能改变它的对像名,用一个动态属
    //    // Scenes->setObjectName(QString("Page_%1").arg(QString::number(ssList.size()-1)));
    mActiveSS->setProperty(DKEY_TXT,QString("页面_%1").arg(QString::number(mCanvasList.size())));
    mActiveSS->setToolTip(mActiveSS->property(DKEY_TXT).toString());
    mCanvasList.append(mActiveSS);
    mWindow->lDock->setEnabled(true);
    stack->addWidget(mActiveSS);
    stack->setCurrentWidget(mActiveSS);
    // stack->setGeometry(stackRect);
    // 清理treeWidget 的行
    mWindow->tree->deleteAllitem();
    screenshot();
    BaseForm::mLayout->setEnabled(false);
    BaseForm::setObjectTempEnabled(false);
    return mActiveSS;
}

int CanvasManager::activeIndex()
{
    return stack->currentIndex();
}

void CanvasManager::setActiveSS(int index)
{
    if(index == -1) return;
    if(index >= mCanvasList.size())
        return;


    stack->setCurrentIndex(index);
    // 清理treeWidget 的行
    mWindow->tree->deleteAllitem();
    mActiveSS = (ScenesScreen*)(stack->currentWidget());

    // 把当前页的布局重新添加到treeWidget上

    foreach (QWidget *w, mActiveSS->childlist) {
        mWindow->tree->addItemToRoot(w);
        if(!w->isHidden())
        {
            ((BaseForm*)w)->addChildrenToTree();
        }
    }
    screenshot();
}

void CanvasManager::deleteCurrentPage()
{
//    ScenesScreen *ss = this->activeSS();
    if(!mActiveSS) return;

    int index = stack->currentIndex();
    if(index < 0)
    {
        mActiveSS = 0;
        return;
    }
    stack->removeWidget(mActiveSS);
    mWindow->pageView->delPage(index);
    mWindow->tree->deleteAllitem();
    mCanvasList.removeAt(index);

    mActiveSS->delAllObjects();
    delPage->setEnabled(stack->count() == 0 ? false : true);
    mWindow->lDock->setEnabled(stack->count() == 0 ? false : true);
    setActiveSS(stack->currentIndex());

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
    if(ret == QMessageBox::Yes)
    {
        deleteCurrentPage();
    }

}

void CanvasManager::onCreateNewProject()
{

    if(BaseForm::mPrjIsChanged)
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

    QSize ps = mWindow->size();
    int w = ( ps.width() - pd->width() )/ 2;
    int h = (ps.height() - pd->height()) / 2;
    pd->move(pd->mapFromParent(QPoint(w,h)));
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
    if (!PrjFile.open(QFile::ReadOnly|QIODevice::Text))
        return;

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
            BaseForm::mPrjIsChanged = false;
            mIsOpenProject = true;
        }
    }else{
        // qDebug() << " read Json file error";
        qDebug() << json_error.errorString();
    }

    mWindow->statusBar()->showMessage(QString("控件数量: %1").arg(QString::number(BaseForm::mSeqEnameMap.size())));

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
    }
}




void CanvasManager::closeCurrentProject()
{
    while(mActiveSS)
    {
        deleteCurrentPage();
    }

    BaseForm::mObjectMap.clear();
    mIsOpenProject = false;
    autoSaveTimer->stop();
}

void CanvasManager::onCreateNewScenesScreen()
{
    mIsOpenProject = true;
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
QJsonArray saveProjectJson(QProgressDialog *pd,QWidgetList mCanvasList )
{
    QJsonArray CanvasArray;
    int n = 0;
    foreach (QWidget *w, mCanvasList) {
        pd->setValue(n++);
        CanvasArray.append(((ScenesScreen*)w)->writeToJson());
    }
    return CanvasArray;
}

void CanvasManager::saveProject(QString fname)
{


    ProgressDlg *pd = new ProgressDlg(0,mCanvasList.length(),mWindow);
    QThread *dlgth = new QThread();
    pd->moveToThread(dlgth);
    connect(dlgth,&QThread::started,[=]{
        pd->exec();
        dlgth->exit();
    });

    QThread *saveth = new QThread();
    connect(saveth,SIGNAL(started()),dlgth,SIGNAL(started()));

    QObject::connect(saveth,&QThread::started,[=]{
        QJsonArray CanvasArray;
        int n = 0;
        foreach (QWidget *w, mCanvasList) {
            pd->mProgressBar->setValue(n++);
            CanvasArray.append(((ScenesScreen*)w)->writeToJson());
        }
         QFile saveFile(fname);
         if (!saveFile.open(QIODevice::WriteOnly)) {
             qWarning("Couldn't open save file.");

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
         pd->mProgressBar->setValue(n);
         emit pd->accepted();
         saveth->exit();
    });

    connect(saveth,SIGNAL(finished()),saveth,SLOT(deleteLater()));
    connect(dlgth,SIGNAL(finished()),dlgth,SLOT(deleteLater()));
    saveth->start();

    mIsOpenProject = false;


    //    pd->exec();
}

void CanvasManager::onSaveAsProject()
{
    QVariant prjvar = mWindow->mGlobalSet->value(INI_PRJDIR);
    QString fname = QFileDialog::getSaveFileName(mWindow,
                                                 tr("保存工程文件"),
                                                 prjvar.toString(),
                                                 tr("json files, json file (*.json)"));

    if(fname.isEmpty())
        return;

    if(!fname.endsWith(".json"))
    {
        fname = fname+".json";
    }
    mWindow->mGlobalSet->setValue(INI_PRJLAST,fname.toUtf8());
    saveProject(fname);

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
    BaseForm::mPrjIsChanged = false;


}



void CanvasManager::readProjectJson(const QJsonArray &array)
{
    bool readflag = false;
    BaseForm::mReadJson = false;
    foreach (QJsonValue val, array) {
        readflag = true;
        // QVariantMap  qjm = val.toObject().toVariantMap();
        // 创建一个页面.
        switch (val.type()) {
        case QJsonValue::Object:
        {
            int w =0,h =0;
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
            mActiveSS = createNewCanvas();

            // 递归读取它的页面.
            mActiveSS->readLayer(valobj[LAYER].toArray());
            mActiveSS->mXYLine->raise();

        }
            break;
        default:
            break;
        }
    }
    BaseForm::mReadJson = false;
    newPage->setEnabled(readflag);
    savePrj->setEnabled(readflag);
    saveas->setEnabled(readflag);
    delPage->setEnabled(readflag);
    confPrj->setEnabled(readflag);
}

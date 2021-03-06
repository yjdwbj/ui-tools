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
#include <QTimer>`
#include <QFileDialog>
#include <QTextBrowser>
#include <functional>
#include <QThreadPool>
#include <QTimer>
#include <QMetaObject>
#include <QKeyEvent>
#include <signal.h>
#include <QtConcurrent>


#include "zoomproject.h"



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


HVSlider*  CanvasManager::mHSlier = NULL;
HVSlider*  CanvasManager::mVSlier = NULL;


HVSlider::HVSlider(Qt::Orientation hv,int wh,QWidget *parent)
    :QFrame(parent),
      mOrient(hv),
      mLen(wh),
      mFixHW(15),
      mStep(5),
      mGradPos(10),
      mMaxStep(50),
      mRectLen(0),
      mRectStart(0)
{

    setFrameShadow(QFrame::Plain);
    //    setStyleSheet("HVSlider {background-color: #ffffff;}");
    if(mOrient == Qt::Horizontal)
    {
        setFixedSize(mLen,mFixHW);
    }else{
        setFixedSize(mFixHW,mLen);
    }

    setStyleSheet("background-color: white;");

}

void HVSlider::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::gray);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    QPen blackpen;
    blackpen.setColor(Qt::black);
    blackpen.setStyle(Qt::SolidLine);

    if(mOrient == Qt::Horizontal)
    {
        QLine line;
        for(int i = 0 ; i < mLen;i+=mStep)
        {
            if(i % mMaxStep)
            {
                line.setP1(QPoint(i,mGradPos));
                line.setP2(QPoint(i,mFixHW));
            }else{
                painter.setPen(blackpen);
                line.setP1(QPoint(i,0));
                line.setP2(QPoint(i,mFixHW ));
                painter.drawText(QPoint(i,mGradPos),QString::number(i));
                painter.setPen(pen);
            }
            painter.drawLine(line);

        }
    }else{
        QLine line;
        for(int i = 0 ; i < mLen;i+=mStep)
        {
            if(i % mMaxStep)
            {
                line.setP1(QPoint(mGradPos,i));
                line.setP2(QPoint(mFixHW,i));
            }else{

                painter.setPen(blackpen);
                line.setP1(QPoint(0,i));
                line.setP2(QPoint(mFixHW ,i));
                painter.rotate(90);
                painter.drawText(QPoint(i,0),QString::number(i));
                painter.setPen(pen);
                painter.rotate(-90);
            }
            painter.drawLine(line);
        }
    }

    if(mRectLen)
    {
        //画当前对像的高亮区域.
        QPen rectpen;
      //  rectpen.setColor("#e9e9e9");
        rectpen.setColor(QColor("#e9e9e9"));
        rectpen.setStyle(Qt::SolidLine);
        painter.setPen(rectpen);
        QColor c = QColor("#ffd8bf").rgba();
        c.setAlpha(150);
        if(mOrient == Qt::Horizontal)
        {
            QRect rect(mRectStart,0,mRectLen,mFixHW);
            painter.drawRect(rect);
            painter.fillRect(rect,c);
        }else{
            QRect rect(0,mRectStart,mFixHW,mRectLen);
            painter.drawRect(rect);
            painter.fillRect(rect,c);
        }
    }
}


void HVSlider::setNewLen(int len)
{
    mLen = len;
    if(mOrient == Qt::Horizontal)
    {
        setFixedSize(mLen,mFixHW);
    }else{
        setFixedSize(mFixHW,mLen);
    }
    repaint();
}


void HVSlider::setRectPos(const QPoint &p)
{
    mRectStart = mOrient == Qt::Horizontal ? p.x() : p.y();
    repaint();
}

void HVSlider::setRectSize(const QSize &s)
{
    mRectLen =  mOrient == Qt::Horizontal ? s.width() : s.height();
    repaint();
}


ScenesScreen* CanvasManager::mActiveSS = NULL;

CanvasManager::CanvasManager(MainWindow *w):
    mWindow(w),mStack(new QStackedLayout()),
    newPrj(new QPushButton(QIcon(":/icon/icons/category_vcs.png"),tr("新建工程"))),
    newPage(new QPushButton(QIcon(":/icon/icons/canvas-diagram.png"),tr("新建页面"))),
    delPage(new QPushButton(QIcon(":/icon/icons/removesubmitfield.png"),tr("删除当前页"))),
    savePrj(new QPushButton(QIcon(":/icon/icons/document-save-as.png"),tr("保存工程"))),
    confPrj(new QPushButton(tr("工程设置"))),
    zoomPrj(new QPushButton(QIcon(":/icon/icons/interface.png"),"工程缩放")),
    saveas(new QPushButton(QIcon(":/icon/icons/document-save-as.png"),"另存为")),
    mRedo(new QPushButton(QIcon(":/icon/icons/Redo.png"),"重做")),
    mUndo(new QPushButton(QIcon(":/icon/icons/Undo.png"),"撤消")),
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

    if(!mVSlier)
        mVSlier = new HVSlider(Qt::Vertical,BaseForm::mPageSize.width());
    if(!mHSlier)
        mHSlier = new HVSlider(Qt::Horizontal,BaseForm::mPageSize.height());


    QComboBox *cb = new QComboBox();

    QPushButton *openPrj = new QPushButton("打开工程");
    QPushButton *globalbtn = new QPushButton(QIcon(":/icon/icons/preferences-system.png"),"全局设置");
    globalbtn->setToolTip("软件的全局设置,需要重启软件后生效.");

    QPushButton *recordbtn = new QPushButton(QIcon(":/icon/icons/player_play.png"),"录屏");
    recordbtn->setToolTip("录取当前程序大小的窗口区域,最终保存成GIF图片");
    QPushButton *sshoot = new QPushButton(QIcon(":/icon/icons/Screenshot.png"),"截屏");
    sshoot->setToolTip("截取程序的界面,并保存成PNG图片");
    QPushButton *aboutbtn = new QPushButton(QIcon(":/icon/icons/mode_help@2x.png"),"关于");
    cb->addItems(QStyleFactory::keys());

    zoomPrj->setToolTip("对当前工程的页面尺寸进行缩放,宽高最好要按比例缩放,不然会出现截断与坐标清零.");

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

    QGridLayout *gridLayout = new QGridLayout(mWindow);

    gridLayout->setSpacing(0);
    gridLayout->setMargin(1);
    gridLayout->addWidget(mHSlier,0,1);
    gridLayout->addWidget(mVSlier,1,0,1,1,Qt::AlignTop);
    gridLayout->addLayout(mStack,1,1);
    //    gridLayout->setOriginCorner(Qt::TopLeftCorner);

    mWindow->centralWidget()->setLayout(gridLayout);
    // 按屏幕的大小比例调整.

    connect(newPrj,SIGNAL(clicked(bool)),SLOT(onCreateNewProject()));
    connect(newPage,SIGNAL(clicked(bool)),SLOT(onCreateNewScenesScreen()));
    connect(delPage,SIGNAL(clicked(bool)),SLOT(onDelCurrentScenesScreen()));
    connect(savePrj,SIGNAL(clicked(bool)),SLOT(onSaveProject()));
    connect(confPrj,SIGNAL(clicked(bool)),SLOT(onConfProject()));


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

    mWindow->addWidgetToToolBar(zoomPrj);

    connect(zoomPrj,&QPushButton::clicked,[=]{

        ZoomProject *zp = new ZoomProject(BaseForm::mPageSize,mWindow);
        connect(zp,SIGNAL(accepted()),SLOT(onUpdateNewProjectSize()));
        connect(zp,SIGNAL(accepted()),zp,SLOT(deleteLater()));
        zp->exec();
    });

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


void CanvasManager::setSliderPos(const QPoint &p)
{
    mHSlier->setRectPos(p);
    mVSlier->setRectPos(p);
}

void CanvasManager::setSliderSize(const QSize &size)
{
    mHSlier->setRectSize(size);
    mVSlier->setRectSize(size);
}

static void my_logoutput(void* ptr, int level, const char* fmt,va_list vl){
    FILE *fp = fopen("my_log.txt","a+");
    if(!fp)
        return;
    vfprintf(fp,fmt,vl);
    fflush(fp);
    fclose(fp);

}

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



void CanvasManager::onUpdateNewProjectSize()
{
    ZoomProject *p = (ZoomProject*)(QObject::sender());
    QSize nsize = p->getNewSize();
    if(nsize.width() == BaseForm::mPageSize.width() &&
            nsize.height()  == BaseForm::mPageSize.height())
        return;

    BaseForm::mWidthRate = float(nsize.width()) / float(BaseForm::mPageSize.width());
    BaseForm::mHeightRate = float(nsize.height()) / float(BaseForm::mPageSize.height());

    foreach (QWidget *w, mCanvasList) {
        ((ScenesScreen*)w)->updateNewPageSize();
    }

    BaseForm::setPageSize(BaseForm::mWidthRate * BaseForm::mPageSize.width(),
                          BaseForm::mHeightRate * BaseForm::mPageSize.height());

    mHSlier->setNewLen(BaseForm::mPageSize.width());
    mVSlier->setNewLen(BaseForm::mPageSize.height());
    BaseForm::mPrjIsChanged = true;
}

void CanvasManager::screenshot()
{
    // 对原来的那个页面进行截屏
    if(!mCanvasList.size())
        return;

    QWidget *wd = mStack->currentWidget();
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

    QPixmap pixmap(mStack->currentWidget()->size());
    mStack->currentWidget()->render(&pixmap,QPoint(),QRegion(mStack->currentWidget()->rect()));
    QVariant vp = mStack->currentWidget()->property(DKEY_SHOT);
    if( vp.isValid() && vp.toBool() == false)
    {
        mWindow->pageView->addNewPage(pixmap,
                                      mStack->currentWidget()->property(DKEY_TXT).toString());

        //pixmap.save("test.png");　//　这里如果有需要可以保存成文件.
        mStack->currentWidget()->setProperty(DKEY_SHOT,true);
    }else{
        int index = mStack->currentIndex();
        mWindow->pageView->delPage(index); // 删除当前的,更新每新的.
        mWindow->pageView->InsertPage(index,pixmap,mStack->currentWidget()->property(DKEY_TXT).toString());
    }
}

ScenesScreen * CanvasManager::createNewCanvas()
{

    mActiveSS = new ScenesScreen(BaseForm::mPageSize,(QWidget*)mWindow);
    mHSlier->setNewLen(BaseForm::mPageSize.width());
    mVSlier->setNewLen(BaseForm::mPageSize.height());
    mActiveSS->setProperty(DKEY_SHOT,false);  // 检查该页面是否创建过截图.

    // 这里不能改变它的对像名,用一个动态属
    //    // Scenes->setObjectName(QString("Page_%1").arg(QString::number(ssList.size()-1)));
    mActiveSS->setProperty(DKEY_TXT,QString("页面_%1").arg(QString::number(mCanvasList.size())));
    mActiveSS->setToolTip(mActiveSS->property(DKEY_TXT).toString());
    mCanvasList.append(mActiveSS);
    mWindow->lDock->setEnabled(true);
    mStack->addWidget(mActiveSS);
    mStack->setCurrentWidget(mActiveSS);
    // stack->setGeometry(stackRect);
    // 清理treeWidget 的行
    mWindow->tree->deleteAllitem();

    BaseForm::mLayout->setEnabled(false);
    BaseForm::setObjectTempEnabled(false);
    return mActiveSS;
}

int CanvasManager::activeIndex()
{
    return mStack->currentIndex();
}

void CanvasManager::setActiveSS(int index)
{
    if(index == -1) return;
    if(index >= mCanvasList.size())
        return;


    mStack->setCurrentIndex(index);
    // 清理treeWidget 的行
    mWindow->tree->deleteAllitem();
    mActiveSS = (ScenesScreen*)(mStack->currentWidget());

    // 把当前页的布局重新添加到treeWidget上

    BaseForm::mReadJson = true;

    foreach (QWidget *w, mActiveSS->childlist) {
        mWindow->tree->addItemToRoot(w);
        if(!w->isHidden())
        {
            ((BaseForm*)w)->addChildrenToTree();
        }
    }

    BaseForm::mReadJson = false;
    screenshot();
    ((BaseForm*)mActiveSS->mActiveObj)->onSelectMe();
}

void CanvasManager::deleteCurrentPage()
{
    //    ScenesScreen *ss = this->activeSS();
    if(!mActiveSS) return;

    int index = mStack->currentIndex();
    if(index < 0)
    {
        mActiveSS = 0;
        return;
    }
    mStack->removeWidget(mActiveSS);
    mWindow->pageView->delPage(index);
    mWindow->tree->deleteAllitem();
    mCanvasList.removeAt(index);

    mActiveSS->delAllObjects();
    delPage->setEnabled(mStack->count() == 0 ? false : true);
    mWindow->lDock->setEnabled(mStack->count() == 0 ? false : true);
    setActiveSS(mStack->currentIndex());

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

void CanvasManager::closeProjectDlg()
{
    if(BaseForm::mPrjIsChanged)
    {

        QMessageBox ExtmsgBox;
        ExtmsgBox.setWindowTitle("新建工程提示");
        ExtmsgBox.setText("是否关闭当前工程,新建工程?");
        ExtmsgBox.setIconPixmap(QPixmap(":/icon/icons/ask.png"));
        ExtmsgBox.setWindowIcon(QIcon(":/icon/icons/info.png"));

        ExtmsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        ExtmsgBox.setButtonText(QMessageBox::Yes,"关闭");
        ExtmsgBox.setButtonText(QMessageBox::Cancel,"取消");
        ExtmsgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = ExtmsgBox.exec();
        if(ret == QMessageBox::Cancel)
        {
            return;
        }

        // 当前工程有修改还没有保存.
        QMessageBox msgBox;
        msgBox.setWindowTitle("新建工程提示");
        msgBox.setIconPixmap(QPixmap(":/icon/icons/ask.png"));
        msgBox.setWindowIcon(QIcon(":/icon/icons/info.png"));
        msgBox.setText("当前编辑的工程有新的修改没有保存,选请择<保存>进行保存.");
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Yes,"保存");
        msgBox.setButtonText(QMessageBox::Cancel,"取消");
        msgBox.setDefaultButton(QMessageBox::Cancel);
        ret = msgBox.exec();
        //qDebug() << " QMessageBox result " << ret;
        if(ret == QMessageBox::Yes)
        {
            //　需要保存
            onSaveProject();
        }
    }
    closeCurrentProject(); // 关闭当前工程.

}

void CanvasManager::onCreateNewProject()
{

    closeProjectDlg();
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
    closeProjectDlg();
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
    screenshot();
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
    QThread *saveth = new QThread();
    QObject::connect(saveth,&QThread::started,[=]{
        QJsonArray CanvasArray;
        foreach (QWidget *w, mCanvasList) {
            CanvasArray.append(((ScenesScreen*)w)->writeToJson());
        }
        QFile saveFile(fname);
        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");

        }
        QJsonObject obj ;
        obj[NAME] = mProjectName;
        obj[ACTPAGE] = mStack->currentIndex();
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
        savePrj->setEnabled(true);
        saveth->quit();
    });

    connect(saveth,SIGNAL(finished()),saveth,SLOT(deleteLater()));
    connect(saveth,SIGNAL(finished()),savePrj,SLOT(repaint()));
    saveth->start();
    savePrj->setEnabled(false);
    savePrj->repaint();

    mIsOpenProject = false;
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
    BaseForm::mReadJson = true;
    foreach (QJsonValue val, array) {
        readflag = true;
        // QVariantMap  qjm = val.toObject().toVariantMap();
        // 创建一个页面.
        switch (val.type()) {
        case QJsonValue::Object:
        {
            //            int w =0,h =0;
            QSize sm;
            QJsonObject valobj = val.toObject();
            foreach (QJsonValue pval, valobj[PROPERTY].toArray()) {
                QJsonObject pobj = pval.toObject();
                if(pobj.contains(KEY_RECT))
                {
                    sm.setWidth(pobj[KEY_RECT].toObject()[WIDTH].toInt());
                    sm.setHeight(pobj[KEY_RECT].toObject()[HEIGHT].toInt());
                    break;
                }
            }
            //            setDefaultPageSize(QSize(w,h));
            BaseForm::setPageSize(sm);
            mActiveSS = createNewCanvas();
            // 递归读取它的页面.
            mActiveSS->readLayer(valobj[LAYER].toArray());
            mActiveSS->mXYLine->raise();
            screenshot();

        }
            break;
        default:
            break;
        }
    }
    BaseForm::mReadJson = false;
    ((BaseForm*)mActiveSS->mActiveObj)->onSelectMe();
    newPage->setEnabled(readflag);
    savePrj->setEnabled(readflag);
    saveas->setEnabled(readflag);
    delPage->setEnabled(readflag);
    confPrj->setEnabled(readflag);
    mWindow->statusBar()->showMessage(QString("控件数量: %1").arg(QString::number(BaseForm::mSeqEnameMap.size())));
}

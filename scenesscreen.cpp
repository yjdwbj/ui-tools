#include "scenesscreen.h"

ScenesScreen::ScenesScreen(QWidget *parent) : QFrame(parent),mWindow((MainWindow*)parent)
{

   setObjectName("ActiveScreen");
   setStyleSheet("QWidget#ActiveScreen{border: 1.5px solid gray;"\
                 "background-color: #EEEEEE;"\
                 "background-image: url(:/test/pic/bmp/backgroud-image.bmp);}");

   setFixedSize(580,420);
   show();



}

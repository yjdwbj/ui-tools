#include "dragwidget.h"

DragWidget::DragWidget(QWidget *parent)
    :QFrame(parent)
{
    this->setMinimumSize(320,480);

    setFrameStyle(QFrame::Sunken| QFrame::StyledPanel);

    setAcceptDrops(true);


}

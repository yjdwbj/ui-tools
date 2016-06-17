#include "imagelayer.h"
#include "config.h"

ImageLayer::ImageLayer(QWidget *parent)
    :QFrame(parent)
{

   setFrameShadow(QFrame::Raised);
   setFrameShape(QFrame::StyledPanel);
}


void ImageLayer::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->pos();
}

void ImageLayer::mouseMoveEvent(QMouseEvent *event)
{
    move( pos() + (event->pos() - mOffset));
}


void ImageLayer::mouseReleaseEvent(QMouseEvent *event)
{
    /* 放开鼠标时检查它的是否出了边界要 */
    /*
    NewFrame *p =(NewFrame*) (this->parentWidget());
    QPoint pos = p->pos();
    if(p->x() < 0)
    {
        pos.setX(0);
        p->move(pos);

    }
    if(p->y() < 0 )
    {
        pos.setY(0);
        p->move(pos);

    }

    QSize ms = mWindow->mCanvas->size();
    if((p->x() + p->size().width()) > ms.width())
    {
        pos.setX( ms.width() - p->size().width() );
        p->move(pos);
    }

    if((p->y() + p->size().height()) > ms.height())
    {
        pos.setY(ms.height() - p->size().height());
        p->move(pos);
    }
    */
}

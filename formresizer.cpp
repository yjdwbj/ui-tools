#include "formresizer.h"
#include "sizehandlerect.h"

#include <QDebug>

#include <QResizeEvent>
#include <QPalette>
#include <QLayout>
#include <QFrame>
#include "config.h"

enum { debugFormResizer = 0 };

/* 在控件周围创建８个点用于鼠标拖动 */

FormResizer::FormResizer(QWidget *parent) :
    QWidget(parent),
    m_frame(new QFrame),
    mBorderColor("#FFFFFF")
 //   m_formWindow(0)
{
    // Make the resize grip of a mainwindow form find us as resizable window.
    setWindowFlags(windowFlags() | Qt::SubWindow);
    setBackgroundRole(QPalette::Base);

    QVBoxLayout *handleLayout = new QVBoxLayout(this);
    handleLayout->setMargin(SELECTION_MARGIN);
    // 如果设置下面两行,控制只能看到一半的内点.
    handleLayout->setSpacing(0);
    handleLayout->setContentsMargins(0,0,0,0);
    //handleLayout->setMargin(1);
    handleLayout->addWidget(m_frame);

    m_frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    QVBoxLayout *layout = new QVBoxLayout(m_frame);
    layout->setMargin(0);

    //layout->setContentsMargins(0,0,0,0);
    // handles
    m_handles.reserve(SizeHandleRect::Left);
    for (int i = SizeHandleRect::LeftTop; i <= SizeHandleRect::Left; ++i) {
        SizeHandleRect *shr = new SizeHandleRect(this, static_cast<SizeHandleRect::Direction>(i), this);
        connect(shr, &SizeHandleRect::mouseButtonReleased, this, &FormResizer::formWindowSizeChanged);
        m_handles.push_back(shr);
    }
    setState(SelectionHandleActive);
    updateGeometry();
}

void FormResizer::updateGeometry()
{
    const QRect &geom = m_frame->geometry();

    if (debugFormResizer)
        qDebug() << "FormResizer::updateGeometry() " << size() << " frame " << geom;

    const int w = SELECTION_HANDLE_SIZE;
    const int h = SELECTION_HANDLE_SIZE;

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        SizeHandleRect *hndl = *it;
        switch (hndl->dir()) {
        case SizeHandleRect::LeftTop:
            hndl->move(geom.x() - w / 2, geom.y() - h / 2);
            break;
        case SizeHandleRect::Top:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() - h / 2);
            break;
        case SizeHandleRect::RightTop:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() - h / 2);
            break;
        case SizeHandleRect::Right:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        case SizeHandleRect::RightBottom:
            hndl->move(geom.x() + geom.width() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case SizeHandleRect::Bottom:
            hndl->move(geom.x() + geom.width() / 2 - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case SizeHandleRect::LeftBottom:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() - h / 2);
            break;
        case SizeHandleRect::Left:
            hndl->move(geom.x() - w / 2, geom.y() + geom.height() / 2 - h / 2);
            break;
        default:
            break;
        }
    }
}

void FormResizer::update()
{
    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it) {
        (*it)->update();
    }
}

void FormResizer::setState(SelectionHandleState st)
{
    if (debugFormResizer)
        qDebug() << "FormResizer::setState " << st;

    const Handles::iterator hend =  m_handles.end();
    for (Handles::iterator it = m_handles.begin(); it != hend; ++it)
        (*it)->setState(st);
}

//void FormResizer::setFormWindow(QDesignerFormWindowInterface *fw)
//{
//    if (debugFormResizer)
//        qDebug() << "FormResizer::setFormWindow " << fw;
//    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_frame->layout());
//    Q_ASSERT(layout);
//    if (layout->count())
//        delete layout->takeAt(0);
//    m_formWindow = fw;

//    if (m_formWindow)
//        layout->addWidget(m_formWindow);
//    mainContainerChanged();
//     connect(fw, &QDesignerFormWindowInterface::mainContainerChanged, this, &FormResizer::mainContainerChanged);
//}

void FormResizer::resizeEvent(QResizeEvent *event)
{
    if (debugFormResizer)
        qDebug() << ">FormResizer::resizeEvent" <<  event->size();
    updateGeometry();
    QWidget::resizeEvent(event);
    if (debugFormResizer)
        qDebug() << "<FormResizer::resizeEvent";

}

QSize FormResizer::decorationSize() const
{

    const int margin = 2 * SELECTION_MARGIN + 2 * m_frame->lineWidth();
    qDebug() << " margin " << margin;
    return QSize(margin, margin);
}


void FormResizer::onBorderChangedValue(int v)
{
    QString name = QObject::sender()->objectName();
//    qDebug() << " my object name is " << this->objectName()
//             << " class name " << this->metaObject()->className()
//             << " parent class name " << this->parentWidget()->metaObject()->className();
    if(!name.compare(X))
    {

        mBorder.setX(v);
    }else if(!name.compare(Y))
    {

        mBorder.setY(v);
    }else if(!name.compare(W))
    {

        mBorder.setWidth(v);
    }else if(!name.compare(H))
    {

        mBorder.setHeight(v);
    }

    updateBorderColor();

//    QString str = QString("border-left: %1px solid %5;"\
//                          "border-top: %2px solid %5;" \
//                          "border-right: %3px solid %5;"\
//                          "border-bottom: %4px solid %5;").arg(QString::number(mBorder.x()),
//                                                               QString::number(mBorder.y()),
//                                                               QString::number(mBorder.width()),
//                                                               QString::number(mBorder.height()),
//                                                               mBorderColor);
//    m_frame->setStyleSheet(str);
//    qDebug() << " border style string " << str;
    m_frame->update();
    this->blockSignals(true);
}

void FormResizer::updateBorderColor()
{
    QString str = QString("border-style: solid;"\
                          "border-color: %5;"\
                          "subcontrol-origin: padding;"\
                          "subcontrol-position: right bottom;"\

                          "border-left-width: %1px;"\
                          "border-top-width: %2px;" \
                          "border-right-width: %3px;"\
                          "border-bottom-width: %4px;").arg(QString::number(mBorder.x()),
                                                               QString::number(mBorder.y()),
                                                               QString::number(mBorder.width()),
                                                               QString::number(mBorder.height()),
                                                               mBorderColor);
    this->setStyleSheet(QString("%1{%2}").arg(this->metaObject()->className(),str));
    qDebug()  << " this stylesheet " << this->styleSheet();
}

//QWidget *FormResizer::mainContainer()
//{
//    if (m_formWindow)
//        return m_formWindow->mainContainer();
//    return 0;
//}

//void FormResizer::mainContainerChanged()
//{
//    const QSize maxWidgetSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
//    if (const QWidget *mc = mainContainer()) {
//        // Set Maximum size which is not handled via a hint (as opposed to minimum size)
//        const QSize maxWidgetSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
//        const QSize formMaxSize = mc->maximumSize();
//        QSize newMaxSize = maxWidgetSize;
//        if (formMaxSize != maxWidgetSize)
//            newMaxSize = formMaxSize + decorationSize();
//        if (debugFormResizer)
//            qDebug() << "FormResizer::mainContainerChanged" <<  mc << " Size " << mc->size()<< newMaxSize;
//        setMaximumSize(newMaxSize);
//        resize(decorationSize() + mc->size());
//    } else {
//        setMaximumSize(maxWidgetSize);
//    }
//}

//void FormResizer::mousePressEvent(QMouseEvent *e)
//{
//    mOffset = e->pos();
//}

//void FormResizer::mouseMoveEvent(QMouseEvent *e)
//{
//     move(this->pos() + (e->pos() - mOffset));
//}

//void FormResizer::mouseReleaseEvent(QMouseEvent *e)
//{

//}

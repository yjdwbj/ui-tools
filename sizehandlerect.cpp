#include "sizehandlerect.h"
#include <QPainter>
#include <QMouseEvent>


#include "canvasmanager.h"
#include "scenesscreen.h"
#include "module.h"
class CanvasManager;
class ScenesScreen;
class BaseForm;

enum { debugSizeHandle = 0 };

SizeHandleRect::SizeHandleRect(QWidget *parent, Direction d, QWidget *resizable) :
    QWidget(parent),
    m_dir(d),
    m_resizable(resizable),
    m_state(SelectionHandleOff)
{
    setBackgroundRole(QPalette::Text);
    setAutoFillBackground(true);

    setFixedSize(SELECTION_HANDLE_SIZE, SELECTION_HANDLE_SIZE);
    setMouseTracking(false);
    updateCursor();
}

void SizeHandleRect::updateCursor()
{
    switch (m_dir) {
    case Right:
    case RightTop:
        setCursor(Qt::SizeHorCursor);
        return;
    case RightBottom:
        setCursor(Qt::SizeFDiagCursor);
        return;
    case LeftBottom:
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        return;
    default:
        break;
    }

    setCursor(Qt::ArrowCursor);
}

void SizeHandleRect::paintEvent(QPaintEvent *)
{
    switch (m_state) {
    case SelectionHandleOff:
        break;
    case SelectionHandleInactive: {
        QPainter p(this);
        p.setPen(Qt::red);
        p.drawRect(0, 0, width() - 1, height() - 1);
    }
        break;
    case SelectionHandleActive: {
        QPainter p(this);
        p.setPen(Qt::blue);
       // p.drawRect(0, 0, width() - 1, height() - 1);
        p.drawRect(0, 0, width() , height() );

    }
        break;
    }
}

void SizeHandleRect::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    if (e->button() != Qt::LeftButton)
        return;

    CanvasManager::mActiveSS->mXYLine->setHidden(false);

    m_startSize = m_curSize = m_resizable->size();
    m_startPos = m_curPos = m_resizable->mapFromGlobal(e->globalPos());
    if (debugSizeHandle)
        qDebug() << "SizeHandleRect::mousePressEvent" << m_startSize << m_startPos << m_curPos;

}

void SizeHandleRect::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
        return;

    // Try resize with delta against start position.
    // We don't take little deltas in consecutive move events as this
    // causes the handle and the mouse cursor to become out of sync
    // once a min/maxSize limit is hit. When the cursor reenters the valid
    // areas, it will now snap to it.
    // qDebug() << " try to move a control point" << e;
    m_curPos = m_resizable->mapFromGlobal(e->globalPos());
    //    qDebug() << " resizable name " <<
    //                m_resizable->objectName()
    //             << m_resizable->metaObject()->className()
    //             << " this pos " << m_resizable->pos();
    m_resizable->setProperty(DKEY_OLDPOS,m_resizable->pos());
    QSize delta = QSize(m_curPos.x() - m_startPos.x(), m_curPos.y() -  m_startPos.y());
    switch (m_dir) {
    case Right:
    case RightTop: // Only width
        delta.setHeight(0);
        break;
    case RightBottom: // All dimensions
        break;
    case LeftBottom:
    case Bottom: // Only height
        delta.setWidth(0);
        break;
    default:
        delta = QSize(0, 0);
        break;
    }
    if (delta != QSize(0, 0))
        tryResize(delta);
    //    qDebug() << " this pos " << m_resizable->pos();

    QPoint mp = mapTo(CanvasManager::mActiveSS,e->pos()) - e->pos();
    QPoint gp = QPoint(mp.x() + SELECTION_MARGIN,
                       mp.y() + SELECTION_MARGIN);
//    CanvasManager::mActiveSS->mXYLine->setPos(gp);
    CanvasManager::mActiveSS->mXYLine->setPos(gp,m_resizable->size());
    ((BaseForm*)m_resizable)->posWidget->updateSize(m_resizable);
}

//这里的事件不能开启使用.
//void SizeHandleRect::mouseReleaseEvent(QMouseEvent *e)
//{
//    if (e->button() != Qt::LeftButton)
//        return;
//    CanvasManager::mActiveSS->mXYLine->setHidden(true);

//    e->accept();
//        if (m_startSize != m_curSize) {
//            const QRect startRect = QRect(0, 0, m_startPos.x(), m_startPos.y());
//            const QRect newRect = QRect(0, 0, m_curPos.x(), m_curPos.y());
//            if (debugSizeHandle)
//                qDebug() << "SizeHandleRect::mouseReleaseEvent" << startRect << newRect;
//            emit mouseButtonReleased(startRect, newRect);
//        }
//}

void SizeHandleRect::tryResize(const QSize &delta)
{
    // Try resize with delta against start position

    QSize newSize = m_startSize + delta;
    newSize = newSize.expandedTo(m_resizable->minimumSizeHint());
    newSize = newSize.expandedTo(m_resizable->minimumSize());
    newSize = newSize.boundedTo(m_resizable->maximumSize());
    if (newSize == m_resizable->size())
        return;
    if(newSize.width() < 5)
        newSize.setWidth(5);
    if(newSize.height() < 5)
        newSize.setHeight(5);
    if (debugSizeHandle)
        qDebug() << "SizeHandleRect::tryResize by (" << m_startSize << '+' <<  delta << ')' << newSize;
    m_resizable->resize(newSize);
    m_curSize = m_resizable->size();
}

void SizeHandleRect::setState(SelectionHandleState st)
{
    if (st == m_state)
        return;
    switch (st) {
    case SelectionHandleOff:
        hide();
        break;
    case SelectionHandleInactive:
    case SelectionHandleActive:
        show();
        raise();
        break;
    }
    m_state = st;
}

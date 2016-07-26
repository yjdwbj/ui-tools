#ifndef FORMRESIZER_H
#define FORMRESIZER_H
#include <QWidget>
#include <QVector>
#include <QFrame>
#include "config.h"

class SizeHandleRect;

/* A window to embed a form window interface as follows:
 *
 *            Widget
 *              |
 *          +---+----+
 *          |        |
 *          |        |
 *       Handles   QVBoxLayout [margin: SELECTION_MARGIN]
 *                   |
 *                 Frame [margin: lineWidth]
 *                   |
 *                 QVBoxLayout
 *                   |
 *                 QDesignerFormWindowInterface
 *
 * Can be embedded into a QScrollArea. */


class Canvas: public QFrame
{
public:
    QPixmap backImage;
    QString mBorderColor;
    QRect mBorder;

protected:
    void paintEvent(QPaintEvent *);
};


class FormResizer : public QWidget
{
    Q_OBJECT
public:

    FormResizer(QWidget *parent = 0);

    void updateGeometry();
    void setState(SelectionHandleState st);
    void update();
    virtual void onSelectMe() =0;
   // void updateBorderColor();
   // virtual void onBindValue(QWidget *w,const QVariant &val) =0;

   // void setFormWindow(QDesignerFormWindowInterface *fw);
    QFrame *m_frame;
//    QRect mBorder;
//    QString mBorderColor;

signals:
    void formWindowSizeChanged(const QRect &oldGeo, const QRect &newGeo);

protected:
    void resizeEvent(QResizeEvent *event) override;

//    void mouseMoveEvent(QMouseEvent *);
//    void mousePressEvent(QMouseEvent *);
//    void mouseReleaseEvent(QMouseEvent *);

private slots:
   // void mainContainerChanged();
   // void onBorderChangedValue(int v);


private:
    QSize decorationSize() const;
    QWidget *mainContainer();
    QPoint mOffset;
    typedef QVector<SizeHandleRect*> Handles;
    Handles m_handles;


  //  QDesignerFormWindowInterface * m_formWindow;
};





#endif // FORMRESIZER_H

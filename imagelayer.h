#ifndef IMAGELAYER_H
#define IMAGELAYER_H
#include <QFrame>
#include <QWidget>
#include <QMouseEvent>

class MainWindow;
class ImageLayer : public QFrame
{
public:
    explicit ImageLayer(QWidget *parent=0);
    void addMainWindow(MainWindow *mw) { mWindow = mw;}

protected:
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private:
    MainWindow *mWindow;
    QPoint mOffset;
};

#endif // IMAGELAYER_H

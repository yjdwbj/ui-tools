#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include <QWidget>

class MainWindow;
class PageView : public QDockWidget
{
public:
    PageView();
    void addMainWindow(MainWindow *w) { mWindow = w;}


    MainWindow* mWindow;
};

#endif // PAGEVIEW_H

#ifndef ZOOMPROJECT_H
#define ZOOMPROJECT_H

#include <QDialog>

namespace Ui {
class ZoomProject;
}

class ZoomProject : public QDialog
{
    Q_OBJECT

public:
    explicit ZoomProject(const QSize size,QWidget *parent = 0);
    QSize getNewSize() const;

    ~ZoomProject();

private:
    Ui::ZoomProject *ui;
};

#endif // ZOOMPROJECT_H

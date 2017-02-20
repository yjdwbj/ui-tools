#include "zoomproject.h"
#include "ui_zoomproject.h"
#include <QPushButton>

ZoomProject::ZoomProject(const QSize size, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZoomProject)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确认");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    ui->lab_oldh->setText(QString::number(size.height()));
    ui->lab_oldw->setText(QString::number(size.width()));
    ui->spinBoxH->setValue(size.height());
    ui->spinBoxW->setValue(size.width());
    connect(this,SIGNAL(rejected()),this,SLOT(deleteLater()));


    this->setModal(true);

}

ZoomProject::~ZoomProject()
{
    delete ui;
}

QSize ZoomProject::getNewSize()const
{
    return QSize(ui->spinBoxW->value(),ui->spinBoxH->value());
}

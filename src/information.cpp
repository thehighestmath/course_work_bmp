#include "information.h"
#include "ui_information.h"

Information::Information(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Information)
{
    ui->setupUi(this);
}

Information::~Information()
{
    delete ui;
}

void Information::write_information(){
    ui->name->setText(info_s.name.toLocal8Bit().constData());
    ui->path->setText(info_s.path.toLocal8Bit().constData());
    ui->size->setText(QString::number(info_s.weight));
    ui->width->setText(QString::number(info_s.width));
    ui->height->setText(QString::number(info_s.heigth));
    if (info_s.version == 40)
        ui->version->setText("3");
    else if (info_s.version == 108)
        ui->version->setText("4");
    else
        ui->version->setText("5");
    if (info_s.is_readable)
        ui->is_readable->setText("Есть");
    else
        ui->is_readable->setText("Нет");
    if (info_s.is_writeable)
        ui->is_writeable->setText("Есть");
    else
        ui->is_writeable->setText("Нет");
}

void Information::on_pushButton_clicked()
{
    this->close();
}

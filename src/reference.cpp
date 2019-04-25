#include "reference.h"
#include "ui_reference.h"

#include <qlabel.h>



Reference::Reference(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Reference)
{
    ui->setupUi(this);
    ui->label->setWordWrap(true);
}

Reference::~Reference()
{
    delete ui;
}

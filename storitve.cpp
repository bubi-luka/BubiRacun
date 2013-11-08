#include "storitve.h"
#include "ui_storitve.h"

storitve::storitve(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::storitve)
{
    ui->setupUi(this);
}

storitve::~storitve()
{
    delete ui;
}

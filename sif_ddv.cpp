#include "sif_ddv.h"
#include "ui_sif_ddv.h"

sif_ddv::sif_ddv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sif_ddv)
{
    ui->setupUi(this);
}

sif_ddv::~sif_ddv()
{
    delete ui;
}

#include "wid_ddv.h"
#include "ui_wid_ddv.h"

wid_ddv::wid_ddv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_ddv)
{
    ui->setupUi(this);
}

wid_ddv::~wid_ddv()
{
    delete ui;
}

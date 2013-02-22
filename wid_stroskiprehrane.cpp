#include "wid_stroskiprehrane.h"
#include "ui_wid_stroskiprehrane.h"

wid_stroskiprehrane::wid_stroskiprehrane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_stroskiprehrane)
{
    ui->setupUi(this);
}

wid_stroskiprehrane::~wid_stroskiprehrane()
{
    delete ui;
}

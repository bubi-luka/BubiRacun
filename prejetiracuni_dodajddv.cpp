#include <QKeyEvent>

#include "kodiranje.h"
#include "varnost.h"
#include "prejetiracuni_dodajddv.h"
#include "ui_prejetiracuni_dodajddv.h"

prejetiracuni_dodajddv::prejetiracuni_dodajddv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prejetiracuni_dodajddv)
{
    ui->setupUi(this);

    ui->txt_izbor->setText("");

}

prejetiracuni_dodajddv::~prejetiracuni_dodajddv()
{
    delete ui;
}

void prejetiracuni_dodajddv::on_btn_izhod_clicked() {

    close();

}

void prejetiracuni_dodajddv::on_btn_sprejmi_clicked() {

}

void prejetiracuni_dodajddv::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Return)
    {
        this->on_btn_sprejmi_clicked();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        this->on_btn_izhod_clicked();
    }

}

void prejetiracuni_dodajddv::on_rb_aktivni_toggled() {

}

void prejetiracuni_dodajddv::on_rb_vsi_toggled() {

}

void prejetiracuni_dodajddv::on_tbl_ddv_itemSelectionChanged() {

}

QString prejetiracuni_dodajddv::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString prejetiracuni_dodajddv::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void prejetiracuni_dodajddv::prejem(QString besedilo) {

}

#include <QtSql>
#include <QMessageBox>
#include <QKeyEvent>

#include "kodiranje.h"
#include "varnost.h"
#include "sif_ddv.h"
#include "ui_sif_ddv.h"

sif_ddv::sif_ddv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sif_ddv)
{
    ui->setupUi(this);

    ui->txt_id->setText("");
    ui->txt_vrednost->setText("");

    ui->rb_aktiven->setChecked(true);

    ui->txt_id->setEnabled(false);

}

sif_ddv::~sif_ddv()
{
    delete ui;
}

void sif_ddv::on_btn_izhod_clicked() {

    close();

}

void sif_ddv::on_btn_sprejmi_clicked() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE");
    base.setDatabaseName(dbase_path);
    base.database();
    base.open();
    if(base.isOpen() != true){
        QMessageBox msgbox;
        msgbox.setText("Baze ni bilo moc odpreti");
        msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
        msgbox.exec();
    }
    else {

        QSqlQuery sql_vnesi;
        QSqlQuery sql_preveri;
        sql_preveri.prepare("SELECT * FROM sif_ddv WHERE vrednost LIKE '" +
                             pretvori_v_double(pretvori(ui->txt_vrednost->text())) + "'");
        sql_preveri.exec();
        if ( sql_preveri.next() ) { // v bazi ze obstaja DDV s podano vrednostjo
            sql_vnesi.prepare("UPDATE sif_ddv SET vrednost = ?, aktivnost = ? WHERE vrednost LIKE '" +
                              pretvori_v_double(pretvori(ui->txt_vrednost->text())) + "'");
        }
        else {
            sql_vnesi.prepare("INSERT INTO sif_ddv (vrednost, aktivnost) VALUES (?, ?)");
        }
        sql_vnesi.bindValue(0, pretvori_v_double(pretvori(ui->txt_vrednost->text())));
        if ( ui->rb_aktiven->isChecked() ) {
            sql_vnesi.bindValue(1, "1"); // je aktiven
        }
        else {
            sql_vnesi.bindValue(1, "0"); // ni aktiven
        }
        sql_vnesi.exec();
    }
    base.close();

    // send signal to reload widget
    poslji("ddv");

    // close this window
    close();

}

void sif_ddv::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Return)
    {
        this->on_btn_sprejmi_clicked();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        this->on_btn_izhod_clicked();
    }

}

void sif_ddv::on_rb_aktiven_toggled() {

    if ( ui->rb_aktiven->isChecked() ) {
        ui->rb_aktiven->setChecked(true);
        ui->rb_neaktiven->setChecked(false);
    }
    else {
        ui->rb_aktiven->setChecked(false);
        ui->rb_neaktiven->setChecked(true);
    }

}

void sif_ddv::on_rb_neaktiven_toggled() {

    if ( ui->rb_aktiven->isChecked() ) {
        ui->rb_aktiven->setChecked(true);
        ui->rb_neaktiven->setChecked(false);
    }
    else {
        ui->rb_aktiven->setChecked(false);
        ui->rb_neaktiven->setChecked(true);
    }

}

QString sif_ddv::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString sif_ddv::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

QString sif_ddv::pretvori_v_double(QString besedilo) {

    /*
    * pretvarja znake v format double
    * prejme poljubni format, vrne double
    */

    besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
    besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

    if ( !besedilo.contains(".") ) {
        besedilo.append(".0");
    }

    return besedilo;

}

QString sif_ddv::pretvori_iz_double(QString besedilo) {

    /*
    * pretvarja stevilke v valuto, primerno za obdelavo naprej
    * ni nujno, da je vhodna stevilka resnicno double, lahko gre za drugacno obliko
    */

    besedilo.replace(".", ","); // zamenja decimalno piko (double) za vejiso (SI)
    besedilo.remove(QRegExp("[^0-9,]")); // odstrani vse znake razen stevilk in decimalne vejice

    while ( besedilo.left(1) == "0" ) { // odstranimo vse vodilne nicle
        besedilo.remove(0,1);
    }
    if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0,00
        besedilo.append("0");
    }
    if ( !besedilo.contains(",") ) {
        besedilo.append(",");
    }
    if ( besedilo.left(1) == "," ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
        besedilo.prepend("0");
    }
    if ( besedilo.right(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 0
        besedilo.append("0");
    }

    besedilo.append(" %"); // doda oznako za odstotek

    return besedilo;

}

void sif_ddv::prejem(QString besedilo) {

    if ( besedilo != "Nov ddv") {
        // besedilo nosi ID ze obstojec racun, potrebno je napolniti polja
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE");
        base.setDatabaseName(dbase_path);
        base.database();
        base.open();
        if(base.isOpen() != true){
            QMessageBox msgbox;
            msgbox.setText("Baze ni bilo moc odpreti");
            msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
            msgbox.exec();
        }
        else {
            QSqlQuery sql_napolni;
            sql_napolni.prepare("SELECT * FROM sif_ddv WHERE id LIKE '" + besedilo + "'");
            sql_napolni.exec();
            if (sql_napolni.next()) {
                ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
                ui->txt_vrednost->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString())));
                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("aktivnost")).toString()) == "1" ) { // je aktiven
                    ui->rb_aktiven->setChecked(true);
                }
                else {
                    ui->rb_neaktiven->setChecked(true);
                }
            }
            sql_napolni.clear();
        }
        base.close();

    }

}

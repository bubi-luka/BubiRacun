#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>

#include "popusti.h"
#include "kodiranje.h"

#include "nastavitve.h"
#include "ui_nastavitve.h"

nastavitve::nastavitve(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nastavitve)
{
    ui->setupUi(this);

    // nastavi polja iz baze
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
        // the database is opened
        QSqlQuery sql_napolni;
        sql_napolni.prepare("SELECT * FROM nastavitve");
        sql_napolni.exec();
        while ( sql_napolni.next() ) {
            if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()) == "pot" ) {
                ui->txt_pot->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString()));
            }
            if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()) == "delavniki" ) {
                ui->txt_pot_do_delavnikov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString()));
            }
        }
    }
    base.close();

    // ce so polja prazna, jih nastavi na privzete vrednosti
    if ( ui->txt_pot->text() == "" ) {
        ui->txt_pot->setText(QDir::homePath() + "/BubiRacun-Dokumenti");
    }
    if ( ui->txt_pot_do_delavnikov->text() == "" ) {
        ui->txt_pot_do_delavnikov->setText("http://www.racunovodja.com/mdokumenti/delure2002.asp");
    }

}

nastavitve::~nastavitve()
{
    delete ui;
}

void nastavitve::on_btn_pot_clicked() {

    // izberi mapo za shranjevanje podatkov
    QString mapa_za_shranjevanje = QFileDialog::getExistingDirectory(this,
                                                                     "Izberite mapo za shranjevanje dokumentov",
                                                                     QDir::homePath(), QFileDialog::ShowDirsOnly);

    // ustvariti pot do ustrezne mape
    QDir mapa(mapa_za_shranjevanje);
    if ( !mapa_za_shranjevanje.contains("BubiRacun-Dokumenti") ) {
        mapa.mkdir("BubiRacun-Dokumenti");
        mapa_za_shranjevanje.append("/BubiRacun-Dokumenti");
    }

    ui->txt_pot->setText(mapa_za_shranjevanje);

}

void nastavitve::on_btn_popusti_clicked() {

    popusti *okno = new popusti;
    okno->open();

}

void nastavitve::on_btn_ponastavi_clicked() {

    ui->txt_pot->setText(QDir::homePath() + "/BubiRacun-Dokumenti");

}

void nastavitve::on_btn_izhod_clicked() {

    if ( ui->txt_pot->text() == "" ) {
        exit(0);
    }
    else {
        close();
    }

}

void nastavitve::on_btn_shrani_clicked() {

    // nastavi polja iz baze
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
        // the database is opened
        QSqlQuery sql_shrani;
        sql_shrani.prepare("UPDATE nastavitve SET vrednost = ? WHERE naziv LIKE '" + pretvori("pot")+ "'");
        sql_shrani.bindValue(0, pretvori(ui->txt_pot->text()));
        sql_shrani.exec();
        sql_shrani.clear();
        sql_shrani.prepare("UPDATE nastavitve SET vrednost = ? WHERE naziv LIKE '" + pretvori("delavniki")+ "'");
        sql_shrani.bindValue(0, pretvori(ui->txt_pot_do_delavnikov->text()));
        sql_shrani.exec();
        sql_shrani.clear();
    }
    base.close();

    QMessageBox sporocilo;
    sporocilo.setText("Nastavitve so uspesno shranjene!");
    sporocilo.exec();

    close();

}

// pretvori v in iz kodirane oblike
QString nastavitve::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString nastavitve::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

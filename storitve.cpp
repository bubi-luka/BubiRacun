#include <QtSql>
#include <QMessageBox>
#include "kodiranje.h"

#include "storitve.h"
#include "ui_storitve.h"

storitve::storitve(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::storitve)
{
    ui->setupUi(this);

    // napolnimo spustne sezname
    napolni_davke();
    napolni_enote();
    napolni_kategorije();

}

storitve::~storitve()
{
    delete ui;
}

void storitve::on_btn_izhod_clicked() {

    close();

}

void storitve::on_btn_izbrisi_clicked() {

    // spraznimo vsa vnosna polja
    ui->txt_id->setText("");
    ui->txt_sifra->setText("");
    ui->txt_storitev->setText("");
    ui->txt_cena_brez_ddv->setText("");
    ui->txt_znesek_ddv->setText("");
    ui->txt_znesek_skupaj->setText("");

    ui->cb_enota->clear();
    ui->cb_kategorija->clear();
    ui->cb_podkategorija->clear();
    ui->cb_stopnja_ddv->clear();

    ui->cb_aktivnost->setChecked(false);

    // spremenimo vrednost gumba na vnos
    ui->btn_vnesi->setText("Vnesi");

    // na novo napolnimo spustne sezname
    napolni_davke();
    napolni_enote();
    napolni_kategorije();

}

void storitve::on_btn_vnesi_clicked() {

    bool napaka = false;

    izracunaj_cene();

    if ( ui->txt_sifra->text() == "" ) {
        tvori_sifro();
    }

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_kategorije");
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
        // baza je odprta

        // preveri vnosna polja
        bool napaka = false;
        QSqlQuery sql_preveri;
        sql_preveri.prepare("SELECT * FROM sif_storitve WHERE sifra LIKE '" + pretvori(ui->txt_sifra->text()) + "'");
        sql_preveri.exec();
        if ( sql_preveri.next() ) {
            if ( ui->txt_id->text() != prevedi(sql_preveri.value(sql_preveri.record().indexOf("id")).toString()) ) {
                napaka = true;
                QMessageBox box_napaka;
                box_napaka.setText("Napaka pri vnosu");
                box_napaka.setInformativeText("Ta sifra storitve ze obstaja. Prosim, izberite novo!");
                box_napaka.exec();
            }
        }

        QString stavek = "";
        if ( ui->btn_vnesi->text() == "Vnesi" ) {
            stavek = "INSERT INTO sif_storitve (sifra, kategorija, podkategorija, storitev, enota, stopnja_ddv, znesek_brez_ddv, "
                    "znesek_ddv, znesek_z_ddv, aktivnost) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        }
        else if ( ui->btn_vnesi->text() == "Popravi" ) {
            stavek = "UPDATE sif_storitve SET sifra = ?, kategorija = ?, podkategorija = ?, storitev = ?, enota = ?, "
                    "stopnja_ddv = ?, znesek_brez_ddv = ?, znesek_ddv = ?, znesek_z_ddv = ?, aktivnost = ? WHERE id LIKE '" +
                    pretvori(ui->txt_id->text()) + "'";
        }

        QSqlQuery sql_vnesi;
        sql_vnesi.prepare(stavek);
        sql_vnesi.bindValue(0, pretvori(ui->txt_sifra->text()));
        sql_vnesi.bindValue(1, pretvori(ui->cb_kategorija->currentText()));
        sql_vnesi.bindValue(2, pretvori(ui->cb_podkategorija->currentText()));
        sql_vnesi.bindValue(3, pretvori(ui->txt_storitev->text()));
        sql_vnesi.bindValue(4, pretvori(ui->cb_enota->currentText()));
        sql_vnesi.bindValue(5, pretvori(ui->cb_stopnja_ddv->currentText()));
        sql_vnesi.bindValue(6, pretvori(ui->txt_cena_brez_ddv->text()));
        sql_vnesi.bindValue(7, pretvori(ui->txt_znesek_ddv->text()));
        sql_vnesi.bindValue(8, pretvori(ui->txt_znesek_skupaj->text()));
        if ( ui->cb_aktivnost->isChecked() ) {
            sql_vnesi.bindValue(9, pretvori("1"));
        }
        else {
            sql_vnesi.bindValue(9, pretvori("0"));
        }
        if ( napaka == false ) {
            sql_vnesi.exec();
        }

    }
    base.close();

    if ( napaka == false ) {
        // send signal to reload widget
        poslji("storitve");

        // zapri okno
        close();
    }

}

void storitve::on_cb_kategorija_currentIndexChanged() {

    napolni_podkategorije();

}

void storitve::napolni_kategorije() {

    ui->cb_kategorija->clear();
    ui->cb_kategorija->addItem(""); // dodaj prvo prazno vrstico

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_kategorije");
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
        // baza je odprta

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_kategorije WHERE aktivnost LIKE '1' ORDER BY indeks ASC");
        sql_fill.exec();
        while ( sql_fill.next() ) {
            ui->cb_kategorija->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString()));
        }
    }
    base.close();

    if ( ui->btn_izhod->text() != "Polnim" ) {
        ui->txt_sifra->clear();
    }
}

void storitve::napolni_podkategorije() {

    ui->cb_podkategorija->clear();
    ui->cb_podkategorija->addItem(""); // dodaj prvo prazno vrstico

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_podkategorije");
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
        // baza je odprta

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_podkategorije WHERE aktivnost LIKE '1' AND kategorija LIKE '" + pretvori(ui->cb_kategorija->currentText()) + "'  ORDER BY indeks ASC");
        sql_fill.exec();
        while ( sql_fill.next() ) {
            ui->cb_podkategorija->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString()));
        }
    }
    base.close();

    if ( ui->btn_izhod->text() != "Polnim" ) {
        ui->txt_sifra->clear();
    }

}

void storitve::napolni_enote() {

    ui->cb_enota->clear();
    ui->cb_enota->addItem(""); // dodaj prvo prazno vrstico

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_enote");
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
        // baza je odprta

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_enote WHERE aktivnost LIKE '1'");
        sql_fill.exec();
        while ( sql_fill.next() ) {
            ui->cb_enota->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString()));
        }
    }
    base.close();
}

void storitve::napolni_davke() {

    ui->cb_stopnja_ddv->clear();
    ui->cb_stopnja_ddv->addItem(""); // dodaj prvo prazno vrstico

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_davke");
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
        // baza je odprta

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_ddv WHERE aktivnost LIKE '1'");
        sql_fill.exec();
        while ( sql_fill.next() ) {
            ui->cb_stopnja_ddv->addItem(pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString())) + " %");
        }
    }
    base.close();

}

void storitve::tvori_sifro() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "doloci_sifro");
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
        // baza je odprta

        QString sifra = "";

        // doloci del sifre pripadajoc kategoriji
        QSqlQuery sql_sifra;
        sql_sifra.prepare("SELECT * FROM sif_kategorije WHERE kategorija LIKE '" + pretvori(ui->cb_kategorija->currentText()) + "'");
        sql_sifra.exec();
        if ( sql_sifra.next() ) {
            sifra = prevedi(sql_sifra.value(sql_sifra.record().indexOf("indeks")).toString());
        }
        else {
            sifra = "99";
        }
        sql_sifra.clear();

        sql_sifra.prepare("SELECT * FROM sif_podkategorije WHERE podkategorija LIKE '" + pretvori(ui->cb_podkategorija->currentText()) + "'"
                          " AND kategorija LIKE '" + pretvori(ui->cb_kategorija->currentText()) + "'");
        sql_sifra.exec();
        if ( sql_sifra.next() ) {
            sifra += prevedi(sql_sifra.value(sql_sifra.record().indexOf("indeks")).toString());
        }
        else {
            sifra += "99";
        }
        sql_sifra.clear();

        QString nova_sifra = "";
        QString stara_sifra = sifra;
        for ( int i = 1; i <= 99; i++ ) {
            sifra = stara_sifra;
            qDebug("Zanka for i");
            if ( i << 10 ) {
                sifra += "0" + QString::number(i, 10);
            }
            else {
                sifra += QString::number(i, 10);
            }

            qDebug(sifra.toUtf8());
            if ( nova_sifra == "" ) {
                qDebug("Nova sifra je prazna");
                QSqlQuery sql_check;
                sql_check.prepare("SELECT * FROM sif_storitve WHERE sifra LIKE '" + pretvori(sifra) + "'");
                sql_check.exec();
                if ( !sql_check.next() ) {
                    qDebug("Dobimo novo sifro");
                    nova_sifra = sifra;
                }
            }
        }

        ui->txt_sifra->setText(nova_sifra);
    }

}

void storitve::izracunaj_cene() {

    if ( ui->btn_izhod->text() != "Polnim" ) {
        double stopnja_ddv;
        double znesek_brez_ddv;
        double znesek_ddv;
        double znesek_z_ddv;

        stopnja_ddv = pretvori_v_double(ui->cb_stopnja_ddv->currentText()).toDouble();
        znesek_brez_ddv = pretvori_v_double(ui->txt_cena_brez_ddv->text()).toDouble();

        // izracunamo znesek DDV-ja
        znesek_ddv = znesek_brez_ddv * stopnja_ddv / 100;

        // izracunamo koncni znesek z DDVjem
        znesek_z_ddv = znesek_brez_ddv + znesek_ddv;

        // napolnimo vsa polja z ustreznimi zneski pretvorjenimi v ustrezno obliko
        ui->txt_cena_brez_ddv->setText(QString::number(znesek_brez_ddv, 'f', 2) + " EUR");
        ui->txt_znesek_ddv->setText(QString::number(znesek_ddv, 'f', 2) + " EUR");
        ui->txt_znesek_skupaj->setText(QString::number(znesek_z_ddv, 'f', 2) + " EUR");
    }

}

void storitve::prejem(QString besedilo) {

    QString gumb = "";
    gumb = ui->btn_izhod->text();
    ui->btn_izhod->setText("Polnim");

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_podkategorije");
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
        // baza je odprta

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_storitve WHERE id LIKE '" + pretvori(besedilo) + "'");
        sql_fill.exec();
        if ( sql_fill.next() ) {
            ui->txt_id->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
            ui->txt_sifra->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("sifra")).toString()));
            if ( ui->cb_kategorija->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString())) != -1 ) {
                ui->cb_kategorija->setCurrentIndex(ui->cb_kategorija->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString())));
            }
            else {
                ui->cb_kategorija->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString()));
                ui->cb_kategorija->setCurrentIndex(ui->cb_kategorija->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString())));
            }
            if ( ui->cb_podkategorija->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString())) != -1 ) {
                ui->cb_podkategorija->setCurrentIndex(ui->cb_podkategorija->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString())));
            }
            else {
                ui->cb_podkategorija->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString()));
                ui->cb_podkategorija->setCurrentIndex(ui->cb_podkategorija->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString())));
            }
            ui->txt_storitev->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("storitev")).toString()));
            if ( ui->cb_enota->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString())) != -1 ) {
                ui->cb_enota->setCurrentIndex(ui->cb_enota->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString())));
            }
            else {
                ui->cb_enota->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString()));
                ui->cb_enota->setCurrentIndex(ui->cb_enota->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString())));
            }
            if ( ui->cb_stopnja_ddv->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString())) != -1 ) {
                ui->cb_stopnja_ddv->setCurrentIndex(ui->cb_stopnja_ddv->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString())));
            }
            else {
                ui->cb_stopnja_ddv->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString()));
                ui->cb_stopnja_ddv->setCurrentIndex(ui->cb_stopnja_ddv->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString())));
            }
            ui->txt_cena_brez_ddv->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("znesek_brez_ddv")).toString()));
            ui->txt_znesek_ddv->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("znesek_ddv")).toString()));
            ui->txt_znesek_skupaj->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("znesek_z_ddv")).toString()));
            if ( prevedi(sql_fill.value(sql_fill.record().indexOf("aktivnost")).toString()) == "1" ) {
                ui->cb_aktivnost->setChecked(true);
            }
            else {
                ui->cb_aktivnost->setChecked(false);
            }
            ui->btn_vnesi->setText("Popravi");
        }
        else {
            on_btn_izbrisi_clicked();
        }
    }
    base.close();

    ui->btn_izhod->setText(gumb);

}

void storitve::on_btn_sifra_clicked() {

    tvori_sifro();

}

QString storitve::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString storitve::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

QString storitve::pretvori_v_double(QString besedilo) {

    besedilo.remove(" ");
    besedilo.remove("%");
    besedilo.remove("EUR");
    besedilo.replace(",", ".");

    if ( besedilo.left(1) == "0" ) {
        besedilo.remove(0,1);
    }
    if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0.0
        besedilo.append("0.0");
    }
    if ( besedilo.left(1) == "." ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
        besedilo.prepend("0");
    }
    if ( besedilo.right(1) == "." ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 0
        besedilo.append("0");
    }
    if ( !besedilo.contains(".") ) { // ce je celo stevilo dodamo decimalno locilo in vrednost 0
        besedilo.append(".0");
    }

    return besedilo;

}

QString storitve::pretvori_iz_double(QString besedilo) {

    besedilo.replace(".",",");

    return besedilo;

}

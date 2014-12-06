#include <QtSql>
#include <QMessageBox>
#include "kodiranje.h"

#include "podkategorije.h"
#include "ui_podkategorije.h"

podkategorije::podkategorije(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::podkategorije)
{
    ui->setupUi(this);

    napolni_kategorije();

    napolni_tabelo_podkategorij();

}

podkategorije::~podkategorije()
{
    delete ui;
}

void podkategorije::napolni_tabelo_podkategorij() {

    on_btn_zbrisi_clicked(); // zbrisemo vsa vnosna polja, da nas ne ovirajo pri polnenju tabele

    QString gumb = ui->btn_shrani->text();

    ui->btn_shrani->setText("Polnim");

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "sql_polnjenje_podkategorij");
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

        QString stavek = "";
        if ( ui->rb_aktivni->isChecked() ) {
            stavek = "aktivnost LIKE '1'";
        }

        if ( ui->cb_filter_kategorije->currentText() != "" ) {
            if ( stavek != "" ) { // v primeru, da je pogoj ze prisoten, moramo dodati veznik IN
                stavek += " AND ";
            }
            stavek += "kategorija LIKE '" + pretvori(ui->cb_filter_kategorije->currentText()) + "'";
        }

        // v primeru, da ima pogoj vsebino, dodamo se uvodni del
        if ( stavek != "" ) {
            stavek = " WHERE " + stavek;
        }

        // dodamo sortiranje
        stavek += " ORDER BY kategorija ASC, indeks ASC";

        // clear previous content
        ui->tbl_podkategorije->clear();

        for (int i = 0; i <= 4; i++) {
            ui->tbl_podkategorije->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM sif_podkategorije");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_podkategorije->removeRow(0);
        }

        // start filling the table
        ui->tbl_podkategorije->insertColumn(0);
        ui->tbl_podkategorije->insertColumn(1);
        ui->tbl_podkategorije->insertColumn(2);
        ui->tbl_podkategorije->insertColumn(3);
        ui->tbl_podkategorije->insertColumn(4);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Ime kategorije");
        naslov2->setText("Ime podkategorije");
        naslov3->setText("Indeks");
        naslov4->setText("Aktivnost");

        ui->tbl_podkategorije->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_podkategorije->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_podkategorije->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_podkategorije->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_podkategorije->setHorizontalHeaderItem(4, naslov4);

        ui->tbl_podkategorije->setColumnWidth(0, 35);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_podkategorije" + stavek);
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_podkategorije->insertRow(row);
            ui->tbl_podkategorije->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[5] = {"id", "kategorija", "podkategorija", "indeks", "aktivnost"};

            while (col <= 4) {
                QTableWidgetItem *celica = new QTableWidgetItem;
                if ( polja[i] == "id" ) {
                    celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                }
                else if ( polja[i] == "aktivnost" ) {
                    if ( prevedi(sql_fill.value(sql_fill.record().indexOf("aktivnost")).toString()) == "1" ) { // je aktivna
                        celica->setText("Aktivna");
                    }
                    else { // vrednost je 0
                        celica->setText("Neaktivna");
                    }
                }
                else {
                    celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                }
                ui->tbl_podkategorije->setItem(row, col, celica);

                col++;
                i++;

            }

            row++;
        }
    }
    base.close();

    ui->btn_shrani->setText(gumb);

}

void podkategorije::napolni_kategorije() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "sql_polnenje_kategorij");
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

        // najprej spraznimo vse spustne sezname
        ui->cb_filter_kategorije->clear();
        ui->cb_kategorija->clear();

        // dodamo prvo prazno mesto
        ui->cb_filter_kategorije->addItem("");
        ui->cb_kategorija->addItem("");

        // napolnimo kategorije z novimi zadetki
        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_kategorije WHERE aktivnost LIKE '1' ORDER BY indeks ASC");
        sql_fill.exec();
        while ( sql_fill.next() ) {
            ui->cb_filter_kategorije->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString()));
            ui->cb_kategorija->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString()));
        }
    }
    base.close();

}

void podkategorije::on_btn_izhod_clicked() {

    close();

}

void podkategorije::on_btn_zbrisi_clicked() {

    ui->txt_id->setText("");
    ui->txt_indeks->setValue(10);
    ui->cb_kategorija->setCurrentIndex(ui->cb_kategorija->findText(""));
    ui->txt_podkategorija->setText("");
    ui->cb_aktivnost->setChecked(false);
    ui->btn_shrani->setText("Vnesi");

}

void podkategorije::on_btn_shrani_clicked() {

    // preverimo ustreznost vnostnih polj
    bool napaka = false;

    if ( ui->cb_kategorija->currentText() == "" ) {
        napaka = true;
        QMessageBox box_napaka;
        box_napaka.setText("Napaka pri vnosu");
        box_napaka.setInformativeText("Prosim, izberite ustrezno kategorijo!");
        box_napaka.exec();
    }
    if ( ui->txt_podkategorija->text() == "" ) {
        napaka = true;
        QMessageBox box_napaka;
        box_napaka.setText("Napaka pri vnosu");
        box_napaka.setInformativeText("Prosim, vnesite ime podkategorije!");
        box_napaka.exec();
    }

    if ( napaka == false ) {
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "sql_shrani");
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

            QSqlQuery sql_vnesi;
            if (ui->btn_shrani->text() == "Vnesi") { // vnesi novo kategorijo
                sql_vnesi.prepare("INSERT INTO sif_podkategorije (kategorija, podkategorija, indeks, aktivnost) VALUES (?, ?, ?, ?)");
            }
            else { // popravi ze obstojeci vnos
                sql_vnesi.prepare("UPDATE sif_podkategorije SET kategorija = ?, podkategorija = ?, indeks = ?, aktivnost = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
            }
            sql_vnesi.bindValue(0, pretvori(ui->cb_kategorija->currentText()));
            sql_vnesi.bindValue(1, pretvori(ui->txt_podkategorija->text()));
            sql_vnesi.bindValue(2, pretvori(ui->txt_indeks->text()));
            if ( ui->cb_aktivnost->isChecked() ) {
                sql_vnesi.bindValue(3, pretvori("1"));
            }
            else {
               sql_vnesi.bindValue(3, pretvori("0"));
            }
            sql_vnesi.exec();
        }
        base.close();

        // da osvežimo tabelo, jo ponovno naložimo
        napolni_tabelo_podkategorij();
    }

}

void podkategorije::on_tbl_podkategorije_itemSelectionChanged() {

    if ( ui->btn_shrani->text() != "Polnim" ) {

        if ( ui->tbl_podkategorije->selectedItems().count() == 5 ) { // prikazi zadetke samo, ce je oznacena ena in samo ena vrstica
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

                QSqlQuery sql_fill;
                sql_fill.prepare("SELECT * FROM sif_podkategorije WHERE id LIKE '" + pretvori(ui->tbl_podkategorije->selectedItems().takeAt(0)->text()) + "'");
                sql_fill.exec();
                if ( sql_fill.next() ) {
                    ui->txt_id->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
                    ui->txt_indeks->setValue(prevedi(sql_fill.value(sql_fill.record().indexOf("indeks")).toString()).toInt());
                    ui->txt_podkategorija->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString()));
                    ui->cb_kategorija->setCurrentIndex(ui->cb_kategorija->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString())));
                    if ( prevedi(sql_fill.value(sql_fill.record().indexOf("aktivnost")).toString()) == "1" ) { // je aktivna
                        ui->cb_aktivnost->setChecked(true);
                    }
                    else {
                        ui->cb_aktivnost->setChecked(false);
                    }
                }
            }
            base.close();
            ui->btn_shrani->setText("Popravi");
        }
        else {
            ui->txt_id->setText("");
            ui->txt_podkategorija->setText("");
            ui->txt_indeks->setValue(10);
            ui->cb_aktivnost->setChecked(false);
            ui->btn_shrani->setText("Vnesi");
        }
    }

}

void podkategorije::on_rb_vsi_toggled() {

    napolni_tabelo_podkategorij();

}

void podkategorije::on_rb_aktivni_toggled() {

    napolni_tabelo_podkategorij();

}

void podkategorije::on_cb_filter_kategorije_currentIndexChanged() {

    napolni_tabelo_podkategorij();

}

QString podkategorije::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString podkategorije::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

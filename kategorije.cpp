#include <QtSql>
#include <QMessageBox>
#include "kodiranje.h"

#include "kategorije.h"
#include "ui_kategorije.h"

kategorije::kategorije(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kategorije)
{
    ui->setupUi(this);

    napolni_tabelo_kategorij();

}

kategorije::~kategorije()
{
    delete ui;
}

void kategorije::napolni_tabelo_kategorij() {

    on_btn_zbrisi_clicked(); // zbrisemo vsa vnosna polja, da nas ne ovirajo pri polnenju tabele

    QString gumb = ui->btn_shrani->text();

    ui->btn_shrani->setText("Polnim");

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

        QString stavek = "";
        if ( ui->rb_aktivni->isChecked() ) {
            stavek = " WHERE aktivnost LIKE '1'";
        }

        // clear previous content
        ui->tbl_kategorije->clear();

        for (int i = 0; i <= 3; i++) {
            ui->tbl_kategorije->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM sif_kategorije");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_kategorije->removeRow(0);
        }

        // start filling the table
        ui->tbl_kategorije->insertColumn(0);
        ui->tbl_kategorije->insertColumn(1);
        ui->tbl_kategorije->insertColumn(2);
        ui->tbl_kategorije->insertColumn(3);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Ime kategorije");
        naslov2->setText("Indeks");
        naslov3->setText("Aktivnost");

        ui->tbl_kategorije->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_kategorije->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_kategorije->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_kategorije->setHorizontalHeaderItem(3, naslov3);

        ui->tbl_kategorije->setColumnWidth(0, 35);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_kategorije ORDER BY indeks ASC" + stavek);
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_kategorije->insertRow(row);
            ui->tbl_kategorije->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[4] = {"id", "kategorija", "indeks", "aktivnost"};

            while (col <= 3) {
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
                ui->tbl_kategorije->setItem(row, col, celica);

                col++;
                i++;

            }

            row++;
        }
    }
    base.close();

    ui->btn_shrani->setText(gumb);

}

void kategorije::on_btn_izhod_clicked() {

    close();

}

void kategorije::on_btn_zbrisi_clicked() {

    ui->txt_id->setText("");
    ui->txt_indeks->setValue(10);
    ui->txt_kategorija->setText("");
    ui->cb_aktivnost->setChecked(false);
    ui->btn_shrani->setText("Vnesi");

}

void kategorije::on_btn_shrani_clicked() {

    // preverimo vnosna polja
    bool napaka = false;
    if ( ui->txt_kategorija->text() == "" ) {
        napaka = true;
        QMessageBox box_napaka;
        box_napaka.setText("Napaka pri vnosu");
        box_napaka.setInformativeText("Prosim, vnesite ime kategorije!");
        box_napaka.exec();
    }

    if ( napaka == false ) {
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
                sql_vnesi.prepare("INSERT INTO sif_kategorije (kategorija, indeks, aktivnost) VALUES (?, ?, ?)");
            }
            else { // popravi ze obstojeci vnos
                sql_vnesi.prepare("UPDATE sif_kategorije SET kategorija = ?, indeks = ?, aktivnost = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
            }
            sql_vnesi.bindValue(0, pretvori(ui->txt_kategorija->text()));
            sql_vnesi.bindValue(1, pretvori(ui->txt_indeks->text()));
            if ( ui->cb_aktivnost->isChecked() ) {
                sql_vnesi.bindValue(2, pretvori("1"));
            }
            else {
               sql_vnesi.bindValue(2, pretvori("0"));
            }
            sql_vnesi.exec();
        }
        base.close();

        // da osvežimo tabelo, jo ponovno naložimo
        napolni_tabelo_kategorij();
    }

}

void kategorije::on_tbl_kategorije_itemSelectionChanged() {

    if ( ui->btn_shrani->text() != "Polnim" ) {

        if ( ui->tbl_kategorije->selectedItems().count() == 4 ) { // prikazi zadetke samo, ce je oznacena ena in samo ena vrstica
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
                sql_fill.prepare("SELECT * FROM sif_kategorije WHERE id LIKE '" + pretvori(ui->tbl_kategorije->selectedItems().takeAt(0)->text()) + "'");
                sql_fill.exec();
                if ( sql_fill.next() ) {
                    ui->txt_id->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
                    ui->txt_indeks->setValue(prevedi(sql_fill.value(sql_fill.record().indexOf("indeks")).toString()).toInt());
                    ui->txt_kategorija->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString()));
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
            ui->txt_kategorija->setText("");
            ui->txt_indeks->setValue(10);
            ui->cb_aktivnost->setChecked(false);
            ui->btn_shrani->setText("Vnesi");
        }
    }

}

void kategorije::on_rb_vsi_toggled() {

    napolni_tabelo_kategorij();

}

void kategorije::on_rb_aktivni_toggled() {

    napolni_tabelo_kategorij();

}

QString kategorije::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString kategorije::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

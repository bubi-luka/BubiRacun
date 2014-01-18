#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"

#include "wid_dobropis.h"
#include "ui_wid_dobropis.h"

wid_dobropis::wid_dobropis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_dobropis)
{
    ui->setupUi(this);

    ui->txt_id_projekt->setVisible(false);
    ui->txt_id_racuna->setVisible(false);
    ui->txt_id_stranka->setVisible(false);

}

wid_dobropis::~wid_dobropis()
{
    delete ui;
}

void wid_dobropis::on_btn_izhod_clicked() {

    close();

}

void wid_dobropis::on_btn_izprazni_clicked() {

    ui->tbl_storitve->clearSelection();

}

void wid_dobropis::prejem(QString besedilo) {

    if ( besedilo.left(8) == "Dobropis" ) {
        ui->txt_id_racuna->setText(besedilo.right(besedilo.length() - 8));
        napolni_racun();
        napolni_tabelo();
        izracunaj();
    }

}

void wid_dobropis::napolni_racun() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_racune");
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
        // the database is open

        QSqlQuery sql_racun;
        sql_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(ui->txt_id_racuna->text()) + "'");
        sql_racun.exec();
        if ( sql_racun.next() ) {
            ui->txt_id_stranka->setText(prevedi(sql_racun.value(sql_racun.record().indexOf("stranka")).toString()));
            ui->txt_id_projekt->setText(prevedi(sql_racun.value(sql_racun.record().indexOf("projekt")).toString()));
        }
    }
    base.close();

}

void wid_dobropis::napolni_tabelo() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni-tabelo-racunov");
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

        // clear previous content
        ui->tbl_storitve->clear();

        for (int i = 0; i <= 4; i++) {
            ui->tbl_storitve->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM racuni");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_storitve->removeRow(0);
        }

        // start filling the table
        ui->tbl_storitve->insertColumn(0);
        ui->tbl_storitve->insertColumn(1);
        ui->tbl_storitve->insertColumn(2);
        ui->tbl_storitve->insertColumn(3);
        ui->tbl_storitve->insertColumn(4);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Ime storitve");
        naslov2->setText("St. ur");
        naslov3->setText("Del dobropisa");
        naslov4->setText("St. ur dobropisa");

        ui->tbl_storitve->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_storitve->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_storitve->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_storitve->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_storitve->setHorizontalHeaderItem(4, naslov4);

        ui->tbl_storitve->setColumnWidth(0, 0);
        ui->tbl_storitve->setColumnWidth(1, 200);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id_racuna->text()) + "'");
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            // filtriramo glede na mesec in leto
            QString filter = "pozitivno";

            if ( filter == "pozitivno" ) {
                ui->tbl_storitve->insertRow(row);
                ui->tbl_storitve->setRowHeight(row, 20);
                int col = 0;
                int i = 0;
                QString polja[5] = {"id", "opravilo_storitev", "ur_dela", "dobropis", "dobropis_st_ur"};

                while (col <= 4) {
                    QTableWidgetItem *celica = new QTableWidgetItem;

                    // storniranim vrsticam spremenimo barvo ozadja in besedila
                    if ( prevedi(sql_fill.value(sql_fill.record().indexOf("stornacija")).toString()) == "1" ) {
                        celica->setBackgroundColor(Qt::red);
                        celica->setTextColor(Qt::black);
                    }

                    if ( polja[i] == "id" ) {
                        celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                    }
                    else if ( polja[i] == "dobropis" ) {
                        if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) == "1" ) { // storitev je na dobropistu
                            celica->setText("DA");
                        }
                        else { // storitev ni na dobropisu
                            celica->setText("NE");
                        }
                    }
                    else if ( polja[i] == "ur_dela" || polja[i] == "dobropis_st_ur" ) {
                        celica->setText(prevedi(pretvori_iz_double(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString())));
                    }
                    else {
                        celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    }

                    ui->tbl_storitve->setItem(row, col, celica);
                    col++;
                    i++;
                }
                row++;
            }
        }
    }
    base.close();

}

void wid_dobropis::izracunaj() {

    // nastavimo spremenljivke
    double dobropis_znesek = 0.0;
    double dobropis_ddv = 0.0;
    double dobropis_koncni = 0.0;

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni-tabelo-racunov");
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

        // sestej vse zneske pod tabelo
        QSqlQuery sql_sestej;
        sql_sestej.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id_racuna->text()) + "'");
        sql_sestej.exec();
        while ( sql_sestej.next() ) {
            dobropis_znesek += pretvori_v_double(prevedi(sql_sestej.value(sql_sestej.record().indexOf("dobropis_znesek")).toString())).toDouble();
            dobropis_ddv += pretvori_v_double(prevedi(sql_sestej.value(sql_sestej.record().indexOf("dobropis_ddv")).toString())).toDouble();
            dobropis_koncni += pretvori_v_double(prevedi(sql_sestej.value(sql_sestej.record().indexOf("dobropis_koncni")).toString())).toDouble();
        }

        // prikazemo zneske v poljih zdruzkov pod tabelo
        ui->txt_znesek_brez_ddv->setText(QString::number(dobropis_znesek, 'f', 2));
        ui->txt_znesek_ddv->setText(QString::number(dobropis_ddv, 'f', 2));
        ui->txt_znesek_z_ddv->setText(QString::number(dobropis_koncni, 'f', 2));

    }
    base.close();

}

QString wid_dobropis::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_dobropis::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

QString wid_dobropis::pretvori_v_double(QString besedilo) {

    besedilo.replace(",", ".");
    besedilo.remove(QRegExp("[^0-9\\.]"));

    besedilo = QString::number(besedilo.toDouble(), 'f', 2);

    return besedilo;

}

QString wid_dobropis::pretvori_iz_double(QString besedilo) {

    besedilo.replace(".",",");

    return besedilo;

}

void wid_dobropis::on_tbl_storitve_itemSelectionChanged() {

    if ( ui->tbl_storitve->selectedItems().count() == 0 ) {
        ui->txt_id_storitve->setText("");
        ui->txt_ime_storitve->setText("");
        ui->txt_st_enot_na_dobropisu->setText("");
        ui->txt_st_enot_na_racunu->setText("");
        ui->cb_aktivirana->setChecked(false);
    }
    else {
        ui->txt_id_storitve->setText(ui->tbl_storitve->selectedItems().takeAt(0)->text());
        ui->txt_ime_storitve->setText(ui->tbl_storitve->selectedItems().takeAt(1)->text());
        ui->txt_st_enot_na_racunu->setText(ui->tbl_storitve->selectedItems().takeAt(2)->text());
        if ( ui->tbl_storitve->selectedItems().takeAt(3)->text() == "DA" ) {
            ui->cb_aktivirana->setChecked(true);
        }
        else {
            ui->cb_aktivirana->setChecked(false);
        }
        ui->txt_st_enot_na_dobropisu->setText(ui->tbl_storitve->selectedItems().takeAt(4)->text());
    }

}

void wid_dobropis::on_btn_potrdi_clicked() {

    // shrani spremembe nazaj v tabelo
    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni-tabelo-racunov");
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

        // nastavimo spremenljivke
        QString dobropis = "";
        double dobropis_st_ur = 0.0;

        // ugotovimo, ali je opravilo na dobropisu ali ne
        if ( ui->cb_aktivirana->isChecked() ) {
            dobropis = "1";
        }
        else {
            dobropis = "0";
        }

        if ( dobropis == "0" ) {
            ui->txt_st_enot_na_dobropisu->setText("0.00");
        }

        dobropis_st_ur = pretvori_v_double(ui->txt_st_enot_na_dobropisu->text()).toDouble();

        // nastavimo spremenljivke
        double urna_postavka = 0.0;
        double opravilo_znesek = 0.0;
        double opravilo_ddv = 0.0;
        double opravilo_koncno = 0.0;

        // preracunamo vrednost glede na popuste in podrazitve
        QSqlQuery sql_pridobi;
        sql_pridobi.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id_storitve->text()) + "'");
        sql_pridobi.exec();
        if ( sql_pridobi.next() ) {

            if ( dobropis == "1" ) {
                // preracunaj popuste
                double popusti = 0.0;
                popusti += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("popust_fb1")).toString())).toDouble();
                popusti += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("popust_fb2")).toString())).toDouble();
                popusti += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("popust_komb1")).toString())).toDouble();
                popusti += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("popust_komb1")).toString())).toDouble();
                popusti += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("popust_stranka")).toString())).toDouble();
                popusti += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("popust_kupon")).toString())).toDouble();
                popusti += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("popust_akcija")).toString())).toDouble();

                // preracunaj podrazitve
                double podrazitve = 0.0;
                if ( prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("pribitek_vikend")).toString()) == "1" ) {
                    podrazitve += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("podrazitev_vikend")).toString())).toDouble();
                }
                if ( prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("pribitek_hitrost")).toString()) == "1" ) {
                    podrazitve += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("podrazitev_hitrost")).toString())).toDouble();
                }
                if ( prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("pribitek_zapleti")).toString()) == "1" ) {
                    podrazitve += pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("podrazitev_zapleti")).toString())).toDouble();
                }

                // upostevaj pri urni postavki
                urna_postavka = pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("urna_postavka_brez_ddv")).toString())).toDouble();
                urna_postavka = ( 1 + podrazitve / 100 ) * urna_postavka;

                opravilo_znesek = dobropis_st_ur * urna_postavka;
                opravilo_znesek = opravilo_znesek * ( 1 - popusti / 100 );

                opravilo_ddv = opravilo_znesek * pretvori_v_double(prevedi(sql_pridobi.value(sql_pridobi.record().indexOf("ddv")).toString())).toDouble() / 100;

                opravilo_koncno = opravilo_znesek + opravilo_ddv;
            }

            // ce je opravilo na dobropisu, vnesemo izracunane parametre, drugace jih prepisemo z 0
            QSqlQuery sql_shrani;
            sql_shrani.prepare("UPDATE opravila SET dobropis = ?, dobropis_st_ur = ?, dobropis_znesek = ?, dobropis_ddv = ?, dobropis_koncni = ? WHERE id LIKE '" + pretvori(ui->txt_id_storitve->text()) + "'");
            sql_shrani.bindValue(0, pretvori(dobropis));
            sql_shrani.bindValue(1, pretvori(QString::number(dobropis_st_ur, 'f', 2)));
            sql_shrani.bindValue(2, pretvori(QString::number(opravilo_znesek, 'f', 2)));
            sql_shrani.bindValue(3, pretvori(QString::number(opravilo_ddv, 'f', 2)));
            sql_shrani.bindValue(4, pretvori(QString::number(opravilo_koncno, 'f', 2)));
            sql_shrani.exec();
        }
    }
    base.close();

    // osvezi pogled v tabeli opravil
    napolni_tabelo();
    izracunaj();

}

void wid_dobropis::on_cb_aktivirana_toggled() {

    if ( ui->cb_aktivirana->isChecked() ) {
        ui->txt_st_enot_na_dobropisu->setText(ui->txt_st_enot_na_racunu->text());
    }
    else {
        ui->txt_st_enot_na_dobropisu->setText("0.00");
    }

}

void wid_dobropis::on_txt_st_enot_na_dobropisu_editingFinished() {

    ui->txt_st_enot_na_dobropisu->setText(pretvori_iz_double(pretvori_v_double(ui->txt_st_enot_na_dobropisu->text())));
    double ure_opravljene = 0.0;
    double ure_dobropis = 0.0;

    ure_opravljene = pretvori_v_double(ui->txt_st_enot_na_racunu->text()).toDouble();
    ure_dobropis = pretvori_v_double(ui->txt_st_enot_na_dobropisu->text()).toDouble();

    if ( ure_opravljene < ure_dobropis ) {
        ui->txt_st_enot_na_dobropisu->setText(pretvori_iz_double(pretvori_v_double(ui->txt_st_enot_na_racunu->text())));
    }

}

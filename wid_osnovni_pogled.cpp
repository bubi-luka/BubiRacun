#include <QtSql>
#include <QMessageBox>
#include <QDate>

#include "wid_osnovni_pogled.h"
#include "ui_wid_osnovni_pogled.h"
#include "kodiranje.h"
#include "varnost.h"
#include "datum.h"
#include "stranke.h"
#include "potninalogi.h"
#include "prejetiracuni.h"

wid_osnovni_pogled::wid_osnovni_pogled(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_osnovni_pogled)
{
    ui->setupUi(this);


    napolni_projekte();
    napolni_stranke();
    napolni_potne_naloge();
    napolni_prejete_racune();
    napolni_izdane_racune();
    napolni_predracune();

}

wid_osnovni_pogled::~wid_osnovni_pogled()
{
    delete ui;
}

QString wid_osnovni_pogled::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_osnovni_pogled::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void wid_osnovni_pogled::napolni_projekte() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "op_projekti");
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

        // prestej odprte projekte
        int odprti_projekti = 0;
        QStringList seznam_projektov;

        QString datum = "." + QDate::currentDate().toString("MM") + "." + QDate::currentDate().toString("yyyy");

        QSqlQuery sql_odprti_projekti;
        // stevilo projektov, ki so trenutno v teku
        sql_odprti_projekti.prepare("SELECT * FROM projekti WHERE status_projekta LIKE '" + pretvori("2") + "'"); // projekt je v teku
        //        Za primer, ko gledamo samo stevilo projektov, ki so bili potrjeni v tekocem mesecu
        //        sql_odprti_projekti.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori("1") +
        //                                    "' AND status_racuna LIKE 'Pmoilii' AND datum_oddaje_racuna LIKE '%" + datum + "%'");
        sql_odprti_projekti.exec();
        while ( sql_odprti_projekti.next() ) {
            // stevilo projektov, ki so trenutno v teku
            odprti_projekti++;
            //            Za primer, ko gledamo samo stevilo projektov, ki so bili potrjeni v tekocem mesecu
            //            if ( !seznam_projektov.contains(prevedi(sql_odprti_projekti.value(sql_odprti_projekti.record().indexOf("projekt")).toString())) ) {
            //                seznam_projektov.append(prevedi(sql_odprti_projekti.value(sql_odprti_projekti.record().indexOf("projekt")).toString()));
            //                odprti_projekti++;
            //            }
        }
        seznam_projektov.clear();

        ui->txt_odprti_projekti->setText(QString::number(odprti_projekti, 10));

        // prestej zavrnjene predracune / zavrnjene projekte
        int zavrnjeni_projekti = 0;

        QSqlQuery sql_zavrnjeni_projekti;
        sql_zavrnjeni_projekti.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori("1") +
                                       "' AND status_racuna LIKE 'Zvripnzj' AND datum_oddaje_racuna LIKE '%" + datum + "%'");
        sql_zavrnjeni_projekti.exec();
        while ( sql_zavrnjeni_projekti.next() ) {
            if ( !seznam_projektov.contains(prevedi(sql_zavrnjeni_projekti.value(sql_zavrnjeni_projekti.record().indexOf("projekt")).toString())) ) {
                seznam_projektov.append(prevedi(sql_zavrnjeni_projekti.value(sql_zavrnjeni_projekti.record().indexOf("projekt")).toString()));
                zavrnjeni_projekti++;
            }
        }

        ui->txt_zavrnjeni_projekti->setText(QString::number(zavrnjeni_projekti, 10));
    }
    base.close();

}

void wid_osnovni_pogled::napolni_stranke() {

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_stranke->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_stranke->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_stranke->horizontalHeader()->sortIndicatorSection();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "op_stranke");
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

        ui->tbl_stranke->clear();

        for (int i = 0; i <= 3; i++) {
            ui->tbl_stranke->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM stranke");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_stranke->removeRow(0);
        }

        // start filling the table
        ui->tbl_stranke->insertColumn(0);
        ui->tbl_stranke->insertColumn(1);
        ui->tbl_stranke->insertColumn(2);

        // set proper width to the first four columns

        ui->tbl_stranke->setColumnWidth(0, 0);
        ui->tbl_stranke->setColumnWidth(1, 150);
        ui->tbl_stranke->setColumnWidth(2, 130);

        // start filling the table
        QStringList naslovi;
        naslovi.append("ID");
        naslovi.append("Ime / Naziv");
        naslovi.append("Kontakt");

        ui->tbl_stranke->setHorizontalHeaderLabels(naslovi);

        // set list of custumers, that have open projects
        QStringList stranke;
        stranke.clear();

        // search for the project, that are currently open
        QSqlQuery sql_projekti;
        sql_projekti.prepare("SELECT * FROM projekti WHERE status_projekta LIKE '" + pretvori("2") + "'"); // projekt je v teku
        sql_projekti.exec();
        while ( sql_projekti.next() ) {
            if ( !stranke.contains(prevedi(sql_projekti.value(sql_projekti.record().indexOf("stranka")).toString())) ) {
                stranke.append(prevedi(sql_projekti.value(sql_projekti.record().indexOf("stranka")).toString()));
            }
        }

        // for each of the customer add a line in the table
        int row = 0;

        for ( int a = 0; a < stranke.count(); a++ ) {
            QSqlQuery sql_stranke;
            sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(stranke.value(a)) + "'");
            sql_stranke.exec();
            if ( sql_stranke.next() ) {

                // insert new row
                ui->tbl_stranke->insertRow(row);
                ui->tbl_stranke->setRowHeight(row, 20);

                int col = 0;
                int i = 0;

                QString polja[3] = {"id", "ime", "telefon"};

                // fill columns
                while ( col < 3 ) {

                    QTableWidgetItem *celica = new QTableWidgetItem;
                    if ( polja[i] == "id" ) {
                        celica->setData(Qt::DisplayRole, prevedi(sql_stranke.value(sql_stranke.record().indexOf(polja[i])).toString()).toInt());
                    }
                    else if ( polja[i] == "ime" ) {
                        QString stranka = "";
                        if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "1" ) { // fizicna
                            stranka = prevedi(sql_stranke.value(sql_stranke.record().indexOf("priimek")).toString()) + " " +
                                      prevedi(sql_stranke.value(sql_stranke.record().indexOf("ime")).toString());
                        }
                        else { // pravna
                            stranka = prevedi(sql_stranke.value(sql_stranke.record().indexOf("ime")).toString());
                        }
                        celica->setText(stranka);
                    }
                    else if ( polja[i] == "telefon" ) {
                        QString telefon = "";
                        if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("gsm")).toString()) != "+(0)/--" ) {
                            telefon = prevedi(sql_stranke.value(sql_stranke.record().indexOf("gsm")).toString());
                        }
                        else if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("telefon")).toString()) != "+(0)/--" ) {
                            telefon = prevedi(sql_stranke.value(sql_stranke.record().indexOf("telefon")).toString());
                        }
                        else {
                            telefon = prevedi(sql_stranke.value(sql_stranke.record().indexOf("email")).toString());
                        }
                        celica->setText(telefon);
                    }
                    else {
                        celica->setText(sql_stranke.value(sql_stranke.record().indexOf(polja[i])).toString());
                    }

                    if ( celica->text() != "" ) {
                        ui->tbl_stranke->setItem(row, col, celica);
                    }

                    col++;
                    i++;

                }

                row++;
            }
        }
    }
    base.close();

    ui->tbl_stranke->selectRow(izbranec);
    ui->tbl_stranke->sortByColumn(razvrsti, Qt::AscendingOrder);

}

void wid_osnovni_pogled::napolni_potne_naloge() {

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_potni_nalogi->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_potni_nalogi->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_potni_nalogi->horizontalHeader()->sortIndicatorSection();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "op_potni_nalogi");
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

        ui->tbl_potni_nalogi->clear();

        for (int i = 0; i <= 3; i++) {
            ui->tbl_potni_nalogi->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM potni_nalogi");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_potni_nalogi->removeRow(0);
        }

        // start filling the table
        ui->tbl_potni_nalogi->insertColumn(0);
        ui->tbl_potni_nalogi->insertColumn(1);
        ui->tbl_potni_nalogi->insertColumn(2);

        // set proper width to the first four columns

        ui->tbl_potni_nalogi->setColumnWidth(0, 0);
        ui->tbl_potni_nalogi->setColumnWidth(1, 100);
        ui->tbl_potni_nalogi->setColumnWidth(2, 150);

        // start filling the table
        QStringList naslovi;
        naslovi.append("ID");
        naslovi.append("Datum");
        naslovi.append("St. naloga");

        ui->tbl_potni_nalogi->setHorizontalHeaderLabels(naslovi);

        datum *delegate = new datum(this);
        ui->tbl_potni_nalogi->setItemDelegateForColumn(1, delegate);

        int st_nalogov = 0;
        int row = 0;

        QString datum = "." + QDate::currentDate().toString("MM") + "." + QDate::currentDate().toString("yyyy");

        QSqlQuery sql_potni_nalog;
        sql_potni_nalog.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%" + datum + "%'");
        sql_potni_nalog.exec();
        while ( sql_potni_nalog.next() ) {
            st_nalogov++;

            // dodaj novo vrstico v tabelo
            ui->tbl_potni_nalogi->insertRow(row);
            ui->tbl_potni_nalogi->setRowHeight(row, 20);

            int col = 0;
            int i = 0;

            QString polja[3] = {"id", "datum_naloga", "stevilka_naloga"};

            // napolni stolpce
            while ( col < 3 ) {

                QTableWidgetItem *celica = new QTableWidgetItem;
                if ( polja[i] == "id" ) {
                    celica->setData(Qt::DisplayRole, prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf(polja[i])).toString()).toInt());
                }
                else if ( polja[i] == "datum_naloga" ) {
                    celica->setData(Qt::DisplayRole, QDate::fromString(prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf(polja[i])).toString()), "dd'.'MM'.'yyyy"));
                }
                else {
                    celica->setText(prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf(polja[i])).toString()));
                }

                if ( celica->text() != "" ) {
                    ui->tbl_potni_nalogi->setItem(row, col, celica);
                }

                col++;
                i++;

            }

            row++;

        }

        ui->txt_potni_nalogi->setText(QString::number(st_nalogov, 10));

    }
    base.close();

    ui->tbl_potni_nalogi->selectRow(izbranec);
    ui->tbl_potni_nalogi->sortByColumn(razvrsti, Qt::AscendingOrder);

}

void wid_osnovni_pogled::napolni_prejete_racune() {

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_prejeti_racuni->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_prejeti_racuni->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_prejeti_racuni->horizontalHeader()->sortIndicatorSection();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "op_prejeti_racuni");
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

        // podatki v vrsticah
        QString datum1 = "." + QDate::currentDate().toString("MM") + "." + QDate::currentDate().toString("yyyy");

        int st_prejetih_racunov = 0;
        double znesek_prejetih_racunov = 0.0;

        QSqlQuery sql_prejeti_racuni; // na zeljo narocnika sesteje vse, prejete v tekocem mesecu
        sql_prejeti_racuni.prepare("SELECT * FROM prejeti_racuni WHERE datum_prejema LIKE '%" + datum1 + "%'");
        sql_prejeti_racuni.exec();
        while ( sql_prejeti_racuni.next() ) {
            st_prejetih_racunov++;

            // znesek + DDV - zelja narocnika
            znesek_prejetih_racunov += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek")).toString()).toDouble();
        }

        ui->txt_prejeti_racuni->setText(QString::number(st_prejetih_racunov, 10));
        ui->txt_znesek_prejetih_racunov->setText(QString::number(znesek_prejetih_racunov, 'f', 2).replace(".", ",") + " EUR");

        // podatki v tabeli
        ui->tbl_prejeti_racuni->clear();

        for (int i = 0; i <= 4; i++) {
            ui->tbl_prejeti_racuni->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM prejeti_racuni");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_prejeti_racuni->removeRow(0);
        }

        // start filling the table
        ui->tbl_prejeti_racuni->insertColumn(0);
        ui->tbl_prejeti_racuni->insertColumn(1);
        ui->tbl_prejeti_racuni->insertColumn(2);
        ui->tbl_prejeti_racuni->insertColumn(3);

        // set proper width to the first four columns

        ui->tbl_prejeti_racuni->setColumnWidth(0, 0);
        ui->tbl_prejeti_racuni->setColumnWidth(1, 100);
        ui->tbl_prejeti_racuni->setColumnWidth(2, 150);
        ui->tbl_prejeti_racuni->setColumnWidth(3, 150);

        // start filling the table
        QStringList naslovi;
        naslovi.append("ID");
        naslovi.append("Naziv podjetja");
        naslovi.append("Rok placila");
        naslovi.append("Status racunovodstva");

        ui->tbl_prejeti_racuni->setHorizontalHeaderLabels(naslovi);

        datum *delegate = new datum(this);
        ui->tbl_prejeti_racuni->setItemDelegateForColumn(2, delegate);

        int row = 0;

        sql_prejeti_racuni.clear();
        sql_prejeti_racuni.prepare("SELECT * FROM prejeti_racuni WHERE rok_placila LIKE '%" + datum1 + "%' AND status_placila NOT LIKE '" + pretvori("Pla") + "%'");
        sql_prejeti_racuni.exec();
        while ( sql_prejeti_racuni.next() ) {

            // dodaj novo vrstico v tabelo
            ui->tbl_prejeti_racuni->insertRow(row);
            ui->tbl_prejeti_racuni->setRowHeight(row, 20);

            int col = 0;
            int i = 0;

            QString polja[4] = {"id", "izdajatelj_kratki", "rok_placila", "status_racunovodstva"};

            // napolni stolpce
            while ( col < 4 ) {

                QTableWidgetItem *celica = new QTableWidgetItem;

                // vrstice z rokom placila danasnji datum napravi odebeljene
                if ( QDate::currentDate().toString("dd.MM.yyyy") == prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("rok_placila")).toString()) ) {
                    QFont pisava;
                    pisava.setBold(true);
                    celica->setFont(pisava);
                }

                if ( polja[i] == "id" ) {
                    celica->setData(Qt::DisplayRole, prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf(polja[i])).toString()).toInt());
                }
                else if ( polja[i] == "rok_placila" ) {
                    celica->setData(Qt::DisplayRole, QDate::fromString(prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf(polja[i])).toString()), "dd'.'MM'.'yyyy"));
                }
                else {
                    celica->setText(prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf(polja[i])).toString()));
                }

                if ( celica->text() != "" ) {
                    ui->tbl_prejeti_racuni->setItem(row, col, celica);
                }

                col++;
                i++;

            }


            row++;

        }

    }
    base.close();

    ui->tbl_prejeti_racuni->selectRow(izbranec);
    ui->tbl_prejeti_racuni->sortByColumn(razvrsti, Qt::AscendingOrder);

}

void wid_osnovni_pogled::napolni_izdane_racune() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "op_izdani_racuni");
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

        QString datum = "." + QDate::currentDate().toString("MM") + "." + QDate::currentDate().toString("yyyy");

        int st_predracunov = 0;
        double znesek_predracunov = 0.0;
        double znesek_avansa_predracunov = 0.0;

        int st_predplacilnih_racunov = 0;
        double znesek_predplacilnih_racunov = 0.0;

        int st_racunov = 0;
        double znesek_racunov = 0.0;

        QSqlQuery sql_racuni;
        sql_racuni.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%" + datum + "%'");
        sql_racuni.exec();
        while ( sql_racuni.next() ) {

            double znesek = 0.0;
            double avans = 0.0;

            QSqlQuery sql_opravila;
            sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_racuni.value(sql_racuni.record().indexOf("id")).toString() + "'");
            sql_opravila.exec();
            while ( sql_opravila.next() ) {
                // znesek brez DDV - zelja narocnika
                znesek += prevedi(sql_opravila.value(sql_opravila.record().indexOf("znesek_koncni")).toString()).toDouble();
            }
            avans = prevedi(sql_racuni.value(sql_racuni.record().indexOf("avans")).toString()).toDouble();

            // predracuni
            if ( prevedi(sql_racuni.value(sql_racuni.record().indexOf("tip_racuna")).toString()) == "1" ) {
                if ( sql_racuni.value(sql_racuni.record().indexOf("status_racuna")).toString() != pretvori("Potrjen") ) {
                    st_predracunov++;
                    znesek_predracunov += znesek;
                    znesek_avansa_predracunov += avans;
                }
            }
            // predplacilni racuni
            else if ( prevedi(sql_racuni.value(sql_racuni.record().indexOf("tip_racuna")).toString()) == "2" ) {
                st_predplacilnih_racunov++;
                znesek_predplacilnih_racunov += avans;

            }
            // racuni
            else if ( prevedi(sql_racuni.value(sql_racuni.record().indexOf("tip_racuna")).toString()) == "3" ) {
                st_racunov++;
                znesek_racunov += znesek;

            }
        }

        ui->txt_predracuni->setText(QString::number(st_predracunov, 10));
        ui->txt_znesek_predracunov->setText(QString::number(znesek_predracunov, 'f', 2).replace(".", ",") + " EUR");
        ui->txt_avans_predracuna->setText(QString::number(znesek_avansa_predracunov, 'f', 2).replace(".", ",") + " EUR");

        ui->txt_predplacilni->setText(QString::number(st_predplacilnih_racunov, 10));
        ui->txt_znesek_predplacilnih->setText(QString::number(znesek_predplacilnih_racunov, 'f', 2).replace(".", ",") + " EUR");

        ui->txt_racuni->setText(QString::number(st_racunov, 10));
        ui->txt_znesek_racunov->setText(QString::number(znesek_racunov, 'f', 2).replace(".", ",") + " EUR");

    }
    base.close();

}

void wid_osnovni_pogled::on_tbl_stranke_doubleClicked() {

    stranke *uredi = new stranke;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_stranke->selectedItems().takeAt(0)->text());
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_osnovni_pogled::on_tbl_potni_nalogi_doubleClicked() {

    potninalogi *uredi = new potninalogi;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_potni_nalogi->selectedItems().takeAt(0)->text());
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_osnovni_pogled::on_tbl_prejeti_racuni_doubleClicked() {

    prejetiracuni *uredi = new prejetiracuni;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_prejeti_racuni->selectedItems().takeAt(0)->text());
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_osnovni_pogled::osvezi(QString beseda) {

    if ( beseda == "stranke" ) {
        napolni_stranke();
    }
    else if ( beseda == "potninalog" ) {
        napolni_potne_naloge();
    }
    else if ( beseda == "racun" ) {
        napolni_prejete_racune();
    }

}

void wid_osnovni_pogled::napolni_predracune() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "wid_racuni");
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
        ui->tbl_predracun->clear();

        for (int i = 0; i <= 9; i++) {
            ui->tbl_predracun->removeColumn(0);
        }

        QSqlQuery sql_clear("wid_racuni");
        sql_clear.prepare("SELECT * FROM racuni");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_racuni->removeRow(0);
        }

        // start filling the table
        ui->tbl_racuni->insertColumn(0);
        ui->tbl_racuni->insertColumn(1);
        ui->tbl_racuni->insertColumn(2);
        ui->tbl_racuni->insertColumn(3);
        ui->tbl_racuni->insertColumn(4);
        ui->tbl_racuni->insertColumn(5);
        ui->tbl_racuni->insertColumn(6);
        ui->tbl_racuni->insertColumn(7);
        ui->tbl_racuni->insertColumn(8);
        ui->tbl_racuni->insertColumn(9);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;
        QTableWidgetItem *naslov5 = new QTableWidgetItem;
        QTableWidgetItem *naslov6 = new QTableWidgetItem;
        QTableWidgetItem *naslov7 = new QTableWidgetItem;
        QTableWidgetItem *naslov8 = new QTableWidgetItem;
        QTableWidgetItem *naslov9 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Tip racuna");
        naslov2->setText("St. racuna");
        naslov3->setText("Datum izdaje");
        naslov4->setText("Stranka");
        naslov5->setText("Projekt");
        naslov6->setText("Znesek za placilo");
        naslov7->setText("Se placati");
        naslov8->setText("Status placila");
        naslov9->setText("Status racunovodstva");

        ui->tbl_racuni->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_racuni->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_racuni->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_racuni->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_racuni->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_racuni->setHorizontalHeaderItem(5, naslov5);
        ui->tbl_racuni->setHorizontalHeaderItem(6, naslov6);
        ui->tbl_racuni->setHorizontalHeaderItem(7, naslov7);
        ui->tbl_racuni->setHorizontalHeaderItem(8, naslov8);
        ui->tbl_racuni->setHorizontalHeaderItem(9, naslov9);

        ui->tbl_racuni->setColumnWidth(0, 35);

        datum *delegate = new datum(this);
        ui->tbl_racuni->setItemDelegateForColumn(3, delegate);

        QString projekt = "";

        QSqlQuery sql_projekt;
        sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
        sql_projekt.exec();
        if ( sql_projekt.next() ) {
            projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
        }

        QString stavek = "";

        if ( ui->cb_racun->currentText() != "" ) {
            stavek += " AND tip_racuna LIKE '" + pretvori(ui->cb_racun->currentText()).left(1) + "'";
        }
        if ( ui->cb_placilo->currentText() != "" ) {
            stavek += " AND status_placila LIKE '" + pretvori (ui->cb_placilo->currentText()) + "'";
        }
        if ( ui->cb_racunovodstvo->currentText() != "" ) {
            stavek += " AND status_racunovodstva LIKE '" + pretvori (ui->cb_racunovodstvo->currentText()) + "'";
        }

        if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
            stavek = " WHERE" + stavek.right(stavek.length() - 4);
        }

        QSqlQuery sql_fill("wid_racuni");
        if ( ui->txt_stprojekta->text() != "*" ) {
            sql_fill.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + projekt + "'" + stavek);
        }
        else {
            sql_fill.prepare("SELECT * FROM racuni" + stavek);
        }
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            // filtriramo glede na mesec in leto
            QString filter = "pozitivno";
            if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
                QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
                QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
                if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
                    filter = "negativno";
                }
            }
            else if ( ui->cb_mesec->currentText() != "" ) {
                QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
                if ( mesec != ui->cb_mesec->currentText().left(2) ) {
                    filter = "negativno";
                }
            }
            else if ( ui->cb_leto->currentText() != "" ) {
                QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
                if ( leto != ui->cb_leto->currentText() ) {
                    filter = "negativno";
                }
            }
            // filtriramo glede na javni, zasebni status racuna
            if ( vApp->state() != pretvori("private") ) {

                // doloci vse tri cifre
                QString sklic = prevedi(sql_fill.value(sql_fill.record().indexOf("sklic")).toString());
                sklic = sklic.right(sklic.length() - 5); // odbijemo drzavo in model
                sklic = sklic.right(sklic.length() - 5); // odbijemo stevilko racuna
                int cifra_1 = sklic.left(1).toInt();
                sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_1 in dan
                int cifra_2 = sklic.left(1).toInt();
                sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_2 in mesec
                int cifra_3 = sklic.left(1).toInt();

                // iz prvih dveh izracunaj kontrolno stevilko
                int kontrolna = 0;

                int sestevek = 3 * cifra_1 + 2 * cifra_2;

                int ostanek = sestevek % 11;

                kontrolna = 11 - ostanek;

                if ( kontrolna >= 9 ) {
                    kontrolna = 0;
                }

                // od cifre_3 odstej kontrolno stevilko
                // tako dobis 0 => racun je javen ali 1 => racun je zaseben
                int razlika = cifra_3 - kontrolna;

                if ( razlika == 1 ) {
                    filter = "negativno"; // racuna ne prikazi
                }

            }

            if ( filter == "pozitivno" ) {
                ui->tbl_racuni->insertRow(row);
                ui->tbl_racuni->setRowHeight(row, 20);
                int col = 0;
                int i = 0;
                QString polja[10] = {"id", "tip_racuna", "stevilka_racuna", "datum_izdaje", "stranka", "projekt", "znesek_za_placilo",
                                                         "se_placati", "status_placila", "status_racunovodstva"};

                while (col <= 9) {
                    QTableWidgetItem *celica = new QTableWidgetItem;
                    if ( polja[i] == "id" ) {
                        celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                    }
                    else if ( polja[i] == "datum_izdaje" ) {
                        celica->setData(Qt::DisplayRole, QDate::fromString(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()), "dd'.'MM'.'yyyy"));
                    }
                    else if ( polja[i] == "tip_racuna" ) {
                        if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "1" ) {
                            celica->setText("Predracun");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "2" ) {
                            celica->setText("Predplacilo");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "3" ) {
                            celica->setText("Racun");
                        }
                    }
                    else if ( polja[i] == "stranka" ) {
                        QSqlQuery sql_find_stranka;
                        sql_find_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
                        sql_find_stranka.exec();
                        if ( sql_find_stranka.next() ) {
                            if ( prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("tip")).toString()) == "1" ) {
                                celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("priimek")).toString()) + " " +
                                                prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
                            }
                            else {
                                 celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
                            }
                        }
                        else {
                            celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                        }
                    }
                    else if ( polja[i] == "projekt" ) {
                        QSqlQuery sql_kodiraj;
                        sql_kodiraj.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("projekt")).toString() + "'");
                        sql_kodiraj.exec();
                        if ( sql_kodiraj.next() ) {
                            celica->setText(prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("naslov_projekta")).toString()));
                        }
                    }
                    else if ( polja[i] == "znesek_za_placilo" ) {
                        QSqlQuery sql_kodiraj;
                        sql_kodiraj.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() +
                                                                "' AND tip_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString() + "'");
                        sql_kodiraj.exec();
                        double znesek = 0.0;
                        while ( sql_kodiraj.next() ) {
                            znesek += prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("znesek_koncni")).toString()).toDouble() *
                                    ( prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("ddv")).toString()).toDouble() + 100.0 ) / 100.0;
                        }
                        celica->setText(QString::number(znesek, 'f', 2).replace(".", ",") + " EUR");
                    }
                    else if ( polja[i] == "se_placati" ) {
                        QSqlQuery sql_kodiraj;
                        sql_kodiraj.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() +
                                                                "' AND tip_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString() + "'");
                        sql_kodiraj.exec();
                        double znesek = 0.0;
                        while ( sql_kodiraj.next() ) {
                            znesek += prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("znesek_koncni")).toString()).toDouble() *
                                      ( prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("ddv")).toString()).toDouble() + 100.0 ) / 100.0;
                        }
                        znesek -= prevedi(sql_fill.value(sql_fill.record().indexOf("avans")).toString()).toDouble();
                        celica->setText(QString::number(znesek, 'f', 2).replace(".", ",") + " EUR");
                    }
                    else {
                        celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    }
                    ui->tbl_racuni->setItem(row, col, celica);
                    col++;
                    i++;
                }
                row++;
            }
        }
    }
    base.close();

    ui->tbl_racuni->selectRow(izbranec);
    ui->tbl_racuni->sortByColumn(razvrsti, Qt::AscendingOrder);


}

void wid_osnovni_pogled::on_tbl_predracun_doubleClicked() {

}

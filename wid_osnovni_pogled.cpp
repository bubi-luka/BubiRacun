#include <QtSql>
#include <QMessageBox>
#include <QDate>

#include "wid_osnovni_pogled.h"
#include "ui_wid_osnovni_pogled.h"
#include "kodiranje.h"
#include "varnost.h"
#include "datum.h"

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

        QString mesec = "." + QDate::currentDate().toString("MM") + ".";

        QSqlQuery sql_odprti_projekti;
        sql_odprti_projekti.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori("1") +
                                    "' AND status_racuna LIKE 'Pmoilii' AND datum_oddaje_racuna LIKE '%" + mesec + "%'");
        sql_odprti_projekti.exec();
        while ( sql_odprti_projekti.next() ) {
            if ( !seznam_projektov.contains(prevedi(sql_odprti_projekti.value(sql_odprti_projekti.record().indexOf("projekt")).toString())) ) {
                seznam_projektov.append(prevedi(sql_odprti_projekti.value(sql_odprti_projekti.record().indexOf("projekt")).toString()));
                odprti_projekti++;
            }
        }
        seznam_projektov.clear();

        ui->txt_odprti_projekti->setText(QString::number(odprti_projekti, 10));

        // prestej zavrnjene predracune / zavrnjene projekte
        int zavrnjeni_projekti = 0;

        QSqlQuery sql_zavrnjeni_projekti;
        sql_zavrnjeni_projekti.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori("1") +
                                       "' AND status_racuna LIKE 'Zvripnzj' AND datum_oddaje_racuna LIKE '%" + mesec + "%'");
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

        // get current month
//        QString tekoci_mesec = "." + QDate::currentDate().toString("MM") + ".";

        // set list of custumers, that have open projects
        QStringList stranke;

        // search for the project, that are currently open
        QSqlQuery sql_projekti;
      //  sql_projekti.prepare("SELECT * FROM projekti WHERE status_projekta LIKE '" + pretvori("2") + "'"); // projekt je v teku
        sql_projekti.prepare("SELECT * FROM projekti"); // projekt je v teku
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

}

void wid_osnovni_pogled::napolni_potne_naloge() {

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

        QString mesec = "." + QDate::currentDate().toString("MM") + ".";

        QSqlQuery sql_potni_nalog;
        sql_potni_nalog.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%" + mesec + "%'");
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

}

void wid_osnovni_pogled::napolni_prejete_racune() {

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

        QString mesec = "." + QDate::currentDate().toString("MM") + ".";

        int st_prejetih_racunov = 0;
        double znesek_prejetih_racunov = 0.0;

        QSqlQuery sql_prejeti_racuni;
        sql_prejeti_racuni.prepare("SELECT * FROM prejeti_racuni WHERE datum_prejema LIKE '%" + mesec + "%'");
        sql_prejeti_racuni.exec();
        while ( sql_prejeti_racuni.next() ) {
            st_prejetih_racunov++;

            // znesek + DDV - zelja narocnika
            znesek_prejetih_racunov += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek")).toString()).toDouble();
        }

        ui->txt_prejeti_racuni->setText(QString::number(st_prejetih_racunov, 10));
        ui->txt_znesek_prejetih_racunov->setText(QString::number(znesek_prejetih_racunov, 'f', 2).replace(".", ",") + " EUR");

    }
    base.close();

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

        QString mesec = "." + QDate::currentDate().toString("MM") + ".";

        int st_predracunov = 0;
        double znesek_predracunov = 0.0;

        int st_predplacilnih_racunov = 0;
        double znesek_predplacilnih_racunov = 0.0;

        int st_racunov = 0;
        double znesek_racunov = 0.0;

        QSqlQuery sql_racuni;
//        sql_racuni.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%" + mesec + "%'");
        sql_racuni.prepare("SELECT * FROM racuni");
        sql_racuni.exec();
        while ( sql_racuni.next() ) {

            double znesek = 0.0;

            QSqlQuery sql_opravila;
            sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_racuni.value(sql_racuni.record().indexOf("id")).toString() + "'");
            sql_opravila.exec();
            while ( sql_opravila.next() ) {
                // znesek brez DDV - zelja narocnika
                znesek += prevedi(sql_opravila.value(sql_opravila.record().indexOf("znesek_koncni")).toString()).toDouble();
            }

            // predracuni
            if ( prevedi(sql_racuni.value(sql_racuni.record().indexOf("tip_racuna")).toString()) == "1" ) {
                st_predracunov++;
                znesek_predracunov += znesek;

            }
            // predplacilni racuni
            else if ( prevedi(sql_racuni.value(sql_racuni.record().indexOf("tip_racuna")).toString()) == "2" ) {
                st_predplacilnih_racunov++;
                znesek_predplacilnih_racunov += znesek;

            }
            // racuni
            else if ( prevedi(sql_racuni.value(sql_racuni.record().indexOf("tip_racuna")).toString()) == "3" ) {
                st_racunov++;
                znesek_racunov += znesek;

            }
        }

        ui->txt_predracuni->setText(QString::number(st_predracunov, 10));
        ui->txt_znesek_predracunov->setText(QString::number(znesek_predracunov, 'f', 2).replace(".", ",") + " EUR");

        ui->txt_predplacilni->setText(QString::number(st_predplacilnih_racunov, 10));
        ui->txt_znesek_predplacilnih->setText(QString::number(znesek_predplacilnih_racunov, 'f', 2).replace(".", ",") + " EUR");

        ui->txt_racuni->setText(QString::number(st_racunov, 10));
        ui->txt_znesek_racunov->setText(QString::number(znesek_racunov, 'f', 2).replace(".", ",") + " EUR");

    }
    base.close();

}

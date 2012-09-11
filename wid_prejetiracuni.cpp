#include <QtSql>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>

#include "wid_prejetiracuni.h"
#include "ui_wid_prejetiracuni.h"
#include "prejetiracuni.h"
#include "kodiranje.h"
#include "varnost.h"
#include "tiskanje.h"
#include "datum.h"

wid_prejetiracuni::wid_prejetiracuni(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_prejetiracuni)
{
    ui->setupUi(this);

        // disable and hide
        ui->txt_stprojekta->setEnabled(false);
        ui->txt_stprojekta->setVisible(false);

        // napolni filtrirne spustne sezname
        QString gumb = ui->btn_nov->text();
        ui->btn_nov->setText("");

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

            // filtriraj po mesecu
            ui->cb_mesec->addItem("");
            ui->cb_mesec->addItem("01) Januar");
            ui->cb_mesec->addItem("02) Februar");
            ui->cb_mesec->addItem("03) Marec");
            ui->cb_mesec->addItem("04) April");
            ui->cb_mesec->addItem("05) Maj");
            ui->cb_mesec->addItem("06) Junij");
            ui->cb_mesec->addItem("07) Julij");
            ui->cb_mesec->addItem("08) Avgust");
            ui->cb_mesec->addItem("09) September");
            ui->cb_mesec->addItem("10) Oktober");
            ui->cb_mesec->addItem("11) November");
            ui->cb_mesec->addItem("12) December");

            // filtriraj po letu
            ui->cb_leto->addItem("");
            int min_leto = QDate::currentDate().year();
            int max_leto = QDate::currentDate().year();

            sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE avtor LIKE '" + pretvori(vApp->id()) + "'");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                if ( min_leto > prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt() ) {
                    min_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt();
                }
                if ( max_leto < prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt() ) {
                    max_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt();
                }
            }

            for ( int i = min_leto; i <= max_leto; i++ ) {
                ui->cb_leto->addItem(QString::number(i, 10));
            }
            sql_napolni.clear();

            // filtriraj po izdajatelju racuna
            ui->cb_izdajatelj->addItem("");
            sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE avtor LIKE '" + pretvori(vApp->id()) + "' ORDER BY izdajatelj_kratki ASC");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                if ( ui->cb_izdajatelj->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("izdajatelj_kratki")).toString()).toUpper()) == -1 ) {
                    ui->cb_izdajatelj->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("izdajatelj_kratki")).toString()).toUpper());
                }
            }
            sql_napolni.clear();

            // filtriraj po statusu placila
            ui->cb_placilo->addItem("");
            sql_napolni.prepare("SELECT * FROM sif_status_placila");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                ui->cb_placilo->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString()));
            }
            sql_napolni.clear();

            // filtriraj po statusu racunovodstva
            ui->cb_racunovodstvo->addItem("");
            sql_napolni.prepare("SELECT * FROM sif_status_racunovodstva");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                ui->cb_racunovodstvo->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString()));
            }
            sql_napolni.clear();

        }
        base.close();

        napolni();

        ui->btn_nov->setText(gumb);

}

wid_prejetiracuni::~wid_prejetiracuni()
{
    delete ui;
}

void wid_prejetiracuni::on_cb_mesec_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_prejetiracuni::on_cb_leto_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_prejetiracuni::on_cb_izdajatelj_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_prejetiracuni::on_cb_placilo_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_prejetiracuni::on_cb_racunovodstvo_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_prejetiracuni::napolni() {

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_racuni->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_racuni->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_racuni->horizontalHeader()->sortIndicatorSection();

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

        // clear previous content
        ui->tbl_racuni->clear();

        for (int i = 0; i <= 7; i++) {
            ui->tbl_racuni->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM prejeti_racuni");
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

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;
        QTableWidgetItem *naslov5 = new QTableWidgetItem;
        QTableWidgetItem *naslov6 = new QTableWidgetItem;
        QTableWidgetItem *naslov7 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Stevilka racuna");
        naslov2->setText("Datum prejema");
        naslov3->setText("Izdajatelj");
        naslov4->setText("Rok placila");
        naslov5->setText("Status placila");
        naslov6->setText("Status racunovodstva");
        naslov7->setText("Znesek");

        ui->tbl_racuni->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_racuni->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_racuni->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_racuni->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_racuni->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_racuni->setHorizontalHeaderItem(5, naslov5);
        ui->tbl_racuni->setHorizontalHeaderItem(6, naslov6);
        ui->tbl_racuni->setHorizontalHeaderItem(7, naslov7);

        ui->tbl_racuni->setColumnWidth(0, 35);

        datum *delegate = new datum(this);
        ui->tbl_racuni->setItemDelegateForColumn(2, delegate);
        ui->tbl_racuni->setItemDelegateForColumn(4, delegate);

        QString projekt = "";

        QSqlQuery sql_projekt;
        sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
        sql_projekt.exec();
        if ( sql_projekt.next() ) {
            projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
        }

        QString stavek = "";

        if ( ui->cb_izdajatelj->currentText() != "" ) {
            stavek += " AND izdajatelj_kratki LIKE '" + pretvori(ui->cb_izdajatelj->currentText()) + "'";
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

        QSqlQuery sql_fill;
        if ( ui->txt_stprojekta->text() != "*" ) {
            sql_fill.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_projekta LIKE '" + projekt + "'" + stavek);
        }
        else {
            sql_fill.prepare("SELECT * FROM prejeti_racuni" + stavek);
        }
        sql_fill.exec();

        int row = 0;

        while (sql_fill.next()) {
            // filtriramo glede na mesec in leto
            QString filter = "pozitivno";
            if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
                QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
                QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
                if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
                    filter = "negativno";
                }
            }
            else if ( ui->cb_mesec->currentText() != "" ) {
                QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
                if ( mesec != ui->cb_mesec->currentText().left(2) ) {
                    filter = "negativno";
                }
            }
            else if ( ui->cb_leto->currentText() != "" ) {
                QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
                if ( leto != ui->cb_leto->currentText() ) {
                    filter = "negativno";
                }
            }

            if ( filter == "pozitivno" ) {
                ui->tbl_racuni->insertRow(row);
                ui->tbl_racuni->setRowHeight(row, 20);
                int col = 0;
                int i = 0;
                QString polja[8] = {"id", "stevilka_racuna", "datum_prejema", "izdajatelj_kratki", "rok_placila", "status_placila",
                                                        "status_racunovodstva", "znesek"};

                while (col <= 7) {

                    QTableWidgetItem *celica = new QTableWidgetItem;
                    if ( polja[i] == "id" ) {
                        celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                    }
                    else if ( polja[i] == "datum_prejema" || polja[i] == "rok_placila") {
                        celica->setData(Qt::DisplayRole, QDate::fromString(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()), "dd'.'MM'.'yyyy"));
                    }
                    else if ( polja[i] == "znesek" ) {
                        QString vrednost = prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ",") + " EUR";
                        celica->setText(vrednost);
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

void wid_prejetiracuni::on_btn_brisi_clicked() {

    QString id = ui->tbl_racuni->selectedItems().takeAt(0)->text();

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
        QSqlQuery sql_brisi;
        sql_brisi.prepare("DELETE FROM prejeti_racuni WHERE id LIKE '" + id + "'");
        sql_brisi.exec();
    }
    base.close();

    ui->tbl_racuni->removeRow(ui->tbl_racuni->selectedItems().takeAt(0)->row());
    osvezi("prejetiracuniracuni");

}

void wid_prejetiracuni::on_btn_nov_clicked() {

    prejetiracuni *uredi = new prejetiracuni;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos("Nov racun");
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_prejetiracuni::on_tbl_racuni_doubleClicked() {

    prejetiracuni *uredi = new prejetiracuni;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_racuni->selectedItems().takeAt(0)->text());
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_prejetiracuni::osvezi(QString beseda) {

    if ( beseda == "racun" ) {
        napolni();
    }

}

QString wid_prejetiracuni::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_prejetiracuni::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void wid_prejetiracuni::prejem(QString besedilo) {

    ui->txt_stprojekta->setText(besedilo);

    napolni();

}

void wid_prejetiracuni::on_btn_refresh_clicked() {

    napolni();

}

void wid_prejetiracuni::on_btn_print_clicked() {

    QString seznam_id = "";
    QModelIndexList selectedList = ui->tbl_racuni->selectionModel()->selectedRows();

    for( int i = 0; i < selectedList.count(); i++) {
        seznam_id.append(ui->tbl_racuni->item(selectedList.at(i).row(), 0)->text() + ",");
    }

    // posljemo v tisk
    tiskanje *print = new tiskanje;
    print->show();
    QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
                 print , SLOT(prejem(QString, QString, QString)));
    tiskaj("prejeti-racuni", seznam_id, "tisk"); // vrsta, id stevilke, format

}

void wid_prejetiracuni::on_btn_print_pdf_clicked() {

    QString seznam_id = "";
    QModelIndexList selectedList = ui->tbl_racuni->selectionModel()->selectedRows();

    for( int i = 0; i < selectedList.count(); i++) {
        seznam_id.append(ui->tbl_racuni->item(selectedList.at(i).row(), 0)->text() + ",");
    }

    // posljemo v tisk
    tiskanje *print = new tiskanje;
    print->show();
    QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
                 print , SLOT(prejem(QString, QString, QString)));
    tiskaj("prejeti-racuni", seznam_id, "pdf"); // vrsta, id stevilke, format

}

void wid_prejetiracuni::on_btn_print_seznam_clicked() {

}

void wid_prejetiracuni::on_btn_prestevilci_clicked() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prestevilci-zapise");
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

        // poisci vsa leta; dobimo seznam vseh let, v katerih smo uradovali
        QStringList leta;

        QSqlQuery sql_leta;
        sql_leta.prepare("SELECT * FROM prejeti_racuni");
        sql_leta.exec();
        while ( sql_leta.next() ) {
            if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_prejema")).toString()).right(4)) ) {
                leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_prejema")).toString()).right(4));

            }
        }

        // razvrsti leta narascajoce
        leta.sort();

        // za vsako leto poisci mesece, dneve in id-je ter jih zapisi v nov seznam
        for ( int i_leta = 0; i_leta < leta.count(); i_leta++ ) {

            // ustvari nov seznam id-jev, mesecev in zaporednih stevilk
            QStringList seznam_vnosov;
            QStringList meseci;
            int zaporedna_stevilka = 0;

            // poisci vse mesece, dobimo seznam mesecev v danem letu
            QSqlQuery sql_meseci;
            sql_meseci.prepare("SELECT * FROM prejeti_racuni WHERE datum_prejema LIKE '%." + leta.value(i_leta) + "'");
            sql_meseci.exec();
            while ( sql_meseci.next() ) {
                if ( !meseci.contains(prevedi(sql_meseci.value(sql_meseci.record().indexOf("datum_prejema")).toString()).right(7).left(2)) ) {
                    meseci.append(prevedi(sql_meseci.value(sql_meseci.record().indexOf("datum_prejema")).toString()).right(7).left(2));
                }
            }

            // razvrsti po mesecu
            meseci.sort();

            // za vsak mesec poiscemo dneve, idje ter jih zapisemo v nov seznam
            for ( int i_meseci = 0; i_meseci < meseci.count(); i_meseci++ ) {

                // ustvarimo nov seznam dnevov
                QStringList dnevi;

                // poisci vse dneve, dobimo seznam dnevov v danem letu in danem mesecu
                QSqlQuery sql_dnevi;
                sql_dnevi.prepare("SELECT * FROM prejeti_racuni WHERE datum_prejema LIKE '%." + meseci.value(i_meseci) + "." + leta.value(i_leta) + "'");
                sql_dnevi.exec();
                while ( sql_dnevi.next() ) {
                    if ( !dnevi.contains(prevedi(sql_dnevi.value(sql_dnevi.record().indexOf("datum_prejema")).toString()).left(2)) ) {
                        dnevi.append(prevedi(sql_dnevi.value(sql_dnevi.record().indexOf("datum_prejema")).toString()).left(2));
                    }
                }

                // razvrsti po dnevih
                dnevi.sort();

                // za vsak dan poisci id-je ter jih zapisi v nov seznam
                for ( int i_dnevi = 0; i_dnevi < dnevi.count(); i_dnevi++ ) {

                    // poisci vse id-je, dobimo seznam id-jev v danem letu in danem mesecu in danem dnevu
                    QSqlQuery sql_id_ji;
                    sql_id_ji.prepare("SELECT * FROM prejeti_racuni WHERE datum_prejema LIKE '" + dnevi.value(i_dnevi) + "." + meseci.value(i_meseci) + "." + leta.value(i_leta) + "'");
                    sql_id_ji.exec();
                    while ( sql_id_ji.next() ) {
                        if ( !seznam_vnosov.contains(prevedi(sql_id_ji.value(sql_id_ji.record().indexOf("id")).toString())) ) {
                            seznam_vnosov.append(prevedi(sql_id_ji.value(sql_id_ji.record().indexOf("id")).toString()));
                        }
                    }

                    sql_id_ji.clear();

                } // for ( int i_dnevi = 0; i_dnevi < dnevi.count(); i_dnevi++ )

                dnevi.clear();
                sql_dnevi.clear();

            } // for ( int i_meseci = 0; i_meseci < meseci.count(); i_meseci++ )

            // pojdi cez cel seznam vnosov (notri so IDji po vrstnem redu) in vsakega izpisi ( kasneje popravi )
            for ( int i_seznam_vnosov = 0; i_seznam_vnosov < seznam_vnosov.count(); i_seznam_vnosov++ ) {

                zaporedna_stevilka++;
                QString zaporedna = "";
                if ( zaporedna_stevilka < 10 ) {
                    zaporedna = "00" + QString::number(zaporedna_stevilka, 10);
                }
                else if ( zaporedna_stevilka < 100 ) {
                    zaporedna = "0" + QString::number(zaporedna_stevilka, 10);
                }
                else {
                    zaporedna = "" + QString::number(zaporedna_stevilka, 10);
                }

                QSqlQuery sql_prestevilci;
                sql_prestevilci.prepare("UPDATE prejeti_racuni SET stevilka_vnosa = ? WHERE id LIKE '" + pretvori(seznam_vnosov.value(i_seznam_vnosov)) + "'");
                sql_prestevilci.bindValue(0, pretvori("PR-" + leta.value(i_leta) + "-" + zaporedna));
                sql_prestevilci.exec();

            } // for ( int i_seznam_vnosov = 0; i_seznam_vnosov < seznam_vnosov.count(); i_seznam_vnosov++ )

            meseci.clear();
            sql_meseci.clear();
            seznam_vnosov.clear();

        } // for ( int i_leta = 0; i_leta < leta.count(); i_leta++ )

        leta.clear();
        sql_leta.clear();

    }
    base.close();

    // sporocilo ob zakljucku prestevilcevanja
    QMessageBox zakljucek;
    zakljucek.setText("Prestevilcenje zakljuceno");
    zakljucek.setInformativeText("Prestevilcenje zahtevanih vnosov je bilo uspesno zakljuceno.");
    zakljucek.exec();

    napolni();

}

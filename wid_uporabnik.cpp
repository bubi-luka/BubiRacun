#include <QtSql>
#include <QMessageBox>

#include "wid_uporabnik.h"
#include "ui_wid_uporabnik.h"
#include "uporabnik.h"
#include "kodiranje.h"
#include "varnost.h"
#include "glavnookno.h"

wid_uporabnik::wid_uporabnik(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_uporabnik)
{
    ui->setupUi(this);
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

            // zaradi nepotrebnih filtriranj tabele spremenimo napis na enem od gumbov
            QString besedilo = ui->btn_nov->text();
            ui->btn_nov->setText("");

            QSqlQuery sql_napolni;

            ui->cb_pogodba->addItem("");
            sql_napolni.prepare("SELECT * FROM sif_pogodbe");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                ui->cb_pogodba->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()) + ") " +
                                                                prevedi(sql_napolni.value(sql_napolni.record().indexOf("pogodba")).toString()));
            }
            sql_napolni.clear();

            ui->cb_mesto->addItem("");
            sql_napolni.prepare("SELECT * FROM sif_naziv");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                ui->cb_mesto->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()) + ") " +
                                                                prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()));
            }
            sql_napolni.clear();

            ui->btn_nov->setText(besedilo);
        }
        base.close();

    napolni();

}

wid_uporabnik::~wid_uporabnik()
{
    delete ui;
}

void wid_uporabnik::on_cb_mesto_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_uporabnik::on_cb_pogodba_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_uporabnik::napolni() {

    QString stavek = "";

    if ( ui->cb_pogodba->currentText() != "" ) {
        stavek += " AND pogodba LIKE '" + pretvori(ui->cb_pogodba->currentText().left(ui->cb_pogodba->currentText().indexOf(") ", 0))) + "'";
    }
    if ( ui->cb_mesto->currentText() != "" ) {
        stavek += " AND naziv LIKE '" + pretvori(ui->cb_mesto->currentText().left(ui->cb_mesto->currentText().indexOf(") ", 0))) + "'";
    }

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
        ui->tbl_uporabnik->clear();

        for (int i = 0; i <= 8; i++) {
            ui->tbl_uporabnik->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM uporabniki");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_uporabnik->removeRow(0);
        }

        // start filling the table
        ui->tbl_uporabnik->insertColumn(0);
        ui->tbl_uporabnik->insertColumn(1);
        ui->tbl_uporabnik->insertColumn(2);
        ui->tbl_uporabnik->insertColumn(3);
        ui->tbl_uporabnik->insertColumn(4);
        ui->tbl_uporabnik->insertColumn(5);
        ui->tbl_uporabnik->insertColumn(6);
        ui->tbl_uporabnik->insertColumn(7);
        ui->tbl_uporabnik->insertColumn(8);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;
        QTableWidgetItem *naslov5 = new QTableWidgetItem;
        QTableWidgetItem *naslov6 = new QTableWidgetItem;
        QTableWidgetItem *naslov7 = new QTableWidgetItem;
        QTableWidgetItem *naslov8 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Ime");
        naslov2->setText("Priimek");
        naslov3->setText("Up. ime");
        naslov4->setText("Telefon");
        naslov5->setText("GSM");
        naslov6->setText("E-mail");
        naslov7->setText("Tip pogodbe");
        naslov8->setText("Delavno mesto");

        ui->tbl_uporabnik->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_uporabnik->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_uporabnik->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_uporabnik->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_uporabnik->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_uporabnik->setHorizontalHeaderItem(5, naslov5);
        ui->tbl_uporabnik->setHorizontalHeaderItem(6, naslov6);
        ui->tbl_uporabnik->setHorizontalHeaderItem(7, naslov7);
        ui->tbl_uporabnik->setHorizontalHeaderItem(8, naslov8);
        ui->tbl_uporabnik->setColumnWidth(0, 35);
        ui->tbl_uporabnik->setColumnWidth(1, 60);
        ui->tbl_uporabnik->setColumnWidth(2, 60);
        ui->tbl_uporabnik->setColumnWidth(3, 60);
        ui->tbl_uporabnik->setColumnWidth(4, 140);
        ui->tbl_uporabnik->setColumnWidth(5, 140);
        ui->tbl_uporabnik->setColumnWidth(6, 100);
        ui->tbl_uporabnik->setColumnWidth(7, 150);
        ui->tbl_uporabnik->setColumnWidth(8, 150);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM uporabniki WHERE podjetje LIKE '" + pretvori(vApp->firm()) + "'" + stavek);
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_uporabnik->insertRow(row);
            ui->tbl_uporabnik->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[9] = {"id", "ime", "priimek", "user_name", "telefon", "gsm", "email", "pogodba", "naziv"};

            while (col <= 8) {

                QTableWidgetItem *celica = new QTableWidgetItem;
                if ( polja[i] == "id" ) {
                    celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                }
                else if ( polja[i] == "pogodba" ) {
                    QSqlQuery sql_besedilo;
                    sql_besedilo.prepare("SELECT * FROM sif_pogodbe WHERE id LIKE '" + pretvori(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) + "'");
                    sql_besedilo.exec();
                    if ( sql_besedilo.next() ) {
                        celica->setText(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf(polja[i])).toString()));
                    }
                }
                else if ( polja[i] == "naziv" ) {
                    QSqlQuery sql_besedilo;
                    sql_besedilo.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + pretvori(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) + "'");
                    sql_besedilo.exec();
                    if ( sql_besedilo.next() ) {
                        celica->setText(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf(polja[i])).toString()));
                    }
                }
                else {
                    celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                }
                ui->tbl_uporabnik->setItem(row, col, celica);

                col++;
                i++;

            }

            row++;

        }
    }
    base.close();

}

void wid_uporabnik::on_tbl_uporabnik_doubleClicked() {

    uporabnik *uredi = new uporabnik;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_uporabnik->selectedItems().takeAt(0)->text());
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}


void wid_uporabnik::on_btn_brisi_clicked() {

    QString id = ui->tbl_uporabnik->selectedItems().takeAt(0)->text();

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
        sql_brisi.prepare("DELETE FROM uporabniki WHERE id LIKE '" + id + "'");
        sql_brisi.exec();
    }
    base.close();

    ui->tbl_uporabnik->removeRow(ui->tbl_uporabnik->selectedItems().takeAt(0)->row());
    osvezi("brisi");

}

void wid_uporabnik::osvezi(QString beseda) {

 //   if ( beseda == "uporabnik" ) {
        napolni();
 //   }
 //   else if ( beseda == "brisi" ) {
 //       napolni();
 //   }

}

QString wid_uporabnik::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_uporabnik::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void wid_uporabnik::on_btn_nov_clicked() {

    uporabnik *uredi = new uporabnik;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos("Nov zaposleni");
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"
#include "avtomobili.h"
#include "wid_avtomobili.h"
#include "ui_wid_avtomobili.h"

wid_avtomobili::wid_avtomobili(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_avtomobili)
{
    ui->setupUi(this);

    napolni();

}

wid_avtomobili::~wid_avtomobili()
{
    delete ui;
}

void wid_avtomobili::on_btn_brisi_clicked() {

    QString id = ui->tbl_avtomobili->selectedItems().takeAt(0)->text();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "brisi-avtomobil");
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
        sql_brisi.prepare("DELETE FROM avtomobili WHERE id LIKE '" + id + "'");
        sql_brisi.exec();
    }
    base.close();

    ui->tbl_avtomobili->removeRow(ui->tbl_avtomobili->selectedItems().takeAt(0)->row());
    osvezi("brisi");

}

void wid_avtomobili::on_btn_nov_clicked() {

    avtomobili *uredi = new avtomobili;
    uredi->show();

    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos("Nov");
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_avtomobili::on_btn_refresh_clicked() {

    napolni();

}

void wid_avtomobili::osvezi(QString beseda) {

    napolni();

}

void wid_avtomobili::on_tbl_avtomobili_doubleClicked() {

    avtomobili *uredi = new avtomobili;
    uredi->show();

    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_avtomobili->selectedItems().takeAt(0)->text());
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_avtomobili::napolni() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "avto-napolni");
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
        ui->tbl_avtomobili->clear();

        for (int i = 0; i <= 5; i++) {
            ui->tbl_avtomobili->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM avtomobili");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_avtomobili->removeRow(0);
        }

        // start filling the table
        ui->tbl_avtomobili->insertColumn(0);
        ui->tbl_avtomobili->insertColumn(1);
        ui->tbl_avtomobili->insertColumn(2);
        ui->tbl_avtomobili->insertColumn(3);
        ui->tbl_avtomobili->insertColumn(4);
        ui->tbl_avtomobili->insertColumn(5);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;
        QTableWidgetItem *naslov5 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Proizvajalec");
        naslov2->setText("Znamka");
        naslov3->setText("Model");
        naslov4->setText("Registrska st.");
        naslov5->setText("Lastnik");

        ui->tbl_avtomobili->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_avtomobili->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_avtomobili->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_avtomobili->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_avtomobili->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_avtomobili->setHorizontalHeaderItem(5, naslov5);

        ui->tbl_avtomobili->setColumnWidth(0, 35);
        ui->tbl_avtomobili->setColumnWidth(1, 100);
        ui->tbl_avtomobili->setColumnWidth(2, 100);
        ui->tbl_avtomobili->setColumnWidth(3, 100);
        ui->tbl_avtomobili->setColumnWidth(4, 100);
        ui->tbl_avtomobili->setColumnWidth(5, 100);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM avtomobili");
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_avtomobili->insertRow(row);
            ui->tbl_avtomobili->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[6] = {"id", "proizvajalec", "znamka", "tip", "registrska_stevilka", "lastnik"};

            while (col <= 5) {

                QTableWidgetItem *celica = new QTableWidgetItem;
                if ( polja[i] == "id" ) {
                    celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                }
                else if ( polja[i] == "lastnik" ) {
                    QSqlQuery sql_lastnik;
                    if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "1" ) { // zasebnik
                        sql_lastnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("lastnik")).toString() + "'");
                    }
                    else if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "0" ) { // podjetje
                        sql_lastnik.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("lastnik")).toString() + "'");
                    }
                    sql_lastnik.exec();
                    if ( sql_lastnik.next() ) {
                        if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "1" ) { // zasebnik
                            celica->setText(prevedi(sql_lastnik.value(sql_lastnik.record().indexOf("ime")).toString()) + " " +
                                            prevedi(sql_lastnik.value(sql_lastnik.record().indexOf("priimek")).toString()));
                        }
                        else if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "0" ) { // podjetje
                            celica->setText(prevedi(sql_lastnik.value(sql_lastnik.record().indexOf("ime")).toString()));
                        }
                    }
                }
                else {
                    celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                }
                ui->tbl_avtomobili->setItem(row, col, celica);

                col++;
                i++;

            }

            row++;

        }
    }
    base.close();

}


QString wid_avtomobili::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_avtomobili::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

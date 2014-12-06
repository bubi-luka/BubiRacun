#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"
#include "varnost.h"
#include "opombepriracunih.h"
#include "wid_opombepriracunih.h"
#include "ui_wid_opombepriracunih.h"

wid_opombepriracunih::wid_opombepriracunih(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_opombepriracunih)
{
    ui->setupUi(this);

        napolni();

}

wid_opombepriracunih::~wid_opombepriracunih()
{
    delete ui;
}

void wid_opombepriracunih::napolni() {

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_opombe->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_opombe->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_opombe->horizontalHeader()->sortIndicatorSection();

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
        ui->tbl_opombe->clear();

        for (int i = 0; i <= 2; i++) {
            ui->tbl_opombe->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM sif_opombe_pri_racunih");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_opombe->removeRow(0);
        }

        // start filling the table
        ui->tbl_opombe->insertColumn(0);
        ui->tbl_opombe->insertColumn(1);
        ui->tbl_opombe->insertColumn(2);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Naslov");
        naslov2->setText("Besedilo");

        ui->tbl_opombe->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_opombe->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_opombe->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_opombe->setColumnWidth(0, 20);
        ui->tbl_opombe->setColumnWidth(1, 100);
        ui->tbl_opombe->setColumnWidth(2, 400);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_opombe_pri_racunih");
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_opombe->insertRow(row);
            ui->tbl_opombe->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[3] = {"id", "naslov", "besedilo"};

            while (col <= 2) {

                QTableWidgetItem *celica = new QTableWidgetItem;
                celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                ui->tbl_opombe->setItem(row, col, celica);

                col++;
                i++;

            }

            row++;

        }
    }
    base.close();

    ui->tbl_opombe->selectRow(izbranec);
    ui->tbl_opombe->sortByColumn(razvrsti, Qt::AscendingOrder);

}

void wid_opombepriracunih::on_tbl_opombe_doubleClicked() {

    opombepriracunih *uredi = new opombepriracunih;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
                 uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_opombe->selectedItems().takeAt(0)->text());
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
                 this , SLOT(osvezi(QString)));

}

void wid_opombepriracunih::on_btn_brisi_clicked() {

    QString id = ui->tbl_opombe->selectedItems().takeAt(0)->text();

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
        sql_brisi.prepare("DELETE FROM sif_opombe_pri_racunih WHERE id LIKE '" + id + "'");
        sql_brisi.exec();
    }
    base.close();

    ui->tbl_opombe->removeRow(ui->tbl_opombe->selectedItems().takeAt(0)->row());
    osvezi("opombe");

}

void wid_opombepriracunih::osvezi(QString beseda) {

    if ( beseda == "opombe" ) {
        napolni();
    }

}

QString wid_opombepriracunih::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_opombepriracunih::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void wid_opombepriracunih::on_btn_nov_clicked() {

    opombepriracunih *uredi = new opombepriracunih;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
                 uredi , SLOT(prejem(QString)));
    prenos("Nova opomba");
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
                 this , SLOT(osvezi(QString)));

}

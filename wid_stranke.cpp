#include <QtSql>
#include <QMessageBox>

#include "wid_stranke.h"
#include "ui_wid_stranke.h"
#include "stranke.h"
#include "kodiranje.h"

wid_stranke::wid_stranke(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_stranke)
{
    ui->setupUi(this);

	napolni();

}

wid_stranke::~wid_stranke()
{
    delete ui;
}


void wid_stranke::napolni() {

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
		ui->tbl_stranke->clear();

		for (int i = 0; i <= 7; i++) {
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
		ui->tbl_stranke->insertColumn(3);
		ui->tbl_stranke->insertColumn(4);
		ui->tbl_stranke->insertColumn(5);
		ui->tbl_stranke->insertColumn(6);
		ui->tbl_stranke->insertColumn(7);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;
		QTableWidgetItem *naslov6 = new QTableWidgetItem;
		QTableWidgetItem *naslov7 = new QTableWidgetItem;
		naslov0->setText("ID");
		naslov1->setText("Ime/Naziv");
		naslov2->setText("Priimek/Polni naziv");
		naslov3->setText("Telefon");
		naslov4->setText("GSM");
		naslov5->setText("E-mail");
		naslov6->setText("Izobr. ustanova");
		naslov7->setText("Fizicna/Pravna");

		ui->tbl_stranke->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_stranke->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_stranke->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_stranke->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_stranke->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_stranke->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_stranke->setHorizontalHeaderItem(6, naslov6);
		ui->tbl_stranke->setHorizontalHeaderItem(7, naslov7);

		ui->tbl_stranke->setColumnWidth(0, 20);
		ui->tbl_stranke->setColumnWidth(1, 80);
		ui->tbl_stranke->setColumnWidth(2, 120);
		ui->tbl_stranke->setColumnWidth(3, 140);
		ui->tbl_stranke->setColumnWidth(4, 140);
		ui->tbl_stranke->setColumnWidth(5, 100);
		ui->tbl_stranke->setColumnWidth(6, 100);
		ui->tbl_stranke->setColumnWidth(7, 100);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM stranke");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_stranke->insertRow(row);
			ui->tbl_stranke->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[8] = {"id", "ime", "priimek", "telefon", "gsm", "email", "ustanova", "tip"};

			while (col <= 7) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				ui->tbl_stranke->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}
	}
	base.close();

}

void wid_stranke::on_tbl_stranke_doubleClicked() {

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

void wid_stranke::on_btn_brisi_clicked() {

	QString id = ui->tbl_stranke->selectedItems().takeAt(0)->text();

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
		sql_brisi.prepare("DELETE FROM stranke WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_stranke->removeRow(ui->tbl_stranke->selectedItems().takeAt(0)->row());
	osvezi("stranke");

}

void wid_stranke::on_btn_nov_clicked() {

	stranke *uredi = new stranke;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nova stranka");
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_stranke::osvezi(QString beseda) {

	if ( beseda == "stranke" ) {
		napolni();
	}

}

QString wid_stranke::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_stranke::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

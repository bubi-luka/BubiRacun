#include <QtSql>
#include <QMessageBox>

#include "wid_podjetje.h"
#include "ui_wid_podjetje.h"
#include "podjetje.h"
#include "kodiranje.h"
#include "podjetje.h"

wid_podjetje::wid_podjetje(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_podjetje)
{
    ui->setupUi(this);

	napolni();

}

wid_podjetje::~wid_podjetje()
{
    delete ui;
}

void wid_podjetje::napolni() {

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
		ui->tbl_podjetje->clear();

		for (int i = 0; i <= 6; i++) {
			ui->tbl_podjetje->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM podjetje");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_podjetje->removeRow(0);
		}

		// start filling the table
		ui->tbl_podjetje->insertColumn(0);
		ui->tbl_podjetje->insertColumn(1);
		ui->tbl_podjetje->insertColumn(2);
		ui->tbl_podjetje->insertColumn(3);
		ui->tbl_podjetje->insertColumn(4);
		ui->tbl_podjetje->insertColumn(5);
		ui->tbl_podjetje->insertColumn(6);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;
		QTableWidgetItem *naslov6 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Naziv");
		naslov2->setText("Polni naziv");
		naslov3->setText("Telefon");
		naslov4->setText("GSM");
		naslov5->setText("Email");
		naslov6->setText("Davcna st.");

		ui->tbl_podjetje->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_podjetje->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_podjetje->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_podjetje->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_podjetje->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_podjetje->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_podjetje->setHorizontalHeaderItem(6, naslov6);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM podjetje");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_podjetje->insertRow(row);
			ui->tbl_podjetje->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[7] = {"id", "ime", "polnoime", "telefon", "gsm", "email", "davcnastevilka"};

			while (col <= 6) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				ui->tbl_podjetje->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}
	}
	base.close();

}

void wid_podjetje::on_tbl_podjetje_doubleClicked() {

	podjetje *uredi = new podjetje;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_podjetje->selectedItems().takeAt(0)->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_podjetje::on_btn_brisi_clicked() {

	QString id = ui->tbl_podjetje->selectedItems().takeAt(0)->text();

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
		sql_brisi.prepare("DELETE FROM podjetje WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_podjetje->removeRow(ui->tbl_podjetje->selectedItems().takeAt(0)->row());
	osvezi("podjetje");

}

void wid_podjetje::on_btn_nov_clicked() {

	podjetje *oknopodjetja = new podjetje;
	oknopodjetja->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   oknopodjetja , SLOT(prejem(QString)));
	prenos("Novo podjetje");

}

void wid_podjetje::osvezi(QString beseda) {

	if ( beseda == "podjetje" ) {
		napolni();
	}

}

QString wid_podjetje::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_podjetje::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

#include <QtSql>
#include <QMessageBox>

#include "wid_potninalogi.h"
#include "ui_wid_potninalogi.h"
#include "potninalogi.h"
#include "kodiranje.h"

wid_potninalogi::wid_potninalogi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_potninalogi)
{
    ui->setupUi(this);

	napolni();

}

wid_potninalogi::~wid_potninalogi()
{
    delete ui;
}

void wid_potninalogi::napolni() {

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
		ui->tbl_potninalogi->clear();

		for (int i = 0; i <= 3; i++) {
			ui->tbl_potninalogi->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM potninalogi");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_potninalogi->removeRow(0);
		}

		// start filling the table
		ui->tbl_potninalogi->insertColumn(0);
		ui->tbl_potninalogi->insertColumn(1);
		ui->tbl_potninalogi->insertColumn(2);
		ui->tbl_potninalogi->insertColumn(3);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		naslov0->setText("ID");
		naslov1->setText("Podjetje");
		naslov2->setText("St.  naloga");
		naslov3->setText("Datum");
		ui->tbl_potninalogi->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_potninalogi->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_potninalogi->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_potninalogi->setHorizontalHeaderItem(3, naslov3);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM potninalogi");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_potninalogi->insertRow(row);
			ui->tbl_potninalogi->setRowHeight(row, 20);
			int col = 0;
			while (col <= 3) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				celica->setText(prevedi(sql_fill.value(col).toString()));
				ui->tbl_potninalogi->setItem(row, col, celica);

				col++;

			}

			row++;

		}
	}
	base.close();

}

void wid_potninalogi::on_tbl_potninalogi_doubleClicked() {

	potninalogi *uredi = new potninalogi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_potninalogi->selectedItems().takeAt(0)->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_potninalogi::on_btn_brisi_clicked() {

	QString id = ui->tbl_potninalogi->selectedItems().takeAt(0)->text();
	QString stnaloga = ui->tbl_potninalogi->selectedItems().takeAt(2)->text();

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
		sql_brisi.prepare("DELETE FROM pot WHERE potninalog LIKE '" + pretvori(stnaloga) + "'");
		sql_brisi.exec();
		sql_brisi.clear();

		sql_brisi.prepare("DELETE FROM stroski WHERE potninalog LIKE '" + pretvori(stnaloga) + "'");
		sql_brisi.exec();
		sql_brisi.clear();

		sql_brisi.prepare("DELETE FROM potninalogi WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_potninalogi->removeRow(ui->tbl_potninalogi->selectedItems().takeAt(0)->row());
	osvezi("potninalogi");

}

void wid_potninalogi::on_btn_nov_clicked() {

	potninalogi *uredi = new potninalogi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov nalog");
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_potninalogi::osvezi(QString beseda) {

	if ( beseda == "potninalog" ) {
		napolni();
	}

}

QString wid_potninalogi::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_potninalogi::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

#include <QtSql>
#include <QMessageBox>

#include "wid_stroski.h"
#include "ui_wid_stroski.h"
#include "stroski.h"
#include "kodiranje.h"
#include "potninalogi.h"

wid_stroski::wid_stroski(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_stroski)
{
	ui->setupUi(this);

	// disable and hide
	ui->txt_stnaloga->setEnabled(false);
	ui->txt_stnaloga->setVisible(false);

	napolni();
}

wid_stroski::~wid_stroski()
{
	delete ui;
}

void wid_stroski::napolni() {

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
		ui->tbl_stroski->clear();

		for (int i = 0; i <= 2; i++) {
			ui->tbl_stroski->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM stroski");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_stroski->removeRow(0);
		}

		// start filling the table
		ui->tbl_stroski->insertColumn(0);
		ui->tbl_stroski->insertColumn(1);
		ui->tbl_stroski->insertColumn(2);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Opis stroska");
		naslov2->setText("Cena z DDV");

		ui->tbl_stroski->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_stroski->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_stroski->setHorizontalHeaderItem(2, naslov2);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(ui->txt_stnaloga->text()) + "'");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_stroski->insertRow(row);
			ui->tbl_stroski->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[8] = {"id", "strosek", "cena"};

			while (col <= 2) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "id" ) {
					celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
				}
				else if ( polja[i] == "cena" ) {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ",") + " EUR");
				}
				else {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				}
				ui->tbl_stroski->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}
	}
	base.close();

}

void wid_stroski::on_tbl_stroski_doubleClicked() {

	stroski *uredi = new stroski;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_stroski->selectedItems().takeAt(0)->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_stroski::on_btn_brisi_clicked() {

	QString id = ui->tbl_stroski->selectedItems().takeAt(0)->text();

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
		sql_brisi.prepare("DELETE FROM stroski WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_stroski->removeRow(ui->tbl_stroski->selectedItems().takeAt(0)->row());
	osvezi("stroski");

}

void wid_stroski::osvezi(QString beseda) {

	if ( beseda == "stroski" ) {
		napolni();
		prenesi();
	}

}

QString wid_stroski::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_stroski::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void wid_stroski::on_btn_nov_clicked() {

	stroski *uredi = new stroski;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov strosek" + ui->txt_stnaloga->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_stroski::prejem(QString besedilo) {

	ui->txt_stnaloga->setText(besedilo);

	napolni();

}

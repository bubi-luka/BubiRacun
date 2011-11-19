#include <QtSql>
#include <QMessageBox>

#include "wid_projekti.h"
#include "ui_wid_projekti.h"
#include "projekti.h"
#include "kodiranje.h"

wid_projekti::wid_projekti(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_projekti)
{
    ui->setupUi(this);

	napolni();
}

wid_projekti::~wid_projekti()
{
    delete ui;
}

void wid_projekti::napolni() {

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
		ui->tbl_projekti->clear();

		for (int i = 0; i <= 8; i++) {
			ui->tbl_projekti->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM projekti");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_projekti->removeRow(0);
		}

		// start filling the table
		ui->tbl_projekti->insertColumn(0);
		ui->tbl_projekti->insertColumn(1);
		ui->tbl_projekti->insertColumn(2);
		ui->tbl_projekti->insertColumn(3);
		ui->tbl_projekti->insertColumn(4);
		ui->tbl_projekti->insertColumn(5);
		ui->tbl_projekti->insertColumn(6);
		ui->tbl_projekti->insertColumn(7);
		ui->tbl_projekti->insertColumn(8);

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
		naslov1->setText("St. projekta");
		naslov2->setText("Stranka");
		naslov3->setText("Projekt");
		naslov4->setText("Datum pricetka");
		naslov5->setText("Datum konca");
		naslov6->setText("Placilo");
		naslov7->setText("Se placati");
		naslov8->setText("Status placila");

		ui->tbl_projekti->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_projekti->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_projekti->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_projekti->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_projekti->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_projekti->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_projekti->setHorizontalHeaderItem(6, naslov6);
		ui->tbl_projekti->setHorizontalHeaderItem(7, naslov7);
		ui->tbl_projekti->setHorizontalHeaderItem(8, naslov8);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM projekti");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_projekti->insertRow(row);
			ui->tbl_projekti->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[9] = {"id", "stprojekta", "stranka", "naziv", "pricetek", "konec", "znesek", "seplacati", "statusplacila"};

			while (col <= 8) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "stranka" ) {
					QSqlQuery sql_stranka;
					sql_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
					sql_stranka.exec();
					if ( sql_stranka.next() ) {
						if ( prevedi(sql_stranka.value(sql_stranka.record().indexOf("tip")).toString()) == "fizicna" ) {
							celica->setText(prevedi(sql_stranka.value(sql_stranka.record().indexOf("ime")).toString()) + " " +
											prevedi(sql_stranka.value(sql_stranka.record().indexOf("priimek")).toString()));
						}
						else {
							celica->setText(prevedi(sql_stranka.value(sql_stranka.record().indexOf("ime")).toString()));
						}
					}
				}
				else {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				}
				ui->tbl_projekti->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}
	}
	base.close();

}

void wid_projekti::on_tbl_projekti_doubleClicked() {

	projekti *uredi = new projekti;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_projekti->selectedItems().takeAt(1)->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_projekti::on_btn_brisi_clicked() {

	QString id = ui->tbl_projekti->selectedItems().takeAt(0)->text();

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
		sql_brisi.prepare("DELETE FROM projekti WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_projekti->removeRow(ui->tbl_projekti->selectedItems().takeAt(0)->row());
	osvezi("projekti");

}

void wid_projekti::on_btn_nov_clicked() {

	projekti *uredi = new projekti;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov projekt");
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_projekti::osvezi(QString beseda) {

	if ( beseda == "projekti" ) {
		napolni();
	}

}

QString wid_projekti::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_projekti::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

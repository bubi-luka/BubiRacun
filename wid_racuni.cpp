#include <QtSql>
#include <QMessageBox>

#include "wid_racuni.h"
#include "ui_wid_racuni.h"
#include "racun.h"
#include "kodiranje.h"
#include "potninalogi.h"

wid_racuni::wid_racuni(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_racuni)
{
    ui->setupUi(this);

	// disable and hide
	ui->txt_stprojekta->setEnabled(false);
	ui->txt_stprojekta->setVisible(false);

	napolni();
}

wid_racuni::~wid_racuni()
{
	delete ui;
}

void wid_racuni::napolni() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "wid_racuni");
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

		for (int i = 0; i <= 8; i++) {
			ui->tbl_racuni->removeColumn(0);
		}

		QSqlQuery sql_clear("wid_racuni");
		sql_clear.prepare("SELECT * FROM racuni");
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
		ui->tbl_racuni->insertColumn(8);

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
		naslov1->setText("Stev. racuna");
		naslov2->setText("Stev. projekta");
		naslov3->setText("Stranka");
		naslov4->setText("Pricetek");
		naslov5->setText("Konec");
		naslov6->setText("Stat. placila");
		naslov7->setText("Stat. racunovodstva");
		naslov8->setText("Tip racuna");

		ui->tbl_racuni->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_racuni->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_racuni->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_racuni->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_racuni->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_racuni->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_racuni->setHorizontalHeaderItem(6, naslov6);
		ui->tbl_racuni->setHorizontalHeaderItem(7, naslov7);
		ui->tbl_racuni->setHorizontalHeaderItem(8, naslov8);

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE stprojekta LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM racuni WHERE stprojekta LIKE '" + projekt + "'");
		}
		else {
			sql_fill.prepare("SELECT * FROM racuni");
		}
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_racuni->insertRow(row);
			ui->tbl_racuni->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[9] = {"id", "stracuna", "stprojekta", "stranka", "datumpricetka", "datumkonca", "statusplacila", "statusracunovodstva", "tipracuna"};

			while (col <= 8) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "stranka" ) {
					QSqlQuery sql_find_stranka;
					sql_find_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
					sql_find_stranka.exec();
					if ( sql_find_stranka.next() ) {
						if ( prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("tip")).toString()) == "fizicna" ) {
							celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()) + " " +
											prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("priimek")).toString()));
						}
						else {
							 celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
						}
					}
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
	base.close();

}

void wid_racuni::on_tbl_racuni_doubleClicked() {

	racun *uredi = new racun;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_racuni->selectedItems().takeAt(0)->text()); // ce racun ze obstaja, naprej posljemo id. racuna
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_racuni::on_btn_brisi_clicked() {

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
		sql_brisi.prepare("DELETE FROM racuni WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_racuni->removeRow(ui->tbl_racuni->selectedItems().takeAt(0)->row());
	osvezi("racuni");

}

void wid_racuni::osvezi(QString beseda) {

	if ( beseda == "racuni" ) {
		napolni();
		prenesi();
	}

}

QString wid_racuni::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_racuni::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void wid_racuni::on_btn_nov_clicked() {

	racun *uredi = new racun;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov racun" + ui->txt_stprojekta->text()); // ce racun ne obstaja, naprej posljemo st. projekta
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_racuni::prejem(QString besedilo) {

	ui->txt_stprojekta->setText(besedilo);

	napolni();

}

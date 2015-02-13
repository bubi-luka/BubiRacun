#include <QtSql>
#include <QMessageBox>

#include "wid_kuponi.h"
#include "ui_wid_kuponi.h"
#include "kuponi.h"
#include "kodiranje.h"

wid_kuponi::wid_kuponi(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::wid_kuponi)
{
	ui->setupUi(this);

	napolni();

}

wid_kuponi::~wid_kuponi()
{
	delete ui;
}


void wid_kuponi::napolni() {

	int izbranec = 0;
	int razvrsti = 0;

	if ( ui->tbl_kuponi->selectedItems().count() > 0 ) {
		izbranec = ui->tbl_kuponi->selectedItems().takeAt(0)->row();
	}

	razvrsti = ui->tbl_kuponi->horizontalHeader()->sortIndicatorSection();

		// clear previous content
		ui->tbl_kuponi->clear();

		for (int i = 0; i <= 5; i++) {
			ui->tbl_kuponi->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM kuponi");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_kuponi->removeRow(0);
		}

		// start filling the table
		ui->tbl_kuponi->insertColumn(0);
		ui->tbl_kuponi->insertColumn(1);
		ui->tbl_kuponi->insertColumn(2);
		ui->tbl_kuponi->insertColumn(3);
		ui->tbl_kuponi->insertColumn(4);
		ui->tbl_kuponi->insertColumn(5);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("St. kupona");
		naslov2->setText("Prejemnik");
		naslov3->setText("Datum pridobitve");
		naslov4->setText("Uveljavitelj");
		naslov5->setText("Datum uveljavitve");

		ui->tbl_kuponi->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_kuponi->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_kuponi->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_kuponi->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_kuponi->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_kuponi->setHorizontalHeaderItem(5, naslov5);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM kuponi");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_kuponi->insertRow(row);
			ui->tbl_kuponi->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[6] = {"id", "kupon", "prejemnik", "datumprejema", "uporabitelj", "datumuporabe"};

			while (col <= 5) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "id" ) {
					celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
				}
				else if ( polja[i] == "prejemnik" ) {
					QSqlQuery sql_oseba;
					sql_oseba.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
					sql_oseba.exec();
					if ( sql_oseba.next() ) {
						if ( prevedi(sql_oseba.value(sql_oseba.record().indexOf("tip")).toString()) == "fizicna" ) {
							celica->setText(prevedi(sql_oseba.value(sql_oseba.record().indexOf("ime")).toString()) +
											" " + prevedi(sql_oseba.value(sql_oseba.record().indexOf("priimek")).toString()));
						}
						else {
							celica->setText(prevedi(sql_oseba.value(sql_oseba.record().indexOf("ime")).toString()));
						}
					}
				}
				else if ( polja[i] == "uporabitelj" ) {
					QSqlQuery sql_oseba;
					sql_oseba.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
					sql_oseba.exec();
					if ( sql_oseba.next() ) {
						if ( prevedi(sql_oseba.value(sql_oseba.record().indexOf("tip")).toString()) == "fizicna" ) {
							celica->setText(prevedi(sql_oseba.value(sql_oseba.record().indexOf("ime")).toString()) +
											" " + prevedi(sql_oseba.value(sql_oseba.record().indexOf("priimek")).toString()));
						}
						else {
							celica->setText(prevedi(sql_oseba.value(sql_oseba.record().indexOf("ime")).toString()));
						}
					}
				}
				else {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				}
				ui->tbl_kuponi->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}

	ui->tbl_kuponi->selectRow(izbranec);
	ui->tbl_kuponi->sortByColumn(razvrsti, Qt::AscendingOrder);

}

void wid_kuponi::on_tbl_kuponi_doubleClicked() {

	kuponi *uredi = new kuponi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_kuponi->selectedItems().takeAt(0)->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_kuponi::on_btn_nov_clicked() {

	kuponi *uredi = new kuponi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov kupon");
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_kuponi::on_btn_brisi_clicked() {

	QString id = ui->tbl_kuponi->selectedItems().takeAt(0)->text();

		QSqlQuery sql_brisi;
		sql_brisi.prepare("DELETE FROM kuponi WHERE id LIKE '" + id + "'");
		sql_brisi.exec();

	ui->tbl_kuponi->removeRow(ui->tbl_kuponi->selectedItems().takeAt(0)->row());
	osvezi("kuponi");

}

void wid_kuponi::osvezi(QString beseda) {

	if ( beseda == "kuponi" ) {
		napolni();
	}

}

QString wid_kuponi::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_kuponi::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

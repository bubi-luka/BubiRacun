#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"
#include "varnost.h"
#include "sif_ddv.h"
#include "wid_ddv.h"
#include "ui_wid_ddv.h"

wid_ddv::wid_ddv(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::wid_ddv)
{
	ui->setupUi(this);

	ui->rb_aktivni->setChecked(true);

}

wid_ddv::~wid_ddv()
{
	delete ui;
}

void wid_ddv::on_btn_nov_clicked() {

	sif_ddv *uredi = new sif_ddv;
	uredi->show();

	// poslji signal, da je to nov ddv
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov ddv");
	this->disconnect();

	// prejmi signal za osvezevanje tabele
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_ddv::on_tbl_ddv_doubleClicked() {

	sif_ddv *uredi = new sif_ddv;
	uredi->show();

	// poslji signal za urejanje ddv-ja skupaj z id stevilko vnosa
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_ddv->selectedItems().takeAt(0)->text());
	this->disconnect();

	// prejmi signal za osvezevanje tabele
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_ddv::osvezi(QString beseda) {

	if ( beseda == "ddv" ) {
		napolni();
	}

}

void wid_ddv::on_rb_aktivni_toggled() {

	if ( ui->rb_aktivni->isChecked() ) {
		ui->rb_aktivni->setChecked(true);
		ui->rb_vsi->setChecked(false);
	}
	else {
		ui->rb_aktivni->setChecked(false);
		ui->rb_vsi->setChecked(true);
	}

	napolni();

}

void wid_ddv::on_rb_vsi_toggled() {

	if ( ui->rb_aktivni->isChecked() ) {
		ui->rb_aktivni->setChecked(true);
		ui->rb_vsi->setChecked(false);
	}
	else {
		ui->rb_aktivni->setChecked(false);
		ui->rb_vsi->setChecked(true);
	}

	napolni();

}

void wid_ddv::napolni() {

		// clear previous content
		ui->tbl_ddv->clear();

		for (int i = 0; i <= 2; i++) {
			ui->tbl_ddv->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM sif_ddv");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_ddv->removeRow(0);
		}

		// start filling the table
		ui->tbl_ddv->insertColumn(0);
		ui->tbl_ddv->insertColumn(1);
		ui->tbl_ddv->insertColumn(2);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Vrednost DDV");
		naslov2->setText("Aktiven");

		ui->tbl_ddv->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_ddv->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_ddv->setHorizontalHeaderItem(2, naslov2);

		ui->tbl_ddv->setColumnWidth(0, 35);

		int row = 0;

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->rb_aktivni->isChecked() ) {
			sql_fill.prepare("SELECT * FROM sif_ddv WHERE aktivnost LIKE '1' ORDER BY id DESC");
		}
		else {
			sql_fill.prepare("SELECT * FROM sif_ddv ORDER BY id DESC");
		}
		sql_fill.exec();

		while (sql_fill.next()) {

			ui->tbl_ddv->insertRow(row);
			ui->tbl_ddv->setRowHeight(row, 20);

			int col = 0;
			int i = 0;

			QString polja[3] = {"id", "vrednost", "aktivnost"};

			while (col <= 2) {
				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "id" ) {
					celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
				}
				else if ( polja[i] == "vrednost") {
					celica->setText(pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString())));
				}
				else if ( polja[i] == "aktivnost" ) {
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt() == 1 ) { // je aktiven
						celica->setText("Da");
					}
					else {
						celica->setText("Ne");
					}
				}
				else {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				}
				ui->tbl_ddv->setItem(row, col, celica);

				col++;
				i++;

			}
			row++;

		}

}

QString wid_ddv::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_ddv::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString wid_ddv::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejico (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	if ( !besedilo.contains(".") ) {
		besedilo.append(".0");
	}

	return besedilo;

}

QString wid_ddv::pretvori_iz_double(QString besedilo) {

	/*
	* pretvarja stevilke v valuto, primerno za obdelavo naprej
	* ni nujno, da je vhodna stevilka resnicno double, lahko gre za drugacno obliko
	*/

	besedilo.replace(".", ","); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9,]")); // odstrani vse znake razen stevilk in decimalne vejice

	while ( besedilo.left(1) == "0" ) { // odstranimo vse vodilne nicle
		besedilo.remove(0,1);
	}
	if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0,00
		besedilo.append("0");
	}
	if ( !besedilo.contains(",") ) {
		besedilo.append(",");
	}
	if ( besedilo.left(1) == "," ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
		besedilo.prepend("0");
	}
	if ( besedilo.right(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 0
		besedilo.append("0");
	}

	besedilo.append(" %"); // doda oznako za odstotek

	return besedilo;

}

#include <QKeyEvent>
#include <QMessageBox>
#include <QtSql>

#include "kodiranje.h"
#include "varnost.h"
#include "prejetiracuni_dodajddv.h"
#include "ui_prejetiracuni_dodajddv.h"

prejetiracuni_dodajddv::prejetiracuni_dodajddv(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::prejetiracuni_dodajddv)
{
	ui->setupUi(this);

	ui->txt_izbor->setText("");
	ui->txt_izbor->setHidden(true);

}

prejetiracuni_dodajddv::~prejetiracuni_dodajddv()
{
	delete ui;
}

void prejetiracuni_dodajddv::on_btn_izhod_clicked() {

	close();

}

void prejetiracuni_dodajddv::on_btn_sprejmi_clicked() {

	poslji(pretvori_v_double(ui->tbl_ddv->selectedItems().at(1)->text()));
	close();

}

void prejetiracuni_dodajddv::on_tbl_ddv_doubleClicked() {

	on_btn_sprejmi_clicked();

}

void prejetiracuni_dodajddv::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}

}

void prejetiracuni_dodajddv::on_rb_aktivni_toggled() {

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

void prejetiracuni_dodajddv::on_rb_vsi_toggled() {

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

QString prejetiracuni_dodajddv::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString prejetiracuni_dodajddv::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void prejetiracuni_dodajddv::prejem(QString besedilo) {

	ui->txt_izbor->setText(besedilo);
	ui->rb_aktivni->setChecked(true);

}

void prejetiracuni_dodajddv::napolni() {

		// clear previous content
		ui->tbl_ddv->clear();

		for (int i = 0; i <= 1; i++) {
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

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Vrednost DDV");

		ui->tbl_ddv->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_ddv->setHorizontalHeaderItem(1, naslov1);

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

			if ( !ui->txt_izbor->text().contains(";" + prevedi(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString()) + ";")) {
				ui->tbl_ddv->insertRow(row);
				ui->tbl_ddv->setRowHeight(row, 20);

				int col = 0;
				int i = 0;

				QString polja[2] = {"id", "vrednost"};

				while (col <= 1) {
					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[i] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
					}
					else if ( polja[i] == "vrednost") {
						celica->setText(pretvori_v_double(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString())).replace(".", ",") + " %");
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

}

QString prejetiracuni_dodajddv::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	return besedilo;

}

QString prejetiracuni_dodajddv::pretvori_iz_double(QString besedilo) {

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
	if ( besedilo.left(1) == "," ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
		besedilo.prepend("0");
	}
	if ( besedilo.right(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 00
		besedilo.append("00");
	}
	if ( besedilo.right(2).left(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 00
		besedilo.append("0");
	}
	if ( !besedilo.contains(",") ) { // ce je celo stevilo dodamo decimalno locilo in vrednost 00
		besedilo.append(",00");
	}


	besedilo.append(" EUR"); // doda oznako za evre

	return besedilo;

}

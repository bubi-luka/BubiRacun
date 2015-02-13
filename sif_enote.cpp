#include <QtSql>
#include <QMessageBox>
#include "kodiranje.h"

#include "sif_enote.h"
#include "ui_sif_enote.h"

sif_enote::sif_enote(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::sif_enote)
{
	ui->setupUi(this);

	osvezi_tabelo();

}

sif_enote::~sif_enote()
{
	delete ui;
}

void sif_enote::on_btn_vnesi_clicked() {

		// preverimo polja
		bool napaka = false;

		if ( ui->txt_ime->text() == "" ) {
			napaka = true;
			QMessageBox box_napaka;
			box_napaka.setText("Napaka pri vnosu");
			box_napaka.setInformativeText("Prosim, Vnesite pravilno ime!");
			box_napaka.exec();
		}
		if ( ui->txt_enota->text() == "" ) {
			napaka = true;
			QMessageBox box_napaka;
			box_napaka.setText("Napaka pri vnosu");
			box_napaka.setInformativeText("Prosim, vnesite ustrezno enoto!");
			box_napaka.exec();
		}
		QSqlQuery sql_check;
		sql_check.prepare("SELECT * FROM sif_enote WHERE enota LIKE '" + pretvori(ui->txt_enota->text()) + "'");
		sql_check.exec();
		while ( sql_check.next() ) {
			if ( prevedi(sql_check.value(sql_check.record().indexOf("id")).toString()) != ui->txt_id->text() ) {
				napaka = true;
				QMessageBox box_napaka;
				box_napaka.setText("Napaka pri vnosu");
				box_napaka.setInformativeText("Ta enota ze obstaja! Prosim, vnesite novo, edinstveno enoto ali popravite staro!");
				box_napaka.exec();
			}
		}

		if ( napaka == false ) {
			// glede na stanje gumba za shranjevanje dolocimo vrednost SQL stavka
			QString stavek = "";
			if ( ui->btn_vnesi->text() == "Vnesi" ) {
				stavek = "INSERT INTO sif_enote (ime, enota, aktivnost) VALUES (?, ?, ?)";
			}
			else if ( ui->btn_vnesi->text() == "Popravi" ) {
				stavek = "UPDATE sif_enote SET ime = ?, enota = ?, aktivnost = ? WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'";
			}
			// nastavimo vrednost aktivnosti
			QString aktivnost = "0"; // neaktivna
			if ( ui->cb_aktivnost->isChecked() ) {
				aktivnost = "1"; // aktivna
			}

			QSqlQuery sql_shrani;
			sql_shrani.prepare(stavek);
			sql_shrani.bindValue(0, pretvori(ui->txt_ime->text()));
			sql_shrani.bindValue(1, pretvori(ui->txt_enota->text()));
			sql_shrani.bindValue(2, pretvori(aktivnost));
			sql_shrani.exec();
		}

	osvezi_tabelo();

}

void sif_enote::on_btn_izbrisi_clicked() {

	ui->txt_id->setText("");
	ui->txt_ime->setText("");
	ui->txt_enota->setText("");
	ui->cb_aktivnost->setChecked(false);
	ui->btn_vnesi->setText("Vnesi");

}

void sif_enote::on_btn_izhod_clicked() {

	close();

}

void sif_enote::on_rb_vsi_toggled() {

	osvezi_tabelo();

}

void sif_enote::on_rb_aktivni_toggled() {

	osvezi_tabelo();

}

void sif_enote::on_tbl_enote_itemSelectionChanged() {

	napolni_polja();

}

void sif_enote::napolni_polja() {

	if ( ui->btn_vnesi->text() != "Polnim" ) {

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_enote WHERE id LIKE '" + pretvori(ui->tbl_enote->selectedItems().takeAt(0)->text()) + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				ui->txt_id->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
				ui->txt_ime->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString()));
				ui->txt_enota->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString()));
				if ( prevedi(sql_fill.value(sql_fill.record().indexOf("aktivnost")).toString()) == "1" ) { // enota je aktivna
					ui->cb_aktivnost->setChecked(true);
				}
				else { // enota ni aktivna, aktivnost je 0
					ui->cb_aktivnost->setChecked(false);
				}
			}

		// spremenimo besedilo v gumbu za shranjevanje podatkov v ustrezno obliko - popravi za ze vnesene podatke in vnesi za na novo ustvarjene podatke
		if ( ui->txt_id->text() != "" ) {
			ui->btn_vnesi->setText("Popravi");
		}
		else {
			ui->btn_vnesi->setText("Vnesi");
		}

	}

}

void sif_enote::osvezi_tabelo() {

	on_btn_izbrisi_clicked(); // zbrisemo vsa vnosna polja, da nas ne ovirajo pri polnenju tabele

	QString gumb = ui->btn_vnesi->text();

	ui->btn_vnesi->setText("Polnim");

		// podamo filter glede na aktivnost
		QString stavek = "";
		if ( ui->rb_aktivni->isChecked() ) {
			stavek = " WHERE aktivnost LIKE '1'";
		}

		// clear previous content
		ui->tbl_enote->clear();

		for (int i = 0; i <= 3; i++) {
			ui->tbl_enote->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM sif_enote");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_enote->removeRow(0);
		}

		// start filling the table
		ui->tbl_enote->insertColumn(0);
		ui->tbl_enote->insertColumn(1);
		ui->tbl_enote->insertColumn(2);
		ui->tbl_enote->insertColumn(3);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Ime enote");
		naslov2->setText("Enota");
		naslov3->setText("Aktivnost");

		ui->tbl_enote->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_enote->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_enote->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_enote->setHorizontalHeaderItem(3, naslov3);

		ui->tbl_enote->setColumnWidth(0, 35);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM sif_enote" + stavek);
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_enote->insertRow(row);
			ui->tbl_enote->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[4] = {"id", "ime", "enota", "aktivnost"};

			while (col <= 3) {
				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "id" ) {
					celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
				}
				else if ( polja[i] == "aktivnost" ) {
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("aktivnost")).toString()) == "1" ) { // je aktivna
						celica->setText("Aktivna");
					}
					else { // vrednost je 0
						celica->setText("Neaktivna");
					}
				}
				else {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				}
				ui->tbl_enote->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;
		}

	ui->btn_vnesi->setText(gumb);

}

QString sif_enote::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString sif_enote::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"
#include "avtomobili.h"
#include "ui_avtomobili.h"

avtomobili::avtomobili(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::avtomobili)
{
	ui->setupUi(this);
}

avtomobili::~avtomobili()
{
	delete ui;
}

void avtomobili::on_btn_izhod_clicked() {

	close();

}

void avtomobili::on_btn_izbrisi_clicked() {

	ui->txt_id->setText("");
	ui->txt_model->setText("");
	ui->txt_proizvajalec->setText("");
	ui->txt_registrska_stevilka->setText("");
	ui->txt_znamka->setText("");

	ui->txt_lastnik->setCurrentIndex(0);
	ui->txt_lastnik->clear();

	ui->rb_podjetje->setChecked(false);
	ui->rb_zasebnik->setChecked(false);

}

void avtomobili::on_btn_shrani_clicked() {

		QSqlQuery sql_vnesi;

		if (ui->btn_shrani->text() == "Vnesi avtomobil") {
			sql_vnesi.prepare("INSERT INTO avtomobili (proizvajalec, znamka, tip, registrska_stevilka, lastnik, lastnistvo) "
											"VALUES (?, ?, ?, ?, ?, ?) ");
		}
		else {
			sql_vnesi.prepare("UPDATE avtomobili SET proizvajalec = ?, znamka = ?, tip = ?, registrska_stevilka = ?, lastnik = ?, lastnistvo = ? "
						  "WHERE id LIKE '" + ui->txt_id->text() + "'");
		}
		sql_vnesi.bindValue(0, pretvori(ui->txt_proizvajalec->text()));
		sql_vnesi.bindValue(1, pretvori(ui->txt_znamka->text()));
		sql_vnesi.bindValue(2, pretvori(ui->txt_model->text()));
		sql_vnesi.bindValue(3, pretvori(ui->txt_registrska_stevilka->text()));
		sql_vnesi.bindValue(4, pretvori(ui->txt_lastnik->currentText().left(ui->txt_lastnik->currentText().indexOf(") "))));
		if ( ui->rb_podjetje->isChecked() ) {
			sql_vnesi.bindValue(5, pretvori("0"));
		}
		else if ( ui->rb_zasebnik->isChecked() ) {
			sql_vnesi.bindValue(5, pretvori("1"));
		}
		else {
			sql_vnesi.bindValue(5, pretvori(""));
		}
		sql_vnesi.exec();

	// send signal to reload widget
	poslji("avto");

	// close this window
	close();

}

void avtomobili::on_rb_podjetje_toggled() {

	if ( ui->rb_podjetje->isChecked() ) {
		napolni_lastnike();
	}

}

void avtomobili::on_rb_zasebnik_toggled() {

	if ( ui->rb_zasebnik->isChecked() ) {
		napolni_lastnike();
	}

}

void avtomobili::napolni_lastnike() {

	ui->txt_lastnik->clear();
	ui->txt_lastnik->addItem("");

		QSqlQuery sql_lastniki;
		if ( ui->rb_zasebnik->isChecked() ) {
			sql_lastniki.prepare("SELECT * FROM uporabniki");
		}
		else if ( ui->rb_podjetje->isChecked() ) {
			sql_lastniki.prepare("SELECT * FROM podjetje");
		}
		sql_lastniki.exec();
		while ( sql_lastniki.next() ) {
			QString lastnik = "";
			if ( ui->rb_podjetje->isChecked() ) {
				lastnik += prevedi(sql_lastniki.value(sql_lastniki.record().indexOf("id")).toString());
				lastnik += ") ";
				lastnik += prevedi(sql_lastniki.value(sql_lastniki.record().indexOf("ime")).toString());
			}
			else if ( ui->rb_zasebnik->isChecked() ) {
				lastnik += prevedi(sql_lastniki.value(sql_lastniki.record().indexOf("id")).toString());
				lastnik += ") ";
				lastnik += prevedi(sql_lastniki.value(sql_lastniki.record().indexOf("priimek")).toString());
				lastnik += " ";
				lastnik += prevedi(sql_lastniki.value(sql_lastniki.record().indexOf("ime")).toString());
			}
			ui->txt_lastnik->addItem(lastnik);
		}

}

void avtomobili::prejem(QString beseda) {

		if ( beseda.left(3) == "Nov" ) { // nov vnos opravila
			ui->btn_shrani->setText("Vnesi avtomobil");
			on_btn_izbrisi_clicked();
		}
		else {
			ui->btn_shrani->setText("Popravi vnos");

			// napolni podatke
			QSqlQuery sql_avtomobil;
			sql_avtomobil.prepare("SELECT * FROM avtomobili WHERE id LIKE '" + pretvori(beseda) + "'");
			sql_avtomobil.exec();
			if ( sql_avtomobil.next() ) {
				ui->txt_id->setText(prevedi(sql_avtomobil.value(sql_avtomobil.record().indexOf("id")).toString()));
				ui->txt_proizvajalec->setText(prevedi(sql_avtomobil.value(sql_avtomobil.record().indexOf("proizvajalec")).toString()));
				ui->txt_znamka->setText(prevedi(sql_avtomobil.value(sql_avtomobil.record().indexOf("znamka")).toString()));
				ui->txt_model->setText(prevedi(sql_avtomobil.value(sql_avtomobil.record().indexOf("tip")).toString()));
				ui->txt_registrska_stevilka->setText(prevedi(sql_avtomobil.value(sql_avtomobil.record().indexOf("registrska_stevilka")).toString()));
				if ( prevedi(sql_avtomobil.value(sql_avtomobil.record().indexOf("lastnistvo")).toString()) == "1" ) { // zasebnik
					ui->rb_zasebnik->setChecked(true);
				}
				else { // podjetje
					ui->rb_podjetje->setChecked(true);
				}
				ui->txt_lastnik->setCurrentIndex(ui->txt_lastnik->findText(prevedi(sql_avtomobil.value(sql_avtomobil.record().indexOf("lastnik")).toString()) + ") ", Qt::MatchStartsWith));
			}
		}

}

// pretvori v in iz kodirane oblike
QString avtomobili::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString avtomobili::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

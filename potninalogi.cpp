#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QSize>
#include <QSizeF>
#include <QFile>
#include <QFileDialog>

#include "potninalogi.h"
#include "ui_potninalogi.h"
#include "prijava.h"
#include "glavnookno.h"
#include "kodiranje.h"
#include "wid_potovanja.h"
#include "wid_stroski.h"

potninalogi::potninalogi(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::potninalogi)
{
		ui->setupUi(this);

		// pocisti polja privzetih vrednosti
		ui->txt_id->setText("");
		ui->txt_stevilka_naloga->setText("");
		ui->txt_datum_naloga->setDate(QDate::currentDate());
		ui->txt_namen->clear();
		ui->txt_naziv_ciljnega_podjetja->setText("");
		ui->txt_stevilka_projekta->clear();
		ui->txt_opombe->setPlainText("");
		ui->txt_cena_prevoza->setText("");
		ui->txt_cena_dnevnic->setText("");
		ui->txt_ostali_stroski->setText("");
		ui->txt_stroski_skupaj->setText("");

		ui->txt_skupaj_kilometri->setText("");
		ui->txt_kilometrina->setText("");
		ui->txt_skupaj_dnevi->setText("");
		ui->txt_skupaj_ure->setText("");
		ui->txt_priznana_dnevnica->setText("");
		ui->txt_dnevnica_6_8->setText("");
		ui->txt_dnevnica_8_12->setText("");
		ui->txt_dnevnica_12_24->setText("");
		ui->txt_zajtrk_8_12->setText("");
		ui->txt_zajtrk_12_24->setText("");

		ui->txt_predlagatelj_podjetje->setText("");
		ui->txt_predlagatelj_naslov->setText("");
		ui->txt_predlagatelj_naslov_st->setText("");
		ui->txt_predlagatelj_posta->setText("");
		ui->txt_predlagatelj_postna_stevilka->setText("");
		ui->cb_predlagatelj_oseba->setText("");
		ui->txt_predlagatelj_izbira_oseba->clear();
		ui->txt_predlagatelj_oseba->setText("");
		ui->txt_predlagatelj_naziv->setText("");

		ui->txt_prejemnik_izbira_osebe->clear();
		ui->txt_prejemnik_priimek->setText("");
		ui->txt_prejemnik_ime->setText("");
		ui->txt_prejemnik_naziv->setText("");
		ui->txt_prejemnik_naslov->setText("");
		ui->txt_prejemnik_naslov_st->setText("");
		ui->txt_prejemnik_posta->setText("");
		ui->txt_prejemnik_postna_stevilka->setText("");
		ui->txt_prevoz->clear();
		ui->txt_znamka_avtomobila->setText("");
		ui->txt_model_avtomobila->setText("");
		ui->txt_registrska_stevilka->setText("");

		// onemogocimo dolocena polja
		ui->txt_cena_prevoza->setEnabled(false);
		ui->txt_cena_dnevnic->setEnabled(false);
		ui->txt_ostali_stroski->setEnabled(false);
		ui->txt_stroski_skupaj->setEnabled(false);

		ui->txt_skupaj_kilometri->setEnabled(false);
		ui->txt_kilometrina->setEnabled(false);
		ui->txt_skupaj_dnevi->setEnabled(false);
		ui->txt_skupaj_ure->setEnabled(false);
		ui->txt_dnevnica_6_8->setEnabled(false);
		ui->txt_dnevnica_8_12->setEnabled(false);
		ui->txt_dnevnica_12_24->setEnabled(false);

		ui->txt_predlagatelj_podjetje->setEnabled(false);
		ui->txt_predlagatelj_naslov->setEnabled(false);
		ui->txt_predlagatelj_naslov_st->setEnabled(false);
		ui->txt_predlagatelj_posta->setEnabled(false);
		ui->txt_predlagatelj_postna_stevilka->setEnabled(false);
		ui->txt_predlagatelj_oseba->setEnabled(false);
		ui->txt_predlagatelj_naziv->setEnabled(false);

		ui->txt_prejemnik_priimek->setEnabled(false);
		ui->txt_prejemnik_ime->setEnabled(false);
		ui->txt_prejemnik_naziv->setEnabled(false);
		ui->txt_prejemnik_naslov->setEnabled(false);
		ui->txt_prejemnik_naslov_st->setEnabled(false);
		ui->txt_prejemnik_posta->setEnabled(false);
		ui->txt_prejemnik_postna_stevilka->setEnabled(false);
		ui->txt_znamka_avtomobila->setEnabled(false);
		ui->txt_model_avtomobila->setEnabled(false);
		ui->txt_registrska_stevilka->setEnabled(false);

		// nastavi privzete vrednosti
		ui->txt_priznana_dnevnica->setChecked(true);
		ui->cb_predlagatelj_oseba->setChecked(false);

		// skrij neuporabljena polja
		ui->txt_predlagatelj_izbira_oseba->setHidden(true);

		ui->txt_znamka_avtomobila->setHidden(true);
		ui->txt_registrska_stevilka->setHidden(true);
		ui->txt_model_avtomobila->setHidden(true);
		ui->label_9->setHidden(true);
		ui->label_8->setHidden(true);
		ui->label_40->setHidden(true);

		// pridobi podatke za stevilko naloga
		QString leto = QDate::currentDate().toString("yyyy");
		leto = leto.insert(3, "\\");
		leto = leto.insert(2, "\\");
		leto = leto.insert(1, "\\");
		leto = leto.insert(0, "\\");

		// nastavi obliko leta
		ui->txt_stevilka_naloga->setInputMask("P\\N-9999-999;_");

		leto = QDate::currentDate().toString("yyyy");

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
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
			// baza je odprta

			// zapisi stevilko potnega naloga
			int i = 1;
			QString stevilka = "";
			QSqlQuery sql_insert_stnaloga;
			sql_insert_stnaloga.prepare("SELECT * FROM potni_nalogi WHERE stevilka_naloga LIKE '" + pretvori("PN-" + leto) + "%'");
			sql_insert_stnaloga.exec();
			while (sql_insert_stnaloga.next()) {
				i++;
			}
			if ( i < 10 ) {
				stevilka = "00" + QString::number(i, 10);
			}
			else if ( i < 100 ) {
				stevilka = "0" + QString::number(i, 10);
			}
			else {
				stevilka = "" + QString::number(i, 10);
			}
			ui->txt_stevilka_naloga->setText("PN-" + leto + "-" + stevilka);

			// nastavi privzeto vrednost za kilometrino
			ui->txt_kilometrina->setText("0,37 EUR");

			// dodaj prazno vrstico spustnim seznamom
			ui->txt_namen->addItem("");
		//	ui->txt_stevilka_projekta->addItem("Prosim, izberite prejemnika");
			ui->txt_predlagatelj_izbira_oseba->addItem("");
			ui->txt_prejemnik_izbira_osebe->addItem("");
			ui->txt_prevoz->addItem("");

			// napolni spustne sezname
			QSqlQuery sql_fill_combo;
			sql_fill_combo.prepare("SELECT * FROM sif_namen_potnega_naloga");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				ui->txt_namen->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ") " +
															 prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("namen")).toString()));
			}
			sql_fill_combo.clear();

			// napolni prelagatelja - podjetje
			sql_fill_combo.prepare("SELECT * FROM podjetje WHERE id LIKE '" + vApp->firm() + "'");
			sql_fill_combo.exec();
			if ( sql_fill_combo.next() ) {
				ui->txt_predlagatelj_podjetje->setText(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
				ui->txt_predlagatelj_naslov->setText(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naslov")).toString()));
				ui->txt_predlagatelj_naslov_st->setText(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naslov_st")).toString()));
				ui->txt_predlagatelj_posta->setText(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("posta")).toString()));
				ui->txt_predlagatelj_postna_stevilka->setText(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("postna_stevilka")).toString()));
			}
			sql_fill_combo.clear();
			// napolni predlagatelja in prejemnika - spustni seznam oseb
			sql_fill_combo.prepare("SELECT * FROM uporabniki WHERE podjetje LIKE '" + pretvori(vApp->id()) + "'");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				QString uporabnik;
				uporabnik = prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ") ";
				uporabnik += prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("priimek")).toString()) + " ";
				uporabnik += prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString());
				ui->txt_predlagatelj_izbira_oseba->addItem(uporabnik);
				ui->txt_prejemnik_izbira_osebe->addItem(uporabnik);
			}
			sql_fill_combo.clear();

			sql_fill_combo.prepare("SELECT * FROM sif_prevozna_sredstva");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				ui->txt_prevoz->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ") " +
																prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("prevoz")).toString()));
			}
			sql_fill_combo.clear();
		}
		base.close();

		ui->tab_potni_stroski->setCurrentIndex(0);

}

potninalogi::~potninalogi()
{
		delete ui;
}

void potninalogi::on_btn_izhod_clicked() {

	close();

}
// preveriti pravilnost izvoza
void potninalogi::on_btn_izvozi_clicked() {

	on_btn_sprejmi_clicked();

	print(ui->txt_id->text());

}

void potninalogi::on_cb_predlagatelj_oseba_toggled() {

	if ( ui->cb_predlagatelj_oseba->isChecked() ) {
		ui->txt_predlagatelj_izbira_oseba->setHidden(false);
		ui->txt_predlagatelj_oseba->setHidden(true);
	}
	else {
		ui->txt_predlagatelj_izbira_oseba->setHidden(true);
		ui->txt_predlagatelj_oseba->setHidden(false);
	}

}

void potninalogi::on_txt_predlagatelj_izbira_oseba_currentIndexChanged() {

	ui->txt_predlagatelj_oseba->setText("");
	ui->txt_predlagatelj_naziv->setText("");

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
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
	// baza je odprta

		// poisci indeks podjetja
		QString indeks;
		indeks = prevedi(ui->txt_predlagatelj_izbira_oseba->currentText());
		indeks = indeks.left(indeks.indexOf(") ", 0));
		indeks = pretvori(indeks);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + indeks + "'");
		sql_fill.exec();
		if ( sql_fill.next() ) {
			ui->txt_predlagatelj_oseba->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString()) + " " +
																					prevedi(sql_fill.value(sql_fill.record().indexOf("priimek")).toString()));
			QSqlQuery sql_fill_naziv;
			sql_fill_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("naziv")).toString() + "'");
			sql_fill_naziv.exec();
			if ( sql_fill_naziv.next() ) {
				ui->txt_predlagatelj_naziv->setText(prevedi(sql_fill_naziv.value(sql_fill_naziv.record().indexOf("naziv")).toString()));
			}
		}
	}
	base.close();

}

void potninalogi::on_txt_prejemnik_izbira_osebe_currentIndexChanged() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
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
		// baza je odprta

		// poisci indeks podjetja
		QString indeks;
		indeks = prevedi(ui->txt_prejemnik_izbira_osebe->currentText());
		indeks = indeks.left(indeks.indexOf(") ", 0));
		indeks = pretvori(indeks);

		QSqlQuery sql_fill_ime;
		sql_fill_ime.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + indeks + "'");
		sql_fill_ime.exec();
		if (sql_fill_ime.next()) {
			ui->txt_prejemnik_priimek->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("priimek")).toString()));
			ui->txt_prejemnik_ime->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("ime")).toString()));
			QSqlQuery sql_naziv;
			sql_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + sql_fill_ime.value(sql_fill_ime.record().indexOf("naziv")).toString() + "'");
			sql_naziv.exec();
			if ( sql_naziv.next() ) {
				ui->txt_prejemnik_naziv->setText(prevedi(sql_naziv.value(sql_naziv.record().indexOf("naziv")).toString()));
			}
			ui->txt_prejemnik_naslov->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("naslov")).toString()));
			ui->txt_prejemnik_naslov_st->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("naslov_stevilka")).toString()));
			ui->txt_prejemnik_posta->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("posta")).toString()));
			ui->txt_prejemnik_postna_stevilka->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("postna_stevilka")).toString()));
			ui->txt_znamka_avtomobila->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("avtomobil")).toString()));
			ui->txt_model_avtomobila->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("model_avtomobila")).toString()));
			ui->txt_registrska_stevilka->setText(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("registracija")).toString()));

			// napolni spustni seznam projektov
			ui->txt_stevilka_projekta->clear();
			ui->txt_stevilka_projekta->addItem("");

			QSqlQuery sql_fill_projekt;
			sql_fill_projekt.prepare("SELECT * FROM projekti WHERE avtor_oseba LIKE '" + indeks + "'");
			sql_fill_projekt.exec();
			while ( sql_fill_projekt.next() ) {
				ui->txt_stevilka_projekta->addItem(prevedi(sql_fill_projekt.value(sql_fill_projekt.record().indexOf("id")).toString()) + ") " +
																					 prevedi(sql_fill_projekt.value(sql_fill_projekt.record().indexOf("stevilka_projekta")).toString()) + " - " +
																					 prevedi(sql_fill_projekt.value(sql_fill_projekt.record().indexOf("naslov_projekta")).toString()));
			}
		}
		else {
			ui->txt_prejemnik_priimek->setText("");
			ui->txt_prejemnik_ime->setText("");
			ui->txt_prejemnik_naziv->setText("");
			ui->txt_prejemnik_naslov->setText("");
			ui->txt_prejemnik_naslov_st->setText("");
			ui->txt_prejemnik_posta->setText("");
			ui->txt_prejemnik_postna_stevilka->setText("");
			ui->txt_znamka_avtomobila->setText("");
			ui->txt_model_avtomobila->setText("");
			ui->txt_registrska_stevilka->setText("");
			ui->txt_stevilka_projekta->clear();
			ui->txt_stevilka_projekta->addItem("Prosim, izberite prejemnika");
		}
	}
	base.close();

}

void potninalogi::on_txt_prevoz_currentIndexChanged(int index) {

	if (index == 1) {
		ui->txt_znamka_avtomobila->setHidden(false);
		ui->txt_model_avtomobila->setHidden(false);
		ui->txt_registrska_stevilka->setHidden(false);
		ui->label_9->setHidden(false);
		ui->label_8->setHidden(false);
		ui->label_40->setHidden(false);
	}
	else {
		ui->txt_znamka_avtomobila->setHidden(true);
		ui->txt_model_avtomobila->setHidden(true);
		ui->txt_registrska_stevilka->setHidden(true);
		ui->label_9->setHidden(true);
		ui->label_8->setHidden(true);
		ui->label_40->setHidden(true);
	}

}
// ne preverja pravilnosti vnesenih podatkov
void potninalogi::on_btn_sprejmi_clicked() {

	izracun(); // just to be safe

	// check for correct fields
	QString napaka = "";
/*
	// nastavitev polja za napako
	QFont font_error;
	font_error.setBold(true);
	font_error.setUnderline(true);

	QFont font_normal;
	font_normal.setBold(false);
	font_normal.setUnderline(false);

	QPalette palette_error;
	QPalette palette_normal;

	QBrush brush_error(QColor(255, 0, 0, 255));
	brush_error.setStyle(Qt::SolidPattern);

	QBrush brush_normal(QColor(0, 0, 0, 255));
	brush_normal.setStyle(Qt::SolidPattern);

	palette_error.setBrush(QPalette::Active, QPalette::WindowText, brush_error);
	palette_error.setBrush(QPalette::Inactive, QPalette::WindowText, brush_error);
	palette_normal.setBrush(QPalette::Active, QPalette::WindowText, brush_normal);
	palette_normal.setBrush(QPalette::Inactive, QPalette::WindowText, brush_normal);
*/
	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
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
		// baza je odprta

/*		// st. naloga ne sme biti ze vnesena
		QSqlQuery sql_check;
		sql_check.prepare("SELECT * FROM potninalogi WHERE stevilkanaloga LIKE '" + ui->txt_stnaloga->text() + "'");
		sql_check.exec();
		if (sql_check.next()) {
			if (sql_check.value(sql_check.record().indexOf("id")).toString() == ui->txt_id->text()) {
				ui->label_2->setPalette(palette_normal);
				ui->label_2->setFont(font_normal);
			}
			else {
				ui->label_2->setPalette(palette_error);
				ui->label_2->setFont(font_error);
				napaka = "error";
				ui->txt_stnaloga->setFocus();
			}
		}
		else {
			ui->label_2->setPalette(palette_normal);
			ui->label_2->setFont(font_normal);
		}
		sql_check.clear();

		// polja, ki ne smejo biti prazna: zacetek, konec, kilometri, vsi podatki o izdajatelju
		// preveri se, ce je avto da ima tako tip kot registrsko

		if (ui->txt_kilometri->text() == "") {
			ui->label_18->setPalette(palette_error);
			ui->label_18->setFont(font_error);
			napaka = "error";
			ui->txt_kilometri->setFocus();
		}
		else {
			ui->label_18->setPalette(palette_normal);
			ui->label_18->setFont(font_normal);
		}

		if (ui->txt_podjetje->currentText() == "") {
			ui->label_4->setPalette(palette_error);
			ui->label_4->setFont(font_error);
			napaka = "error";
			ui->txt_podjetje->setFocus();
		}
		else {
			ui->label_4->setPalette(palette_normal);
			ui->label_4->setFont(font_normal);
		}
		if (ui->txt_predlagatelj->currentText() == "") {
			ui->label_13->setPalette(palette_error);
			ui->label_13->setFont(font_error);
			napaka = "error";
			ui->txt_predlagatelj->setFocus();
		}
		else {
			ui->label_13->setPalette(palette_normal);
			ui->label_13->setFont(font_normal);
		}
		if (ui->txt_nazivpredlagatelja->currentText() == "") {
			ui->label_5->setPalette(palette_error);
			ui->label_5->setFont(font_error);
			napaka = "error";
			ui->txt_nazivpredlagatelja->setFocus();
		}
		else {
			ui->label_5->setPalette(palette_normal);
			ui->label_5->setFont(font_normal);
		}
		if (ui->txt_naslovpodjetja->toPlainText() == "") {
			ui->label_6->setPalette(palette_error);
			ui->label_6->setFont(font_error);
			napaka = "error";
			ui->txt_naslovpodjetja->setFocus();
		}
		else {
			ui->label_6->setPalette(palette_normal);
			ui->label_6->setFont(font_normal);
		}

		if (ui->txt_prejemnik->currentText() == "") {
			ui->label_7->setPalette(palette_error);
			ui->label_7->setFont(font_error);
			napaka = "error";
			ui->txt_prejemnik->setFocus();
		}
		else {
			ui->label_7->setPalette(palette_normal);
			ui->label_7->setFont(font_normal);
		}
		if (ui->txt_prejemnikime->currentText() == "") {
			ui->label_22->setPalette(palette_error);
			ui->label_22->setFont(font_error);
			napaka = "error";
			ui->txt_prejemnikime->setFocus();
		}
		else {
			ui->label_22->setPalette(palette_normal);
			ui->label_22->setFont(font_normal);
		}
		if (ui->txt_nazivprejemnika->currentText() == "") {
			ui->label_10->setPalette(palette_error);
			ui->label_10->setFont(font_error);
			napaka = "error";
			ui->txt_nazivprejemnika->setFocus();
		}
		else {
			ui->label_10->setPalette(palette_normal);
			ui->label_10->setFont(font_normal);
		}
		if (ui->txt_naslovprejemnika->toPlainText() == "") {
			ui->label_11->setPalette(palette_error);
			ui->label_11->setFont(font_error);
			napaka = "error";
			ui->txt_naslovprejemnika->setFocus();
		}
		else {
			ui->label_11->setPalette(palette_normal);
			ui->label_11->setFont(font_normal);
		}
		if (ui->txt_prevoz->currentText() == "") {
			ui->label_12->setPalette(palette_error);
			ui->label_12->setFont(font_error);
			napaka = "error";
			ui->txt_prevoz->setFocus();
		}
		else {
			ui->label_12->setPalette(palette_normal);
			ui->label_12->setFont(font_normal);
		}
		if (ui->txt_kilometrina->text() == "") {
			ui->label_25->setPalette(palette_error);
			ui->label_25->setFont(font_error);
			napaka = "error";
			ui->txt_kilometrina->setFocus();
		}
		else {
			ui->label_25->setPalette(palette_normal);
			ui->label_25->setFont(font_normal);
		}
		if (ui->txt_naloga->text() == "") {
			ui->label_24->setPalette(palette_error);
			ui->label_24->setFont(font_error);
			napaka = "error";
			ui->txt_naloga->setFocus();
		}
		else {
			ui->label_24->setPalette(palette_normal);
			ui->label_24->setFont(font_normal);
		}
		if (ui->txt_namen->currentText() == "") {
			ui->label_23->setPalette(palette_error);
			ui->label_23->setFont(font_error);
			napaka = "error";
			ui->txt_namen->setFocus();
		}
		else {
			ui->label_23->setPalette(palette_normal);
			ui->label_23->setFont(font_normal);
		}

		if (ui->txt_prevoz->currentText() == "Avtomobil" && ui->txt_avtomobil->text() == "") {
			ui->label_9->setPalette(palette_error);
			ui->label_9->setFont(font_error);
			napaka = "error";
			ui->txt_avtomobil->setFocus();
		}
		else {
			ui->label_9->setPalette(palette_normal);
			ui->label_9->setFont(font_normal);
		}
		if (ui->txt_prevoz->currentText() == "Avtomobil" && ui->txt_registrska->text() == "") {
			ui->label_8->setPalette(palette_error);
			ui->label_8->setFont(font_error);
			napaka = "error";
			ui->txt_registrska->setFocus();
		}
		else {
			ui->label_8->setPalette(palette_normal);
			ui->label_8->setFont(font_normal);
		}
*/

		// javi napake, ce ni napak vnesi v bazo
		if (napaka == "") {
			QSqlQuery sql_vnesi_uporabnika;
			if (ui->btn_sprejmi->text() == "Vnesi potni nalog") { // vnesi novega uporabnika

			sql_vnesi_uporabnika.prepare("INSERT INTO potni_nalogi (stevilka_naloga, datum_naloga, namen_naloga, naziv_ciljnega_podjetja, "
																	 "stevilka_projekta, opombe, cena_prevoza, cena_dnevnic, ostali_stroski, stroski_skupaj, skupaj_kilometri, "
																	 "kilometrina, skupaj_dnevi, skupaj_ure, priznana_dnevnica, cena_dnevnice, dnevnica_6_8, "
																	 "dnevnica_8_12, dnevnica_12_24, zajtrk_8_12, zajtrk_12_24, predlagatelj_podjetje, "
																	 "predlagatelj_oseba, prejemnik_oseba, prevozno_sredstvo) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
																	 "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_uporabnika.prepare("UPDATE potni_nalogi SET stevilka_naloga = ?, datum_naloga = ?, namen_naloga = ?, "
																		 "naziv_ciljnega_podjetja = ?, stevilka_projekta = ?, opombe = ?, cena_prevoza = ?, cena_dnevnic = ?, "
																		 "ostali_stroski = ?, stroski_skupaj = ?, skupaj_kilometri = ?, kilometrina = ?, skupaj_dnevi = ?, "
																		 "skupaj_ure = ?, priznana_dnevnica = ?, cena_dnevnice = ?, dnevnica_6_8 = ?, dnevnica_8_12 = ?, "
																		 "dnevnica_12_24 = ?, zajtrk_8_12 = ?, zajtrk_12_24 = ?, predlagatelj_podjetje = ?, "
																		 "predlagatelj_oseba = ?, prejemnik_oseba = ?, prevozno_sredstvo = ? WHERE id LIKE '" +
																		 ui->txt_id->text() + "'");
			}
			sql_vnesi_uporabnika.bindValue(0, pretvori(ui->txt_stevilka_naloga->text()));
			sql_vnesi_uporabnika.bindValue(1, pretvori(ui->txt_datum_naloga->text()));
			QString indeks;
			indeks = prevedi(ui->txt_namen->currentText());
			indeks = indeks.left(indeks.indexOf(") ", 0));
			indeks = pretvori(indeks);
			sql_vnesi_uporabnika.bindValue(2, indeks);
			sql_vnesi_uporabnika.bindValue(3, pretvori(ui->txt_naziv_ciljnega_podjetja->text()));
			indeks = prevedi(ui->txt_stevilka_projekta->currentText());
			indeks = indeks.left(indeks.indexOf(") ", 0));
			indeks = pretvori(indeks);
			sql_vnesi_uporabnika.bindValue(4, indeks);
			sql_vnesi_uporabnika.bindValue(5, pretvori(ui->txt_opombe->toPlainText()));
			sql_vnesi_uporabnika.bindValue(6, pretvori(ui->txt_cena_prevoza->text()));
			sql_vnesi_uporabnika.bindValue(7, pretvori(ui->txt_cena_dnevnic->text()));
			sql_vnesi_uporabnika.bindValue(8, pretvori(ui->txt_ostali_stroski->text()));
			sql_vnesi_uporabnika.bindValue(9, pretvori(ui->txt_stroski_skupaj->text()));
			sql_vnesi_uporabnika.bindValue(10, pretvori(ui->txt_skupaj_kilometri->text()));
			sql_vnesi_uporabnika.bindValue(11, pretvori(ui->txt_kilometrina->text()));
			sql_vnesi_uporabnika.bindValue(12, pretvori(ui->txt_skupaj_dnevi->text()));
			sql_vnesi_uporabnika.bindValue(13, pretvori(ui->txt_skupaj_ure->text()));
			if ( ui->txt_priznana_dnevnica->isChecked() ) {
				sql_vnesi_uporabnika.bindValue(14, pretvori("1"));
			}
			else {
				sql_vnesi_uporabnika.bindValue(14, pretvori("0"));
			}
			sql_vnesi_uporabnika.bindValue(15, pretvori("0")); // cena dnevnice ni dolocena
			sql_vnesi_uporabnika.bindValue(16, pretvori(ui->txt_dnevnica_6_8->text()));
			sql_vnesi_uporabnika.bindValue(17, pretvori(ui->txt_dnevnica_8_12->text()));
			sql_vnesi_uporabnika.bindValue(18, pretvori(ui->txt_dnevnica_12_24->text()));
			if ( ui->txt_zajtrk_8_12->isChecked() ) {
				sql_vnesi_uporabnika.bindValue(19, pretvori("1"));
			}
			else {
				sql_vnesi_uporabnika.bindValue(19, pretvori("0"));
			}
			if ( ui->txt_zajtrk_12_24->isChecked() ) {
				sql_vnesi_uporabnika.bindValue(20, pretvori("1"));
			}
			else {
				sql_vnesi_uporabnika.bindValue(20, pretvori("0"));
			}

			indeks = indeks.left(indeks.indexOf(") ", 0));
			indeks = pretvori(indeks);
			sql_vnesi_uporabnika.bindValue(21, indeks);

			indeks = prevedi(ui->txt_predlagatelj_izbira_oseba->currentText());
			indeks = indeks.left(indeks.indexOf(") ", 0));
			indeks = pretvori(indeks);
			sql_vnesi_uporabnika.bindValue(22, indeks);

			indeks = prevedi(ui->txt_prejemnik_izbira_osebe->currentText());
			indeks = indeks.left(indeks.indexOf(") ", 0));
			indeks = pretvori(indeks);
			sql_vnesi_uporabnika.bindValue(23, indeks);

			indeks = prevedi(ui->txt_prevoz->currentText());
			indeks = indeks.left(indeks.indexOf(") ", 0));
			indeks = pretvori(indeks);
			sql_vnesi_uporabnika.bindValue(24, indeks);
			sql_vnesi_uporabnika.exec();

			// send signal to reload widget
			poslji("potninalog");

			// close this window
			close();
		}
		else {
			QMessageBox msgbox;
			msgbox.setText("Dolocena polja niso pravilno izpolnjena");
			msgbox.exec();
		}
	}
	base.close();

}

void potninalogi::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}
}

void potninalogi::prejem(QString besedilo) {

	if (besedilo == "Nov nalog") {
		ui->btn_sprejmi->setText("Vnesi potni nalog");
		ui->btn_izvozi->setEnabled(false); // nalog je potrebno najprej shraniti, sele nato ga lahko tiskamo
		ui->wid_pot->setEnabled(false);
		ui->wid_st->setEnabled(false);
	}
	else {
		ui->btn_sprejmi->setText("Popravi potni nalog");
		ui->btn_izvozi->setEnabled(true);
		ui->wid_pot->setEnabled(true);
		ui->wid_st->setEnabled(true);
		// besedilo nosi ID ze obstojecega uporabnika, potrebno je napolniti polja
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
			QSqlQuery sql_napolni;
			sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(sql_napolni.value(sql_napolni.record().indexOf("id")).toString());
				ui->txt_stevilka_naloga->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_naloga")).toString()));
				ui->txt_datum_naloga->setDate(QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_naloga")).toString()), "dd'.'MM'.'yyyy"));
				ui->txt_naziv_ciljnega_podjetja->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv_ciljnega_podjetja")).toString()));
				ui->txt_opombe->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opombe")).toString()));
				ui->txt_cena_prevoza->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_prevoza")).toString()));
				ui->txt_cena_dnevnic->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnic")).toString()));
				ui->txt_ostali_stroski->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ostali_stroski")).toString()));
				ui->txt_stroski_skupaj->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stroski_skupaj")).toString()));

				ui->txt_skupaj_kilometri->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("skupaj_kilometri")).toString()));
				ui->txt_kilometrina->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kilometrina")).toString()));
				ui->txt_skupaj_dnevi->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("skupaj_dnevi")).toString()));
				ui->txt_skupaj_ure->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("skupaj_ure")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("priznana_dnevnica")).toString()) == "1" ) {
					ui->txt_priznana_dnevnica->setChecked(true);
				}
				else {
					ui->txt_priznana_dnevnica->setChecked(false);
				}

				ui->txt_dnevnica_6_8->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("dnevnica_6_8")).toString()));
				ui->txt_dnevnica_8_12->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("dnevnica_8_12")).toString()));
				ui->txt_dnevnica_12_24->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("dnevnica_12_24")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("zajtrk_8_12")).toString()) == "1" ) {
					ui->txt_zajtrk_8_12->setChecked(true);
				}
				else {
					ui->txt_zajtrk_8_12->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("zajtrk_12_24")).toString()) == "1" ) {
					ui->txt_zajtrk_12_24->setChecked(true);
				}
				else {
					ui->txt_zajtrk_12_24->setChecked(false);
				}

				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("namen_naloga")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_namen->setCurrentIndex(ui->txt_namen->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") " +
																																 prevedi(sql_combo.value(sql_combo.record().indexOf("namen")).toString())));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("predlagatelj_oseba")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					QString uporabnik;
					uporabnik = prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") ";
					uporabnik += prevedi(sql_combo.value(sql_combo.record().indexOf("priimek")).toString()) + " ";
					uporabnik += prevedi(sql_combo.value(sql_combo.record().indexOf("ime")).toString());
					ui->txt_predlagatelj_izbira_oseba->setCurrentIndex(ui->txt_predlagatelj_izbira_oseba->findText(uporabnik));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("prejemnik_oseba")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					QString uporabnik;
					uporabnik = prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") ";
					uporabnik += prevedi(sql_combo.value(sql_combo.record().indexOf("priimek")).toString()) + " ";
					uporabnik += prevedi(sql_combo.value(sql_combo.record().indexOf("ime")).toString());
					ui->txt_prejemnik_izbira_osebe->setCurrentIndex(ui->txt_prejemnik_izbira_osebe->findText(uporabnik));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("stevilka_projekta")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					QString projekt;
					projekt = prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") ";
					projekt += prevedi(sql_combo.value(sql_combo.record().indexOf("stevilka_projekta")).toString()) + " - ";
					projekt += prevedi(sql_combo.value(sql_combo.record().indexOf("naslov_projekta")).toString());
					ui->txt_stevilka_projekta->setCurrentIndex(ui->txt_stevilka_projekta->findText(projekt));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_prevozna_sredstva WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("prevozno_sredstvo")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					QString prevoz;
					prevoz = prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") ";
					prevoz += prevedi(sql_combo.value(sql_combo.record().indexOf("prevoz")).toString());
					ui->txt_prevoz->setCurrentIndex(ui->txt_prevoz->findText(prevoz));
				}
				sql_combo.clear();
			}
		}
		base.close();
	}

	// insert widgets
	wid_stroski *widst = new wid_stroski;
	ui->wid_st->setWidget(widst);
	QObject::connect(this, SIGNAL(prenos(QString)),
				 widst , SLOT(prejem(QString)));
	prenos(ui->txt_stevilka_naloga->text());
	this->disconnect();

	QObject::connect(widst, SIGNAL(prenesi()),
					 this, SLOT(sprejmist()));

	wid_potovanja *widpot = new wid_potovanja;
	ui->wid_pot->setWidget(widpot);

	QObject::connect(this, SIGNAL(prenos(QString)),
				 widpot , SLOT(prejem(QString)));
	prenos(ui->txt_stevilka_naloga->text());
	this->disconnect();

	QObject::connect(widpot, SIGNAL(prenesi()),
					 this, SLOT(sprejmipot()));

	izracun();

}

QString potninalogi::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString potninalogi::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void potninalogi::izracun() {

	double kilometri = 0;
	double cenaprevoza = 0;
	double ostalo = 0;

	QDateTime prvidatum = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
	QDateTime zadnjidatum = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");

	int razlika;

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "izracun");
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
		// baza je odprta

		// stroski, povezani s potjo
		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + pretvori(ui->txt_stevilka_naloga->text()) + "'");
		sql_pot.exec();
		while ( sql_pot.next() ) {
			QString i = prevedi(sql_pot.value(sql_pot.record().indexOf("kilometri")).toString());
			double x = i.replace(";", ".").toDouble();
			kilometri = kilometri + x;

			// fill dates
			if ( prvidatum == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
				prvidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( zadnjidatum == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
				zadnjidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( prvidatum.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) < 0 ) {
				prvidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( zadnjidatum.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) > 0 ) {
				zadnjidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
		}

		ui->txt_skupaj_kilometri->setText(QString::number(kilometri, 'f', 3));
		double kilometrina = ui->txt_kilometrina->text().remove(" EUR").replace(",", ".").toDouble();
		cenaprevoza = kilometri * kilometrina;
		ui->txt_cena_prevoza->setText(QString::number(cenaprevoza, 'f', 2));

		// dnevi
		razlika =	prvidatum.daysTo(zadnjidatum);
		ui->txt_skupaj_dnevi->setText(QString::number(razlika, 10));

		//ure
		int minute = prvidatum.secsTo(zadnjidatum)/60;
		int ure = minute/60;
		minute = minute - ure*60;
		ure = ure - razlika*24;

		ui->txt_skupaj_ure->setText(QString::number(ure, 10) + " ur " + QString::number(minute, 10) + " minut");

		ui->txt_dnevnica_12_24->setText(ui->txt_skupaj_dnevi->text());
		ui->txt_dnevnica_8_12->setText("0");
		ui->txt_dnevnica_6_8->setText("0");

		if ( ure >= 12 ) {
			ui->txt_dnevnica_12_24->setText(QString::number(ui->txt_skupaj_dnevi->text().toInt() + 1, 10));
		}
		else if ( ure >= 8 ) {
			ui->txt_dnevnica_8_12->setText("1");
		}
		else if ( ure >= 6 ) {
			ui->txt_dnevnica_6_8->setText("1");
		}

		// izracun dnevnic
		if ( ui->txt_priznana_dnevnica->isChecked() ) {
			double cenadnevnice = 0;
			cenadnevnice = 7.45 * ui->txt_dnevnica_6_8->text().toDouble();
			if ( ui->txt_zajtrk_8_12->isChecked() ) {
				cenadnevnice = cenadnevnice + 10.68 * ui->txt_dnevnica_8_12->text().toDouble() * 0.85;
			}
			else {
				cenadnevnice = cenadnevnice + 10.68 * ui->txt_dnevnica_8_12->text().toDouble();
			}
			if ( ui->txt_zajtrk_12_24->isChecked() ) {
				cenadnevnice = cenadnevnice + 21.39 * ui->txt_dnevnica_12_24->text().toDouble() * 0.90;
			}
			else {
				cenadnevnice = cenadnevnice + 21.39 * ui->txt_dnevnica_12_24->text().toDouble();
			}
			ui->txt_cena_dnevnic->setText(QString::number(cenadnevnice, 'f', 2));
		}
		else {
			ui->txt_cena_dnevnic->setText("0.00");
		}
		// ostali stroski
		QSqlQuery sql_ostalo;
		sql_ostalo.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(ui->txt_stevilka_naloga->text()) + "'");
		sql_ostalo.exec();
		while ( sql_ostalo.next() ) {
			ostalo = ostalo + prevedi(sql_ostalo.value(sql_ostalo.record().indexOf("cena")).toString()).replace(";", ".").toDouble();
		}
		ui->txt_ostali_stroski->setText(QString::number(ostalo, 'f', 2));

		// stroski skupaj
		ui->txt_stroski_skupaj->setText(QString::number(ui->txt_cena_prevoza->text().toDouble() + ui->txt_cena_dnevnic->text().toDouble() +
												ui->txt_ostali_stroski->text().toDouble(), 'f', 2));
	}
	base.close();

}

void potninalogi::sprejmist() {

	izracun();

}

void potninalogi::sprejmipot() {

	izracun();

}

void potninalogi::on_txt_priznana_dnevnica_toggled() {

	izracun();

}

void potninalogi::on_txt_zajtrk_8_12_toggled() {

	izracun();

}

void potninalogi::on_txt_zajtrk_12_24_toggled() {

	izracun();

}

void potninalogi::print(QString id) {

	/**
		* id nosi id stevilko potnega naloga, s cimer lahko hkrati natisnemo
		* vec dokumentov, bodisi istih, bodisi razlicnih
		**/

	/**
		* Nastavimo spremenljivke, ki jih bomo uporabili v dokumentu
		**/
	// podatki o potnem nalogu
	QString stevilka_naloga;
	QString datum_naloga;
	QString namen_potnega_naloga;
	QString prevozno_sredstvo;
	QString cena_dnevnice;
	QString stevilo_dnevnic;
	QString stroski_skupaj;
	QString razdalja;
	QString kilometrina;
	QString ostali_stroski;
	QString znesek_drugih_stroskov;
	QString zvisanje_dnevnic = "0";

	// podatki o predlagatelju - podjetje
	QString predlagatelj_podjetje_ime;
	QString predlagatelj_podjetje_polno_ime;
	QString predlagatelj_podjetje_naslov;
	QString predlagatelj_podjetje_naslov_st;
	QString predlagatelj_podjetje_postna_st;
	QString predlagatelj_podjetje_posta;

	// podatki o predlagatelju - oseba
	QString predlagatelj_oseba_ime;
	QString predlagatelj_oseba_priimek;
	QString predlagatelj_oseba_naslov;
	QString predlagatelj_oseba_naslov_stevilka;
	QString predlagatelj_oseba_postna_stevilka;
	QString predlagatelj_oseba_posta;

	// podatki o prejemniku
	QString prejemnik_ime;
	QString prejemnik_priimek;
	QString prejemnik_naziv;
	QString prejemnik_naslov;
	QString prejemnik_naslov_stevilka;
	QString prejemnik_postna_stevilka;
	QString prejemnik_posta;
	QString prejemnik_znamka_avtomobila;
	QString prejemnik_model_avtomobila;
	QString prejemnik_registrska_stevilka;

	// podatki o potovanju
	QString relacija;
	QString datum_odhoda;
	QString ura_odhoda;
	QString potovanje;
	QString datum_prihoda;
	QString ura_prihoda;
	QString dnevi_potovanja;
	QDateTime dt_trajanje;
	QDateTime dt_datum_odhoda;
	QDateTime dt_datum_prihoda;
	int minute;
	int ure;

	// odpri podatke o potnem nalogu, prejemniku, stroskih...
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
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
			// baza je odprta

		/**
			* na zacetku zberemo podatke o potnem nalogu, izdajatelju potnega naloga
			* (podjetje, oseba), prejemniku potnega naloga ter opravljeni poti.
			**/

			// podatki o potnem nalogu
			QSqlQuery sql_potni_nalog;
			sql_potni_nalog.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_potni_nalog.exec();
			if ( sql_potni_nalog.next() ) {
				stevilka_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString());
				datum_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("datum_naloga")).toString());
				namen_potnega_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("namen_naloga")).toString());
				cena_dnevnice = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnic")).toString());
				prevozno_sredstvo = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prevozno_sredstvo")).toString());
				double dnevnice = 0;
				dnevnice = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_6_8")).toString()).toDouble();
				dnevnice += prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_8_12")).toString()).toDouble();
				dnevnice += prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_12_24")).toString()).toDouble();
				stevilo_dnevnic = QString::number(dnevnice, 'f', 0);
				stroski_skupaj = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stroski_skupaj")).toString());
				razdalja = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("skupaj_kilometri")).toString());
				kilometrina = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("kilometrina")).toString());
				kilometrina = kilometrina.remove(kilometrina.length() - 4, 4).replace(",", ".");

				// podatki o drugih stroskih
				QSqlQuery sql_stroski;
				sql_stroski.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(stevilka_naloga) + "'");
				sql_stroski.exec();
				double stroski = 0.0;
				while ( sql_stroski.next() ) {
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("strosek")).toString()) + ",";
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()) + ";";
					stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()).toDouble();
				}
				znesek_drugih_stroskov = QString::number(stroski, 'f', 2);

				// podatki o predlagatelju - podjetje
				QSqlQuery sql_predlagatelj_podjetje;
				sql_predlagatelj_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje")).toString() + "'");
				sql_predlagatelj_podjetje.exec();
				if ( sql_predlagatelj_podjetje.next() ) {
					predlagatelj_podjetje_ime = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("ime")).toString());
					predlagatelj_podjetje_polno_ime = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("polnoime")).toString());
					predlagatelj_podjetje_naslov = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("naslov")).toString());
					predlagatelj_podjetje_naslov_st = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("naslov_st")).toString());
					predlagatelj_podjetje_postna_st = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("postna_stevilka")).toString());
					predlagatelj_podjetje_posta = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("posta")).toString());
				}

				// podatki o predlagatelju - oseba
				QSqlQuery sql_predlagatelj_oseba;
				sql_predlagatelj_oseba.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba")).toString() + "'");
				sql_predlagatelj_oseba.exec();
				if ( sql_predlagatelj_oseba.next() ) {
					predlagatelj_oseba_ime = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("ime")).toString());
					predlagatelj_oseba_priimek = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("priimek")).toString());
					predlagatelj_oseba_naslov = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("naslov")).toString());
					predlagatelj_oseba_naslov_stevilka = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("naslov_stevilka")).toString());
					predlagatelj_oseba_postna_stevilka = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("postna_stevilka")).toString());
					predlagatelj_oseba_posta = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("posta")).toString());
				}

				// podatki o prejemniku
				QSqlQuery sql_prejemnik;
				sql_prejemnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba")).toString() + "'");
				sql_prejemnik.exec();
				if ( sql_prejemnik.next() ) {
					prejemnik_ime = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("ime")).toString());
					prejemnik_priimek = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("priimek")).toString());
					prejemnik_naziv = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naziv")).toString());
					prejemnik_naslov = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov")).toString());
					prejemnik_naslov_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov_stevilka")).toString());
					prejemnik_postna_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("postna_stevilka")).toString());
					prejemnik_posta = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("posta")).toString());
					prejemnik_znamka_avtomobila = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("avtomobil")).toString());
					prejemnik_model_avtomobila = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("model_avtomobila")).toString());
					prejemnik_registrska_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("registracija")).toString());
				}
				QSqlQuery sql_prejemnik_naziv;
				sql_prejemnik_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + pretvori(prejemnik_naziv) + "'");
				sql_prejemnik_naziv.exec();
				if ( sql_prejemnik_naziv.next() ) {
					prejemnik_naziv = prevedi(sql_prejemnik_naziv.value(sql_prejemnik_naziv.record().indexOf("naziv")).toString());
				}

				QSqlQuery sql_naloga;
				sql_naloga.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + pretvori(namen_potnega_naloga) + "'");
				sql_naloga.exec();
				if ( sql_naloga.next() ) {
					namen_potnega_naloga = prevedi(sql_naloga.value(sql_naloga.record().indexOf("namen")).toString());
				}

				QSqlQuery sql_prevoz;
				sql_prevoz.prepare("SELECT * FROM sif_prevozna_sredstva WHERE id LIKE '" + pretvori(prevozno_sredstvo) + "'");
				sql_prevoz.exec();
				if ( sql_prevoz.next() ) {
					prevozno_sredstvo = prevedi(sql_prevoz.value(sql_prevoz.record().indexOf("prevoz")).toString());
				}

				// podatki o potovanju
				QSqlQuery sql_pot;
				sql_pot.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString() + "'");
				sql_pot.exec();
				QString rel_start = "";
				QString rel_cilj = "";
				dt_datum_odhoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
				dt_datum_prihoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");

				while ( sql_pot.next() ) {

					// racunanje relacije potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString()) != rel_cilj ) {
						rel_start = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString());
					}
					rel_cilj = ""; // izpraznimo cilj, da se ne podvaja

					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString()) != rel_start ) {
						rel_cilj = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString());
					}
					if ( rel_start != "" && relacija != "" ) {
						rel_start = " - " + rel_start;
					}
					if ( rel_cilj != "" ) {
						rel_cilj = " - " + rel_cilj;
					}
					if ( relacija != "" ) {
						relacija = relacija + rel_cilj;
					}
					else {
						relacija = rel_start + rel_cilj;
					}
					rel_start = ""; // izpraznimo start, da se ne podvaja

					// racunanje koncnih tock potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()) != "" ) {
						potovanje = prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()).replace("\n", ", ") + "\n";
					}
			// uporabi samo, ce se bo pokazala potreba po odstranitvi zadnje vrstice
			// trenutno odstranjeno, saj ce zanji kraj nima vnesenega naslova, vseeno reze znake
			//		potovanje = potovanje.remove(potovanje.length() - 2, 2); // zadnji kraj ima tudi prelom v novo vrstico, ki pa tu ni potreben

					// racunanje podatkov o odhodu
					if ( dt_datum_odhoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_odhoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) < 0 ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_odhoda = dt_datum_odhoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					dt_trajanje = dt_datum_odhoda;
					ura_odhoda = datum_odhoda.right(5);
					datum_odhoda = datum_odhoda.left(datum_odhoda.length() - 6);

					if ( dt_datum_prihoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_prihoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) > 0 ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_prihoda = dt_datum_prihoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					ura_prihoda = datum_prihoda.right(5);
					datum_prihoda = datum_prihoda.left(datum_prihoda.length() - 6);
				}
				dnevi_potovanja = QString::number(dt_trajanje.daysTo(dt_datum_prihoda), 10);
				minute = (dt_trajanje.secsTo(dt_datum_prihoda) - dt_trajanje.daysTo(dt_datum_prihoda) * 24 * 60 * 60) / 60;
				ure = minute / 60;
				minute = minute - ure * 60;
			}
		}
		base.close();

	/**
		* Odpremo sifrant z besedilom potnega naloga
		* Besedilo bomo vztavljali z readLine
		**/

		QFile datoteka(app_path + "/potni-nalog-besedilo.csv");
		if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return;
		}
		QTextStream potni_nalog(&datoteka);

	/**
		* Nastavimo podatke o tiskanju. Tu tiskamo v PDF dokument
		* velikost A4, kar po obliki ustreza tiskanju v koncni razlicici, ko
		* se bo tiskalo tako v PDF dokument kot direktno na tiskalnik
		*/

	// podatki o printerju

	QPrinter printer;
	printer.setPaperSize(QPrinter::A4);
	printer.setResolution(300);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Natisni potni nalog"));

	if (dialog->exec() == QDialog::Accepted) {
		QPainter painter;

		if (! painter.begin(&printer))  { // failed to open file
			qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
			return;
		}

		double pozicija = 0;
		double visina_vrstice = 0;
		double sirina_besedila = 0;
		double razmik_med_vrsticami = 0;
		double faktor_razmika_med_vrsticami_1 = 0.3; // pri tabelah
		double faktor_razmika_med_vrsticami_2 = 0.0; // pri besedilu

		QString besedilo = "";

		QFont vstavljeno_besedilo("Arial", 10);
		QFont stalno_besedilo("Arial", 10, QFont::Bold);

	// narisemo glavo
		QPen *svincnik = new QPen;

		// dolocimo visino vrstice
		besedilo = "To je testno besedilo";
		painter.setFont(stalno_besedilo);
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsicami, za lazje branje dokumenta

		// crta zgoraj
		svincnik->setWidth(0.5);
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, 0, printer.width(), 0));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, printer.width() / 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_polno_ime;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, printer.width() / 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo odmik od crte
		pozicija = razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami + printer.width() / 2, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta pod naslovom naloga
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() / 2, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami + printer.width() / 2;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = stevilka_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dodamo okvir besedila
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, 0, 0, pozicija)); // crta levo
		painter.drawLine(QLine(printer.width() / 2, 0, printer.width() / 2, pozicija)); // crta sredina
		painter.drawLine(QLine(printer.width(), 0, printer.width(), pozicija)); // crta desno
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija)); // crta spodaj

	// podatki o prejemniku potnega naloga
		// nastavimo nov razmik med vrsticami
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_2;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_ime + " " + prejemnik_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naziv;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naslov + " " + prejemnik_naslov_stevilka + ", " + prejemnik_postna_stevilka + " " + prejemnik_posta;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potovanje;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = namen_potnega_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = "(" + prevozno_sredstvo + "): " + prejemnik_znamka_avtomobila + " " + prejemnik_model_avtomobila;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_polno_ime;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = cena_dnevnice.replace(".", ",") + " EUR";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + 15;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// manjka besedilo posebnih dodatkov, dokler ne izvemo, kaj to je...
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

	// tabela predujma
		int prvotna_visina = pozicija; // ohranimo zacetno visino za prvo in drugo tretjino
		int visina_sklopa = 0; // najprej narisemo desno tretjino, ki nosi najvec podatkov in je tudi najvisja

		// prva vrstica je prazna
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() * 2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis odredbodajalca)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dve prazni vrstici
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() * 2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis prejemnika)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo koncno visino
		visina_sklopa = pozicija;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo prvo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

	// glava obracuna potnih stroskov
		pozicija = visina_sklopa;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_1;
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() - razmik_med_vrsticami, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta sredina
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_ime + " " + predlagatelj_oseba_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_naslov + " " + predlagatelj_oseba_naslov_stevilka + ", " + predlagatelj_oseba_posta + " " + predlagatelj_oseba_postna_stevilka;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta sredina
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// crti levo in desno
		painter.drawLine(QLine(0, visina_sklopa, 0, pozicija)); // levo
		painter.drawLine(QLine(printer.width(), visina_sklopa, printer.width(), pozicija)); // desno

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += razmik_med_vrsticami;

	// obracun potnih stroskov
		// nastavitev zacetne pozicije
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_1;
		prvotna_visina = pozicija;
		double polje_1 = printer.width() * 7 / 8;
		double polje_2 = printer.width() / 8;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odsotnost)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(3 * polje_2 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dnevnice)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (cena dnevnic)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (skupaj)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 7 + sirina_besedila, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, polje_2 * 5, pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum odhoda)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila, pozicija, 5 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum prihoda)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = QString::number(ure, 10) + " ur " + QString::number(minute, 10) + " minut";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila, pozicija, 5 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dnevnice)
		besedilo = stevilo_dnevnic;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (cena dnevnic)
		besedilo = QString::number(cena_dnevnice.replace(",", ".").toDouble() / stevilo_dnevnic.toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (skupaj)
		besedilo = cena_dnevnice.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 7 + sirina_besedila, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// razmik med crtami
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// pokoncne crte
		painter.drawLine(QLine(3 * polje_2, prvotna_visina, 3 * polje_2, pozicija));
		painter.drawLine(QLine(5 * polje_2, prvotna_visina, 5 * polje_2, pozicija));
		painter.drawLine(QLine(6 * polje_2, prvotna_visina, 6 * polje_2, pozicija));

		prvotna_visina += visina_vrstice + razmik_med_vrsticami * 2;

		painter.drawLine(QLine(1 * polje_2, prvotna_visina, 1 * polje_2, pozicija));
		painter.drawLine(QLine(2 * polje_2, prvotna_visina, 2 * polje_2, pozicija));
		painter.drawLine(QLine(4 * polje_2, prvotna_visina, 4 * polje_2, pozicija));

		// vrnitev prvotne pozicije
		prvotna_visina = prvotna_visina - visina_vrstice - razmik_med_vrsticami * 2 + 1;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (% zvisanja dnevnic)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = zvisanje_dnevnic;
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = cena_dnevnice.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (prevozni stroski)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 * 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo (št. km)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = razdalja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (km x )
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = kilometrina;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (EUR)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo (relacija)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = relacija;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo (registracija)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_registrska_stevilka;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(kilometrina.toDouble() * razdalja.toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (drugi stroski)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, polje_2 * 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// za vsak strosek dodamo novo vrstico
		if ( ostali_stroski != "" ) {
			do {
				QString strosek = ostali_stroski.left(ostali_stroski.indexOf(";", 0));
				ostali_stroski = ostali_stroski.right(ostali_stroski.length() - ostali_stroski.indexOf(";", 0) - 1);
				QString ime_stroska = strosek.left(strosek.indexOf(",", 0));
				QString vrednost_stroska = strosek.right(strosek.length() - strosek.indexOf(",", 0) - 1).replace(".", ",") + " EUR";
				// nastavimo tip pisave
				painter.setFont(vstavljeno_besedilo);
				// narisemo besedilo
				painter.drawText(polje_2 * 2, pozicija, polje_2 * 6, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, ime_stroska);
				// narisemo besedilo
				painter.drawText(polje_2 * 6, pozicija, polje_2 * 7, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, vrednost_stroska);
				// nastavimo razmik med crto
				pozicija += visina_vrstice + visina_vrstice * faktor_razmika_med_vrsticami_2;
			} while ( ostali_stroski.indexOf(";", 0) != -1 );
			// postavimo pozicijo na prejsnjo vrstico
			pozicija -= visina_vrstice - visina_vrstice * faktor_razmika_med_vrsticami_2;

			// nastavimo besedilo
			besedilo = znesek_drugih_stroskov.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = polje_1 + razmik_med_vrsticami;
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// narisemo besedilo
			painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		}

		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj v znesku)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, polje_1, 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnice.replace(",", ".").toDouble() + razdalja.toDouble() * kilometrina.toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (ostane za izplacilo)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnice.replace(",", ".").toDouble() + razdalja.toDouble() * kilometrina.toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// pokoncne crte
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, prvotna_visina, 0, pozicija)); // levo
		painter.drawLine(QLine(polje_1, prvotna_visina, polje_1, pozicija)); // sredina
		painter.drawLine(QLine(printer.width(), prvotna_visina, printer.width(), pozicija)); // desno

	// noga
		// nastavimo nov razmik
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_2;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += razmik_med_vrsticami;

		// zapomnimo si zgornjo visino
		prvotna_visina = pozicija;

		// leva tretjina
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_posta + " ";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Likvidator)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Blagajnik)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo tretjo tretjino
		pozicija = prvotna_visina;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Predlagatelj racuna)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj

		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odredbodajalec)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo odmik od crte
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Prejemnik)
		besedilo = potni_nalog.readLine();
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		painter.end();

	}

}

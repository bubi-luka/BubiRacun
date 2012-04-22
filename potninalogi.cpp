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
#include "tiskanje.h"

potninalogi::potninalogi(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::potninalogi)
{
		ui->setupUi(this);

		// pocisti polja privzetih vrednosti
		ui->txt_id->setText("");
		ui->txt_stevilka_naloga->setText("");
		ui->txt_stevilka_dokumenta->setText("");
		ui->txt_datum_naloga->setDate(QDate::currentDate());
		ui->txt_stevilka_projekta->clear();
		ui->txt_opombe->setPlainText("");
		ui->txt_cena_prevoza->setText("");
		ui->txt_cena_dnevnic->setText("");
		ui->txt_ostali_stroski->setText("");
		ui->txt_stroski_skupaj->setText("");
		ui->txt_priloge->clear();

		ui->txt_skupaj_kilometri->setText("");
		ui->txt_kilometrina->setText("");
		ui->txt_skupaj_dnevi->setText("");
		ui->txt_skupaj_ure->setText("");
		ui->txt_priznana_dnevnica->setText("");
		ui->txt_cena_dnevnice_6_8->setText("");
		ui->txt_cena_dnevnice_8_12->setText("");
		ui->txt_cena_dnevnice_12_24->setText("");
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
		ui->txt_id->setEnabled(false);
//		ui->txt_stevilka_naloga->setEnabled(false);

		ui->txt_cena_prevoza->setEnabled(false);
		ui->txt_cena_dnevnic->setEnabled(false);
		ui->txt_ostali_stroski->setEnabled(false);
		ui->txt_stroski_skupaj->setEnabled(false);

		ui->txt_skupaj_kilometri->setEnabled(false);
		ui->txt_kilometrina->setEnabled(false);
		ui->txt_skupaj_dnevi->setEnabled(false);
		ui->txt_skupaj_ure->setEnabled(false);
		ui->txt_cena_dnevnice_6_8->setEnabled(false);
		ui->txt_cena_dnevnice_8_12->setEnabled(false);
		ui->txt_cena_dnevnice_12_24->setEnabled(false);
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
		ui->txt_priznana_dnevnica->setChecked(false);
		ui->cb_predlagatelj_oseba->setChecked(false);

		// skrij neuporabljena polja
		ui->txt_predlagatelj_izbira_oseba->setHidden(true);

		ui->label_19->setVisible(false);
		ui->gb_dnevnica->setVisible(false);

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

			QSqlQuery sql_dnevnice;
			sql_dnevnice.prepare("SELECT * FROM sif_dnevnice");
			sql_dnevnice.exec();
			while ( sql_dnevnice.next() ) {
				ui->txt_cena_dnevnice_6_8->setText(pretvori_iz_double(prevedi(sql_dnevnice.value(sql_dnevnice.record().indexOf("dnevnica_1")).toString())));
				ui->txt_cena_dnevnice_8_12->setText(pretvori_iz_double(prevedi(sql_dnevnice.value(sql_dnevnice.record().indexOf("dnevnica_2")).toString())));
				ui->txt_cena_dnevnice_12_24->setText(pretvori_iz_double(prevedi(sql_dnevnice.value(sql_dnevnice.record().indexOf("dnevnica_3")).toString())));
			}

			QSqlQuery sql_kilometrina;
			sql_kilometrina.prepare("SELECT * FROM sif_kilometrina");
			sql_kilometrina.exec();
			while ( sql_kilometrina.next() ) {
				ui->txt_kilometrina->setText(prevedi(sql_kilometrina.value(sql_kilometrina.record().indexOf("kilometrina")).toString()).replace(".", ",") + " km");
			}

			// dodaj prazno vrstico spustnim seznamom
			//	ui->txt_stevilka_projekta->addItem("Prosim, izberite prejemnika");
			ui->txt_predlagatelj_izbira_oseba->addItem("");
			ui->txt_prejemnik_izbira_osebe->addItem("");
			ui->txt_prevoz->addItem("");

			// napolni spustne sezname
			QSqlQuery sql_fill_combo;

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

		stevilka_racuna();

		ui->tab_potni_stroski->setCurrentIndex(0);

}

potninalogi::~potninalogi()
{
		delete ui;
}

void potninalogi::on_btn_izhod_clicked() {

	close();

}

void potninalogi::on_btn_izvozi_clicked() {

	// shranimo podatke
	on_btn_sprejmi_clicked();

	// posljemo v tisk
	tiskanje *print = new tiskanje;
	print->show();
	QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
				 print , SLOT(prejem(QString, QString, QString)));
	tiskaj("potni-nalogi", ui->txt_id->text() + ",", "tisk"); // vrsta, id stevilke, format

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

			sql_vnesi_uporabnika.prepare("INSERT INTO potni_nalogi (stevilka_naloga, datum_naloga, "
																	 "stevilka_projekta, opombe, cena_prevoza, cena_dnevnic, ostali_stroski, stroski_skupaj, skupaj_kilometri, "
																	 "kilometrina, skupaj_dnevi, skupaj_ure, priznana_dnevnica, cena_dnevnice_6_8, cena_dnevnice_8_12, cena_dnevnice_12_24, dnevnica_6_8, "
																	 "dnevnica_8_12, dnevnica_12_24, zajtrk_8_12, zajtrk_12_24, predlagatelj_podjetje, "
																	 "predlagatelj_oseba, prejemnik_oseba, prevozno_sredstvo, priloge, stevilka_dokumenta) VALUES "
																	 "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_uporabnika.prepare("UPDATE potni_nalogi SET stevilka_naloga = ?, datum_naloga = ?, stevilka_projekta = ?, opombe = ?, cena_prevoza = ?, cena_dnevnic = ?, "
																		 "ostali_stroski = ?, stroski_skupaj = ?, skupaj_kilometri = ?, kilometrina = ?, skupaj_dnevi = ?, "
																		 "skupaj_ure = ?, priznana_dnevnica = ?, cena_dnevnice_6_8 = ?, cena_dnevnice_8_12 = ?, cena_dnevnice_12_24 = ?, "
																		 "dnevnica_6_8 = ?, dnevnica_8_12 = ?, dnevnica_12_24 = ?, zajtrk_8_12 = ?, zajtrk_12_24 = ?, predlagatelj_podjetje = ?, "
																		 "predlagatelj_oseba = ?, prejemnik_oseba = ?, prevozno_sredstvo = ?, priloge = ?, stevilka_dokumenta = ? WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			}
			sql_vnesi_uporabnika.bindValue(0, pretvori(ui->txt_stevilka_naloga->text()));
			sql_vnesi_uporabnika.bindValue(1, pretvori(ui->txt_datum_naloga->text()));
			QString indeks = prevedi(ui->txt_stevilka_projekta->currentText());
			indeks = indeks.left(indeks.indexOf(") ", 0));
			indeks = pretvori(indeks);
			sql_vnesi_uporabnika.bindValue(2, indeks);
			sql_vnesi_uporabnika.bindValue(3, pretvori(ui->txt_opombe->toPlainText()));
			sql_vnesi_uporabnika.bindValue(4, pretvori(pretvori_v_double(ui->txt_cena_prevoza->text())));
			sql_vnesi_uporabnika.bindValue(5, pretvori(pretvori_v_double(ui->txt_cena_dnevnic->text())));
			sql_vnesi_uporabnika.bindValue(6, pretvori(pretvori_v_double(ui->txt_ostali_stroski->text())));
			sql_vnesi_uporabnika.bindValue(7, pretvori(pretvori_v_double(ui->txt_stroski_skupaj->text())));
			sql_vnesi_uporabnika.bindValue(8, pretvori(pretvori_v_double(ui->txt_skupaj_kilometri->text())));
			sql_vnesi_uporabnika.bindValue(9, pretvori(pretvori_v_double(ui->txt_kilometrina->text())));
			sql_vnesi_uporabnika.bindValue(10, pretvori(ui->txt_skupaj_dnevi->text()));
			sql_vnesi_uporabnika.bindValue(11, pretvori(ui->txt_skupaj_ure->text()));
			if ( ui->txt_priznana_dnevnica->isChecked() ) {
				sql_vnesi_uporabnika.bindValue(12, pretvori("1"));
			}
			else {
				sql_vnesi_uporabnika.bindValue(12, pretvori("0"));
			}
			sql_vnesi_uporabnika.bindValue(13, pretvori(pretvori_v_double(ui->txt_cena_dnevnice_6_8->text())));
			sql_vnesi_uporabnika.bindValue(14, pretvori(pretvori_v_double(ui->txt_cena_dnevnice_8_12->text())));
			sql_vnesi_uporabnika.bindValue(15, pretvori(pretvori_v_double(ui->txt_cena_dnevnice_12_24->text())));
			sql_vnesi_uporabnika.bindValue(16, pretvori(pretvori_v_double(ui->txt_dnevnica_6_8->text())));
			sql_vnesi_uporabnika.bindValue(17, pretvori(pretvori_v_double(ui->txt_dnevnica_8_12->text())));
			sql_vnesi_uporabnika.bindValue(18, pretvori(pretvori_v_double(ui->txt_dnevnica_12_24->text())));
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

			sql_vnesi_uporabnika.bindValue(21, vApp->firm());

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
			sql_vnesi_uporabnika.bindValue(25, pretvori(ui->txt_priloge->toPlainText()));
			sql_vnesi_uporabnika.bindValue(26, pretvori(ui->txt_stevilka_dokumenta->text()));
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
		ui->txt_datum_naloga->setEnabled(true);
	}
	else {
		ui->btn_sprejmi->setText("Polnim");
		ui->btn_izvozi->setEnabled(true);
		ui->wid_pot->setEnabled(true);
		ui->wid_st->setEnabled(true);
		ui->txt_datum_naloga->setEnabled(false);
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
				ui->txt_stevilka_dokumenta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_dokumenta")).toString()));
				ui->txt_datum_naloga->setDate(QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_naloga")).toString()), "dd'.'MM'.'yyyy"));
				ui->txt_opombe->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opombe")).toString()));
				ui->txt_cena_prevoza->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_prevoza")).toString())));
				ui->txt_cena_dnevnic->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnic")).toString())));
				ui->txt_ostali_stroski->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ostali_stroski")).toString())));
				ui->txt_stroski_skupaj->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stroski_skupaj")).toString())));
				ui->txt_priloge->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("priloge")).toString()));
				ui->txt_skupaj_kilometri->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("skupaj_kilometri")).toString()).replace(".", ",") + " km");
				ui->txt_kilometrina->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kilometrina")).toString())));
				ui->txt_skupaj_dnevi->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("skupaj_dnevi")).toString()));
				ui->txt_skupaj_ure->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("skupaj_ure")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("priznana_dnevnica")).toString()) == "1" ) {
					ui->txt_priznana_dnevnica->setChecked(true);
				}
				else {
					ui->txt_priznana_dnevnica->setChecked(false);
				}

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnice_6_8")).toString()) != "" ) {
					ui->txt_cena_dnevnice_6_8->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnice_6_8")).toString()));
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnice_8_12")).toString()) != "" ) {
					ui->txt_cena_dnevnice_8_12->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnice_8_12")).toString()));
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnice_12_24")).toString()) != "" ) {
					ui->txt_cena_dnevnice_12_24->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_dnevnice_12_24")).toString()));
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

		ui->btn_sprejmi->setText("Popravi potni nalog");
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

		ui->txt_skupaj_kilometri->setText(QString::number(kilometri, 'f', 3).replace(".", ",") + " km");
		double kilometrina = pretvori_v_double(ui->txt_kilometrina->text()).toDouble();
		cenaprevoza = kilometri * kilometrina;
		ui->txt_cena_prevoza->setText(pretvori_iz_double(QString::number(cenaprevoza, 'f', 2)));

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
			ui->label_19->setVisible(true);
			ui->gb_dnevnica->setVisible(true);
		}
		else if ( ure >= 8 ) {
			ui->txt_dnevnica_8_12->setText("1");
			ui->label_19->setVisible(true);
			ui->gb_dnevnica->setVisible(true);
		}
		else if ( ure >= 6 ) {
			ui->txt_dnevnica_6_8->setText("1");
			ui->label_19->setVisible(true);
			ui->gb_dnevnica->setVisible(true);
		}
		else {
			ui->label_19->setVisible(false);
			ui->gb_dnevnica->setVisible(false);
		}

		// izracun dnevnic
		if ( ui->txt_priznana_dnevnica->isChecked() ) {
			double cenadnevnice = 0;
			cenadnevnice = ui->txt_cena_dnevnice_6_8->text().toDouble() * ui->txt_dnevnica_6_8->text().toDouble();
			if ( ui->txt_zajtrk_8_12->isChecked() ) {
				cenadnevnice = cenadnevnice + ui->txt_cena_dnevnice_8_12->text().toDouble() * ui->txt_dnevnica_8_12->text().toDouble() * 0.85;
			}
			else {
				cenadnevnice = cenadnevnice + ui->txt_cena_dnevnice_8_12->text().toDouble() * ui->txt_dnevnica_8_12->text().toDouble();
			}
			if ( ui->txt_zajtrk_12_24->isChecked() ) {
				cenadnevnice = cenadnevnice + ui->txt_cena_dnevnice_12_24->text().toDouble() * ui->txt_dnevnica_12_24->text().toDouble() * 0.90;
			}
			else {
				cenadnevnice = cenadnevnice + ui->txt_cena_dnevnice_12_24->text().toDouble() * ui->txt_dnevnica_12_24->text().toDouble();
			}
			ui->txt_cena_dnevnic->setText(QString::number(cenadnevnice, 'f', 2));
		}
		else {
			ui->txt_cena_dnevnic->setText("0.00");
		}
		ui->txt_cena_dnevnic->setText(pretvori_iz_double(ui->txt_cena_dnevnic->text()));

		// ostali stroski
		QSqlQuery sql_ostalo;
		sql_ostalo.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(ui->txt_stevilka_naloga->text()) + "'");
		sql_ostalo.exec();
		while ( sql_ostalo.next() ) {
			ostalo = ostalo + prevedi(sql_ostalo.value(sql_ostalo.record().indexOf("cena")).toString()).replace(";", ".").toDouble();
		}
		ui->txt_ostali_stroski->setText(pretvori_iz_double(QString::number(ostalo, 'f', 2)));

		// stroski skupaj
		ui->txt_stroski_skupaj->setText(pretvori_iz_double(QString::number(
																												 pretvori_v_double(ui->txt_cena_prevoza->text()).toDouble() +
																												 pretvori_v_double(ui->txt_cena_dnevnic->text()).toDouble() +
																												 pretvori_v_double(ui->txt_ostali_stroski->text()).toDouble(), 'f', 2)));
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

	if ( ui->txt_priznana_dnevnica->isChecked() ) {
		ui->label_19->setVisible(true);
		ui->gb_dnevnica->setVisible(true);
	}
	else {
		ui->label_19->setVisible(false);
		ui->gb_dnevnica->setVisible(false);
	}

}

void potninalogi::on_txt_zajtrk_8_12_toggled() {

	izracun();

}

void potninalogi::on_txt_zajtrk_12_24_toggled() {

	izracun();

}

QString potninalogi::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	return besedilo;

}

QString potninalogi::pretvori_iz_double(QString besedilo) {

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

void potninalogi::on_txt_datum_naloga_dateChanged() {

	stevilka_racuna();

}

void potninalogi::stevilka_racuna() {

	if ( ui->btn_sprejmi->text() != "Polnim" ) { // da ne pade v neskoncno zanko
	QString leto = ui->txt_datum_naloga->text().right(4);

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

			bool tvori = true; // tvorimo stevilko naloga
			QString stara_stevilka_naloga = "";
			if ( ui->txt_id->text() != "") {
				QSqlQuery sql_stara_stevilka;
				sql_stara_stevilka.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
				sql_stara_stevilka.exec();
				if ( sql_stara_stevilka.next() ) {
					stara_stevilka_naloga = prevedi(sql_stara_stevilka.value(sql_stara_stevilka.record().indexOf("stevilka_naloga")).toString());
					if ( stara_stevilka_naloga.left(7).right(4) == leto ) {
						ui->txt_stevilka_naloga->setText(stara_stevilka_naloga);
						tvori = false;
					}
				}
			}

			// zapisi stevilko potnega naloga
			if ( tvori == true ) {
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
			}
		}
		base.close();
	}

}

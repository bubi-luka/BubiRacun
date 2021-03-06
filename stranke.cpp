#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "stranke.h"
#include "ui_stranke.h"
#include "projekti.h"
#include "kodiranje.h"
#include "varnost.h"

stranke::stranke(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::stranke)
{
	ui->setupUi(this);

		// izprazni vnosna polja
		ui->txt_id->setText("");
		ui->txt_ime->setText("");
		ui->txt_priimek->setText("");
		ui->txt_naslov->setText("");
		ui->txt_naslov_st->setText("");
		ui->txt_posta->clear();
		ui->txt_postna_stevilka->setText("");
		ui->cb_davcni_zavezanec->setText("");
		ui->txt_davcna->setText("");
		ui->txt_kontaktna->setText("");
		ui->txt_telefon->setText("");
		ui->txt_gsm->setText("");
		ui->txt_email->setText("");
		ui->txt_spletna_stran->setText("");
		ui->txt_ustanova->setText("");
		ui->txt_opomba->clear();

		ui->cb_popust_fb1->setText("");
		ui->cb_popust_fb2->setText("");
		ui->cb_popust_komb1->setText("");
		ui->cb_popust_komb2->setText("");
		ui->cb_popust_stalna->setText("");
		ui->cb_popust_akcija->setText("");
		ui->cb_podrazitev_vikend->setText("");
		ui->cb_podrazitev_hitrost->setText("");
		ui->cb_podrazitev_zapleti->setText("");
		ui->txt_popust_fb1->setText("");
		ui->txt_popust_fb2->setText("");
		ui->txt_popust_komb1->setText("");
		ui->txt_popust_komb2->setText("");
		ui->txt_popust_stalna_stranka->setText("");
		ui->txt_popust_akcija->setText("");
		ui->txt_vsi_popusti_facebook_1->setText("");
		ui->txt_vsi_popusti_facebook_2->setText("");
		ui->txt_popusti_skupaj_1->setText("");
		ui->txt_popusti_skupaj_2->setText("");
		ui->txt_podrazitev_vikend->setText("");
		ui->txt_podrazitev_hitrost->setText("");
		ui->txt_podrazitev_zapleti->setText("");

		ui->tbl_projekti->clear();
		ui->cb_leto->clear();
		ui->cb_mesec->clear();
		ui->cb_projekt->clear();

		/*
			* Nastavimo validacijo polj
			* Polja vsebujejo eno ali dve celi stevilki, sledi vejica ali pika,
			* sledi ena stevilka, nato je lahko presledek in znak za odstotek
			* ali pa nic.
		*/
			QRegExp rx_procent("\\d{0,2}([(.|,)][(0-9)]){0,1}[ ]?[%]?");
			QValidator *v_procent = new QRegExpValidator(rx_procent, this);

			ui->txt_popust_fb1->setValidator(v_procent);
			ui->txt_popust_fb2->setValidator(v_procent);
			ui->txt_popust_komb1->setValidator(v_procent);
			ui->txt_popust_komb2->setValidator(v_procent);
			ui->txt_popust_stalna_stranka->setValidator(v_procent);
			ui->txt_popust_akcija->setValidator(v_procent);

			ui->txt_vsi_popusti_facebook_1->setValidator(v_procent);
			ui->txt_vsi_popusti_facebook_2->setValidator(v_procent);
			ui->txt_popusti_skupaj_1->setValidator(v_procent);
			ui->txt_popusti_skupaj_2->setValidator(v_procent);

			ui->txt_podrazitev_vikend->setValidator(v_procent);
			ui->txt_podrazitev_hitrost->setValidator(v_procent);
			ui->txt_podrazitev_zapleti->setValidator(v_procent);

		// set name labels
		ui->label->setText("Ime");
		ui->label_2->setText("Priimek");

		// set visible and enabled fields
		ui->txt_id->setDisabled(true);
		ui->txt_kontaktna->setVisible(false);
		ui->label_8->setVisible(false);
		ui->label_47->setVisible(false);
		ui->cb_davcni_zavezanec->setVisible(false);
		ui->txt_davcna->setVisible(false);
		ui->label_4->setVisible(false);
		ui->label_11->setVisible(true);
		ui->txt_ustanova->setVisible(true);
		ui->txt_vsi_popusti_facebook_1->setDisabled(true);
		ui->txt_vsi_popusti_facebook_2->setDisabled(true);
		ui->txt_popusti_skupaj_1->setDisabled(true);
		ui->txt_popusti_skupaj_2->setDisabled(true);
		ui->cb_podrazitev_vikend->setChecked(false);
		ui->txt_popust_fb1->setDisabled(true);
		ui->txt_popust_fb2->setDisabled(true);
		ui->txt_popust_komb1->setDisabled(true);
		ui->txt_popust_komb2->setDisabled(true);
		ui->txt_popust_stalna_stranka->setDisabled(true);
		ui->txt_popust_akcija->setDisabled(true);
		ui->cb_podrazitev_hitrost->setChecked(false);
		ui->cb_podrazitev_zapleti->setChecked(false);
		ui->txt_podrazitev_vikend->setEnabled(false);
		ui->txt_podrazitev_hitrost->setEnabled(false);
		ui->txt_podrazitev_zapleti->setEnabled(false);

		ui->btn_brisi->setEnabled(false);
		ui->btn_brisi->setVisible(false);

		//set input mask for phone fields
		ui->txt_gsm->setInputMask("+990 (\\0)99/999-999;_");
		ui->txt_telefon->setInputMask("+990 (\\0)9/99-99-999;_");

		// fill combo box and tables

			// fill combo box
			QSqlQuery sql_fill_combo;
			sql_fill_combo.prepare("SELECT * FROM sif_posta");
			sql_fill_combo.exec();
			ui->txt_posta->addItem("");
			while ( sql_fill_combo.next() ) {
				ui->txt_posta->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("posta")).toString()));
			}
			sql_fill_combo.clear();

			sql_fill_combo.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_vikend") + "'");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_podrazitev_vikend->setText(pretvori_iz_double(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("vrednost")).toString())));
			}
			sql_fill_combo.clear();
			sql_fill_combo.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_hitrost") + "'");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("vrednost")).toString())));
			}
			sql_fill_combo.clear();
			sql_fill_combo.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_zapleti") + "'");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("vrednost")).toString())));
			}
			sql_fill_combo.clear();
			sql_fill_combo.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("max_pop_facebook") + "'");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_vsi_popusti_facebook_1->setText(pretvori_iz_double(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("vrednost")).toString())));
			}
			sql_fill_combo.clear();
			sql_fill_combo.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("max_pop_vsi") + "'");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_popusti_skupaj_1->setText(pretvori_iz_double(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("vrednost")).toString())));
			}
			sql_fill_combo.clear();

			ui->txt_banka->addItem("");
			sql_fill_combo.prepare("SELECT * FROM sif_banke ORDER BY ime_banke ASC");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_banka->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime_banke")).toString()));
			}
			sql_fill_combo.clear();

			// napolni spustne sezname pri projektih - filtriranje
			// zaradi nepotrebnih filtriranj tabele spremenimo napis na enem od gumbov
			QString besedilo = ui->btn_novprojekt->text();
			ui->btn_novprojekt->setText("");

			ui->cb_mesec->addItem("");
			ui->cb_mesec->addItem("01) Januar");
			ui->cb_mesec->addItem("02) Februar");
			ui->cb_mesec->addItem("03) Marec");
			ui->cb_mesec->addItem("04) April");
			ui->cb_mesec->addItem("05) Maj");
			ui->cb_mesec->addItem("06) Junij");
			ui->cb_mesec->addItem("07) Julij");
			ui->cb_mesec->addItem("08) Avgust");
			ui->cb_mesec->addItem("09) September");
			ui->cb_mesec->addItem("10) Oktober");
			ui->cb_mesec->addItem("11) November");
			ui->cb_mesec->addItem("12) December");

			QSqlQuery sql_napolni;
			ui->cb_projekt->addItem("");
			sql_napolni.prepare("SELECT * FROM sif_status_projekta");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_projekt->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()) + ") " +
																prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString()));
			}

			ui->btn_novprojekt->setText(besedilo);

		napolni_projekte();

		ui->tab_stranka->setCurrentIndex(0);

}

stranke::~stranke()
{
	delete ui;
}

void stranke::on_cb_mesec_currentIndexChanged() {

	if ( ui->btn_novprojekt->text() != "" ) {
		napolni_projekte();
	}

}

void stranke::on_cb_leto_currentIndexChanged() {

	if ( ui->btn_novprojekt->text() != "" ) {
		napolni_projekte();
	}

}

void stranke::on_cb_projekt_currentIndexChanged() {

	if ( ui->btn_novprojekt->text() != "" ) {
		napolni_projekte();
	}

}

void stranke::napolni_projekte() {


	QString stavek = "";

	if ( ui->cb_projekt->currentText() != "" ) {
		stavek += " AND status_projekta LIKE '" + pretvori(ui->cb_projekt->currentText().left(ui->cb_projekt->currentText().indexOf(") ", 0))) + "'";
	}

		// clear previous content
		ui->tbl_projekti->clear();

		for (int i = 0; i <= 5; i++) {
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

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("St. projekta");
		naslov2->setText("Ime projekt");
		naslov3->setText("Datum pricetka");
		naslov4->setText("Datum zakljucka");
		naslov5->setText("Status projekta");

		ui->tbl_projekti->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_projekti->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_projekti->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_projekti->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_projekti->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_projekti->setHorizontalHeaderItem(5, naslov5);

		ui->tbl_projekti->setColumnWidth(0, 35);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM projekti WHERE avtor_oseba LIKE '" + pretvori(vApp->id()) + "' AND stranka LIKE '" + pretvori(ui->txt_id->text()) + "'" + stavek);
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {

			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("pricetek_dela")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("pricetek_dela")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("pricetek_dela")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("pricetek_dela")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}

			if ( filter == "pozitivno" ) {
				ui->tbl_projekti->insertRow(row);
				ui->tbl_projekti->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[6] = {"id", "stevilka_projekta", "naslov_projekta", "pricetek_dela", "konec_dela", "status_projekta"};

				while (col <= 5) {

					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[i] == "status_projekta" ) {
						QSqlQuery sql_status;
						sql_status.prepare("SELECT * FROM sif_status_projekta WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
						sql_status.exec();
						if ( sql_status.next() ) {
							celica->setText(prevedi(sql_status.value(sql_status.record().indexOf("status")).toString()));
						}
						else {
							celica->setText("Neopredeljeno");
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

}

void stranke::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_vnesi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}
	else if ((event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::AltModifier))
	{
		this->on_btn_brisi_clicked();
	}
}

void stranke::on_rb_fizicna_toggled(bool stanje) {

	if (stanje == true) { // fizicna oseba
		ui->label->setText("Ime");
		ui->label_2->setText("Priimek");
		ui->label_8->setHidden(true);
		ui->txt_kontaktna->setHidden(true);
		ui->label_11->setHidden(false);
		ui->txt_ustanova->setHidden(false);
		ui->label_47->setHidden(true);
		ui->cb_davcni_zavezanec->setHidden(true);
		ui->txt_davcna->setHidden(true);
		ui->label_4->setHidden(true);
	}
	else { // pravna oseba
		ui->label->setText("Naziv");
		ui->label_2->setText("Polni naziv");
		ui->label_8->setHidden(false);
		ui->txt_kontaktna->setHidden(false);
		ui->label_11->setHidden(true);
		ui->txt_ustanova->setHidden(true);
		ui->label_47->setHidden(false);
		ui->cb_davcni_zavezanec->setHidden(false);
		ui->txt_davcna->setHidden(false);
		ui->label_4->setHidden(false);
	}

}

void stranke::on_rb_stalna_toggled(bool stanje)  {

	if ( stanje == true) {
		ui->cb_popust_stalna->setChecked(true);
	}
	else {
		ui->cb_popust_stalna->setChecked(false);
	}

}

void stranke::on_tbl_projekti_doubleClicked() {

	projekti *uredi = new projekti;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_projekti->selectedItems().takeAt(1)->text()); // ce projekt obstaja, poslji naprej st. projekta
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void stranke::on_btn_novprojekt_clicked() {

	projekti *uredi = new projekti;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov projekt" + ui->txt_id->text()); // ce projekt ne obstaja, poslji naprej st. uporabnika
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void stranke::on_txt_posta_currentIndexChanged(QString besedilo) {

		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_posta WHERE posta LIKE '" + pretvori(besedilo) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_postna_stevilka->setText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("postna_stevilka")).toString()));
		}

}

void stranke::on_txt_postna_stevilka_textChanged(QString besedilo) {

		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_posta WHERE postna_stevilka LIKE '" + pretvori(besedilo) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_posta->setCurrentIndex(ui->txt_posta->findText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("posta")).toString())));
		}

}

// prazno, dokler ne definiramo funkcije gumba
void stranke::on_btn_brisi_clicked() {

}

void stranke::on_btn_izhod_clicked() {

	close();

}

// ne preverja vnesenih polj
void stranke::on_btn_vnesi_clicked() {

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

	// preveri polja
		if (ui->txt_ime->text() == "") {
			ui->label->setPalette(palette_error);
			ui->label->setFont(font_error);
			napaka = "true";
			ui->txt_ime->setFocus();
		}
		else {
			QSqlQuery sql_preveri_ime;
			sql_preveri_ime.prepare("SELECT * FROM projekti WHERE ime LIKE '" + pretvori(ui->txt_ime->text()) + "'");
			sql_preveri_ime.exec();
			if (sql_preveri_ime.next()) {
				if (ui->txt_priimek->text() == "") {
					ui->label_2->setPalette(palette_error);
					ui->label_2->setFont(font_error);
					napaka = "true";
					ui->txt_priimek->setFocus();
				}
				else  if ( ui->txt_priimek->text() == prevedi(sql_preveri_ime.value(sql_preveri_ime.record().indexOf("priimek")).toString())) { // ze obstaja v bazi
					ui->label_2->setPalette(palette_error);
					ui->label_2->setFont(font_error);
					napaka = "true";
					ui->txt_ime->setFocus();
				}
				else {
					ui->label_2->setPalette(palette_normal);
					ui->label_2->setFont(font_normal);
				}
			}
			else {
				ui->label_2->setPalette(palette_normal);
				ui->label_2->setFont(font_normal);
			}
		}

	// preveri davcno stevilko
	if (ui->rb_pravna->isChecked() && ui->txt_davcna->text() != "") {
		if (ui->txt_davcna->text().length() < 8) { // dolzina
			ui->label_4->setPalette(palette_error);
			ui->label_4->setFont(font_error);
			napaka = "true";
			ui->txt_davcna->setFocus();
		}
		else { // pravilnost zapisa
			QString tax = ui->txt_davcna->text();
			QString tax_error = "";
			bool ok;

			int i_sum = 8*tax.left(1).toInt(&ok, 10) +
						7*tax.left(2).right(1).toInt(&ok, 10) +
						6*tax.left(3).right(1).toInt(&ok, 10) +
						5*tax.left(4).right(1).toInt(&ok, 10) +
						4*tax.left(5).right(1).toInt(&ok, 10) +
						3*tax.left(6).right(1).toInt(&ok, 10) +
						2*tax.left(7).right(1).toInt(&ok, 10);
			int i_mod = i_sum % 11;
			int i_control = 11 - i_mod;
			if (i_control == 10) {
				i_control = 0;
			}
			if (i_control == 11) {
				tax_error = "Error";
			}

			if (i_control != tax.right(1).toInt(&ok, 10)) {
				tax_error = "Error";
			}

			if (tax_error == "Error") {
				ui->label_4->setPalette(palette_error);
				ui->label_4->setFont(font_error);
				napaka = "true";
				ui->txt_davcna->setFocus();
			}
			else {
				ui->label_4->setPalette(palette_normal);
				ui->label_4->setFont(font_normal);
			}
		}
	}
	else {
		ui->label_4->setPalette(palette_normal);
		ui->label_4->setFont(font_normal);
	}

	// preveri email naslov
	ui->txt_email->setText(ui->txt_email->text().toLower());
	QRegExp rx_email("^[a-z0-9\\-\\_\\.]*[@][a-z0-9\\-\\_\\.]*[\\.][a-z]{2,3}$");

	if (!rx_email.exactMatch(ui->txt_email->text())) {
		ui->label_7->setPalette(palette_error);
		ui->label_7->setFont(font_error);
		napaka = "true";
		ui->txt_email->setFocus();
	}
	else {
		ui->label_7->setPalette(palette_normal);
		ui->label_7->setFont(font_normal);
	}

	// preveri url
	if (ui->txt_spletnastran->text() != "") {
		if (!(ui->txt_spletnastran->text().startsWith("http") || ui->txt_spletnastran->text().contains("://"))) {
			ui->txt_spletnastran->setText("http://" + ui->txt_spletnastran->text());
		}
		if (!(ui->txt_spletnastran->text().endsWith("/") || ui->txt_spletnastran->text().endsWith(".htm") || ui->txt_spletnastran->text().endsWith(".html"))) {
			ui->txt_spletnastran->setText(ui->txt_spletnastran->text().append("/"));
		}
	}

	// ponovno preveri obvezno izpolnjena polja
	if (ui->txt_ime->text() == "") {
		napaka = "true";
		ui->label->setPalette(palette_error);
		ui->label->setFont(font_error);
	}
	else {
		ui->label->setPalette(palette_normal);
		ui->label->setFont(font_normal);
	}
	if (ui->txt_priimek->text() == "") {
		napaka = "true";
		ui->label_2->setPalette(palette_error);
		ui->label_2->setFont(font_error);
	}
	else {
	   ui->label_2->setPalette(palette_normal);
	   ui->label_2->setFont(font_normal);
	}
	if (ui->txt_telefon->text() == "+ (0)/--" && ui->txt_gsm->text() == "+ (0)/-") {
		napaka = "true";
		ui->label_5->setPalette(palette_error);
		ui->label_5->setFont(font_error);
		ui->label_6->setPalette(palette_error);
		ui->label_6->setFont(font_error);
	}
	else {
		ui->label_5->setPalette(palette_normal);
		ui->label_5->setFont(font_normal);
		ui->label_6->setPalette(palette_normal);
		ui->label_6->setFont(font_normal);
	}
*/

	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {

			QSqlQuery sql_vnesi_stranko;
			if (ui->btn_vnesi->text() == "Vnesi stranko") { // vnesi novega uporabnika
				sql_vnesi_stranko.prepare("INSERT INTO stranke (ime, priimek, naslov, naslov_st, posta, postna_stevilka, davcna, "
																	"kontakt, telefon, gsm, email, spletna_stran, ustanova, opomba, tip, stalnost, aktivnost, "
																	"placilnost, pop_facebook_1, pop_facebook_2, "
																	"pop_kombinacija_1, pop_kombinacija_2, pop_stranka, pop_akcija, pop_vsi_facebook, "
																	"pop_vsi, pod_vikend, pod_hitrost, pod_zapleti, avtor_podjetje, avtor_oseba, davcni_zavezanec, "
																	"banka, bic_banke, trr) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
																	"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_stranko.prepare("UPDATE stranke SET ime = ?, priimek = ?, naslov = ?, naslov_st = ?, posta = ?, postna_stevilka = ?, "
																	"davcna = ?, kontakt = ?, telefon = ?, gsm = ?, email = ?, spletna_stran = ?, ustanova = ?, "
																	"opomba = ?, tip = ?, stalnost = ?, aktivnost = ?, placilnost = ?, pop_facebook_1 = ?, pop_facebook_2 = ?, "
																	"pop_kombinacija_1 = ?, pop_kombinacija_2 = ?, pop_stranka = ?, pop_akcija = ?, "
																	"pop_vsi_facebook = ?, pop_vsi = ?, pod_vikend = ?, pod_hitrost = ?, pod_zapleti = ? , "
																	"avtor_podjetje = ?, avtor_oseba = ?, davcni_zavezanec = ?, banka = ?, bic_banke = ?, trr = ? "
																	"WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_stranko.bindValue(0, pretvori(ui->txt_ime->text()));
			sql_vnesi_stranko.bindValue(1, pretvori(ui->txt_priimek->text()));
			sql_vnesi_stranko.bindValue(2, pretvori(ui->txt_naslov->text()));
			sql_vnesi_stranko.bindValue(3, pretvori(ui->txt_naslov_st->text()));
			sql_vnesi_stranko.bindValue(4, pretvori(ui->txt_posta->currentText()));
			sql_vnesi_stranko.bindValue(5, pretvori(ui->txt_postna_stevilka->text()));
			sql_vnesi_stranko.bindValue(6, pretvori(ui->txt_davcna->text()));
			sql_vnesi_stranko.bindValue(7, pretvori(ui->txt_kontaktna->text()));
			sql_vnesi_stranko.bindValue(8, pretvori(ui->txt_telefon->text().remove(" ")));
			sql_vnesi_stranko.bindValue(9, pretvori(ui->txt_gsm->text().remove(" ")));
			sql_vnesi_stranko.bindValue(10, pretvori(ui->txt_email->text()));
			sql_vnesi_stranko.bindValue(11, pretvori(ui->txt_spletna_stran->text()));
			sql_vnesi_stranko.bindValue(12, pretvori(ui->txt_ustanova->text()));
			sql_vnesi_stranko.bindValue(13, pretvori(ui->txt_opomba->toPlainText()));

			QString vrednost = "";

			if (ui->rb_fizicna->isChecked()) {
				vrednost = "1";
			}
			else {
				vrednost = "2";
			}
			sql_vnesi_stranko.bindValue(14, pretvori(vrednost));
			vrednost = "";
			if (ui->rb_enkratna->isChecked()) {
				vrednost = "1";
			}
			else {
				vrednost = "2";
			}
			sql_vnesi_stranko.bindValue(15, pretvori(vrednost));
			vrednost = "";
			if (ui->rb_aktivna->isChecked()) {
				vrednost = "1";
			}
			else {
				vrednost = "2";
			}
			sql_vnesi_stranko.bindValue(16, pretvori(vrednost));
			vrednost = "";
			if (ui->rb_redna->isChecked()) {
				vrednost = "1";
			}
			else {
				vrednost = "2";
			}
			sql_vnesi_stranko.bindValue(17, pretvori(vrednost));
			vrednost = "";

			sql_vnesi_stranko.bindValue(18, pretvori(pretvori_v_double(ui->txt_popust_fb1->text())));
			sql_vnesi_stranko.bindValue(19, pretvori(pretvori_v_double(ui->txt_popust_fb2->text())));
			sql_vnesi_stranko.bindValue(20, pretvori(pretvori_v_double(ui->txt_popust_komb1->text())));
			sql_vnesi_stranko.bindValue(21, pretvori(pretvori_v_double(ui->txt_popust_komb2->text())));
			sql_vnesi_stranko.bindValue(22, pretvori(pretvori_v_double(ui->txt_popust_stalna_stranka->text())));
			sql_vnesi_stranko.bindValue(23, pretvori(pretvori_v_double(ui->txt_popust_akcija->text())));

			sql_vnesi_stranko.bindValue(24, pretvori(pretvori_v_double(ui->txt_vsi_popusti_facebook_2->text())));
			sql_vnesi_stranko.bindValue(25, pretvori(pretvori_v_double(ui->txt_popusti_skupaj_2->text())));

			sql_vnesi_stranko.bindValue(26, pretvori(pretvori_v_double(ui->txt_podrazitev_vikend->text())));
			sql_vnesi_stranko.bindValue(27, pretvori(pretvori_v_double(ui->txt_podrazitev_hitrost->text())));
			sql_vnesi_stranko.bindValue(28, pretvori(pretvori_v_double(ui->txt_podrazitev_zapleti->text())));

			sql_vnesi_stranko.bindValue(29, pretvori(vApp->firm()));
			sql_vnesi_stranko.bindValue(30, pretvori(vApp->id()));
			if ( ui->cb_davcni_zavezanec->isChecked() ) {
				sql_vnesi_stranko.bindValue(31, pretvori("1"));
			}
			else {
				sql_vnesi_stranko.bindValue(31, pretvori("0"));
			}
			sql_vnesi_stranko.bindValue(32, pretvori(ui->txt_banka->currentText()));
			sql_vnesi_stranko.bindValue(33, pretvori(ui->txt_bic->text()));
			sql_vnesi_stranko.bindValue(34, pretvori(ui->txt_trr->text()));
			sql_vnesi_stranko.exec();

		// send signal to reload widget
		poslji("stranke");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void stranke::prejem(QString besedilo) {

	if (besedilo == "Nova stranka") {
		ui->btn_vnesi->setText("Vnesi stranko");
		ui->tab_projekti->setDisabled(true);
	}
	else {
		ui->btn_vnesi->setText("Popravi vnos");
		ui->tab_projekti->setDisabled(false);
		// besedilo nosi ID ze obstojeco stranko, potrebno je napolniti polja
			QSqlQuery sql_napolni;
			sql_napolni.prepare("SELECT * FROM stranke WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(sql_napolni.value(sql_napolni.record().indexOf("id")).toString());
				ui->txt_ime->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ime")).toString()));
				ui->txt_priimek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("priimek")).toString()));
				ui->txt_naslov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
				ui->txt_naslov_st->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_st")).toString()));
				ui->txt_postna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("postna_stevilka")).toString()));
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("davcni_zavezanec")).toString()) == "0" ) {
					ui->cb_davcni_zavezanec->setChecked(false);
				}
				else {
					ui->cb_davcni_zavezanec->setChecked(true);
				}
				ui->txt_davcna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("davcna")).toString()));
				ui->txt_kontaktna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kontakt")).toString()));
				ui->txt_telefon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("telefon")).toString()));
				ui->txt_gsm->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("gsm")).toString()));
				ui->txt_email->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("email")).toString()));
				ui->txt_spletna_stran->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("spletna_stran")).toString()));
				ui->txt_ustanova->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ustanova")).toString()));
				ui->txt_opomba->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opomba")).toString()));

				if (( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip")).toString()) == "1") ) {
					ui->rb_fizicna->setChecked(true);
				}
				else {
					ui->rb_pravna->setChecked(true);
				}
				if (( prevedi(sql_napolni.value(sql_napolni.record().indexOf("placilnost")).toString()) == "1") ) {
					ui->rb_redna->setChecked(true);
				}
				else {
					ui->rb_neredna->setChecked(true);
				}

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_facebook_1")).toString()) != "0.0") {
					ui->cb_popust_fb1->setCheckState(Qt::Checked);
					ui->txt_popust_fb1->setEnabled(true);
				}
				else {
					ui->cb_popust_fb1->setCheckState(Qt::Unchecked);
					ui->txt_popust_fb1->setEnabled(false);
				}
				ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_facebook_1")).toString())));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_facebook_2")).toString()) != "0.0") {
					ui->cb_popust_fb2->setCheckState(Qt::Checked);
					ui->txt_popust_fb2->setEnabled(true);
				}
				else {
					ui->cb_popust_fb2->setCheckState(Qt::Unchecked);
					ui->txt_popust_fb2->setEnabled(false);
				}
				ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_facebook_2")).toString())));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_kombinacija_1")).toString()) != "0.0") {
					ui->cb_popust_komb1->setCheckState(Qt::Checked);
					ui->txt_popust_komb1->setEnabled(true);
				}
				else {
					ui->cb_popust_komb1->setCheckState(Qt::Unchecked);
					ui->txt_popust_komb1->setEnabled(false);
				}
				ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_kombinacija_1")).toString())));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_kombinacija_2")).toString()) != "0.0") {
					ui->cb_popust_komb2->setCheckState(Qt::Checked);
					ui->txt_popust_komb2->setEnabled(true);
				}
				else {
					ui->cb_popust_komb2->setCheckState(Qt::Unchecked);
					ui->txt_popust_komb2->setEnabled(false);
				}
				ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_kombinacija_2")).toString())));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_stranka")).toString()) != "0.0") {
					ui->cb_popust_stalna->setCheckState(Qt::Checked);
					ui->txt_popust_stalna_stranka->setEnabled(true);
				}
				else {
					ui->cb_popust_stalna->setCheckState(Qt::Unchecked);
					ui->txt_popust_stalna_stranka->setEnabled(false);
				}
				ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_stranka")).toString())));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_akcija")).toString()) != "0.0") {
					ui->cb_popust_akcija->setCheckState(Qt::Checked);
					ui->txt_popust_akcija->setEnabled(true);
				}
				else {
					ui->cb_popust_akcija->setCheckState(Qt::Unchecked);
					ui->txt_popust_akcija->setEnabled(false);
				}
				ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_akcija")).toString())));

				ui->txt_vsi_popusti_facebook_2->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_vsi_facebook")).toString())));
				ui->txt_popusti_skupaj_2->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_vsi")).toString())));
				ui->txt_podrazitev_vikend->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pod_vikend")).toString())));
				ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pod_hitrost")).toString())));
				ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pod_zapleti")).toString())));

				ui->txt_banka->setCurrentIndex(ui->txt_banka->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("banka")).toString())));
				ui->txt_bic->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("bic_banke")).toString()));
				ui->txt_trr->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("trr")).toString()));

				sql_napolni.clear();

				sql_napolni.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("max_pop_facebook") + "'");
				sql_napolni.exec();
				if ( sql_napolni.next() ) {
					ui->txt_vsi_popusti_facebook_1->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString())));
				}
				sql_napolni.clear();

				sql_napolni.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("max_pop_vsi") + "'");
				sql_napolni.exec();
				if ( sql_napolni.next() ) {
					ui->txt_popusti_skupaj_1->setText(prevedi(pretvori_iz_double(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString())));
				}
				sql_napolni.clear();

				QString gumb = ui->btn_novprojekt->text();
				ui->btn_novprojekt->setText("");

				ui->cb_leto->addItem("");
				int min_leto = QDate::currentDate().year();
				int max_leto = QDate::currentDate().year();

				sql_napolni.prepare("SELECT * FROM projekti WHERE avtor_oseba LIKE '" + pretvori(vApp->id()) + "' AND stranka LIKE '" + pretvori(ui->txt_id->text()) + "'");
				sql_napolni.exec();
				while ( sql_napolni.next() ) {
					if ( min_leto > prevedi(sql_napolni.value(sql_napolni.record().indexOf("pricetek_dela")).toString()).right(4).toInt() ) {
						min_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("pricetek_dela")).toString()).right(4).toInt();
					}
					if ( max_leto < prevedi(sql_napolni.value(sql_napolni.record().indexOf("pricetek_dela")).toString()).right(4).toInt() ) {
						max_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("pricetek_dela")).toString()).right(4).toInt();
					}
				}

				for ( int i = min_leto; i <= max_leto; i++ ) {
					ui->cb_leto->addItem(QString::number(i, 10));
				}
				sql_napolni.clear();

				ui->btn_novprojekt->setText(gumb);

			}

		napolni_projekte();

	}
}

void stranke::izracunaj_popuste(int polje) {

	/*
		* racunali bomo po naslednjem principu
		* najprej izracunamo sestevek vseh vnosnih polj s popusti (razen akcijskega)
		* vrednost mora biti nujno manjsa od najvisje dovoljenje vrednosti
		* ce ni, takoj prekini racunanje in vrni sporocilo z napako
		* poglej, ali bi bil sestevek vseh polj vecji od najvisje dovoljene vrednosti
		* ce ni, vnesi stevilko v polje in popravi sestevke
		* ce je, vnesi najvisjo dovoljeno vrednost v polje in popravi sestevke ter
		* obvesti uporabnika s sporocilom
		*/

	/*
	* polje == 0 => fb1
	* polje == 1 => fb2
	* polje == 2 => komb1
	* polje == 3 => komb2
	* polje == 4 => stranka
	*/

	// vsa vnosna polja s popustom pretvorimo v vrednost double, da bo mozno racunanje
	double popust[5];
	popust[0] = pretvori_v_double(ui->txt_popust_fb1->text()).toDouble();
	popust[1] = pretvori_v_double(ui->txt_popust_fb2->text()).toDouble();
	popust[2] = pretvori_v_double(ui->txt_popust_komb1->text()).toDouble();
	popust[3] = pretvori_v_double(ui->txt_popust_komb2->text()).toDouble();
	popust[4] = pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();

	// maksimalne vrednosti sestevka popustov
	double max_fb = pretvori_v_double(ui->txt_vsi_popusti_facebook_1->text()).toDouble();
	double max_vsi = pretvori_v_double(ui->txt_popusti_skupaj_1->text()).toDouble();

	double sestevek;
	sestevek = popust[0] + popust[1];

	if ( sestevek > max_fb ) {
		if ( polje == 0 ) {
			popust[0] = max_fb - popust[1];
			ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust[0], 'f', 1)));
		}
		else if ( polje == 1 ) {
			popust[1] = max_fb - popust[0];
			ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust[1], 'f', 1)));
		}
		QMessageBox msg_napaka;
		msg_napaka.setText("Vnesli ste previsoko vrednost. Vrednost je avtomatsko "
											 "popravljena na najvisjo dovoljena vrednost za to polje!");
		msg_napaka.exec();
		return;
	}

	sestevek = popust[0] + popust[1];
	ui->txt_vsi_popusti_facebook_2->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)));

	sestevek = 0.0;
	for ( int i = 0; i <= 4; i++ ) {
		sestevek += popust[i];
	}
	if ( sestevek > max_vsi ) {
		if ( polje == 0 ) {
			popust[0] = max_vsi - sestevek + popust[0];
			ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust[0], 'f', 1)));
		}
		else if ( polje == 1 ) {
			popust[1] = max_vsi - sestevek + popust[1];
			ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust[1], 'f', 1)));
		}
		else if ( polje == 2 ) {
			popust[2] = max_vsi - sestevek + popust[2];
			ui->txt_popust_komb1->setText(pretvori_iz_double(QString::number(popust[2], 'f', 1)));
		}
		else if ( polje == 3 ) {
			popust[3] = max_vsi - sestevek + popust[3];
			ui->txt_popust_komb2->setText(pretvori_iz_double(QString::number(popust[3], 'f', 1)));
		}
		else if ( polje == 4 ) {
			popust[4] = max_vsi - sestevek + popust[4];
			ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(QString::number(popust[4], 'f', 1)));
		}
		QMessageBox msg_napaka;
		msg_napaka.setText("Vnesli ste previsoko vrednost. Vrednost je avtomatsko "
											 "popravljena na najvisjo dovoljena vrednost za to polje!");
		msg_napaka.exec();
		return;
	}

	sestevek = 0.0;
	for ( int i = 0; i <= 4; i++ ) {
		sestevek += popust[i];
	}
	ui->txt_popusti_skupaj_2->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)));

}

QString stranke::pretvori_v_double(QString besedilo) {

	besedilo.remove(" ");
	besedilo.remove("%");
	besedilo.replace(",", ".");

	if ( besedilo.left(1) == "0" ) {
		besedilo.remove(0,1);
	}
	if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0.0
		besedilo.append("0.0");
	}
	if ( besedilo.left(1) == "." ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
		besedilo.prepend("0");
	}
	if ( besedilo.right(1) == "." ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 0
		besedilo.append("0");
	}
	if ( !besedilo.contains(".") ) { // ce je celo stevilo dodamo decimalno locilo in vrednost 0
		besedilo.append(".0");
	}

	return besedilo;

}

QString stranke::pretvori_iz_double(QString besedilo) {

	besedilo.append(" %");
	besedilo.replace(".",",");

	return besedilo;
}

QString stranke::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString stranke::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void stranke::osvezi(QString beseda) {

	if ( beseda == "projekti" ) {
		napolni_projekte();
	}

}

void stranke::on_cb_popust_fb1_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_fb1->setEnabled(true);

			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_1")).toString()) != "0.0" ) { // stranka ima popust
					ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_1")).toString())));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_facebook_stranka") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}
	}
	else {
		ui->txt_popust_fb1->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_fb1->setEnabled(false);
	}

}

void stranke::on_cb_popust_fb2_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_fb2->setEnabled(true);

			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_2")).toString()) != "0.0" ) { // stranka ima popust
					ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_2")).toString())));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_facebook_prijatelj") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

	}
	else {
		ui->txt_popust_fb2->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_fb2->setEnabled(false);
	}

}

void stranke::on_cb_popust_komb1_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_komb1->setEnabled(true);

			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_1")).toString()) != "0.0" ) { // stranka ima popust
					ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_1")).toString())));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kombinacija_1") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}
	}
	else {
		ui->txt_popust_komb1->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_komb1->setEnabled(false);
	}

}

void stranke::on_cb_popust_komb2_toggled(bool stanje) {

	if ( stanje == true ) {

			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_2")).toString()) != "0.0" ) { // stranka ima popust
					ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_2")).toString())));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kombinacija_2") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}
	}
	else {
		ui->txt_popust_komb2->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_komb2->setEnabled(false);
	}

}

void stranke::on_cb_popust_stalna_toggled(bool stanje) {

	if (stanje == true) {
		ui->rb_stalna->setChecked(true);
	}
	else {
		ui->rb_enkratna->setChecked(true);
	}

	if ( stanje == true ) {
		ui->txt_popust_stalna_stranka->setEnabled(true);

			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_stranka")).toString()) != "0.0" ) { // stranka ima popust
					ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_stranka")).toString())));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_stalna_stranka") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}
	}
	else {
		ui->txt_popust_stalna_stranka->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_stalna_stranka->setEnabled(false);
	}

}

void stranke::on_cb_popust_akcija_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_akcija->setEnabled(true);

			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString()) != "0.0" ) { // stranka ima popust
					ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString())));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}
	}
	else {
		ui->txt_popust_akcija->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_akcija->setEnabled(false);
	}

}

void stranke::on_cb_podrazitev_vikend_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_podrazitev_vikend->setEnabled(true);
	}
	else {
		ui->txt_podrazitev_vikend->setEnabled(false);
	}

}

void stranke::on_cb_podrazitev_hitrost_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_podrazitev_hitrost->setEnabled(true);
	}
	else {
		ui->txt_podrazitev_hitrost->setEnabled(false);
	}

}

void stranke::on_cb_podrazitev_zapleti_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_podrazitev_zapleti->setEnabled(true);
	}
	else {
		ui->txt_podrazitev_zapleti->setEnabled(false);
	}

}

void stranke::on_txt_popust_fb1_textChanged() {

	izracunaj_popuste(0);

}

void stranke::on_txt_popust_fb2_textChanged() {

	izracunaj_popuste(1);

}

void stranke::on_txt_popust_komb1_textChanged() {

	izracunaj_popuste(2);

}

void stranke::on_txt_popust_komb2_textChanged() {

	izracunaj_popuste(3);

}

void stranke::on_txt_popust_stalna_stranka_textChanged() {

	izracunaj_popuste(4);

}

void stranke::on_txt_popust_fb1_editingFinished() {

	ui->txt_popust_fb1->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_fb1->text())));

}

void stranke::on_txt_popust_fb2_editingFinished() {

	ui->txt_popust_fb2->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_fb2->text())));

}

void stranke::on_txt_popust_komb1_editingFinished() {

	ui->txt_popust_komb1->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_komb1->text())));

}

void stranke::on_txt_popust_komb2_editingFinished() {

	ui->txt_popust_komb2->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_komb2->text())));

}

void stranke::on_txt_popust_stalna_stranka_editingFinished() {

	ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_stalna_stranka->text())));

}

void stranke::on_txt_popust_akcija_editingFinished() {

	ui->txt_popust_akcija->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_akcija->text())));

}

void stranke::on_txt_podrazitev_vikend_editingFinished() {

	ui->txt_podrazitev_vikend->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_vikend->text())));

}

void stranke::on_txt_podrazitev_hitrost_editingFinished() {

	ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_hitrost->text())));

}

void stranke::on_txt_podrazitev_zapleti_editingFinished() {

	ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_zapleti->text())));

}

void stranke::on_txt_banka_currentIndexChanged() {

		// v bazi poiscemo pot do mesta shranjevanja podatkov
		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM sif_banke WHERE ime_banke LIKE '" + pretvori(ui->txt_banka->currentText()) + "'");
		sql_pot.exec();
		if ( sql_pot.next() ) {
			ui->txt_bic->setText(prevedi(sql_pot.value(sql_pot.record().indexOf("bic")).toString()));
		}

}

#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QFileDialog>
#include <QDir>
#include <QPainter>

#include "podjetje.h"
#include "ui_podjetje.h"
#include "prijava.h"
#include "kodiranje.h"

podjetje::podjetje(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::podjetje)
{
	ui->setupUi(this);

		// podjetje
			ui->txt_id->setText("");
			ui->txt_ime->setText("");
			ui->txt_polnoime->setText("");
			ui->txt_skd->setText("");
			ui->txt_url->setText("");
			ui->txt_maticna_stevilka->setText("");
			ui->txt_davcna->setText("");
			ui->txt_tekoci_racun->setText("");
			ui->txt_naslov->setText("");
			ui->txt_naslov_st->setText("");
			ui->txt_postna_stevilka->setText("");
			ui->txt_email->setText("");
			ui->txt_telefon->setText("");
			ui->txt_gsm->setText("");

			ui->txt_banka->clear();
			ui->txt_bic->setText("");
			ui->txt_koda_namena_avans->clear();
			ui->txt_koda_namena->clear();

			ui->txt_skd_besedilo->clear();
			ui->txt_posta->clear();
			ui->txt_zavezanec->setText("");

			ui->img_logotip->setText("");
			ui->lbl_logotip->setText("");

			ui->lbl_logotip->setVisible(false);

		// lastnik
			ui->txt_lastnik->clear();
			ui->txt_lastnik_ime->setText("");
			ui->txt_lastnik_priimek->setText("");
			ui->txt_lastnik_naziv->setText("");
			ui->txt_lastnik_naslov->setText("");
			ui->txt_lastnik_st->setText("");
			ui->txt_lastnik_posta->setText("");
			ui->txt_lastnik_postna_stevilka->setText("");
			ui->txt_lastnik_email->setText("");
			ui->txt_lastnik_gsm->setText("");
			ui->txt_lastnik_telefon->setText("");

		// kontaktna oseba
			ui->txt_kontaktna->clear();
			ui->txt_kontaktna_ime->setText("");
			ui->txt_kontaktna_priimek->setText("");
			ui->txt_kontaktna_naziv->setText("");
			ui->txt_kontaktna_naslov->setText("");
			ui->txt_kontaktna_st->setText("");
			ui->txt_kontaktna_posta->setText("");
			ui->txt_kontaktna_postna_stevilka->setText("");
			ui->txt_kontaktna_email->setText("");
			ui->txt_kontaktna_gsm->setText("");
			ui->txt_kontaktna_telefon->setText("");

		// odgovorna oseba
			ui->txt_odgovorna->clear();
			ui->txt_odgovorna_ime->setText("");
			ui->txt_odgovorna_priimek->setText("");
			ui->txt_odgovorna_naziv->setText("");
			ui->txt_odgovorna_naslov->setText("");
			ui->txt_odgovorna_st->setText("");
			ui->txt_odgovorna_posta->setText("");
			ui->txt_odgovorna_postna_stevilka->setText("");
			ui->txt_odgovorna_email->setText("");
			ui->txt_odgovorna_gsm->setText("");
			ui->txt_odgovorna_telefon->setText("");

		// nastavi masko za vnos podatkov
			ui->txt_davcna->setInputMask("99999999;_");
			ui->txt_tekoci_racun->setInputMask("SI56 9999 9999 9999 999;_");
			ui->txt_gsm->setInputMask("+990 (\\0)99/999-999;_");
			ui->txt_telefon->setInputMask("+990 (\\0)9/99-99-999;_");

		// vnesi podatke v spustne sezname
		ui->txt_skd_besedilo->addItem("");
		ui->txt_posta->addItem("");
		ui->txt_lastnik->addItem("");
		ui->txt_odgovorna->addItem("");
		ui->txt_kontaktna->addItem("");

		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM sif_skd ORDER BY skd_stevilka ASC");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_skd_besedilo->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("skd_naziv")).toString()));
		}
		sql_fill_combo.clear();

		sql_fill_combo.prepare("SELECT * FROM sif_posta ORDER BY posta ASC");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_posta->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("posta")).toString()));
		}
		sql_fill_combo.clear();

		int i = 0; // preveri, ce obstaja kaksen uporabnik
		sql_fill_combo.prepare("SELECT * FROM uporabniki WHERE podjetje LIKE '" + vApp->firm() + "'");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			i++;
			QString vrstica;
			vrstica = vrstica + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ") ";
			vrstica = vrstica + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()) + " ";
			vrstica = vrstica + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("priimek")).toString());
			ui->txt_lastnik->addItem(vrstica);
			ui->txt_odgovorna->addItem(vrstica);
			ui->txt_kontaktna->addItem(vrstica);
		}
		sql_fill_combo.clear();

		ui->txt_banka->addItem("");
		sql_fill_combo.prepare("SELECT * FROM sif_banke ORDER BY ime_banke ASC");
		sql_fill_combo.exec();
		while ( sql_fill_combo.next() ) {
			ui->txt_banka->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime_banke")).toString()));
		}
		sql_fill_combo.clear();

		ui->txt_koda_namena->addItem("");
		ui->txt_koda_namena_avans->addItem("");
		sql_fill_combo.prepare("SELECT * FROM sif_koda_namena ORDER BY koda ASC");
		sql_fill_combo.exec();
		while ( sql_fill_combo.next() ) {
			ui->txt_koda_namena->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("koda")).toString()) + " - " +
										 prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("opis_kratek")).toString()));
			ui->txt_koda_namena_avans->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("koda")).toString()) + " - " +
											   prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("opis_kratek")).toString()));
		}
		sql_fill_combo.clear();

		// ce uporabnik ne obstaja, potem skrijemo zavihke za vnos oseb
		if ( i != 0 ) {
			ui->tab_lastnik->setHidden(true);
			ui->tab_kontaktna_oseba->setHidden(true);
			ui->tab_odgovorna_oseba->setHidden(true);
		}
		else {
			ui->tab_lastnik->setHidden(false);
			ui->tab_kontaktna_oseba->setHidden(false);
			ui->tab_odgovorna_oseba->setHidden(false);
		}

	ui->tab_podjetje->setCurrentIndex(0);

}

podjetje::~podjetje() {
	delete ui;
}

void podjetje::on_btn_izhod_clicked() {

	if ( vApp->id() == "" ) {
		exit(0);
	}
	else {
		close();
	}

}

// ne preverja polj
void podjetje::on_btn_potrdi_clicked() {

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

	// preveri ime podjetja, ce ze obstaja
	if (ui->txt_ime->text() == "") {
		ui->label_5->setPalette(palette_error);
		ui->label_5->setFont(font_error);
		napaka = "true";
		ui->txt_ime->setFocus();
	}
	else {
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
			QSqlQuery sql_preveri_podjetje;
			sql_preveri_podjetje.prepare("SELECT * FROM podjetja WHERE ime LIKE '" + ui->txt_ime->text() + "'");
			sql_preveri_podjetje.exec();
			if (sql_preveri_podjetje.next()) {
				if (ui->txt_id->text() != prevedi(sql_preveri_podjetje.value(sql_preveri_podjetje.record().indexOf("id")).toString())) {
					ui->label_5->setPalette(palette_error);
					ui->label_5->setFont(font_error);
					napaka = "true";
					ui->txt_ime->setFocus();
				}
				else {
					ui->label_5->setPalette(palette_normal);
					ui->label_5->setFont(font_normal);
				}
			}
			else {
				ui->label_5->setPalette(palette_normal);
				ui->label_5->setFont(font_normal);
			}
		}
		base.close();
	}

	// preveri davcno stevilko
	QString davcna = ui->txt_davcna->text();
	if (ui->txt_zavezanec->isChecked()) {
		davcna = davcna.right(davcna.length()-2);
	}
	if (davcna.length() < 8 && davcna != "") { // dolzina
		ui->label_8->setPalette(palette_error);
		ui->label_8->setFont(font_error);
		napaka = "true";
		ui->txt_davcna->setFocus();
	}
	else { // pravilnost zapisa
		QString tax = davcna;
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
			ui->label_8->setPalette(palette_error);
			ui->label_8->setFont(font_error);
			napaka = "true";
			ui->txt_davcna->setFocus();
		}
		else {
			ui->label_8->setPalette(palette_normal);
			ui->label_8->setFont(font_normal);
		}
	}

	// preveri trr
	// ne najdem algoritma

	// preveri email naslov
	ui->txt_email->setText(ui->txt_email->text().toLower());
	QRegExp rx_email("^[a-z0-9\\-\\_\\.]*[@][a-z0-9\\-\\_\\.]*[\\.][a-z]{2,3}$");

	if (!rx_email.exactMatch(ui->txt_email->text())) {
		ui->label_11->setPalette(palette_error);
		ui->label_11->setFont(font_error);
		napaka = "true";
		ui->txt_email->setFocus();
	}
	else {
		ui->label_11->setPalette(palette_normal);
		ui->label_11->setFont(font_normal);
	}

	// preveri url
	if (ui->txt_url->text() != "") {
		if (!(ui->txt_url->text().startsWith("http") || ui->txt_url->text().contains("://"))) {
			ui->txt_url->setText("http://" + ui->txt_url->text());
		}
		if (!(ui->txt_url->text().endsWith("/") || ui->txt_url->text().endsWith(".htm") || ui->txt_url->text().endsWith(".html"))) {
			ui->txt_url->setText(ui->txt_url->text().append("/"));
		}
	}
*/
	if (napaka == "") {
			QSqlQuery sql_vnesi;
			QString zavezanec = "NE";
			if (ui->txt_zavezanec->isChecked()) {
				zavezanec = "DA";
			}
			if (ui->btn_potrdi->text() == "Vnesi podjetje") {
			sql_vnesi.prepare("INSERT INTO podjetje (ime, polnoime, skd, skd_besedilo, url, maticna_stevilka, ddv_zavezanec, davcna, "
							  "tekoci_racun, naslov, naslov_st, posta, postna_stevilka, email, gsm, telefon, lastnik, kontaktna, "
							  "odgovorna, logotip, bic, banka, koda_namena_avans, koda_namena)"
							  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ");
			}
			else {
			sql_vnesi.prepare("UPDATE podjetje SET ime = ?, polnoime = ?, skd = ?, skd_besedilo = ?, url = ?, maticna_stevilka = ?, "
							  "ddv_zavezanec = ?, davcna = ?, tekoci_racun = ?, naslov = ?, naslov_st = ?, posta = ?, postna_stevilka = ?, "
							  "email = ?, gsm = ?, telefon = ?, lastnik = ?, kontaktna = ?, odgovorna = ?, logotip = ?, "
							  "bic = ?, banka = ?, koda_namena_avans = ?, koda_namena = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi.bindValue(0, pretvori(ui->txt_ime->text()));
			sql_vnesi.bindValue(1, pretvori(ui->txt_polnoime->text()));
			sql_vnesi.bindValue(2, pretvori(ui->txt_skd->text()));
			sql_vnesi.bindValue(3, pretvori(ui->txt_skd_besedilo->currentText()));
			sql_vnesi.bindValue(4, pretvori(ui->txt_url->text()));
			sql_vnesi.bindValue(5, pretvori(ui->txt_maticna_stevilka->text()));
			sql_vnesi.bindValue(6, pretvori(zavezanec));
			sql_vnesi.bindValue(7, pretvori(ui->txt_davcna->text().remove(" ")));
			sql_vnesi.bindValue(8, pretvori(ui->txt_tekoci_racun->text().remove(" ")));
			sql_vnesi.bindValue(9, pretvori(ui->txt_naslov->text()));
			sql_vnesi.bindValue(10, pretvori(ui->txt_naslov_st->text()));
			sql_vnesi.bindValue(11, pretvori(ui->txt_posta->currentText()));
			sql_vnesi.bindValue(12, pretvori(ui->txt_postna_stevilka->text().remove(" ")));
			sql_vnesi.bindValue(13, pretvori(ui->txt_email->text()));
			sql_vnesi.bindValue(14, pretvori(ui->txt_gsm->text().remove(" ")));
			sql_vnesi.bindValue(15, pretvori(ui->txt_telefon->text().remove(" ")));
			sql_vnesi.bindValue(16, pretvori(ui->txt_lastnik->currentText().left(ui->txt_lastnik->currentText().indexOf(")", 0))));
			sql_vnesi.bindValue(17, pretvori(ui->txt_kontaktna->currentText().left(ui->txt_kontaktna->currentText().indexOf(")", 0))));
			sql_vnesi.bindValue(18, pretvori(ui->txt_odgovorna->currentText().left(ui->txt_odgovorna->currentText().indexOf(")", 0))));
			sql_vnesi.bindValue(19, pretvori(ui->lbl_logotip->text()));
			sql_vnesi.bindValue(20, pretvori(ui->txt_bic->text()));
			sql_vnesi.bindValue(21, pretvori(ui->txt_banka->currentText()));
			sql_vnesi.bindValue(22, pretvori(ui->txt_koda_namena_avans->currentText().left(4)));
			sql_vnesi.bindValue(23, pretvori(ui->txt_koda_namena->currentText().left(4)));
			sql_vnesi.exec();

		// send signal to reload widget
		poslji("podjetje");

		if ( vApp->id() != "" ) {
			this->close();
		}

	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void podjetje::on_txt_zavezanec_stateChanged() {

	if (ui->txt_zavezanec->isChecked()) {
		ui->txt_davcna->setInputMask("SI99999999;_");
	}
	else {
		ui->txt_davcna->setInputMask("99999999;_");
	}
}

void podjetje::on_btn_brisi_clicked() {

	ui->txt_id->setText("");
	ui->txt_ime->setText("");
	ui->txt_polnoime->setText("");
	ui->txt_skd->setText("");
	ui->txt_url->setText("");
	ui->txt_maticna_stevilka->setText("");
	ui->txt_davcna->setText("");
	ui->txt_polnoime->setText("");
	ui->txt_tekoci_racun->setText("");
	ui->txt_naslov->setText("");
	ui->txt_naslov_st->setText("");
	ui->txt_postna_stevilka->setText("");
	ui->txt_email->setText("");
	ui->txt_telefon->setText("");
	ui->txt_gsm->setText("");

	ui->txt_skd_besedilo->setCurrentIndex(0);
	ui->txt_posta->setCurrentIndex(0);
	ui->txt_lastnik->setCurrentIndex(0);
	ui->txt_kontaktna->setCurrentIndex(0);
	ui->txt_odgovorna->setCurrentIndex(0);

	ui->txt_zavezanec->setChecked(false);

}

void podjetje::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_potrdi_clicked();
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

void podjetje::prejem(QString besedilo) {

	if (besedilo == "Novo podjetje") {
		ui->btn_potrdi->setText("Vnesi podjetje");
		ui->tab_lastnik->setDisabled(true);
		ui->tab_kontaktna_oseba->setDisabled(true);
		ui->tab_odgovorna_oseba->setDisabled(true);
	}
	else {
			ui->btn_potrdi->setText("Popravi podjetje");
			ui->tab_lastnik->setDisabled(false);
			ui->tab_kontaktna_oseba->setDisabled(false);
			ui->tab_odgovorna_oseba->setDisabled(false);

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM podjetje WHERE id LIKE '" + besedilo + "'");
			sql_fill.exec();
			if (sql_fill.next()) {
				if (prevedi(sql_fill.value(sql_fill.record().indexOf("ddv_zavezanec")).toString()) == "DA") {
					ui->txt_zavezanec->setChecked(true);
				}
				else {
					ui->txt_zavezanec->setChecked(false);
				}
				ui->txt_id->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
				ui->txt_ime->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString()));
				ui->txt_polnoime->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("polnoime")).toString()));
				ui->txt_skd->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("skd")).toString()));
				ui->txt_url->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("url")).toString()));

				ui->txt_maticna_stevilka->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("maticna_stevilka")).toString()));
				ui->txt_davcna->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("davcna")).toString()));
				ui->txt_tekoci_racun->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("tekoci_racun")).toString()));

				ui->txt_naslov->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("naslov")).toString()));
				ui->txt_naslov_st->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("naslov_st")).toString()));
				ui->txt_postna_stevilka->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("postna_stevilka")).toString()));

				ui->txt_email->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("email")).toString()));
				ui->txt_telefon->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("telefon")).toString()));
				ui->txt_gsm->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("gsm")).toString()));

				// prikazi sliko iz besedila
				ui->lbl_logotip->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("logotip")).toString()));

				// pretvori besedilo slike v bytearray in ga pretvori v sliko
				QByteArray test;
				test.append(ui->lbl_logotip->text());
				QPixmap* pixmap = new QPixmap();
				pixmap->loadFromData(QByteArray::fromBase64(test));
				ui->img_logotip->setPixmap(*pixmap);

				ui->txt_banka->setCurrentIndex(ui->txt_banka->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("banka")).toString())));
				ui->txt_bic->setText((prevedi(sql_fill.value(sql_fill.record().indexOf("bic")).toString())));
				ui->txt_koda_namena_avans->setCurrentIndex(ui->txt_koda_namena_avans->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("koda_namena_avans")).toString()) + " - ", Qt::MatchStartsWith));
				ui->txt_koda_namena->setCurrentIndex(ui->txt_koda_namena->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("koda_namena")).toString()) + " - ", Qt::MatchStartsWith));

				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("lastnik")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					QString izbranec;
					izbranec = prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") ";
					izbranec = izbranec + prevedi(sql_combo.value(sql_combo.record().indexOf("ime")).toString()) + " ";
					izbranec = izbranec + prevedi(sql_combo.value(sql_combo.record().indexOf("priimek")).toString());
					ui->txt_lastnik->setCurrentIndex(ui->txt_lastnik->findText(izbranec));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("kontaktna")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					QString izbranec;
					izbranec = prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") ";
					izbranec = izbranec + prevedi(sql_combo.value(sql_combo.record().indexOf("ime")).toString()) + " ";
					izbranec = izbranec + prevedi(sql_combo.value(sql_combo.record().indexOf("priimek")).toString());
					ui->txt_kontaktna->setCurrentIndex(ui->txt_kontaktna->findText(izbranec));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("odgovorna")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					QString izbranec;
					izbranec = prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()) + ") ";
					izbranec = izbranec + prevedi(sql_combo.value(sql_combo.record().indexOf("ime")).toString()) + " ";
					izbranec = izbranec + prevedi(sql_combo.value(sql_combo.record().indexOf("priimek")).toString());
					ui->txt_odgovorna->setCurrentIndex(ui->txt_odgovorna->findText(izbranec));
				}
				sql_combo.clear();

			}
	}

}

void podjetje::on_txt_lastnik_currentIndexChanged(QString besedilo) {

		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + pretvori(besedilo.left(besedilo.indexOf(") ", 0))) + "'");
		sql_napolni.exec();
		if ( sql_napolni.next() ) {
			ui->txt_lastnik_ime->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ime")).toString()));
			ui->txt_lastnik_priimek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("priimek")).toString()));
			ui->txt_lastnik_naziv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()));
			ui->txt_lastnik_naslov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
			ui->txt_lastnik_st->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_stevilka")).toString()));
			ui->txt_lastnik_posta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("posta")).toString()));
			ui->txt_lastnik_postna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("postna_stevilka")).toString()));
			ui->txt_lastnik_email->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("email")).toString()));
			ui->txt_lastnik_gsm->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("gsm")).toString()));
			ui->txt_lastnik_telefon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("telefon")).toString()));
		}
}

void podjetje::on_txt_kontaktna_currentIndexChanged(QString besedilo) {

		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + pretvori(besedilo.left(besedilo.indexOf(") ", 0))) + "'");
		sql_napolni.exec();
		if ( sql_napolni.next() ) {
			ui->txt_kontaktna_ime->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ime")).toString()));
			ui->txt_kontaktna_priimek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("priimek")).toString()));
			ui->txt_kontaktna_naziv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()));
			ui->txt_kontaktna_naslov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
			ui->txt_kontaktna_st->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_stevilka")).toString()));
			ui->txt_kontaktna_posta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("posta")).toString()));
			ui->txt_kontaktna_postna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("postna_stevilka")).toString()));
			ui->txt_kontaktna_email->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("email")).toString()));
			ui->txt_kontaktna_gsm->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("gsm")).toString()));
			ui->txt_kontaktna_telefon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("telefon")).toString()));
		}

}

void podjetje::on_txt_odgovorna_currentIndexChanged(QString besedilo) {

		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + pretvori(besedilo.left(besedilo.indexOf(") ", 0))) + "'");
		sql_napolni.exec();
		if ( sql_napolni.next() ) {
			ui->txt_odgovorna_ime->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ime")).toString()));
			ui->txt_odgovorna_priimek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("priimek")).toString()));
			ui->txt_odgovorna_naziv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()));
			ui->txt_odgovorna_naslov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
			ui->txt_odgovorna_st->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_stevilka")).toString()));
			ui->txt_odgovorna_posta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("posta")).toString()));
			ui->txt_odgovorna_postna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("postna_stevilka")).toString()));
			ui->txt_odgovorna_email->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("email")).toString()));
			ui->txt_odgovorna_gsm->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("gsm")).toString()));
			ui->txt_odgovorna_telefon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("telefon")).toString()));
		}

}

void podjetje::on_txt_skd_besedilo_currentIndexChanged() {

		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_skd WHERE skd_naziv LIKE '" + pretvori(ui->txt_skd_besedilo->currentText()) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_skd->setText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("skd_stevilka")).toString()));
		}

}

void podjetje::on_txt_skd_textChanged(QString besedilo) {

		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_skd WHERE skd_stevilka LIKE '" + pretvori(besedilo) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_skd_besedilo->setCurrentIndex(ui->txt_skd_besedilo->findText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("skd_naziv")).toString())));
		}

}

void podjetje::on_txt_posta_currentIndexChanged() {

		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_posta WHERE posta LIKE '" + pretvori(ui->txt_posta->currentText()) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_postna_stevilka->setText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("postna_stevilka")).toString()));
		}

}

void podjetje::on_txt_postna_stevilka_textChanged(QString besedilo) {

		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_posta WHERE postna_stevilka LIKE '" + pretvori(besedilo) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_posta->setCurrentIndex(ui->txt_posta->findText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("posta")).toString())));
		}

}

QString podjetje::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString podjetje::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void podjetje::on_btn_logotip_clicked() {

	// izberi sliko za logotip
	QString pot_do_logotipa = "";
	pot_do_logotipa = QFileDialog::getOpenFileName(this, "Izberite logotip podjetja", QDir::homePath(), "Slika (*.png *.jpg *.jpeg *.svg)");

	// sliko prikazemo na zaslonu
	QImage slika(pot_do_logotipa);
	ui->img_logotip->setPixmap(QPixmap::fromImage(slika).scaledToHeight(70));

	// sliko pretvorimo v besedilo
	QByteArray fotografija;
	QBuffer buffer( &fotografija );
	buffer.open( QBuffer::WriteOnly );
	ui->img_logotip->pixmap()->save( &buffer, "PNG" );

	// besedilo prikazemo v oznaki
	ui->lbl_logotip->setText(fotografija.toBase64());

}

void podjetje::on_txt_banka_currentIndexChanged() {

		// v bazi poiscemo pot do mesta shranjevanja podatkov
		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM sif_banke WHERE ime_banke LIKE '" + pretvori(ui->txt_banka->currentText()) + "'");
		sql_pot.exec();
		if ( sql_pot.next() ) {
			ui->txt_bic->setText(prevedi(sql_pot.value(sql_pot.record().indexOf("bic")).toString()));
		}

}

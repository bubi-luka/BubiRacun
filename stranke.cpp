#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "stranke.h"
#include "ui_stranke.h"
#include "projekti.h"
#include "kuponi.h"
#include "kodiranje.h"

stranke::stranke(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::stranke)
{
    ui->setupUi(this);

	// set name labels
	ui->label->setText("Ime");
	ui->label_2->setText("Priimek");

	// set visible and enabled fields
	ui->txt_id->setDisabled(true);
	ui->txt_kontaktna->setVisible(false);
	ui->label_8->setVisible(false);
	ui->txt_davcna->setVisible(false);
	ui->label_4->setVisible(false);
	ui->txt_vir_kupon->setVisible(false);
	ui->label_19->setVisible(false);
	ui->txt_vir_ime->setVisible(false);
	ui->label_20->setVisible(false);
	ui->txt_vir_id->setVisible(false);
	ui->txt_vir_besedilo->setVisible(false);
	ui->label_11->setVisible(true);
	ui->txt_ustanova->setVisible(true);

	//set input mask for phone fields
	ui->txt_gsm->setInputMask("+990 (\\0)99/999-999;_");
	ui->txt_telefon->setInputMask("+990 (\\0)9/99-99-999;_");

	// fill combo box and tables
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

		// fill combo box
		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM sif_virstrank");
		sql_fill_combo.exec();
		ui->txt_vir->addItem("");
		while (sql_fill_combo.next()) {
			ui->txt_vir->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("vir")).toString()));
		}

	}
	base.close();

	napolni_projekte();
	napolni_kupone();

}

stranke::~stranke()
{
    delete ui;
}

void stranke::napolni_projekte() {
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
		// fill table projekti
		// clear previous content
		ui->tbl_projekti->clear();

		for (int i = 0; i <= 3; i++) {
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

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		naslov0->setText("ID");
		naslov1->setText("Projekt");
		naslov2->setText("Pricetek");
		naslov3->setText("Konec");
		ui->tbl_projekti->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_projekti->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_projekti->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_projekti->setHorizontalHeaderItem(3, naslov3);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM projekti WHERE stranka LIKE '" + ui->txt_id->text() + "'");
		sql_fill.exec();

		int row = 0;
		QString racunovodstvo = "";

		while (sql_fill.next()) {
			// check for active project and set custummer as active if found one
			if (prevedi(sql_fill.value(sql_fill.record().indexOf("statusracunovodstvo")).toString()) != "Oddano") {
				racunovodstvo = "V delu";
			}

			ui->tbl_projekti->insertRow(row);
			ui->tbl_projekti->setRowHeight(row, 20);
			int col = 0;
			while (col <= 3) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				celica->setText(prevedi(sql_fill.value(col).toString()));
				ui->tbl_projekti->setItem(row, col, celica);

				col++;

			}

			row++;

		}
		if (racunovodstvo == "") {
			ui->rb_neaktivna->setChecked(true);
		}
		else {
			ui->rb_aktivna->setChecked(true);
		}

	// poda napacne vrednosti popustov, zato stalnost dolocamo rocno
	//	if ( row > 1 ) {
	//		ui->rb_stalna->setChecked(true);
	//	}
	}
	base.close();

}

void stranke::napolni_kupone() {

	// fill combo box and tables
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

		napolni_projekte();

		// fill table kuponi
		// clear previous content
		ui->tbl_kuponi->clear();

		for (int i = 0; i <= 3; i++) {
			ui->tbl_kuponi->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.clear();
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

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		naslov0->setText("ID");
		naslov1->setText("Kupon");
		naslov2->setText("Izdajatelj");
		naslov3->setText("Koristnik");
		ui->tbl_kuponi->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_kuponi->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_kuponi->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_kuponi->setHorizontalHeaderItem(3, naslov3);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM kuponi WHERE prejemnik LIKE '" + ui->txt_id->text() + "'");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_kuponi->insertRow(row);
			ui->tbl_kuponi->setRowHeight(row, 20);
			int col = 0;
			while (col <= 3) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				celica->setText(prevedi(sql_fill.value(col).toString()));
				ui->tbl_kuponi->setItem(row, col, celica);

				col++;

			}

			row++;

		}

	}
	base.close();

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
		ui->txt_davcna->setHidden(false);
		ui->label_4->setHidden(false);
	}

}

void stranke::on_rb_stalna_toggled(bool stanje)  {

	if ( stanje == true) {
		ui->cb_vip->setChecked(true);
	}
	else {
		ui->cb_vip->setChecked(false);
	}

}

void stranke::on_txt_vir_currentIndexChanged(int stanje) {

	if (stanje == 0) {
		ui->label_19->setHidden(true);
		ui->label_20->setHidden(true);
		ui->txt_vir_id->setHidden(true);
		ui->txt_vir_ime->setHidden(true);
		ui->txt_vir_kupon->setHidden(true);
		ui->txt_vir_besedilo->setHidden(true);
	}
	else if (ui->txt_vir->currentText() == "Stranka") {
		ui->label_19->setHidden(false);
		ui->label_20->setHidden(false);
		ui->txt_vir_id->setHidden(true);
		ui->txt_vir_ime->setHidden(false);
		ui->txt_vir_kupon->setHidden(false);
		ui->txt_vir_besedilo->setHidden(true);
	}
	else {
		ui->label_19->setHidden(true);
		ui->label_20->setHidden(true);
		ui->txt_vir_id->setHidden(true);
		ui->txt_vir_ime->setHidden(true);
		ui->txt_vir_kupon->setHidden(true);
		ui->txt_vir_besedilo->setHidden(false);
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

void stranke::on_tbl_kuponi_doubleClicked() {

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

void stranke::on_btn_nov_clicked() {

	kuponi *uredi = new kuponi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov kupon" + ui->txt_id->text());
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

// prazno, dokler ne definiramo funkcije gumba
void stranke::on_btn_brisi_clicked() {

}

void stranke::on_btn_izhod_clicked() {

	close();

}

void stranke::on_btn_vnesi_clicked() {

	QString napaka = "";

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
	}
	base.close();

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

	// kupon
	if ( ui->txt_vir->currentText() == "Stranka" ) {
		if ( ui->txt_vir_kupon->text() == "" ) { // kupona ni
			ui->label_19->setPalette(palette_error);
			ui->label_19->setFont(font_error);
			napaka = "true";
			ui->txt_vir_kupon->setFocus();
		}
		else { // kupon je
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
				QSqlQuery sql_kupon;
				sql_kupon.prepare("SELECT * FROM kuponi WHERE kupon LIKE '" + pretvori(ui->txt_vir_kupon->text()) + "'");
				sql_kupon.exec();
				if ( !sql_kupon.next() ) { // kupon ne obstaja
					ui->label_19->setPalette(palette_error);
					ui->label_19->setFont(font_error);
					napaka = "true";
					ui->txt_vir_kupon->setFocus();
				}
				else if ( prevedi(sql_kupon.value(sql_kupon.record().indexOf("uporabnik")).toString()) != "" ) { // kupon ima dolocenega uporabnika
					if ( prevedi(sql_kupon.value(sql_kupon.record().indexOf("uporabnik")).toString()) != ui->txt_id->text() ) { // uporabnik ni isti
						ui->label_19->setPalette(palette_error);
						ui->label_19->setFont(font_error);
						napaka = "true";
						ui->txt_vir_kupon->setFocus();
					}
					else { // uporabnik je isti
						ui->txt_vir_id->setText(sql_kupon.value(sql_kupon.record().indexOf("id")).toString());
						ui->txt_vir_ime->setText(prevedi(sql_kupon.value(sql_kupon.record().indexOf("lastnik")).toString()));
						ui->label_19->setPalette(palette_normal);
						ui->label_19->setFont(font_normal);
					}
				}
				else {
					ui->txt_vir_id->setText(sql_kupon.value(sql_kupon.record().indexOf("id")).toString());
					ui->txt_vir_ime->setText(prevedi(sql_kupon.value(sql_kupon.record().indexOf("lastnik")).toString()));
					ui->label_19->setPalette(palette_normal);
					ui->label_19->setFont(font_normal);
				}

			}
			base.close();
			ui->label_19->setPalette(palette_normal);
			ui->label_19->setFont(font_normal);
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

	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {
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
			// brisi popuste, kjer jih ni
			if ( !ui->cb_facebook->isChecked() ) ui->txt_p_facebook->setText("");
			if ( !ui->cb_twitter->isChecked() ) ui->txt_p_twitter->setText("");
			if ( !ui->cb_google->isChecked() ) ui->txt_p_google->setText("");
			if ( !ui->cb_blog->isChecked() ) ui->txt_p_blog->setText("");
			if ( !ui->cb_forum->isChecked() ) ui->txt_p_forum->setText("");
			if ( !ui->cb_sfacebook->isChecked() ) ui->txt_p_sfacebook->setText("");
			if ( !ui->cb_stwitter->isChecked() ) ui->txt_p_stwitter->setText("");
			if ( !ui->cb_skuponi->isChecked() ) ui->txt_p_skuponi->setText("");
			if ( !ui->cb_obrazec->isChecked() ) ui->txt_p_obrazec->setText("");
			if ( !ui->cb_kupon->isChecked() ) ui->txt_p_stranka->setText("");
			if ( !ui->cb_akcija->isChecked() ) ui->txt_p_akcija->setText("");
			if ( !ui->cb_vip->isChecked() ) ui->txt_p_vip->setText("");

			// spremeni popuste v pravi format
			ui->txt_p_facebook->setText(ui->txt_p_facebook->text().remove("%").replace(",", "."));
			if (ui->txt_p_facebook->text() == "") {
				ui->txt_p_facebook->setText("0.00");
			}
			if (!ui->txt_p_facebook->text().contains(".")) {
				ui->txt_p_facebook->setText(ui->txt_p_facebook->text() + ".00");
			}
			ui->txt_p_twitter->setText(ui->txt_p_twitter->text().remove("%").replace(",", "."));
			if (ui->txt_p_twitter->text() == "") {
				ui->txt_p_twitter->setText("0.00");
			}
			if (!ui->txt_p_twitter->text().contains(".")) {
				ui->txt_p_twitter->setText(ui->txt_p_twitter->text() + ".00");
			}
			ui->txt_p_google->setText(ui->txt_p_google->text().remove("%").replace(",", "."));
			if (ui->txt_p_google->text() == "") {
				ui->txt_p_google->setText("0.00");
			}
			if (!ui->txt_p_google->text().contains(".")) {
				ui->txt_p_google->setText(ui->txt_p_google->text() + ".00");
			}
			ui->txt_p_blog->setText(ui->txt_p_blog->text().remove("%").replace(",", "."));
			if (ui->txt_p_blog->text() == "") {
				ui->txt_p_blog->setText("0.00");
			}
			if (!ui->txt_p_blog->text().contains(".")) {
				ui->txt_p_blog->setText(ui->txt_p_blog->text() + ".00");
			}
			ui->txt_p_forum->setText(ui->txt_p_forum->text().remove("%").replace(",", "."));
			if (ui->txt_p_forum->text() == "") {
				ui->txt_p_forum->setText("0.00");
			}
			if (!ui->txt_p_forum->text().contains(".")) {
				ui->txt_p_forum->setText(ui->txt_p_forum->text() + ".00");
			}
			ui->txt_p_sfacebook->setText(ui->txt_p_sfacebook->text().remove("%").replace(",", "."));
			if (ui->txt_p_sfacebook->text() == "") {
				ui->txt_p_sfacebook->setText("0.00");
			}
			if (!ui->txt_p_sfacebook->text().contains(".")) {
				ui->txt_p_sfacebook->setText(ui->txt_p_sfacebook->text() + ".00");
			}
			ui->txt_p_stwitter->setText(ui->txt_p_stwitter->text().remove("%").replace(",", "."));
			if (ui->txt_p_stwitter->text() == "") {
				ui->txt_p_stwitter->setText("0.00");
			}
			if (!ui->txt_p_stwitter->text().contains(".")) {
				ui->txt_p_stwitter->setText(ui->txt_p_stwitter->text() + ".00");
			}
			ui->txt_p_skuponi->setText(ui->txt_p_skuponi->text().remove("%").replace(",", "."));
			if (ui->txt_p_skuponi->text() == "") {
				ui->txt_p_skuponi->setText("0.00");
			}
			if (!ui->txt_p_skuponi->text().contains(".")) {
				ui->txt_p_skuponi->setText(ui->txt_p_skuponi->text() + ".00");
			}
			ui->txt_p_obrazec->setText(ui->txt_p_obrazec->text().remove("%").replace(",", "."));
			if (ui->txt_p_obrazec->text() == "") {
				ui->txt_p_obrazec->setText("0.00");
			}
			if (!ui->txt_p_obrazec->text().contains(".")) {
				ui->txt_p_obrazec->setText(ui->txt_p_obrazec->text() + ".00");
			}
			ui->txt_p_stranka->setText(ui->txt_p_stranka->text().remove("%").replace(",", "."));
			if (ui->txt_p_stranka->text() == "") {
				ui->txt_p_stranka->setText("0.00");
			}
			if (!ui->txt_p_stranka->text().contains(".")) {
				ui->txt_p_stranka->setText(ui->txt_p_stranka->text() + ".00");
			}
			ui->txt_p_akcija->setText(ui->txt_p_akcija->text().remove("%").replace(",", "."));
			if (ui->txt_p_akcija->text() == "") {
				ui->txt_p_akcija->setText("0.00");
			}
			if (!ui->txt_p_akcija->text().contains(".")) {
				ui->txt_p_akcija->setText(ui->txt_p_akcija->text() + ".00");
			}
			ui->txt_p_vip->setText(ui->txt_p_vip->text().remove("%").replace(",", "."));
			if (ui->txt_p_vip->text() == "") {
				ui->txt_p_vip->setText("0.00");
			}
			if (!ui->txt_p_vip->text().contains(".")) {
				ui->txt_p_vip->setText(ui->txt_p_vip->text() + ".00");
			}

			QSqlQuery sql_vnesi_stranko;
			if (ui->btn_vnesi->text() == "Vnesi stranko") { // vnesi novega uporabnika
				sql_vnesi_stranko.prepare("INSERT INTO stranke (ime, priimek, naslov, davcna, kontakt, telefon, gsm, email, url, opomba, ustanova, "
										  "tip, stalnost, aktivnost, placilnost, vir, vir_besedilo, facebook, pop_facebook, twitter, pop_twitter, "
										  "google, pop_google, blog, pop_blog, forum, pop_forum, s_facebook, pop_s_facebook, s_twitter, "
										  "pop_s_twitter, s_kuponi, pop_s_kuponi, obrazec, pop_obrazec, kuponi, pop_kuponi, akcija, pop_akcija, "
										  "vip, pop_vip) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
										  "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_stranko.prepare("UPDATE stranke SET ime = ?, priimek = ?, naslov = ?, davcna = ?, kontakt = ?, telefon = ?, "
										  "gsm = ?, email = ?, url = ?, opomba = ?, ustanova = ?, tip = ?, stalnost = ?, aktivnost = ?, "
										  "placilnost = ?, vir = ?, vir_besedilo = ?, facebook = ?, pop_facebook = ?, twitter = ?, "
										  "pop_twitter = ?, google = ?, pop_google = ?, blog = ?, pop_blog = ?, forum = ?, pop_forum = ?, "
										  "s_facebook = ?, pop_s_facebook = ?, s_twitter = ?, pop_s_twitter = ?, s_kuponi = ?, pop_s_kuponi = ?, "
										  "obrazec = ?, pop_obrazec = ?, kuponi = ?, pop_kuponi = ?, akcija = ?, pop_akcija = ?, vip = ?, pop_vip = ? "
										  "WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_stranko.bindValue(0, pretvori(ui->txt_ime->text()));
			sql_vnesi_stranko.bindValue(1, pretvori(ui->txt_priimek->text()));
			sql_vnesi_stranko.bindValue(2, pretvori(ui->txt_naslov->toPlainText()));
			sql_vnesi_stranko.bindValue(3, pretvori(ui->txt_davcna->text()));
			sql_vnesi_stranko.bindValue(4, pretvori(ui->txt_kontaktna->text()));
			sql_vnesi_stranko.bindValue(5, pretvori(ui->txt_telefon->text().remove(" ")));
			sql_vnesi_stranko.bindValue(6, pretvori(ui->txt_gsm->text().remove(" ")));
			sql_vnesi_stranko.bindValue(7, pretvori(ui->txt_email->text()));
			sql_vnesi_stranko.bindValue(8, pretvori(ui->txt_spletnastran->text()));
			sql_vnesi_stranko.bindValue(9, pretvori(ui->txt_opomba->toPlainText()));
			sql_vnesi_stranko.bindValue(10, pretvori(ui->txt_ustanova->text()));

			QString vrednost = "";

			if (ui->rb_fizicna->isChecked()) {
				vrednost = "fizicna";
			}
			else {
				vrednost = "pravna";
			}
			sql_vnesi_stranko.bindValue(11, pretvori(vrednost));
			vrednost = "";

			if (ui->rb_enkratna->isChecked()) {
				vrednost = "enkratna";
			}
			else {
				vrednost = "stalna";
			}
			sql_vnesi_stranko.bindValue(12, pretvori(vrednost));
			vrednost = "";

			if (ui->rb_aktivna->isChecked()) {
				vrednost = "aktivna";
			}
			else {
				vrednost = "neaktivna";
			}
			sql_vnesi_stranko.bindValue(13, pretvori(vrednost));
			vrednost = "";

			if (ui->rb_redna->isChecked()) {
				vrednost = "redna";
			}
			else {
				vrednost = "neredna";
			}
			sql_vnesi_stranko.bindValue(14, pretvori(vrednost));
			vrednost = "";

			sql_vnesi_stranko.bindValue(15, pretvori(ui->txt_vir->currentText()));

			if (ui->txt_vir->currentText() == "Stranka") {
				vrednost = ui->txt_vir_kupon->text();
			}
			else {
				vrednost = ui->txt_vir_besedilo->text();
			}
			sql_vnesi_stranko.bindValue(16, pretvori(vrednost));

			if ( ui->cb_facebook->isChecked() ) {
				vrednost = "facebook";
			}
			sql_vnesi_stranko.bindValue(17, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(18, pretvori(ui->txt_p_facebook->text()));
			vrednost = "";

			if ( ui->cb_twitter->isChecked() ) {
				vrednost = "twitter";
			}
			sql_vnesi_stranko.bindValue(19, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(20, pretvori(ui->txt_p_twitter->text()));
			vrednost = "";

			if ( ui->cb_google->isChecked() ) {
				vrednost = "google";
			}
			sql_vnesi_stranko.bindValue(21, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(22, pretvori(ui->txt_p_google->text()));
			vrednost = "";

			if ( ui->cb_blog->isChecked() ) {
				vrednost = "blog";
			}
			sql_vnesi_stranko.bindValue(23, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(24, pretvori(ui->txt_p_blog->text()));
			vrednost = "";

			if ( ui->cb_forum->isChecked() ) {
				vrednost = "forum";
			}
			sql_vnesi_stranko.bindValue(25, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(26, pretvori(ui->txt_p_forum->text()));
			vrednost = "";

			if ( ui->cb_sfacebook->isChecked() ) {
				vrednost = "sfacebook";
			}
			sql_vnesi_stranko.bindValue(27, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(28, pretvori(ui->txt_p_sfacebook->text()));
			vrednost = "";

			if ( ui->cb_stwitter->isChecked() ) {
				vrednost = "stwitter";
			}
			sql_vnesi_stranko.bindValue(29, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(30, pretvori(ui->txt_p_stwitter->text()));
			vrednost = "";

			if ( ui->cb_skuponi->isChecked() ) {
				vrednost = "skuponi";
			}
			sql_vnesi_stranko.bindValue(31, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(32, pretvori(ui->txt_p_skuponi->text()));
			vrednost = "";

			if ( ui->cb_obrazec->isChecked() ) {
				vrednost = "obrazec";
			}
			sql_vnesi_stranko.bindValue(33, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(34, pretvori(ui->txt_p_obrazec->text()));
			vrednost = "";

			if ( ui->cb_kupon->isChecked() ) {
				vrednost = "kuponi";
			}
			sql_vnesi_stranko.bindValue(35, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(36, pretvori(ui->txt_p_stranka->text()));
			vrednost = "";

			if ( ui->cb_akcija->isChecked() ) {
				vrednost = "akcija";
			}
			sql_vnesi_stranko.bindValue(37, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(38, pretvori(ui->txt_p_akcija->text()));
			vrednost = "";

			if ( ui->cb_vip->isChecked() ) {
				vrednost = "vip";
			}
			sql_vnesi_stranko.bindValue(39, pretvori(vrednost));
			sql_vnesi_stranko.bindValue(40, pretvori(ui->txt_p_vip->text()));
			vrednost = "";

			sql_vnesi_stranko.exec();

			// delo s kuponi
			if ( ui->txt_vir->currentText() == "Stranka" ) {

				// pridobi ID stranke - vlagalca kupona
				QString stranka_id = "";
				if (ui->btn_vnesi->text() == "Vnesi stranko" && ui->txt_vir->currentText() == "Stranka" ) { // posodobi kupon
					QSqlQuery sql_rowid;
					sql_rowid.prepare("select last_insert_rowid()");
					sql_rowid.exec();
					sql_rowid.next();

					stranka_id = sql_rowid.value(0).toString();
				}
				else {
					stranka_id = ui->txt_id->text();
				}

				QSqlQuery sql_update_kuponi;
				sql_update_kuponi.prepare("UPDATE kuponi SET uporabitelj = ? WHERE kupon LIKE '" + pretvori(ui->txt_vir_kupon->text()) + "'");
				sql_update_kuponi.bindValue(0, stranka_id);
				sql_update_kuponi.exec();
			}

		}

		base.close();

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
	}
	else {
		ui->btn_vnesi->setText("Popravi vnos");
		// besedilo nosi ID ze obstojeco stranko, potrebno je napolniti polja
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
			sql_napolni.prepare("SELECT * FROM stranke WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(sql_napolni.value(sql_napolni.record().indexOf("id")).toString());
				ui->txt_ime->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ime")).toString()));
				ui->txt_priimek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("priimek")).toString()));
				ui->txt_naslov->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
				ui->txt_davcna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("davcna")).toString()));
				ui->txt_kontaktna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kontakt")).toString()));
				ui->txt_telefon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("telefon")).toString()));
				ui->txt_gsm->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("gsm")).toString()));
				ui->txt_email->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("email")).toString()));
				ui->txt_spletnastran->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("url")).toString()));
				ui->txt_opomba->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opomba")).toString()));
				ui->txt_ustanova->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ustanova")).toString()));

				if (( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip")).toString()) == "fizicna") ) {
					ui->rb_fizicna->setChecked(true);
				}
				else {
					ui->rb_pravna->setChecked(true);
				}

				if (( prevedi(sql_napolni.value(sql_napolni.record().indexOf("placilnost")).toString()) == "redna") ) {
					ui->rb_redna->setChecked(true);
				}
				else {
					ui->rb_neredna->setChecked(true);
				}

				// vir == kombo + pogojni stavki glede vir_besedilo

				QSqlQuery sql_combo;
				bool ok;
				sql_combo.prepare("SELECT * FROM sif_virstrank WHERE vir LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("vir")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_vir->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}

				if ( ui->txt_vir->currentText() == "Stranka") {
					ui->txt_vir_kupon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vir_besedilo")).toString()));
					QSqlQuery sql_kupon;
					sql_kupon.prepare("SELECT * FROM kuponi WHERE kupon LIKE '" + pretvori(ui->txt_vir_kupon->text()) + "'");
					sql_kupon.exec();
					if ( sql_kupon.next() ) {
						ui->txt_vir_id->setText(sql_kupon.value(sql_kupon.record().indexOf("id")).toString());
						ui->txt_vir_ime->setText(prevedi(sql_kupon.value(sql_kupon.record().indexOf("lastnik")).toString()));
					}
				}
				else {
					ui->txt_vir_besedilo->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vir_besedilo")).toString()));
				}


				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("facebook")).toString()) == "facebook") {
					ui->cb_facebook->setCheckState(Qt::Checked);
					ui->txt_p_facebook->setEnabled(true);
				}
				else {
					ui->cb_facebook->setCheckState(Qt::Unchecked);
					ui->txt_p_facebook->setEnabled(false);
				}
				ui->txt_p_facebook->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_facebook")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("twitter")).toString()) == "twitter") {
					ui->cb_twitter->setCheckState(Qt::Checked);
					ui->txt_p_twitter->setEnabled(true);
				}
				else {
					ui->cb_twitter->setCheckState(Qt::Unchecked);
					ui->txt_p_twitter->setEnabled(false);
				}
				ui->txt_p_twitter->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_twitter")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("google")).toString()) == "google") {
					ui->cb_google->setCheckState(Qt::Checked);
					ui->txt_p_google->setEnabled(true);
				}
				else {
					ui->cb_google->setCheckState(Qt::Unchecked);
					ui->txt_p_google->setEnabled(false);
				}
				ui->txt_p_google->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_google")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("blog")).toString()) == "blog") {
					ui->cb_blog->setCheckState(Qt::Checked);
					ui->txt_p_blog->setEnabled(true);
				}
				else {
					ui->cb_blog->setCheckState(Qt::Unchecked);
					ui->txt_p_blog->setEnabled(false);
				}
				ui->txt_p_blog->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_blog")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("forum")).toString()) == "forum") {
					ui->cb_forum->setCheckState(Qt::Checked);
					ui->txt_p_forum->setEnabled(true);
				}
				else {
					ui->cb_forum->setCheckState(Qt::Unchecked);
					ui->txt_p_forum->setEnabled(false);
				}
				ui->txt_p_forum->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_forum")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("s_facebook")).toString()) == "sfacebook") {
					ui->cb_sfacebook->setCheckState(Qt::Checked);
					ui->txt_p_sfacebook->setEnabled(true);
				}
				else {
					ui->cb_sfacebook->setCheckState(Qt::Unchecked);
					ui->txt_p_sfacebook->setEnabled(false);
				}
				ui->txt_p_sfacebook->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_s_facebook")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("s_twitter")).toString()) == "stwitter") {
					ui->cb_stwitter->setCheckState(Qt::Checked);
					ui->txt_p_stwitter->setEnabled(true);
				}
				else {
					ui->cb_stwitter->setCheckState(Qt::Unchecked);
					ui->txt_p_stwitter->setEnabled(false);
				}
				ui->txt_p_stwitter->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_s_twitter")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("s_kuponi")).toString()) == "skuponi") {
					ui->cb_skuponi->setCheckState(Qt::Checked);
					ui->txt_p_skuponi->setEnabled(true);
				}
				else {
					ui->cb_skuponi->setCheckState(Qt::Unchecked);
					ui->txt_p_skuponi->setEnabled(false);
				}
				ui->txt_p_skuponi->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_s_kuponi")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("obrazec")).toString()) == "obrazec") {
					ui->cb_obrazec->setCheckState(Qt::Checked);
					ui->txt_p_obrazec->setEnabled(true);
				}
				else {
					ui->cb_obrazec->setCheckState(Qt::Unchecked);
					ui->txt_p_obrazec->setEnabled(false);
				}
				ui->txt_p_obrazec->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_obrazec")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("kuponi")).toString()) == "kuponi") {
					ui->cb_kupon->setCheckState(Qt::Checked);
					ui->txt_p_stranka->setEnabled(true);
				}
				else {
					ui->cb_kupon->setCheckState(Qt::Unchecked);
					ui->txt_p_stranka->setEnabled(false);
				}
				ui->txt_p_stranka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_kuponi")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("akcija")).toString()) == "akcija") {
					ui->cb_akcija->setCheckState(Qt::Checked);
					ui->txt_p_akcija->setEnabled(true);
				}
				else {
					ui->cb_akcija->setCheckState(Qt::Unchecked);
					ui->txt_p_akcija->setEnabled(false);
				}
				ui->txt_p_akcija->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_akcija")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("vip")).toString()) == "vip") {
					ui->cb_vip->setCheckState(Qt::Checked);
					ui->txt_p_vip->setEnabled(true);
				}
				else {
					ui->cb_vip->setCheckState(Qt::Unchecked);
					ui->txt_p_vip->setEnabled(false);
				}
				ui->txt_p_vip->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pop_vip")).toString()));
			}
		}
		base.close();

		napolni_projekte();
		napolni_kupone();

	}
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
	if ( beseda == "kuponi" ) {
		napolni_kupone();
	}

}

void stranke::on_cb_facebook_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_facebook->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_facebook->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Facebook osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_facebook->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_facebook->setText("0.00");
		ui->txt_p_facebook->setEnabled(false);
	}

}

void stranke::on_cb_twitter_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_twitter->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_twitter")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_twitter->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_twitter")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Twitter osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_twitter->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_twitter->setText("0.00");
		ui->txt_p_twitter->setEnabled(false);
	}

}

void stranke::on_cb_google_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_google->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_google")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_google->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_google")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Google plus osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_google->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_google->setText("0.00");
		ui->txt_p_google->setEnabled(false);
	}

}

void stranke::on_cb_blog_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_blog->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_blog")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_blog->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_blog")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Blog osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_blog->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_blog->setText("0.00");
		ui->txt_p_blog->setEnabled(false);
	}

}

void stranke::on_cb_forum_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_forum->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_forum")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_forum->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_forum")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Forum osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_forum->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_forum->setText("0.00");
		ui->txt_p_forum->setEnabled(false);
	}

}

void stranke::on_cb_sfacebook_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_sfacebook->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_facebook")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_sfacebook->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_facebook")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Facebook sirjenje") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_sfacebook->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_sfacebook->setText("0.00");
		ui->txt_p_sfacebook->setEnabled(false);
	}

}

void stranke::on_cb_stwitter_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_stwitter->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_twitter")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_stwitter->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_twitter")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Twitter sirjenje") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_stwitter->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_stwitter->setText("0.00");
		ui->txt_p_stwitter->setEnabled(false);
	}

}

void stranke::on_cb_skuponi_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_skuponi->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_kuponi")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_skuponi->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_kuponi")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Kuponi - pridobil stranke") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_skuponi->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_skuponi->setText("0.00");
		ui->txt_p_skuponi->setEnabled(false);
	}

}

void stranke::on_cb_obrazec_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_obrazec->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_obrazec")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_obrazec->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_obrazec")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Obrazec akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_obrazec->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_obrazec->setText("0.00");
		ui->txt_p_obrazec->setEnabled(false);
	}

}

void stranke::on_cb_kupon_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_stranka->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kuponi")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_stranka->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kuponi")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Kupon akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_stranka->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_stranka->setText("0.00");
		ui->txt_p_stranka->setEnabled(false);
	}

}

void stranke::on_cb_akcija_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_akcija->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_akcija->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Akcijski popust akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_akcija->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_akcija->setText("0.00");
		ui->txt_p_akcija->setEnabled(false);
	}

}

void stranke::on_cb_vip_toggled(bool stanje) {

	if (stanje == true) {
		ui->rb_stalna->setChecked(true);
	}
	else {
		ui->rb_enkratna->setChecked(true);
	}

	if ( stanje == true ) {
		ui->txt_p_vip->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_vip")).toString()) != "0.00" ) { // stranka ima popust
					ui->txt_p_vip->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_vip")).toString()));
					nipopusta = "false";
				}
			}
			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori(" stranka akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_vip->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_vip->setText("0.00");
		ui->txt_p_vip->setEnabled(false);
	}

}

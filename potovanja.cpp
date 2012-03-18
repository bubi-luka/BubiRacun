#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "potovanja.h"
#include "ui_potovanja.h"
#include "glavnookno.h"
#include "kodiranje.h"

potovanja::potovanja(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::potovanja)
{
	ui->setupUi(this);

	// zbrisi vsebino polj
	ui->txt_id->setText("");
	ui->txt_stevilka_naloga->setText("");
	ui->txt_kraj_odhoda->setText("");
	ui->txt_kraj_prihoda->setText("");
	ui->txt_kilometri->setText("");
	ui->txt_naziv_ciljnega_podjetja->setText("");
	ui->txt_naslov->setText("");
	ui->txt_naslov_st->setText("");
	ui->txt_postna_stevilka->setText("");
	ui->txt_posta->clear();
	ui->txt_namen->clear();

	// onemogoci polja
	ui->txt_id->setEnabled(false);
	ui->txt_stevilka_naloga->setEnabled(false);

	// nastavi datum
	ui->txt_datum_odhoda->setDateTime(QDateTime::currentDateTime());
	ui->txt_datum_prihoda->setDateTime(QDateTime::currentDateTime());

	// doloci dovoljeno vsebino
	QDoubleValidator *validator = new QDoubleValidator(0, 999999, 2, this);
	ui->txt_kilometri->setValidator(validator);

	// napolni spustne sezname
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

		// napolni spustne sezname
		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM sif_namen_potnega_naloga");
		sql_fill_combo.exec();
		ui->txt_namen->addItem("");
		while (sql_fill_combo.next()) {
			ui->txt_namen->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ") " +
														 prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("namen")).toString()));
		}
		sql_fill_combo.clear();

		sql_fill_combo.prepare("SELECT * FROM sif_posta");
		sql_fill_combo.exec();
		ui->txt_posta->addItem("");
		while ( sql_fill_combo.next() ) {
			ui->txt_posta->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("posta")).toString()));
		}
		sql_fill_combo.clear();

	}
	base.close();

}

potovanja::~potovanja()
{
	delete ui;
}

void potovanja::on_btn_izhod_clicked() {

	close();

}

void potovanja::on_txt_posta_currentIndexChanged(QString besedilo) {

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
		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_posta WHERE posta LIKE '" + pretvori(besedilo) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_postna_stevilka->setText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("postna_stevilka")).toString()));
		}
	}
	base.close();

}

void potovanja::on_txt_postna_stevilka_textChanged(QString besedilo) {

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
		QSqlQuery sql_najdi;
		sql_najdi.prepare("SELECT * FROM sif_posta WHERE postna_stevilka LIKE '" + pretvori(besedilo) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_posta->setCurrentIndex(ui->txt_posta->findText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("posta")).toString())));
		}
	}
	base.close();

}

void potovanja::on_txt_kraj_prihoda_textChanged(QString besedilo) {

	if ( besedilo != "" ) {
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
			// baza je odprta

			QSqlQuery sql_kraj_pricetka;
			sql_kraj_pricetka.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + pretvori(ui->txt_stevilka_naloga->text()) + "' ORDER BY id ASC");
			sql_kraj_pricetka.exec();
			if ( sql_kraj_pricetka.next() ) {
				if ( prevedi(sql_kraj_pricetka.value(sql_kraj_pricetka.record().indexOf("kraj_odhoda")).toString()).toLower() == besedilo.toLower() ) {
					ui->btn_nadaljuj->setEnabled(false);
				}
				else {
					ui->btn_nadaljuj->setEnabled(true);
				}
			}

		}
		base.close();
	}

}

void potovanja::on_txt_naziv_ciljnega_podjetja_editingFinished() {

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

		QSqlQuery sql_podjetje;
		sql_podjetje.prepare("SELECT * FROM potovanja WHERE naziv_ciljnega_podjetja LIKE '" + pretvori(ui->txt_naziv_ciljnega_podjetja->text()) + "'");
		sql_podjetje.exec();
		while ( sql_podjetje.next() ) {
			ui->txt_naslov->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("naslov_ulica")).toString()));
			ui->txt_naslov_st->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("naslov_stevilka")).toString()));
			ui->txt_postna_stevilka->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("naslov_postna_stevilka")).toString()));
		}
	}
	base.close();

}

void potovanja::on_btn_nadaljuj_clicked() {

	QString napaka = preveri_napake();

	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {

		// shrani
		shrani();

		// posljemo signal za osvezevanje tabele
		poslji("potovanja");

		// ponastavimo polja
		ui->btn_sprejmi->setText("Vnesi pot");
		ui->txt_id->setText("");
		ui->txt_kraj_odhoda->setText(ui->txt_kraj_prihoda->text());
		ui->txt_kraj_prihoda->setText("");
		ui->txt_kilometri->setText("");
		ui->txt_naziv_ciljnega_podjetja->setText("");
		ui->txt_naslov->setText("");
		ui->txt_naslov_st->setText("");
		ui->txt_postna_stevilka->setText("");
		ui->txt_posta->setCurrentIndex(0);
		ui->txt_namen->setCurrentIndex(0);

		// nastavi datum
		ui->txt_datum_odhoda->setDateTime(QDateTime::fromString(ui->txt_datum_prihoda->text(), "dd.MM.yyyy HH:mm"));
		ui->txt_datum_prihoda->setDateTime(QDateTime::fromString(ui->txt_datum_prihoda->text(), "dd.MM.yyyy HH:mm").addSecs(3600));
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}

}

void potovanja::on_btn_sprejmi_clicked() {

	QString napaka = preveri_napake();

	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {

		// shrani
		shrani();

		// send signal to reload widget
		poslji("potovanja");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}

}

void potovanja::shrani() {

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
		// baza je odprta

		QSqlQuery sql_vnesi_pot;
		if (ui->btn_sprejmi->text() == "Vnesi pot") { // vnesi novo pot
			sql_vnesi_pot.prepare("INSERT INTO potovanja (potni_nalog, kraj_odhoda, kraj_prihoda, cas_odhoda, cas_prihoda, kilometri, naslov_ulica, naslov_stevilka, "
														"naslov_postna_stevilka, naslov_posta, naziv_ciljnega_podjetja, namen_potovanja) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		}
		else { // popravi ze obstojeci vnos
			sql_vnesi_pot.prepare("UPDATE potovanja SET potni_nalog = ?, kraj_odhoda = ?, kraj_prihoda = ?, cas_odhoda = ?, cas_prihoda = ?, "
														"kilometri = ?, naslov_ulica = ?, naslov_stevilka = ?, naslov_postna_stevilka = ?, naslov_posta = ?, "
														"naziv_ciljnega_podjetja = ?, namen_potovanja = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
		}
		sql_vnesi_pot.bindValue(0, pretvori(ui->txt_stevilka_naloga->text()));
		sql_vnesi_pot.bindValue(1, pretvori(ui->txt_kraj_odhoda->text()));
		sql_vnesi_pot.bindValue(2, pretvori(ui->txt_kraj_prihoda->text()));
		sql_vnesi_pot.bindValue(3, pretvori(ui->txt_datum_odhoda->text()));
		sql_vnesi_pot.bindValue(4, pretvori(ui->txt_datum_prihoda->text()));
		sql_vnesi_pot.bindValue(5, pretvori(ui->txt_kilometri->text().replace(",", ".")));
		sql_vnesi_pot.bindValue(6, pretvori(ui->txt_naslov->text()));
		sql_vnesi_pot.bindValue(7, pretvori(ui->txt_naslov_st->text()));
		sql_vnesi_pot.bindValue(8, pretvori(ui->txt_postna_stevilka->text()));
		sql_vnesi_pot.bindValue(9, pretvori(ui->txt_posta->currentText()));
		sql_vnesi_pot.bindValue(10, pretvori(ui->txt_naziv_ciljnega_podjetja->text()));
		sql_vnesi_pot.bindValue(11, pretvori(ui->txt_namen->currentText()).left(ui->txt_namen->currentText().indexOf(") ")));
		sql_vnesi_pot.exec();
	}
	base.close();

}

QString potovanja::preveri_napake() {

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

	// check that departure time is not greater or equall than arrival time
	if ( ui->txt_datum_odhoda->dateTime().secsTo(ui->txt_datum_prihoda->dateTime()) <= 0) {
		napaka = napaka + "true";
		ui->label_2->setPalette(palette_error);
		ui->label_2->setFont(font_error);
		ui->txt_datum_prihoda->setFocus();
	}

	return napaka;
}

void potovanja::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}

}

void potovanja::prejem(QString besedilo) {

	if (besedilo.left(8) == "Nova pot") {
		ui->btn_sprejmi->setText("Vnesi pot");
		ui->txt_stevilka_naloga->setText(besedilo.right(besedilo.length() - 8));
	}
	else {
		ui->btn_sprejmi->setText("Popravi pot");
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
			sql_napolni.prepare("SELECT * FROM potovanja WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_stevilka_naloga->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("potni_nalog")).toString()));
				ui->txt_kraj_odhoda->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kraj_odhoda")).toString()));
				ui->txt_kraj_prihoda->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kraj_prihoda")).toString()));
				ui->txt_kilometri->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kilometri")).toString()));
				ui->txt_naslov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_ulica")).toString()));
				ui->txt_naslov_st->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_stevilka")).toString()));
				ui->txt_postna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_postna_stevilka")).toString()));
				ui->txt_naziv_ciljnega_podjetja->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv_ciljnega_podjetja")).toString()));

				ui->txt_namen->setCurrentIndex(ui->txt_namen->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("namen_potovanja")).toString()) + ") ", Qt::MatchStartsWith));
				ui->txt_posta->setCurrentIndex(ui->txt_posta->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_posta")).toString())));

				QDateTime datum = QDateTime::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cas_odhoda")).toString()), "d'.'MM'.'yyyy' 'hh':'mm");
				ui->txt_datum_odhoda->setDateTime(datum);
				datum = QDateTime::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cas_prihoda")).toString()), "d'.'MM'.'yyyy' 'hh':'mm");
				ui->txt_datum_prihoda->setDateTime(datum);

			}
		}
		base.close();
	}
}

QString potovanja::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString potovanja::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

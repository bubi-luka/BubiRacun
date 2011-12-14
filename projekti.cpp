#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "projekti.h"
#include "ui_projekti.h"
#include "opravila.h"
#include "kodiranje.h"
#include "wid_racuni.h"
#include "wid_prejetiracuni.h"
#include "wid_potninalogi.h"

projekti::projekti(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::projekti)
{
    ui->setupUi(this);

		// izprazni vsa polja
		ui->txt_id->setText("");
		ui->txt_stevilka_projekta->setText("");
		ui->txt_naziv_projekta->setText("");
		ui->txt_stranka->clear();
		ui->txt_stranka_id->setText("");
		ui->txt_pricetek->setDate(QDate::currentDate());
		ui->txt_konec->setDate(QDate::currentDate());
		ui->txt_status_projekta->clear();
		ui->txt_st_izdanih_racunov->setText("");
		ui->txt_st_prejetih_racunov->setText("");
		ui->txt_znesek_izdanih_racunov->setText("");
		ui->txt_znesek_prejetih_racunov->setText("");
		ui->txt_znesek_skupaj->setText("");
		ui->txt_st_potnih_nalogov->setText("");
		ui->txt_st_ur_dela->setText("");

		ui->tbl_zapisi->clear();
		ui->txt_datum_zapisa->setDateTime(QDateTime::currentDateTime());
		ui->txt_naslov_zapisa->setText("");
		ui->txt_opis_zapisa->setPlainText("");

		ui->txt_izdani_brez_ddv->setText("");
		ui->txt_izdani_ddv->setText("");
		ui->txt_izdani_popusti->setText("");
		ui->txt_izdani_placilo->setText("");

		ui->txt_prejeti_brez_ddv->setText("");
		ui->txt_prejeti_ddv->setText("");
		ui->txt_prejeti_placilo->setText("");

		ui->cb_popust_fb1->setText("");
		ui->cb_popust_fb2->setText("");
		ui->cb_popust_komb1->setText("");
		ui->cb_popust_komb2->setText("");
		ui->cb_popust_stalna->setText("");
		ui->cb_popust_kupon->setText("");
		ui->cb_popust_akcija->setText("");
		ui->cb_podrazitev_vikend->setText("");
		ui->cb_podrazitev_hitrost->setText("");
		ui->cb_podrazitev_zapleti->setText("");
		ui->txt_popust_fb1->setText("");
		ui->txt_popust_fb2->setText("");
		ui->txt_popust_komb1->setText("");
		ui->txt_popust_komb2->setText("");
		ui->txt_popust_stalna_stranka->setText("");
		ui->txt_popust_kupon->setText("");
		ui->txt_popust_akcija->setText("");
		ui->txt_vsi_popusti_facebook_1->setText("");
		ui->txt_vsi_popusti_facebook_2->setText("");
		ui->txt_popusti_skupaj_1->setText("");
		ui->txt_popusti_skupaj_2->setText("");
		ui->txt_podrazitev_vikend->setText("");
		ui->txt_podrazitev_hitrost->setText("");
		ui->txt_podrazitev_zapleti->setText("");

		ui->tab_projekti->setCurrentIndex(0);

		// zacasno
		ui->tab_zapisi->setDisabled(true);
		ui->tab_potni_nalogi->setDisabled(true);
		ui->tab_popusti->setDisabled(true);

	// onemogoci polja
	ui->txt_id->setEnabled(false);
	ui->txt_stranka_id->setEnabled(false);
//	ui->txt_konec->setEnabled(false);
//	ui->txt_pricetek->setEnabled(false);
	ui->txt_izdani_brez_ddv->setEnabled(false);
	ui->txt_izdani_ddv->setEnabled(false);
	ui->txt_izdani_popusti->setEnabled(false);
	ui->txt_izdani_placilo->setEnabled(false);
	ui->txt_prejeti_brez_ddv->setEnabled(false);
	ui->txt_prejeti_ddv->setEnabled(false);
	ui->txt_prejeti_placilo->setEnabled(false);

	ui->txt_stranka_id->setHidden(true);

	// napolni tabele in spustne sezname
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

		// vnesi stevilko projekta
		QString leto = QDate::currentDate().toString("yyyy");
		int i = 1;
		QString stevilka = "";

		QSqlQuery sql_insert_stnaloga;
		sql_insert_stnaloga.prepare("SELECT * FROM projekti WHERE stprojekta LIKE '" + pretvori("SP-" + leto) + "%'");
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
		ui->txt_stevilka_projekta->setText("SP-" + leto + "-" + stevilka);

		// napolni spustne sezname
		ui->txt_status_projekta->addItem("");
		ui->txt_stranka->addItem("");

		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM sif_status_projekta");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_status_projekta->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("status")).toString()));
		}

		sql_fill_combo.clear();
		sql_fill_combo.prepare("SELECT * FROM stranke");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			if (prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("tip")).toString()) == "fizicna" ) {
				ui->txt_stranka->addItem("(" + sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
										 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("priimek")).toString()) + ", "
										 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
			}
			else {
				ui->txt_stranka->addItem("(" + sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
										 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
			}
		}
	}
	base.close();

	napolni_podatke();

}

projekti::~projekti()
{
	delete ui;

}

void projekti::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
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

// prazno, dokler ne definiramo funkcije gumba
void projekti::on_btn_brisi_clicked() {

}

void projekti::on_btn_izhod_clicked() {

	close();

}

void projekti::on_txt_stranka_currentIndexChanged(QString besedilo) {

	if (besedilo != "") {
		ui->txt_stranka_id->setText(besedilo.left(besedilo.indexOf(")", 1)));
		ui->txt_stranka_id->setText(ui->txt_stranka_id->text().right(ui->txt_stranka_id->text().length() - 1));
	}
	else {
		ui->txt_stranka_id->setText("");
	}

}

// ne preverja obveznih polj
void projekti::on_btn_sprejmi_clicked() {

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
			QSqlQuery sql_vnesi_projekt;
			if (ui->btn_sprejmi->text() == "Vnesi projekt") {
				sql_vnesi_projekt.prepare("INSERT INTO projekti (stprojekta, naziv, stranka, pricetek, konec, statusprojekta) "
										  "VALUES (?, ?, ?, ?, ?, ?)");
			}
			else {
				sql_vnesi_projekt.prepare("UPDATE projekti SET stprojekta = ?, naziv = ?, stranka = ?, pricetek = ?, konec = ?, statusprojekta = ? "
										  "WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_projekta->text()));
			sql_vnesi_projekt.bindValue(1, pretvori(ui->txt_naziv_projekta->text()));
			sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_stranka_id->text()));
			sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_pricetek->text()));
			sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_konec->text()));
			sql_vnesi_projekt.bindValue(5, pretvori(ui->txt_status_projekta->currentText()));

			sql_vnesi_projekt.exec();
		}

		base.close();

		// send signal to reload widget
		poslji("projekti");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void projekti::prejem(QString besedilo) {

	if (besedilo.left(11) == "Nov projekt") {
		ui->btn_sprejmi->setText("Vnesi projekt");
		ui->txt_stranka_id->setText(besedilo.right(besedilo.length() - 11));
		ui->wid_prejeti_racuni->setEnabled(false);
		ui->wid_izdani_racuni->setEnabled(false);
		ui->wid_potni_nalogi->setEnabled(false);
	}
	else {
		ui->btn_sprejmi->setText("Popravi vnos");
		ui->wid_prejeti_racuni->setEnabled(true);
		ui->wid_izdani_racuni->setEnabled(true);
		ui->wid_potni_nalogi->setEnabled(true);
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
			sql_napolni.prepare("SELECT * FROM projekti WHERE stprojekta LIKE '" + pretvori(besedilo) + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_stevilka_projekta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stprojekta")).toString()));
				ui->txt_naziv_projekta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()));
				ui->txt_stranka_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString()));

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pricetek")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_pricetek->setDate(datum);
//				ui->txt_pricetek->setEnabled(true);

//				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("konec")).toString()) != "0" ) { // konec projekta je odvisen od stanja combo boxa
					datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("konec")).toString()), "dd'.'MM'.'yyyy");
					ui->txt_konec->setDate(datum);
//					ui->txt_konec->setEnabled(true);
//				}

				QSqlQuery sql_combo;
				bool ok;
				sql_combo.prepare("SELECT * FROM sif_projekt WHERE projekt LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("statusprojekta")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_status_projekta->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
			}
		}
		base.close();

		// widget pri izdanih racunih
		wid_racuni *widizd = new wid_racuni;
		ui->wid_izdani_racuni->setWidget(widizd);

		QObject::connect(this, SIGNAL(prenos(QString)),
					 widizd , SLOT(prejem(QString)));
		prenos(ui->txt_stranka_id->text());
		this->disconnect();
		// widget pri prejetih racunih
		wid_prejetiracuni *widpre = new wid_prejetiracuni;
		ui->wid_prejeti_racuni->setWidget(widpre);

		QObject::connect(this, SIGNAL(prenos(QString)),
					 widpre , SLOT(prejem(QString)));
		prenos(ui->txt_stranka_id->text());
		this->disconnect();
		// widget pri potnih nalogih
		wid_potninalogi *widpot = new wid_potninalogi;
		ui->wid_potni_nalogi->setWidget(widpot);

		QObject::connect(this, SIGNAL(prenos(QString)),
					 widpot , SLOT(prejem(QString)));
		prenos(ui->txt_stranka_id->text());
		this->disconnect();

		izracunaj(); // calculate the values

	}

}

QString projekti::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString projekti::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void projekti::on_txt_stranka_id_textChanged(QString besedilo) {

	int i = 0;

	while ( i <= ui->txt_stranka->count() ) {
		if ( ui->txt_stranka->itemText(i).left(besedilo.length() + 2) == ("(" + besedilo + ")") ) {
			ui->txt_stranka->setCurrentIndex(i);
		}
		i++;
	}

	// widget pri izdanih racunih
	wid_racuni *widizd = new wid_racuni;
	ui->wid_izdani_racuni->setWidget(widizd);

	QObject::connect(this, SIGNAL(prenos(QString)),
				 widizd , SLOT(prejem(QString)));
	prenos(ui->txt_stranka_id->text());

	// widget pri prejetih racunih
	wid_prejetiracuni *widpre = new wid_prejetiracuni;
	ui->wid_prejeti_racuni->setWidget(widpre);

	QObject::connect(this, SIGNAL(prenos(QString)),
				 widpre , SLOT(prejem(QString)));
	prenos(ui->txt_stranka_id->text());

	// widget pri potnih nalogih
	wid_potninalogi *widpot = new wid_potninalogi;
	ui->wid_potni_nalogi->setWidget(widpot);

	QObject::connect(this, SIGNAL(prenos(QString)),
				 widpot , SLOT(prejem(QString)));
	prenos(ui->txt_stranka_id->text());

}

void projekti::izracunaj() {

	double d_znesekbrezddv = 0;
	double d_ddv = 0;
	double d_popusti = 0;
	double d_znesek = 0;

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
		QSqlQuery sql_racun;

		// vsota izdanih racunov
		sql_racun.prepare("SELECT * FROM racuni WHERE stranka LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			QSqlQuery sql_opravilo;
			sql_opravilo.prepare("SELECT * FROM opravila WHERE racun LIKE '" + sql_racun.value(sql_racun.record().indexOf("stracuna")).toString() + "'");
			sql_opravilo.exec();
			while ( sql_opravilo.next() ) {
				d_znesekbrezddv = d_znesekbrezddv + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesekbrezddv")).toString()).toDouble();
				d_ddv = d_ddv + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesekddv")).toString()).toDouble();
				d_popusti = d_popusti + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("popusti")).toString()).toDouble();
				d_znesek = d_znesek + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesekskupaj")).toString()).toDouble();
			}
		}
		ui->txt_izdani_brez_ddv->setText(QString::number(d_znesekbrezddv, 'f', 2));
		ui->txt_izdani_ddv->setText(QString::number(d_ddv, 'f', 2));
		ui->txt_izdani_popusti->setText(QString::number(d_popusti, 'f', 2));
		ui->txt_izdani_placilo->setText(QString::number(d_znesek, 'f', 2));

		// vsota prejetih racunov

		d_znesekbrezddv = 0;
		d_ddv = 0;
		d_popusti = 0;
		d_znesek = 0;

		sql_racun.clear();
		sql_racun.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			d_znesekbrezddv = d_znesekbrezddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesekbrezddv")).toString()).toDouble();
			d_ddv = d_ddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesekddv")).toString()).toDouble();
			d_znesek = d_znesek + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek")).toString()).toDouble();
		}
		ui->txt_prejeti_brez_ddv->setText(QString::number(d_znesekbrezddv, 'f', 2));
		ui->txt_prejeti_ddv->setText(QString::number(d_ddv, 'f', 2));
		ui->txt_prejeti_placilo->setText(QString::number(d_znesek, 'f', 2));
	}
	base.close();

}

// tabele pri prejetih, izdanih racunih in potnih nalogih niso pravilne!!!!
void projekti::napolni_podatke() {

	/*	Metoda pregleda vse izdane in prejete racune in potne naloge, ki se
	*		nanasajo na dani projekt. Sestevek zneskov in stevilk vpise v pripravljena
	*		polja. Metodo klicemo na zagonu programa, verjetno pa tudi (ce se bo dalo,
	*		ko se bo napravila kaksna sprememba v bazi podatkov.
	*/

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
		QSqlQuery sql_napolni;
		int i = 0; // steje stevilo racunov
		double znesek = 0.0; // steje koncne zneske na racunih
		double ure = 0.0; // steje ure dela na projektu

		// preveri prejete racune
		sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_projekta LIKE '" + pretvori(ui->txt_id->text()) + "'");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			i++;
			znesek += prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString()).toDouble();
		}

		// vnesi prejete racune
		ui->txt_st_prejetih_racunov->setText(QString::number(i, 10));
		ui->txt_znesek_prejetih_racunov->setText(QString::number(znesek, 'f', 2) + " EUR");

		sql_napolni.clear();
		i = 0;
		znesek = 0.0;
		ure = 0.0;

		// preveri potne naloge
		sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE stevilka_projekta LIKE '" + pretvori(ui->txt_id->text()) + "'");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			i++;
		}

		// vnesi potne naloge
		ui->txt_st_potnih_nalogov->setText(QString::number(i, 10));

		sql_napolni.clear();
		i = 0;
		znesek = 0.0;
		ure = 0.0;

		// preveri izdane racune
		sql_napolni.prepare("SELECT * FROM izdani_racuni WHERE stevilka_projekta LIKE '" + pretvori(ui->txt_id->text()) + "'");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			i++;
			QSqlQuery sql_racuni;
			sql_racuni.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("id")).toString() + "'");
			sql_racuni.exec();
			while ( sql_racuni.next() ) {
				znesek += prevedi(sql_racuni.value(sql_racuni.record().indexOf("znesek_skupaj")).toString()).toDouble();
				ure += prevedi(sql_racuni.value(sql_racuni.record().indexOf("ure")).toString()).toDouble();
			}
		}

		// vnesi izdane racune
		ui->txt_st_izdanih_racunov->setText(QString::number(i, 10));
		ui->txt_znesek_izdanih_racunov->setText(QString::number(znesek, 'f', 2) + " EUR");
		ui->txt_st_ur_dela->setText(QString::number(ure, 'f', 0));

		// izracunaj skupen zasluzek na projektu
		znesek = 0.0;
		znesek = ui->txt_znesek_izdanih_racunov->text().left(ui->txt_znesek_izdanih_racunov->text().length() - 4).toDouble();
		znesek -= ui->txt_znesek_prejetih_racunov->text().left(ui->txt_znesek_prejetih_racunov->text().length() - 4).toDouble();

		ui->txt_znesek_skupaj->setText(QString::number(znesek, 'f', 2) + " EUR");
	}
	base.close();

}

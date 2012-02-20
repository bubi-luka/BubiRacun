#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QFile>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>

#include "prejetiracuni.h"
#include "ui_prejetiracuni.h"
#include "glavnookno.h"
#include "kodiranje.h"
#include "varnost.h"

prejetiracuni::prejetiracuni(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prejetiracuni)
{
    ui->setupUi(this);

		// izbrisi polja
		ui->txt_id->setText("");
		ui->txt_stevilka_vnosa->setText("");
		ui->txt_stevilka_racuna->setText("");

		ui->txt_naziv_podjetja_kratki->setText("");
		ui->txt_naziv_podjetja_polni->setText("");
		ui->txt_ulica->setText("");
		ui->txt_hisna_stevilka->setText("");
		ui->txt_postna_stevilka->setText("");
		ui->txt_posta->clear();
		ui->txt_zadeva->setText("");

		ui->txt_datum_prejema->setDate(QDate::currentDate());
		ui->txt_datum_placila->setDate(QDate::currentDate());
		ui->txt_rok_placila->setDate(QDate::currentDate());

		ui->cb_oseba->setText("");
		ui->cb_projekt->setText("");
		ui->txt_izberi_osebo->clear();
		ui->txt_izberi_projekt->clear();
		ui->txt_podjetje->setText("");
		ui->txt_oseba->setText("");
		ui->txt_projekt->setText("");

		ui->txt_znesek_brez_ddv_20->setText("");
		ui->txt_znesek_brez_ddv_85->setText("");
		ui->txt_znesek_brez_ddv_0->setText("");
		ui->txt_znesek_ddv->setText("");
		ui->txt_znesek_brez_ddv->setText("");
		ui->txt_znesek->setText("");

		ui->txt_status_placila->clear();
		ui->txt_status_racunovodstva->clear();

		// dodamo obliko zapisa
		ui->txt_stevilka_vnosa->setInputMask("PR-9999-999;_");

		// dodamo validatorje
		QRegExp rx_valuta("([0-9]){0,15}([(.|,)][0-9]{0,2}){0,1}( (E|e)(U|u)(R|r))?");
		QValidator *v_valuta = new QRegExpValidator(rx_valuta, this);

		ui->txt_znesek_brez_ddv_0->setValidator(v_valuta);
		ui->txt_znesek_brez_ddv_85->setValidator(v_valuta);
		ui->txt_znesek_brez_ddv_20->setValidator(v_valuta);

		// dodamo prazno prvo vrstico v spustne sezname
		ui->txt_posta->addItem("");
		ui->txt_izberi_osebo->addItem("");
		ui->txt_izberi_projekt->addItem("");
		ui->txt_status_placila->addItem("");
		ui->txt_status_racunovodstva->addItem("");

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

			// napolni spustne sezname
			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_posta");
			sql_fill.exec();
			while (sql_fill.next()) {
				ui->txt_posta->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("posta")).toString()));
			}
			sql_fill.clear();

			sql_fill.prepare("SELECT * FROM uporabniki WHERE podjetje LIKE '" + pretvori(vApp->firm()) + "'");
			sql_fill.exec();
			while (sql_fill.next()) {
				QString uporabnik;
				uporabnik = prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()) + ") ";
				uporabnik += prevedi(sql_fill.value(sql_fill.record().indexOf("priimek")).toString()) + " ";
				uporabnik += prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString());
				ui->txt_izberi_osebo->addItem(uporabnik);
			}

			sql_fill.prepare("SELECT * FROM sif_status_placila");
			sql_fill.exec();
			while (sql_fill.next()) {
				ui->txt_status_placila->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("status")).toString()));
			}
			sql_fill.clear();

			sql_fill.prepare("SELECT * FROM sif_status_racunovodstva");
			sql_fill.exec();
			while (sql_fill.next()) {
				ui->txt_status_racunovodstva->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("status")).toString()));
			}
			sql_fill.clear();

			// napolnimo podatke o placniku
			sql_fill.prepare("SELECT * FROM podjetje WHERE id LIKE '" + vApp->firm() + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				ui->txt_podjetje->setText(vApp->firm() + ") " + prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString()));
			}
			sql_fill.clear();

			sql_fill.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + vApp->id() + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				ui->txt_oseba->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()) + ") " +
															 prevedi(sql_fill.value(sql_fill.record().indexOf("priimek")).toString()) + " " +
															 prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString()));
			}
			sql_fill.clear();

			sql_fill.prepare("SELECT * FROM projekti WHERE avtor_oseba LIKE '" + vApp->id() + "'");
			sql_fill.exec();
			while ( sql_fill.next() ) {
				ui->txt_izberi_projekt->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()) + ") " +
															 prevedi(sql_fill.value(sql_fill.record().indexOf("stevilka_projekta")).toString()) + " - " +
															 prevedi(sql_fill.value(sql_fill.record().indexOf("naslov_projekta")).toString()));
			}
			sql_fill.clear();

		}
		base.close();

		// nastavi stevilko prejetega racuna
		stevilka_racuna();

		// nastavimo spustne sezname na ustrezna zacetna polja
		ui->txt_posta->setCurrentIndex(0);
		ui->txt_izberi_osebo->setCurrentIndex(0);
		ui->txt_izberi_projekt->setCurrentIndex(0);
		ui->txt_status_placila->setCurrentIndex(0);
		ui->txt_status_racunovodstva->setCurrentIndex(0);

		// skrijemo spustne sezname
		ui->txt_izberi_osebo->setHidden(true);
		ui->txt_izberi_projekt->setHidden(true);

		// onemogocimo urejanje projektov
		ui->cb_projekt->setChecked(false);

}

prejetiracuni::~prejetiracuni()
{
    delete ui;
}

void prejetiracuni::on_btn_izhod_clicked() {

	close();

}

void prejetiracuni::on_btn_izpisi_clicked() {

	on_btn_sprejmi_clicked();

	print(ui->txt_id->text());

}

// ne preverja obveznih polj
void prejetiracuni::on_btn_sprejmi_clicked() {

	QString napaka = "";

	if ( ui->txt_znesek_brez_ddv_0->text() == "" ) {
		ui->txt_znesek_brez_ddv_0->setText("0.00");
	}
	if ( ui->txt_znesek_brez_ddv_85->text() == "" ) {
		ui->txt_znesek_brez_ddv_85->setText("0.00");
	}
	if ( ui->txt_znesek_brez_ddv_20->text() == "" ) {
		ui->txt_znesek_brez_ddv_20->setText("0.00");
	}

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

	// preveri obstoj stevilke narocila
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
		if ( ui->txt_stvnosa->text() == "") {
			ui->label_2->setPalette(palette_error);
			ui->label_2->setFont(font_error);
			napaka = "true";
			ui->txt_stvnosa->setFocus();
		}
		else {
			QSqlQuery sql_preveri_vnos;
			sql_preveri_vnos.prepare("SELECT * FROM prejetiracuni WHERE stevilkavnosa LIKE '" + ui->txt_stvnosa->text() + "'");
			sql_preveri_vnos.exec();
			if (sql_preveri_vnos.next()) {
				if (ui->txt_id->text() != prevedi(sql_preveri_vnos.value(sql_preveri_vnos.record().indexOf("id")).toString()) ) {
					ui->label_2->setPalette(palette_error);
					ui->label_2->setFont(font_error);
					napaka = "true";
					ui->txt_stvnosa->setFocus();
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
		base.close();
	}

	// ponovno preveri obvezno izpolnjena polja
	if (ui->txt_stracuna->text() == "") {
		napaka = "true";
		ui->label_3->setPalette(palette_error);
		ui->label_3->setFont(font_error);
	}
	else {
		ui->label_3->setPalette(palette_normal);
		ui->label_3->setFont(font_normal);
	}
	if (ui->txt_izdajatelj->toPlainText() == "") {
		napaka = "true";
		ui->label_9->setPalette(palette_error);
		ui->label_9->setFont(font_error);
	}
	else {
	   ui->label_9->setPalette(palette_normal);
	   ui->label_9->setFont(font_normal);
	}
	if (ui->txt_zadeva->text() == "") {
		napaka = "true";
		ui->label_11->setPalette(palette_error);
		ui->label_11->setFont(font_error);
	}
	else {
		ui->label_11->setPalette(palette_normal);
		ui->label_11->setFont(font_normal);
	}
	if (ui->txt_podjetje->currentText() == "") {
		napaka = "true";
		ui->label_15->setPalette(palette_error);
		ui->label_15->setFont(font_error);
	}
	else {
		ui->label_15->setPalette(palette_normal);
		ui->label_15->setFont(font_normal);
	}
	if (ui->txt_oseba->currentText() == "") {
		napaka = "true";
		ui->label_16->setPalette(palette_error);
		ui->label_16->setFont(font_error);
	}
	else {
		ui->label_16->setPalette(palette_normal);
		ui->label_16->setFont(font_normal);
	}
	if (ui->txt_znesek->text() == "") {
		napaka = "true";
		ui->label_10->setPalette(palette_error);
		ui->label_10->setFont(font_error);
	}
	else {
		ui->label_10->setPalette(palette_normal);
		ui->label_10->setFont(font_normal);
	}
*/

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
			QSqlQuery sql_oseba;

			QSqlQuery sql_vnesi;
			if (ui->btn_sprejmi->text() == "Vnesi racun") { // vnesi novega uporabnika
				sql_vnesi.prepare("INSERT INTO prejeti_racuni (stevilka_vnosa, stevilka_racuna, izdajatelj_kratki, "
													"izdajatelj_polni, ulica, hisna_stevilka, postna_stevilka, posta, zadeva, datum_prejema, "
													"datum_placila, rok_placila, placnik_podjetje, placnik_oseba, stevilka_projekta, avtor, "
													"znesek_brez_ddv_00, znesek_brez_ddv_85, znesek_brez_ddv_20, znesek_ddv, znesek_brez_ddv, "
													"znesek, status_placila, status_racunovodstva) "
													"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi.prepare("UPDATE prejeti_racuni SET stevilka_vnosa = ?, stevilka_racuna = ?, izdajatelj_kratki = ?, "
													"izdajatelj_polni = ?, ulica = ?, hisna_stevilka = ?, postna_stevilka = ?, posta = ?, "
													"zadeva = ?, datum_prejema = ?, datum_placila = ?, rok_placila = ?, placnik_podjetje = ?, "
													"placnik_oseba = ?, stevilka_projekta = ?, avtor = ?, znesek_brez_ddv_00 = ?, "
													"znesek_brez_ddv_85 = ?, znesek_brez_ddv_20 = ?, znesek_ddv = ?, znesek_brez_ddv = ?, znesek = ?, "
													"status_placila = ?, status_racunovodstva = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi.bindValue(0, pretvori(ui->txt_stevilka_vnosa->text()));
			sql_vnesi.bindValue(1, pretvori(ui->txt_stevilka_racuna->text()));
			sql_vnesi.bindValue(2, pretvori(ui->txt_naziv_podjetja_kratki->text()));
			sql_vnesi.bindValue(3, pretvori(ui->txt_naziv_podjetja_polni->text()));
			sql_vnesi.bindValue(4, pretvori(ui->txt_ulica->text()));
			sql_vnesi.bindValue(5, pretvori(ui->txt_hisna_stevilka->text()));
			sql_vnesi.bindValue(6, pretvori(ui->txt_postna_stevilka->text()));
			sql_vnesi.bindValue(7, pretvori(ui->txt_posta->currentText()));
			sql_vnesi.bindValue(8, pretvori(ui->txt_zadeva->text()));
			sql_vnesi.bindValue(9, pretvori(ui->txt_datum_prejema->text()));
			sql_vnesi.bindValue(10, pretvori(ui->txt_datum_placila->text()));
			sql_vnesi.bindValue(11, pretvori(ui->txt_rok_placila->text()));
			sql_vnesi.bindValue(12, pretvori(ui->txt_podjetje->text().left(ui->txt_podjetje->text().indexOf(") ", 0))));
			sql_vnesi.bindValue(13, pretvori(ui->txt_oseba->text().left(ui->txt_oseba->text().indexOf(") ", 0))));
			sql_vnesi.bindValue(14, pretvori(ui->txt_projekt->text().left(ui->txt_projekt->text().indexOf(") ", 0))));
			sql_vnesi.bindValue(15, pretvori(vApp->id()));
			sql_vnesi.bindValue(16, pretvori(pretvori_v_double(ui->txt_znesek_brez_ddv_0->text())));
			sql_vnesi.bindValue(17, pretvori(pretvori_v_double(ui->txt_znesek_brez_ddv_85->text())));
			sql_vnesi.bindValue(18, pretvori(pretvori_v_double(ui->txt_znesek_brez_ddv_20->text())));
			sql_vnesi.bindValue(19, pretvori(pretvori_v_double(ui->txt_znesek_ddv->text())));
			sql_vnesi.bindValue(20, pretvori(pretvori_v_double(ui->txt_znesek_brez_ddv->text())));
			sql_vnesi.bindValue(21, pretvori(pretvori_v_double(ui->txt_znesek->text())));
			sql_vnesi.bindValue(22, pretvori(ui->txt_status_placila->currentText()));
			sql_vnesi.bindValue(23, pretvori(ui->txt_status_racunovodstva->currentText()));
			sql_vnesi.exec();
		}
		base.close();

		// send signal to reload widget
		poslji("racun");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}

}

void prejetiracuni::on_txt_znesek_brez_ddv_0_editingFinished() {

	ui->txt_znesek_brez_ddv_0->setText(pretvori_iz_double(ui->txt_znesek_brez_ddv_0->text()));
	izracunaj();

}

void prejetiracuni::on_txt_znesek_brez_ddv_85_editingFinished() {

	ui->txt_znesek_brez_ddv_85->setText(pretvori_iz_double(ui->txt_znesek_brez_ddv_85->text()));
	izracunaj();

}

void prejetiracuni::on_txt_znesek_brez_ddv_20_editingFinished() {

	ui->txt_znesek_brez_ddv_20->setText(pretvori_iz_double(ui->txt_znesek_brez_ddv_20->text()));
	izracunaj();

}

void prejetiracuni::on_txt_izberi_osebo_currentIndexChanged(QString besedilo) {

	QString id = besedilo.left(besedilo.indexOf(") ")); // id izbranega uporabnika // za dolocanje njegovih projektov

	// besedilo.remove(0, besedilo.indexOf(") ", 0) + 2);
	ui->txt_oseba->setText(besedilo); // ime in priimek uporabnika // naziv podjetja

	ui->txt_izberi_projekt->clear();
	ui->txt_projekt->setText("");

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

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM projekti WHERE avtor_oseba LIKE '" + pretvori(id) + "'");
		sql_fill.exec();
		while (sql_fill.next()) {
			ui->txt_izberi_projekt->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()) + ") " +
																			prevedi(sql_fill.value(sql_fill.record().indexOf("stevilka_projekta")).toString()) + " - " +
																			prevedi(sql_fill.value(sql_fill.record().indexOf("naslov_projekta")).toString()));
		}

	}
	base.close();

}

void prejetiracuni::on_txt_izberi_projekt_currentIndexChanged(QString besedilo) {

	ui->txt_projekt->setText(besedilo.left(besedilo.indexOf(") ", 0)));

}

void prejetiracuni::on_cb_oseba_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_izberi_osebo->setHidden(false);
		ui->txt_oseba->setHidden(true);
		ui->cb_oseba->setHidden(true);
	}
	else {
		ui->txt_izberi_osebo->setHidden(true);
		ui->txt_oseba->setHidden(false);
	}

}

void prejetiracuni::on_cb_projekt_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_izberi_projekt->setHidden(false);
		ui->txt_projekt->setHidden(true);
	}
	else {
		ui->txt_izberi_projekt->setHidden(true);
		ui->txt_projekt->setHidden(false);
		ui->txt_projekt->setText("");
	}

}

void prejetiracuni::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}

}

void prejetiracuni::prejem(QString besedilo) {

	if (besedilo == "Nov racun") {
		ui->btn_sprejmi->setText("Vnesi racun");
		ui->btn_izpisi->setEnabled(false);
	}
	else {
		ui->btn_sprejmi->setText("Polnim");
		ui->btn_izpisi->setEnabled(true);
		// besedilo nosi ID ze obstojec racun, potrebno je napolniti polja
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
			sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_stevilka_vnosa->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_vnosa")).toString()));
				ui->txt_stevilka_racuna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_racuna")).toString()));
				ui->txt_naziv_podjetja_kratki->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("izdajatelj_kratki")).toString()));
				ui->txt_naziv_podjetja_polni->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("izdajatelj_polni")).toString()));
				ui->txt_ulica->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ulica")).toString()));
				ui->txt_hisna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("hisna_stevilka")).toString()));
				ui->txt_postna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("postna_stevilka")).toString()));
				ui->txt_zadeva->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("zadeva")).toString()));

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()), "dd.MM.yyyy");
				ui->txt_datum_prejema->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_placila")).toString()), "dd.MM.yyyy");
				ui->txt_datum_placila->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("rok_placila")).toString()), "dd.MM.yyyy");
				ui->txt_rok_placila->setDate(datum);

				QSqlQuery sql_podjetje;
				sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("placnik_podjetje")).toString() + "'");
				sql_podjetje.exec();
				if ( sql_podjetje.next() ) {
					ui->txt_podjetje->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("id")).toString()) + ") " +
																		prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("ime")).toString()));
				}

				QSqlQuery sql_oseba;
				sql_oseba.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("placnik_oseba")).toString() + "'");
				sql_oseba.exec();
				if ( sql_oseba.next() ) {
					ui->txt_oseba->setText(prevedi(sql_oseba.value(sql_oseba.record().indexOf("id")).toString()) + ") " +
																 prevedi(sql_oseba.value(sql_oseba.record().indexOf("priimek")).toString()) + " " +
																 prevedi(sql_oseba.value(sql_oseba.record().indexOf("ime")).toString()));
				}
				ui->txt_izberi_osebo->setCurrentIndex(ui->txt_izberi_osebo->findText(ui->txt_oseba->text()));

				QSqlQuery sql_projekt;
				sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("stevilka_projekta")).toString() + "'");
				sql_projekt.exec();
				if ( sql_projekt.next() ) {
					ui->txt_projekt->setText(prevedi(sql_projekt.value(sql_projekt.record().indexOf("id")).toString()) + ") " +
																	 prevedi(sql_projekt.value(sql_projekt.record().indexOf("stevilka_projekta")).toString()) + " - " +
																	 prevedi(sql_projekt.value(sql_projekt.record().indexOf("naslov_projekta")).toString()));
				}
				ui->txt_izberi_projekt->setCurrentIndex(ui->txt_izberi_projekt->findText(ui->txt_projekt->text()));

				if ( ui->txt_projekt->text() == "" ) {
					ui->cb_projekt->setChecked(false);
				}
				else {
					ui->cb_projekt->setChecked(true);
				}

				ui->txt_znesek_brez_ddv_0->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_brez_ddv_00")).toString()));
				ui->txt_znesek_brez_ddv_85->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_brez_ddv_85")).toString()));
				ui->txt_znesek_brez_ddv_20->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_brez_ddv_20")).toString()));
				ui->txt_znesek_ddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_ddv")).toString()));
				ui->txt_znesek_brez_ddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_brez_ddv")).toString()));
				ui->txt_znesek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString()));

				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM sif_status_placila WHERE status LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("status_placila")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_status_placila->setCurrentIndex(ui->txt_status_placila->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("status")).toString())));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_status_racunovodstva WHERE status LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("status_racunovodstva")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_status_racunovodstva->setCurrentIndex(ui->txt_status_racunovodstva->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("status")).toString())));
				}
				sql_combo.clear();
			}
			sql_napolni.clear();
		}
		base.close();

		ui->btn_sprejmi->setText("Popravi racun");
	}

}

QString prejetiracuni::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString prejetiracuni::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void prejetiracuni::izracunaj() {

	double znesek_brez_ddv = 0;
	double znesek = 0;

	/*
	* Vhodno besedilo je oblikovana stevilka v obliki XXX,XX EUR
	* preoblikujemo jo v obliko za double XXX.XX in nato sestejemo ustrezna besedila
	* na koncu sestevke preoblikujemo nazaj v XXX,XX EUR obliko
	*/

	znesek_brez_ddv = pretvori_v_double(ui->txt_znesek_brez_ddv_0->text()).toDouble();
	znesek_brez_ddv += pretvori_v_double(ui->txt_znesek_brez_ddv_20->text()).toDouble();
	znesek_brez_ddv += pretvori_v_double(ui->txt_znesek_brez_ddv_85->text()).toDouble();

	znesek = pretvori_v_double(ui->txt_znesek_brez_ddv_0->text()).toDouble();
	znesek += (pretvori_v_double(ui->txt_znesek_brez_ddv_20->text()).toDouble() * 1.2);
	znesek += (pretvori_v_double(ui->txt_znesek_brez_ddv_85->text()).toDouble() * 1.085);

	ui->txt_znesek->setText(pretvori_iz_double(QString::number(znesek, 'f', 2)));
	ui->txt_znesek_brez_ddv->setText(pretvori_iz_double(QString::number(znesek_brez_ddv, 'f', 2)));
	ui->txt_znesek_ddv->setText(pretvori_iz_double(QString::number(znesek - znesek_brez_ddv, 'f', 2)));

}

QString prejetiracuni::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	return besedilo;

}

QString prejetiracuni::pretvori_iz_double(QString besedilo) {

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

void prejetiracuni::on_txt_posta_currentIndexChanged(QString besedilo) {

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
		sql_najdi.prepare("SELECT * FROM sif_posta WHERE posta LIKE '" + pretvori(ui->txt_posta->currentText()) + "'");
		sql_najdi.exec();
		if ( sql_najdi.next() ) {
			ui->txt_postna_stevilka->setText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("postna_stevilka")).toString()));
		}
	}
	base.close();

}

void prejetiracuni::on_txt_postna_stevilka_textChanged(QString besedilo) {

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

void prejetiracuni::on_txt_naziv_podjetja_kratki_editingFinished() {

	if ( ui->btn_sprejmi->text() == "Vnesi racun" ) {
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
			sql_podjetje.prepare("SELECT * FROM prejeti_racuni WHERE izdajatelj_kratki LIKE '" + pretvori(ui->txt_naziv_podjetja_kratki->text()) + "'");
			sql_podjetje.exec();
			while ( sql_podjetje.next() ) {
				ui->txt_naziv_podjetja_polni->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("izdajatelj_polni")).toString()));
				ui->txt_ulica->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("ulica")).toString()));
				ui->txt_hisna_stevilka->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("hisna_stevilka")).toString()));
				ui->txt_postna_stevilka->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("postna_stevilka")).toString()));
				ui->txt_zadeva->setText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("zadeva")).toString()));
			}
		}
		base.close();
	}

}

void prejetiracuni::print(QString id) {

	QPrinter printer;

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Natisni prejet racun"));
	printer.setPaperSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

	if (dialog->exec() == QDialog::Accepted) {
		QPainter painter;

		if (! painter.begin(&printer))  { // failed to open file
			qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
			return;
		}

		// nastavi pisavo
		painter.setFont(QFont("Arial", 12));

		// spremenljivke
		QString stevilka_vnosa = "";
		QString stevilka_racuna = "";
		QString izdajatelj_racuna = "";
		QString zadeva = "";
		QString placnik = "";
		QString projekt = "";
		QString datum_prejema = "";
		QString datum_placila = "";
		QString rok_placila = "";
		QString znesek_brez_ddv_00 = "";
		QString znesek_brez_ddv_85 = "";
		QString znesek_brez_ddv_20 = "";
		QString znesek_ddv = "";
		QString znesek = "";

		int pozicija = 0;

		// napolnimo spremenljivke z besedilom
		stevilka_vnosa = "Stevilka vnosa: ";
		stevilka_racuna = "Stevilka prejetega racuna: ";
		izdajatelj_racuna = "Izdajatelj racuna: ";
		zadeva = "Zadeva: ";
		placnik = "Placnik racuna: ";
		projekt = "Stevilka projekta: ";
		datum_prejema = "Datum prejema racuna: ";
		datum_placila = "Datum placila racuna: ";
		rok_placila = "Rok placila racuna: ";
		znesek_brez_ddv_00 = "Znesek brez DDV 0,0%: ";
		znesek_brez_ddv_85 = "Znesek brez DDV 8,5%: ";
		znesek_brez_ddv_20 = "Znesek brez DDV 20,0%: ";
		znesek_ddv = "Znesek DDV: ";
		znesek = "Koncni znesek: ";

		// odpremo bazo in napolnimo spremenljivke
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
			QSqlQuery sql_prejeti_racuni;
			sql_prejeti_racuni.prepare("SELECT * FROM prejeti_racuni WHERE id LIKE '" + pretvori(id) + "'");
			sql_prejeti_racuni.exec();
			if ( sql_prejeti_racuni.next() ) {
				stevilka_vnosa += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_vnosa")).toString());
				stevilka_racuna += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_racuna")).toString());
				izdajatelj_racuna += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("izdajatelj_polni")).toString());
				zadeva += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("zadeva")).toString());
				datum_prejema += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("datum_prejema")).toString());;
				datum_placila += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("datum_placila")).toString());
				rok_placila += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("rok_placila")).toString());
				if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_00")).toString()) == "0.00" ) {
					znesek_brez_ddv_00 = "";
				}
				else {
					znesek_brez_ddv_00 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_00")).toString()).replace(".", ",") + " EUR";
				}
				if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_85")).toString()) == "0.00" ) {
					znesek_brez_ddv_85 = "";
				}
				else {
					znesek_brez_ddv_85 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_85")).toString()).replace(".", ",") + " EUR";
				}
				if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_20")).toString()) == "0.00" ) {
					znesek_brez_ddv_20 = "";
				}
				else {
					znesek_brez_ddv_20 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_20")).toString()).replace(".", ",") + " EUR";
				}
				znesek_ddv += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_ddv")).toString()).replace(".", ",") + " EUR";
				znesek += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek")).toString()).replace(".", ",") + " EUR";

				QSqlQuery sql_placnik;
				sql_placnik.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("placnik_podjetje")).toString() + "'");
				sql_placnik.exec();
				if ( sql_placnik.next() ) {
					placnik += prevedi(sql_placnik.value(sql_placnik.record().indexOf("polnoime")).toString());
				}

				QSqlQuery sql_projekt;
				sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_projekta")).toString() + "'");
				sql_projekt.exec();
				if ( sql_projekt.next() ) {
					projekt += prevedi(sql_projekt.value(sql_projekt.record().indexOf("stevilka_projekta")).toString());
				}

			}
		}
		base.close();

		// pripravimo dokument za tiskanje
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, stevilka_racuna);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, stevilka_racuna);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, izdajatelj_racuna);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, izdajatelj_racuna);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, zadeva);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, zadeva);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, placnik);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, placnik);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, projekt);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, projekt);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, datum_prejema);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, datum_prejema);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, datum_placila);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, datum_placila);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, rok_placila);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, rok_placila);
		pozicija += velikost_besedila.height();
		if ( znesek_brez_ddv_00 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_00);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_00);
			pozicija += velikost_besedila.height();
		}
		if ( znesek_brez_ddv_85 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_85);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_85);
			pozicija += velikost_besedila.height();
		}
		if ( znesek_brez_ddv_20 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_20);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_20);
			pozicija += velikost_besedila.height();
		}
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_ddv);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_ddv);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek);
		pozicija += velikost_besedila.height();

		painter.end();
	}

}

void prejetiracuni::on_txt_datum_prejema_dateChanged() {

	stevilka_racuna();

}

void prejetiracuni::stevilka_racuna() {

	if ( ui->btn_sprejmi->text() != "Polnim" ) {
		// dobimo tekoce leto za uporabo pri stevilki racuna
		QString leto = ui->txt_datum_prejema->text().right(4);

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

			// pridobi naslednjo prosto stevilko vnosa in jo uredi glede na navodila
			int i = 1;
			QString stevilka = "";
			QSqlQuery sql_insert_stnaloga;
			sql_insert_stnaloga.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_vnosa LIKE '" + pretvori("PR-" + leto) + "%'");
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
			ui->txt_stevilka_vnosa->setText("PR-" + leto + "-" + stevilka);

		}
		base.close();
	}

}

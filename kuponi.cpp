#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "kuponi.h"
#include "ui_kuponi.h"
#include "glavnookno.h"
#include "kodiranje.h"

kuponi::kuponi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kuponi)
{
    ui->setupUi(this);

	// set certain fields as disabled
	ui->txt_id->setDisabled(true);
	ui->txt_datumuveljavitve->setDisabled(true);
	ui->txt_uveljavitelj->setDisabled(true);
	ui->txt_uveljavitelj_id->setDisabled(true);
	ui->txt_uporabljen->setChecked(false);

	// set date
	ui->txt_datumpridobitve->setDate(QDate::currentDate());
	ui->txt_datumuveljavitve->setDate(QDate::currentDate());

	// set kupon number
	QString leto = QDate::currentDate().toString("yyyy");
	leto = leto.insert(3, "\\");
	leto = leto.insert(2, "\\");
	leto = leto.insert(1, "\\");
	leto = leto.insert(0, "\\");

	ui->txt_kupon->setInputMask("KU-" + leto + "-999;_");

	leto = QDate::currentDate().toString("yyyy");

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

		// fill combo boxes
		ui->txt_prejemnik->addItem("");
		ui->txt_uveljavitelj->addItem("");

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM stranke");
		sql_fill.exec();
		while (sql_fill.next()) {
			QString ime = "";
			if (prevedi(sql_fill.value(sql_fill.record().indexOf("tip")).toString()) == "fizicna") {
				ime = prevedi(sql_fill.value(sql_fill.record().indexOf("Priimek")).toString()) + ", " + prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString());
			}
			else {
				ime = prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString());
			}
			ui->txt_prejemnik->addItem("(" + prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()) + ") " + ime);
			ui->txt_uveljavitelj->addItem("(" + prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()) + ") " + ime);
		}

		// create cupon number
		int i = 1;
		QString stevilka = "";
		QSqlQuery sql_insert_stnaloga;
		sql_insert_stnaloga.prepare("SELECT * FROM kuponi WHERE kupon LIKE 'KU-" + leto + "%'");
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
		ui->txt_kupon->setText("KU-" + leto + "-" + stevilka);

	}
	base.close();

}

kuponi::~kuponi()
{
    delete ui;
}

void kuponi::on_btn_izhod_clicked() {

	close();

}

// prazno, dokler se ne dogovorimo o dejavnosti gumba
void kuponi::on_btn_brisi_clicked() {

}

// ne preverja obveznih polj
void kuponi::on_btn_sprejmi_clicked() {

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

	// preveri uporabnisko ime, ce ze obstaja
	if (ui->txt_uporabnik->text() == "") {
		ui->label_4->setPalette(palette_error);
		ui->label_4->setFont(font_error);
		napaka = "true";
		ui->txt_uporabnik->setFocus();
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
			QSqlQuery sql_vnesi_kupon;
			if (ui->btn_sprejmi->text() == "Vnesi kupon") { // vnesi novega uporabnika
				sql_vnesi_kupon.prepare("INSERT INTO kuponi (kupon, projekt, prejemnik, datumprejema, uporabljen, uporabitelj, datumuporabe) "
											 "VALUES (?, ?, ?, ?, ?, ?, ?) ");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_kupon.prepare("UPDATE kuponi SET kupon = ?, projekt = ?, prejemnik = ?, datumprejema = ?, uporabljen = ?, uporabitelj = ?, "
											 "datumuporabe = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_kupon.bindValue(0, pretvori(ui->txt_kupon->text()));
			sql_vnesi_kupon.bindValue(1, pretvori(ui->txt_projekt->text()));
			sql_vnesi_kupon.bindValue(2, pretvori(ui->txt_prejemnik_id->text()));
			sql_vnesi_kupon.bindValue(3, pretvori(ui->txt_datumpridobitve->text()));
			QString uveljavitev;
			QString uveljavitelj;
			QString datum;
			if ( ui->txt_uporabljen->isChecked() ) {
				uveljavitev = pretvori("uveljavljen");
				uveljavitelj = pretvori(ui->txt_uveljavitelj_id->text());
				datum = pretvori(ui->txt_datumuveljavitve->text());
			}
			else {
				uveljavitev = pretvori("neuveljavljen");
				uveljavitelj = "";
				datum = "";
			}
			sql_vnesi_kupon.bindValue(4, uveljavitev);
			sql_vnesi_kupon.bindValue(5, uveljavitelj);
			sql_vnesi_kupon.bindValue(6, datum);
			sql_vnesi_kupon.exec();
		}
		base.close();

		// send signal to reload widget
		poslji("kuponi");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void kuponi::keyPressEvent(QKeyEvent *event) {
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

// prazno
void kuponi::prejem(QString besedilo) {

	if (besedilo.left(9) == "Nov kupon") {
		ui->btn_sprejmi->setText("Vnesi kupon");
		ui->txt_prejemnik_id->setText(besedilo.right(besedilo.length() - 9));
	}
	else {
		ui->btn_sprejmi->setText("Popravi kupon");
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
			sql_napolni.prepare("SELECT * FROM kuponi WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_kupon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kupon")).toString()));
				ui->txt_projekt->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("projekt")).toString()));

				bool ok;
				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM stranke WHERE id LIKE '" + prevedi(sql_napolni.value(sql_napolni.record().indexOf("prejemnik")).toString()) + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_prejemnik->setCurrentIndex(prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()).toInt(&ok, 10));
				}

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datumprejema")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datumpridobitve->setDate(datum);

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("uporabljen")).toString()) == "uveljavljen" ) {
					ui->txt_uporabljen->setChecked(true);
					ui->txt_uveljavitelj->setEnabled(true);
					ui->txt_uveljavitelj_id->setEnabled(true);
					ui->txt_datumuveljavitve->setEnabled(true);
					sql_combo.clear();
					sql_combo.prepare("SELECT * FROM stranke WHERE id LIKE '" + prevedi(sql_napolni.value(sql_napolni.record().indexOf("uporabitelj")).toString()) + "'");
					sql_combo.exec();
					if (sql_combo.next()) {
						ui->txt_uveljavitelj->setCurrentIndex(prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()).toInt(&ok, 10));
					}

					datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datumuporabe")).toString()), "dd'.'MM'.'yyyy");
					ui->txt_datumuveljavitve->setDate(datum);
				}
				else {
					ui->txt_uporabljen->setChecked(false);
					ui->txt_uveljavitelj->setEnabled(false);
					ui->txt_uveljavitelj_id->setEnabled(false);
					ui->txt_datumuveljavitve->setEnabled(false);
				}
			}
		}
		base.close();
	}
}

QString kuponi::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString kuponi::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void kuponi::on_txt_prejemnik_currentIndexChanged(QString besedilo) {

	if (besedilo != "") {
		ui->txt_prejemnik_id->setText(besedilo.left(besedilo.indexOf(")", 1)));
		ui->txt_prejemnik_id->setText(ui->txt_prejemnik_id->text().right(ui->txt_prejemnik_id->text().length() - 1));
	}
	else {
		ui->txt_prejemnik_id->setText("");
	}

}

void kuponi::on_txt_uveljavitelj_currentIndexChanged(QString besedilo) {

	if (besedilo != "") {
		ui->txt_uveljavitelj_id->setText(besedilo.left(besedilo.indexOf(")", 1)));
		ui->txt_uveljavitelj_id->setText(ui->txt_uveljavitelj_id->text().right(ui->txt_uveljavitelj_id->text().length() - 1));
	}
	else {
		ui->txt_uveljavitelj_id->setText("");
	}

}

void kuponi::on_txt_uporabljen_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_datumuveljavitve->setEnabled(true);
		ui->txt_uveljavitelj->setEnabled(true);
		ui->txt_uveljavitelj_id->setEnabled(true);
	}
	else {
		ui->txt_datumuveljavitve->setEnabled(false);
		ui->txt_uveljavitelj->setEnabled(false);
		ui->txt_uveljavitelj_id->setEnabled(false);
	}

}

void kuponi::on_txt_prejemnik_id_textChanged(QString besedilo) {

	int i = 0;
	bool *ok;

	while ( i <= ui->txt_prejemnik->count() ) {
		if ( ui->txt_prejemnik->itemText(i).left(besedilo.length() + 2) == ("(" + besedilo + ")") ) {
			ui->txt_prejemnik->setCurrentIndex(i);
		}
		i++;
	}

}

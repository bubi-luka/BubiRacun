#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QFile>

#include "prejetiracuni.h"
#include "ui_prejetiracuni.h"
#include "glavnookno.h"
#include "kodiranje.h"

prejetiracuni::prejetiracuni(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prejetiracuni)
{
    ui->setupUi(this);

	ui->txt_stvnosa->setInputMask("PR-9999-999;_");

	QRegExp rx_validate("[0-9,]*");
	QValidator *validator = new QRegExpValidator(rx_validate, this);

	ui->txt_znesekddv_0->setValidator(validator);
	ui->txt_znesekddv_85->setValidator(validator);
	ui->txt_znesekddv_20->setValidator(validator);

	ui->txt_prejem->setDate(QDate::currentDate());
	ui->txt_placilo->setDate(QDate::currentDate());
	ui->txt_rok->setDate(QDate::currentDate());

	ui->txt_podjetje->addItem("");
	ui->txt_oseba->addItem("");
	ui->txt_statusracuna->addItem("");
	ui->txt_statusplacila->addItem("");
	ui->txt_statusracunovodstva->addItem("");

	QString leto = QDate::currentDate().toString("yyyy");

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
		int i = 1;
		QString stevilka = "";
		QSqlQuery sql_insert_stnaloga;
		sql_insert_stnaloga.prepare("SELECT * FROM prejetiracuni WHERE stevilkavnosa LIKE '" + pretvori("PR-" + leto) + "%'");
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
		ui->txt_stvnosa->setText("PR-" + leto + "-" + stevilka);

		//fill combo
		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM podjetje");
		sql_fill.exec();
		while (sql_fill.next()) {
			ui->txt_podjetje->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString()));
			ui->txt_oseba->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("odgovornaoseba")).toString()));
		}
		sql_fill.clear();
		sql_fill.prepare("SELECT * FROM sif_racun");
		sql_fill.exec();
		while (sql_fill.next()) {
			ui->txt_statusracuna->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("racun")).toString()));
		}
		sql_fill.clear();
		sql_fill.prepare("SELECT * FROM sif_pplacilo");
		sql_fill.exec();
		while (sql_fill.next()) {
			ui->txt_statusplacila->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("placilo")).toString()));
		}
		sql_fill.clear();
		sql_fill.prepare("SELECT * FROM sif_racunovodstvo");
		sql_fill.exec();
		while (sql_fill.next()) {
			ui->txt_statusracunovodstva->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("racunovodstvo")).toString()));
		}
		sql_fill.clear();
	}
	base.close();

	ui->txt_podjetje->setCurrentIndex(0);
	ui->txt_oseba->setCurrentIndex(0);
	ui->txt_statusracuna->setCurrentIndex(2);
	ui->txt_statusplacila->setCurrentIndex(0);
	ui->txt_statusracunovodstva->setCurrentIndex(0);

}

prejetiracuni::~prejetiracuni()
{
    delete ui;
}

void prejetiracuni::on_btn_izhod_clicked() {

	close();

}

void prejetiracuni::on_btn_izpisi_clicked() {

	// v string vsebina se shrani celotno besedilo enega prejetega racuna
	QString vsebina;

// odpri podatke o prejetem racunu
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
		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM prejetiracuni WHERE id LIKE '" + ui->txt_id->text() + "'");
		sql_racun.exec();
		if ( sql_racun.next() ) {
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("stevilkavnosa")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("stevilkaracuna")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("posiljatelj")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("zadeva")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("datumprejema")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("datumplacila")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("rokplacila")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("oseba")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("znesek")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("znesekbrezddv")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("znesekddv")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("ddv00")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("ddv85")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("ddv20")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("statusracuna")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("statusplacila")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("statusracunovodstvo")).toString()));
		}
	}
	base.close();

	QFile file1("prejetiracun.cvs");
	if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file1);
	out << vsebina;
	file1.close();

}

void prejetiracuni::on_btn_sprejmi_clicked() {

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
			QSqlQuery sql_vnesi;
			if (ui->btn_sprejmi->text() == "Vnesi racun") { // vnesi novega uporabnika
				sql_vnesi.prepare("INSERT INTO prejetiracuni (stevilkavnosa, stevilkaracuna, posiljatelj, zadeva, datumprejema, "
											 "datumplacila, rokplacila, podjetje, oseba, znesek, znesekbrezddv, znesekddv, ddv00, ddv85, "
											 "ddv20, statusracuna, statusplacila, statusracunovodstvo) "
											 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi.prepare("UPDATE prejetiracuni SET stevilkavnosa = ?, stevilkaracuna = ?, posiljatelj = ?, zadeva = ?, "
								  "datumprejema = ?, datumplacila = ?, rokplacila = ?, podjetje = ?, oseba = ?, znesek = ?, znesekbrezddv = ?, "
								  "znesekddv = ?, ddv00 = ?, ddv85 = ?, ddv20 = ?, statusracuna = ?, statusplacila = ?, "
								  "statusracunovodstvo = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi.bindValue(0, pretvori(ui->txt_stvnosa->text()));
			sql_vnesi.bindValue(1, pretvori(ui->txt_stracuna->text()));
			sql_vnesi.bindValue(2, pretvori(ui->txt_izdajatelj->toPlainText()));
			sql_vnesi.bindValue(3, pretvori(ui->txt_zadeva->text()));
			sql_vnesi.bindValue(4, pretvori(ui->txt_prejem->text()));
			sql_vnesi.bindValue(5, pretvori(ui->txt_placilo->text()));
			sql_vnesi.bindValue(6, pretvori(ui->txt_rok->text()));
			sql_vnesi.bindValue(7, pretvori(ui->txt_podjetje->currentText()));
			sql_vnesi.bindValue(8, pretvori(ui->txt_oseba->currentText()));
			sql_vnesi.bindValue(9, pretvori(ui->txt_znesek->text()));
			sql_vnesi.bindValue(10, pretvori(ui->txt_brezddv->text()));
			sql_vnesi.bindValue(11, pretvori(ui->txt_znesekddv->text()));
			sql_vnesi.bindValue(12, pretvori(ui->txt_znesekddv_0->text()));
			sql_vnesi.bindValue(13, pretvori(ui->txt_znesekddv_85->text()));
			sql_vnesi.bindValue(14, pretvori(ui->txt_znesekddv_20->text()));
			sql_vnesi.bindValue(15, pretvori(ui->txt_statusracuna->currentText()));
			sql_vnesi.bindValue(16, pretvori(ui->txt_statusplacila->currentText()));
			sql_vnesi.bindValue(17, pretvori(ui->txt_statusracunovodstva->currentText()));
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

void prejetiracuni::on_txt_znesekddv_0_editingFinished() {

	// pretvori stevilke v double obliko
	ui->txt_znesekddv_0->setText(ui->txt_znesekddv_0->text().replace(",", "."));

	izracunaj();

}

void prejetiracuni::on_txt_znesekddv_85_editingFinished() {

	// pretvori stevilke v double obliko
	ui->txt_znesekddv_85->setText(ui->txt_znesekddv_85->text().replace(",", "."));

	izracunaj();

}

void prejetiracuni::on_txt_znesekddv_20_editingFinished() {

	// pretvori stevilke v double obliko
	ui->txt_znesekddv_20->setText(ui->txt_znesekddv_20->text().replace(",", "."));

	izracunaj();

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

// zaprto
void prejetiracuni::prejem(QString besedilo) {

	if (besedilo == "Nov racun") {
		ui->btn_sprejmi->setText("Vnesi racun");
	}
	else {
		ui->btn_sprejmi->setText("Popravi racun");
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
			sql_napolni.prepare("SELECT * FROM prejetiracuni WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_stvnosa->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilkavnosa")).toString()));
				ui->txt_stracuna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilkaracuna")).toString()));
				ui->txt_izdajatelj->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("posiljatelj")).toString()));
				ui->txt_zadeva->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("zadeva")).toString()));

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datumprejema")).toString()), "dd.MM.yyyy");
				ui->txt_prejem->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datumplacila")).toString()), "dd.MM.yyyy");
				ui->txt_placilo->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("rokplacila")).toString()), "dd.MM.yyyy");
				ui->txt_rok->setDate(datum);

				bool ok;
				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM podjetje WHERE ime LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("podjetje")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_podjetje->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM podjetje WHERE odgovornaoseba LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("oseba")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_oseba->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();

				ui->txt_znesek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString()));
				ui->txt_brezddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesekbrezddv")).toString()));
				ui->txt_znesekddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesekddv")).toString()));
				ui->txt_znesekddv_0->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv00")).toString()));
				ui->txt_znesekddv_85->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv85")).toString()));
				ui->txt_znesekddv_20->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv20")).toString()));


				sql_combo.prepare("SELECT * FROM sif_racun WHERE racun LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("statusracuna")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_statusracuna->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_pplacilo WHERE placilo LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("statusplacila")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_statusplacila->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_racunovodstvo WHERE racunovodstvo LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("statusracunovodstvo")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_statusracunovodstva->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();
			}
		}
		base.close();
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

	znesek_brez_ddv = ui->txt_znesekddv_0->text().toDouble() + ui->txt_znesekddv_20->text().toDouble() + ui->txt_znesekddv_85->text().toDouble();
	znesek = ui->txt_znesekddv_0->text().toDouble() + (ui->txt_znesekddv_20->text().toDouble()*1.2) + (ui->txt_znesekddv_85->text().toDouble()*1.085);

	ui->txt_znesek->setText(QString::number(znesek, 'f', 2));
	ui->txt_brezddv->setText(QString::number(znesek_brez_ddv, 'f', 2));
	ui->txt_znesekddv->setText(QString::number(znesek - znesek_brez_ddv, 'f', 2));

}

#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QFile>

#include "racun.h"
#include "ui_racun.h"
#include "wid_racuni.h"
#include "projekti.h"
#include "opravila.h"
#include "kodiranje.h"

racun::racun(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::racun)
{
    ui->setupUi(this);

	// disable fields
	ui->txt_id->setEnabled(false);
	ui->txt_projektid->setEnabled(false);
	ui->txt_strankaid->setEnabled(false);
	ui->txt_z_ddv->setEnabled(false);
	ui->txt_z_popusti->setEnabled(false);
	ui->txt_z_seplacati->setEnabled(false);
	ui->txt_z_znesek->setEnabled(false);
	ui->txt_z_znesekbrezddv->setEnabled(false);

	// hide fields
	ui->txt_projektid->setVisible(false);
	ui->txt_strankaid->setVisible(false);

	// set racun as checked
	ui->rb_racun->setChecked(true);

	// fill combo box
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
		// insert project number
		QString leto = QDate::currentDate().toString("yyyy");
		int i = 1;
		QString stevilka = "";

		QSqlQuery sql_insert_stracuna;
		sql_insert_stracuna.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori("SR-" + leto) + "%'");
		sql_insert_stracuna.exec();
		while (sql_insert_stracuna.next()) {
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
		ui->txt_stracuna->setText("SR-" + leto + "-" + stevilka);

		// fill combo box
		ui->txt_projekt->addItem("");
		ui->txt_stranka->addItem("");
		ui->txt_stplacila->addItem("");
		ui->txt_stracunovodstva->addItem("");

		QSqlQuery sql_fill_combo;
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

		sql_fill_combo.clear();
		sql_fill_combo.prepare("SELECT * FROM sif_placilo");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_stplacila->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("placilo")).toString()));
		}
		sql_fill_combo.clear();
		sql_fill_combo.prepare("SELECT * FROM sif_racunovodstvo");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_stracunovodstva->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("racunovodstvo")).toString()));
		}

	}
	base.close();

}

racun::~racun()
{
    delete ui;
}

void racun::on_btn_izpisi_clicked() {


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
		sql_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + ui->txt_id->text() + "'");
		sql_racun.exec();
		if ( sql_racun.next() ) {

			// podatki o podjetju
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // kratki naziv podjetja
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // polni naziv podjetja
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // naslov podjetja
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // davcna st. podjetja

			// podatki o stranki
			QSqlQuery sql_stranka;
			sql_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
			sql_stranka.exec();
			if ( sql_stranka.next() ) {
				if ( prevedi(sql_stranka.value(sql_stranka.record().indexOf("tip")).toString()) == "pravna" ) {
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("ime")).toString()) + ";");
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("priimek")).toString()) + ";");
				}
				else {
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("ime")).toString()) + " ");
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("priimek")).toString()) + ";");
					vsebina.append(";");
				}
				vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("naslov")).toString()) + ";");
				vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("davcna")).toString()) + ";");
			}

			// podatki o racunu
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("tipracuna")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("stracuna")).toString()) + ";");
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // datum izdaje racuna
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // kraj izdaje racuna = kraj podjetja
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // obdobje storitve
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // datum roka placila
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString()) + ";"); // trr podjetja

			// podatki o storitvah
			QSqlQuery sql_storitve;
			sql_storitve.prepare("SELECT * FROM opravila WHERE racun LIKE '" + sql_racun.value(sql_racun.record().indexOf("id")).toString() + "'");
			sql_storitve.exec();
			double znesekbrezddv;
			double ddv20;
			double ddv85;
			double ddv00;
			double popusti;
			double znesek;
			if ( sql_storitve.next() ) {
				vsebina.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()) + ";");
				vsebina.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("ure")).toString()) + ";");
				vsebina.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("cena_ure")).toString()) + ";");
				vsebina.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popusti")).toString()) + ";");
				vsebina.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("ddv")).toString()) + ";");
				vsebina.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesekbrezddv")).toString()) + ";");

				znesekbrezddv = znesekbrezddv + prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesekbrezddv")).toString()).toDouble();
				popusti = popusti + prevedi(sql_storitve.value(sql_storitve.record().indexOf("popusti")).toString()).toDouble();
				znesek = znesek + prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesekskupaj")).toString()).toDouble();
				if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("ddv")).toString()) == "20,0%" )
					ddv20 = ddv20 + prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()).toDouble();
				if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("ddv")).toString()) == "8,5%" )
					ddv85 = ddv85 + prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()).toDouble();
				if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("ddv")).toString()) == "0,0%" )
					ddv00 = ddv00 + prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()).toDouble();
			}

			// koncni znesek
			vsebina.append(QString::number(znesekbrezddv, 'f', 2) + ";");
			vsebina.append(QString::number(ddv20, 'f', 2) + ";");
			vsebina.append(QString::number(ddv85, 'f', 2) + ";");
			vsebina.append(QString::number(ddv00, 'f', 2) + ";");
			vsebina.append(QString::number(znesek, 'f', 2) + ";");

			// podatki o izdajatelju racuna
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("id")).toString())); // izdajatelj racuna
		}
	}
	base.close();

	QFile file1("izdaniracuni.cvs");
	if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file1);
	out << vsebina;
	file1.close();

}

void racun::on_btn_izhod_clicked() {

	close();

}

void racun::on_txt_stranka_currentIndexChanged(QString besedilo) {

	if (besedilo != "") {
		ui->txt_strankaid->setText(besedilo.left(besedilo.indexOf(")", 1)));
		ui->txt_strankaid->setText(ui->txt_strankaid->text().right(ui->txt_strankaid->text().length() - 1));
	}
	else {
		ui->txt_strankaid->setText("");
	}

}

void racun::on_txt_strankaid_textChanged(QString besedilo) {

	int i = 0;

	while ( i <= ui->txt_stranka->count() ) {
		if ( ui->txt_stranka->itemText(i).left(besedilo.length() + 2) == ("(" + besedilo + ")") ) {
			ui->txt_stranka->setCurrentIndex(i);
		}
		i++;
	}

	// empty combo box
	ui->txt_projekt->clear();

	// fill combo box
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
		ui->txt_projekt->addItem("");

		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM projekti WHERE stranka LIKE '" + ui->txt_strankaid->text() + "'");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_projekt->addItem("(" + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ") "
									 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naziv")).toString()));
		}

	}
	base.close();

}

void racun::on_txt_projekt_currentIndexChanged(QString besedilo) {

	if (besedilo != "") {
		ui->txt_projektid->setText(besedilo.left(besedilo.indexOf(")", 1)));
		ui->txt_projektid->setText(ui->txt_projektid->text().right(ui->txt_projektid->text().length() - 1));
	}
	else {
		ui->txt_projektid->setText("");
	}

}

void racun::on_txt_projektid_textChanged(QString besedilo) {

	int i = 0;

	while ( i <= ui->txt_projekt->count() ) {
		if ( ui->txt_projekt->itemText(i).left(besedilo.length() + 2) == ("(" + besedilo + ")") ) {
			ui->txt_projekt->setCurrentIndex(i);
		}
		i++;
	}

}

// ne preverja obveznih polj
void racun::on_btn_sprejmi_clicked() {

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

			// poglej, ce je slucajno prislo do pretvorbe iz predracuna v racun in dodeli spremenljivki predracun ustrezno vrednost
			QSqlQuery sql_iz_predracuna_v_racun;
			bool predracun;
			sql_iz_predracuna_v_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + ui->txt_id->text() + "'");
			sql_iz_predracuna_v_racun.exec();
			if ( sql_iz_predracuna_v_racun.next() ) {
				if ( prevedi(sql_iz_predracuna_v_racun.value(sql_iz_predracuna_v_racun.record().indexOf("tipracuna")).toString()) == "predracun" ) {
					predracun = true;
				}
				else {
					predracun = false;
				}
			}

			QString projekt;

			// doloci SQL query glede na stanje programa: vnesi nov vnos, popravi obstojeci vnos, kopiraj obstojeci vnos in vse pripadajoce opravke
			QSqlQuery sql_vnesi_projekt;
			if (ui->btn_sprejmi->text() == "Vnesi racun") { // nov vnos se neobstojecega (pred)racuna
				sql_vnesi_projekt.prepare("INSERT INTO racuni (stracuna, stprojekta, tipracuna, stranka, datumpricetka, datumkonca, statusplacila, statusracunovodstva, zeplacano) "
										  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else if ( predracun == false || ui->rb_predracun->isChecked() ) { // popravi ze obstojec vnos
				sql_vnesi_projekt.prepare("UPDATE racuni SET stracuna = ?, stprojekta = ?, tipracuna = ?, stranka = ?, datumpricetka = ?, datumkonca = ?, statusplacila = ?, "
											"statusracunovodstva = ?, zeplacano = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			else if ( predracun == true && ui->rb_racun->isChecked() ) { // pretvori iz predracuna v racun
				/*
					* Vnos za racun ze obstaja. Preveri moznost, da se je iz predracuna naredil racun (obratno ne velja).
					* Ce je prislo do te spremembe, ne popravljaj ampak vnesi kot nov racun, starega pusti pri miru.
					* Prav tako kopiraj opravila kot del ze omenjenega racuna.
				*/

				// doloci novo stevilko racuna
				QString stara_stevilka = ui->txt_stracuna->text();
				QString leto = QDate::currentDate().toString("yyyy");
				int i = 1;
				QString stevilka = "";

				QSqlQuery sql_insert_stracuna;
				sql_insert_stracuna.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori("SR-" + leto) + "%'");
				sql_insert_stracuna.exec();
				while (sql_insert_stracuna.next()) {
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
				ui->txt_stracuna->setText("SR-" + leto + "-" + stevilka);

				// pripravi SQL query za vnos
				sql_vnesi_projekt.prepare("INSERT INTO racuni (stracuna, stprojekta, tipracuna, stranka, datumpricetka, datumkonca, statusplacila, statusracunovodstva, zeplacano) "
											"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

				// kopira opravila iz predracuna v racun
				QSqlQuery sql_poisci_opravila;
				sql_poisci_opravila.prepare("SELECT * FROM opravila WHERE racun LIKE '" + pretvori(stara_stevilka) + "'");
				sql_poisci_opravila.exec();
				while ( sql_poisci_opravila.next() ) {
					QSqlQuery sql_kopiraj_opravila;
					sql_kopiraj_opravila.prepare("INSERT INTO opravila (racun, storitev, ure, cena_ure, ddv, p_facebook, p_twitter, p_google, p_blog, "
													 "p_forum, p_sfacebook, p_stwitter, p_skuponi, p_obrazec, p_kupon, p_akcija, p_vip, popusti, k_vikend, "
													 "k_kratekrok, k_zahtevnost, k_neumnosti, k_komunikacija, kontrapopusti, znesekbrezddv, znesekddv, znesekpopust, znesekskupaj) "
													 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
					sql_kopiraj_opravila.bindValue(0, pretvori(ui->txt_stracuna->text()));
					sql_kopiraj_opravila.bindValue(1, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("storitev")).toString()));
					sql_kopiraj_opravila.bindValue(2, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ure")).toString()));
					sql_kopiraj_opravila.bindValue(3, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("cena_ure")).toString()));
					sql_kopiraj_opravila.bindValue(4, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ddv")).toString()));
					sql_kopiraj_opravila.bindValue(5, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_facebook")).toString()));
					sql_kopiraj_opravila.bindValue(6, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_twitter")).toString()));
					sql_kopiraj_opravila.bindValue(7, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_google")).toString()));
					sql_kopiraj_opravila.bindValue(8, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_blog")).toString()));
					sql_kopiraj_opravila.bindValue(9, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_forum")).toString()));
					sql_kopiraj_opravila.bindValue(10, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_sfacebook")).toString()));
					sql_kopiraj_opravila.bindValue(11, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_stwitter")).toString()));
					sql_kopiraj_opravila.bindValue(12, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_skuponi")).toString()));
					sql_kopiraj_opravila.bindValue(13, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_obrazec")).toString()));
					sql_kopiraj_opravila.bindValue(14, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_kupon")).toString()));
					sql_kopiraj_opravila.bindValue(15, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_akcija")).toString()));
					sql_kopiraj_opravila.bindValue(16, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("p_vip")).toString()));
					sql_kopiraj_opravila.bindValue(17, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popusti")).toString()));
					sql_kopiraj_opravila.bindValue(18, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("k_vikend")).toString()));
					sql_kopiraj_opravila.bindValue(19, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("k_kratekrok")).toString()));
					sql_kopiraj_opravila.bindValue(20, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("k_zahtevnost")).toString()));
					sql_kopiraj_opravila.bindValue(21, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("k_neumnosti")).toString()));
					sql_kopiraj_opravila.bindValue(22, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("k_komunikacija")).toString()));
					sql_kopiraj_opravila.bindValue(23, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("kontrapopusti")).toString()));
					sql_kopiraj_opravila.bindValue(24, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesekbrezddv")).toString()));
					sql_kopiraj_opravila.bindValue(25, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesekddv")).toString()));
					sql_kopiraj_opravila.bindValue(26, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesekpopust")).toString()));
					sql_kopiraj_opravila.bindValue(27, pretvori(sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesekskupaj")).toString()));
					sql_kopiraj_opravila.exec();
				}

			}

			// izvrsi SQL query
			sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stracuna->text()));
			sql_vnesi_projekt.bindValue(1, pretvori(ui->txt_projektid->text()));
			if ( ui->rb_predracun->isChecked() ) {
				sql_vnesi_projekt.bindValue(2, pretvori("predracun"));
			}
			else {
				sql_vnesi_projekt.bindValue(2, pretvori("racun"));
			}
			sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_strankaid->text()));
			sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_pricetek->text()));
			sql_vnesi_projekt.bindValue(5, pretvori(ui->txt_konec->text()));
			sql_vnesi_projekt.bindValue(6, pretvori(ui->txt_stplacila->currentText()));
			sql_vnesi_projekt.bindValue(7, pretvori(ui->txt_stracunovodstva->currentText()));
			sql_vnesi_projekt.bindValue(8, pretvori(ui->txt_z_placano->text()));

			sql_vnesi_projekt.exec();
		}

		base.close();

		// send signal to reload widget
		poslji("racuni");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void racun::napolni() {

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
		// the database is opened

		// clear previous content
		ui->tbl_opravila->clear();

		for (int i = 0; i <= 7; i++) {
			ui->tbl_opravila->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM opravila");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_opravila->removeRow(0);
		}

		// start filling the table
		ui->tbl_opravila->insertColumn(0);
		ui->tbl_opravila->insertColumn(1);
		ui->tbl_opravila->insertColumn(2);
		ui->tbl_opravila->insertColumn(3);
		ui->tbl_opravila->insertColumn(4);
		ui->tbl_opravila->insertColumn(5);
		ui->tbl_opravila->insertColumn(6);
		ui->tbl_opravila->insertColumn(7);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;
		QTableWidgetItem *naslov6 = new QTableWidgetItem;
		QTableWidgetItem *naslov7 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Stev. racuna");
		naslov2->setText("Storitev");
		naslov3->setText("Opravljene ure");
		naslov4->setText("Cena ure");
		naslov5->setText("Popusti");
		naslov6->setText("DDV");
		naslov7->setText("Koncni znesek");

		ui->tbl_opravila->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_opravila->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_opravila->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_opravila->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_opravila->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_opravila->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_opravila->setHorizontalHeaderItem(6, naslov6);
		ui->tbl_opravila->setHorizontalHeaderItem(7, naslov7);
		ui->tbl_opravila->setColumnWidth(0, 20);
		ui->tbl_opravila->setColumnWidth(1, 60);
		ui->tbl_opravila->setColumnWidth(2, 60);
		ui->tbl_opravila->setColumnWidth(3, 60);
		ui->tbl_opravila->setColumnWidth(4, 140);
		ui->tbl_opravila->setColumnWidth(5, 140);
		ui->tbl_opravila->setColumnWidth(6, 100);
		ui->tbl_opravila->setColumnWidth(7, 150);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM opravila WHERE racun LIKE '" + pretvori(ui->txt_stracuna->text()) + "'");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_opravila->insertRow(row);
			ui->tbl_opravila->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[8] = {"id", "racun", "storitev", "ure", "cena_ure", "popusti", "znesekddv", "znesekskupaj"};

			while (col <= 7) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				ui->tbl_opravila->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}
	}
	base.close();

}

void racun::prejem(QString besedilo) {

	if (besedilo.left(9) == "Nov racun") {
		ui->btn_sprejmi->setText("Vnesi racun");
		ui->btn_izpisi->setEnabled(false);
		ui->tbl_opravila->setEnabled(false);
		ui->btn_opravilo->setEnabled(false);
		ui->btn_brisi_opravilo->setEnabled(false);

		// from projekt id get stranka id
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
			QSqlQuery sql_stranka;
			sql_stranka.prepare("SELECT * FROM projekti WHERE stprojekta LIKE '" + pretvori(besedilo.right(besedilo.length() - 9)) + "'");
			sql_stranka.exec();
			if ( sql_stranka.next() ) {
				ui->txt_strankaid->setText(pretvori(sql_stranka.value(sql_stranka.record().indexOf("stranka")).toString()));
				ui->txt_projektid->setText(pretvori(sql_stranka.value(sql_stranka.record().indexOf("id")).toString()));
			}
		}
		base.close();
	}
	else {
		ui->btn_sprejmi->setText("Popravi vnos");
		ui->btn_izpisi->setEnabled(true);
		ui->tbl_opravila->setEnabled(true);
		ui->btn_opravilo->setEnabled(true);
		ui->btn_brisi_opravilo->setEnabled(true);
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
			sql_napolni.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(besedilo) + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_strankaid->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString()));
				ui->txt_stracuna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stracuna")).toString()));
				ui->txt_projektid->setText(pretvori(sql_napolni.value(sql_napolni.record().indexOf("stprojekta")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tipracuna")).toString()) == "predracun") {
					ui->rb_predracun->setChecked(true);
				}
				else {
					ui->rb_racun->setChecked(true);
				}

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datumpricetka")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_pricetek->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datumkonca")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_konec->setDate(datum);

				QSqlQuery sql_combo;
				bool ok;
				sql_combo.prepare("SELECT * FROM sif_placilo WHERE placilo LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("statusplacila")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_stplacila->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}

				sql_combo.clear();
				sql_combo.prepare("SELECT * FROM sif_racunovodstvo WHERE racunovodstvo LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("statusracunovodstva")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_stracunovodstva->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}

				ui->txt_z_placano->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("zeplacano")).toString()));
			}
		}
		base.close();

		napolni();

		izracunaj(); // calculate the values

	}
}

void racun::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}
}

QString racun::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString racun::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void racun::izracunaj() {

	double popusti = 0;
	double brezddv = 0;
	double ddv = 0;
	double placilo = 0;

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
		sql_racun.prepare("SELECT * FROM opravila WHERE racun LIKE '" + pretvori(ui->txt_stracuna->text()) + "'");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			popusti = popusti + prevedi(sql_racun.value(sql_racun.record().indexOf("znesekpopust")).toString()).toDouble();
			brezddv = brezddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesekbrezddv")).toString()).toDouble();
			ddv = ddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesekddv")).toString()).toDouble();
			placilo = placilo + prevedi(sql_racun.value(sql_racun.record().indexOf("znesekskupaj")).toString()).toDouble();
		}
	}
	base.close();

	ui->txt_z_popusti->setText(QString::number(popusti, 'f', 2));
	ui->txt_z_znesekbrezddv->setText(QString::number(brezddv, 'f', 2));
	ui->txt_z_ddv->setText(QString::number(ddv, 'f', 2));
	ui->txt_z_znesek->setText(QString::number(placilo, 'f', 2));
	ui->txt_z_seplacati->setText(QString::number(ui->txt_z_znesek->text().toDouble() - ui->txt_z_placano->text().toDouble(), 'f', 2));

}

void racun::on_btn_izracunaj_clicked() {

	izracunaj();

}

void racun::on_btn_brisi_opravilo_clicked() {

	QString id = ui->tbl_opravila->selectedItems().takeAt(0)->text();

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
		QSqlQuery sql_brisi;
		sql_brisi.prepare("DELETE FROM opravila WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_opravila->removeRow(ui->tbl_opravila->selectedItems().takeAt(0)->row());
	osvezi("opravila");

}

void racun::on_btn_opravilo_clicked() {

	opravila *uredi = new opravila;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Novo opravilo" + ui->txt_stracuna->text()); // ce opravila se ni, posljemo naprej st. racuna
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void racun::osvezi(QString beseda) {

	if ( beseda == "opravila" ) {
		napolni();
		izracunaj();
	}

}

void racun::on_tbl_opravila_doubleClicked() {

	opravila *uredi = new opravila;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_opravila->selectedItems().takeAt(0)->text()); // ce opravilo obstaja, posljemo naprej id opravila
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

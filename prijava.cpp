#include <QKeyEvent>
#include <QMessageBox>
#include <QtSql>
#include <QObject>
#include <QFile>
#include <QDir>

#include "prijava.h"
#include "ui_prijava.h"
#include "glavnookno.h"
#include "uporabnik.h"
#include "podjetje.h"
#include "kodiranje.h"
#include "varnost.h"

prijava::prijava(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::prijava)
{
	ui->setupUi(this);

	// ustvari varnostno kopijo
	varnostna_kopija();

	// ustvari tabele
	tabela_podjetje();
	tabela_uporabnik();
	tabela_potni_nalogi();
	tabela_potovanja();
	tabela_stroski();
	tabela_prejeti_racuni();
	tabela_stranke();
	tabela_projekti();
	tabela_racuni();

	// ustvari tabele sifrantov
	tabela_skd();
	tabela_posta();
	tabela_nazivi();
	tabela_pogodbe();
	tabela_dovoljenja();
	tabela_status_projekta();
	tabela_status_racuna();
	tabela_status_placila();
	tabela_status_racunovodstva();
	tabela_popusti();
	tabela_viri_strank();
	tabela_namen_potnega_naloga();
	tabela_prevoz();
	tabela_predracuni();

	// vnese podatke v tabele
	vnesi_skd();
	vnesi_posta();
	vnesi_nazive();
	vnesi_pogodbe();
	vnesi_dovoljenja();
	vnesi_status_projekta();
	vnesi_status_racuna();
	vnesi_status_placila();
	vnesi_status_racunovodstva();
	vnesi_popuste();
	vnesi_vire_strank();
	vnesi_namen_potnega_naloga();
	vnesi_prevoz();
	vnesi_predracune();

	ui->txt_uporabnik->setFocus();

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
		QSqlQuery sql_create_table_opravila;
		sql_create_table_opravila.prepare("CREATE TABLE IF NOT EXISTS opravila ("
										  "id INTEGER PRIMARY KEY, "
										  "racun TEXT,"
										  "storitev TEXT, "
										  "ure TEXT, "
										  "cena_ure TEXT, "
										  "ddv TEXT, "
										  "p_facebook TEXT, "
										  "p_twitter TEXT, "
										  "p_google TEXT, "
										  "p_blog TEXT, "
										  "p_forum TEXT, "
										  "p_sfacebook TEXT, "
										  "p_stwitter TEXT, "
										  "p_skuponi TEXT, "
										  "p_obrazec TEXT, "
										  "p_kupon TEXT, "
										  "p_akcija TEXT, "
										  "p_vip TEXT, "
										  "popusti TEXT, "
										  "k_vikend TEXT, "
										  "k_kratekrok TEXT, "
										  "k_zahtevnost TEXT, "
										  "k_neumnosti TEXT, "
										  "k_komunikacija TEXT, "
										  "kontrapopusti TEXT, "
										  "znesekbrezddv TEXT, "
										  "znesekddv TEXT, "
										  "znesekpopust TEXT, "
										  "znesekskupaj TEXT)"
										  );
		sql_create_table_opravila.exec();

		QSqlQuery sql_create_table_kuponi;
		sql_create_table_kuponi.prepare("CREATE TABLE IF NOT EXISTS kuponi ("
										"id INTEGER PRIMARY KEY, "
										"kupon TEXT, "
										"projekt TEXT, "
										"prejemnik TEXT, "
										"datumprejema TEXT, "
										"uporabljen TEXT, "
										"uporabitelj TEXT, "
										"datumuporabe TEXT)"
										);
		sql_create_table_kuponi.exec();

		// kreiranje sifrantov
		QSqlQuery sql_create_table_projekt;
		sql_create_table_projekt.prepare("CREATE TABLE IF NOT EXISTS sif_projekt ("
										"id INTEGER PRIMARY KEY, "
										"projekt TEXT)"
										);
		sql_create_table_projekt.exec();
		QSqlQuery sql_check_projekt;
		sql_check_projekt.prepare("SELECT * FROM sif_projekt");
		sql_check_projekt.exec();
		if (!sql_check_projekt.next()) {
			QSqlQuery sql_check_projekt;
			sql_check_projekt.prepare("INSERT INTO sif_projekt (projekt) VALUES ('" + pretvori("Dogovor") + "')");
			sql_check_projekt.exec();
			sql_check_projekt.prepare("INSERT INTO sif_projekt (projekt) VALUES ('" + pretvori("Izdan predracun") + "')");
			sql_check_projekt.exec();
			sql_check_projekt.prepare("INSERT INTO sif_projekt (projekt) VALUES ('" + pretvori("Potrjen predracun") + "')");
			sql_check_projekt.exec();
			sql_check_projekt.prepare("INSERT INTO sif_projekt (projekt) VALUES ('" + pretvori("V delu") + "')");
			sql_check_projekt.exec();
			sql_check_projekt.prepare("INSERT INTO sif_projekt (projekt) VALUES ('" + pretvori("Izdan racun") + "')");
			sql_check_projekt.exec();
			sql_check_projekt.prepare("INSERT INTO sif_projekt (projekt) VALUES ('" + pretvori("Placano") + "')");
			sql_check_projekt.exec();
			sql_check_projekt.prepare("INSERT INTO sif_projekt (projekt) VALUES ('" + pretvori("Zakljuceno") + "')");
			sql_check_projekt.exec();
		}

		QSqlQuery sql_create_table_placilo;
		sql_create_table_placilo.prepare("CREATE TABLE IF NOT EXISTS sif_placilo ("
										"id INTEGER PRIMARY KEY, "
										"placilo TEXT)"
										);
		sql_create_table_placilo.exec();
		QSqlQuery sql_check_placilo;
		sql_check_placilo.prepare("SELECT * FROM sif_placilo");
		sql_check_placilo.exec();
		if (!sql_check_placilo.next()) {
			QSqlQuery sql_insert_placilo;
			sql_insert_placilo.prepare("INSERT INTO sif_placilo (placilo) VALUES ('" + pretvori("Ni placano") + "')");
			sql_insert_placilo.exec();
			sql_insert_placilo.prepare("INSERT INTO sif_placilo (placilo) VALUES ('" + pretvori("Cakam na placilo") + "')");
			sql_insert_placilo.exec();
			sql_insert_placilo.prepare("INSERT INTO sif_placilo (placilo) VALUES ('" + pretvori("Placano") + "')");
			sql_insert_placilo.exec();
		}

		QSqlQuery sql_create_table_sif_opravila;
		sql_create_table_sif_opravila.prepare("CREATE TABLE IF NOT EXISTS sif_opravila ("
											"id INTEGER PRIMARY KEY, "
											"opravilo TEXT, "
											"cena TEXT, "
											"ddv TEXT)"
											);
		sql_create_table_sif_opravila.exec();
		QSqlQuery sql_check_sif_opravila;
		sql_check_sif_opravila.prepare("SELECT * FROM sif_opravila");
		sql_check_sif_opravila.exec();
		if (!sql_check_sif_opravila.next()) {
			QSqlQuery sql_insert_sif_opravila;
			sql_insert_sif_opravila.prepare("INSERT INTO sif_opravila (opravilo, cena, ddv) VALUES (?, ?, ?)");
			sql_insert_sif_opravila.bindValue(0, pretvori("Multivariantna analiza"));
			sql_insert_sif_opravila.bindValue(1, pretvori("10.00"));
			sql_insert_sif_opravila.bindValue(2, pretvori("20.0"));
			sql_insert_sif_opravila.exec();
			sql_insert_sif_opravila.prepare("INSERT INTO sif_opravila (opravilo, cena, ddv) VALUES (?, ?, ?)");
			sql_insert_sif_opravila.bindValue(0, pretvori("Faktorska analiza"));
			sql_insert_sif_opravila.bindValue(1, pretvori("8.00"));
			sql_insert_sif_opravila.bindValue(2, pretvori("0.0"));
			sql_insert_sif_opravila.exec();
			sql_insert_sif_opravila.prepare("INSERT INTO sif_opravila (opravilo, cena, ddv) VALUES (?, ?, ?)");
			sql_insert_sif_opravila.bindValue(0, pretvori("T test"));
			sql_insert_sif_opravila.bindValue(1, pretvori("5.00"));
			sql_insert_sif_opravila.bindValue(2, pretvori("8.5"));
			sql_insert_sif_opravila.exec();
			sql_insert_sif_opravila.prepare("INSERT INTO sif_opravila (opravilo, cena, ddv) VALUES (?, ?, ?)");
			sql_insert_sif_opravila.bindValue(0, pretvori("Povprecje"));
			sql_insert_sif_opravila.bindValue(1, pretvori("1.00"));
			sql_insert_sif_opravila.bindValue(2, pretvori("20.0"));
			sql_insert_sif_opravila.exec();

		}
		//go_with_the_flow oz nimam pojma, keri bi ze morali biti tule, hehe

		QSqlQuery sql_read_table_users;
		sql_read_table_users.prepare("SELECT * FROM uporabniki");
		sql_read_table_users.exec();
		if (!sql_read_table_users.next()) { // v bazi ni nobenega uporabnika programa, zazenemo vnosno okno za vnos uporabnika
			uporabnik *oknouporabnika = new uporabnik;
			oknouporabnika->show();
			QObject::connect(this, SIGNAL(prenos1(QString)),
					   oknouporabnika , SLOT(prejem(QString)));
			prijava::prenos1("Nov uporabnik");
		}

		QSqlQuery sql_read_table_firms;
		sql_read_table_firms.prepare("SELECT * FROM podjetje");
		sql_read_table_firms.exec();
		if (!sql_read_table_firms.next()) { // v bazi ni nobenega uporabnika programa, zazenemo vnosno okno za vnos uporabnika
			podjetje *oknopodjetja = new podjetje;
			oknopodjetja->show();
			QObject::connect(this, SIGNAL(prenos2(QString)),
					   oknopodjetja , SLOT(prejem(QString)));
			prijava::prenos2("Novo podjetje");
		}

	}
	base.close();
}

prijava::~prijava() {
    delete ui;
}

void prijava::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_prijavi_clicked();
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

void prijava::on_btn_izhod_clicked() {

	exit(0);

}

void prijava::on_btn_brisi_clicked() {

	ui->txt_geslo->setText("");
	ui->txt_uporabnik->setText("");

	ui->txt_uporabnik->setFocus();

}

void prijava::on_btn_prijavi_clicked() {

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

		QSqlQuery sql_preveri;
		sql_preveri.prepare("SELECT * FROM uporabniki WHERE user_name LIKE '" + pretvori(ui->txt_uporabnik->text().toLower()) + "'");
		sql_preveri.exec();
		if (!sql_preveri.next()) {
			ui->txt_uporabnik->setText("");
			ui->txt_geslo->setText("");

			QMessageBox msgbox;
			msgbox.setText("Uporabnisko ime ni pravilno!");
			msgbox.setInformativeText("V bazi ne obstaja uporabnik s tem uporabniskim imenom.");
			msgbox.exec();
		}
		else {
			if ( prevedi(sql_preveri.value(sql_preveri.record().indexOf("geslo")).toString()) == ui->txt_geslo->text() ) {
				// nastavi uporabnika, stanje, pravice uporabnika
				vApp->set_id(pretvori(ui->txt_uporabnik->text().toLower())); // nastavi uporabniske pravice
				if ( ui->txt_uporabnik->text().left(1) == ui->txt_uporabnik->text().left(1).toLower() ) { // ce je velika zacetnica smo na odprtem dostopu
					vApp->set_state(pretvori("public"));
				}
				else {
					vApp->set_state(pretvori("private"));
				}

				// prikazi glavno okno
				GlavnoOkno *glavnookno = new GlavnoOkno;
				glavnookno->showMaximized();
				glavnookno->show();
				this->close();
			}
			else {
				ui->txt_geslo->setText("");

				QMessageBox msgbox;
				msgbox.setText("Geslo ni pravilno!");
				msgbox.setInformativeText("Vnesli ste napacno geslo! Poskusite ponovno!");
				msgbox.exec();
			}
		}

	}
	base.close();
}

// ustvari varnostno kopijo
void prijava::varnostna_kopija() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile f_baza;
	f_baza.setFileName(dbase_path);

	QDir direktorij;
	direktorij.setPath(QDir::homePath() + "/.BubiRacun");
	if ( !direktorij.exists() ) {
		direktorij.mkdir(QDir::homePath() + "/.BubiRacun");
	}

	direktorij.setPath(QDir::homePath() + "/.BubiRacun/kopija");
	if ( !direktorij.exists() ) {
		direktorij.mkdir(QDir::homePath() + "/.BubiRacun/kopija");
	}

	int i = 1;
	while ( !f_baza.copy(QDir::homePath() + "/.BubiRacun/kopija/base-" + QDate::currentDate().toString("yyyy'-'MM'-'dd") + "-" + QString::number(i, 10) + ".bz.bck") ) {
		i++;
	}

}

// ustvari tabele
void prijava::tabela_podjetje() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS podjetje ("
														"id INTEGER PRIMARY KEY, "
														"ime TEXT, "
														"polnoime TEXT, "
														"skd TEXT, "
														"skd_besedilo TEXT, "
														"url TEXT, "
														"maticna_stevilka TEXT, "
														"ddv_zavezanec TEXT, "
														"davcna TEXT, "
														"tekoci_racun TEXT, "
														"naslov TEXT, "
														"naslov_st TEXT, "
														"posta TEXT, "
														"postna_stevilka TEXT, "
														"email TEXT, "
														"gsm TEXT, "
														"telefon TEXT, "
														"lastnik TEXT, "
														"kontaktna TEXT, "
														"odgovorna TEXT)"
							);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_uporabnik() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS uporabniki ("
														 "id INTEGER PRIMARY KEY, "
														 "user_name TEXT, "
														 "ime TEXT, "
														 "priimek TEXT, "
														 "naziv TEXT, "
														 "geslo TEXT, "
														 "naslov TEXT, "
														 "naslov_stevilka TEXT, "
														 "posta TEXT, "
														 "postna_stevilka TEXT, "
														 "telefon TEXT, "
														 "gsm TEXT, "
														 "email TEXT, "
														 "rojstni_datum TEXT, "
														 "spletna_stran TEXT, "
														 "davcna_stevilka TEXT, "
														 "emso TEXT, "
														 "tekoci_racun TEXT, "
														 "zaposlen TEXT, "
														 "datum_zaposlitve TEXT, "
														 "konec_zaposlitve TEXT, "
														 "pogodba TEXT, "
														 "avtomobil TEXT, "
														 "model_avtomobila TEXT, "
														 "registracija TEXT, "
														 "dovoljenje TEXT, "
														 "podjetje TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_potni_nalogi() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potni_nalogi ("
														 "id INTEGER PRIMARY KEY, "
														 "stevilka_naloga TEXT, "
														 "datum_naloga TEXT, "
														 "namen_naloga TEXT, "
														 "naziv_ciljnega_podjetja TEXT, "
														 "stevilka_projekta TEXT, "
														 "opombe TEXT, "
														 "cena_prevoza TEXT, "
														 "cena_dnevnic TEXT, "
														 "ostali_stroski TEXT, "
														 "stroski_skupaj TEXT, "
														 "skupaj_kilometri TEXT, "
														 "kilometrina TEXT, "
														 "skupaj_dnevi TEXT, "
														 "skupaj_ure TEXT, "
														 "priznana_dnevnica TEXT, "
														 "cena_dnevnice TEXT, "
														 "dnevnica_6_8 TEXT, "
														 "dnevnica_8_12 TEXT, "
														 "dnevnica_12_24 TEXT, "
														 "zajtrk_8_12 TEXT, "
														 "zajtrk_12_24 TEXT, "
														 "predlagatelj_podjetje TEXT, "
														 "predlagatelj_oseba TEXT, "
														 "prejemnik_oseba TEXT, "
														 "prevozno_sredstvo TEXT)"
														);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_potovanja() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potovanja ("
														 "id INTEGER PRIMARY KEY, "
														 "potni_nalog TEXT, "
														 "kraj_odhoda TEXT, "
														 "kraj_prihoda TEXT, "
														 "cas_odhoda TEXT, "
														 "cas_prihoda TEXT, "
														 "kilometri TEXT, "
														 "naslov TEXT)"
														);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_stroski() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS stroski ("
														 "id INTEGER PRIMARY KEY, "
														 "potninalog TEXT, "
														 "strosek TEXT, "
														 "cena TEXT)"
														);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_prejeti_racuni() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS prejeti_racuni ("
														 "id INTEGER PRIMARY KEY, "
														 "stevilka_vnosa TEXT, "
														 "stevilka_racuna TEXT, "
														 "izdajatelj_kratki TEXT, "
														 "izdajatelj_polni TEXT, "
														 "ulica TEXT, "
														 "hisna_stevilka TEXT, "
														 "postna_stevilka TEXT, "
														 "posta TEXT, "
														 "zadeva TEXT, "
														 "datum_prejema TEXT, "
														 "datum_placila TEXT, "
														 "rok_placila TEXT, "
														 "placnik_podjetje TEXT, "
														 "placnik_oseba TEXT, "
														 "stevilka_projekta TEXT, "
														 "avtor TEXT, "
														 "znesek_brez_ddv_00 TEXT, "
														 "znesek_brez_ddv_85 TEXT, "
														 "znesek_brez_ddv_20 TEXT, "
														 "znesek_ddv TEXT, "
														 "znesek_brez_ddv TEXT, "
														 "znesek TEXT, "
														 "status_placila TEXT, "
														 "status_racunovodstva TEXT)"
														);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_stranke() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS stranke ("
														 "id INTEGER PRIMARY KEY, "
														 "ime TEXT, "
														 "priimek TEXT, "
														 "naslov TEXT, "
														 "naslov_st TEXT, "
														 "posta TEXT, "
														 "postna_stevilka TEXT, "
														 "davcna TEXT, "
														 "kontakt TEXT, "
														 "telefon TEXT, "
														 "gsm TEXT, "
														 "email TEXT, "
														 "spletna_stran TEXT, "
														 "ustanova TEXT, "
														 "opomba TEXT, "
														 "tip TEXT, "
														 "stalnost TEXT, "
														 "aktivnost TEXT, "
														 "placilnost TEXT, "
														 "vir TEXT, "
														 "vir_id TEXT, "
														 "vir_kupon TEXT, "
														 "vir_ime TEXT, "
														 "vir_besedilo TEXT, "
														 "pop_facebook_1 TEXT, "
														 "pop_facebook_2 TEXT, "
														 "pop_kombinacija_1 TEXT, "
														 "pop_kombinacija_2 TEXT, "
														 "pop_stranka TEXT, "
														 "pop_kupon TEXT, "
														 "pop_akcija TEXT, "
														 "pop_vsi_facebook TEXT, "
														 "pop_vsi TEXT, "
														 "pod_vikend TEXT, "
														 "pod_hitrost TEXT, "
														 "pod_zapleti TEXT, "
														 "avtor_podjetje TEXT, "
														 "avtor_oseba TEXT)"
														);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_projekti() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS projekti ("
														 "id INTEGER PRIMARY KEY, "
														 "stevilka_projekta TEXT, "
														 "naslov_projekta TEXT, "
														 "stranka TEXT, "
														 "avtor_oseba TEXT, "
														 "pricetek_dela TEXT, "
														 "konec_dela TEXT, "
														 "status_projekta TEXT, "
														 "popust_fb1 TEXT, "
														 "popust_fb2 TEXT, "
														 "popust_komb1 TEXT, "
														 "popust_komb2 TEXT, "
														 "popust_stranka TEXT, "
														 "popust_kupon TEXT, "
														 "popust_akcija TEXT, "
														 "podrazitev_vikend TEXT, "
														 "podrazitev_hitrost TEXT, "
														 "podrazitev_zapleti TEXT)"
														);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_racuni() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS racuni ("
														 "id INTEGER PRIMARY KEY, "
														 "stevilka_racuna TEXT, "
														 "tip_racuna TEXT, "
														 "status_racuna TEXT, "
														 "stranka TEXT, "
														 "projekt TEXT, "
														 "avtor_oseba TEXT, "
														 "datum_pricetka TEXT, "
														 "datum_konca TEXT, "
														 "datum_izdaje TEXT, "
														 "datum_placila TEXT, "
														 "status_placila TEXT, "
														 "status_racunovodstva TEXT, "
														 "avans TEXT)"
										);
		sql_create_table.exec();
	}
	base.close();

}

// sifranti
void prijava::tabela_skd() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_skd ("
														 "id INTEGER PRIMARY KEY, "
														 "skd_stevilka TEXT, "
														 "skd_naziv TEXT)"
							);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_posta() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_posta ("
														 "id INTEGER PRIMARY KEY, "
														 "postna_stevilka TEXT, "
														 "posta TEXT)"
							);
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_nazivi() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_naziv ("
														 "id INTEGER PRIMARY KEY, "
														 "naziv TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_pogodbe() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_pogodbe ("
														 "id INTEGER PRIMARY KEY, "
														 "pogodba TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_dovoljenja() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_dovoljenja ("
														 "id INTEGER PRIMARY KEY, "
														 "dovoljenje TEXT, "
														 "opis TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_status_projekta() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_projekta ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_status_racuna() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_racuna ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_status_placila() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_placila ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_status_racunovodstva() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_racunovodstva ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_popusti() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_popusti ("
														 "id INTEGER PRIMARY KEY, "
														 "popust TEXT, "
														 "vrednost TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_viri_strank() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_viri_strank ("
														 "id INTEGER PRIMARY KEY, "
														 "vir TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_namen_potnega_naloga() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_namen_potnega_naloga ("
														 "id INTEGER PRIMARY KEY, "
														 "namen TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_prevoz() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_prevozna_sredstva ("
														 "id INTEGER PRIMARY KEY, "
														 "prevoz TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

void prijava::tabela_predracuni() {

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
		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_predracuna ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();
	}
	base.close();

}

// vnese podatke v tabele
void prijava::vnesi_skd() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/skd.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString skd_stevilka = vrstica.left(vrstica.indexOf(";", 0));
			QString skd_besedilo = vrstica.right(vrstica.length() - vrstica.indexOf(";", 0) - 1);

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_skd WHERE skd_stevilka LIKE '" + pretvori(skd_stevilka) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_skd (skd_stevilka, skd_naziv) VALUES (?, ?)");
				sql_insert_data.bindValue(0, pretvori(skd_stevilka));
				sql_insert_data.bindValue(1, pretvori(skd_besedilo));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_posta() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/posta.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString posta_stevilka = vrstica.left(vrstica.indexOf(",", 0));
			QString posta_besedilo = vrstica.right(vrstica.length() - vrstica.indexOf(",", 0) - 1);

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_posta WHERE postna_stevilka LIKE '" + pretvori(posta_stevilka) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_posta (postna_stevilka, posta) VALUES (?, ?)");
				sql_insert_data.bindValue(0, pretvori(posta_stevilka));
				sql_insert_data.bindValue(1, pretvori(posta_besedilo));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_dovoljenja() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/dovoljenja.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString ime = vrstica.left(vrstica.indexOf(",", 0));
			QString opis = vrstica.right(vrstica.length() - vrstica.indexOf(",", 0) - 1);

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_dovoljenja WHERE dovoljenje LIKE '" + pretvori(ime) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_dovoljenja (dovoljenje, opis) VALUES (?, ?)");
				sql_insert_data.bindValue(0, pretvori(ime));
				sql_insert_data.bindValue(1, pretvori(opis));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_nazive() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/nazivi.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString naziv = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_naziv WHERE naziv LIKE '" + pretvori(naziv) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_naziv (naziv) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(naziv));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_pogodbe() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/pogodbe.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString pogodba = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_pogodbe WHERE pogodba LIKE '" + pretvori(pogodba) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_pogodbe (pogodba) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(pogodba));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_status_projekta() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/status_projekta.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString naziv = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_status_projekta WHERE status LIKE '" + pretvori(naziv) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_status_projekta (status) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(naziv));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_status_racuna() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/status_racuna.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString status = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_status_racuna WHERE status LIKE '" + pretvori(status) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_status_racuna (status) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(status));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_status_placila() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/status_placila.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString status = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_status_placila WHERE status LIKE '" + pretvori(status) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_status_placila (status) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(status));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_status_racunovodstva() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/status_racunovodstva.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString status = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_status_racunovodstva WHERE status LIKE '" + pretvori(status) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_status_racunovodstva (status) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(status));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_popuste() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/popusti.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString popust = vrstica.left(vrstica.indexOf(",", 0));
			QString vrednost = vrstica.right(vrstica.length() - vrstica.indexOf(",", 0) - 1);

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori(popust) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_popusti (popust, vrednost) VALUES (?, ?)");
				sql_insert_data.bindValue(0, pretvori(popust));
				sql_insert_data.bindValue(1, pretvori(vrednost));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_vire_strank() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/vir-strank.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString vir = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_viri_strank WHERE vir LIKE '" + pretvori(vir) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_viri_strank (vir) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(vir));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_namen_potnega_naloga() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/namen_potnega_naloga.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString namen = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE namen LIKE '" + pretvori(namen) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_namen_potnega_naloga (namen) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(namen));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_prevoz() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/prevozna_sredstva.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString vrstica = besedilo.readLine();
			QString prevoz = vrstica.left(vrstica.indexOf(",", 0));

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_prevozna_sredstva WHERE prevoz LIKE '" + pretvori(prevoz) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_prevozna_sredstva (prevoz) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(prevoz));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

void prijava::vnesi_predracune() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QFile datoteka(app_path + "/status_predracuna.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

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

		/*
		*	prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnosze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			QString predracun = besedilo.readLine();

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_status_predracuna WHERE status LIKE '" + pretvori(predracun) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_status_predracuna (status) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(predracun));
				sql_insert_data.exec();
			}
		}
	}
	base.close();
	datoteka.remove();

}

// pretvori v in iz kodirane oblike
QString prijava::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString prijava::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

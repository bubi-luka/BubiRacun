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

	ui->txt_uporabnik->setFocus();

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
	while ( !f_baza.copy(QDir::homePath() + "/.BubiRacun/kopija/base-" + QDate::currentDate().toString("yyyy'-'MM'-'dd") + "-" + QString::number(i, 10) + ".bz.bck")) {
		i++;
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
		// the database is opened

		// kreiranje podatkov o uporabniku programa
		QSqlQuery sql_create_table_users;
		sql_create_table_users.prepare("CREATE TABLE IF NOT EXISTS uporabniki ("
									   "id INTEGER PRIMARY KEY, "
									   "user_name TEXT, "
									   "ime TEXT, "
									   "priimek TEXT, "
									   "geslo TEXT, "
									   "naslov TEXT, "
									   "telefon TEXT, "
									   "gsm TEXT, "
									   "email TEXT, "
									   "rojstnidatum TEXT, "
									   "spletnastran TEXT, "
									   "naziv TEXT, "
									   "davcnastevilka TEXT, "
									   "emso TEXT, "
									   "trr TEXT, "
									   "zaposlen TEXT, "
									   "datumzaposlitve TEXT, "
									   "koneczaposlitve TEXT, "
									   "pogodba TEXT, "
									   "avtomobil TEXT, "
										 "registracija TEXT, "
										 "dovoljenje TEXT)"
									   );
		sql_create_table_users.exec();

		// kreiranje podatkov o podjetju
		QSqlQuery sql_create_table_firm;
		sql_create_table_firm.prepare("CREATE TABLE IF NOT EXISTS podjetje ("
							  "id INTEGER PRIMARY KEY, "
							  "ime TEXT, "
							  "polnoime TEXT, "
							  "naslov TEXT, "
							  "davcnastevilka TEXT, "
							  "ddvzavezanec TEXT, "
							  "logo TEXT, "
							  "email TEXT, "
							  "spletnastran TEXT, "
							  "maticnastevilka TEXT, "
							  "trr TEXT, "
							  "skd TEXT, "
							  "lastnik TEXT, "
							  "odgovornaoseba TEXT, "
							  "nazivodgovorneosebe TEXT, "
							  "gsm TEXT, "
							  "telefon TEXT)"
							  );
		sql_create_table_firm.exec();

		// kreiranje podatkov za racunovodstvo
		QSqlQuery sql_create_table_stranke;
		sql_create_table_stranke.prepare("CREATE TABLE IF NOT EXISTS stranke ("
										 "id INTEGER PRIMARY KEY, "
										 "ime TEXT, "
										 "priimek TEXT, "
										 "naslov TEXT, "
										 "davcna TEXT, "
										 "kontakt TEXT, "
										 "telefon TEXT, "
										 "gsm TEXT, "
										 "email TEXT, "
										 "url TEXT, "
										 "opomba TEXT, "
										 "ustanova TEXT, "
										 "tip TEXT, "
										 "stalnost TEXT, "
										 "aktivnost TEXT, "
										 "placilnost TEXT, "
										 "vir TEXT, "
										 "vir_besedilo TEXT, "
										 "facebook TEXT, "
										 "pop_facebook TEXT, "
										 "twitter TEXT, "
										 "pop_twitter TEXT, "
										 "google TEXT, "
										 "pop_google TEXT, "
										 "blog TEXT, "
										 "pop_blog TEXT, "
										 "forum TEXT, "
										 "pop_forum TEXT, "
										 "s_facebook TEXT, "
										 "pop_s_facebook TEXT, "
										 "s_twitter TEXT, "
										 "pop_s_twitter TEXT, "
										 "s_kuponi TEXT, "
										 "pop_s_kuponi TEXT, "
										 "obrazec TEXT, "
										 "pop_obrazec TEXT, "
										 "kuponi TEXT, "
										 "pop_kuponi TEXT, "
										 "akcija TEXT, "
										 "pop_akcija TEXT, "
										 "vip TEXT, "
										 "pop_vip TEXT)"
								 );
		sql_create_table_stranke.exec();

		QSqlQuery sql_create_table_projekti;
		sql_create_table_projekti.prepare("CREATE TABLE IF NOT EXISTS projekti ("
										  "id INTEGER PRIMARY KEY, "
										  "stprojekta TEXT, "
										  "naziv TEXT, "
										  "stranka TEXT, "
										  "pricetek TEXT, "
										  "konec TEXT, "
										  "statusprojekta TEXT)"
										  );
		sql_create_table_projekti.exec();

		QSqlQuery sql_create_table_racuni;
		sql_create_table_racuni.prepare("CREATE TABLE IF NOT EXISTS racuni ("
										"id INTEGER PRIMARY KEY, "
										"stracuna TEXT, "
										"stprojekta, "
										"tipracuna TEXT, "
										"stranka TEXT, "
										"datumpricetka TEXT, "
										"datumkonca TEXT, "
										"statusplacila TEXT, "
										"statusracunovodstva TEXT, "
										"zeplacano TEXT)"
										);
		sql_create_table_racuni.exec();

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

		QSqlQuery sql_create_table_prejetiracuni;
		sql_create_table_prejetiracuni.prepare("CREATE TABLE IF NOT EXISTS prejetiracuni ("
											   "id INTEGER PRIMARY KEY, "
											   "stevilkavnosa TEXT, "
											   "stevilkaracuna TEXT, "
											   "posiljatelj TEXT, "
											   "zadeva TEXT, "
											   "datumprejema TEXT, "
											   "datumplacila TEXT, "
											   "rokplacila TEXT, "
											   "podjetje TEXT, "
											   "oseba TEXT, "
											   "znesek TEXT, "
											   "znesekbrezddv TEXT, "
											   "znesekddv TEXT, "
											   "ddv00 TEXT, "
											   "ddv85 TEXT, "
											   "ddv20 TEXT, "
											   "statusracuna TEXT, "
											   "statusplacila TEXT, "
											   "statusracunovodstvo TEXT)"
											   );
		sql_create_table_prejetiracuni.exec();

		QSqlQuery sql_create_table_potninalogi;
		sql_create_table_potninalogi.prepare("CREATE TABLE IF NOT EXISTS potninalogi ("
											 "id INTEGER PRIMARY KEY, "
											 "podjetje TEXT, "
											 "stevilkanaloga TEXT, "
											 "datum TEXT, "
											 "naslovnik TEXT, "
											 "naslovnikime TEXT, "
											 "naziv TEXT, "
											 "naslov TEXT, "
											 "naloga TEXT, "
											 "namen TEXT, "
											 "prevoznosredstvo TEXT, "
											 "tipavtomobila TEXT, "
											 "registrskastevilka TEXT, "
											 "razdalja TEXT, "
											 "kilometrina TEXT, "
											 "opombe TEXT, "
											 "predlagatelj TEXT, "
											 "nazivpredlagatelja TEXT, "
											 "naslovpodjetja TEXT, "
											 "zajtrk2 TEXT, "
											 "zajtrk3 TEXT)"
											 );
		sql_create_table_potninalogi.exec();

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

		QSqlQuery sql_create_table_stroski;
		sql_create_table_stroski.prepare("CREATE TABLE IF NOT EXISTS stroski ("
										"id INTEGER PRIMARY KEY, "
										"potninalog TEXT, "
										"strosek TEXT, "
										"cena TEXT)"
										);
		sql_create_table_stroski.exec();

		QSqlQuery sql_create_table_pot;
		sql_create_table_pot.prepare("CREATE TABLE IF NOT EXISTS pot ("
										"id INTEGER PRIMARY KEY, "
										"potninalog TEXT, "
										"odhod TEXT, "
										"prihod TEXT, "
										"casodhod TEXT, "
										"casprihod TEXT, "
										"kilometri TEXT, "
										"naslov TEXT)"
										);
		sql_create_table_pot.exec();

		// kreiranje sifrantov
		QSqlQuery sql_create_table_naziv;
		sql_create_table_naziv.prepare("CREATE TABLE IF NOT EXISTS sif_nazivi ("
									   "id INTEGER PRIMARY KEY, "
									   "naziv TEXT)"
									   );
		sql_create_table_naziv.exec();
		QSqlQuery sql_check_naziv;
		sql_check_naziv.prepare("SELECT * FROM sif_nazivi");
		sql_check_naziv.exec();
		if (!sql_check_naziv.next()) {
			QSqlQuery sql_insert_naziv;
			sql_insert_naziv.prepare("INSERT INTO sif_nazivi (naziv) VALUES ('" + pretvori("Direktor") + "')");
			sql_insert_naziv.exec();
			sql_insert_naziv.prepare("INSERT INTO sif_nazivi (naziv) VALUES ('" + pretvori("Raziskovalec") + "')");
			sql_insert_naziv.exec();
		}

		QSqlQuery sql_create_table_pogodbe;
		sql_create_table_pogodbe.prepare("CREATE TABLE IF NOT EXISTS sif_pogodbe ("
										 "id INTEGER PRIMARY KEY, "
										 "pogodba TEXT)"
										 );
		sql_create_table_pogodbe.exec();
		QSqlQuery sql_check_pogodba;
		sql_check_pogodba.prepare("SELECT * FROM sif_pogodbe");
		sql_check_pogodba.exec();
		if (!sql_check_pogodba.next()) {
			QSqlQuery sql_insert_pogodba;
			sql_insert_pogodba.prepare("INSERT INTO sif_pogodbe (pogodba) VALUES ('" + pretvori("Lastnik") + "')");
			sql_insert_pogodba.exec();
			sql_insert_pogodba.prepare("INSERT INTO sif_pogodbe (pogodba) VALUES ('" + pretvori("Pogodba za nedolocen cas") + "')");
			sql_insert_pogodba.exec();
			sql_insert_pogodba.prepare("INSERT INTO sif_pogodbe (pogodba) VALUES ('" + pretvori("Pogodba za dolocen cas") + "')");
			sql_insert_pogodba.exec();
			sql_insert_pogodba.prepare("INSERT INTO sif_pogodbe (pogodba) VALUES ('" + pretvori("Pogodbeno delo") + "')");
			sql_insert_pogodba.exec();
		}

		QSqlQuery sql_create_table_prevoz;
		sql_create_table_prevoz.prepare("CREATE TABLE IF NOT EXISTS sif_prevoz ("
										"id INTEGER PRIMARY KEY, "
										"prevoz TEXT)"
										);
		sql_create_table_prevoz.exec();
		QSqlQuery sql_check_prevoz;
		sql_check_prevoz.prepare("SELECT * FROM sif_prevoz");
		sql_check_prevoz.exec();
		if (!sql_check_prevoz.next()) {
			QSqlQuery sql_insert_prevoz;
			sql_insert_prevoz.prepare("INSERT INTO sif_prevoz (prevoz) VALUES ('" + pretvori("Avtomobil") + "')");
			sql_insert_prevoz.exec();
			sql_insert_prevoz.prepare("INSERT INTO sif_prevoz (prevoz) VALUES ('" + pretvori("Taksi") + "')");
			sql_insert_prevoz.exec();
			sql_insert_prevoz.prepare("INSERT INTO sif_prevoz (prevoz) VALUES ('" + pretvori("Vlak") + "')");
			sql_insert_prevoz.exec();
			sql_insert_prevoz.prepare("INSERT INTO sif_prevoz (prevoz) VALUES ('" + pretvori("Avtobus") + "')");
			sql_insert_prevoz.exec();
			sql_insert_prevoz.prepare("INSERT INTO sif_prevoz (prevoz) VALUES ('" + pretvori("Letalo") + "')");
			sql_insert_prevoz.exec();
		}

		QSqlQuery sql_create_table_namen;
		sql_create_table_namen.prepare("CREATE TABLE IF NOT EXISTS sif_namen ("
										"id INTEGER PRIMARY KEY, "
										"namen TEXT)"
										);
		sql_create_table_namen.exec();
		QSqlQuery sql_check_namen;
		sql_check_namen.prepare("SELECT * FROM sif_namen");
		sql_check_namen.exec();
		if (!sql_check_namen.next()) {
			QSqlQuery sql_insert_namen;
			sql_insert_namen.prepare("INSERT INTO sif_namen (namen) VALUES ('" + pretvori("Sestanek") + "')");
			sql_insert_namen.exec();
			sql_insert_namen.prepare("INSERT INTO sif_namen (namen) VALUES ('" + pretvori("Uvodni sestanek") + "')");
			sql_insert_namen.exec();
			sql_insert_namen.prepare("INSERT INTO sif_namen (namen) VALUES ('" + pretvori("Oddaja porocila") + "')");
			sql_insert_namen.exec();
			sql_insert_namen.prepare("INSERT INTO sif_namen (namen) VALUES ('" + pretvori("Predstavitev porocila") + "')");
			sql_insert_namen.exec();
			sql_insert_namen.prepare("INSERT INTO sif_namen (namen) VALUES ('" + pretvori("Izobrazevanje") + "')");
			sql_insert_namen.exec();
			sql_insert_namen.prepare("INSERT INTO sif_namen (namen) VALUES ('" + pretvori("PR") + "')");
			sql_insert_namen.exec();
		}

		QSqlQuery sql_create_table_racun;
		sql_create_table_racun.prepare("CREATE TABLE IF NOT EXISTS sif_racun ("
										"id INTEGER PRIMARY KEY, "
										"racun TEXT)"
										);
		sql_create_table_racun.exec();
		QSqlQuery sql_check_racun;
		sql_check_racun.prepare("SELECT * FROM sif_racun");
		sql_check_racun.exec();
		if (!sql_check_racun.next()) {
			QSqlQuery sql_insert_racun;
			sql_insert_racun.prepare("INSERT INTO sif_racun (racun) VALUES ('" + pretvori("Predracun") + "')");
			sql_insert_racun.exec();
			sql_insert_racun.prepare("INSERT INTO sif_racun (racun) VALUES ('" + pretvori("Racun") + "')");
			sql_insert_racun.exec();
			sql_insert_racun.prepare("INSERT INTO sif_racun (racun) VALUES ('" + pretvori("Dogovor") + "')");
			sql_insert_racun.exec();
		}

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

		QSqlQuery sql_create_table_pplacilo;
		sql_create_table_pplacilo.prepare("CREATE TABLE IF NOT EXISTS sif_pplacilo ("
										"id INTEGER PRIMARY KEY, "
										"placilo TEXT)"
										);
		sql_create_table_pplacilo.exec();
		QSqlQuery sql_check_pplacilo;
		sql_check_pplacilo.prepare("SELECT * FROM sif_pplacilo");
		sql_check_pplacilo.exec();
		if (!sql_check_pplacilo.next()) {
			QSqlQuery sql_insert_pplacilo;
			sql_insert_pplacilo.prepare("INSERT INTO sif_pplacilo (placilo) VALUES ('" + pretvori("Se placati") + "')");
			sql_insert_pplacilo.exec();
			sql_insert_pplacilo.prepare("INSERT INTO sif_pplacilo (placilo) VALUES ('" + pretvori("V cakalnici") + "')");
			sql_insert_pplacilo.exec();
			sql_insert_pplacilo.prepare("INSERT INTO sif_pplacilo (placilo) VALUES ('" + pretvori("Placano") + "')");
			sql_insert_pplacilo.exec();
		}

		QSqlQuery sql_create_table_racunovodstvo;
		sql_create_table_racunovodstvo.prepare("CREATE TABLE IF NOT EXISTS sif_racunovodstvo ("
										"id INTEGER PRIMARY KEY, "
										"racunovodstvo TEXT)"
										);
		sql_create_table_racunovodstvo.exec();
		QSqlQuery sql_check_racunovodstvo;
		sql_check_racunovodstvo.prepare("SELECT * FROM sif_racunovodstvo");
		sql_check_racunovodstvo.exec();
		if (!sql_check_racunovodstvo.next()) {
			QSqlQuery sql_insert_racunovodstvo;
			sql_insert_racunovodstvo.prepare("INSERT INTO sif_racunovodstvo (racunovodstvo) VALUES ('" + pretvori("Oddano") + "')");
			sql_insert_racunovodstvo.exec();
			sql_insert_racunovodstvo.prepare("INSERT INTO sif_racunovodstvo (racunovodstvo) VALUES ('" + pretvori("Se odnesti") + "')");
			sql_insert_racunovodstvo.exec();
		}

		QSqlQuery sql_create_table_virstrank;
		sql_create_table_virstrank.prepare("CREATE TABLE IF NOT EXISTS sif_virstrank ("
										"id INTEGER PRIMARY KEY, "
										"vir TEXT)"
										);
		sql_create_table_virstrank.exec();
		QSqlQuery sql_check_virstrank;
		sql_check_virstrank.prepare("SELECT * FROM sif_virstrank");
		sql_check_virstrank.exec();
		if (!sql_check_virstrank.next()) {
			QSqlQuery sql_insert_virstrank;
			sql_insert_virstrank.prepare("INSERT INTO sif_virstrank (vir) VALUES ('" + pretvori("Stranka") + "')");
			sql_insert_virstrank.exec();
			sql_insert_virstrank.prepare("INSERT INTO sif_virstrank (vir) VALUES ('" + pretvori("Oseba") + "')");
			sql_insert_virstrank.exec();
			sql_insert_virstrank.prepare("INSERT INTO sif_virstrank (vir) VALUES ('" + pretvori("Splet") + "')");
			sql_insert_virstrank.exec();
			sql_insert_virstrank.prepare("INSERT INTO sif_virstrank (vir) VALUES ('" + pretvori("Forum") + "')");
			sql_insert_virstrank.exec();
			sql_insert_virstrank.prepare("INSERT INTO sif_virstrank (vir) VALUES ('" + pretvori("Blog") + "')");
			sql_insert_virstrank.exec();
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

		QSqlQuery sql_create_table_popusti;
		sql_create_table_popusti.prepare("CREATE TABLE IF NOT EXISTS sif_popusti ("
										"id INTEGER PRIMARY KEY, "
										"popust TEXT, "
										 "procent TEXT, "
										 "tip TEXT)"
										);
		sql_create_table_popusti.exec();
		QSqlQuery sql_check_popusti;
		sql_check_popusti.prepare("SELECT * FROM sif_popusti");
		sql_check_popusti.exec();
		if (!sql_check_popusti.next()) {
			QSqlQuery sql_insert_popusti;
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Facebook osebni"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Twitter osebni"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Google plus osebni"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Blog osebni"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Forum osebni"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Facebook sirjenje"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Twitter sirjenje"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Kuponi - pridobil stranke"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Obrazec akcija"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Kupon akcija"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Akcijski popust akcija"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Stalna stranka akcija"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("popust"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Vikend"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("podrazitev"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Kratek rok"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("podrazitev"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Zahtevna stranka"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("podrazitev"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Neumnosti"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("podrazitev"));
			sql_insert_popusti.exec();
			sql_insert_popusti.prepare("INSERT INTO sif_popusti (popust, procent, tip) VALUES (?, ?, ?)");
			sql_insert_popusti.bindValue(0, pretvori("Komunikacija"));
			sql_insert_popusti.bindValue(1, pretvori("1.00"));
			sql_insert_popusti.bindValue(2, pretvori("podrazitev"));
			sql_insert_popusti.exec();

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
		// the database is opened

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
				// set user name, permission and programm state



				// show main window
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

QString prijava::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString prijava::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

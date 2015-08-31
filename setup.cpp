#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include <QtSql>
#include <QObject>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QWidget>
#include <QSettings>

#include "kodiranje.h"
#include "setup.h"
#include "baza.h"

setup::setup()
{
}
void setup::start_first_run() {

	// ustvari bazo podatkov
	baza_podatkov();

	// ustvari tabelo s podatki o programu in bazi
	glavna_tabela();

	QSqlQuery sql_razlicica;
	sql_razlicica.prepare("SELECT * FROM 'glavna'");
	sql_razlicica.exec();
	if ( !sql_razlicica.next() ) {
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
		tabela_opravila();
		tabela_opombe();
		tabela_nastavitve();
		tabela_avtomobili();
		tabela_stroski_prehrane();
		tabela_prioriteta_strank();

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
		tabela_namen_potnega_naloga();
		tabela_prevoz();
		tabela_predracuni();
		tabela_kategorije();
		tabela_podkategorije();
		tabela_storitev();
		tabela_oddaje_racuna();
		tabela_opombe_pri_racunih();
		tabela_dnevnice();
		tabela_kilometrina();
		tabela_cenamalice();
		tabela_banke();
		tabela_koda_namena();
		tabela_ddv();
		tabela_enote();

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
		vnesi_namen_potnega_naloga();
		vnesi_prevoz();
		vnesi_predracune();
		vnesi_oddaja_racuna();
		vnesi_nastavitve();
		vnesi_banke();
		vnesi_koda_namena();

		// dodaj v stalno obliko

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

	}

	// posodobitev baze
	posodobi_bazo();

}

// ustvari bazo podatkov
void setup::baza_podatkov() {

	// uvedi nov objekt
	baza nova_baza;

	// preglej, ali obstajajo kaksni podatki o legi baze podatkov, drugace jih ustvari
	QSettings nastavitve("BubiTech", "BubiRacun");

	if ( nastavitve.value("name").toString() != "" && nastavitve.value("path").toString() != "" && nastavitve.value("type").toString() != "" ) {
		nova_baza.set_name(nastavitve.value("name").toString());
		nova_baza.set_path(nastavitve.value("path").toString());
		nova_baza.set_type(nastavitve.value("type").toString());

		nova_baza.open_database();

	} // if
	else {

		nova_baza.ask_for_database();

	} // else

}

// ustvari tabelo s podatki o bazi in programu
void setup::glavna_tabela() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS glavna ("	// ustvarimo tabelo, ce se ne obstaja
														 "id INTEGER PRIMARY KEY, "							// kljuc
														 "parameter TEXT, "											// ime parametra, katerega lastnosti shranjujemo ("verzija")
														 "vrednost TEXT, "											// vrednost parametra ("1.0.0")
														 "opis TEXT, "													// opis parametra ("Zadnja razlicica programa")
														 "razlicica TEXT)"											// razlicica parametra ("5") - integer, stevilo sprememb parametra
														 );
		sql_create_table.exec();

}

// ustvari tabele
void setup::tabela_podjetje() {


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
														 "logotip TEXT, "
														 "lastnik TEXT, "
														 "kontaktna TEXT, "
														 "odgovorna TEXT, "
														 "bic TEXT, "
														 "banka TEXT, "
														 "koda_namena_avans TEXT, "
														 "koda_namena TEXT)"
							);
		sql_create_table.exec();

}

void setup::tabela_uporabnik() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS uporabniki ("
														 "id INTEGER PRIMARY KEY, "
														 "user_name TEXT, "
														 "ime TEXT, "
														 "priimek TEXT, "
														 "spol TEXT, "
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
														 "dovoljenje TEXT, "
														 "podjetje TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_potni_nalogi() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potni_nalogi ("
								 "id INTEGER PRIMARY KEY, "
								 "stevilka_naloga TEXT, "
								 "stevilka_dokumenta TEXT, "
								 "datum_naloga TEXT, "
								 "namen_naloga TEXT, "
								 "naziv_ciljnega_podjetja TEXT, "
								 "stevilka_projekta TEXT, "
								 "opombe TEXT, "
								 "priloge TEXT, "
								 "cena_prevoza TEXT, "
								 "cena_dnevnic TEXT, "
								 "ostali_stroski TEXT, "
								 "stroski_skupaj TEXT, "
								 "skupaj_kilometri TEXT, "
								 "kilometrina TEXT, "
								 "skupaj_dnevi TEXT, "
								 "skupaj_ure TEXT, "
								 "priznana_dnevnica TEXT, "
								 "cena_dnevnice_6_8 TEXT, "
								 "cena_dnevnice_8_12 TEXT, "
								 "cena_dnevnice_12_24 TEXT, "
								 "dnevnica_6_8 TEXT, "
								 "dnevnica_8_12 TEXT, "
								 "dnevnica_12_24 TEXT, "
								 "zajtrk_8_12 TEXT, "
								 "zajtrk_12_24 TEXT, "
								 "predlagatelj_podjetje_id TEXT, "
								 "predlagatelj_podjetje_dolgi TEXT, "
								 "predlagatelj_podjetje_kratki TEXT, "
								 "predlagatelj_podjetje_naslov_ulica TEXT, "
								 "predlagatelj_podjetje_naslov_stevilka TEXT, "
								 "predlagatelj_podjetje_naslov_posta TEXT, "
								 "predlagatelj_podjetje_naslov_postna_stevilka TEXT, "
								 "predlagatelj_podjetje_logotip TEXT, "
								 "predlagatelj_oseba_id TEXT, "
								 "predlagatelj_oseba_ime TEXT, "
								 "predlagatelj_oseba_priimek TEXT, "
								 "predlagatelj_oseba_naziv TEXT, "
								 "predlagateljk_oseba_naslov_ulica TEXT, "
								 "predlagatelj_oseba_naslov_stevilka TEXT, "
								 "predlagatelj_oseba_naslov_posta TEXT, "
								 "predlagatelj_oseba_naslov_postna_stevilka TEXT, "
								 "prejemnik_oseba_id TEXT, "
								 "prejemnik_oseba_ime TEXT, "
								 "prejemnik_oseba_priimek TEXT, "
								 "prejemnik_oseba_naziv TEXT, "
								 "prejemnik_oseba_naslov_ulica TEXT, "
								 "prejemnik_oseba_naslov_stevilka TEXT, "
								 "prejemnik_oseba_naslov_posta TEXT, "
								 "prejemnik_oseba_naslov_postna_stevilka TEXT, "
								 "prevozno_sredstvo TEXT, "
								 "proizvajalec TEXT, "
								 "znamka TEXT, "
								 "tip TEXT, "
								 "registrska_stevilka TEXT)"
														);
		sql_create_table.exec();

}

void setup::tabela_potovanja() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potovanja ("
														 "id INTEGER PRIMARY KEY, "
														 "potni_nalog TEXT, "
														 "kraj_odhoda TEXT, "
														 "kraj_prihoda TEXT, "
														 "cas_odhoda TEXT, "
														 "cas_prihoda TEXT, "
														 "kilometri TEXT, "
														 "naslov_ulica TEXT, "
														 "naslov_stevilka TEXT, "
														 "naslov_postna_stevilka TEXT, "
														 "naslov_posta TEXT, "
														 "naziv_ciljnega_podjetja TEXT, "
														 "namen_potovanja TEXT)"
														 );
		sql_create_table.exec();

}

void setup::tabela_stroski() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS stroski ("
														 "id INTEGER PRIMARY KEY, "
														 "potninalog TEXT, "
														 "strosek TEXT, "
														 "cena TEXT)"
														);
		sql_create_table.exec();

}

void setup::tabela_prejeti_racuni() {


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
														 "placnik_podjetje_id TEXT, "
														 "placnik_podjetje_kratki TEXT, "
														 "placnik_podjetje_polni TEXT, "
														 "placnik_oseba_id TEXT, "
														 "placnik_oseba_ime TEXT, "
														 "placnik_oseba_priimek TEXT, "
														 "placnik_oseba_naziv TEXT, "
														 "stevilka_projekta TEXT, "
														 "avtor TEXT, "
														 "ddv_array TEXT, "
														 "znesek_brez_ddv_array TEXT, "
														 "znesek_ddv TEXT, "
														 "znesek_brez_ddv TEXT, "
														 "znesek TEXT, "
														 "status_placila TEXT, "
														 "status_racunovodstva TEXT)"
														);
		sql_create_table.exec();

}

void setup::tabela_stranke() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS stranke ("
														 "id INTEGER PRIMARY KEY, "
														 "ime TEXT, "
														 "priimek TEXT, "
														 "naslov TEXT, "
														 "naslov_st TEXT, "
														 "posta TEXT, "
														 "postna_stevilka TEXT, "
														 "davcni_zavezanec TEXT, "
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
														 "pop_facebook_1 TEXT, "
														 "pop_facebook_2 TEXT, "
														 "pop_kombinacija_1 TEXT, "
														 "pop_kombinacija_2 TEXT, "
														 "pop_stranka TEXT, "
														 "pop_akcija TEXT, "
														 "pop_vsi_facebook TEXT, "
														 "pop_vsi TEXT, "
														 "pod_vikend TEXT, "
														 "pod_hitrost TEXT, "
														 "pod_zapleti TEXT, "
														 "avtor_podjetje TEXT, "
														 "avtor_oseba TEXT, "
														 "banka TEXT, "
														 "bic_banke TEXT, "
														 "trr TEXT)"
														);
		sql_create_table.exec();

}

void setup::tabela_projekti() {


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
														 "popust_akcija TEXT, "
														 "podrazitev_vikend TEXT, "
														 "podrazitev_hitrost TEXT, "
														 "podrazitev_zapleti TEXT)"
														);
		sql_create_table.exec();

}

void setup::tabela_racuni() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS racuni ("
								 "id INTEGER PRIMARY KEY, "
								 "stevilka_racuna TEXT, "
								 "stevilka_starsa TEXT, "
								 "stara_stevilka_racuna TEXT, "
								 "sklic TEXT, "
								 "tip_racuna TEXT, "
								 "status_racuna TEXT, "
								 "stornacija TEXT, "
								 "dobropis TEXT, "
								 "razlog_stornacije TEXT, "
								 "stranka TEXT, "
								 "projekt TEXT, "
								 "avtor_oseba TEXT, "
								 "datum_pricetka TEXT, "
								 "datum_konca TEXT, "
								 "datum_izdaje TEXT, "
								 "datum_placila TEXT, "
								 "rok_placila TEXT, "
								 "status_placila TEXT, "
								 "status_racunovodstva TEXT, "
								 "odstotek_avansa TEXT, "
								 "avans TEXT, "
								 "avans_ddv TEXT, "
								 "datum_placila_avansa TEXT, "
								 "status_oddaje_racuna TEXT, "
								 "datum_oddaje_racuna TEXT, "
								 "opombe TEXT, "
								 "podjetje_id TEXT, "
								 "podjetje_kratki TEXT, "
								 "podjetje_polni TEXT, "
								 "podjetje_naslov_ulica TEXT, "
								 "podjetje_naslov_stevilka TEXT, "
								 "podjetje_naslov_posta TEXT, "
								 "podjetje_naslov_postna_stevilka TEXT, "
								 "podjetje_url TEXT, "
								 "podjetje_email TEXT, "
								 "podjetje_telefon TEXT, "
								 "podjetje_ddv TEXT, "
								 "podjetje_bic TEXT, "
								 "podjetje_banka TEXT, "
								 "podjetje_tekoci_racun TEXT, "
								 "podjetje_koda_namena TEXT, "
								 "podjetje_logotip TEXT, "
								 "podjetje_maticna TEXT, "
								 "izdajatelj_id TEXT, "
								 "izdajatelj_ime TEXT, "
								 "izdajatelj_priimek TEXT, "
								 "izdajatelj_naziv TEXT, "
								 "narocnik_id TEXT, "
								 "narocnik_naziv TEXT, "
								 "narocnik_naslov TEXT, "
								 "narocnik_posta TEXT, "
								 "narocnik_davcna TEXT)"
								 );
		sql_create_table.exec();

}

void setup::tabela_opravila() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS opravila ("
														 "id INTEGER PRIMARY KEY, "
														 "stevilka_stranke TEXT, "
														 "stevilka_projekta TEXT, "
														 "stevilka_racuna TEXT, "
														 "vrstni_red TEXT"
														 "tip_racuna TEXT, "
														 "enota TEXT, "
														 "opravilo_sklop TEXT, "
														 "opravilo_skupina TEXT, "
														 "opravilo_storitev TEXT, "
														 "opravilo_rocno TEXT, "
														 "urna_postavka_brez_ddv TEXT, "
														 "urna_postavka_z_ddv TEXT, "
														 "ddv TEXT, "
														 "popust_fb1 TEXT, "
														 "popust_fb2 TEXT, "
														 "popust_komb1 TEXT, "
														 "popust_komb2 TEXT, "
														 "popust_stranka TEXT, "
														 "popust_akcija TEXT, "
														 "podrazitev_vikend TEXT, "
														 "podrazitev_hitrost TEXT, "
														 "podrazitev_zapleti TEXT, "
														 "pribitek_vikend TEXT, "
														 "pribitek_hitrost TEXT, "
														 "pribitek_zapleti TEXT, "
														 "tip_ur TEXT, "
														 "ur_dela TEXT, "
														 "rocni_vnos_ur TEXT, "
														 "znesek_popustov TEXT, "
														 "znesek_ddv TEXT, "
														 "znesek_koncni TEXT, "
								 "dobropis TEXT, "
								 "dobropis_st_ur TEXT, "
								 "dobropis_znesek TEXT, "
								 "dobropis_ddv TEXT, "
								 "dobropis_koncni TEXT, "
								 "casovnice TEXT, "
								 "sifra TEXT)"
								 );
		sql_create_table.exec();

}

void setup::tabela_opombe() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS opombe ("
														 "id INTEGER PRIMARY KEY, "
														 "stevilka_stranke TEXT, "
														 "stevilka_projekta TEXT, "
														 "stevilka_racuna TEXT, "
														 "tip_racuna TEXT, "
														 "datum TEXT, "
														 "naslov TEXT, "
														 "besedilo TEXT)"
										);
		sql_create_table.exec();

}

void setup::tabela_nastavitve() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS nastavitve ("
														 "id INTEGER PRIMARY KEY, "
														 "naziv TEXT, "
														 "vrednost TEXT)"
										);
		sql_create_table.exec();

}

void setup::tabela_avtomobili() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS avtomobili ("
								 "id INTEGER PRIMARY KEY, "
								 "proizvajalec TEXT, "
								 "znamka TEXT, "
								 "tip TEXT, "
								 "registrska_stevilka TEXT, "
								 "lastnistvo TEXT, "
								 "lastnik TEXT)"
										);
		sql_create_table.exec();

}

void setup::tabela_stroski_prehrane() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS stroski_prehrane ("
								 "id INTEGER PRIMARY KEY, "
								 "leto TEXT, "
								 "mesec TEXT, "
								 "delavniki TEXT, "
								 "prazniki TEXT, "
								 "skupaj TEXT, "
								 "ure_na_mesec TEXT, "
								 "cena_malice TEXT)"
								 );
		sql_create_table.exec();

}

void setup::tabela_prioriteta_strank() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS stranke_prioriteta ("
								 "id INTEGER PRIMARY KEY, "
								 "id_stranke TEXT, "
								 "prioriteta TEXT, "
								 "aktivnost TEXT)"
								 );
		sql_create_table.exec();

}

// sifranti
void setup::tabela_skd() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_skd ("
														 "id INTEGER PRIMARY KEY, "
														 "skd_stevilka TEXT, "
														 "skd_naziv TEXT)"
							);
		sql_create_table.exec();

}

void setup::tabela_posta() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_posta ("
														 "id INTEGER PRIMARY KEY, "
														 "postna_stevilka TEXT, "
														 "posta TEXT)"
							);
		sql_create_table.exec();

}

void setup::tabela_nazivi() {

		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_naziv ("
														 "id INTEGER PRIMARY KEY, "
														 "naziv_moski TEXT, "
														 "naziv_zenski TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_pogodbe() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_pogodbe ("
														 "id INTEGER PRIMARY KEY, "
														 "pogodba TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_dovoljenja() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_dovoljenja ("
														 "id INTEGER PRIMARY KEY, "
														 "dovoljenje TEXT, "
														 "opis TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_status_projekta() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_projekta ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_status_racuna() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_racuna ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_status_placila() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_placila ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_status_racunovodstva() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_racunovodstva ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_popusti() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_popusti ("
														 "id INTEGER PRIMARY KEY, "
														 "popust TEXT, "
														 "vrednost TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_namen_potnega_naloga() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_namen_potnega_naloga ("
														 "id INTEGER PRIMARY KEY, "
														 "namen TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_prevoz() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_prevozna_sredstva ("
														 "id INTEGER PRIMARY KEY, "
														 "prevoz TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_predracuni() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_predracuna ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_kategorije() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_kategorije ("
														 "id INTEGER PRIMARY KEY, "
														 "kategorija TEXT, "
														 "indeks TEXT, "
														 "aktivnost TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_podkategorije() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_podkategorije ("
														 "id INTEGER PRIMARY KEY, "
														 "kategorija TEXT, "
														 "podkategorija TEXT, "
														 "indeks TEXT, "
														 "aktivnost TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_storitev() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_storitve ("
								 "id INTEGER PRIMARY KEY, "
								 "sifra TEXT, "
								 "kategorija TEXT, "
								 "podkategorija TEXT, "
								 "storitev TEXT, "
								 "enota TEXT, "
								 "stopnja_ddv TEXT, "
								 "znesek_brez_ddv TEXT, "
								 "znesek_ddv TEXT, "
								 "znesek_z_ddv TEXT, "
								 "aktivnost TEXT)"
								 );
		sql_create_table.exec();

}

void setup::tabela_oddaje_racuna() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_status_oddaje_racuna ("
														 "id INTEGER PRIMARY KEY, "
														 "status TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_opombe_pri_racunih() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_opombe_pri_racunih ("
														 "id INTEGER PRIMARY KEY, "
														 "naslov TEXT, "
														 "besedilo TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_dnevnice() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_dnevnice ("
														 "id INTEGER PRIMARY KEY, "
														 "dnevnica_1 TEXT, "
														 "dnevnica_2 TEXT, "
														 "dnevnica_3 TEXT, "
														 "datum TEXT, "
														 "avtor_oseba TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_kilometrina() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_kilometrina ("
														 "id INTEGER PRIMARY KEY, "
														 "kilometrina TEXT, "
														 "datum TEXT, "
														 "avtor_oseba TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_cenamalice() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_cena_malice ("
														 "id INTEGER PRIMARY KEY, "
														 "cena TEXT, "
														 "datum TEXT, "
														 "avtor_oseba TEXT)"
										 );
		sql_create_table.exec();

}

void setup::tabela_banke() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_banke ("
								 "id INTEGER PRIMARY KEY, "
								 "ime_banke TEXT, "
								 "id_banke TEXT, "
								 "bic TEXT, "
								 "tip TEXT)"
								 );
		sql_create_table.exec();

}

void setup::tabela_koda_namena() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_koda_namena ("
														 "id INTEGER PRIMARY KEY, "
														 "koda TEXT, "
														 "opis_kratek TEXT, "
														 "opis_poln TEXT)"
										 );
		sql_create_table.exec();

}

// vnese podatke v tabele
void setup::vnesi_skd() {

	QString app_path = QApplication::applicationDirPath();
	QFile datoteka(app_path + "/skd.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_posta() {

	QString app_path = QApplication::applicationDirPath();
	QFile datoteka(app_path + "/posta.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_dovoljenja() {

	QString app_path = QApplication::applicationDirPath();

	QFile datoteka(app_path + "/dovoljenja.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_nazive() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/nazivi.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
			QString vrstica = besedilo.readLine();
			QString naziv_moski = vrstica.left(vrstica.indexOf(",", 0));
			QString naziv_zenski = vrstica.right(vrstica.length() - vrstica.indexOf(",", 0) - 2);

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_naziv WHERE naziv_moski LIKE '" + pretvori(naziv_moski) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_naziv (naziv_moski, naziv_zenski) VALUES (?, ?)");
				sql_insert_data.bindValue(0, pretvori(naziv_moski));
				sql_insert_data.bindValue(1, pretvori(naziv_zenski));
				sql_insert_data.exec();
			}
		}


}

void setup::vnesi_pogodbe() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/pogodbe.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_status_projekta() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/status_projekta.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_status_racuna() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/status_racuna.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_status_placila() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/status_placila.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_status_racunovodstva() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/status_racunovodstva.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_popuste() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/popusti.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_namen_potnega_naloga() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/namen_potnega_naloga.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_prevoz() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/prevozna_sredstva.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_predracune() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/status_predracuna.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
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

void setup::vnesi_oddaja_racuna() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/status_oddaje_racuna.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
			QString status = besedilo.readLine();

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_status_oddaje_racuna WHERE status LIKE '" + pretvori(status) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_status_oddaje_racuna (status) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(status));
				sql_insert_data.exec();
			}
		}


}

void setup::vnesi_nastavitve() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/sif_nastavitve.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
			QString naziv = besedilo.readLine();

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM nastavitve WHERE naziv LIKE '" + pretvori(naziv) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO nastavitve (naziv) VALUES (?)");
				sql_insert_data.bindValue(0, pretvori(naziv));
				sql_insert_data.exec();
			}
		}


}

void setup::vnesi_banke() {

	QString app_path = QApplication::applicationDirPath();

	QFile datoteka(app_path + "/sif_banke.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
			QString opravilo = besedilo.readLine();
			QString ime_banke = opravilo.left(opravilo.indexOf(";", 0));
			opravilo = opravilo.right(opravilo.length() - opravilo.indexOf(";", 0) - 1);
			QString id_banke = opravilo.left(opravilo.indexOf(";", 0));
			opravilo = opravilo.right(opravilo.length() - opravilo.indexOf(";", 0) - 1);
			QString bic = opravilo.left(opravilo.indexOf(";", 0));
			opravilo = opravilo.right(opravilo.length() - opravilo.indexOf(";", 0) - 1);
			QString tip = opravilo;

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_banke WHERE id_banke LIKE '" + pretvori(id_banke) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_banke (ime_banke, id_banke, bic, tip) "
																"VALUES (?, ?, ?, ?)");
				sql_insert_data.bindValue(0, pretvori(ime_banke));
				sql_insert_data.bindValue(1, pretvori(id_banke));
				sql_insert_data.bindValue(2, pretvori(bic));
				sql_insert_data.bindValue(3, pretvori(tip));
				sql_insert_data.exec();
			}
		}


}

void setup::vnesi_koda_namena() {

	QString app_path = QApplication::applicationDirPath();


	QFile datoteka(app_path + "/sif_koda_namena.csv");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}



		/*
		* prebere vsako vrstico besedila, iz nje izlusci z vejico locene vrednosti
		* prevedi, ali vnos ze obstaja v bazi, ce se ne obstaja obe vrednosti vnese v bazo
		*/
		QTextStream besedilo(&datoteka);
		while (!besedilo.atEnd()) {
			qApp->processEvents();
			QString opravilo = besedilo.readLine();
			QString koda = opravilo.left(opravilo.indexOf(";", 0));
			opravilo = opravilo.right(opravilo.length() - opravilo.indexOf(";", 0) - 1);
			QString opis_kratek = opravilo.left(opravilo.indexOf(";", 0));
			opravilo = opravilo.right(opravilo.length() - opravilo.indexOf(";", 0) - 1);
			QString opis_poln = opravilo;

			QSqlQuery sql_check_table;
			sql_check_table.prepare("SELECT * FROM sif_koda_namena WHERE koda LIKE '" + pretvori(koda) + "'");
			sql_check_table.exec();
			if ( !sql_check_table.next() ) {
				QSqlQuery sql_insert_data;
				sql_insert_data.prepare("INSERT INTO sif_koda_namena (koda, opis_kratek, opis_poln) "
																"VALUES (?, ?, ?)");
				sql_insert_data.bindValue(0, pretvori(koda));
				sql_insert_data.bindValue(1, pretvori(opis_kratek));
				sql_insert_data.bindValue(2, pretvori(opis_poln));
				sql_insert_data.exec();
			}
		}


}

void setup::vnesi_stroski_prehrane() {

}

void setup::tabela_ddv() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_ddv ("                     // ustvarimo tabelo, ce se ne obstaja
														 "id INTEGER PRIMARY KEY, "			// kljuc
														 "vrednost TEXT, "					// vrednost ddv-ja
														 "aktivnost TEXT)"					// je vrednost aktivna (1) ali neaktivna (0)
														 );
		sql_create_table.exec();

}

void setup::tabela_enote() {


		QSqlQuery sql_create_table;
		sql_create_table.prepare("CREATE TABLE IF NOT EXISTS sif_enote ("   // ustvarimo tabelo, ce se ne obstaja
								 "id INTEGER PRIMARY KEY, "                 // kljuc
								 "ime TEXT, "                               // ime enote
								 "enota TEXT, "                             // oznaka enote
								 "aktivnost TEXT)"                          // je vrednost aktivna (1) ali neaktivna (0)
								);
		sql_create_table.exec();

}

// pretvori v in iz kodirane oblike
QString setup::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString setup::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void setup::posodobi_bazo() {

	QString app_path = QApplication::applicationDirPath();

		/**
			* Preverimo stevilko razlicice programa, ki nam pove, ali je morda cas za
			* kak prepis baze.
			* Preverimo tudi stevilko razlicice baze, ki nam pove, ali je potrebno
			* bazo posodobiti, dodati kak update, ...
			* Max nam pove, da je prislo do tezkih sprememb v bazi. Pri teh spremembah
			* se obicajno naredi izvoz in ponovni uvoz baze.
			* Mid pove, da je prislo do prenosa polj med tabelami, zaradi cesar se
			* mora del podatkov prenesti ali izvoziti in ponovno uvoziti v novo tabelo.
			* Min pove, da so dolocena polja v tabeli ali dodana ali izbrisana,
			* prepis podatkov ni potreben
			**/

		QString stevilka_programa = "";
		int zaporedna_stevilka_stevilke_programa = 0;
		int zaporedna_stevilka_stevilke_baze = 0;
		int zaporedna_stevilka_datuma_spremembe = 0;
		int stevilka_baze_max = 0;
		int stevilka_baze_mid = 0;
		int stevilka_baze_min = 0;

		QSqlQuery podatki;
		podatki.prepare("SELECT * FROM glavna");
		podatki.exec();
		while ( podatki.next() ) {
			qApp->processEvents();
			if ( podatki.value(podatki.record().indexOf("parameter")).toString() == "Verzija programa" ) {
				stevilka_programa = podatki.value(podatki.record().indexOf("vrednost")).toString();
				zaporedna_stevilka_stevilke_programa = podatki.value(podatki.record().indexOf("razlicica")).toInt();
			}
			else if ( podatki.value(podatki.record().indexOf("parameter")).toString() == "Verzija baze" ) {
				QString stevilka_baze = podatki.value(podatki.record().indexOf("vrednost")).toString();
				zaporedna_stevilka_stevilke_baze = podatki.value(podatki.record().indexOf("razlicica")).toInt();
				stevilka_baze_max = stevilka_baze.left(stevilka_baze.indexOf(".", 0)).toInt();
				stevilka_baze = stevilka_baze.right(stevilka_baze.length() - stevilka_baze.indexOf(".", 0) - 1);
				stevilka_baze_mid = stevilka_baze.left(stevilka_baze.indexOf(".", 0)).toInt();
				stevilka_baze = stevilka_baze.right(stevilka_baze.length() - stevilka_baze.indexOf(".", 0) - 1);
				stevilka_baze_min = stevilka_baze.left(stevilka_baze.indexOf(".", 0)).toInt();
			}
			else if ( podatki.value(podatki.record().indexOf("parameter")).toString() == "Datum spremembe" ) {
				zaporedna_stevilka_datuma_spremembe = podatki.value(podatki.record().indexOf("razlicica")).toInt();
			}
		}
		podatki.clear();

		QSqlQuery update;

		if ( stevilka_baze_max == 0 ) {
			if ( stevilka_baze_mid == 0 ) {
				if ( stevilka_baze_min == 0  ) {
					update.prepare("ALTER TABLE opravila ADD COLUMN 'enota' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE opravila ADD COLUMN 'opravilo_sklop' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE opravila ADD COLUMN 'opravilo_rocno' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'priloge' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE racuni ADD COLUMN 'opombe' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'cena_dnevnice_6_8' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'cena_dnevnice_8_12' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'cena_dnevnice_12_24' TEXT");
					update.exec();
					update.clear();

					update.prepare("ALTER TABLE stranke ADD COLUMN 'davcni_zavezanec' TEXT");
					update.exec();
					update.clear();

					update.prepare("INSERT INTO glavna (parameter, vrednost, opis, razlicica) VALUES (?, ?, ?, ?)");
					update.bindValue(0, "Verzija programa");
					update.bindValue(1, "0.5.0");
					update.bindValue(2, "Stevilka trenutne razlicice programa");
					update.bindValue(3, "1");
					update.exec();
					update.clear();

					update.prepare("INSERT INTO glavna (parameter, vrednost, opis, razlicica) VALUES (?, ?, ?, ?)");
					update.bindValue(0, "Verzija baze");
					update.bindValue(1, "0.5.0");
					update.bindValue(2, "Stevilka trenutne razlicice baze, ki je v uporabi");
					update.bindValue(3, "1");
					update.exec();
					update.clear();

					posodobi_bazo();

				}
			}
			if ( stevilka_baze_mid == 5 ) {
				if ( stevilka_baze_min == 0 ) {
					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.0");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.0");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
			}
			if ( stevilka_baze_mid == 9 ) {
				if ( stevilka_baze_min == 0 ) {
					QSqlQuery sql_create_table;

					// tvorimo backup tabelo potovanj, ki pa ze ima nova polja
					sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potovanja_old ("
																		"id INTEGER PRIMARY KEY, "
																		"potni_nalog TEXT, "
																		"kraj_odhoda TEXT, "
																		"kraj_prihoda TEXT, "
																		"cas_odhoda TEXT, "
																		"cas_prihoda TEXT, "
																		"kilometri TEXT, "
																		"naslov_ulica TEXT, "
																		"naslov_stevilka TEXT, "
																		"naslov_postna_stevilka TEXT, "
																		"naslov_posta TEXT, "
																		"naziv_ciljnega_podjetja TEXT, "
																		"namen_potovanja TEXT)"
																		);
					sql_create_table.exec();
					sql_create_table.clear();

					// naredimo prepis podatkov o potovanjih iz stare tabele v backup tabelo
					// pri cemer ze upostevamo nova polja za naslov
					QSqlQuery sql_izbor;
					sql_izbor.prepare("SELECT * FROM potovanja ORDER BY id ASC");
					sql_izbor.exec();
					while ( sql_izbor.next() ) {
			qApp->processEvents();
						qApp->processEvents();
						QString naslov = prevedi(sql_izbor.value(sql_izbor.record().indexOf("naslov")).toString());
						QString naslov_ulica = naslov.left(naslov.indexOf("\n"));
						naslov = naslov.right(naslov.length() - 1 - naslov.indexOf("\n"));
						QString naslov_stevilka = "";
						QString naslov_postna_stevilka = naslov.left(4);
						naslov = naslov.right(naslov.length() - 5);
						QString naslov_posta = naslov;

						QSqlQuery sql_prenos;
						sql_prenos.prepare("INSERT INTO potovanja_old (potni_nalog, kraj_odhoda, kraj_prihoda, cas_odhoda, "
															 "cas_prihoda, kilometri, naslov_ulica, naslov_stevilka, naslov_postna_stevilka, naslov_posta, "
															 "naziv_ciljnega_podjetja, namen_potovanja) VALUES "
															 "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
						sql_prenos.bindValue(0, sql_izbor.value(sql_izbor.record().indexOf("potni_nalog")).toString());
						sql_prenos.bindValue(1, sql_izbor.value(sql_izbor.record().indexOf("kraj_odhoda")).toString());
						sql_prenos.bindValue(2, sql_izbor.value(sql_izbor.record().indexOf("kraj_prihoda")).toString());
						sql_prenos.bindValue(3, sql_izbor.value(sql_izbor.record().indexOf("cas_odhoda")).toString());
						sql_prenos.bindValue(4, sql_izbor.value(sql_izbor.record().indexOf("cas_prihoda")).toString());
						sql_prenos.bindValue(5, sql_izbor.value(sql_izbor.record().indexOf("kilometri")).toString());
						sql_prenos.bindValue(6, pretvori(naslov_ulica));
						sql_prenos.bindValue(7, pretvori(naslov_stevilka));
						sql_prenos.bindValue(8, pretvori(naslov_postna_stevilka));
						sql_prenos.bindValue(9, pretvori(naslov_posta));
						sql_prenos.bindValue(10, "");
						sql_prenos.bindValue(11, "");
						sql_prenos.exec();
						sql_prenos.clear();
					}
					sql_izbor.clear();

					// izbrisemo staro tabelo
					sql_create_table.prepare("DROP TABLE IF EXISTS potovanja");
					sql_create_table.exec();
					sql_create_table.clear();

					// v backup tabelo vnesemo podatke iz tabele potnih nalogov
					sql_izbor.prepare("SELECT * FROM potni_nalogi");
					sql_izbor.exec();
					while ( sql_izbor.next() ) {
			qApp->processEvents();
						qApp->processEvents();
						QSqlQuery sql_prenos;
						sql_prenos.prepare("UPDATE potovanja_old SET naziv_ciljnega_podjetja = ?, namen_potovanja = ? WHERE potni_nalog LIKE '" +
															 sql_izbor.value(sql_izbor.record().indexOf("stevilka_naloga")).toString() + "'");
						sql_prenos.bindValue(0, sql_izbor.value(sql_izbor.record().indexOf("naziv_ciljnega_podjetja")).toString());
						sql_prenos.bindValue(1, sql_izbor.value(sql_izbor.record().indexOf("namen_naloga")).toString());
						sql_prenos.exec();
						sql_prenos.clear();
					}
					sql_izbor.clear();

					// tvorimo novo tabelo
					sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potovanja ("
																		"id INTEGER PRIMARY KEY, "
																		"potni_nalog TEXT, "
																		"kraj_odhoda TEXT, "
																		"kraj_prihoda TEXT, "
																		"cas_odhoda TEXT, "
																		"cas_prihoda TEXT, "
																		"kilometri TEXT, "
																		"naslov_ulica TEXT, "
																		"naslov_stevilka TEXT, "
																		"naslov_postna_stevilka TEXT, "
																		"naslov_posta TEXT, "
																		"naziv_ciljnega_podjetja TEXT, "
																		"namen_potovanja TEXT)"
																		);
					sql_create_table.exec();
					sql_create_table.clear();

					// v novo tabelo prepisemo podatke iz backup tabele
					sql_izbor.prepare("SELECT * FROM potovanja_old ORDER BY id ASC");
					sql_izbor.exec();
					while ( sql_izbor.next() ) {
			qApp->processEvents();
						qApp->processEvents();
						QSqlQuery sql_prenos;
						sql_prenos.prepare("INSERT INTO potovanja (potni_nalog, kraj_odhoda, kraj_prihoda, cas_odhoda, "
															 "cas_prihoda, kilometri, naslov_ulica, naslov_stevilka, naslov_postna_stevilka, naslov_posta, "
															 "naziv_ciljnega_podjetja, namen_potovanja) VALUES "
															 "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
						sql_prenos.bindValue(0, sql_izbor.value(sql_izbor.record().indexOf("potni_nalog")).toString());
						sql_prenos.bindValue(1, sql_izbor.value(sql_izbor.record().indexOf("kraj_odhoda")).toString());
						sql_prenos.bindValue(2, sql_izbor.value(sql_izbor.record().indexOf("kraj_prihoda")).toString());
						sql_prenos.bindValue(3, sql_izbor.value(sql_izbor.record().indexOf("cas_odhoda")).toString());
						sql_prenos.bindValue(4, sql_izbor.value(sql_izbor.record().indexOf("cas_prihoda")).toString());
						sql_prenos.bindValue(5, sql_izbor.value(sql_izbor.record().indexOf("kilometri")).toString());
						sql_prenos.bindValue(6, sql_izbor.value(sql_izbor.record().indexOf("naslov_ulica")).toString());
						sql_prenos.bindValue(7, sql_izbor.value(sql_izbor.record().indexOf("naslov_stevilka")).toString());
						sql_prenos.bindValue(8, sql_izbor.value(sql_izbor.record().indexOf("naslov_postna_stevilka")).toString());
						sql_prenos.bindValue(9, sql_izbor.value(sql_izbor.record().indexOf("naslov_posta")).toString());
						sql_prenos.bindValue(10, sql_izbor.value(sql_izbor.record().indexOf("naziv_ciljnega_podjetja")).toString());
						sql_prenos.bindValue(11, sql_izbor.value(sql_izbor.record().indexOf("namen_potovanja")).toString());
						sql_prenos.exec();
						sql_prenos.clear();
					}
					sql_izbor.clear();

					// izbrisemo backup tabelo
					sql_create_table.prepare("DROP TABLE IF EXISTS potovanja_old");
					sql_create_table.exec();
					sql_create_table.clear();

					// naredimo backup tabelo za potne naloge
					sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potni_nalogi_old ("
																	 "id INTEGER PRIMARY KEY, "
																	 "stevilka_naloga TEXT, "
																	 "stevilka_dokumenta TEXT, "
																	 "datum_naloga TEXT, "
																	 "namen_naloga TEXT, "
																	 "naziv_ciljnega_podjetja TEXT, "
																	 "stevilka_projekta TEXT, "
																	 "opombe TEXT, "
																	 "priloge TEXT, "
																	 "cena_prevoza TEXT, "
																	 "cena_dnevnic TEXT, "
																	 "ostali_stroski TEXT, "
																	 "stroski_skupaj TEXT, "
																	 "skupaj_kilometri TEXT, "
																	 "kilometrina TEXT, "
																	 "skupaj_dnevi TEXT, "
																	 "skupaj_ure TEXT, "
																	 "priznana_dnevnica TEXT, "
																	 "cena_dnevnice_6_8 TEXT, "
																	 "cena_dnevnice_8_12 TEXT, "
																	 "cena_dnevnice_12_24 TEXT, "
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
					sql_create_table.clear();

					// prenesemo vse potne naloge iz stare tabele v backup tabelo
					sql_izbor.prepare("SELECT * FROM potni_nalogi ORDER BY id ASC");
					sql_izbor.exec();
					while ( sql_izbor.next() ) {
			qApp->processEvents();
						qApp->processEvents();
						QSqlQuery sql_prenos;
						sql_prenos.prepare("INSERT INTO potni_nalogi_old (stevilka_naloga, datum_naloga, namen_naloga, naziv_ciljnega_podjetja, "
															 "stevilka_projekta, opombe, cena_prevoza, cena_dnevnic, ostali_stroski, stroski_skupaj, skupaj_kilometri, "
															 "kilometrina, skupaj_dnevi, skupaj_ure, priznana_dnevnica, cena_dnevnice_6_8, cena_dnevnice_8_12, cena_dnevnice_12_24, dnevnica_6_8, "
															 "dnevnica_8_12, dnevnica_12_24, zajtrk_8_12, zajtrk_12_24, predlagatelj_podjetje, "
															 "predlagatelj_oseba, prejemnik_oseba, prevozno_sredstvo, priloge, stevilka_dokumenta) VALUES "
															 "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
						sql_prenos.bindValue(0, sql_izbor.value(sql_izbor.record().indexOf("stevilka_naloga")).toString());
						sql_prenos.bindValue(1, sql_izbor.value(sql_izbor.record().indexOf("datum_naloga")).toString());
						sql_prenos.bindValue(2, sql_izbor.value(sql_izbor.record().indexOf("namen_naloga")).toString());
						sql_prenos.bindValue(3, sql_izbor.value(sql_izbor.record().indexOf("naziv_ciljnega_podjetja")).toString());
						sql_prenos.bindValue(4, sql_izbor.value(sql_izbor.record().indexOf("stevilka_projekta")).toString());
						sql_prenos.bindValue(5, sql_izbor.value(sql_izbor.record().indexOf("opombe")).toString());
						sql_prenos.bindValue(6, sql_izbor.value(sql_izbor.record().indexOf("cena_prevoza")).toString());
						sql_prenos.bindValue(7, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnic")).toString());
						sql_prenos.bindValue(8, sql_izbor.value(sql_izbor.record().indexOf("ostali_stroski")).toString());
						sql_prenos.bindValue(9, sql_izbor.value(sql_izbor.record().indexOf("stroski_skupaj")).toString());
						sql_prenos.bindValue(10, sql_izbor.value(sql_izbor.record().indexOf("skupaj_kilometri")).toString());
						sql_prenos.bindValue(11, sql_izbor.value(sql_izbor.record().indexOf("kilometrina")).toString());
						sql_prenos.bindValue(12, sql_izbor.value(sql_izbor.record().indexOf("skupaj_dnevi")).toString());
						sql_prenos.bindValue(13, sql_izbor.value(sql_izbor.record().indexOf("skupaj_ure")).toString());
						sql_prenos.bindValue(14, sql_izbor.value(sql_izbor.record().indexOf("priznana_dnevnica")).toString());
						sql_prenos.bindValue(15, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnice_6_8")).toString());
						sql_prenos.bindValue(16, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnice_8_12")).toString());
						sql_prenos.bindValue(17, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnice_12_24")).toString());
						sql_prenos.bindValue(18, sql_izbor.value(sql_izbor.record().indexOf("dnevnica_6_8")).toString());
						sql_prenos.bindValue(19, sql_izbor.value(sql_izbor.record().indexOf("dnevnica_8_12")).toString());
						sql_prenos.bindValue(20, sql_izbor.value(sql_izbor.record().indexOf("dnevnica_12_24")).toString());
						sql_prenos.bindValue(21, sql_izbor.value(sql_izbor.record().indexOf("zajtrk_8_12")).toString());
						sql_prenos.bindValue(22, sql_izbor.value(sql_izbor.record().indexOf("zajtrk_12_24")).toString());
						sql_prenos.bindValue(23, sql_izbor.value(sql_izbor.record().indexOf("predlagatelj_podjetje")).toString());
						sql_prenos.bindValue(24, sql_izbor.value(sql_izbor.record().indexOf("predlagatelj_oseba")).toString());
						sql_prenos.bindValue(25, sql_izbor.value(sql_izbor.record().indexOf("prejemnik_oseba")).toString());
						sql_prenos.bindValue(26, sql_izbor.value(sql_izbor.record().indexOf("prevozno_sredstvo")).toString());
						sql_prenos.bindValue(27, sql_izbor.value(sql_izbor.record().indexOf("priloge")).toString());
						sql_prenos.bindValue(28, sql_izbor.value(sql_izbor.record().indexOf("stevilka_dokumenta")).toString());
						sql_prenos.exec();
						sql_prenos.clear();
					}
					sql_izbor.clear();

					// izbrisemo staro tabelo potnih nalogov
					sql_create_table.prepare("DROP TABLE IF EXISTS potni_nalogi");
					sql_create_table.exec();
					sql_create_table.clear();

					// naredimo novo tabelo za potne naloge
					sql_create_table.prepare("CREATE TABLE IF NOT EXISTS potni_nalogi ("
																	 "id INTEGER PRIMARY KEY, "
																	 "stevilka_naloga TEXT, "
																	 "stevilka_dokumenta TEXT, "
																	 "datum_naloga TEXT, "
																	 "stevilka_projekta TEXT, "
																	 "opombe TEXT, "
																	 "priloge TEXT, "
																	 "cena_prevoza TEXT, "
																	 "cena_dnevnic TEXT, "
																	 "ostali_stroski TEXT, "
																	 "stroski_skupaj TEXT, "
																	 "skupaj_kilometri TEXT, "
																	 "kilometrina TEXT, "
																	 "skupaj_dnevi TEXT, "
																	 "skupaj_ure TEXT, "
																	 "priznana_dnevnica TEXT, "
																	 "cena_dnevnice_6_8 TEXT, "
																	 "cena_dnevnice_8_12 TEXT, "
																	 "cena_dnevnice_12_24 TEXT, "
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

					// prenesemo vse potne naloge iz backup tabele v novo tabelo
					sql_izbor.prepare("SELECT * FROM potni_nalogi_old ORDER BY id ASC");
					sql_izbor.exec();
					while ( sql_izbor.next() ) {
			qApp->processEvents();
						QSqlQuery sql_prenos;
						sql_prenos.prepare("INSERT INTO potni_nalogi (stevilka_naloga, datum_naloga, "
															 "stevilka_projekta, opombe, cena_prevoza, cena_dnevnic, ostali_stroski, stroski_skupaj, skupaj_kilometri, "
															 "kilometrina, skupaj_dnevi, skupaj_ure, priznana_dnevnica, cena_dnevnice_6_8, cena_dnevnice_8_12, cena_dnevnice_12_24, dnevnica_6_8, "
															 "dnevnica_8_12, dnevnica_12_24, zajtrk_8_12, zajtrk_12_24, predlagatelj_podjetje, "
															 "predlagatelj_oseba, prejemnik_oseba, prevozno_sredstvo, priloge, stevilka_dokumenta) VALUES "
															 "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
						sql_prenos.bindValue(0, sql_izbor.value(sql_izbor.record().indexOf("stevilka_naloga")).toString());
						sql_prenos.bindValue(1, sql_izbor.value(sql_izbor.record().indexOf("datum_naloga")).toString());
						sql_prenos.bindValue(2, sql_izbor.value(sql_izbor.record().indexOf("stevilka_projekta")).toString());
						sql_prenos.bindValue(3, sql_izbor.value(sql_izbor.record().indexOf("opombe")).toString());
						sql_prenos.bindValue(4, sql_izbor.value(sql_izbor.record().indexOf("cena_prevoza")).toString());
						sql_prenos.bindValue(5, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnic")).toString());
						sql_prenos.bindValue(6, sql_izbor.value(sql_izbor.record().indexOf("ostali_stroski")).toString());
						sql_prenos.bindValue(7, sql_izbor.value(sql_izbor.record().indexOf("stroski_skupaj")).toString());
						sql_prenos.bindValue(8, sql_izbor.value(sql_izbor.record().indexOf("skupaj_kilometri")).toString());
						sql_prenos.bindValue(9, sql_izbor.value(sql_izbor.record().indexOf("kilometrina")).toString());
						sql_prenos.bindValue(10, sql_izbor.value(sql_izbor.record().indexOf("skupaj_dnevi")).toString());
						sql_prenos.bindValue(11, sql_izbor.value(sql_izbor.record().indexOf("skupaj_ure")).toString());
						sql_prenos.bindValue(12, sql_izbor.value(sql_izbor.record().indexOf("priznana_dnevnica")).toString());
						sql_prenos.bindValue(13, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnice_6_8")).toString());
						sql_prenos.bindValue(14, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnice_8_12")).toString());
						sql_prenos.bindValue(15, sql_izbor.value(sql_izbor.record().indexOf("cena_dnevnice_12_24")).toString());
						sql_prenos.bindValue(16, sql_izbor.value(sql_izbor.record().indexOf("dnevnica_6_8")).toString());
						sql_prenos.bindValue(17, sql_izbor.value(sql_izbor.record().indexOf("dnevnica_8_12")).toString());
						sql_prenos.bindValue(18, sql_izbor.value(sql_izbor.record().indexOf("dnevnica_12_24")).toString());
						sql_prenos.bindValue(19, sql_izbor.value(sql_izbor.record().indexOf("zajtrk_8_12")).toString());
						sql_prenos.bindValue(20, sql_izbor.value(sql_izbor.record().indexOf("zajtrk_12_24")).toString());
						sql_prenos.bindValue(21, sql_izbor.value(sql_izbor.record().indexOf("predlagatelj_podjetje")).toString());
						sql_prenos.bindValue(22, sql_izbor.value(sql_izbor.record().indexOf("predlagatelj_oseba")).toString());
						sql_prenos.bindValue(23, sql_izbor.value(sql_izbor.record().indexOf("prejemnik_oseba")).toString());
						sql_prenos.bindValue(24, sql_izbor.value(sql_izbor.record().indexOf("prevozno_sredstvo")).toString());
						sql_prenos.bindValue(25, sql_izbor.value(sql_izbor.record().indexOf("priloge")).toString());
						sql_prenos.bindValue(26, sql_izbor.value(sql_izbor.record().indexOf("stevilka_dokumenta")).toString());
						sql_prenos.exec();
						sql_prenos.clear();
					}
					sql_izbor.clear();

					// izbrisemo backup tabelo potnih nalogov
					sql_create_table.prepare("DROP TABLE IF EXISTS potni_nalogi_old");
					sql_create_table.exec();
					sql_create_table.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.1");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.1");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 1 ) {
					update.prepare("ALTER TABLE opravila ADD COLUMN 'casovnice' TEXT");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.2");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.2");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();
				}
				if ( stevilka_baze_min == 2 ) {
					update.prepare("ALTER TABLE racuni ADD COLUMN 'rok_placila' TEXT");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.3");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.3");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();
				}
				if ( stevilka_baze_min == 3 ) {
					// popravi napacne vrednosti pri predracunih - status predracuna je zavrnjen, status placila je placano
					update.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '1' AND status_racuna LIKE 'Zvripnzj'");
					update.exec();
					while ( update.next() ) {
			qApp->processEvents();
						QSqlQuery sql_delete;
						sql_delete.prepare("UPDATE racuni SET status_placila = '' WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						sql_delete.exec();
						sql_delete.clear();
					}
					update.clear();

					update.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '1' AND status_racuna LIKE ''");
					update.exec();
					while ( update.next() ) {
			qApp->processEvents();
						QSqlQuery sql_delete;
						sql_delete.prepare("UPDATE racuni SET status_placila = '' WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						sql_delete.exec();
						sql_delete.clear();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.4");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.4");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();
				}
				if ( stevilka_baze_min == 4 ) {
					// popravi časovnice iz stare oblike 0.00 na novo obliko HH:mm
					update.prepare("SELECT * FROM opravila WHERE casovnice NOT LIKE ''");
					update.exec();
					while ( update.next() ) {

						qApp->processEvents();
						QString casovnice = prevedi(update.value(update.record().indexOf("casovnice")).toString());
						QString nove_casovnice = "";

						int st_casovnic = casovnice.count(";");

						// pojdi skozi vse casovnice danega opravila, jih razbij na datum in cas in sestavi skupaj
						for ( int i = 0; i < st_casovnic; i++ ) {

							// razbij casovnico na datum
							QString datum = casovnice.left(11); // "yyyy.MM.dd."
							QString cas = casovnice.left(casovnice.indexOf(";")); // "yyyy.MM.dd.,0.00"
							cas = cas.right(cas.length() - datum.length() - 1); // "0.00"

							// izrezi iz stare casovnice trenutne podatke, da ni potrebe dolocati pozicije
							casovnice = casovnice.right(casovnice.length() - casovnice.indexOf(";") - 1);

							// pretvori double v sekunde
							int double_v_sekunde = cas.toDouble() * 3600;
							QTime cas_v_minutah =QTime::fromString("00:00", "HH:mm");
							cas_v_minutah = cas_v_minutah.addSecs(double_v_sekunde);

							// cas v sekundah pretvori nazaj v cas kot oblika "H:mm
							cas = cas_v_minutah.toString("H:mm");

							nove_casovnice += datum + "," + cas + ";";

						}

						// posodobi casovnice
						QSqlQuery sql_posodobi_casovnice;
						sql_posodobi_casovnice.prepare("UPDATE opravila SET casovnice = '" + pretvori(nove_casovnice) + "' WHERE id LIKE '" +
													   update.value(update.record().indexOf("id")).toString() + "'");
						sql_posodobi_casovnice.exec();
						sql_posodobi_casovnice.clear();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.5");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.5");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();
				}
				if ( stevilka_baze_min == 5 ) {
					// nastavi pot shranjevanja podatkov glede na pot logotipa
					QString mapa_za_shranjevanje = "";

					update.prepare("SELECT * FROM podjetje");
					update.exec();
					if ( update.next() ) {
						QString logotip = prevedi(update.value(update.record().indexOf("logotip")).toString());
						mapa_za_shranjevanje = logotip.left(logotip.lastIndexOf("/") - 5);
					}
					update.clear();

					update.prepare("UPDATE nastavitve SET vrednost = ? WHERE naziv LIKE '" + pretvori("pot") + "'");
					update.bindValue(0, pretvori(mapa_za_shranjevanje));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.6");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.6");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();
				}
				if ( stevilka_baze_min == 6 ) {
					// prepisi podatke o avtomobilih iz uporabnika v svojo bazo

					update.prepare("SELECT * FROM uporabniki");
					update.exec();
					while ( update.next() ) {

						qApp->processEvents();
						// preveri, ali registracija ze obstaja v bazi avtomobilov
						QSqlQuery preveri;
						preveri.prepare("SELECT * FROM avtomobili WHERE registrska_stevilka LIKE '" + update.value(update.record().indexOf("registracija")).toString() + "'");
						preveri.exec();
						if ( !preveri.next() ) {
							// prepisi iz ene baze v drugo
							QSqlQuery vnesi;
							vnesi.prepare("INSERT INTO avtomobili (proizvajalec, znamka, tip, registrska_stevilka, lastnistvo, lastnik) "
										  "VALUES (?,?,?,?,?,?)");
							vnesi.bindValue(0, update.value(update.record().indexOf("avtomobil")).toString());
							vnesi.bindValue(1, update.value(update.record().indexOf("model_avtomobila")).toString());
							vnesi.bindValue(2, "");
							vnesi.bindValue(3, update.value(update.record().indexOf("registracija")).toString());
							vnesi.bindValue(4, "1");
							vnesi.bindValue(5, update.value(update.record().indexOf("id")).toString());
							vnesi.exec();
						}
					}
					update.clear();

					// bazi potnih nalogov dodaj nove stolpce
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'proizvajalec' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'znamka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'tip' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'registrska_stevilka' TEXT");
					update.exec();
					update.clear();

					// prepisi ze obstojec avtomobil kot prevozno sredstvo v vse potne naloge
					QSqlQuery sql_uporabniki;
					sql_uporabniki.prepare("SELECT * FROM uporabniki");
					sql_uporabniki.exec();
					while ( sql_uporabniki.next() ) {
						update.prepare("UPDATE potni_nalogi SET proizvajalec = ?, znamka = ?, tip = ?, registrska_stevilka = ? "
									   "WHERE prejemnik_oseba LIKE '" + sql_uporabniki.value(sql_uporabniki.record().indexOf("id")).toString() + "'"
									   " AND prevozno_sredstvo LIKE '1'");
						update.bindValue(0, sql_uporabniki.value(sql_uporabniki.record().indexOf("avtomobil")).toString());
						update.bindValue(1, sql_uporabniki.value(sql_uporabniki.record().indexOf("model_avtomobila")).toString());
						update.bindValue(2, "");
						update.bindValue(3, sql_uporabniki.value(sql_uporabniki.record().indexOf("registracija")).toString());
						update.exec();
					}

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.7");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.7");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

				   posodobi_bazo();
				}
				// update na 0.9.8 je postal brezpredmeten, zato ga crtamo
				if ( stevilka_baze_min == 7 ) {
/*
					// bazi uporabnikov dodaj nove stolpce
					update.prepare("ALTER TABLE uporabniki ADD COLUMN 'starsi' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE uporabniki ADD COLUMN 'aktivnost' TEXT");
					update.exec();
					update.clear();

					// vse uporabnike obravnavaj kot aktivne in jim daj id starsa kar svoj id
					update.prepare("SELECT * FROM uporabniki");
					update.exec();
					while ( update.next() ) {
						qApp->processEvents();
						QSqlQuery sql_uporabnik;
						sql_uporabnik.prepare("UPDATE uporabniki SET aktivnost = ?, starsi = ? WHERE id LIKE '" +
											  update.value(update.record().indexOf("id")).toString() + "'");
						sql_uporabnik.bindValue(0, "1");
						sql_uporabnik.bindValue(1, update.value(update.record().indexOf("id")).toString());
						sql_uporabnik.exec();
					}
					update.clear();
*/
					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.8");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.8");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

				   posodobi_bazo();
				}
				if ( stevilka_baze_min == 8 ) {

					// bazi potnih nalogov dodaj nove stolpce
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_dolgi' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_kratki' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_naslov_ulica' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_naslov_stevilka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_naslov_posta' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_naslov_postna_stevilka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_podjetje_logotip' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_ime' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_priimek' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_naziv' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_naslov_ulica' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_naslov_stevilka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_naslov_posta' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'predlagatelj_oseba_naslov_postna_stevilka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_ime' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_priimek' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_naziv' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_naslov_ulica' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_naslov_stevilka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_naslov_posta' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE potni_nalogi ADD COLUMN 'prejemnik_oseba_naslov_postna_stevilka' TEXT");
					update.exec();
					update.clear();

					// sprehodi se skozi potne naloge in popravi podatke o uporabnikih in podjetju
					update.prepare("SELECT * FROM potni_nalogi");
					update.exec();
					while ( update.next() ) {
			qApp->processEvents();
						QSqlQuery sql_podjetje;
						sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" +
											  update.value(update.record().indexOf("predlagatelj_podjetje")).toString() + "'");
						sql_podjetje.exec();
						if ( sql_podjetje.next() ) {
							QSqlQuery sql_popravi_podjetje;
							sql_popravi_podjetje.prepare("UPDATE potni_nalogi SET "
														 "predlagatelj_podjetje_id = ?, "
														 "predlagatelj_podjetje_dolgi = ?, "
														 "predlagatelj_podjetje_kratki = ?, "
														 "predlagatelj_podjetje_naslov_ulica = ?, "
														 "predlagatelj_podjetje_naslov_stevilka = ?, "
														 "predlagatelj_podjetje_naslov_posta = ?, "
														 "predlagatelj_podjetje_naslov_postna_stevilka = ?, "
														 "predlagatelj_podjetje_logotip = ? "
														 "WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
							sql_popravi_podjetje.bindValue(0, sql_podjetje.value(sql_podjetje.record().indexOf("id")).toString());
							sql_popravi_podjetje.bindValue(1, sql_podjetje.value(sql_podjetje.record().indexOf("polnoime")).toString());
							sql_popravi_podjetje.bindValue(2, sql_podjetje.value(sql_podjetje.record().indexOf("ime")).toString());
							sql_popravi_podjetje.bindValue(3, sql_podjetje.value(sql_podjetje.record().indexOf("naslov")).toString());
							sql_popravi_podjetje.bindValue(4, sql_podjetje.value(sql_podjetje.record().indexOf("naslov_st")).toString());
							sql_popravi_podjetje.bindValue(5, sql_podjetje.value(sql_podjetje.record().indexOf("posta")).toString());
							sql_popravi_podjetje.bindValue(6, sql_podjetje.value(sql_podjetje.record().indexOf("postna_stevilka")).toString());
							sql_popravi_podjetje.bindValue(7, sql_podjetje.value(sql_podjetje.record().indexOf("logotip")).toString());
							sql_popravi_podjetje.exec();
						}

						QSqlQuery sql_predlagatelj;
						sql_predlagatelj.prepare("SELECT * FROM uporabniki WHERE id LIKE '" +
												 update.value(update.record().indexOf("predlagatelj_oseba")).toString() + "'");
						sql_predlagatelj.exec();
						if ( sql_predlagatelj.next() ) {
							QSqlQuery sql_popravi_predlagatelja;
							sql_popravi_predlagatelja.prepare("UPDATE potni_nalogi SET "
														   "predlagatelj_oseba_id = ?, "
														   "predlagatelj_oseba_ime = ?, "
														   "predlagatelj_oseba_priimek = ?, "
														   "predlagatelj_oseba_naziv = ?, "
														   "predlagatelj_oseba_naslov_ulica = ?, "
														   "predlagatelj_oseba_naslov_stevilka = ?, "
														   "predlagatelj_oseba_naslov_posta = ?, "
														   "predlagatelj_oseba_naslov_postna_stevilka = ? "
														   "WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
							sql_popravi_predlagatelja.bindValue(0, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("id")).toString());
							sql_popravi_predlagatelja.bindValue(1, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("ime")).toString());
							sql_popravi_predlagatelja.bindValue(2, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("priimek")).toString());
							sql_popravi_predlagatelja.bindValue(3, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("naziv")).toString());
							sql_popravi_predlagatelja.bindValue(4, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("naslov")).toString());
							sql_popravi_predlagatelja.bindValue(5, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("naslov_stevilka")).toString());
							sql_popravi_predlagatelja.bindValue(6, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("posta")).toString());
							sql_popravi_predlagatelja.bindValue(7, sql_predlagatelj.value(sql_predlagatelj.record().indexOf("postna_stevilka")).toString());
							sql_popravi_predlagatelja.exec();
						}

						QSqlQuery sql_prejemnik;
						sql_prejemnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" +
											  update.value(update.record().indexOf("prejemnik_oseba")).toString() + "'");
						sql_prejemnik.exec();
						if ( sql_prejemnik.next() ) {
							QSqlQuery sql_popravi_prejemnika;
							sql_popravi_prejemnika.prepare("UPDATE potni_nalogi SET "
														   "prejemnik_oseba_id = ?, "
														   "prejemnik_oseba_ime = ?, "
														   "prejemnik_oseba_priimek = ?, "
														   "prejemnik_oseba_naziv = ?, "
														   "prejemnik_oseba_naslov_ulica = ?, "
														   "prejemnik_oseba_naslov_stevilka = ?, "
														   "prejemnik_oseba_naslov_posta = ?, "
														   "prejemnik_oseba_naslov_postna_stevilka = ? "
														   "WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
							sql_popravi_prejemnika.bindValue(0, sql_prejemnik.value(sql_prejemnik.record().indexOf("id")).toString());
							sql_popravi_prejemnika.bindValue(1, sql_prejemnik.value(sql_prejemnik.record().indexOf("ime")).toString());
							sql_popravi_prejemnika.bindValue(2, sql_prejemnik.value(sql_prejemnik.record().indexOf("priimek")).toString());
							sql_popravi_prejemnika.bindValue(3, sql_prejemnik.value(sql_prejemnik.record().indexOf("naziv")).toString());
							sql_popravi_prejemnika.bindValue(4, sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov")).toString());
							sql_popravi_prejemnika.bindValue(5, sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov_stevilka")).toString());
							sql_popravi_prejemnika.bindValue(6, sql_prejemnik.value(sql_prejemnik.record().indexOf("posta")).toString());
							sql_popravi_prejemnika.bindValue(7, sql_prejemnik.value(sql_prejemnik.record().indexOf("postna_stevilka")).toString());
							sql_popravi_prejemnika.exec();
						}

					}
					update.clear();

					// posodobi tabelo prejetih racunov z novimi stolpci
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'placnik_podjetje_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'placnik_podjetje_kratki' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'placnik_podjetje_polni' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'placnik_oseba_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'placnik_oseba_ime' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'placnik_oseba_priimek' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'placnik_oseba_naziv' TEXT");
					update.exec();
					update.clear();

					// zapolni nova polja v tabeli prejetih racunov
					update.prepare("SELECT * FROM prejeti_racuni");
					update.exec();
					while ( update.next() ) {
			qApp->processEvents();
						QSqlQuery sql_podjetje;
						sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + update.value(update.record().indexOf("placnik_podjetje")).toString() + "'");
						sql_podjetje.exec();
						if ( sql_podjetje.next() ) {
							QSqlQuery sql_update;
							sql_update.prepare("UPDATE prejeti_racuni SET placnik_podjetje_id = ?, placnik_podjetje_kratki = ?, placnik_podjetje_polni = ? "
											   "WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
							sql_update.bindValue(0, sql_podjetje.value(sql_podjetje.record().indexOf("id")).toString());
							sql_update.bindValue(1, sql_podjetje.value(sql_podjetje.record().indexOf("ime")).toString());
							sql_update.bindValue(2, sql_podjetje.value(sql_podjetje.record().indexOf("polnoime")).toString());
							sql_update.exec();
						}

						QSqlQuery sql_oseba;
						sql_oseba.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + update.value(update.record().indexOf("placnik_oseba")).toString() + "'");
						sql_oseba.exec();
						if ( sql_oseba.next() ) {
							QSqlQuery sql_update;
							sql_update.prepare("UPDATE prejeti_racuni SET placnik_oseba_id = ?, placnik_oseba_ime = ?, placnik_oseba_priimek = ?, "
											   "placnik_oseba_naziv = ? WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
							sql_update.bindValue(0, sql_oseba.value(sql_oseba.record().indexOf("id")).toString());
							sql_update.bindValue(1, sql_oseba.value(sql_oseba.record().indexOf("ime")).toString());
							sql_update.bindValue(2, sql_oseba.value(sql_oseba.record().indexOf("priimek")).toString());
							sql_update.bindValue(3, sql_oseba.value(sql_oseba.record().indexOf("naziv")).toString());
							sql_update.exec();
						}

					}

					// dodaj nove stolpce v tabelo podjetja
					update.prepare("ALTER TABLE podjetje ADD COLUMN 'bic' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE podjetje ADD COLUMN 'banka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE podjetje ADD COLUMN 'koda_namena_avans' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE podjetje ADD COLUMN 'koda_namena' TEXT");
					update.exec();
					update.clear();

					// posodobi nova polja v tabeli podjetja
					update.prepare("UPDATE podjetje SET bic = ?, banka = ?, koda_namena_avans = ?, koda_namena = ?");
					update.bindValue(0, pretvori("ABAN SI 2X"));
					update.bindValue(1, pretvori("ABANKA VIPA d.d."));
					update.bindValue(2, pretvori("ADVA"));
					update.bindValue(3, pretvori("SCVE"));
					update.exec();
					update.clear();

					// dodaj nove stolpce v tabelo racunov
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_kratki' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_polni' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_naslov_ulica' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_naslov_stevilka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_naslov_posta' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_naslov_postna_stevilka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_url' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_email' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_telefon' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_ddv' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_bic' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_banka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_tekoci_racun' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_koda_namena' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_logotip' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'izdajatelj_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'izdajatelj_ime' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'izdajatelj_priimek' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'izdajatelj_naziv' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'narocnik_id' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'narocnik_naziv' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'narocnik_naslov' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'narocnik_posta' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE racuni ADD COLUMN 'narocnik_davcna' TEXT");
					update.exec();
					update.clear();

					// izpolni novonastala polja
					update.prepare("SELECT * FROM racuni");
					update.exec();
					while ( update.next() ) {
			qApp->processEvents();
						QString podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov, podjetje_naslov_stevilka,
								podjetje_naslov_posta, podjetje_naslov_postna_stevilka, podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv,
								podjetje_bic, podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip;
						QString izdajatelj_id, izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv;
						QString narocnik_id, narocnik_naziv, narocnik_naslov, narocnik_posta, narocnik_davcna;

						QSqlQuery sql_uporabnik;
						sql_uporabnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + update.value(update.record().indexOf("avtor_oseba")).toString() + "'");
						sql_uporabnik.exec();
						if ( sql_uporabnik.next() ) {
							izdajatelj_id = sql_uporabnik.value(sql_uporabnik.record().indexOf("id")).toString();
							izdajatelj_ime = sql_uporabnik.value(sql_uporabnik.record().indexOf("ime")).toString();
							izdajatelj_priimek = sql_uporabnik.value(sql_uporabnik.record().indexOf("priimek")).toString();
							izdajatelj_naziv = sql_uporabnik.value(sql_uporabnik.record().indexOf("naziv")).toString();

							QSqlQuery sql_podjetje;
							sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_uporabnik.value(sql_uporabnik.record().indexOf("podjetje")).toString() + "'");
							sql_podjetje.exec();
							if ( sql_podjetje.next() ) {
								podjetje_id = sql_podjetje.value(sql_podjetje.record().indexOf("id")).toString();
								podjetje_kratki = sql_podjetje.value(sql_podjetje.record().indexOf("ime")).toString();
								podjetje_polni = sql_podjetje.value(sql_podjetje.record().indexOf("polnoime")).toString();
								podjetje_naslov = sql_podjetje.value(sql_podjetje.record().indexOf("naslov")).toString();
								podjetje_naslov_stevilka = sql_podjetje.value(sql_podjetje.record().indexOf("naslov_st")).toString();
								podjetje_naslov_posta = sql_podjetje.value(sql_podjetje.record().indexOf("posta")).toString();
								podjetje_naslov_postna_stevilka = sql_podjetje.value(sql_podjetje.record().indexOf("postna_stevilka")).toString();
								podjetje_url = sql_podjetje.value(sql_podjetje.record().indexOf("url")).toString();
								podjetje_email = sql_podjetje.value(sql_podjetje.record().indexOf("email")).toString();
								podjetje_telefon = sql_podjetje.value(sql_podjetje.record().indexOf("telefon")).toString();
								if ( podjetje_telefon == "+(0)/--" ) {
									podjetje_telefon = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("gsm")).toString());
								}
								podjetje_ddv = sql_podjetje.value(sql_podjetje.record().indexOf("davcna")).toString();
								podjetje_bic = sql_podjetje.value(sql_podjetje.record().indexOf("bic")).toString();
								podjetje_banka = sql_podjetje.value(sql_podjetje.record().indexOf("banka")).toString();
								podjetje_tekoci_racun = sql_podjetje.value(sql_podjetje.record().indexOf("tekoci_racun")).toString();
								if ( update.value(update.record().indexOf("tip_racuna")).toString() == "1" ) { // predracun
									podjetje_koda_namena = sql_podjetje.value(sql_podjetje.record().indexOf("koda_namena_avans")).toString();
								}
								else if ( update.value(update.record().indexOf("tip_racuna")).toString() == "3" ) { // racun
									podjetje_koda_namena = sql_podjetje.value(sql_podjetje.record().indexOf("koda_namena")).toString();
								}
								else { // predplacilni racun
									podjetje_koda_namena = "";
								}
								podjetje_logotip = sql_podjetje.value(sql_podjetje.record().indexOf("logotip")).toString();
							}
						}

						QSqlQuery sql_narocnik;
						sql_narocnik.prepare("SELECT * from stranke WHERE id LIKE '" + update.value(update.record().indexOf("avtor_oseba")).toString() + "'");
						sql_narocnik.exec();
						if ( sql_narocnik.next() ) {
							narocnik_id = sql_narocnik.value(sql_narocnik.record().indexOf("id")).toString();
							narocnik_davcna = "";
							if ( sql_narocnik.value(sql_narocnik.record().indexOf("tip")).toString() == "1" ) {
							narocnik_naziv = pretvori(prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("priimek")).toString()) + " " +
									prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("ime")).toString()));
							}
							else {
							   narocnik_naziv = sql_narocnik.value(sql_narocnik.record().indexOf("priimek")).toString();
							   if ( sql_narocnik.value(sql_narocnik.record().indexOf("davcni_zavezanec")).toString() == "1" ) {
								   narocnik_davcna = sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString();
							   }
							}
							narocnik_naslov = pretvori(prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("naslov")).toString()) + " " +
									prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("naslov_st")).toString()));
							narocnik_posta = pretvori(prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("postna_stevilka")).toString()) + " " +
									prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("posta")).toString()));
						}

						QSqlQuery sql_posodobi;
						sql_posodobi.prepare("UPDATE racuni SET podjetje_id = ?, podjetje_kratki = ?, podjetje_polni = ?, podjetje_naslov_ulica = ?, "
											 "podjetje_naslov_stevilka = ?, podjetje_naslov_posta = ?, podjetje_naslov_postna_stevilka = ?, "
											 "podjetje_url = ?, podjetje_email = ?, podjetje_telefon = ?, podjetje_ddv = ?, podjetje_bic = ?, "
											 "podjetje_banka = ?, podjetje_tekoci_racun = ?, podjetje_koda_namena = ?, podjetje_logotip = ?, "
											 "izdajatelj_id = ?, izdajatelj_ime = ?, izdajatelj_priimek = ?, izdajatelj_naziv = ?, narocnik_id = ?, "
											 "narocnik_naziv = ?, narocnik_naslov = ?, narocnik_posta = ?, narocnik_davcna = ?"
											 "WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						sql_posodobi.bindValue(0, podjetje_id);
						sql_posodobi.bindValue(1, podjetje_kratki);
						sql_posodobi.bindValue(2, podjetje_polni);
						sql_posodobi.bindValue(3, podjetje_naslov);
						sql_posodobi.bindValue(4, podjetje_naslov_stevilka);
						sql_posodobi.bindValue(5, podjetje_naslov_posta);
						sql_posodobi.bindValue(6, podjetje_naslov_postna_stevilka);
						sql_posodobi.bindValue(7, podjetje_url);
						sql_posodobi.bindValue(8, podjetje_email);
						sql_posodobi.bindValue(9, podjetje_telefon);
						sql_posodobi.bindValue(10, podjetje_ddv);
						sql_posodobi.bindValue(11, podjetje_bic);
						sql_posodobi.bindValue(12, podjetje_banka);
						sql_posodobi.bindValue(13, podjetje_tekoci_racun);
						sql_posodobi.bindValue(14, podjetje_koda_namena);
						sql_posodobi.bindValue(15, podjetje_logotip);
						sql_posodobi.bindValue(16, izdajatelj_id);
						sql_posodobi.bindValue(17, izdajatelj_ime);
						sql_posodobi.bindValue(18, izdajatelj_priimek);
						sql_posodobi.bindValue(19, izdajatelj_naziv);
						sql_posodobi.bindValue(20, narocnik_id);
						sql_posodobi.bindValue(21, narocnik_naziv);
						sql_posodobi.bindValue(22, narocnik_naslov);
						sql_posodobi.bindValue(23, narocnik_posta);
						sql_posodobi.bindValue(24, narocnik_davcna);
						sql_posodobi.exec();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.9");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.9");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

				   posodobi_bazo();
				}
				if ( stevilka_baze_min == 9 ) {

					update.prepare("UPDATE sif_storitve SET storitev = ? WHERE storitev LIKE '" + pretvori("Multivariantna analiza") + "'");
					update.bindValue(0, pretvori("Multivariatna analiza"));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.10");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.10");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

				   posodobi_bazo();
				}
				if ( stevilka_baze_min == 10 ) {

					QSqlQuery vnesi;
					vnesi.prepare("INSERT INTO glavna (parameter, vrednost, opis, razlicica) "
								  "VALUES (?, ?, ?, ?)");
					vnesi.bindValue(0, "Ime programa");
					vnesi.bindValue(1, "BubiRacun");
					vnesi.bindValue(2, "Prikaze uradno in polno ime programa.");
					vnesi.bindValue(3, "1");
					vnesi.exec();
					vnesi.clear();

					vnesi.prepare("INSERT INTO glavna (parameter, vrednost, opis, razlicica) "
								  "VALUES (?, ?, ?, ?)");
					vnesi.bindValue(0, "Avtor programa");
					vnesi.bindValue(1, "Luka Oman");
					vnesi.bindValue(2, "Prikaze ime in priimek avtorja programa in nosilca avtorskih pravic.");
					vnesi.bindValue(3, "1");
					vnesi.exec();
					vnesi.clear();

					vnesi.prepare("INSERT INTO glavna (parameter, vrednost, opis, razlicica) "
								  "VALUES (?, ?, ?, ?)");
					vnesi.bindValue(0, "Datum spremembe");
					vnesi.bindValue(1, "09.10.2012");
					vnesi.bindValue(2, "Prikaze datum zadnje spremembe programa ali programske spremembe baze v formatu DD.MM.YYYY.");
					vnesi.bindValue(3, "0");
					vnesi.exec();
					vnesi.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.11");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.11");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "10.10.2012");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

				   posodobi_bazo();
				}
				if ( stevilka_baze_min == 11 ) {

					// za vsake obstojecega uporabnika tvorimo potrebne stolpce pri prehrani
					QSqlQuery sql_uporabniki;
					sql_uporabniki.prepare("SELECT * FROM uporabniki");
					sql_uporabniki.exec();
					while ( sql_uporabniki.next() ) {
						update.prepare("ALTER TABLE stroski_prehrane ADD COLUMN 'bolezen_" +
									   pretvori(sql_uporabniki.value(sql_uporabniki.record().indexOf("id")).toString()) +
									   "' TEXT");
						update.exec();
						update.clear();
						update.prepare("ALTER TABLE stroski_prehrane ADD COLUMN 'dopust_" +
									   pretvori(sql_uporabniki.value(sql_uporabniki.record().indexOf("id")).toString()) +
									   "' TEXT");
						update.exec();
						update.clear();
						update.prepare("ALTER TABLE stroski_prehrane ADD COLUMN 'izplacilo_dni_" +
									   pretvori(sql_uporabniki.value(sql_uporabniki.record().indexOf("id")).toString()) +
									   "' TEXT");
						update.exec();
						update.clear();
						update.prepare("ALTER TABLE stroski_prehrane ADD COLUMN 'izplacilo_znesek_" +
									   pretvori(sql_uporabniki.value(sql_uporabniki.record().indexOf("id")).toString()) +
									   "' TEXT");
						update.exec();
						update.clear();
					}

					update.prepare("INSERT INTO nastavitve (naziv, vrednost) VALUES (?, ?)");
					update.bindValue(0, pretvori("delavniki"));
					update.bindValue(1, pretvori("http://www.racunovodja.com/mdokumenti/delure2002.asp"));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.12");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.12");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "22.02.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

				   posodobi_bazo();
				}
				if ( stevilka_baze_min == 12 ) {
					// dodaj nov stolpec v tabelo opravila

					update.prepare("ALTER TABLE opravila ADD COLUMN 'vrstni_red' TEXT");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.13");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.13");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "15.06.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 13 ) {
					// dodaj vrednosti v sifrant ddv

					update.prepare("INSERT INTO sif_ddv (vrednost, aktivnost) VALUES (?, ?)");
					update.bindValue(0, pretvori("20.0"));
					update.bindValue(1, pretvori("1"));
					update.exec();
					update.clear();
					update.prepare("INSERT INTO sif_ddv (vrednost, aktivnost) VALUES (?, ?)");
					update.bindValue(0, pretvori("8.5"));
					update.bindValue(1, pretvori("1"));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.14");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.14");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "15.06.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 14 ) {

					// dodaj nova stolpca v tabelo prejeti_racuni
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'ddv_array' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE prejeti_racuni ADD COLUMN 'znesek_brez_ddv_array' TEXT");
					update.exec();
					update.clear();

					// preracunaj v nove vrednosti in napolni nova arraya
					update.prepare("SELECT * FROM prejeti_racuni");
					update.exec();
					while ( update.next() ) {
			qApp->processEvents();
						QString ddv_array = "";
						QString znesek_brez_ddv_array = "";
						QString znesek = "";

						if ( prevedi(update.value(update.record().indexOf("znesek_brez_ddv_00")).toString()) != "0.00" ) {
							ddv_array += "0.0,0.00;";
							znesek_brez_ddv_array += "0.0," + prevedi(update.value(update.record().indexOf("znesek_brez_ddv_00")).toString()) + ";";
						}
						if ( prevedi(update.value(update.record().indexOf("znesek_brez_ddv_85")).toString()) != "0.00" ) {
							znesek = QString::number(prevedi(update.value(update.record().indexOf("znesek_brez_ddv_85")).toString()).toDouble() * 8.5 / 100, 'f', 2);
							ddv_array += "8.5," + znesek + ";";
							znesek_brez_ddv_array += "8.5," + prevedi(update.value(update.record().indexOf("znesek_brez_ddv_85")).toString()) + ";";
						}
						if ( prevedi(update.value(update.record().indexOf("znesek_brez_ddv_20")).toString()) != "0.00" ) {
							znesek = QString::number(prevedi(update.value(update.record().indexOf("znesek_brez_ddv_20")).toString()).toDouble() * 20 / 100, 'f', 2);
							ddv_array += "20.0," + znesek + ";";
							znesek_brez_ddv_array += "20.0," + prevedi(update.value(update.record().indexOf("znesek_brez_ddv_20")).toString()) + ";";
						}

						QSqlQuery sql_vnesi_array;
						sql_vnesi_array.prepare("UPDATE prejeti_racuni SET ddv_array = ?, znesek_brez_ddv_array = ? "
												  "WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						sql_vnesi_array.bindValue(0, pretvori(ddv_array));
						sql_vnesi_array.bindValue(1, pretvori(znesek_brez_ddv_array));
						sql_vnesi_array.exec();
						sql_vnesi_array.clear();

					}
					update.clear();

					// brisanje stolpcev ni mozno, zato jih izpraznimo, da njihove vrednosti ne bodo delale problemov v prihodnje
					update.prepare("SELECT * FROM prejeti_racuni");
					update.exec();
					while ( update.next() ) {
						qApp->processEvents();
						QSqlQuery sql_delete_fields;
						sql_delete_fields.prepare("UPDATE prejeti_racuni SET znesek_brez_ddv_00 = ?, znesek_brez_ddv_85 = ?, znesek_brez_ddv_20 = ? "
												  "WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						sql_delete_fields.bindValue(0, "");
						sql_delete_fields.bindValue(1, "");
						sql_delete_fields.bindValue(2, "");
						sql_delete_fields.exec();
						sql_delete_fields.clear();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.15");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.15");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "16.06.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 15 ) {

					// popravi vnose za poštne številke
					update.prepare("SELECT * FROM sif_posta WHERE posta LIKE '% -0 %'");
					update.exec();
					while ( update.next() ) {
						qApp->processEvents();

						QString ime_poste = prevedi(update.value(update.record().indexOf("posta")).toString());
						ime_poste = ime_poste.replace("-0", "-");
						ime_poste = pretvori(ime_poste);

						QSqlQuery posodobi;
						posodobi.prepare("UPDATE sif_posta set posta = ? WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						posodobi.bindValue(0, ime_poste);
						posodobi.exec();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.16");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.16");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "20.09.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 16 ) {

					// popravi potne naloge, potovanja in stroške potnih nalogov
					update.prepare("SELECT * FROM potni_nalogi");
					update.exec();
					while ( update.next() ) {
						qApp->processEvents();
						QString stevilka_naloga = update.value(update.record().indexOf("stevilka_naloga")).toString();
						QString id_naloga = update.value(update.record().indexOf("id")).toString();

						QSqlQuery potovanje;
						potovanje.prepare("UPDATE potovanja SET potni_nalog = ? WHERE potni_nalog LIKE '" + stevilka_naloga + "'");
						potovanje.bindValue(0, id_naloga);
						potovanje.exec();
						potovanje.clear();

						potovanje.prepare("UPDATE stroski SET potninalog = ? WHERE potninalog LIKE '" + stevilka_naloga + "'");
						potovanje.bindValue(0, id_naloga);
						potovanje.exec();
						potovanje.clear();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.17");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.17");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "22.09.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 17 ) {

					// popravi vnose za poštne številke
					update.prepare("SELECT * FROM sif_posta WHERE posta LIKE '% - %'");
					update.exec();
					while ( update.next() ) {
						qApp->processEvents();

						QString ime_poste = update.value(update.record().indexOf("posta")).toString();
						ime_poste = ime_poste.replace("-", "- ");
						ime_poste = prevedi(ime_poste);
						ime_poste = ime_poste.replace("- ", "-");
						ime_poste = pretvori(ime_poste);

						QSqlQuery posodobi;
						posodobi.prepare("UPDATE sif_posta set posta = ? WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						posodobi.bindValue(0, ime_poste);
						posodobi.exec();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.18");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.18");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "23.09.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 18 ) {

					// dodaj polje k racunu za stornacijo
					update.prepare("ALTER TABLE racuni ADD COLUMN 'stornacija' TEXT");
					update.exec();
					update.clear();

					// vnesi vsem racunom vrednost stornacije == 0

					update.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'");
					update.exec();
					while ( update.next() ) {
						qApp->processEvents();
						QSqlQuery posodobi;
						posodobi.prepare("UPDATE racuni SET stornacija = ? WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						posodobi.bindValue(0, "0");
						posodobi.exec();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.19");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.19");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "21.10.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 19 ) {

					// izbrisi tabelo storitev
					update.prepare("DROP TABLE IF EXISTS sif_storitve");
					update.exec();
					update.clear();

					// naredi novo tabelo za storitve, saj smo staro zbrisali
					update.prepare("CREATE TABLE IF NOT EXISTS sif_storitve ("
								   "id INTEGER PRIMARY KEY, "
								   "sifra TEXT, "
								   "kategorija TEXT, "
								   "podkategorija TEXT, "
								   "storitev TEXT, "
								   "enota TEXT, "
								   "stopnja_ddv TEXT, "
								   "znesek_brez_ddv TEXT, "
								   "znesek_ddv TEXT, "
								   "znesek_z_ddv TEXT, "
								   "aktivnost TEXT)"
								   );
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.20");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.20");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "07.11.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 20 ) {

					// dodaj polje v tabelo opravil
					update.prepare("ALTER TABLE opravila ADD COLUMN 'sifra' TEXT");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.21");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.21");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "20.11.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 21 ) {

					// popravi vsa rocno vnesena opravila na normalno vnesena in njihovo sifro na 999999
					QSqlQuery sql_preberi;
					sql_preberi.prepare("SELECT * FROM opravila WHERE opravilo_rocno NOT LIKE ''");
					sql_preberi.exec();
					while ( sql_preberi.next() ) {
						update.prepare("UPDATE opravila SET opravilo_storitev = ?, sifra = ?, opravilo_rocno  = ? WHERE id LIKE '" +
									   pretvori(sql_preberi.value(sql_preberi.record().indexOf("id")).toString()) + "'");
						update.bindValue(0, sql_preberi.value(sql_preberi.record().indexOf("opravilo_rocno")).toString());
						update.bindValue(1, "999999");
						update.bindValue(2, "");
						update.exec();
						update.clear();
					}


					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.22");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.22");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "22.11.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 22 ) {

					// dodaj nov stolpec v tabelo racunov - razlog stornacije
					update.prepare("ALTER TABLE racuni ADD COLUMN 'razlog_stornacije' TEXT");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.23");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.23");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "23.11.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 23 ) {

					// odstrani vse stornacijske racune
					update.prepare("DELETE FROM racuni WHERE tip_racuna LIKE '" + pretvori("4") + "'");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.24");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.24");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "23.11.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 24 ) {

					// dodaj nove stolpce v tabelo opravil - dobropis
					update.prepare("ALTER TABLE opravila ADD COLUMN 'dobropis' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE opravila ADD COLUMN 'dobropis_st_ur' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE opravila ADD COLUMN 'dobropis_znesek' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE opravila ADD COLUMN 'dobropis_ddv' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE opravila ADD COLUMN 'dobropis_koncni' TEXT");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.25");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.25");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "25.12.2013");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 25 ) {

					// dodaj nove stolpce v tabelo racunov - dobropis
					update.prepare("ALTER TABLE racuni ADD COLUMN 'dobropis' TEXT");
					update.exec();
					update.clear();

					update.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'");
					update.exec();
					while ( update.next() ) {
						qApp->processEvents();
						QSqlQuery posodobi;
						posodobi.prepare("UPDATE racuni SET dobropis = ? WHERE id LIKE '" + update.value(update.record().indexOf("id")).toString() + "'");
						posodobi.bindValue(0, "0");
						posodobi.exec();
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.26");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.26");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "2.1.2014");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 26 ) {

					// dodaj nove stolpce v tabelo strank - podatki o banki
					update.prepare("ALTER TABLE stranke ADD COLUMN 'banka' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE stranke ADD COLUMN 'bic_banke' TEXT");
					update.exec();
					update.clear();
					update.prepare("ALTER TABLE stranke ADD COLUMN 'trr' TEXT");
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.27");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.27");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "12.1.2014");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 27 ) {

					// izbrisi opombe v predplacilnih racunih, ki se ne pricnejo in koncajo z ",", saj so le slednji dejanske opombe
					update.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '2'");
					update.exec();
					while ( update.next() ) {
						QString id = update.value(update.record().indexOf("id")).toString();
						QString opombe = update.value(update.record().indexOf("opombe")).toString();
						if ( opombe.left(1) != "," && opombe.right(1) != "," ) {
							QSqlQuery spremeni;
							spremeni.prepare("UPDATE racuni SET opombe = '' WHERE id LIKE '" + id + "'");
							spremeni.exec();
							spremeni.clear();
						}
					}
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.28");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.28");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "12.2.2014");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 28 ) {

					// dodamo stolpec za spol
					update.prepare("ALTER TABLE uporabniki ADD COLUMN 'spol' TEXT");
					update.exec();
					update.clear();

					// izbrisemo tabelo za nazive in dodamo ter napolnimo novo
					update.prepare("DROP TABLE IF EXISTS sif_naziv");
					update.exec();
					update.clear();

					tabela_nazivi();
					vnesi_nazive();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.29");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.29");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "6.6.2014");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 29 ) {

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.30");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.30");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "7.3.2014");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 30 ) {

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.31");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.31");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "29.12.2014");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 31 ) {

					QSqlQuery posodobi;

					// posodobi nazive prejemnika potnega naloga, kjer ga ni
					posodobi.prepare("SELECT * from potni_nalogi");
					posodobi.exec();
					while ( posodobi.next() ) {

						QString txt_naziv = "";

						// pridobi id prejemnika naloga, da iz tega dobis spol in naziv
						QSqlQuery sql_prejemnik;
						sql_prejemnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" +
											  pretvori(posodobi.value(posodobi.record().indexOf("prejemnik_oseba_id")).toString()) + "'");
						sql_prejemnik.exec();
						if ( sql_prejemnik.next() ) {
							QSqlQuery sql_naziv;
							sql_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" +
											  pretvori(sql_prejemnik.value(sql_prejemnik.record().indexOf("naziv")).toString()) + "'");
							sql_naziv.exec();
							if ( sql_naziv.next() ) {
								if ( pretvori(sql_prejemnik.value(sql_prejemnik.record().indexOf("spol")).toString()) == "1" ) { // moski
									txt_naziv = prevedi(sql_naziv.value(sql_naziv.record().indexOf("naziv_moski")).toString());
								}
								else {
									txt_naziv = prevedi(sql_naziv.value(sql_naziv.record().indexOf("naziv_zenski")).toString());
								}
							}
						}

						// posodobi profile
						update.prepare("UPDATE potni_nalogi SET prejemnik_oseba_naziv = ? WHERE id LIKE '" +
									   prevedi(posodobi.value(posodobi.record().indexOf("id")).toString()) + "'");
						update.bindValue(0, pretvori(txt_naziv));
						update.exec();
						update.clear();

						qApp->processEvents();
					}

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.32");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.32");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "01.01.2015");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 32 ) {

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.33");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.33");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "03.01.2015");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 33 ) {

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.34");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.34");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "03.01.2015");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 34 ) {

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					update.bindValue(0, "0.9.35");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					update.bindValue(0, "0.9.35");
					update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					update.exec();
					update.clear();

					update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					update.bindValue(0, "02.02.2015");
					update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					update.exec();
					update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 35 ) {

					// logotip
					// program vprasa za nov logotip, ga shrani v bazo na mesto podjetja in racunov
					QString pot = "";
					pot = QFileDialog::getOpenFileName(0, "Izberite sliko", QDir::homePath(), "Slika (*.png)");
					QImage slika(pot);

					QByteArray fotografija;
					QBuffer buffer( &fotografija );
					buffer.open( QBuffer::WriteOnly );
					QPixmap::fromImage(slika).scaledToWidth(70).save(&buffer, "PNG");

					QString besedilo = "";
					besedilo = fotografija.toBase64();

					update.prepare("UPDATE racuni SET podjetje_logotip = ?");
					update.bindValue(0, pretvori(besedilo));
					update.exec();
					update.clear();

					update.prepare("UPDATE podjetje SET logotip = ?");
					update.bindValue(0, pretvori(besedilo));
					update.exec();
					update.clear();

					// odstranitev kuponov iz baze

					// izbrisemo posamezne stolpce, ki se nanasajo na kupone
					// opravila
					update.prepare("ALTER TABLE opravila RENAME TO opravila_old");
					update.exec();
					update.clear();

					update.prepare("CREATE TABLE IF NOT EXISTS opravila ("
								   "id INTEGER PRIMARY KEY, "
								   "stevilka_stranke TEXT, "
								   "stevilka_projekta TEXT, "
								   "stevilka_racuna TEXT, "
								   "vrstni_red TEXT, "
								   "tip_racuna TEXT, "
								   "enota TEXT, "
								   "opravilo_sklop TEXT, "
								   "opravilo_skupina TEXT, "
								   "opravilo_storitev TEXT, "
								   "opravilo_rocno TEXT, "
								   "urna_postavka_brez_ddv TEXT, "
								   "urna_postavka_z_ddv TEXT, "
								   "ddv TEXT, "
								   "popust_fb1 TEXT, "
								   "popust_fb2 TEXT, "
								   "popust_komb1 TEXT, "
								   "popust_komb2 TEXT, "
								   "popust_stranka TEXT, "
								   "popust_akcija TEXT, "
								   "podrazitev_vikend TEXT, "
								   "podrazitev_hitrost TEXT, "
								   "podrazitev_zapleti TEXT, "
								   "pribitek_vikend TEXT, "
								   "pribitek_hitrost TEXT, "
								   "pribitek_zapleti TEXT, "
								   "tip_ur TEXT, "
								   "ur_dela TEXT, "
								   "rocni_vnos_ur TEXT, "
								   "znesek_popustov TEXT, "
								   "znesek_ddv TEXT, "
								   "znesek_koncni TEXT, "
								   "dobropis TEXT, "
								   "dobropis_st_ur TEXT, "
								   "dobropis_znesek TEXT, "
								   "dobropis_ddv TEXT, "
								   "dobropis_koncni TEXT, "
								   "casovnice TEXT, "
								   "sifra TEXT)"
								   );
					update.exec();
					update.clear();

					update.prepare("INSERT INTO opravila SELECT id, stevilka_stranke, stevilka_projekta, stevilka_racuna, vrstni_red, tip_racuna, enota, "
								   "opravilo_sklop, opravilo_skupina, opravilo_storitev, opravilo_rocno, urna_postavka_brez_ddv, urna_postavka_z_ddv, ddv, popust_fb1, "
								   "popust_fb2, popust_komb1, popust_komb2, popust_stranka, popust_akcija, podrazitev_vikend, podrazitev_hitrost, "
								   "podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, "
								   "znesek_ddv, znesek_koncni, dobropis, dobropis_st_ur, dobropis_znesek, dobropis_ddv, dobropis_koncni, casovnice, sifra "
								   "FROM opravila_old");
					update.exec();
					update.clear();

					// projekti
					update.prepare("ALTER TABLE projekti RENAME TO projekti_old");
					update.exec();
					update.clear();

					update.prepare("CREATE TABLE IF NOT EXISTS projekti ("
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
								   "popust_akcija TEXT, "
								   "podrazitev_vikend TEXT, "
								   "podrazitev_hitrost TEXT, "
								   "podrazitev_zapleti TEXT)"
								   );
					update.exec();
					update.clear();

					update.prepare("INSERT INTO projekti SELECT id, stevilka_projekta, naslov_projekta, stranka, avtor_oseba, pricetek_dela, konec_dela, "
								   "status_projekta, popust_fb1, popust_fb2, popust_komb1, popust_komb2, popust_stranka, popust_akcija, podrazitev_vikend, "
								   "podrazitev_hitrost, podrazitev_zapleti FROM projekti_old");
					update.exec();
					update.clear();

					// stranke
					update.prepare("ALTER TABLE stranke RENAME TO stranke_old");
					update.exec();
					update.clear();

					update.prepare("CREATE TABLE IF NOT EXISTS stranke ("
								   "id INTEGER PRIMARY KEY, "
								   "ime TEXT, "
								   "priimek TEXT, "
								   "naslov TEXT, "
								   "naslov_st TEXT, "
								   "posta TEXT, "
								   "postna_stevilka TEXT, "
								   "davcni_zavezanec TEXT, "
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
								   "pop_facebook_1 TEXT, "
								   "pop_facebook_2 TEXT, "
								   "pop_kombinacija_1 TEXT, "
								   "pop_kombinacija_2 TEXT, "
								   "pop_stranka TEXT, "
								   "pop_akcija TEXT, "
								   "pop_vsi_facebook TEXT, "
								   "pop_vsi TEXT, "
								   "pod_vikend TEXT, "
								   "pod_hitrost TEXT, "
								   "pod_zapleti TEXT, "
								   "avtor_podjetje TEXT, "
								   "avtor_oseba TEXT, "
								   "banka TEXT, "
								   "bic_banke TEXT, "
								   "trr TEXT)"
								   );
					update.exec();
					update.clear();

					update.prepare("INSERT INTO stranke SELECT id, ime, priimek, naslov, naslov_st, posta, postna_stevilka, davcni_zavezanec, davcna, kontakt, "
								   "telefon, gsm, email, spletna_stran, ustanova, opomba, tip, stalnost, aktivnost, placilnost, pop_facebook_1, pop_facebook_2, "
								   "pop_kombinacija_1, pop_kombinacija_2, pop_stranka, pop_akcija, pop_vsi_facebook, pop_vsi, pod_vikend, pod_hitrost, "
								   "pod_zapleti, avtor_podjetje, avtor_oseba, banka, bic_banke, trr FROM stranke_old");
					update.exec();
					update.clear();

					// odstrani vnos za pop_kupon iz sif_popusti
					update.prepare("DELETE FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kupon") + "'");
					update.exec();
					update.clear();

					// izbrisemo tabele, ki se nanasajo samo na kupone
					baza nova_baza;
					nova_baza.close_database();
					nova_baza.open_database();

					QSqlQuery sql_update;
					sql_update.prepare("DROP TABLE IF EXISTS kuponi");
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("DROP TABLE IF EXISTS sif_viri_strank");
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("DROP TABLE IF EXISTS opravila_old");
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("DROP TABLE IF EXISTS projekti_old");
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("DROP TABLE IF EXISTS stranke_old");
					sql_update.exec();
					sql_update.clear();

					// update database with new parm.
					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					sql_update.bindValue(0, "0.9.36");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					sql_update.bindValue(0, "0.9.36");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					sql_update.bindValue(0, "17.02.2015");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					sql_update.exec();
					sql_update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 36 ) {

					QSqlQuery sql_tabela;
					sql_tabela.prepare("ALTER TABLE racuni ADD COLUMN 'avans_ddv' TEXT");
					sql_tabela.exec();
					sql_tabela.clear();

					QSqlQuery sql_racuni;
					sql_racuni.prepare("SELECT * FROM racuni");
					sql_racuni.exec();
					while ( sql_racuni.next() ) {
						qApp->processEvents();
						double znesek_ddv = 0.0;
						QSqlQuery sql_opravila;
						sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(sql_racuni.value(sql_racuni.record().indexOf("id")).toString()) + "'");
						sql_opravila.exec();
						while ( sql_opravila.next() ) {
							qApp->processEvents();
							znesek_ddv += prevedi(sql_opravila.value(sql_opravila.record().indexOf("znesek_ddv")).toString()).toDouble();
						}

						znesek_ddv = znesek_ddv * prevedi(sql_racuni.value(sql_racuni.record().indexOf("odstotek_avansa")).toString()).toDouble() / 100;

						QSqlQuery sql_posodobi;
						sql_posodobi.prepare("UPDATE racuni SET avans_ddv = ? WHERE id LIKE '" + pretvori(sql_racuni.value(sql_racuni.record().indexOf("id")).toString()) + "'");
						sql_posodobi.bindValue(0, pretvori(QString::number(znesek_ddv, 'f', 2)));
						sql_posodobi.exec();
						qApp->processEvents();

						sql_posodobi.clear();
						sql_opravila.clear();
						znesek_ddv = 0.0;
						qApp->processEvents();
					}

					// update database with new parm.
					QSqlQuery sql_update;
					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					sql_update.bindValue(0, "0.9.37");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					sql_update.bindValue(0, "0.9.37");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					sql_update.bindValue(0, "08.04.2015");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					sql_update.exec();
					sql_update.clear();

					posodobi_bazo();

				}
				if ( stevilka_baze_min == 37 ) {

					QSqlQuery sql_tabela;
					sql_tabela.prepare("ALTER TABLE racuni ADD COLUMN 'podjetje_maticna' TEXT");
					sql_tabela.exec();
					sql_tabela.clear();

					QSqlQuery sql_racuni;
					sql_racuni.prepare("SELECT * FROM racuni");
					sql_racuni.exec();
					while ( sql_racuni.next() ) {
						qApp->processEvents();
						QSqlQuery sql_maticna;
						sql_maticna.prepare("SELECT * FROM podjetje WHERE id LIKE '" + pretvori(sql_racuni.value(sql_racuni.record().indexOf("podjetje_id")).toString()) + "'");
						sql_maticna.exec();
						if ( sql_maticna.next() ) {
							qApp->processEvents();
							QSqlQuery sql_posodobi;
							sql_posodobi.prepare("UPDATE racuni SET podjetje_maticna = ? WHERE id LIKE '" + pretvori(sql_racuni.value(sql_racuni.record().indexOf("id")).toString()) + "'");
							sql_posodobi.bindValue(0, pretvori(sql_maticna.value(sql_maticna.record().indexOf("maticna_stevilka")).toString()));
							sql_posodobi.exec();
							sql_posodobi.clear();
							qApp->processEvents();
						}
						sql_maticna.clear();
						qApp->processEvents();
					}
					sql_racuni.clear();

					// update database with new parm.
					QSqlQuery sql_update;
					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija programa'");
					sql_update.bindValue(0, "0.9.38");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_programa + 1, 10));
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Verzija baze'");
					sql_update.bindValue(0, "0.9.38");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_stevilke_baze + 1, 10));
					sql_update.exec();
					sql_update.clear();

					sql_update.prepare("UPDATE glavna SET vrednost = ?, razlicica = ? WHERE parameter LIKE 'Datum spremembe'");
					sql_update.bindValue(0, "31.08.2015");
					sql_update.bindValue(1, QString::number(zaporedna_stevilka_datuma_spremembe + 1, 10));
					sql_update.exec();
					sql_update.clear();

					posodobi_bazo();

				}
			}

		}

}

#include <QKeyEvent>
#include <QtSql>
#include <QDateTime>
#include <QMessageBox>
#include <QPainter>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>


#include "tiskanje.h"
#include "ui_tiskanje.h"
#include "kodiranje.h"
#include "varnost.h"

tiskanje::tiskanje(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::tiskanje)
{
	ui->setupUi(this);

		// izpraznimo polja
		ui->txt_vrsta_tiskanja->setText("");
		ui->txt_stevilke_dokumentov->setText("");
		ui->txt_format_tiskanja->setText("");

		ui->txt_druzina_pisave->clear();
		ui->txt_velikost_naslova->setText("9");
		ui->txt_velikost_vecja->setText("9");
		ui->txt_velikost_manjsa->setText("8");

		ui->txt_crta_debela->setText("2");
		ui->txt_crta_tanka->setText("1");

		ui->txt_presledek_med_vrsticami_tabela->setText("0,3");
		ui->txt_presledek_med_vrsticami_besedilo->setText("0,1");

		ui->txt_velikost_papirja->clear();
		ui->txt_kvaliteta_tiska->clear();
		ui->txt_stevilo_kopij->setText("1");
		ui->txt_uporaba_barv->clear();
		ui->txt_lega_lista->clear();
		ui->txt_duplex->clear();

		ui->cb_opombe->setText("Natisnem tudi opombe?");

		ui->txt_rob_lista_zgoraj->setText("20,0"); // v milimetrih
		ui->txt_rob_lista_levo->setText("20,0"); // v milimetrih
		ui->txt_rob_lista_desno->setText("20,0"); // v milimetrih
		ui->txt_rob_lista_spodaj->setText("20,0"); // v milimetrih

		// napolnimo spustne sezname
		ui->txt_druzina_pisave->addItem("");
		ui->txt_druzina_pisave->addItem("Arial");
		ui->txt_druzina_pisave->addItem("Times New Roman");
		ui->txt_druzina_pisave->addItem("Courier New");

		ui->txt_velikost_papirja->addItem("");
		ui->txt_velikost_papirja->addItem("A4");
		ui->txt_velikost_papirja->addItem("A5");

		ui->txt_kvaliteta_tiska->addItem("");
		ui->txt_kvaliteta_tiska->addItem("Osnutek");
		ui->txt_kvaliteta_tiska->addItem("Normalno");
		ui->txt_kvaliteta_tiska->addItem("Kvalitetno");

		ui->txt_uporaba_barv->addItem("");
		ui->txt_uporaba_barv->addItem("Crnobelo");
		ui->txt_uporaba_barv->addItem("Barvno");

		ui->txt_lega_lista->addItem("");
		ui->txt_lega_lista->addItem("Lezece");
		ui->txt_lega_lista->addItem("Pokoncno");

		ui->txt_duplex->addItem("");
		ui->txt_duplex->addItem("DA");
		ui->txt_duplex->addItem("NE");

		// izberemo privzete vrednosti v spustnih seznamih
		ui->txt_druzina_pisave->setCurrentIndex(1);
		ui->txt_velikost_papirja->setCurrentIndex(1);
		ui->txt_kvaliteta_tiska->setCurrentIndex(2);
		ui->txt_uporaba_barv->setCurrentIndex(2);
		ui->txt_lega_lista->setCurrentIndex(2);
		ui->txt_duplex->setCurrentIndex(1);

		// prikazemo lbl_list kot bel list pravilnih dimenzij
		ui->lbl_list->setAutoFillBackground(true);
		ui->lbl_list->setPalette(QPalette(qRgb(255,255,255)));

		// skrijemo polja, ki jih (se) ne moremo uporabljati
		ui->lbl_kvaliteta_tiska->setHidden(true);
		ui->txt_kvaliteta_tiska->setHidden(true);

		ui->txt_vrsta_tiskanja->setHidden(true);
		ui->txt_stevilke_dokumentov->setHidden(true);
		ui->txt_format_tiskanja->setHidden(true);

		ui->tab_tiskanje->setCurrentIndex(0);

}

tiskanje::~tiskanje() {
	delete ui;
}

void tiskanje::on_btn_izhod_clicked() {

	close();

}

void tiskanje::on_btn_natisni_clicked() {

	QString staro_besedilo = ui->btn_natisni->text();
	ui->btn_natisni->setEnabled(false);
	ui->btn_natisni->setText("");

	int stevilo_dokumentov = 0;
	if ( ui->txt_stevilke_dokumentov->text() != "" ) {
		stevilo_dokumentov = ui->txt_stevilke_dokumentov->text().count(",");
	//	stevilo_dokumentov += 1; // zadnji dokument nima vejice
	}

	if ( stevilo_dokumentov != 0 ) { // ce nimamo prisotnih nobenih dokumentov, potem nimamo kaj tiskati

		QString stevilke_dokumentov = ui->txt_stevilke_dokumentov->text();

		for ( int i = 1; i <= stevilo_dokumentov; i++ ) {

			ui->btn_natisni->setText(QString::number(i, 10) + " od " + QString::number(stevilo_dokumentov));
			qApp->processEvents();

			QString id = stevilke_dokumentov.left(stevilke_dokumentov.indexOf(",", 0));
			stevilke_dokumentov = stevilke_dokumentov.right(stevilke_dokumentov.length() - stevilke_dokumentov.indexOf(",", 0) - 1);

			if ( ui->txt_vrsta_tiskanja->text() == "prejeti-racuni" ) {
				natisni_prejeti_racun(id);
			} // if ( ui->txt_vrsta_tiskanja->text() == "prejeti-racuni" )

			else if ( ui->txt_vrsta_tiskanja->text() == "izdani-racuni" ) {
				natisni_izdani_racun(id);
			} // if ( ui->txt_vrsta_tiskanja->text() == "izdani-racuni" )

			else if ( ui->txt_vrsta_tiskanja->text() == "potni-nalogi" ) {
				natisni_potni_nalog(id);
			} // if ( ui->txt_vrsta_tiskanja->text() == "potni-nalogi" )

		} // for ( int i = 1; i <= stevilo_dokumentov; i++ )

		if ( ui->txt_format_tiskanja->text() == "pdf" ) {
			QMessageBox sporocilo;
			sporocilo.setText("Tiskanje v PDF format je koncano, \nustvarili ste " + QString::number(stevilo_dokumentov, 10) + " dokumentov.");
			sporocilo.exec();
		} // if ( ui->txt_format_tiskanja->text() == "pdf" )

	}	// if ( stevilo_dokumentov != 0 )
	else {
		QMessageBox sporocilo;
		sporocilo.setText("Ni dokumentov za tisk!");
		sporocilo.exec();
	}

	close();

	ui->btn_natisni->setText(staro_besedilo);
	ui->btn_natisni->setEnabled(true);

}

void tiskanje::on_btn_nastavitve_racuni_clicked() {

	ui->txt_velikost_naslova->setText("10");
	ui->txt_velikost_vecja->setText("9");
	ui->txt_velikost_manjsa->setText("8");

	ui->txt_crta_debela->setText("2");
	ui->txt_crta_tanka->setText("1");

	ui->txt_presledek_med_vrsticami_tabela->setText("0,3");
	ui->txt_presledek_med_vrsticami_besedilo->setText("0,1");

}

void tiskanje::on_btn_nastavitve_potni_nalogi_clicked(){

	ui->txt_velikost_naslova->setText("9");
	ui->txt_velikost_vecja->setText("9");
	ui->txt_velikost_manjsa->setText("8");

	ui->txt_crta_debela->setText("0");
	ui->txt_crta_tanka->setText("0");

	ui->txt_presledek_med_vrsticami_tabela->setText("0,3");
	ui->txt_presledek_med_vrsticami_besedilo->setText("0,1");

}

void tiskanje::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_natisni_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}

}

QString tiskanje::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString tiskanje::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString tiskanje::pretvori_v_int(QString besedilo) {

	besedilo.replace(",", "."); // zamenja decimalno piko (int) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice
	besedilo = besedilo.left(besedilo.indexOf(".")); // obdrzi samo prvi, celi del stevila

	return besedilo;

}

QString tiskanje::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	return besedilo;

}

QString tiskanje::pretvori_iz_double(QString besedilo) {

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

void tiskanje::prejem(QString vrsta, QString stevilke, QString format) {

	ui->txt_vrsta_tiskanja->setText(vrsta);
	ui->txt_stevilke_dokumentov->setText(stevilke);
	ui->txt_format_tiskanja->setText(format);
	ui->gb_ostalo->setHidden(false);

	if ( vrsta == "potni-nalogi" ) {
		on_btn_nastavitve_potni_nalogi_clicked();
	}
	else {
		on_btn_nastavitve_racuni_clicked();
	}

}

void tiskanje::natisni_potni_nalog(QString id) {

	/**
		* id nosi id stevilko potnega naloga, s cimer lahko hkrati natisnemo
		* vec dokumentov, bodisi istih, bodisi razlicnih
		**/

	/**
		* Nastavimo spremenljivke, ki jih bomo uporabili v dokumentu
		**/
	// podatki o potnem nalogu
	QString stevilka_naloga = "";
	QString stevilka_dokumenta = "";
	QString datum_naloga = "";
	QString namen_potnega_naloga = "";
	QString prevozno_sredstvo = "";
	QString cena_dnevnice_6_8 = "0.00";
	QString cena_dnevnice_8_12 = "0.00";
	QString cena_dnevnice_12_24 = "0.00";
	QString stevilo_dnevnic_6_8 = "0";
	QString stevilo_dnevnic_8_12 = "0";
	QString stevilo_dnevnic_12_24 = "0";
	QString stroski_skupaj = "";
	QString razdalja = "0";
	QString kilometrina = "";
	QString ostali_stroski = "";
	QString znesek_drugih_stroskov = "";
	QString zvisanje_dnevnic = "0";
	QString priloge = "";
	QString stevilo_dnevnic_1 = "0";
	QString stevilo_dnevnic_2 = "0";
	QString cena_dnevnic_1 = "0.00";
	QString cena_dnevnic_2 = "0.00";
	QString cena_dnevnic = "0.00";
	QString opombe = "";

	// podatki o predlagatelju - podjetje
	QString predlagatelj_podjetje_ime;
	QString predlagatelj_podjetje_polno_ime;
	QString predlagatelj_podjetje_naslov;
	QString predlagatelj_podjetje_naslov_st;
	QString predlagatelj_podjetje_postna_st;
	QString predlagatelj_podjetje_posta;
	QString preglagatelj_podjetje_logotip;

	// podatki o predlagatelju - oseba
	QString predlagatelj_oseba_ime;
	QString predlagatelj_oseba_priimek;
	QString predlagatelj_oseba_naslov;
	QString predlagatelj_oseba_naslov_stevilka;
	QString predlagatelj_oseba_postna_stevilka;
	QString predlagatelj_oseba_posta;

	// podatki o prejemniku
	QString prejemnik_ime;
	QString prejemnik_priimek;
	QString prejemnik_naziv;
	QString prejemnik_naslov;
	QString prejemnik_naslov_stevilka;
	QString prejemnik_postna_stevilka;
	QString prejemnik_posta;
	QString prejemnik_znamka_avtomobila;
	QString prejemnik_model_avtomobila;
	QString prejemnik_registrska_stevilka;

	// podatki o potovanju
	QString relacija;
	QString datum_odhoda;
	QString ura_odhoda;
	QString potovanje;
	QString datum_prihoda;
	QString ura_prihoda;
	QString dnevi_potovanja;
	QDateTime dt_trajanje;
	QDateTime dt_datum_odhoda;
	QDateTime dt_datum_prihoda;
	int minute;
	int ure;

	QString mapa_za_shranjevanje = "";

	// odpri podatke o potnem nalogu, prejemniku, stroskih...
		/**
			* na zacetku zberemo podatke o potnem nalogu, izdajatelju potnega naloga
			* (podjetje, oseba), prejemniku potnega naloga ter opravljeni poti.
			**/

			// podatki o potnem nalogu
			QSqlQuery sql_potni_nalog;
			sql_potni_nalog.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + pretvori(id) + "'");
			sql_potni_nalog.exec();
			if ( sql_potni_nalog.next() ) {
				stevilka_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString());
				stevilka_dokumenta = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_dokumenta")).toString());
				datum_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("datum_naloga")).toString());
				namen_potnega_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("namen_naloga")).toString());
				opombe = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("opombe")).toString());

				// podatki o predlagatelju - podjetje
				predlagatelj_podjetje_ime = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_kratki")).toString());
				predlagatelj_podjetje_polno_ime = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_dolgi")).toString());
				predlagatelj_podjetje_naslov = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_naslov_ulica")).toString());
				predlagatelj_podjetje_naslov_st = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_naslov_stevilka")).toString());
				predlagatelj_podjetje_postna_st = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_postna_stevilka")).toString());
				predlagatelj_podjetje_posta = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_naslov_posta")).toString());
				preglagatelj_podjetje_logotip = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_logotip")).toString());

				// podatki o predlagatelju - oseba
				predlagatelj_oseba_ime = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba_ime")).toString());
				predlagatelj_oseba_priimek = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba_priimek")).toString());
				predlagatelj_oseba_naslov = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba_naslov_ulica")).toString());
				predlagatelj_oseba_naslov_stevilka = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba_naslov_stevilka")).toString());
				predlagatelj_oseba_postna_stevilka = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba_naslov_postna_stevilka")).toString());
				predlagatelj_oseba_posta = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba_naslov_posta")).toString());

				// podatki o prejemniku
				prejemnik_ime = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba_ime")).toString());
				prejemnik_priimek = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba_priimek")).toString());
				prejemnik_naziv = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba_naziv")).toString());
				prejemnik_naslov = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba_naslov_ulica")).toString());
				prejemnik_naslov_stevilka = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba_naslov_stevilka")).toString());
				prejemnik_postna_stevilka = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba_naslov_postna_stevilka")).toString());
				prejemnik_posta = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba_naslov_posta")).toString());
				prejemnik_znamka_avtomobila = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("proizvajalec")).toString());
				prejemnik_model_avtomobila = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("znamka")).toString());
				prejemnik_registrska_stevilka = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("registrska_stevilka")).toString());

//				cena_dnevnice = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnic")).toString());

				prevozno_sredstvo = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prevozno_sredstvo")).toString());

				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("priznana_dnevnica")).toString()) != "0" ) {
					cena_dnevnice_6_8 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_6_8")).toString());
					cena_dnevnice_8_12 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_8_12")).toString());
					cena_dnevnice_12_24 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_12_24")).toString());

					stevilo_dnevnic_6_8 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_6_8")).toString());
					stevilo_dnevnic_8_12 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_8_12")).toString());
					stevilo_dnevnic_12_24 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_12_24")).toString());
				}

				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("zajtrk_8_12")).toString()) != "0" ) {
					double cenadnevnice = 0.0;
					cenadnevnice = cena_dnevnice_8_12.toDouble() * 0.85;
					cena_dnevnice_8_12 = QString::number(cenadnevnice, 'f', 2);
				}
				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("zajtrk_12_24")).toString()) != "0" ) {
					double cenadnevnice = 0.0;
					cenadnevnice = cena_dnevnice_12_24.toDouble() * 0.90;
					cena_dnevnice_12_24 = QString::number(cenadnevnice, 'f', 2);
				}

				stroski_skupaj = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stroski_skupaj")).toString());
				razdalja = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("skupaj_kilometri")).toString()).replace(".", ",");
				kilometrina = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("kilometrina")).toString()).replace(".", ",");
				priloge =  prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("priloge")).toString()).replace("\n", ", ");

				// uredi dnevnice
				/**
					* ce obataja dnevnica/dnevnice za vec kot 12 ur, jih dodaj kot
					* dnevnica_2, ki jo bomo prikazali v spodnji vrstici
					* poglej, ce obstaja poleg dnevnice_2 se dnevnica_1, ki je lahko
					* polovicna ali tretjinska, ne moreta biti pa obe hkrati
					* dodaj jo kot dnevnica_1
					* ce ne obstaja celodnevna, potem kot dnevnica_2 dodaj ali polovicno
					* ali
					**/
				if ( stevilo_dnevnic_12_24 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_12_24;
					cena_dnevnic_2 = cena_dnevnice_12_24;
					if ( stevilo_dnevnic_6_8 != "0" ) {
						stevilo_dnevnic_1 = stevilo_dnevnic_6_8;
						cena_dnevnic_1 = cena_dnevnice_6_8;
					}
					else if ( stevilo_dnevnic_8_12 != "0" ) {
						stevilo_dnevnic_1 = stevilo_dnevnic_8_12;
						cena_dnevnic_1 = cena_dnevnice_8_12;
					}
				}
				else if ( stevilo_dnevnic_6_8 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_6_8;
					cena_dnevnic_2 = cena_dnevnice_6_8;
				}
				else if ( stevilo_dnevnic_8_12 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_8_12;
					cena_dnevnic_2 = cena_dnevnice_8_12;
				}

				// podatki o drugih stroskih
				QSqlQuery sql_stroski;
				sql_stroski.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(id) + "'");
				sql_stroski.exec();
				double stroski = 0.0;
				while ( sql_stroski.next() ) {
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("strosek")).toString()) + ",";
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()) + ";";
					stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()).toDouble();
				}
				znesek_drugih_stroskov = QString::number(stroski, 'f', 2);

				QSqlQuery sql_naloga;
				sql_naloga.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + pretvori(namen_potnega_naloga) + "'");
				sql_naloga.exec();
				if ( sql_naloga.next() ) {
					namen_potnega_naloga = prevedi(sql_naloga.value(sql_naloga.record().indexOf("namen")).toString());
				}

				QSqlQuery sql_prevoz;
				sql_prevoz.prepare("SELECT * FROM sif_prevozna_sredstva WHERE id LIKE '" + pretvori(prevozno_sredstvo) + "'");
				sql_prevoz.exec();
				if ( sql_prevoz.next() ) {
					prevozno_sredstvo = prevedi(sql_prevoz.value(sql_prevoz.record().indexOf("prevoz")).toString());
				}

				// podatki o potovanju
				QSqlQuery sql_pot;
				sql_pot.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + pretvori(id) + "'");
				sql_pot.exec();
				QString rel_start = "";
				QString rel_cilj = "";
				dt_datum_odhoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
				dt_datum_prihoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");

				while ( sql_pot.next() ) {

					// racunanje relacije potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString()) != rel_cilj ) {
						rel_start = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString());
					}
					rel_cilj = ""; // izpraznimo cilj, da se ne podvaja

					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString()) != rel_start ) {
						rel_cilj = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString());
					}
					if ( rel_start != "" && relacija != "" ) {
						rel_start = " - " + rel_start;
					}
					if ( rel_cilj != "" ) {
						rel_cilj = " - " + rel_cilj;
					}
					if ( relacija != "" ) {
						relacija = relacija + rel_cilj;
					}
					else {
						relacija = rel_start + rel_cilj;
					}
					rel_start = ""; // izpraznimo start, da se ne podvaja

					// racunanje koncnih tock potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()) != "" ) {
						potovanje = prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()).replace("\n", ", ") + "\n";
					}
			// uporabi samo, ce se bo pokazala potreba po odstranitvi zadnje vrstice
			// trenutno odstranjeno, saj ce zanji kraj nima vnesenega naslova, vseeno reze znake
			//		potovanje = potovanje.remove(potovanje.length() - 2, 2); // zadnji kraj ima tudi prelom v novo vrstico, ki pa tu ni potreben

					// racunanje podatkov o odhodu
					if ( dt_datum_odhoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_odhoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) < 0 ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_odhoda = dt_datum_odhoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					dt_trajanje = dt_datum_odhoda;
					ura_odhoda = datum_odhoda.right(5);
					datum_odhoda = datum_odhoda.left(datum_odhoda.length() - 6);

					if ( dt_datum_prihoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_prihoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) > 0 ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_prihoda = dt_datum_prihoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					ura_prihoda = datum_prihoda.right(5);
					datum_prihoda = datum_prihoda.left(datum_prihoda.length() - 6);
				}
				dnevi_potovanja = QString::number(dt_trajanje.daysTo(dt_datum_prihoda), 10);
				minute = (dt_trajanje.secsTo(dt_datum_prihoda) - dt_trajanje.daysTo(dt_datum_prihoda) * 24 * 60 * 60) / 60;
				ure = minute / 60;
				minute = minute - ure * 60;
			}

			QSqlQuery sql_pot;
			sql_pot.prepare("SELECT * FROM nastavitve WHERE naziv LIKE '" + pretvori("pot") + "'");
			sql_pot.exec();
			if ( sql_pot.next() ) {
				mapa_za_shranjevanje = prevedi(sql_pot.value(sql_pot.record().indexOf("vrednost")).toString());
			}

	/**
		* Odpremo sifrant z besedilom potnega naloga
		* Besedilo bomo vztavljali z readLine
		**/

		QString app_path = QApplication::applicationDirPath();
		QFile datoteka(app_path + "/potni-nalog-besedilo.csv");
		if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return;
		}
		QTextStream potni_nalog(&datoteka);
        potni_nalog.setCodec("UTF-8");

	/**
		* Nastavimo podatke o tiskanju. Tu tiskamo v PDF dokument
		* velikost A4, kar po obliki ustreza tiskanju v koncni razlicici, ko
		* se bo tiskalo tako v PDF dokument kot direktno na tiskalnik
		*/

	// ustvariti pot do ustrezne mape
	QDir mapa(mapa_za_shranjevanje);
	mapa.mkdir("potni-nalogi");
	mapa.cd("potni-nalogi");
	mapa.mkdir(datum_naloga.right(4));
	mapa.cd(datum_naloga.right(4));

	// podatki o printerju
	QPrinter printer;
	if ( ui->txt_format_tiskanja->text() == "pdf" ) {
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(mapa.path() + "/potni-nalog-" + datum_naloga.right(4) + "-" + stevilka_naloga.right(11) + ".pdf");
	}

	if ( ui->txt_velikost_papirja->currentText() == "A4" ) {
		printer.setPaperSize(QPrinter::A4);
	}
	else if ( ui->txt_velikost_papirja->currentText() == "A5" ) {
		printer.setPaperSize(QPrinter::A5);
	}

	if ( ui->txt_lega_lista->currentText() == "Lezece" ) {
		printer.setOrientation(QPrinter::Landscape);
	}
	else if ( ui->txt_lega_lista->currentText() == "Pokoncno" ) {
		printer.setOrientation(QPrinter::Portrait);
	}

	if ( ui->txt_uporaba_barv->currentText() == "Barvno" ) {
		printer.setColorMode(QPrinter::Color);
	}
	else if ( ui->txt_uporaba_barv->currentText() == "Crnobelo" ) {
		printer.setColorMode(QPrinter::GrayScale);
	}

	if ( ui->txt_duplex->currentText() == "DA" ) {
		printer.setDuplex(QPrinter::DuplexLongSide);
	}
	else if ( ui->txt_duplex->currentText() == "NE" ) {
		printer.setDuplex(QPrinter::DuplexNone);
	}
	else {
		printer.setDuplex(QPrinter::DuplexAuto);
	}

	printer.setCopyCount(pretvori_v_int(ui->txt_stevilo_kopij->text()).toInt());

	printer.setPageMargins(pretvori_v_double(ui->txt_rob_lista_levo->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_zgoraj->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_desno->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_spodaj->text()).toDouble(),
												 QPrinter::Millimeter);

	QPainter painter;

	if (! painter.begin(&printer))  { // failed to open file
		qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
		return;
	}

	double pozicija = 0;
	double sirina_besedila = 0;
	double razmik_med_vrsticami = 0;
	double faktor_razmika_med_vrsticami_1 = pretvori_v_double(ui->txt_presledek_med_vrsticami_tabela->text()).toDouble(); // pri tabelah
	double faktor_razmika_med_vrsticami_2 = pretvori_v_double(ui->txt_presledek_med_vrsticami_besedilo->text()).toDouble(); // pri besedilu

	int stevilka_strani = 1;
	int visina_glave = 0;
	int visina_noge = 0;

	QPen *svincnik = new QPen;

	QString besedilo = "";

	QFont vstavljeno_besedilo(ui->txt_druzina_pisave->currentText(), pretvori_v_double(ui->txt_velikost_vecja->text()).toDouble());
	QFont stalno_besedilo(ui->txt_druzina_pisave->currentText(), pretvori_v_double(ui->txt_velikost_vecja->text()).toDouble(), QFont::Bold);
	QFont naslovno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_naslova->text().toInt(), QFont::Bold);

	// narisi glavo
		visina_glave = natisni_glavo_potni_nalog(painter, id);
		visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);

		pozicija += visina_glave;

		// nastavitev pisave
		painter.setFont(stalno_besedilo);
		QRect velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Lorem Ipsum");

	// podatki o prejemniku potnega naloga
		// nastavimo nov razmik med vrsticami
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() / 2 + razmik_med_vrsticami;

		// nastavimo besedilo (Odrejam, da odpotuje:)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.width(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_ime + " " + prejemnik_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (na delovnem mestu)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naziv;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (stanujoc)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naslov + " " + prejemnik_naslov_stevilka + ", " + prejemnik_postna_stevilka + " " + prejemnik_posta;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (dne: ob: uri)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (po nalogu/dokumentu)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		if ( stevilka_dokumenta != "" ) {
			// nastavimo besedilo
			besedilo = stevilka_dokumenta;
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// narisemo besedilo
			painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		}
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// naslov vmesnih in koncnih destinacij ter posameznih nalog

			// nastavimo besedilo
			QString besedilo_naslov = potni_nalog.readLine() + " ";
			QString besedilo_naloga = " " + potni_nalog.readLine() + " ";

			QSqlQuery sql_potovanje;
			sql_potovanje.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + pretvori(id) + "'");
			sql_potovanje.exec();
			while ( sql_potovanje.next() ) {

				// morebitni preskok na novo stran
				if ( ( pozicija + visina_glave + visina_noge + velikost_besedila.height() ) >= painter.window().height() ) { // prelom na novo stran
					printer.newPage();

					// natisnemo novo glavo in nogo
					visina_glave = natisni_glavo_potni_nalog(painter, id);
					visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
					pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
				}

				if ( sql_potovanje.value(sql_potovanje.record().indexOf("naslov_ulica")).toString() != "" ) { // izognemo se zadnjemu polju, ki nima naslova - izhodisce ...
					QString potni_nalog_naslov = prevedi(sql_potovanje.value(sql_potovanje.record().indexOf("naslov_ulica")).toString()) + " " +
							prevedi(sql_potovanje.value(sql_potovanje.record().indexOf("naslov_stevilka")).toString()) + ", " +
							prevedi(sql_potovanje.value(sql_potovanje.record().indexOf("naslov_postna_stevilka")).toString()) + " " +
							prevedi(sql_potovanje.value(sql_potovanje.record().indexOf("naslov_posta")).toString())
							;
					if ( prevedi(sql_potovanje.value(sql_potovanje.record().indexOf("naziv_ciljnega_podjetja")).toString()) != "" ) {
						potni_nalog_naslov = prevedi(sql_potovanje.value(sql_potovanje.record().indexOf("naziv_ciljnega_podjetja")).toString()) +
								" (" + potni_nalog_naslov + ")";
					}

					QString potni_nalog_namen = "";
					QSqlQuery sql_namen;
					sql_namen.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + sql_potovanje.value(sql_potovanje.record().indexOf("namen_potovanja")).toString() + "'");
					sql_namen.exec();
					if ( sql_namen.next() ) {
						potni_nalog_namen = prevedi(sql_namen.value(sql_namen.record().indexOf("namen")).toString());
					}

					// nastavimo besedilo ( v/na )
					besedilo = besedilo_naslov;
					// nastavimo tip pisave
					painter.setFont(stalno_besedilo);
					// nastavimo polozaj na listu, kjer zapisemo besedilo
					sirina_besedila = 0;
					velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					// narisemo besedilo
					painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

					// nastavimo besedilo
					besedilo = potni_nalog_naslov;
					// nastavimo tip pisave
					painter.setFont(vstavljeno_besedilo);
					// nastavimo polozaj na listu, kjer zapisemo besedilo
					velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					// narisemo besedilo
					painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

					// nastavimo besedilo ( z nalogo )
					besedilo = besedilo_naloga;
					// nastavimo tip pisave
					painter.setFont(stalno_besedilo);
					// nastavimo polozaj na listu, kjer zapisemo besedilo
					velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					// narisemo besedilo
					painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

					// nastavimo besedilo
					besedilo = potni_nalog_namen;
					// nastavimo tip pisave
					painter.setFont(vstavljeno_besedilo);
					// nastavimo polozaj na listu, kjer zapisemo besedilo
					velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					// narisemo besedilo
					painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
					sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

					// nastavimo novo pozicijo besedila
					pozicija += velikost_besedila.height() + razmik_med_vrsticami;
				}

			}

		// morebitni preskok na novo stran ( 2 vrstici )
		if ( ( pozicija + visina_glave + visina_noge + velikost_besedila.height() * 2 + razmik_med_vrsticami * 2 ) >= painter.window().height() ) { // prelom na novo stran
			printer.newPage();

			// natisnemo novo glavo in nogo
			visina_glave = natisni_glavo_potni_nalog(painter, id);
			visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
			pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
		}

		// nastavimo besedilo (Potovanje bo tajalo do:)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo ( to je )
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo ( dni )
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo ( Odobravam uporabo )
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		if ( prevozno_sredstvo == "Avtomobil" ) {
			// nastavimo besedilo
			besedilo = "(" + prevozno_sredstvo + "): " + prejemnik_znamka_avtomobila + " " + prejemnik_model_avtomobila;
		}
		else {
			// nastavimo besedilo
			besedilo = prevozno_sredstvo;
		}
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() / 2 + razmik_med_vrsticami;

		// morebitni preskok na novo stran ( 10 vrstic )
		if ( ( pozicija + visina_glave + visina_noge + velikost_besedila.height() * 10 + razmik_med_vrsticami * 10 ) >= painter.window().height() ) { // prelom na novo stran
			printer.newPage();

			// natisnemo novo glavo in nogo
			visina_glave = natisni_glavo_potni_nalog(painter, id);
			visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
			pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
		}

		// nastavimo besedilo (Potne stroske placa)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_polno_ime;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (visina dnevnice)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
//		besedilo = cena_dnevnice.replace(".", ",") + " EUR";
		besedilo = cena_dnevnice_12_24.replace(".", ",") + " EUR";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + 15;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// manjka besedilo posebnih dodatkov, dokler ne izvemo, kaj to je...
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (Odobravam izplacilo predujma)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() / 2 + razmik_med_vrsticami;

	// tabela predujma
		int prvotna_visina = pozicija; // ohranimo zacetno visino za prvo in drugo tretjino
		int visina_sklopa = 0; // najprej narisemo desno tretjino, ki nosi najvec podatkov in je tudi najvisja

		// prva vrstica je prazna
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta zgoraj
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(painter.window().width() * 2 / 3, pozicija, painter.window().width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis odredbodajalca)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(painter.window().width() * 2 / 3, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// dve prazni vrstici
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(painter.window().width() * 2 / 3, pozicija, painter.window().width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis prejemnika)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(painter.window().width() * 2 / 3, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() / 2 + razmik_med_vrsticami;

		// dolocimo koncno visino
		visina_sklopa = pozicija;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo ( ZIG )
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (Posluje brez ziga)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// napisemo prvo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (Predujem EUR)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (Prejel dne)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// morebitni preskok na novo stran ( 2 vrstici )
		if ( ( pozicija + visina_glave + visina_noge + velikost_besedila.height() * 4 + razmik_med_vrsticami * 4 ) >= painter.window().height() ) { // prelom na novo stran
			printer.newPage();

			// natisnemo novo glavo in nogo
			visina_glave = natisni_glavo_potni_nalog(painter, id);
			visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
			pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
		}

	// glava obracuna potnih stroskov
		pozicija = visina_sklopa;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// nastavimo odmik od crte
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1;
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Obracun potnih stroskov)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(naslovno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, painter.window().width() - razmik_med_vrsticami, 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width() - razmik_med_vrsticami, velikost_besedila.height(), Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta sredina
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_ime + " " + predlagatelj_oseba_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_naslov + " " + predlagatelj_oseba_naslov_stevilka + ", " + predlagatelj_oseba_postna_stevilka + " " + predlagatelj_oseba_posta;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta spodaj
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// crti levo in desno
		painter.drawLine(QLine(0, visina_sklopa, 0, pozicija)); // levo
		painter.drawLine(QLine(painter.window().width(), visina_sklopa, painter.window().width(), pozicija)); // desno

		// nastavimo odmik od crte
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// morebitni preskok na novo stran ( 12 vrstic )
		if ( ( pozicija + visina_glave + visina_noge + velikost_besedila.height() * 12 + razmik_med_vrsticami * 12 ) >= painter.window().height() ) { // prelom na novo stran
			printer.newPage();

			// natisnemo novo glavo in nogo
			visina_glave = natisni_glavo_potni_nalog(painter, id);
			visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
			pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
		}

	// obracun potnih stroskov
		// nastavitev zacetne pozicije
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1;
		prvotna_visina = pozicija;
		double polje_1 = painter.window().width() * 7 / 8;
		double polje_2 = painter.window().width() / 8;
		double polje_3 = polje_2 / 2; // dnevi ne potrebujejo toliko prostora, ure/minute pa več

		// crta zgoraj
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odsotnost)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(3 * polje_2 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		if ( stevilo_dnevnic_1 == "0" ) { // ce ni dnevnic ali je samo ena, potem so napisi prek dveh vrstic, v nasprotnem primeru samo prek ene
			// nastavimo besedilo (dnevnice)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, velikost_besedila.height() * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, velikost_besedila.height() * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, velikost_besedila.height() * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo novo pozicijo besedila
			pozicija += velikost_besedila.height() + razmik_med_vrsticami;

			// crta vmesna vodoravna 1
			svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
			painter.setPen(*svincnik);
			painter.drawLine(QLine(0, pozicija, polje_2 * 5, pozicija));
		}
		else  { // obstajata dva razlicna tipa dnevnic, zato se naslov razpotega zgolj prek ene same vrstice
			// nastavimo besedilo (dnevnice)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo novo pozicijo besedila
			pozicija += velikost_besedila.height() + razmik_med_vrsticami;

			// crta vodoravna 2
			svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
			painter.setPen(*svincnik);
			painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));
		}

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum odhoda)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila - polje_3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila - polje_3, pozicija, 5 * polje_2 - sirina_besedila + polje_3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		if ( stevilo_dnevnic_1 != "0" ) { // obstajata dva razlicna tipa dnevnic
			// nastavimo besedilo (dnevnice_1)
			besedilo = stevilo_dnevnic_1.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic_1)
			besedilo = cena_dnevnic_1.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = QString::number(pretvori_v_double(cena_dnevnic_1).toDouble() * pretvori_v_double(stevilo_dnevnic_1).toDouble(), 'f', 2).replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		}
		// razmik med crtami
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta vodoravna 3
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum prihoda)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila - polje_3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = QString::number(ure, 10) + " ur " + QString::number(minute, 10) + " minut";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila - polje_3, pozicija, 5 * polje_2 - sirina_besedila + polje_3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dnevnice_2)
		besedilo = stevilo_dnevnic_2.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (cena dnevnic_2)
		besedilo = cena_dnevnic_2.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (skupaj)
		besedilo = QString::number(pretvori_v_double(cena_dnevnic_2).toDouble() * pretvori_v_double(stevilo_dnevnic_2).toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// razmik med crtami
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// pokoncne crte 1 sklop
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(3 * polje_2, prvotna_visina, 3 * polje_2, pozicija));
		painter.drawLine(QLine(5 * polje_2, prvotna_visina, 5 * polje_2, pozicija));
		painter.drawLine(QLine(6 * polje_2, prvotna_visina, 6 * polje_2, pozicija));

		prvotna_visina += velikost_besedila.height() + razmik_med_vrsticami * 2;

		painter.drawLine(QLine(1 * polje_2, prvotna_visina, 1 * polje_2, pozicija));
		painter.drawLine(QLine(2 * polje_2, prvotna_visina, 2 * polje_2, pozicija));
		painter.drawLine(QLine(4 * polje_2 - polje_3, prvotna_visina, 4 * polje_2 - polje_3, pozicija));

		// vrnitev prvotne pozicije
		prvotna_visina = prvotna_visina - velikost_besedila.height() - razmik_med_vrsticami * 2 + 1;

		// crta vodoravna 4
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (% zvisanja dnevnic)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = zvisanje_dnevnic;
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta vodoravna 5
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, velikost_besedila.height(), Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		cena_dnevnic = QString::number(pretvori_v_double(cena_dnevnice_6_8).toDouble() * pretvori_v_double(stevilo_dnevnic_6_8).toDouble() +
																	 pretvori_v_double(cena_dnevnice_8_12).toDouble() * pretvori_v_double(stevilo_dnevnic_8_12).toDouble() +
																	 pretvori_v_double(cena_dnevnice_12_24).toDouble() * pretvori_v_double(stevilo_dnevnic_12_24).toDouble(), 'f', 2).replace(".", ",");
		besedilo = cena_dnevnic;
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta vodoravna 6
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (prevozni stroski)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 * 2, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo (št. km)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = razdalja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (km x )
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = kilometrina;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (EUR)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

		// nova vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (relacija)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = relacija;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nova vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo (registracija)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_registrska_stevilka;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(pretvori_v_double(kilometrina).toDouble() * pretvori_v_double(razdalja).toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta vodoravna 7
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (drugi stroski)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, polje_2 * 2, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// za vsak strosek dodamo novo vrstico
		if ( ostali_stroski != "" ) {
			do {
				QString strosek = ostali_stroski.left(ostali_stroski.indexOf(";", 0));
				ostali_stroski = ostali_stroski.right(ostali_stroski.length() - ostali_stroski.indexOf(";", 0) - 1);
				QString ime_stroska = strosek.left(strosek.indexOf(",", 0));
				QString vrednost_stroska = strosek.right(strosek.length() - strosek.indexOf(",", 0) - 1).replace(".", ",") + " EUR";
				// nastavimo tip pisave
				painter.setFont(vstavljeno_besedilo);
				// narisemo besedilo
				painter.drawText(polje_2 * 2, pozicija, polje_2 * 6, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, ime_stroska);
				// narisemo besedilo
				painter.drawText(polje_2 * 6, pozicija, polje_2 * 7, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, vrednost_stroska);
				// nastavimo razmik med crto
				pozicija += velikost_besedila.height() + velikost_besedila.height() * faktor_razmika_med_vrsticami_2;
			} while ( ostali_stroski.indexOf(";", 0) != -1 );
			// postavimo pozicijo na prejsnjo vrstico
			pozicija -= velikost_besedila.height() - velikost_besedila.height() * faktor_razmika_med_vrsticami_2;

			// nastavimo besedilo
			besedilo = znesek_drugih_stroskov.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = polje_1 + razmik_med_vrsticami;
			velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// narisemo besedilo
			painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		}

		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta vodoravna 8
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj v znesku)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, polje_1, 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, velikost_besedila.height(), Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnic.replace(",", ".").toDouble() + razdalja.replace(",", ".").toDouble() * kilometrina.replace(",", ".").toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta vodoravna 9
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (ostane za izplacilo)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, velikost_besedila.height(), Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnic.replace(",", ".").toDouble() + razdalja.toDouble() * kilometrina.toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta spodaj
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija));

		// pokoncne crte
		svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, prvotna_visina, 0, pozicija)); // levo
		painter.drawLine(QLine(polje_1, prvotna_visina, polje_1, pozicija)); // sredina
		painter.drawLine(QLine(painter.window().width(), prvotna_visina, painter.window().width(), pozicija)); // desno

		// morebitni preskok na novo stran ( 1 vrstica )
		if ( ( pozicija + visina_glave + visina_noge + velikost_besedila.height() + razmik_med_vrsticami ) >= painter.window().height() ) { // prelom na novo stran
			printer.newPage();

			// natisnemo novo glavo in nogo
			visina_glave = natisni_glavo_potni_nalog(painter, id);
			visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
			pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
		}

		// nastavimo nov razmik
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Priloge)
		besedilo = potni_nalog.readLine() + ": ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		if ( priloge != "" ) {
			besedilo = priloge;
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// narisemo besedilo
			painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		}
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// morebitni preskok na novo stran ( 14 vrstic )
		if ( ( pozicija + visina_glave + visina_noge + velikost_besedila.height() * 14 + razmik_med_vrsticami * 14 ) >= painter.window().height() ) { // prelom na novo stran
			printer.newPage();

			// natisnemo novo glavo in nogo
			visina_glave = natisni_glavo_potni_nalog(painter, id);
			visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
			pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
		}

		// nastavimo besedilo (Opomba)
		besedilo = potni_nalog.readLine() + ": ";

		if ( ui->cb_opombe->isChecked() && opombe != "" ) {

			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = 0;
			velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// narisemo besedilo
			painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

			if ( opombe != "" ) {
				besedilo = opombe;
				// nastavimo tip pisave
				painter.setFont(vstavljeno_besedilo);
				// nastavimo polozaj na listu, kjer zapisemo besedilo
				velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
				// narisemo besedilo
				painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			}
			pozicija += velikost_besedila.height() + 2 * razmik_med_vrsticami;

		}

		// morebitni preskok na novo stran ( 14 vrstic )
		besedilo = "Hello World!";
		velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		if ( ( pozicija + visina_noge + velikost_besedila.height() * 14 + razmik_med_vrsticami * 14 ) >= painter.window().height() ) { // prelom na novo stran
			printer.newPage();

			// natisnemo novo glavo in nogo
			visina_glave = natisni_glavo_potni_nalog(painter, id);
			visina_noge = natisni_nogo_potni_nalog(painter, stevilka_strani);
			pozicija = visina_glave + velikost_besedila.height() / 2 + razmik_med_vrsticami;;
		}

		// zapomnimo si zgornjo visino
		prvotna_visina = pozicija;

		// leva tretjina
		// nastavimo besedilo (V)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_posta + " ";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (dne)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Likvidator)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, painter.window().width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Blagajnik)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width() / 3, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// napisemo tretjo tretjino
		pozicija = prvotna_visina;

		// prazna vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(painter.window().width() *2 / 3, pozicija, painter.window().width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Predlagatelj racuna)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(painter.window().width() * 2 / 3, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta zgoraj

		painter.setPen(*svincnik);
		painter.drawLine(QLine(painter.window().width() *2 / 3, pozicija, painter.window().width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odredbodajalec)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(painter.window().width() * 2 / 3, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// nastavimo odmik od crte
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(painter.window().width() *2 / 3, pozicija, painter.window().width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Prejemnik)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(painter.window().width() * 2 / 3, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += velikost_besedila.height() + razmik_med_vrsticami;

		painter.end();
}

int tiskanje::natisni_glavo_potni_nalog(QPainter &painter, QString id) {

	// pridobimo podatke, pomembne za vnos v glavo
	QString stevilka_naloga = "";
	QString datum_naloga = "";
	QString predlagatelj_podjetje_polno_ime = "";
	QString besedilo = "";

	QFont vstavljeno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_vecja->text().toInt());
	QFont stalno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_vecja->text().toInt(), QFont::Bold);
	QFont naslovno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_naslova->text().toInt(), QFont::Bold);

	double pozicija = 0.0;
	double sirina_besedila = 0.0;
	double faktor_razmika_med_vrsticami_1 = pretvori_v_double(ui->txt_presledek_med_vrsticami_tabela->text()).toDouble(); // pri tabelah

	// odpri podatke o potnem nalogu, prejemniku, stroskih...

	/**
		* na zacetku zberemo podatke o potnem nalogu, izdajatelju potnega naloga
		* (podjetje, oseba), prejemniku potnega naloga ter opravljeni poti.
		**/

		// podatki o potnem nalogu
		QSqlQuery sql_potni_nalog;
		sql_potni_nalog.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + pretvori(id) + "'");
		sql_potni_nalog.exec();
		if ( sql_potni_nalog.next() ) {
			stevilka_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString());
			datum_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("datum_naloga")).toString());
			predlagatelj_podjetje_polno_ime = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje_dolgi")).toString());
		}

	// pridobimo staticno besedilo potnega naloga
	QString app_path = QApplication::applicationDirPath();
	QFile datoteka1(app_path + "/potni-nalog-besedilo-glava.csv");
	if (!datoteka1.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return 0;
	}
	QTextStream potni_nalog(&datoteka1);
    potni_nalog.setCodec("UTF-8");

	// ponastavimo podatke painter-ja
	painter.save();
	painter.resetTransform();

	// narisemo glavo
	QPen *svincnik = new QPen;

	// nastavitev pisave
	painter.setFont(stalno_besedilo);
	QRect velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Lorem Ipsum");
	double razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsicami, za lazje branje dokumenta

	// crta zgoraj
	svincnik->setWidth(pretvori_v_double(ui->txt_crta_debela->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(0, 0, painter.window().width(), 0));

	// nastavimo odmik od crte
	pozicija += razmik_med_vrsticami;

	// nastavimo besedilo ( (podjetje) )
	besedilo = potni_nalog.readLine() + " ";
	// nastavimo tip pisave
	painter.setFont(stalno_besedilo);
	// narisemo besedilo
	painter.drawText(razmik_med_vrsticami, pozicija, painter.window().width() / 2, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// nastavimo besedilo
	besedilo = predlagatelj_podjetje_polno_ime;
	// nastavimo tip pisave
	painter.setFont(vstavljeno_besedilo);
	// narisemo besedilo
	painter.drawText(razmik_med_vrsticami, pozicija, painter.window().width() / 2, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// nastavimo odmik od crte
	pozicija = razmik_med_vrsticami;

	// nastavimo besedilo (Nalog za sluzbeno potovanje)
	besedilo = potni_nalog.readLine() + " ";
	// nastavimo tip pisave
	painter.setFont(naslovno_besedilo);
	// dolocimo velikost besedila
	velikost_besedila = painter.boundingRect(razmik_med_vrsticami + painter.window().width() / 2, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(razmik_med_vrsticami + painter.window().width() / 2, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// crta pod naslovom naloga
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(painter.window().width() / 2, pozicija, painter.window().width(), pozicija));

	// nastavimo odmik od crte
	pozicija += razmik_med_vrsticami;

	// nastavimo besedilo (st., datum:)
	besedilo = potni_nalog.readLine() + " ";
	// nastavimo tip pisave
	painter.setFont(stalno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	sirina_besedila = razmik_med_vrsticami + painter.window().width() / 2;
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
	// nastavimo besedilo
	besedilo = stevilka_naloga;
	// nastavimo tip pisave
	painter.setFont(vstavljeno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
	// nastavimo besedilo
	besedilo = potni_nalog.readLine() + " ";
	// nastavimo tip pisave
	painter.setFont(stalno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
	// nastavimo besedilo
	besedilo = datum_naloga;
	// nastavimo tip pisave
	painter.setFont(vstavljeno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// dodamo okvir besedila
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(painter.window().width() / 2, 0, painter.window().width() / 2, pozicija)); // crta sredina
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(0, 0, 0, pozicija)); // crta levo
	painter.drawLine(QLine(painter.window().width(), 0, painter.window().width(), pozicija)); // crta desno
	painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija)); // crta spodaj

	// vrnemo zacetne nastavitve painter-ja
	painter.restore();

	// vrnemo visino glave
	return pozicija;

}

int tiskanje::natisni_nogo_potni_nalog(QPainter &painter, int &stevilka_strani) {

	QString besedilo = "Stran: " + QString::number(stevilka_strani, 10);
	int visina = 0;

	// ponastavimo podatke painter-ja
	painter.save();
	painter.resetTransform();

	// dodamo parametre strani
	QFont stalno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_vecja->text().toInt());

	double faktor_razmika_med_vrsticami_1 = pretvori_v_double(ui->txt_presledek_med_vrsticami_tabela->text()).toDouble(); // pri tabelah

	// nastavitev pisave
	painter.setFont(stalno_besedilo);
	QRect velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Lorem Ipsum");
	double razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsicami, za lazje branje dokumenta

	// narisemo nogo
	QPen *svincnik = new QPen;
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
	painter.setPen(*svincnik);

	// izracunamo visinski odmik od konca papirja
	velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
	// crta zgoraj
	painter.setFont(stalno_besedilo);
	painter.drawLine(0,
									 painter.window().height() - velikost_besedila.height() - razmik_med_vrsticami,
									 painter.window().width(),
									 painter.window().height() - velikost_besedila.height() - razmik_med_vrsticami);
	visina += razmik_med_vrsticami;
	painter.drawText(0, painter.window().height() - velikost_besedila.height(), painter.window().width(), painter.window().height(), Qt::AlignRight | Qt::TextWordWrap, besedilo);
	visina += velikost_besedila.height();

	// povecamo stevilo strani za 1
	stevilka_strani++;

	// vrnemo zacetne nastavitve painter-ja
	painter.restore();

	// vrnemo visino noge
	return visina;

}

void tiskanje::natisni_prejeti_racun(QString id) {

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
	QString placnik_logotip = "";

	// napolnimo spremenljivke z besedilom
//	stevilka_vnosa = "Stevilka vnosa: ";
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

	QString mapa_za_shranjevanje = "";

	// odpremo bazo in napolnimo spremenljivke

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

			placnik += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("placnik_podjetje_polni")).toString());

			QSqlQuery sql_placnik;
			sql_placnik.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("placnik_podjetje_id")).toString() + "'");
			sql_placnik.exec();
			if ( sql_placnik.next() ) {
				placnik_logotip = prevedi(sql_placnik.value(sql_placnik.record().indexOf("logotip")).toString());
			}

			QSqlQuery sql_projekt;
			sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_projekta")).toString() + "'");
			sql_projekt.exec();
			if ( sql_projekt.next() ) {
				projekt += prevedi(sql_projekt.value(sql_projekt.record().indexOf("stevilka_projekta")).toString());
			}

		}

		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM nastavitve WHERE naziv LIKE '" + pretvori("pot") + "'");
		sql_pot.exec();
		if ( sql_pot.next() ) {
			mapa_za_shranjevanje = prevedi(sql_pot.value(sql_pot.record().indexOf("vrednost")).toString());
		}

	// ustvariti pot do ustrezne mape
	QDir mapa(mapa_za_shranjevanje);
	mapa.mkdir("prejeti-racuni");
	mapa.cd("prejeti-racuni");
	mapa.mkdir(datum_prejema.right(4));
	mapa.cd(datum_prejema.right(4));

	// podatki o printerju
	QPrinter printer;
	if ( ui->txt_format_tiskanja->text() == "pdf" ) {
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(mapa.path() + "/prejeti-racun-" + datum_prejema.right(4) + "-" + stevilka_vnosa.right(11) + ".pdf");
	}

	if ( ui->txt_velikost_papirja->currentText() == "A4" ) {
		printer.setPaperSize(QPrinter::A4);
	}
	else if ( ui->txt_velikost_papirja->currentText() == "A5" ) {
		printer.setPaperSize(QPrinter::A5);
	}

	if ( ui->txt_lega_lista->currentText() == "Lezece" ) {
		printer.setOrientation(QPrinter::Landscape);
	}
	else if ( ui->txt_lega_lista->currentText() == "Pokoncno" ) {
		printer.setOrientation(QPrinter::Portrait);
	}

	if ( ui->txt_uporaba_barv->currentText() == "Barvno" ) {
		printer.setColorMode(QPrinter::Color);
	}
	else if ( ui->txt_uporaba_barv->currentText() == "Crnobelo" ) {
		printer.setColorMode(QPrinter::GrayScale);
	}

	if ( ui->txt_duplex->currentText() == "DA" ) {
		printer.setDuplex(QPrinter::DuplexLongSide);
	}
	else if ( ui->txt_duplex->currentText() == "NE" ) {
		printer.setDuplex(QPrinter::DuplexNone);
	}
	else {
		printer.setDuplex(QPrinter::DuplexAuto);
	}

	printer.setCopyCount(pretvori_v_int(ui->txt_stevilo_kopij->text()).toInt());

	printer.setPageMargins(pretvori_v_double(ui->txt_rob_lista_levo->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_zgoraj->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_desno->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_spodaj->text()).toDouble(),
												 QPrinter::Millimeter);

	QPainter painter;

	if (! painter.begin(&printer))  { // failed to open file
		qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
		return;
	}

	double pozicija = 0;
//	double sirina_besedila = 0;
//	double razmik_med_vrsticami = 0;
//	double faktor_razmika_med_vrsticami_1 = pretvori_v_double(ui->txt_presledek_med_vrsticami_tabela->text()).toDouble(); // pri tabelah
//	double faktor_razmika_med_vrsticami_2 = pretvori_v_double(ui->txt_presledek_med_vrsticami_besedilo->text()).toDouble(); // pri besedilu

	int stevilka_strani = 1;
	int visina_glave = 0;
	int visina_noge = 0;

//	QPen *svincnik = new QPen;

//	QString besedilo = "";

	QFont vstavljeno_besedilo(ui->txt_druzina_pisave->currentText(), pretvori_v_double(ui->txt_velikost_vecja->text()).toDouble());
//	QFont stalno_besedilo(ui->txt_druzina_pisave->currentText(), pretvori_v_double(ui->txt_velikost_vecja->text()).toDouble(), QFont::Bold);
//	QFont naslovno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_naslova->text().toInt(), QFont::Bold);

	painter.setFont(vstavljeno_besedilo);

	// narisi glavo
	visina_glave = natisni_glavo_prejeti_racun(painter, id);
	visina_noge = natisni_nogo_prejeti_racun(painter, stevilka_strani);

	// pripravimo dokument za tiskanje
	QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
	pozicija += visina_glave + velikost_besedila.height();
//	painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
//	pozicija += velikost_besedila.height();
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

int tiskanje::natisni_glavo_prejeti_racun(QPainter &painter, QString id) {

	// pridobimo podatke, pomembne za vnos v glavo
	QString stevilka_naloga = "";
	QString datum_naloga = "";
	QString predlagatelj_podjetje_polno_ime = "";
	QString besedilo = "";

	QFont vstavljeno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_vecja->text().toInt());
	QFont stalno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_vecja->text().toInt(), QFont::Bold);
	QFont naslovno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_naslova->text().toInt(), QFont::Bold);

	double pozicija = 0.0;
	double sirina_besedila = 0.0;
	double faktor_razmika_med_vrsticami_1 = pretvori_v_double(ui->txt_presledek_med_vrsticami_tabela->text()).toDouble(); // pri tabelah

	// odpri podatke o potnem nalogu, prejemniku, stroskih...

	/**
		* na zacetku zberemo podatke o potnem nalogu, izdajatelju potnega naloga
		* (podjetje, oseba), prejemniku potnega naloga ter opravljeni poti.
		**/

		// podatki o potnem nalogu
		QSqlQuery sql_potni_nalog;
		sql_potni_nalog.prepare("SELECT * FROM prejeti_racuni WHERE id LIKE '" + pretvori(id) + "'");
		sql_potni_nalog.exec();
		if ( sql_potni_nalog.next() ) {
			stevilka_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_vnosa")).toString());
			datum_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("datum_prejema")).toString());
			predlagatelj_podjetje_polno_ime = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("placnik_podjetje_polni")).toString());

		}

	// pridobimo staticno besedilo potnega naloga
	QString app_path = QApplication::applicationDirPath();
	QFile datoteka1(app_path + "/potni-nalog-besedilo-glava.csv");
	if (!datoteka1.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return 0;
	}
    QTextStream potni_nalog(&datoteka1);
    potni_nalog.setCodec("UTF-8");

	// ponastavimo podatke painter-ja
	painter.save();
	painter.resetTransform();

	// narisemo glavo
	QPen *svincnik = new QPen;

	// nastavitev pisave
	painter.setFont(stalno_besedilo);
	QRect velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Lorem Ipsum");
	double razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsicami, za lazje branje dokumenta

	// crta zgoraj
	svincnik->setWidth(pretvori_v_double(ui->txt_crta_debela->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(0, 0, painter.window().width(), 0));

	// nastavimo odmik od crte
	pozicija += razmik_med_vrsticami;

	// nastavimo besedilo ( (podjetje) )
	besedilo = potni_nalog.readLine() + " ";
	// nastavimo tip pisave
	painter.setFont(stalno_besedilo);
	// narisemo besedilo
	painter.drawText(razmik_med_vrsticami, pozicija, painter.window().width() / 2, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// nastavimo besedilo
	besedilo = predlagatelj_podjetje_polno_ime;
	// nastavimo tip pisave
	painter.setFont(vstavljeno_besedilo);
	// narisemo besedilo
	painter.drawText(razmik_med_vrsticami, pozicija, painter.window().width() / 2, velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// nastavimo odmik od crte
	pozicija = razmik_med_vrsticami;

	// nastavimo besedilo (Prejeti racun)
	besedilo = potni_nalog.readLine() + " ";
	besedilo = "Prejeti racun";
	// nastavimo tip pisave
	painter.setFont(naslovno_besedilo);
	// dolocimo velikost besedila
	velikost_besedila = painter.boundingRect(razmik_med_vrsticami + painter.window().width() / 2, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(razmik_med_vrsticami + painter.window().width() / 2, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// crta pod naslovom naloga
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(painter.window().width() / 2, pozicija, painter.window().width(), pozicija));

	// nastavimo odmik od crte
	pozicija += razmik_med_vrsticami;

	// nastavimo besedilo (st., datum:)
	besedilo = potni_nalog.readLine() + " ";
	// nastavimo tip pisave
	painter.setFont(stalno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	sirina_besedila = razmik_med_vrsticami + painter.window().width() / 2;
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
	// nastavimo besedilo
	besedilo = stevilka_naloga;
	// nastavimo tip pisave
	painter.setFont(vstavljeno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
	// nastavimo besedilo
	besedilo = potni_nalog.readLine() + " ";
	// nastavimo tip pisave
	painter.setFont(stalno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
	// nastavimo besedilo
	besedilo = datum_naloga;
	// nastavimo tip pisave
	painter.setFont(vstavljeno_besedilo);
	// nastavimo polozaj na listu, kjer zapisemo besedilo
	velikost_besedila = painter.boundingRect(sirina_besedila, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// narisemo besedilo
	painter.drawText(sirina_besedila, pozicija, painter.window().width(), velikost_besedila.height(), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo besedila
	pozicija += velikost_besedila.height() + razmik_med_vrsticami;

	// dodamo okvir besedila
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(painter.window().width() / 2, 0, painter.window().width() / 2, pozicija)); // crta sredina
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
	painter.setPen(*svincnik);
	painter.drawLine(QLine(0, 0, 0, pozicija)); // crta levo
	painter.drawLine(QLine(painter.window().width(), 0, painter.window().width(), pozicija)); // crta desno
	painter.drawLine(QLine(0, pozicija, painter.window().width(), pozicija)); // crta spodaj

	// vrnemo zacetne nastavitve painter-ja
	painter.restore();

	// vrnemo visino glave
	return pozicija;

}

int tiskanje::natisni_nogo_prejeti_racun(QPainter &painter, int &stevilka_strani) {

	QString besedilo = "Stran: " + QString::number(stevilka_strani, 10);
	int visina = 0;

	// ponastavimo podatke painter-ja
	painter.save();
	painter.resetTransform();

	// dodamo parametre strani
	QFont stalno_besedilo(ui->txt_druzina_pisave->currentText(), ui->txt_velikost_vecja->text().toInt());

	double faktor_razmika_med_vrsticami_1 = pretvori_v_double(ui->txt_presledek_med_vrsticami_tabela->text()).toDouble(); // pri tabelah

	// nastavitev pisave
	painter.setFont(stalno_besedilo);
	QRect velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Lorem Ipsum");
	double razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsicami, za lazje branje dokumenta

	// narisemo nogo
	QPen *svincnik = new QPen;
	svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());
	painter.setPen(*svincnik);

	// izracunamo visinski odmik od konca papirja
	velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
	// crta zgoraj
	painter.setFont(stalno_besedilo);
	painter.drawLine(0,
									 painter.window().height() - velikost_besedila.height() - razmik_med_vrsticami,
									 painter.window().width(),
									 painter.window().height() - velikost_besedila.height() - razmik_med_vrsticami);
	visina += razmik_med_vrsticami;
	painter.drawText(0, painter.window().height() - velikost_besedila.height(), painter.window().width(), painter.window().height(), Qt::AlignRight | Qt::TextWordWrap, besedilo);
	visina += velikost_besedila.height();

	// povecamo stevilo strani za 1
	stevilka_strani++;

	// vrnemo zacetne nastavitve painter-ja
	painter.restore();

	// vrnemo visino noge
	return visina;

}

void tiskanje::natisni_izdani_racun(QString id) {

	/**
		* Izberemo spremenljivke in jim iz baze dodamo ustrezne vrednosti. Pri
		* tem ne smemo pozabiti na šifrantne tabele. Spremenljivke nato vstavimo
		* v formo za tisk in natisnemo.
		**/

	// priprava spremenljivk
	QString podjetje_logo = "";
	QString podjetje_kratki = "";
	QString podjetje_polni = "";
	QString podjetje_oseba = "";
	QString podjetje_oseba_naziv = "";
	QString podjetje_url = "";
	QString podjetje_email = "";
	QString podjetje_ddv = "";
	QString podjetje_naslov_ulica = "";
	QString podjetje_naslov_stevilka = "";
	QString podjetje_naslov_postna_stevilka = "";
	QString podjetje_naslov_posta = "";
	QString podjetje_trr = "";
	QString podjetje_telefon = "";
	QString podjetje_banka = "";
	QString podjetje_bic = "";
	QString podjetje_koda_namena = "";

	QString narocnik_naziv = "";
	QString narocnik_naslov = "";
	QString narocnik_posta = "";
	QString narocnik_davcna = "";
	QString narocnik_banka = "";
	QString narocnik_bic = "";
	QString narocnik_trr = "";

	QString racun_tip = "";
	QString racun_stevilka = "";
	QString racun_datum_izdaje = "";
	QString racun_datum_zacetka  = "";
	QString racun_rok_izvedbe = "";
	QString racun_avans = "";
	QString racun_znesek_avansa = "";
	QString racun_rok_placila = "";
	QString racun_datum_placila_avansa = "";
	QString racun_stevilka_sklica = "";
	QString racun_opombe = "";
	QString racun_id_starsa = "";
	QString racun_stevilka_starsa = "";
	QString razlog_stornacije = "";
	QString racun_avtor = "";

	QString storitev_id = "";
	QString storitev_ime = "";
	QString storitev_sifra = "";
	QString storitev_kolicina = "";
	QString storitev_cena_brez_ddv_na_enoto = "";
	QString storitev_popust = "";
	QString storitev_odstotek_ddv = "";
	QString storitev_cena_brez_ddv = "";
	QString storitev_enota = "";

	QString skupaj_brez_ddv = "";
	QString skupaj_ddv_od_osnove_20 = "";
	QString skupaj_ddv_od_osnove_85 = "";
	QString skupaj_ddv_od_osnove_00 = "";
	QString skupaj_znesek = "";
	QString skupaj_znesek_avansa = "";
	QString skupaj_znesek_avansa_brez_ddv = "";
	QString skupaj_ddv_avansa = "";
	QString skupaj_se_za_placati = "";
	QString skupaj_se_za_placati_ddv = "";

	QStringList seznam_ddv;
	QList<double> znesek_ddv;

	double skupajbrezddv = 0.0;
	double skupajddvodosnove20 = 0.0;
	double skupajznesek = 0.0;
	double skupajznesekavansabrezddv = 0.0;
	double skupajddvavansa = 0.0;

	int stevilka_strani = 1;

	QString mapa_za_shranjevanje = "";

	// priprava baze in polnenje spremenljivk (razen storitve)

		// napolni vrednosti racuna
		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
		sql_racun.exec();
		if ( sql_racun.next() ) {
			racun_avtor = prevedi(sql_racun.value(sql_racun.record().indexOf("avtor_oseba")).toString());
			racun_tip = prevedi(sql_racun.value(sql_racun.record().indexOf("tip_racuna")).toString());
			racun_stevilka = prevedi(sql_racun.value(sql_racun.record().indexOf("stara_stevilka_racuna")).toString());
			if ( racun_stevilka == "" ) {
				racun_stevilka = prevedi(sql_racun.value(sql_racun.record().indexOf("stevilka_racuna")).toString());
			}
			racun_datum_izdaje = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_izdaje")).toString());
			racun_datum_zacetka = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_pricetka")).toString());
			racun_rok_izvedbe = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_konca")).toString());
			racun_avans = prevedi(sql_racun.value(sql_racun.record().indexOf("odstotek_avansa")).toString());
			racun_znesek_avansa = prevedi(sql_racun.value(sql_racun.record().indexOf("avans")).toString());
			racun_rok_placila = prevedi(sql_racun.value(sql_racun.record().indexOf("rok_placila")).toString());
			if ( sql_racun.value(sql_racun.record().indexOf("stevilka_starsa")).toString() == "" ) {
				racun_datum_placila_avansa = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_placila_avansa")).toString());
			}
			else {
				QSqlQuery sql_stars;
				sql_stars.prepare("SELECT * FROM racuni WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stevilka_starsa")).toString() + "'");
				sql_stars.exec();
				if ( sql_stars.next() ) {
					racun_datum_placila_avansa = prevedi(sql_stars.value(sql_stars.record().indexOf("datum_placila_avansa")).toString());
				}
			}
			racun_stevilka_sklica = prevedi(sql_racun.value(sql_racun.record().indexOf("sklic")).toString());
			racun_opombe = prevedi(sql_racun.value(sql_racun.record().indexOf("opombe")).toString());
			razlog_stornacije = prevedi(sql_racun.value(sql_racun.record().indexOf("razlog_stornacije")).toString());

			podjetje_oseba = prevedi(sql_racun.value(sql_racun.record().indexOf("avtor_oseba")).toString());
//            narocnik = prevedi(sql_racun.value(sql_racun.record().indexOf("stranka")).toString());
			racun_id_starsa = prevedi(sql_racun.value(sql_racun.record().indexOf("stevilka_starsa")).toString());

			// napolni podatke o podjetju
			podjetje_logo = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_logotip")).toString());
			podjetje_kratki = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_kratki")).toString());
			podjetje_polni = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_polni")).toString());
			podjetje_url = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_url")).toString());
			podjetje_email = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_email")).toString());
			podjetje_telefon = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_telefon")).toString());
			podjetje_ddv = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_ddv")).toString());
			podjetje_naslov_ulica = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_naslov_ulica")).toString());
			podjetje_naslov_stevilka = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_naslov_stevilka")).toString());
			podjetje_naslov_postna_stevilka = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_naslov_postna_stevilka")).toString());
			podjetje_naslov_posta = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_naslov_posta")).toString());
			podjetje_trr = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_tekoci_racun")).toString());
			podjetje_banka = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_banka")).toString());
			podjetje_bic = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_bic")).toString());
			podjetje_koda_namena = prevedi(sql_racun.value(sql_racun.record().indexOf("podjetje_koda_namena")).toString());
			// vstavi presledke med vsake stiri znake pri trr-ju
			podjetje_trr.insert(4, " ");
			podjetje_trr.insert(9, " ");
			podjetje_trr.insert(14, " ");
			podjetje_trr.insert(19, " ");

			// napolni podatke o avtorju/podpisniku/izdajatelju racuna
			podjetje_oseba = prevedi(sql_racun.value(sql_racun.record().indexOf("izdajatelj_priimek")).toString()) + " " +
											 prevedi(sql_racun.value(sql_racun.record().indexOf("izdajatelj_ime")).toString());
			podjetje_oseba_naziv = prevedi(sql_racun.value(sql_racun.record().indexOf("izdajatelj_naziv")).toString());
			// napolni vrednosti avtorjev naziv / ce ne ustreza, ga pusti pri miru
			QSqlQuery sql_naziv;
			sql_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + pretvori(podjetje_oseba_naziv) + "'");
			sql_naziv.exec();
			if ( sql_naziv.next() ) {
				QSqlQuery sql_spol;
				sql_spol.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + pretvori(racun_avtor) + "'");
				sql_spol.exec();
				if ( sql_spol.next() ) {
					if ( prevedi(sql_spol.value(sql_spol.record().indexOf("spol")).toString()) == "2" ) {
						podjetje_oseba_naziv = prevedi(sql_naziv.value(sql_naziv.record().indexOf("naziv_zenski")).toString());
					}
					else {
						podjetje_oseba_naziv = prevedi(sql_naziv.value(sql_naziv.record().indexOf("naziv_moski")).toString());
					}
				}
			}

			// napolni podatke o narocniku
			narocnik_naziv = prevedi(sql_racun.value(sql_racun.record().indexOf("narocnik_naziv")).toString());
			narocnik_naslov = prevedi(sql_racun.value(sql_racun.record().indexOf("narocnik_naslov")).toString());
			narocnik_posta = prevedi(sql_racun.value(sql_racun.record().indexOf("narocnik_posta")).toString());
			narocnik_davcna = prevedi(sql_racun.value(sql_racun.record().indexOf("narocnik_davcna")).toString());

			QSqlQuery sql_stranka;
			sql_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("narocnik_id")).toString() + "'");
			sql_stranka.exec();
			if ( sql_stranka.next() ) {
				narocnik_banka = prevedi(sql_stranka.value(sql_stranka.record().indexOf("banka")).toString());
				narocnik_bic = prevedi(sql_stranka.value(sql_stranka.record().indexOf("bic_banke")).toString());
				narocnik_trr = prevedi(sql_stranka.value(sql_stranka.record().indexOf("trr")).toString());
			}
		}
		if ( racun_id_starsa != "" ) {
			QSqlQuery sql_stars;
			sql_stars.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(racun_id_starsa) + "'");
			sql_stars.exec();
			if ( sql_stars.next() ) {
				racun_stevilka_starsa = prevedi(sql_stars.value(sql_stars.record().indexOf("stevilka_racuna")).toString());
			}
		}

		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM nastavitve WHERE naziv LIKE '" + pretvori("pot") + "'");
		sql_pot.exec();
		if ( sql_pot.next() ) {
			mapa_za_shranjevanje = prevedi(sql_pot.value(sql_pot.record().indexOf("vrednost")).toString());
		}

	/**
		* Odpremo sifrant z besedilom potnega naloga
		* Besedilo bomo vztavljali z readLine
		**/

	QString app_path = QApplication::applicationDirPath();
	QFile datoteka;
	if ( racun_tip == "1" ) {
		datoteka.setFileName(app_path + "/racun-besedilo-predracun.csv");
	}
	else if ( racun_tip == "2" ) {
		datoteka.setFileName(app_path + "/racun-besedilo-predplacilo.csv");
	}
	else if ( racun_tip == "3" ) {
		datoteka.setFileName(app_path + "/racun-besedilo-racun.csv");
	}
	else if ( racun_tip == "4" ) {
		datoteka.setFileName(app_path + "/racun-besedilo-dobropis.csv");
	}

	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	QTextStream racun(&datoteka);
    racun.setCodec("UTF-8");

	// tiskanje
	QString besedilo = "";
	QString besedilo_noga_ddv = racun.readLine();
	QString besedilo_noga_maticna = racun.readLine ();

	// ustvariti pot do ustrezne mape
	QDir mapa(mapa_za_shranjevanje);
	mapa.mkdir("izdani-racuni");
	mapa.cd("izdani-racuni");
	mapa.mkdir(racun_datum_izdaje.right(4));
	mapa.cd(racun_datum_izdaje.right(4));

	QString ime_datoteke = "";

	if ( racun_tip == "1" ) {
		mapa.mkdir("01-predracuni");
		mapa.cd("01-predracuni");
		ime_datoteke = "izdani-predracun";
	}
	else if ( racun_tip == "2" ) {
		mapa.mkdir("02-predplacilni-racuni");
		mapa.cd("02-predplacilni-racuni");
		ime_datoteke = "izdani-predplacilni-racun";
	}
	else if ( racun_tip == "3" ) {
		mapa.mkdir("03-racuni");
		mapa.cd("03-racuni");
		ime_datoteke = "izdani-racun";
	}
	else if ( racun_tip == "4" ) {
		mapa.mkdir("04-dobropisi");
		mapa.cd("04-dobropisi");
		ime_datoteke = "izdani-dobropisi";
	}

	// podatki o printerju
	QPrinter printer;
	if ( ui->txt_format_tiskanja->text() == "pdf" ) {
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(mapa.path() + "/" + ime_datoteke + "-" + racun_datum_izdaje.right(4) + "-" + racun_stevilka.right(11) + ".pdf");
	}

	if ( ui->txt_velikost_papirja->currentText() == "A4" ) {
		printer.setPaperSize(QPrinter::A4);
	}
	else if ( ui->txt_velikost_papirja->currentText() == "A5" ) {
		printer.setPaperSize(QPrinter::A5);
	}

	if ( ui->txt_lega_lista->currentText() == "Lezece" ) {
		printer.setOrientation(QPrinter::Landscape);
	}
	else if ( ui->txt_lega_lista->currentText() == "Pokoncno" ) {
		printer.setOrientation(QPrinter::Portrait);
	}

	if ( ui->txt_uporaba_barv->currentText() == "Barvno" ) {
		printer.setColorMode(QPrinter::Color);
	}
	else if ( ui->txt_uporaba_barv->currentText() == "Crnobelo" ) {
		printer.setColorMode(QPrinter::GrayScale);
	}

	if ( ui->txt_duplex->currentText() == "DA" ) {
		printer.setDuplex(QPrinter::DuplexLongSide);
	}
	else if ( ui->txt_duplex->currentText() == "NE" ) {
		printer.setDuplex(QPrinter::DuplexNone);
	}
	else {
		printer.setDuplex(QPrinter::DuplexAuto);
	}

	printer.setCopyCount(pretvori_v_int(ui->txt_stevilo_kopij->text()).toInt());

	printer.setPageMargins(pretvori_v_double(ui->txt_rob_lista_levo->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_zgoraj->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_desno->text()).toDouble(),
												 pretvori_v_double(ui->txt_rob_lista_spodaj->text()).toDouble(),
												 QPrinter::Millimeter);

	QPainter painter;

	if (! painter.begin(&printer))  { // failed to open file
		qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
		return;
	}

		int i = 1;
		double pozicija = 0;
		double visina_vrstice = 0;
		double razmik_med_vrsticami = 0;
		double faktor_razmika_med_vrsticami_1 = 0.1; // pri tabelah
		double faktor_razmika_med_vrsticami_2 = 0.05; // pri besedilu

		// nastravitve
		QFont normalno("Arial", 10);
		QFont debelo("Arial", 10, QFont::Bold);
		QFont veliko("Arial", 14, QFont::Bold);
		QFont malo("Arial", 8);
		QFont malo_debelo("Arial", 8, QFont::Bold);

		QPen *debel_svincnik = new QPen;
		QPen *tanek_svincnik = new QPen;
		QPen *rdec_svincnik = new QPen;
		QPen *crn_svincnik = new QPen;
		debel_svincnik->setWidth(2.0);
		tanek_svincnik->setWidth(1.0);
		rdec_svincnik->setColor(Qt::red);
		crn_svincnik->setColor(Qt::black);

		// narisemo glavo in nogo prve strani
		int visina_glave = natisni_glavo_izdani_racun(painter, id);
		int visina_noge = natisni_nogo_izdani_racun(painter, id, stevilka_strani, besedilo_noga_ddv, besedilo_noga_maticna);

		pozicija = visina_glave;

	// podatki o (pred)racunu
		// narocnik
		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine();
		double pozicija_zgoraj = pozicija;
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Narocnik: ")
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nova vrstica
		pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

		// nastavi parametre
		painter.setFont(normalno);
		// dolocimo velikost kvadrata, ki ga tvori besedilo (ime/naziv)
		velikost_besedila = painter.boundingRect(0, 0, ( printer.width() / 2 ) - 20, 0, Qt::AlignJustify | Qt::TextWordWrap, narocnik_naziv);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, ( printer.width() / 2 ) - 20 , visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_naziv);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo velikost kvadrata, ki ga tvori besedilo (naslov - ulica)
		velikost_besedila = painter.boundingRect(0, 0, ( printer.width() / 2 ) - 20, 0, Qt::AlignJustify | Qt::TextWordWrap, narocnik_naslov);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, ( printer.width() / 2 ) - 20, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_naslov);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo velikost kvadrata, ki ga tvori besedilo (naslov - posta)
		velikost_besedila = painter.boundingRect(0, 0, ( printer.width() / 2 ) - 20, 0, Qt::AlignJustify | Qt::TextWordWrap, narocnik_posta);
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, ( printer.width() / 2 ) - 20, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_posta);
		// nova vrstica
		pozicija += visina_vrstice * 6 + razmik_med_vrsticami * 6;

		besedilo = racun.readLine();
		if ( narocnik_davcna != "" ) {
			painter.setFont(debelo);
			// dolocimo velikost kvadrata, ki ga tvori besedilo (davcna stevilka)
			velikost_besedila = painter.boundingRect(0, 0, ( printer.width() / 2 ) - 20, 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo + " ");
			// natisnemo besedilo
			painter.drawText(QRectF(0, pozicija, ( printer.width() / 2 ) - 20, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo + " ");
			painter.setFont(normalno);
			painter.drawText(QRectF(velikost_besedila.width(), pozicija, ( printer.width() / 2) - 20, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_davcna);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}
		double pozicija_spodaj = pozicija; // postavimo višino, ki ustreza crti pod podatki (pred)racuna

	// podatki o (pred)racunu
		pozicija = pozicija_zgoraj;

		// nastavi parametre
		painter.setFont(debelo);

		if ( razlog_stornacije != "" ) {
			// dolocimo velikost kvadrata, ki ga tvori besedilo (STORNIRANO)
			besedilo = "STORNIRANO";
			painter.setFont(veliko);
			painter.setPen(*rdec_svincnik);
			velikost_besedila = painter.boundingRect( ( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsticami, za lazje branje dokumenta
			// natisnemo besedilo
			painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
			painter.setPen(*crn_svincnik);
			painter.setFont(debelo);
		}

		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("(Pred)Racun st.: ")
		velikost_besedila = painter.boundingRect( ( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_stevilka);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		if ( racun_tip == "4" ) { // JE dobropis
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Stevilka racuna: ")
			velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_stevilka_starsa);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Datum izdaje: ")
		velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_izdaje);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Kraj izdaje: ")
		velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_posta);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		if ( racun_tip != "3" && racun_tip != "4" ) { // NI racun, NI dobropis
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Predviden rok izvedbe: ")
			velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_izvedbe);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}
		else if ( racun_tip == "3" ){ // JE racun, NI dobropis
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Pricetek dela: ")
			velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_zacetka);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;

			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Zakljucek dela: ")
			velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_izvedbe);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}

			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			if ( racun_tip == "1" && racun_avans != "0.0" ) {
				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Rok placila (avansa): ")
				velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
				// nastavimo parametre
				visina_vrstice = velikost_besedila.height();
				// natisnemo besedilo
				painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
				// natisnemo besedilo
				painter.setFont(normalno);
				if ( racun_tip != "2" ) {
					painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_placila);
				}
				else {
					painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_placila_avansa);
				}
				// nova vrstica
				pozicija += visina_vrstice + razmik_med_vrsticami;
			}
			if ( racun_tip != "1" && racun_avans != "100.0" ) {
				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Rok placila (avansa): ")
				velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
				// nastavimo parametre
				visina_vrstice = velikost_besedila.height();
				// natisnemo besedilo
				painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
				// natisnemo besedilo
				painter.setFont(normalno);
				if ( racun_tip != "2" ) {
					painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_placila);
				}
				else {
					painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_placila_avansa);
				}
				// nova vrstica
				pozicija += visina_vrstice + razmik_med_vrsticami;
			}

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Nacin placila: ")
		velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		besedilo = racun.readLine();
		painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = "BIC banke: ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("BIC banke: ")
		velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		if ( racun_tip == "4" ) { // JE dobropis
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_bic);
		}
		else {
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_bic);
		}
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("TRR Abanka: ")
		velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		if ( racun_tip == "4" ) { // JE dobropis
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_trr);
		}
		else {
			painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_trr);
		}
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		if ( racun_tip != "2" ) {
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = "Koda namena: ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Koda namena: ")
			velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			if ( racun_tip == "1") { // pri predracunu
				painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_koda_namena);
			}
			else if ( racun_tip == "3" ) { // pri racunu
				painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_koda_namena);
			}
			else if ( racun_tip == "4" ) { // JE dobropis
				painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, "OTHR");
			}
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}

		if ( racun_tip != "2" ) { // samo pri predracunu in racunu
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Sklic: ")
			velikost_besedila = painter.boundingRect(( printer.width() / 2 ) + 20, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(( printer.width() / 2 ) + 20, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			if ( racun_tip == "4" ) { // JE dobropis
				painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_stevilka_sklica);
			}
			else {
				painter.drawText(QRectF(( printer.width() / 2 ) + 20 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_stevilka_sklica);
			}
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}
		pozicija += visina_vrstice + razmik_med_vrsticami;

		if ( pozicija < pozicija_spodaj ) {
			pozicija = pozicija_spodaj + razmik_med_vrsticami;
		}

		// crta pod podatki
		painter.setPen(*debel_svincnik);
		painter.drawLine(0, pozicija, printer.width(), pozicija);

	// storitve
		// glava tabele
		// nastavi parametre
		painter.setPen(*tanek_svincnik);
		painter.setFont(debelo);

		double sirina_vecja = printer.width() / 8;
		double sirina_manjsa = printer.width() / 12;
		double crta_0 = 0;														// Zacetek
		double crta_1 = sirina_manjsa / 2;										// Pozicija
		double crta_2 = crta_1 + sirina_manjsa;									// Sifra
		double crta_3 = printer.width() - sirina_vecja - sirina_manjsa * 5;		// Storitev
		double crta_4 = crta_3 + sirina_manjsa;									// Kolicina
		double crta_5 = crta_4 + sirina_manjsa;									// Enota
		double crta_6 = crta_5 + sirina_vecja;									// Cena na enoto brez DDV
		double crta_7 = crta_6 + sirina_manjsa;									// Popust
		double crta_8 = crta_7 + sirina_manjsa;									// Stopnja DDV

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Sifra")
		velikost_besedila = painter.boundingRect(crta_0, pozicija, sirina_manjsa / 2, pozicija, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height() * 3 + razmik_med_vrsticami;
		// natisnemo besedilo
		painter.drawText(QRectF(crta_0, pozicija, sirina_manjsa / 2, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Sifra")
		velikost_besedila = painter.boundingRect(crta_1, pozicija, sirina_manjsa, pozicija, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height() * 3 + razmik_med_vrsticami;
		// natisnemo besedilo
		painter.drawText(QRectF(crta_1, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Storitev")
		velikost_besedila = painter.boundingRect(crta_2, pozicija, printer.width() - sirina_vecja - sirina_manjsa * 6 - sirina_manjsa / 2, pozicija, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height() * 3 + razmik_med_vrsticami;
		// natisnemo besedilo
		painter.drawText(QRectF(crta_2, pozicija, printer.width() - sirina_vecja - sirina_manjsa * 6 - sirina_manjsa / 2, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Kolicina")
		velikost_besedila = painter.boundingRect(crta_3, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_3, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Enota")
		velikost_besedila = painter.boundingRect(crta_4, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_4, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Cena na enoto brez DDV")
		velikost_besedila = painter.boundingRect(crta_5, pozicija, sirina_vecja, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_5, pozicija, sirina_vecja, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Popust %")
		velikost_besedila = painter.boundingRect(crta_6, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_6, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("DDV %")
		velikost_besedila = painter.boundingRect(crta_7, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_7, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Vrednost brez DDV")
		velikost_besedila = painter.boundingRect(crta_8, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_8, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		// nova pozicija = nova vrstica v tabeli
		pozicija += visina_vrstice;

		// crta pod storitvami
		painter.setPen(*debel_svincnik);
		painter.drawLine(0, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += razmik_med_vrsticami / 2;

		// priprava baze in polnenje storitve

			QSqlQuery sql_ddv;
			sql_ddv.prepare("SELECT * FROM sif_ddv ORDER BY id DESC");
			sql_ddv.exec();
			while ( sql_ddv.next() ) {
				seznam_ddv.append(prevedi(sql_ddv.value(sql_ddv.record().indexOf("vrednost")).toString()));
				znesek_ddv.append(0);
			}

			QSqlQuery sql_storitve;
			int trenutna_pozicija = 0;
			int koncna_pozicija = 0;

			sql_storitve.exec();
			while ( sql_storitve.next() ) {
				koncna_pozicija++;
			}
			sql_storitve.clear();

			if ( racun_tip == "4" ) {
				sql_storitve.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id) + "' AND tip_racuna LIKE '" + pretvori(racun_tip) +
									 "' AND dobropis LIKE '1' ORDER BY vrstni_red ASC");
			}
			else {
				sql_storitve.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id) + "' AND tip_racuna LIKE '" + pretvori(racun_tip) +
									 "' ORDER BY vrstni_red ASC");
			}
			sql_storitve.exec();

			i = 1; // za zaporedno stevilko storitve

			while ( sql_storitve.next() ) {
				storitev_sifra = prevedi(sql_storitve.value(sql_storitve.record().indexOf("sifra")).toString());
				if ( storitev_sifra == "" ) {
					storitev_sifra = "000000";
				}

				storitev_id = QString::number(i, 10) + ".";
				i++;

				storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_storitev")).toString());

				storitev_enota = prevedi(sql_storitve.value(sql_storitve.record().indexOf("enota")).toString());

				storitev_kolicina = prevedi(sql_storitve.value(sql_storitve.record().indexOf("ur_dela")).toString()).replace(".", ",");

				double ddv = pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("ddv")).toString())).toDouble();
				storitev_odstotek_ddv = QString::number(ddv, 'f', 1).replace(".", ",");

				double podrazitev = 0.0;
				double cena_na_enoto_brez_ddv = pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("urna_postavka_brez_ddv")).toString())).toDouble();

				// dodatki in preracuni kot posledica podrazitev
				if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("pribitek_vikend")).toString()) == "1" ) {
					storitev_ime += " - Delo med vikendom";
					podrazitev += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("podrazitev_vikend")).toString())).toDouble();
				}
				if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("pribitek_hitrost")).toString()) == "1" ) {
					storitev_ime += " - Prednostna obdelava";
					podrazitev += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("podrazitev_hitrost")).toString())).toDouble();
				}
				if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("pribitek_zapleti")).toString()) == "1" ) {
					storitev_ime += " - Zahtevna naloga";
					podrazitev += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("podrazitev_zapleti")).toString())).toDouble();
				}
				cena_na_enoto_brez_ddv = cena_na_enoto_brez_ddv * ( 1 + podrazitev / 100 );
				storitev_cena_brez_ddv_na_enoto = QString::number(cena_na_enoto_brez_ddv, 'f', 2).replace(".", ",");

				// popusti
				double popusti = 0.0;
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_fb1")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_fb2")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_komb1")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_komb2")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_stranka")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_akcija")).toString())).toDouble();
				if ( popusti > 100.0 ) {
					popusti = 100.0;
				}
				storitev_popust = QString::number(popusti, 'f', 1).replace(".", ",");

				// ceno storitve brez ddv
				double cena = pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesek_koncni")).toString())).toDouble();
				storitev_cena_brez_ddv = QString::number(cena, 'f', 2).replace(".", ",");

				// izracunaj zdruzke
				skupajbrezddv += cena;
				int zaporedje = seznam_ddv.indexOf(prevedi(sql_storitve.value(sql_storitve.record().indexOf("ddv")).toString()));

				znesek_ddv[zaporedje] += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesek_ddv")).toString())).toDouble();

				// tiskanje storitve
				painter.setFont(malo);
				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Storitev")
				velikost_besedila = painter.boundingRect(crta_2, pozicija, printer.width() - sirina_vecja - sirina_manjsa * 6 - sirina_manjsa / 2, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_ime);
				// nastavimo parametre
				visina_vrstice = velikost_besedila.height() + razmik_med_vrsticami;

				// preveri, ce je potrebna nova stran
				if( pozicija + visina_noge + visina_vrstice >= printer.height() ) {
					printer.newPage();

					int visina_glave = natisni_glavo_izdani_racun(painter, id);

					pozicija = visina_glave;
				}

				// nastavimo majhno pisavo
				painter.setFont(malo);

				// natisnemo besedilo (Pozicija)
				painter.drawText(QRectF(crta_0, pozicija, sirina_manjsa / 2, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_id);

				// natisnemo besedilo (Sifra)
				painter.drawText(QRectF(crta_1, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_sifra);

				// natisnemo besedilo (Storitev)
				painter.drawText(QRectF(crta_2, pozicija, printer.width() - sirina_vecja - sirina_manjsa * 6 - sirina_manjsa / 2, visina_vrstice), Qt::AlignLeft | Qt::TextWordWrap | Qt::AlignVCenter, storitev_ime);

				// natisnemo besedilo ("Kolicina")
				painter.drawText(QRectF(crta_3, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_kolicina);

				// natisnemo besedilo ("Enota")
				painter.drawText(QRectF(crta_4, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_enota);

				// natisnemo besedilo ("Cena na enoto brez DDV")
				painter.drawText(QRectF(crta_5, pozicija, sirina_vecja, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_cena_brez_ddv_na_enoto);

				// natisnemo besedilo ("Popust %")
				painter.drawText(QRectF(crta_6, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_popust);

				// natisnemo besedilo ("DDV %")
				painter.drawText(QRectF(crta_7, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_odstotek_ddv);

				// natisnemo besedilo ("Vrednost brez DDV")
				painter.drawText(QRectF(crta_8, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_cena_brez_ddv);

				// nova pozicija = nova vrstica v tabeli
				pozicija += visina_vrstice + razmik_med_vrsticami / 2;

				trenutna_pozicija++;
				if ( trenutna_pozicija < koncna_pozicija ) {
				// crta pod storitvami
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, pozicija, printer.width(), pozicija);
				}

			} // while ( sql_storitve.next() )

		// crta pod storitvami
		painter.setPen(*debel_svincnik);
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Test), samo, da ponovno nastavimo naso velikost vrstice in ne tiste s tabele
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Test");
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_2;
		painter.drawLine(0, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += razmik_med_vrsticami;

	// zdruzki
		skupaj_brez_ddv = QString::number(skupajbrezddv, 'f', 2).replace(".", ",") + " EUR";
		skupaj_znesek = QString::number(skupajznesek, 'f', 2).replace(".", ",") + " EUR";
		skupaj_znesek_avansa = racun_znesek_avansa.replace(".", ",") + " EUR";
		racun_avans = racun_avans.replace(".", ",") + " %";
		//skupajddvavansa = 16.66667 / 100 * pretvori_v_double(racun_znesek_avansa).toDouble();
		skupajddvavansa = 12345.00;
		skupaj_ddv_avansa = QString::number(skupajddvavansa, 'f', 2).replace(".", ",") + " EUR";
		skupajznesekavansabrezddv = pretvori_v_double(racun_znesek_avansa).toDouble() - skupajddvavansa;
		skupaj_znesek_avansa_brez_ddv = QString::number(skupajznesekavansabrezddv, 'f', 2).replace(".", ",") + " EUR";
		skupaj_se_za_placati = QString::number(skupajznesek - pretvori_v_double(racun_znesek_avansa).toDouble(), 'f', 2).replace(".", ",") + " EUR";
		skupaj_se_za_placati_ddv = QString::number(skupajddvodosnove20 - skupajddvavansa, 'f', 2).replace(".", ",") + " EUR";
		// vcasih poda negativni predznak pred zneskom 0,00
		if ( skupaj_se_za_placati == "-0,00 EUR" ) {
			skupaj_se_za_placati = "0,00 EUR";
		}
		if ( skupaj_se_za_placati_ddv == "-0,00 EUR" ) {
			skupaj_se_za_placati_ddv = "0,00 EUR";
		}

		// nastavi parametre
		painter.setFont(malo_debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Skupaj brez DDV: ")
		velikost_besedila = painter.boundingRect(printer.width() * 1 / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "DDV 20,0 % od osnove: pa se malo");
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height() * 2;

		// izracun visine odseka skupnih zneskov
		int visina_odseka = 0;

		// izracun visine polj brez DDV-ja
		if ( racun_tip == "1" ) {
			visina_odseka = 4 * visina_vrstice + 8 * razmik_med_vrsticami;
		}
		else if ( racun_tip == "2" ) {
			visina_odseka = 5 * visina_vrstice + 10 * razmik_med_vrsticami;
		}
		else if ( racun_tip == "3" || racun_tip == "4" ) {
			visina_odseka = 7 * visina_vrstice + 14 * razmik_med_vrsticami;
		}

		// dodatek k visini za polja DDV
		if ( skupaj_ddv_od_osnove_20 != "0,00 EUR" ) {
			visina_odseka += visina_vrstice + 2 * razmik_med_vrsticami;
		}
		if ( skupaj_ddv_od_osnove_85 != "0,00 EUR" ) {
			visina_odseka += visina_vrstice + 2 * razmik_med_vrsticami;
		}
		if ( skupaj_ddv_od_osnove_00 != "0,00 EUR" ) {
			visina_odseka += visina_vrstice + 2 * razmik_med_vrsticami;
		}

		// preveri, ce je potrebna nova stran
		if( pozicija + visina_noge + visina_odseka >= printer.height() ) {
			printer.newPage();

			int visina_glave = natisni_glavo_izdani_racun(painter, id);

			pozicija = visina_glave;
		}

		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
		// natisnemo besedilo
		painter.setFont(malo);
		painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_brez_ddv);
		// nova vrstica * 2
		pozicija += visina_vrstice;

		// crta
		painter.setPen(*tanek_svincnik);
		painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += razmik_med_vrsticami;

		/**
			* Najprej preberemo vrstico datoteke, nato klicemo zanko
			* tako imamo lahko staticno datoteko z besedili in naknadno izlocimo
			* nepotrebne vrstice. V primeru besedilnih prevodov bo seveda to izpadlo.
			**/

		QString besedilo_ddv = racun.readLine() + " ";
		QString besedilo_od_osnove = " " + racun.readLine();

		double znesekddv = 0.0;

		for ( int stevilo_davkov = 0; stevilo_davkov < seznam_ddv.count(); stevilo_davkov++ ) {
			if ( QString::number(znesek_ddv[stevilo_davkov], 'f', 2) != "0.00" ) {
				// nastavi parametre (XX,X % DDV)
				painter.setFont(malo_debelo);
				besedilo = besedilo_ddv + seznam_ddv[stevilo_davkov].replace(".", ",") + " %" + besedilo_od_osnove;
				// natisnemo besedilo
				painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
				// natisnemo besedilo
				painter.setFont(malo);
				painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width() * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, QString::number(znesek_ddv[stevilo_davkov], 'f', 2).replace(".", ",") + " EUR");
				// nova vrstica * 2
				pozicija += visina_vrstice;

				// crta pod storitvami
				painter.setPen(*tanek_svincnik);
				painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
				// nova vrstica
				pozicija += razmik_med_vrsticami;

				znesekddv += znesek_ddv[stevilo_davkov];

			}
		}

		skupajznesek = skupajbrezddv + znesekddv;

		// nastavi parametre ("Skupaj v EUR: ")
		painter.setFont(malo_debelo);
		besedilo = racun.readLine() + " ";
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
		// natisnemo besedilo
		painter.setFont(malo);
		painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, QString::number(skupajznesek, 'f', 2).replace(".", ",") + " EUR");
		// nova vrstica * 2
		pozicija += visina_vrstice;

		// crta
		painter.setPen(*debel_svincnik);
		painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += razmik_med_vrsticami;

		if ( racun_tip == "1" && racun_avans != "0,0 % " ) {
			// nastavi parametre ("Skupaj % avans: ")
			painter.setFont(malo_debelo);
			QString besedilo_skupaj = racun.readLine();
			QString besedilo_avans = racun.readLine();
			besedilo = besedilo_skupaj + " " + racun_avans + besedilo_avans;
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 1 / 2, pozicija,velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(malo);
			painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_znesek_avansa);
			// nova vrstica * 2
			pozicija += visina_vrstice;

			// crta pod storitvami
			painter.setPen(*tanek_svincnik);
			painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;

			// novo besedilo za avans
			besedilo_ddv = racun.readLine();
			besedilo_od_osnove = racun.readLine();

			for ( int stevilo_davkov = 0; stevilo_davkov < seznam_ddv.count(); stevilo_davkov++ ) {
				if ( QString::number(znesek_ddv[stevilo_davkov], 'f', 2) != "0.00" ) {
					// nastavi parametre (XX,X % DDV)
					painter.setFont(malo_debelo);
					besedilo = besedilo_ddv + seznam_ddv[stevilo_davkov].replace(".", ",") + " %" + besedilo_od_osnove;
					// natisnemo besedilo
					painter.setFont(malo);
					painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
					// natisnemo besedilo
					painter.setFont(malo);
					painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, QString::number(znesek_ddv[stevilo_davkov] * pretvori_v_double(racun_avans).toDouble() / 100, 'f', 2).replace(".", ",") + " EUR");
					// nova vrstica * 2
					pozicija += visina_vrstice;

					// crta pod storitvami
					painter.setPen(*tanek_svincnik);
					painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
					// nova vrstica
					pozicija += razmik_med_vrsticami;

					znesekddv += znesek_ddv[stevilo_davkov];

				}
			}

			// crta pod storitvami
			painter.setPen(*debel_svincnik);
			painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami + visina_vrstice;
		}
		else if ( racun_tip == "1" ) {
			besedilo = racun.readLine() + racun.readLine() + racun.readLine() + racun.readLine();
		}

		if ( racun_tip != "1" && racun_tip != "4" ) { // predplacilo in racun
			if ( racun_avans != "0,0 % " ) {
				// nastavi parametre ("Placano po predracunu st: ")
				painter.setFont(malo_debelo);
				besedilo = racun.readLine() + " " + racun_stevilka_starsa + ": ";
				// natisnemo besedilo
				painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice * 3 / 2), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
				// natisnemo besedilo
				painter.setFont(malo);
				painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice * 3 / 2), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_znesek_avansa);
				// nova vrstica * 2
				pozicija += visina_vrstice * 3 / 2;

				// crta pod storitvami
				painter.setPen(*tanek_svincnik);
				painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
				// nova vrstica
				pozicija += razmik_med_vrsticami;

				// novo besedilo za avans
				besedilo_ddv = racun.readLine();
				besedilo_od_osnove = racun.readLine();

				for ( int stevilo_davkov = 0; stevilo_davkov < seznam_ddv.count(); stevilo_davkov++ ) {
					if ( QString::number(znesek_ddv[stevilo_davkov], 'f', 2) != "0.00" ) {
						// nastavi parametre (XX,X % DDV)
						painter.setFont(malo_debelo);
						besedilo = besedilo_ddv + seznam_ddv[stevilo_davkov].replace(".", ",") + " %" + besedilo_od_osnove;
						// natisnemo besedilo
						painter.setFont(malo);
						painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
						// natisnemo besedilo
						painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, QString::number(znesek_ddv[stevilo_davkov] * pretvori_v_double(racun_avans).toDouble() / 100, 'f', 2).replace(".", ",") + " EUR");
						// nova vrstica * 2
						pozicija += visina_vrstice;

						// crta pod storitvami
						painter.setPen(*tanek_svincnik);
						painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
						// nova vrstica
						pozicija += razmik_med_vrsticami;

						znesekddv += znesek_ddv[stevilo_davkov];

					}
				}

				// crta pod storitvami
				painter.setPen(*debel_svincnik);
				painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
				// nova vrstica
				pozicija += razmik_med_vrsticami;
			}
			else {
				besedilo = racun.readLine() + racun.readLine() + racun.readLine();
			}
		}
		if ( racun_tip == "3" ) { // samo racun
			// nastavi parametre ("Se za placilo: ")
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, QString::number(skupajznesek - pretvori_v_double(skupaj_znesek_avansa).toDouble(), 'f', 2).replace(".", ",") + " EUR");
			// nova vrstica
			pozicija += visina_vrstice;

			// crta pod storitvami
			painter.setPen(*tanek_svincnik);
			painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;

			// novo besedilo za avans
			besedilo_ddv = racun.readLine();
			besedilo_od_osnove = racun.readLine();

			for ( int stevilo_davkov = 0; stevilo_davkov < seznam_ddv.count(); stevilo_davkov++ ) {
				if ( QString::number(znesek_ddv[stevilo_davkov], 'f', 2) != "0.00" ) {
					// nastavi parametre (XX,X % DDV)
					painter.setFont(malo_debelo);
					besedilo = besedilo_ddv + seznam_ddv[stevilo_davkov].replace(".", ",") + " %" + besedilo_od_osnove;
					// natisnemo besedilo
					painter.setFont(malo);
					painter.drawText(QRectF(printer.width() * 1 / 2, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
					// natisnemo besedilo
					painter.setFont(malo);
					painter.drawText(QRectF(printer.width()  * 1 / 2 + velikost_besedila.width() + 10, pozicija, printer.width()  * 1 / 2 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, QString::number(znesek_ddv[stevilo_davkov] * ( 1 - pretvori_v_double(racun_avans).toDouble() / 100 ), 'f', 2).replace(".", ",") + " EUR");
					// nova vrstica * 2
					pozicija += visina_vrstice;

					// crta pod storitvami
					painter.setPen(*tanek_svincnik);
					painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
					// nova vrstica
					pozicija += razmik_med_vrsticami;

					znesekddv += znesek_ddv[stevilo_davkov];

				}
			}

			// crta pod storitvami
			painter.setPen(*debel_svincnik);
			painter.drawLine(printer.width()  * 1 / 2 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami + visina_vrstice;
		}

		if ( racun_tip == "2" || ( racun_tip == "1" && racun_avans == "0,0 % " ) ) {
			pozicija += razmik_med_vrsticami + visina_vrstice;
		}

	// razlog stornacije
		// opomba morebitne stornacije
		besedilo = racun.readLine();
		if ( razlog_stornacije != "" ) {
			// napisemo naslov
			painter.setFont(malo_debelo);
			// dolocimo velikost kvadrata, ki ga tvori besedilo (razlog stornacije)
			velikost_besedila = painter.boundingRect(0, pozicija, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta

			// preveri, ce je potrebna nova stran
			if( pozicija + visina_noge + visina_vrstice + razmik_med_vrsticami >= printer.height() ) {
				printer.newPage();

				int visina_glave = natisni_glavo_izdani_racun(painter, id);

				pozicija = visina_glave;
			}

			// natisnemo besedilo
			painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;

			// napisemo razlog stornacije
			painter.setFont(malo);
			besedilo = razlog_stornacije;

			// dolocimo velikost kvadrata, ki ga tvori besedilo (razlog stornacije)
			velikost_besedila = painter.boundingRect(0, pozicija, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta

			// preveri, ce je potrebna nova stran
			if( pozicija + visina_noge + visina_vrstice + razmik_med_vrsticami >= printer.height() ) {
				printer.newPage();

				int visina_glave = natisni_glavo_izdani_racun(painter, id);

				pozicija = visina_glave;
			}

			// natisnemo besedilo
			painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}

	// opombe
		// priprava baze in polnenje storitve

			QString opombe = "";
			QString preostanek_opombe = "";
			QString ident = "";

			QString beseda = "";

			QSqlQuery sql_opombe;
			sql_opombe.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
			sql_opombe.exec();
			if ( sql_opombe.next() ) {
				opombe = prevedi(sql_opombe.value(sql_opombe.record().indexOf("opombe")).toString()).replace(",,", ",");
			}

			if ( opombe != "" ) {
				// napisemo naslov
				painter.setFont(debelo);
				besedilo = "Opombe";
				// dolocimo velikost kvadrata, ki ga tvori besedilo (razlog stornacije)
				velikost_besedila = painter.boundingRect(0, pozicija, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
				// nastavimo parametre
				visina_vrstice = velikost_besedila.height();
				razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta

				// preveri, ce je potrebna nova stran
				if( pozicija + visina_noge + visina_vrstice + razmik_med_vrsticami >= printer.height() ) {
					printer.newPage();

					int visina_glave = natisni_glavo_izdani_racun(painter, id);

					pozicija = visina_glave;
				}

				// natisnemo besedilo
				painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
				// nova vrstica
				pozicija += visina_vrstice + razmik_med_vrsticami;
			}

			preostanek_opombe = opombe;
			for ( int a = 1; a < opombe.count(","); a++ ) {
				preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - 1); // odstranimo zacetno vejico
				int vejica = preostanek_opombe.indexOf(",", 0); // dobimo pozicijo prve vejice
				ident = preostanek_opombe.left(vejica); // dobimo id opombe
				preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - ident.length() ); // opombe skrajsamo za dolzino enega id-ja

				QSqlQuery sql_besedilo;
				sql_besedilo.prepare("SELECT * FROM sif_opombe_pri_racunih WHERE id LIKE '" + pretvori(ident) + "'");
				sql_besedilo.exec();
				if ( sql_besedilo.next() ) {
					beseda = QString::number(a, 10) + ") " +
									 prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("besedilo")).toString());
				}

				painter.setFont(malo);
				// dolocimo velikost kvadrata, ki ga tvori besedilo (opomba)
				velikost_besedila = painter.boundingRect(0, pozicija, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, beseda);
				// nastavimo parametre
				visina_vrstice = velikost_besedila.height();
				razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta

				// preveri, ce je potrebna nova stran
				if( pozicija + visina_noge + visina_vrstice + razmik_med_vrsticami >= printer.height() ) {
					printer.newPage();

					int visina_glave = natisni_glavo_izdani_racun(painter, id);

					pozicija = visina_glave;
				}

				// natisnemo besedilo
				painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, beseda);
				// nova vrstica
				pozicija += visina_vrstice + razmik_med_vrsticami;

			}

		pozicija += visina_vrstice + 2 * razmik_med_vrsticami;

	// podpis

		// naziv
		painter.setFont(normalno);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo (naziv podpisnika)
		velikost_besedila = painter.boundingRect(0, pozicija, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_oseba_naziv);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta

		// preveri, ce je potrebna nova stran
		if( pozicija + visina_noge + visina_vrstice * 4 + razmik_med_vrsticami * 4 >= printer.height() ) { // zahvala + naziv + podpis
			printer.newPage();

			int visina_glave = natisni_glavo_izdani_racun(painter, id);

			pozicija = visina_glave;
		}

		// natisnemo besedilo // Hvala za zaupanje!
		painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

		painter.drawText(QRectF(printer.width() - velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_oseba_naziv);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// podpis
		// dolocimo velikost kvadrata, ki ga tvori besedilo (ime podpisnika)
		velikost_besedila = painter.boundingRect(0, pozicija, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_oseba);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_oseba);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		painter.end();


}

int tiskanje::natisni_glavo_izdani_racun(QPainter &painter, QString id) {

	// ponastavimo podatke painter-ja
	painter.save();
	painter.resetTransform();

	// priprava podatkov o tiskanju
	double pozicija = 0;
	double visina_vrstice = 0;
	double razmik_med_vrsticami = 0;
	double faktor_razmika_med_vrsticami = pretvori_v_double(ui->txt_presledek_med_vrsticami_besedilo->text()).toDouble();

	// nastravitve
	QFont debelo(ui->txt_druzina_pisave->currentText(), pretvori_v_double(ui->txt_velikost_vecja->text()).toDouble(), QFont::Bold);

	QPen *tanek_svincnik = new QPen;
	tanek_svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());

	// priprava spremenljivk
	QString podjetje_logo = "";
	QString podjetje_kratki = "";
	QString podjetje_polni = "";
	QString podjetje_naslov_ulica = "";
	QString podjetje_naslov_stevilka = "";
	QString podjetje_naslov_postna_stevilka = "";
	QString podjetje_naslov_posta = "";

	// priprava baze in polnenje spremenljivk (razen storitve)

		// podjetje izvemo iz stevilke podjetja, ki ji pripada uporabnik, kateri tiska racun
		QSqlQuery sql_podjetje;
		sql_podjetje.prepare("SELECT * FROM racuni WHERE id LIKE '" + id + "'");
		sql_podjetje.exec();
		if ( sql_podjetje.next() ) {
			podjetje_logo = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_logotip")).toString());
			podjetje_kratki = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_kratki")).toString());
			podjetje_polni = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_polni")).toString());
			podjetje_naslov_ulica = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_ulica")).toString());
			podjetje_naslov_stevilka = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_stevilka")).toString());
			podjetje_naslov_postna_stevilka = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_postna_stevilka")).toString());
			podjetje_naslov_posta = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_posta")).toString());
		}

	// logotip podjetja
	// pretvori besedilo slike v bytearray
	QByteArray slika_array;
	slika_array.append(podjetje_logo);

	QPixmap *logotip_pixmap = new QPixmap();
	// pretvorimo iz base64 v normalen bytearray
	logotip_pixmap->loadFromData(QByteArray::fromBase64(slika_array));

	// iz pixmap-a naredimo sliko, ki jo prenesemo naprej
	QImage logo(logotip_pixmap->toImage());

	// narisi logotip
	if ( logo.height() != 70 ) {
		painter.drawImage(0, pozicija, logotip_pixmap->scaledToHeight(70).toImage(), 0, 0, 0, 0);
	}
	else {
		painter.drawImage(0, pozicija, logotip_pixmap->toImage(), 0, 0, 0, 0);
	}

	pozicija = 10; // zaradi velikosti logotipa
	// ime in naslov podjetja
	painter.setFont(debelo);
	// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
	QRect velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_kratki);
	// nastavimo parametre
	visina_vrstice = velikost_besedila.height();
	razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami; // razmik med vrsticami, za lazje branje dokumenta
	// natisnemo besedilo
	painter.drawText(QRectF(0, pozicija, painter.window().width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_kratki);
	// nova vrstica
	pozicija += visina_vrstice + razmik_med_vrsticami;

	// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, polni naziv)
	velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni);
	// natisnemo besedilo
	painter.drawText(QRectF(0, pozicija,painter.window().width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_polni);
	// nova vrstica
	pozicija += visina_vrstice + razmik_med_vrsticami;

	// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, ulica)
	velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
	// natisnemo besedilo
	painter.drawText(QRectF(0, pozicija, painter.window().width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
	// nova vrstica
	pozicija += visina_vrstice + razmik_med_vrsticami;

	// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, pos)
	velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
	// natisnemo besedilo
	painter.drawText(QRectF(0, pozicija, painter.window().width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
	// nova vrstica
	pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

	// crta pod glavo
	painter.setPen(*tanek_svincnik);
	painter.drawLine(0, pozicija, painter.window().width(), pozicija);

	// nova vrstica
	pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

	// vrnemo zacetne nastavitve painter-ja
	painter.restore();

	// vrnemo visino glave
	return pozicija;

}

int tiskanje::natisni_nogo_izdani_racun(QPainter &painter, QString id, int &stevilka_strani, QString besedilo_noga_ddv, QString besedilo_noga_maticna) {

	// ponastavimo podatke painter-ja
	painter.save();
	painter.resetTransform();

	// ponastavimo podatke painter-ja
	painter.save();
	painter.resetTransform();

	// priprava podatkov o tiskanju
	double pozicija = 0;
	double visina_vrstice = 0;
	double razmik_med_vrsticami = 0;
	double faktor_razmika_med_vrsticami = pretvori_v_double(ui->txt_presledek_med_vrsticami_besedilo->text()).toDouble();

	// nastravitve
	QFont normalna(ui->txt_druzina_pisave->currentText(), pretvori_v_double(ui->txt_velikost_vecja->text()).toDouble(), QFont::Bold);
	QFont mala(ui->txt_druzina_pisave->currentText(), pretvori_v_double(ui->txt_velikost_manjsa->text()).toDouble(), QFont::Bold);

	QPen *tanek_svincnik = new QPen;
	tanek_svincnik->setWidth(pretvori_v_int(ui->txt_crta_tanka->text()).toInt());
	QPen *debel_svincnik = new QPen;
	debel_svincnik->setWidth(pretvori_v_int(ui->txt_crta_debela->text()).toInt());

	// priprava spremenljivk
	QString podjetje_kratki = "";
	QString podjetje_polni = "";
	QString podjetje_url = "";
	QString podjetje_email = "";
	QString podjetje_ddv = "";
	QString podjetje_naslov_ulica = "";
	QString podjetje_naslov_stevilka = "";
	QString podjetje_naslov_postna_stevilka = "";
	QString podjetje_naslov_posta = "";
	QString podjetje_telefon = "";
	QString podjetje_maticna = "";

	// priprava baze in polnenje spremenljivk (razen storitve)
		// podjetje izvemo iz stevilke podjetja, ki ji pripada uporabnik, kateri tiska racun
		QSqlQuery sql_podjetje;
		sql_podjetje.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
		sql_podjetje.exec();
		if ( sql_podjetje.next() ) {
			podjetje_kratki = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_kratki")).toString());
			podjetje_polni = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_polni")).toString());
			podjetje_url = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_url")).toString());
			podjetje_email = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_email")).toString());
			podjetje_ddv = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_ddv")).toString());
			podjetje_naslov_ulica = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_ulica")).toString());
			podjetje_naslov_stevilka = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_stevilka")).toString());
			podjetje_naslov_postna_stevilka = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_postna_stevilka")).toString());
			podjetje_naslov_posta = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_naslov_posta")).toString());
			podjetje_telefon = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_telefon")).toString());
			podjetje_maticna = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("podjetje_maticna")).toString());
		}
		sql_podjetje.clear();

	// risati pricnemo na spodnjem robu lista

	// podatki o podjetju
	painter.setFont(mala);

	// besedilo (url, telefon, email, DDV)
	QString besedilo = podjetje_url +	" | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga_ddv + " " + podjetje_ddv + " | " + besedilo_noga_maticna + " " + podjetje_maticna;
//	QString besedilo = podjetje_url +	" | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga_ddv + " " + podjetje_ddv;
	// dolocimo velikost kvadrata, ki ga tvori besedilo
	QRect velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
	// nastavimo parametre
	visina_vrstice = velikost_besedila.height();
	razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami; // razmik med vrsticami, za lazje branje dokumenta
	// natisnemo besedilo
	painter.drawText(QRectF(0, painter.window().height() - visina_vrstice, painter.window().width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo za pisanje
	pozicija += visina_vrstice + razmik_med_vrsticami;

	// besedilo (podjetje, kratki naziv)
	besedilo = podjetje_polni + " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " +
						 podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta;
	// dolocimo velikost kvadrata, ki ga tvori besedilo
	velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
	// nastavimo parametre
	visina_vrstice = velikost_besedila.height();
	razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami; // razmik med vrsticami, za lazje branje dokumenta
	// natisnemo besedilo
	painter.drawText(QRectF(0, painter.window().height() - pozicija - visina_vrstice, painter.window().width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo za pisanje
	pozicija += visina_vrstice + razmik_med_vrsticami * 2;

	// crta nad podatki o podjetju
	painter.setPen(*tanek_svincnik);
	painter.drawLine(0, painter.window().height() - pozicija, painter.window().width(), painter.window().height() - pozicija);

	// nastavimo novo pozicijo za pisanje
	pozicija += razmik_med_vrsticami * 4;

	// podatki o st. strani
	painter.setFont(normalna);

	// besedilo (stran)
	besedilo = "Stran: " + QString::number(stevilka_strani, 10);
	// dolocimo velikost kvadrata, ki ga tvori besedilo
	velikost_besedila = painter.boundingRect(0, 0, painter.window().width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
	// nastavimo parametre
	visina_vrstice = velikost_besedila.height();
	razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami; // razmik med vrsticami, za lazje branje dokumenta
	// natisnemo besedilo
	painter.drawText(QRectF(0, painter.window().height() - pozicija - visina_vrstice, painter.window().width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, besedilo);
	// nastavimo novo pozicijo za pisanje
	pozicija += visina_vrstice + razmik_med_vrsticami * 2;

	// crta nad podatki o strani
//	painter.setPen(*tanek_svincnik);
//	painter.drawLine(0, painter.window().height() - pozicija, painter.window().width(), painter.window().height() - pozicija);

	// nastavimo novo pozicijo za pisanje
	pozicija += razmik_med_vrsticami * 4;

	// vrnemo zacetne nastavitve painter-ja
	painter.restore();

	// stevilko strani povecamo za 1
	stevilka_strani++;

	// vrnemo visino glave
	return pozicija;

}

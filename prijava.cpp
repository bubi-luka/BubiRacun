#include <QMessageBox>
#include <QtSql>
#include <QObject>
#include <QFile>
#include <QDir>
#include <QKeyEvent>

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

	// reset user
	vApp->set_id("");

//    this->activateWindow();
//    this->setFocus();
	ui->txt_uporabnik->setFocus();

}

prijava::~prijava() {
	delete ui;
}

void prijava::on_btn_izhod_clicked() {

	if ( vApp->id() == "" ) {
		exit(0);
	}
	else {
		close();
	}

}

void prijava::on_btn_brisi_clicked() {

	ui->txt_geslo->setText("");
	ui->txt_uporabnik->setText("");

	ui->txt_uporabnik->setFocus();

}

void prijava::on_btn_prijavi_clicked() {

		QString user_id = "";

		QSqlQuery sql_preveri;
		sql_preveri.prepare("SELECT * FROM uporabniki WHERE user_name LIKE '" + pretvori(ui->txt_uporabnik->text().toLower()) + "'");
		sql_preveri.exec();
		if ( !sql_preveri.next() ) { // uporabnik ne obstaja

			ui->txt_uporabnik->setText("");
			ui->txt_geslo->setText("");

			vApp->set_id("");

			QMessageBox msgbox;
			msgbox.setText("Uporabnisko ime ni pravilno!");
			msgbox.setInformativeText("V bazi ne obstaja uporabnik s tem uporabniskim imenom.");
			msgbox.exec();

		}
		else { // uporabnik obstaja

			user_id = prevedi(sql_preveri.value(sql_preveri.record().indexOf("id")).toString());

			if ( prevedi(sql_preveri.value(sql_preveri.record().indexOf("geslo")).toString()) != ui->txt_geslo->text() ) { // geslo se ne ujema

				ui->txt_geslo->setText("");

				vApp->set_id("");

				QMessageBox msgbox;
				msgbox.setText("Geslo ni pravilno!");
				msgbox.setInformativeText("Vnesli ste napacno geslo! Poskusite ponovno!");
				msgbox.exec();

			}
			else { // geslo se ujema

				// nastavi uporabnika, stanje, pravice uporabnika
				vApp->set_id(pretvori(user_id)); // nastavi uporabniske pravice

				if ( ui->txt_uporabnik->text().left(1) == ui->txt_uporabnik->text().left(1).toLower() ) { // ce je velika zacetnica smo na odprtem dostopu
					vApp->set_state(pretvori("private")); // samo za oci pisarja
				}
				else {
					vApp->set_state(pretvori("public")); // odprto za oci ljudske mnozice
				}

				// ustvari varnostno kopijo
				varnostna_kopija();

				// skrij prijavno okno in odpri glavno okno
				poslji("loginok");

			}
		}

}

// ustvari varnostno kopijo
void prijava::varnostna_kopija() {
/*
		QString mapa_za_shranjevanje = "";

		// v bazi poiscemo pot do mesta shranjevanja podatkov
		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM nastavitve WHERE naziv LIKE '" + pretvori("pot") + "'");
		sql_pot.exec();
		if ( sql_pot.next() ) {
			mapa_za_shranjevanje = prevedi(sql_pot.value(sql_pot.record().indexOf("vrednost")).toString());
		}

		if ( mapa_za_shranjevanje != "" ) { // varnostno kopiranje podatkov izvedemo zgolj v primeru, da obstaja mapa za shranjevanje podatkov

			// ustvari mapo za shranjevanje baze podatkov

			QDir mapa(mapa_za_shranjevanje);
			if ( mapa.exists() ) {
				mapa.mkdir("arhiv"); // osnovna mapa
				mapa.cd("arhiv");
				mapa.mkdir("dnevni"); // dnevni back-up
				mapa.mkdir("mesecni"); // mesecni back-up
*/
				/**
					* naredi mesecni arhiv
					* pogleda, ali ze obstaja arhivska datoteka za tekoci mesec
					* ce obstaja, jo izbrise in napravi novo za tekoci mesec
					* ce ne obstaja to pomeni, da smo na zacetku meseca
					* sedaj naredi novi mesecni arhiv in hkrati izbrise dnevni arhiv "prejsnjega"
					* meseca, ki sedaj ni vec aktualen (drugace bomo kmalu imeli nepregledno mnozico baz)
					* to pa ni smiselno
					**/
/*				QFile f_baza_org;
				f_baza_org.setFileName(dbase_path);
				QFile f_baza_mesec;
				f_baza_mesec.setFileName(mapa_za_shranjevanje + "/arhiv/mesecni/base-" + QDate::currentDate().toString("yyyy'-'MM") + ".bz.bck");

				if ( f_baza_mesec.exists() ) { // ce obstaja, smo v tekocem mesecu in zgolj zbrisemo trenutno bazo in jo nadomestimo z novo
					f_baza_mesec.remove();
					f_baza_org.copy(mapa_za_shranjevanje + "/arhiv/mesecni/base-" + QDate::currentDate().toString("yyyy'-'MM") + ".bz.bck");
				}
				else { // gre za nov mesec, baze ne brisemo, brisemo pa vse baze v arhivu za
					f_baza_org.copy(mapa_za_shranjevanje + "/arhiv/mesecni/base-" + QDate::currentDate().toString("yyyy'-'MM") + ".bz.bck");

					// poiscemo vse baze iz dnevnega arhiva
					mapa.cd("dnevni");

					QStringList seznam_arhiva = mapa.entryList();

					for ( int a = 0; a < seznam_arhiva.size(); a++ ) {
						QFile f_arhiv;
						f_arhiv.setFileName(mapa_za_shranjevanje + "/arhiv/dnevni/" + seznam_arhiva.at(a).toUtf8());
						f_arhiv.remove();
					}

				}
*/
				/**
					* naredimo dnevni arhiv tako, da shranimo bazo kot datoteko s tekoco zadnjo
					* identifikacijsko stevilko
					* arhiv je samo za tekoci mesec, za prejsnje datoteke izbrisemo
					**/
/*				int i = 1;
				while ( !f_baza_org.copy(mapa_za_shranjevanje + "/arhiv/dnevni/base-" + QDate::currentDate().toString("yyyy'-'MM'-'dd") + "-" + QString::number(i, 10) + ".bz.bck") ) {
					i++;
				}
			}

		}
*/
}

// pretvori v in iz kodirane oblike
QString prijava::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString prijava::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void prijava::keyPressEvent(QKeyEvent *event) {

	if ( event->key() == Qt::Key_Escape ) {
		on_btn_brisi_clicked();
	}
	else if ( ( event->key() == Qt::Key_Enter ) || ( event->key() == Qt::Key_Return ) ) {
		on_btn_prijavi_clicked();
	}

}

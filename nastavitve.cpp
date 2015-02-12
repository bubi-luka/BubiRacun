#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>

#include "popusti.h"
#include "kodiranje.h"
#include "prijava.h"

#include "nastavitve.h"
#include "ui_nastavitve.h"

nastavitve::nastavitve(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::nastavitve)
{
	ui->setupUi(this);

	// nastavi polja iz baze
		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM nastavitve");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()) == "pot" ) {
				ui->txt_pot->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString()));
			}
			if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()) == "delavniki" ) {
				ui->txt_pot_do_delavnikov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString()));
			}
		}

	// ce so polja prazna, jih nastavi na privzete vrednosti
	if ( ui->txt_pot->text() == "" ) {
		ui->txt_pot->setText(QDir::homePath() + "/BubiRacun-Dokumenti");
	}
	if ( ui->txt_pot_do_delavnikov->text() == "" ) {
		ui->txt_pot_do_delavnikov->setText("http://www.racunovodja.com/mdokumenti/delure2002.asp");
	}

}

nastavitve::~nastavitve()
{
	delete ui;
}

void nastavitve::on_btn_pot_clicked() {

	// izberi mapo za shranjevanje podatkov
	QString mapa_za_shranjevanje = QFileDialog::getExistingDirectory(this,
																	 "Izberite mapo za shranjevanje dokumentov",
																	 QDir::homePath(), QFileDialog::ShowDirsOnly);

	// ustvariti pot do ustrezne mape
	QDir mapa(mapa_za_shranjevanje);
	if ( !mapa_za_shranjevanje.contains("BubiRacun-Dokumenti") ) {
		mapa.mkdir("BubiRacun-Dokumenti");
		mapa_za_shranjevanje.append("/BubiRacun-Dokumenti");
	}

	ui->txt_pot->setText(mapa_za_shranjevanje);

}

void nastavitve::on_btn_popusti_clicked() {

	popusti *okno = new popusti;
	okno->open();

}

void nastavitve::on_btn_ponastavi_clicked() {

	ui->txt_pot->setText(QDir::homePath() + "/BubiRacun-Dokumenti");

}

void nastavitve::on_btn_izhod_clicked() {

	if ( ui->txt_pot->text() == "" ) {
		exit(0);
	}
	else {
		close();
	}

}

void nastavitve::on_btn_shrani_clicked() {

	// nastavi polja iz baze
		QSqlQuery sql_shrani;
		sql_shrani.prepare("UPDATE nastavitve SET vrednost = ? WHERE naziv LIKE '" + pretvori("pot")+ "'");
		sql_shrani.bindValue(0, pretvori(ui->txt_pot->text()));
		sql_shrani.exec();
		sql_shrani.clear();
		sql_shrani.prepare("UPDATE nastavitve SET vrednost = ? WHERE naziv LIKE '" + pretvori("delavniki")+ "'");
		sql_shrani.bindValue(0, pretvori(ui->txt_pot_do_delavnikov->text()));
		sql_shrani.exec();
		sql_shrani.clear();

	QMessageBox sporocilo;
	sporocilo.setText("Nastavitve so uspesno shranjene!");
	sporocilo.exec();

	close();

}

// import and export of whole database
void nastavitve::on_btn_import_clicked() {

	// nastavi pot do baze, ki je arhivirana
	QString baza_stara = QFileDialog::getOpenFileName(this,
															"Izberite datoteko za uvoz v program",
															QDir::homePath(), "Baza (*.bac.bz)");

	// nastavi pot do baze, kot jo uporablja aplikacija
	QString app_path = QApplication::applicationDirPath();
	QString baza_nova = app_path + "/base.bz";

	// zamenjaj trenutno bazo z novo
	if ( baza_stara != "" ) {
		QFile::remove(baza_nova);
		QFile::copy(baza_stara, baza_nova);

		// odjavi uporabnika, da se baza ponovno nalozi
		prijava *okno_prijava = new prijava;
		okno_prijava->show();

		this->close();
	}

}

void nastavitve::on_btn_export_clicked() {

	// nastavi pot do baze, ki je arhivirana
	QString mapa_za_shranjevanje = QFileDialog::getExistingDirectory(this,
																	 "Izberite mapo za shranjevanje dokumentov",
																	 QDir::homePath(), QFileDialog::ShowDirsOnly);
	QString baza_nova = mapa_za_shranjevanje + "/base-" + QDate::currentDate().toString("yyyy-MM-dd") + ".bac.bz";

	// nastavi pot do baze, kot jo arhiviramo
	QString app_path = QApplication::applicationDirPath();
	QString baza_stara = app_path + "/base.bz";

	// zamenjaj trenutno bazo z novo
	if ( mapa_za_shranjevanje != "" ) {
		QFile::copy(baza_stara, baza_nova);
	}

}

// pretvori v in iz kodirane oblike
QString nastavitve::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString nastavitve::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

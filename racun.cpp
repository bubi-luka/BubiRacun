#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QFile>
#include <QFileDialog>

#include "racun.h"
#include "ui_racun.h"
#include "wid_racuni.h"
#include "projekti.h"
#include "opravila.h"
#include "kodiranje.h"
#include "varnost.h"

racun::racun(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::racun)
{
    ui->setupUi(this);

		// pocisti polja
		ui->txt_id->setText("");
		ui->txt_stevilka_racuna->setText("");
		ui->txt_stara_stevilka_racuna->setText("");
		ui->txt_status_predracuna->clear();
		ui->txt_stranka_id->setText("");
		ui->txt_stranka->clear();
		ui->txt_projekt_id->setText("");
		ui->txt_projekt->clear();
		ui->txt_pricetek->setDate(QDate::currentDate());
		ui->txt_konec->setDate(QDate::currentDate());

		ui->txt_avans->setText("");
		ui->txt_se_placati->setText("");
		ui->txt_popusti->setText("");
		ui->txt_znesek_brez_ddv->setText("");
		ui->txt_znesek_ddv->setText("");
		ui->txt_znesek->setText("");
		ui->txt_sklic->setText("");

		ui->txt_datum_izdaje_racuna->setDate(QDate::currentDate());
		ui->txt_rok_placila->setDate(QDate::currentDate());
		ui->txt_status_placila->clear();
		ui->txt_status_racunovodstva->clear();
		ui->txt_status_oddaje_racuna->clear();
		ui->txt_datum_oddaje_racuna->setDate(QDate::currentDate());
		ui->txt_datum_placila_avansa->setDate(QDate::currentDate());

		ui->txt_odstotek_avansa->setText("0,0 %");

		ui->tbl_opravila->clear();

		// onemogoci polja
		ui->txt_id->setEnabled(false);
		ui->txt_projekt_id->setEnabled(false);
		ui->txt_stranka_id->setEnabled(false);
		ui->txt_znesek_ddv->setEnabled(false);
		ui->txt_popusti->setEnabled(false);
		ui->txt_se_placati->setEnabled(false);
		ui->txt_znesek->setEnabled(false);
		ui->txt_znesek_brez_ddv->setEnabled(false);
		ui->txt_avans->setEnabled(false);
		ui->txt_stara_stevilka_racuna->setEnabled(false);

		// skrij polja
		ui->txt_projekt_id->setVisible(false);
		ui->txt_stranka_id->setVisible(false);

		// napolni spustne sezname
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
			// baza podatkov je odprta

			// napolni spustne sezname
			ui->txt_status_predracuna->addItem("");
			ui->txt_projekt->addItem("");
			ui->txt_stranka->addItem("");
			ui->txt_status_placila->addItem("");
			ui->txt_status_racunovodstva->addItem("");
			ui->txt_status_oddaje_racuna->addItem("");

			QSqlQuery sql_fill_combo;
			sql_fill_combo.prepare("SELECT * FROM sif_status_predracuna");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_status_predracuna->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("status")).toString()));
			}
			sql_fill_combo.clear();

			sql_fill_combo.prepare("SELECT * FROM stranke");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				if (prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("tip")).toString()) == "1" ) {
					ui->txt_stranka->addItem(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
											 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("priimek")).toString()) + ", "
											 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
				}
				else {
					ui->txt_stranka->addItem(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
											 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
				}
			}
			sql_fill_combo.clear();

			sql_fill_combo.prepare("SELECT * FROM sif_status_placila");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				ui->txt_status_placila->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("status")).toString()));
			}
			sql_fill_combo.clear();

			sql_fill_combo.prepare("SELECT * FROM sif_status_racunovodstva");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				ui->txt_status_racunovodstva->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("status")).toString()));
			}
			sql_fill_combo.clear();

			sql_fill_combo.prepare("SELECT * FROM sif_status_oddaje_racuna");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				ui->txt_status_oddaje_racuna->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("status")).toString()));
			}
			sql_fill_combo.clear();

		}
		base.close();

		// nastavi privzete vrednosti
		ui->rb_predracun->setChecked(true);
		ui->txt_status_placila->setCurrentIndex(1);
		ui->txt_status_racunovodstva->setCurrentIndex(1);

		ui->tab_racuni->setCurrentIndex(0);

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
			QString kraj;
			QString tekoci_racun;
			QSqlQuery sql_podjetje;
			sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + pretvori(vApp->firm()) + "'");
			sql_podjetje.exec();
			if ( sql_podjetje.next() ) {
				vsebina.append(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("ime")).toString()) + ";"); // kratki naziv podjetja
				vsebina.append(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("polnoime")).toString()) + ";"); // polni naziv podjetja
				vsebina.append(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("naslov")).toString()) + " "); // naslov podjetja
				vsebina.append(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("naslov_st")).toString()) + ", "); // naslov podjetja
				vsebina.append(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("postna_stevilka")).toString()) + " "); // naslov podjetja
				vsebina.append(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("posta")).toString()) + ";"); // naslov podjetja
				vsebina.append(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("davcna")).toString()) + ";"); // davcna st. podjetja

				// podatki za nadaljno obdelavo
				kraj = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("posta")).toString());
				tekoci_racun = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("tekoci_racun")).toString());
			}
			else {
				vsebina.append(";"); // kratki naziv podjetja
				vsebina.append(";"); // polni naziv podjetja
				vsebina.append(";"); // naslov podjetja
				vsebina.append(";"); // davcna st. podjetja
			}

			// podatki o stranki
			QSqlQuery sql_stranka;
			sql_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
			sql_stranka.exec();
			if ( sql_stranka.next() ) {
				if ( prevedi(sql_stranka.value(sql_stranka.record().indexOf("tip")).toString()) == "pravna" ) {
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("ime")).toString()) + ";"); // ime stranke - podjetja
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("priimek")).toString()) + ";"); // polno ime stranke - podjetja
				}
				else {
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("ime")).toString()) + " "); // ime stranke - oseba
					vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("priimek")).toString()) + ";"); // ime stranke - oseba
					vsebina.append(";"); // prazna (polno ime) stranke - oseba
				}
				vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("naslov")).toString()) + " "); // naslov stranke
				vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("naslov_st")).toString()) + ";"); // naslov stranke
				vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("postna_stevilka")).toString()) + " "); // naslov stranke
				vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("posta")).toString()) + ";"); // naslov stranke
				vsebina.append(prevedi(sql_stranka.value(sql_stranka.record().indexOf("davcna")).toString()) + ";");
			}

			// podatki o racunu
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("tip_racuna")).toString()) + ";"); // racun, predracun
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("stevilka_racuna")).toString()) + ";"); // enako UPN sklicu
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("datum_izdaje")).toString()) + ";"); // datum izdaje racuna
			vsebina.append(kraj + ";"); // kraj izdaje racuna = kraj podjetja
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("datum_pricetka")).toString()) + " - "); // obdobje storitve
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("datum_konca")).toString()) + ";"); // obdobje storitve
			vsebina.append(prevedi(sql_racun.value(sql_racun.record().indexOf("datum_placila")).toString()) + ";"); // datum roka placila
			vsebina.append(tekoci_racun + ";"); // trr podjetja

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
			QString storitve;
			while ( sql_storitve.next() ) {
				storitve.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()) + ",");
				storitve.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("ure")).toString()) + ",");
				storitve.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("cena_ure")).toString()) + ",");
				storitve.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popusti")).toString()) + ",");
				storitve.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("ddv")).toString()) + ",");
				storitve.append(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesekbrezddv")).toString()) + ",");
				storitve.append("\n");

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
			vsebina += storitve + ";";
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

	QString pot_izvoza = QFileDialog::getSaveFileName(this, tr("Izberite kraj izvoza izdanega racuna"),
														 QDir::homePath() + "/Documents/izdani-racuni.csv",
														 "Urejena tekstovna datoteka (*.csv *.txt *.xml, xls, ods)");

	QFile file1(pot_izvoza);
	if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file1);
	out << vsebina;
	file1.close();

}

void racun::on_btn_izhod_clicked() {

	close();

}

void racun::on_txt_stranka_currentIndexChanged() {

	QString besedilo = ui->txt_stranka->currentText();

	if (besedilo != "") {
		ui->txt_stranka_id->setText(besedilo.left(besedilo.indexOf(") ", 0)));
	}
	else {
		ui->txt_stranka_id->setText("");
	}

}

void racun::on_txt_stranka_id_textChanged() {

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
		QSqlQuery sql_set_stranka;
		sql_set_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
		sql_set_stranka.exec();
		if ( sql_set_stranka.next() ) {
			QString stranka;
			if (prevedi(sql_set_stranka.value(sql_set_stranka.record().indexOf("tip")).toString()) == "1" ) {
				stranka = prevedi(sql_set_stranka.value(sql_set_stranka.record().indexOf("id")).toString()) + ") " +
									prevedi(sql_set_stranka.value(sql_set_stranka.record().indexOf("priimek")).toString()) + ", " +
									prevedi(sql_set_stranka.value(sql_set_stranka.record().indexOf("ime")).toString());
			}
			else {
				stranka = prevedi(sql_set_stranka.value(sql_set_stranka.record().indexOf("id")).toString()) + ") " +
									prevedi(sql_set_stranka.value(sql_set_stranka.record().indexOf("ime")).toString());
			}

			ui->txt_stranka->setCurrentIndex(ui->txt_stranka->findText(stranka));

		}

		// fill combo box
		ui->txt_projekt->addItem("");

		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM projekti WHERE stranka LIKE '" + ui->txt_stranka_id->text() + "' AND avtor_oseba LIKE '" + pretvori(vApp->id()) + "'");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_projekt->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ") " +
															 prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("stevilka_projekta")).toString()) + " - " +
															 prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naslov_projekta")).toString()));
		}

	}
	base.close();

}

void racun::on_txt_projekt_currentIndexChanged() {

	QString besedilo = ui->txt_projekt->currentText();

	if (besedilo != "") {
		ui->txt_projekt_id->setText(besedilo.left(besedilo.indexOf(") ", 0)));
	}
	else {
		ui->txt_projekt_id->setText("");
	}

}

void racun::on_txt_projekt_id_textChanged() {

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
		QSqlQuery sql_set_projekt;
		sql_set_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_projekt_id->text()) + "'");
		sql_set_projekt.exec();
		if ( sql_set_projekt.next() ) {
			QString projekt;
			projekt = prevedi(sql_set_projekt.value(sql_set_projekt.record().indexOf("id")).toString()) + ") " +
								prevedi(sql_set_projekt.value(sql_set_projekt.record().indexOf("stevilka_projekta")).toString()) + " - " +
								prevedi(sql_set_projekt.value(sql_set_projekt.record().indexOf("naslov_projekta")).toString());

			ui->txt_projekt->setCurrentIndex(ui->txt_projekt->findText(projekt));

		}

	}
	base.close();

}

// ne preverja obveznih polj
void racun::on_btn_sprejmi_clicked() {

	/*
		vprašaj, če je to končna različica predračuna, če lahko izdela komplet zadevo
		če ne, shrani samo predračun
		če ja, potem izdela predračun, predplačilo (če je podan odstotek avansa višji kot 0%), račun
		če so na voljo predplačilo in račun, onemogoči urejanje predplačila in predračuna, ob odprtju zapri vsa polja
		če ni predplačila in računa lahko ureja predračun
		*/
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
*/
	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {

		/**
			*	Ce urejamo predracun, potem prikazemo sporocilo, kjer se uporabnik odloci, ali bo koncal
			*	urejanje predracuna in iz njega napravil
			*	predplacilni racun in koncni racun. Na voljo ima omenjeno opcijo (Yes, Da), samo shranjevanje predracuna
			*	prekinitev postopka in vec informacij o tem, kar odkrije bolj nazorno razlago moznih odgovorov.
			*	V ostalih primerih vprasanja ne postavimo, ampak samo shranimo v ustrezen zapis.
			*/
		int izbira = 0;
		if ( ui->rb_predracun->isChecked() ) {
			QMessageBox koncano;
			koncano.setIcon(QMessageBox::Question);
			koncano.setText("Zelite zakljuciti urejanje predracuna, ga shraniti in ustvariti predplacilni racun in racun?");
			koncano.setInformativeText("Za vec informacij kliknite Vec informacij!");
			koncano.setDetailedText("Da (Yes): konca urejanje predracuna, ga shrani, ustvari predplacilni racun in racun\n"
															"Ne (No): konca urejanje predracuna in ga shrani\n"
															"Prekini (Cancel): prekine delo in se vrne na formo za urejanje predracuna");
			koncano.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			koncano.setDefaultButton(QMessageBox::Yes);
			int ret = koncano.exec(); // nosi vrednost pritisnjenega gumba za nadaljno obravnavo

			switch (ret) {
				case QMessageBox::Yes:
					izbira = 1;
					break;
				case QMessageBox::No:
					izbira = 2;
					break;
				case QMessageBox::Cancel:
					izbira = 0;
					break;
				default:
					izbira = 0;
					break;
			}
		} // if predracun
		else {
			izbira = 2;
		} // else predracun

		if ( izbira > 0 ) {

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

				// doloci SQL query glede na stanje programa: vnesi nov vnos, popravi obstojeci vnos, kopiraj obstojeci vnos in vse pripadajoce opravke
				QSqlQuery sql_vnesi_projekt;
				QString stavek = "INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
												 "datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
												 "status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa) "
												 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
				if (ui->btn_sprejmi->text() == "Vnesi racun") { // nov vnos se neobstojecega (pred)racuna
					sql_vnesi_projekt.prepare(stavek);
				}
				else { // popravi ze obstojec vnos
					sql_vnesi_projekt.prepare("UPDATE racuni SET stevilka_racuna = ?, tip_racuna = ?, status_racuna = ?, stranka = ?, projekt = ?, "
																		"avtor_oseba = ?, datum_pricetka = ?, datum_konca = ?, datum_izdaje = ?, datum_placila = ?, "
																		"status_placila = ?, status_racunovodstva = ?, avans = ?, odstotek_avansa = ?, status_oddaje_racuna = ?, "
																		"datum_oddaje_racuna = ?, stara_stevilka_racuna = ?, sklic = ?, datum_placila_avansa = ? "
																		"WHERE id LIKE '" + ui->txt_id->text() + "'");
				}

				sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_racuna->text()));
				if ( ui->rb_predracun->isChecked() ) {
					sql_vnesi_projekt.bindValue(1, pretvori("1")); // predracun
				}
				else if ( ui->rb_predplacilo->isChecked() ) {
					sql_vnesi_projekt.bindValue(1, pretvori("2")); // predplacilo
				}
				else if ( ui->rb_racun->isChecked() ) {
					sql_vnesi_projekt.bindValue(1, pretvori("3")); // racun
				}
				else {
					sql_vnesi_projekt.bindValue(1, pretvori("")); // prazno
				}
				sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_status_predracuna->currentText()));
				sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_stranka_id->text()));
				sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_projekt_id->text()));
				sql_vnesi_projekt.bindValue(5, pretvori(vApp->id()));
				sql_vnesi_projekt.bindValue(6, pretvori(ui->txt_pricetek->text()));
				sql_vnesi_projekt.bindValue(7, pretvori(ui->txt_konec->text()));
				sql_vnesi_projekt.bindValue(8, pretvori(ui->txt_datum_izdaje_racuna->text()));
				sql_vnesi_projekt.bindValue(9, pretvori(ui->txt_rok_placila->text()));
				sql_vnesi_projekt.bindValue(10, pretvori(ui->txt_status_placila->currentText()));
				sql_vnesi_projekt.bindValue(11, pretvori(ui->txt_status_racunovodstva->currentText()));
				sql_vnesi_projekt.bindValue(12, pretvori(ui->txt_avans->text()));
				sql_vnesi_projekt.bindValue(13, pretvori(ui->txt_odstotek_avansa->text()));
				sql_vnesi_projekt.bindValue(14, pretvori(ui->txt_status_oddaje_racuna->currentText()));
				sql_vnesi_projekt.bindValue(15, pretvori(ui->txt_datum_oddaje_racuna->text()));
				sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
				sql_vnesi_projekt.bindValue(17, pretvori(ui->txt_sklic->text()));
				sql_vnesi_projekt.bindValue(18, pretvori(ui->txt_datum_placila_avansa->text()));

				sql_vnesi_projekt.exec();

				if ( izbira == 1 ) { // vnesi predplacilo in racun

					for ( int i = 2; i <=3; i++ ) {
						sql_vnesi_projekt.clear();

						sql_vnesi_projekt.prepare(stavek);

						sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_racuna->text()));
						sql_vnesi_projekt.bindValue(1, pretvori(QString::number(i, 10))); // predplacilo (2), racun (3)
						sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_status_predracuna->currentText()));
						sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_stranka_id->text()));
						sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_projekt_id->text()));
						sql_vnesi_projekt.bindValue(5, pretvori(vApp->id()));
						sql_vnesi_projekt.bindValue(6, pretvori(ui->txt_pricetek->text()));
						sql_vnesi_projekt.bindValue(7, pretvori(ui->txt_konec->text()));
						sql_vnesi_projekt.bindValue(8, pretvori(ui->txt_datum_izdaje_racuna->text()));
						sql_vnesi_projekt.bindValue(9, pretvori(ui->txt_rok_placila->text()));
						sql_vnesi_projekt.bindValue(10, pretvori(ui->txt_status_placila->currentText()));
						sql_vnesi_projekt.bindValue(11, pretvori(ui->txt_status_racunovodstva->currentText()));
						sql_vnesi_projekt.bindValue(12, pretvori(ui->txt_avans->text()));
						sql_vnesi_projekt.bindValue(13, pretvori(ui->txt_odstotek_avansa->text()));
						sql_vnesi_projekt.bindValue(14, pretvori(ui->txt_status_oddaje_racuna->currentText()));
						sql_vnesi_projekt.bindValue(15, pretvori(ui->txt_datum_oddaje_racuna->text()));
						sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
						sql_vnesi_projekt.bindValue(17, pretvori(ui->txt_sklic->text()));
						sql_vnesi_projekt.bindValue(18, pretvori(ui->txt_datum_placila_avansa->text()));

						sql_vnesi_projekt.exec();

						// kopira opravila iz predracuna v racun
						QSqlQuery sql_poisci_opravila;
						sql_poisci_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori("1") + "'");
						sql_poisci_opravila.exec();
						while ( sql_poisci_opravila.next() ) {
							QSqlQuery sql_kopiraj_opravila;
							sql_kopiraj_opravila.prepare("INSERT INTO opravila (stevilka_stranke, stevilka_projekta, stevilka_racuna, tip_racuna, opravilo_skupina, "
																				 "opravilo_storitev, urna_postavka_brez_ddv, urna_postavka_z_ddv, ddv, popust_fb1, popust_fb2, "
																				 "popust_komb1, popust_komb2, popust_stranka, popust_kupon, popust_akcija, podrazitev_vikend, "
																				 "podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, "
																				 "tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, znesek_koncni) "
																				 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
							sql_kopiraj_opravila.bindValue(0, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_stranke")).toString());
							sql_kopiraj_opravila.bindValue(1, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_projekta")).toString());
							sql_kopiraj_opravila.bindValue(2, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_racuna")).toString());
							sql_kopiraj_opravila.bindValue(3, pretvori(QString::number(i, 10))); // predplacilo (2), racun (3)
							sql_kopiraj_opravila.bindValue(4, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_skupina")).toString());
							sql_kopiraj_opravila.bindValue(5, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_storitev")).toString());
							sql_kopiraj_opravila.bindValue(6, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("urna_postavka_brez_ddv")).toString());
							sql_kopiraj_opravila.bindValue(7, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("urna_postavka_z_ddv")).toString());
							sql_kopiraj_opravila.bindValue(8, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ddv")).toString());
							sql_kopiraj_opravila.bindValue(9, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_fb1")).toString());
							sql_kopiraj_opravila.bindValue(10, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_fb2")).toString());
							sql_kopiraj_opravila.bindValue(11, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_komb1")).toString());
							sql_kopiraj_opravila.bindValue(12, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_komb2")).toString());
							sql_kopiraj_opravila.bindValue(13, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_stranka")).toString());
							sql_kopiraj_opravila.bindValue(14, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_kupon")).toString());
							sql_kopiraj_opravila.bindValue(15, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_akcija")).toString());
							sql_kopiraj_opravila.bindValue(16, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_vikend")).toString());
							sql_kopiraj_opravila.bindValue(17, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_hitrost")).toString());
							sql_kopiraj_opravila.bindValue(18, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_zapleti")).toString());
							sql_kopiraj_opravila.bindValue(19, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_vikend")).toString());
							sql_kopiraj_opravila.bindValue(20, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_hitrost")).toString());
							sql_kopiraj_opravila.bindValue(21, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_zapleti")).toString());
							sql_kopiraj_opravila.bindValue(22, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("tip_ur")).toString());
							sql_kopiraj_opravila.bindValue(23, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ur_dela")).toString());
							sql_kopiraj_opravila.bindValue(24, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("rocni_vnos_ur")).toString());
							sql_kopiraj_opravila.bindValue(25, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_popustov")).toString());
							sql_kopiraj_opravila.bindValue(26, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_ddv")).toString());
							sql_kopiraj_opravila.bindValue(27, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_koncni")).toString());
							sql_kopiraj_opravila.exec();
						} // while ( sql_poisci_opravila.next() )
					} // for ( int i = 2; i <= 3; i++ )
				} // if ( izbira == 1 )
			} // else ( base.isOpen() )

			base.close();

			// send signal to reload widget
			poslji("racuni");

			// close this window
			close();

		} // if izbira > 0

	} // if napaka
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	} // else napaka

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
		naslov1->setText("Skupina");
		naslov2->setText("Storitev");
		naslov3->setText("Stevilo ur dela");
		naslov4->setText("Urna postavka");
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
		ui->tbl_opravila->setColumnWidth(1, 100);
		ui->tbl_opravila->setColumnWidth(2, 300);
		ui->tbl_opravila->setColumnWidth(3, 100);
		ui->tbl_opravila->setColumnWidth(4, 100);
		ui->tbl_opravila->setColumnWidth(5, 100);
		ui->tbl_opravila->setColumnWidth(6, 100);
		ui->tbl_opravila->setColumnWidth(7, 100);

		QSqlQuery sql_fill;
		QString tip = "";
		if ( ui->rb_predracun->isChecked() ) {
			tip = "1";
		}
		else if ( ui->rb_predplacilo->isChecked() ) {
			tip = "2";
		}
		else if ( ui->rb_racun->isChecked() ) {
			tip = "3";
		}

		QSqlQuery sql_predracun;
		QString id_predracuna;
		sql_predracun.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stevilka_racuna->text()) +
													"' AND tip_racuna LIKE '" + pretvori("1") + "'");
		sql_predracun.exec();
		if ( sql_predracun.next() ) {
			id_predracuna = sql_predracun.value(sql_predracun.record().indexOf("id")).toString();
		}
		sql_predracun.clear();

		sql_fill.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + id_predracuna + "' AND tip_racuna LIKE '" + pretvori(tip) + "'");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_opravila->insertRow(row);
			ui->tbl_opravila->setRowHeight(row, 20);

			int col = 0;
			int i = 0;
			QString polja[8] = {"id", "opravilo_skupina", "opravilo_storitev", "ur_dela", "urna_postavka", "znesek_popustov", "znesek_ddv", "znesek_koncni"};

			while (col <= 7) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[col] == "opravilo_storitev" ) {
					QString storitev = "";
					storitev = prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString());
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("pribitek_vikend")).toString()) == "1" ) {
						storitev += " - Delo med vikendom";
					}
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("pribitek_hitrost")).toString()) == "1" ) {
						storitev += " - Prednostna obdelava";
					}
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("pribitek_zapleti")).toString()) == "1" ) {
						storitev += " - Zahtevna naloga";
					}
					celica->setText(storitev);
				}
				else if ( polja[col] == "urna_postavka" ) {
					double urna_postavka = 0.00;
					urna_postavka = prevedi(sql_fill.value(sql_fill.record().indexOf("urna_postavka_brez_ddv")).toString()).toDouble();
					double podrazitev = 0.0;
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("pribitek_vikend")).toString()) == "1" ) {
						podrazitev += prevedi(sql_fill.value(sql_fill.record().indexOf("podrazitev_vikend")).toString()).toDouble();
					}
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("pribitek_hitrost")).toString()) == "1" ) {
						podrazitev += prevedi(sql_fill.value(sql_fill.record().indexOf("podrazitev_hitrost")).toString()).toDouble();
					}
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("pribitek_zapleti")).toString()) == "1" ) {
						podrazitev += prevedi(sql_fill.value(sql_fill.record().indexOf("podrazitev_zapleti")).toString()).toDouble();
					}
					celica->setText(QString::number(urna_postavka * ( 1 + podrazitev / 100 ), 'f', 2).replace(".", ",") + " EUR");
				}
				else if ( polja[col] == "znesek_popustov" ) {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) + " EUR");
				}
				else if ( polja[col] == "znesek_ddv" ) {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) + " EUR");
				}
				else if ( polja[col] == "znesek_koncni" ) {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) + " EUR");
				}
				else {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				}
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

		ui->rb_predplacilo->setEnabled(false);
		ui->rb_predracun->setEnabled(false);
		ui->rb_racun->setEnabled(false);
		ui->rb_predracun->setChecked(true);

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
			sql_stranka.prepare("SELECT * FROM projekti WHERE stevilka_projekta LIKE '" + pretvori(besedilo.right(besedilo.length() - 9)) + "'");
			sql_stranka.exec();
			if ( sql_stranka.next() ) {
				ui->txt_stranka_id->setText(pretvori(sql_stranka.value(sql_stranka.record().indexOf("stranka")).toString()));
				ui->txt_projekt_id->setText(pretvori(sql_stranka.value(sql_stranka.record().indexOf("id")).toString()));
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

		ui->rb_predplacilo->setEnabled(false);
		ui->rb_predracun->setEnabled(false);
		ui->rb_racun->setEnabled(false);

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
				ui->txt_stranka_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString()));
				ui->txt_stevilka_racuna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_racuna")).toString()));
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("stara_stevilka_racuna")).toString()) != "" ) {
					ui->cb_stara_stevilka_racuna->setChecked(true);
					ui->txt_stara_stevilka_racuna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stara_stevilka_racuna")).toString()));
				}
				else {
					ui->cb_stara_stevilka_racuna->setChecked(false);
					ui->txt_stara_stevilka_racuna->setText("");
				}
				ui->txt_projekt_id->setText(pretvori(sql_napolni.value(sql_napolni.record().indexOf("projekt")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_racuna")).toString()) == "1") {
					ui->rb_predracun->setChecked(true);
				}
				else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_racuna")).toString()) == "2") {
					ui->rb_predplacilo->setChecked(true);
				}
				else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_racuna")).toString()) == "3") {
					ui->rb_racun->setChecked(true);
				}

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_pricetka")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_pricetek->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_konca")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_konec->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_izdaje_racuna->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_placila")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_rok_placila->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_oddaje_racuna")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_oddaje_racuna->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_placila_avansa")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_placila_avansa->setDate(datum);

				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM sif_status_placila WHERE status LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("status_placila")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_status_placila->setCurrentIndex(ui->txt_status_placila->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("status")).toString())));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_status_racunovodstva WHERE status LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("status_racunovodstva")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_status_racunovodstva->setCurrentIndex(ui->txt_status_racunovodstva->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("status")).toString())));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_status_predracuna WHERE status LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("status_racuna")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_status_predracuna->setCurrentIndex(ui->txt_status_predracuna->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("status")).toString())));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_status_oddaje_racuna WHERE status LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("status_oddaje_racuna")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_status_oddaje_racuna->setCurrentIndex(ui->txt_status_oddaje_racuna->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("status")).toString())));
				}
				sql_combo.clear();

				ui->txt_avans->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("avans")).toString()));
				ui->txt_odstotek_avansa->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("odstotek_avansa")).toString()));

				ui->txt_sklic->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("sklic")).toString()));
			}

			// onemogoci shranjevanje podatkov v predplacilo
			if ( ui->rb_predplacilo->isChecked() ) {
				ui->btn_sprejmi->setEnabled(false);
				ui->btn_opravilo->setEnabled(false);
				ui->btn_brisi_opravilo->setEnabled(false);
			}

			// ce obstaja racun in predplacilni racun, onemogoci shranjevanje podatkov v predracun
			if ( ui->rb_predracun->isChecked() ) {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stevilka_racuna->text()) +
													"' AND tip_racuna LIKE '" + pretvori("3") + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					ui->btn_sprejmi->setEnabled(false);
					ui->btn_opravilo->setEnabled(false);
					ui->btn_brisi_opravilo->setEnabled(false);
				}
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

		QString tip = "";
		if ( ui->rb_predracun->isChecked() ) {
			tip = "1";
		}
		else if ( ui->rb_predplacilo->isChecked() ) {
			tip = "2";
		}
		else if ( ui->rb_racun->isChecked() ) {
			tip = "3";
		}

		QSqlQuery sql_predracun;
		QString id_predracuna;
		sql_predracun.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stevilka_racuna->text()) +
													"' AND tip_racuna LIKE '" + pretvori("1") + "'");
		sql_predracun.exec();
		if ( sql_predracun.next() ) {
			id_predracuna = sql_predracun.value(sql_predracun.record().indexOf("id")).toString();
		}
		sql_predracun.clear();

		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + id_predracuna + "' AND tip_racuna LIKE '" + pretvori(tip) + "'");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			popusti = popusti + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_popustov")).toString()).toDouble();
			brezddv = brezddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_koncni")).toString()).toDouble();
			ddv = ddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_ddv")).toString()).toDouble();
		}
	}
	base.close();

	ui->txt_avans->setText(pretvori_iz_double(QString::number(pretvori_v_double(ui->txt_znesek->text()).toDouble() *
																														pretvori_v_double(ui->txt_odstotek_avansa->text()).toDouble() / 100, 'f', 2)) + " EUR");
	ui->txt_popusti->setText(pretvori_iz_double(QString::number(popusti, 'f', 2)) + " EUR");
	ui->txt_znesek_brez_ddv->setText(pretvori_iz_double(QString::number(brezddv, 'f', 2)) + " EUR");
	ui->txt_znesek_ddv->setText(pretvori_iz_double(QString::number(ddv, 'f', 2)) + " EUR");
	ui->txt_znesek->setText(pretvori_iz_double(QString::number(brezddv + ddv, 'f', 2)) + " EUR");
	ui->txt_se_placati->setText(pretvori_iz_double(QString::number(pretvori_v_double(ui->txt_znesek->text()).toDouble() -
																																 pretvori_v_double(ui->txt_avans->text()).toDouble(), 'f', 2)) + " EUR");

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

	QString tip_racuna = "";

	if ( ui->rb_predracun->isChecked() ) {
		tip_racuna = "1";
	}
	else if ( ui->rb_predplacilo->isChecked() ) {
		tip_racuna = "2";
	}
	else if ( ui->rb_racun->isChecked() ) {
		tip_racuna = "3";
	}

	opravila *uredi = new opravila;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Novo opravilo" + ui->txt_id->text() + tip_racuna); // ce opravila se ni, posljemo naprej id
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void racun::osvezi(QString beseda) {

	if ( beseda == "opravilo" ) {
		napolni();
		izracunaj();
	}

}

void racun::on_tbl_opravila_doubleClicked() {

	opravila *uredi = new opravila;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Staro opravilo" + ui->tbl_opravila->selectedItems().takeAt(0)->text()); // ce opravilo obstaja, posljemo naprej id opravila
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

QString racun::pretvori_v_double(QString besedilo) {

	besedilo.remove(" ");
	besedilo.remove("%");
	besedilo.remove("EUR");
	besedilo.replace(",", ".");

	if ( besedilo.left(1) == "0" ) {
		besedilo.remove(0,1);
	}
	if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0.0
		besedilo.append("0.0");
	}
	if ( besedilo.left(1) == "." ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
		besedilo.prepend("0");
	}
	if ( besedilo.right(1) == "." ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 0
		besedilo.append("0");
	}
	if ( !besedilo.contains(".") ) { // ce je celo stevilo dodamo decimalno locilo in vrednost 0
		besedilo.append(".0");
	}

	return besedilo;

}

QString racun::pretvori_iz_double(QString besedilo) {

	besedilo.replace(".",",");

	return besedilo;

}

void racun::on_rb_predracun_toggled() {

	if ( ui->rb_predracun->isChecked() ) {
		ui->txt_status_predracuna->setHidden(false);
		ui->label_10->setHidden(false);
		ui->txt_status_placila->setHidden(true);
		ui->label_6->setHidden(true);
		ui->txt_status_racunovodstva->setHidden(true);
		ui->label_7->setHidden(true);

		ui->txt_odstotek_avansa->setEnabled(true);

	}

}

void racun::on_rb_predplacilo_toggled() {

	if ( ui->rb_predplacilo->isChecked() ) {
		ui->txt_status_predracuna->setHidden(true);
		ui->label_10->setHidden(true);
		ui->txt_status_placila->setHidden(false);
		ui->label_6->setHidden(false);
		ui->txt_status_racunovodstva->setHidden(false);
		ui->label_7->setHidden(false);

		ui->txt_odstotek_avansa->setEnabled(false);

	}

}

void racun::on_rb_racun_toggled() {

	if ( ui->rb_racun->isChecked() ) {
		ui->txt_status_predracuna->setHidden(true);
		ui->label_10->setHidden(true);
		ui->txt_status_placila->setHidden(false);
		ui->label_6->setHidden(false);
		ui->txt_status_racunovodstva->setHidden(false);
		ui->label_7->setHidden(false);

		ui->txt_odstotek_avansa->setEnabled(false);

	}

}

void racun::on_txt_odstotek_avansa_editingFinished() {

	ui->txt_odstotek_avansa->setText(pretvori_iz_double(pretvori_v_double(ui->txt_odstotek_avansa->text())) + " %");

	ui->txt_avans->setText(pretvori_iz_double(QString::number(pretvori_v_double(ui->txt_znesek->text()).toDouble() *
																														pretvori_v_double(ui->txt_odstotek_avansa->text()).toDouble() / 100, 'f', 2)) + " EUR");

	izracunaj();

}

void racun::on_cb_stara_stevilka_racuna_toggled() {

	if ( ui->cb_stara_stevilka_racuna->isChecked() ) {
		ui->txt_stara_stevilka_racuna->setEnabled(true);
	}
	else {
		ui->txt_stara_stevilka_racuna->setEnabled(false);
	}

}

void racun::stevilka_racuna() {

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
		QString leto = ui->txt_datum_izdaje_racuna->text().right(4);
		QString mesec = ui->txt_datum_izdaje_racuna->text().left(5).right(2);
		QString dan = ui->txt_datum_izdaje_racuna->text().left(2);

		int max_st_racuna = 0;

		// izracunamo zaporedno stevilko racuna v tekocem letu
		QSqlQuery sql_stetje_racunov;
		sql_stetje_racunov.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%." + pretvori(leto) + "' AND tip_racuna LIKE '1' ORDER BY stevilka_racuna ASC");
		sql_stetje_racunov.exec();
		while ( sql_stetje_racunov.next() ) {
			int st_racuna = 0;
			st_racuna = prevedi(sql_stetje_racunov.value(sql_stetje_racunov.record().indexOf("stevilka_racuna")).toString()).right(3).toInt();
			if ( st_racuna > max_st_racuna ) {
				max_st_racuna = st_racuna;
			}
		}
		max_st_racuna = max_st_racuna + 1;
		QString st_racuna = QString::number(max_st_racuna, 10);

		// iz stevilke racuna ustvarimo tromestno stevilko, pretvorjeno v besedo
	//	racun = QString::number(i, 10);
		if ( st_racuna.length() == 1 ) {
			st_racuna = "00" + st_racuna;
		}
		else if ( st_racuna.length() == 2 ) {
			st_racuna = "0" + st_racuna;
		}

		// imamo dovolj podatkov za tvorbo stevilke racuna

		ui->txt_stevilka_racuna->setText(leto.right(2) + st_racuna);

		/**
			*	Tvorimo stevilko sklica
			*	1) Tvorimo nakljucna tri stevila, katerih matematicna operacija nam pove, ali je racun javen ali zaseben
			*	2) Zdruzimo podatke in nakljucna stevila
			*	3) Po modulu 11 izracunamo kontrolne stevilke in jih vnesemo v ustrezna polja
			*	4) Dodamo oznako modela in drzave
			*/

		// 2) Tvorimo nakljucna tri stevila, katerih matematicna operacija nam pove, ali je racun javen ali zaseben
		QTime polnoc = QTime::fromString("00:00:00:001", "hh:mm:ss:zzz");
		int cas = polnoc.msecsTo(QTime::currentTime());
		qsrand(cas);
		int	cifra_1 = qrand()%(9);

		polnoc = QTime::fromString("00:00:01:001", "hh:mm:ss:zzz");
		cas = polnoc.msecsTo(QTime::currentTime());
		qsrand(cas);
		int cifra_2 = qrand()%(9);

		int sestevek = 3 * cifra_1 + 2 * cifra_2;

		int ostanek = sestevek % 11;

		int kontrolna = 11 - ostanek;

		if ( kontrolna >= 9 ) {
			kontrolna = 0;
		}

		int cifra_3 = 9;
		if ( vApp->state() == pretvori("private") ) { // ker je dostop zaseben, imajo do njega pravico samo zaposleni
			QMessageBox dostop;
			dostop.setIcon(QMessageBox::Question);
			dostop.setText("Naj bo racun javno dostopen?");
			dostop.setInformativeText("Za vec informacij kliknite Vec informacij!");
			dostop.setDetailedText("Da (Yes): Racun je javno dostopen, vidijo ga lahko vsi, prikaze se na vseh tiskaninah\n"
															"Ne (No): Racun je zaseben, dostop do njega imajo samo zaposleni v ustreznem nacinu dela\n"
															"Prekini (Cancel): Se obravnava kot ne");
			dostop.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			dostop.setDefaultButton(QMessageBox::Yes);
			int ret = dostop.exec(); // nosi vrednost pritisnjenega gumba za nadaljno obravnavo

			switch (ret) {
				case QMessageBox::Yes:
					cifra_3 = kontrolna;
					break;
				case QMessageBox::No:
					cifra_3 = kontrolna + 1;
					break;
				case QMessageBox::Cancel:
					cifra_3 = kontrolna;
					break;
				default:
					cifra_3 = kontrolna;
					break;
			}
		}
		else { // javen dostop, vsi lahko vidijo ta racun
			cifra_3 = kontrolna;
		}

		// 2) Zdruzimo podatke in nakljucna stevila
		QString sklic = "";
		sklic += ui->txt_stevilka_racuna->text();
		sklic += QString::number(cifra_1, 10);
		sklic += dan;
		sklic += QString::number(cifra_2, 10);
		sklic += mesec;
		sklic += QString::number(cifra_3, 10);

		// 3) Po modulu 11 izracunamo kontrolne stevilke in jih vnesemo v ustrezna polja
		sestevek = 0;
		int n = sklic.length() + 1;

		for ( int i = 1; i <= sklic.length(); i++ ) {
			sestevek += n * sklic.left(i).right(1).toInt();
			n--;
		}

		ostanek = 0;
		ostanek = sestevek % 11;

		kontrolna = 0;
		kontrolna = 11 - ostanek;

		if ( kontrolna >= 10 ) {
			kontrolna = 0;
		}

		sklic += QString::number(kontrolna, 10);

		// 4) Dodamo oznako modela in drzave
		sklic = "SI12 " + sklic;

		ui->txt_sklic->setText(sklic);

	}
	base.close();

}

void racun::on_txt_datum_izdaje_racuna_dateChanged() {

	stevilka_racuna();

}

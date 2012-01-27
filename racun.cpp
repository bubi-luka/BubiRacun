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

		ui->txt_datum_izdaje_racuna->setDate(QDate::currentDate());
		ui->txt_rok_placila->setDate(QDate::currentDate());
		ui->txt_status_placila->clear();
		ui->txt_status_racunovodstva->clear();
		ui->txt_status_oddaje_racuna->clear();
		ui->txt_datum_oddaje_racuna->setDate(QDate::currentDate());

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

			// vnesi stevilko projekta
			QString leto = QDate::currentDate().toString("yyyy");
			int i = 1;
			QString stevilka = "";

			QSqlQuery sql_insert_stracuna;
			sql_insert_stracuna.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori("SR-" + leto) + "%'");
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
			ui->txt_stevilka_racuna->setText("SR-" + leto + "-" + stevilka);

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
// ne kopira opravil pri pretvorbi iz racuna v predracun
void racun::on_btn_sprejmi_clicked() {

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
				if ( prevedi(sql_iz_predracuna_v_racun.value(sql_iz_predracuna_v_racun.record().indexOf("tip_racuna")).toString()) == "1" ) {
					predracun = true;
				}
				else {
					predracun = false;
				}
			}

			// doloci SQL query glede na stanje programa: vnesi nov vnos, popravi obstojeci vnos, kopiraj obstojeci vnos in vse pripadajoce opravke
			QSqlQuery sql_vnesi_projekt;
			if (ui->btn_sprejmi->text() == "Vnesi racun") { // nov vnos se neobstojecega (pred)racuna
				sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
																	"datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, status_oddaje_racuna, "
																	"datum_oddaje_racuna) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else if ( predracun == false || ui->rb_predracun->isChecked() ) { // popravi ze obstojec vnos
				sql_vnesi_projekt.prepare("UPDATE racuni SET stevilka_racuna = ?, tip_racuna = ?, status_racuna = ?, stranka = ?, projekt = ?, "
																	"avtor_oseba = ?, datum_pricetka = ?, datum_konca = ?, datum_izdaje = ?, datum_placila = ?, "
																	"status_placila = ?, status_racunovodstva = ?, avans = ?, status_oddaje_racuna = ?, datum_oddaje_racuna = ? "
																	"WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			else if ( predracun == true && ui->rb_racun->isChecked() ) { // pretvori iz predracuna v racun
				/*
					* Vnos za racun ze obstaja. Preveri moznost, da se je iz predracuna naredil racun (obratno ne velja).
					* Ce je prislo do te spremembe, ne popravljaj ampak vnesi kot nov racun, starega pusti pri miru.
					* Prav tako kopiraj opravila kot del ze omenjenega racuna.
				*/

				// doloci novo stevilko racuna
				QString stara_stevilka = ui->txt_stevilka_racuna->text();
				QString leto = QDate::currentDate().toString("yyyy");
				int i = 1;
				QString stevilka = "";

				QSqlQuery sql_insert_stracuna;
				sql_insert_stracuna.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori("SR-" + leto) + "%'");
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
				ui->txt_stevilka_racuna->setText("SR-" + leto + "-" + stevilka);

				// pripravi SQL query za vnos
				sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
																	"datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, status_oddaje_racuna, "
																	"datum_oddaje_racuna) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

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
					sql_kopiraj_opravila.bindValue(0, pretvori(ui->txt_stevilka_racuna->text()));
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
			sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_racuna->text()));
			if ( ui->rb_predracun->isChecked() ) {
				sql_vnesi_projekt.bindValue(1, pretvori("1")); // predracun
			}
			else {
				sql_vnesi_projekt.bindValue(1, pretvori("2")); // racun
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
			sql_vnesi_projekt.bindValue(13, pretvori(ui->txt_status_oddaje_racuna->currentText()));
			sql_vnesi_projekt.bindValue(14, pretvori(ui->txt_datum_oddaje_racuna->text()));

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
		sql_fill.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "'");
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
				ui->txt_projekt_id->setText(pretvori(sql_napolni.value(sql_napolni.record().indexOf("projekt")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_racuna")).toString()) == "1") {
					ui->rb_predracun->setChecked(true);
				}
				else {
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
		sql_racun.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "'");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			popusti = popusti + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_popustov")).toString()).toDouble();
			brezddv = brezddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_koncni")).toString()).toDouble();
			ddv = ddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_ddv")).toString()).toDouble();
		}
	}
	base.close();

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

	opravila *uredi = new opravila;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Novo opravilo" + ui->txt_stevilka_racuna->text()); // ce opravila se ni, posljemo naprej st. racuna
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

	/*
	* ce je oznacen, potem delamo na predracunu, drugace pa na racunu
	*/

	if ( ui->rb_predracun->isChecked() ) {
		ui->txt_status_predracuna->setHidden(false);
		ui->label_10->setHidden(false);
		ui->txt_status_placila->setHidden(true);
		ui->label_6->setHidden(true);
		ui->txt_status_racunovodstva->setHidden(true);
		ui->label_7->setHidden(true);
	}
	else {
		ui->txt_status_predracuna->setHidden(true);
		ui->label_10->setHidden(true);
		ui->txt_status_placila->setHidden(false);
		ui->label_6->setHidden(false);
		ui->txt_status_racunovodstva->setHidden(false);
		ui->label_7->setHidden(false);
	}

}

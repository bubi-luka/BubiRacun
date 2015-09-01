#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QFile>
#include <QFileDialog>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QList>

#include "racun.h"
#include "ui_racun.h"
#include "wid_racuni.h"
#include "projekti.h"
#include "opravila.h"
#include "kodiranje.h"
#include "varnost.h"
#include "tiskanje.h"
#include "wid_dobropis.h"

racun::racun(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::racun)
{
	ui->setupUi(this);

		// pocisti polja
		ui->btn_sprejmi->setText("Odpiram");

		ui->lbl_storno->setText("");
		ui->txt_id->setText("");
		ui->txt_stevilka_racuna->setText("");
		ui->txt_stara_stevilka_racuna->setText("");
		ui->txt_stevilka_starsa->setText("");
		ui->txt_status_predracuna->clear();
		ui->txt_stranka_id->setText("");
		ui->txt_stranka->clear();
		ui->txt_projekt_id->setText("");
		ui->txt_projekt->clear();
		ui->txt_datum_pricetka->setDate(QDate::currentDate());
		ui->txt_datum_zakljucka->setDate(QDate::currentDate());

		ui->rb_fizicna->setText("");
		ui->rb_pravna->setText("");
		ui->rb_fizicna->setChecked(true);
		ui->rb_pravna->setChecked(false);

		ui->txt_banka->clear();
		ui->txt_bic->setText("");
		ui->txt_koda_namena->clear();
		ui->txt_koda_namena_avans->clear();

		ui->txt_avans->setText("");
		ui->txt_se_placati->setText("");
		ui->txt_popusti->setText("");
		ui->txt_znesek_brez_ddv->setText("");
		ui->txt_znesek_ddv->setText("");
		ui->txt_znesek->setText("");
		ui->txt_sklic->setText("");

		ui->txt_id_zapisa_2->setText("");
		ui->tbl_zapisi_2->clear();
		ui->txt_datum_zapisa_2->setDateTime(QDateTime::currentDateTime());
		ui->txt_naslov_zapisa_2->setText("");
		ui->txt_opis_zapisa_2->setPlainText("");

		ui->txt_datum_izdaje_racuna->setDate(QDate::currentDate().addDays(1));
		ui->txt_rok_placila->setDate(QDate::currentDate());
		ui->txt_status_placila->clear();
		ui->txt_status_racunovodstva->clear();
		ui->txt_status_oddaje_racuna->clear();
		ui->txt_datum_oddaje_racuna->setDate(QDate::currentDate());
		ui->txt_datum_placila_avansa->setDate(QDate::currentDate());
		ui->txt_datum_placila_racuna->setDate(QDate::currentDate());

		ui->txt_odstotek_avansa->setText("0,0 %");

		ui->tbl_opravila->clear();

		ui->tbl_vse_opombe->clear();
		ui->tbl_vnesene_opombe->clear();
		ui->txt_vse_opombe->setText("");
		ui->txt_vnesene_opombe->setText("");

		// onemogoci polja
		ui->txt_id->setEnabled(false);
//		ui->txt_stevilka_racuna->setEnabled(false);
		ui->txt_stevilka_starsa->setEnabled(false);
		ui->txt_sklic->setEnabled(false);
		ui->txt_projekt_id->setEnabled(false);
		ui->txt_stranka_id->setEnabled(false);
		ui->txt_znesek_ddv->setEnabled(false);
		ui->txt_popusti->setEnabled(false);
		ui->txt_se_placati->setEnabled(false);
		ui->txt_znesek->setEnabled(false);
		ui->txt_znesek_brez_ddv->setEnabled(false);
		ui->txt_avans->setEnabled(false);
		ui->txt_stara_stevilka_racuna->setEnabled(false);
		ui->txt_status_predracuna->setEnabled(false);
//		ui->txt_datum_oddaje_racuna->setEnabled(false);
		ui->txt_status_racunovodstva->setEnabled(false);
		// ui->txt_rok_placila->setEnabled(false);
		ui->txt_odstotek_avansa->setEnabled(false);
//		ui->txt_datum_placila_avansa->setEnabled(false);
//		ui->txt_datum_placila_racuna->setEnabled(false);
		ui->txt_datum_izdaje_racuna->setEnabled(false);
		ui->txt_status_placila->setEnabled(false);

		ui->btn_predplacilni_racun->setEnabled(false);
		ui->btn_racun->setEnabled(false);

		// odznaci kljukice
		ui->cb_stara_stevilka_racuna->setChecked(false);
		ui->cb_stevilka_starsa->setChecked(false);

		// skrij polja
		ui->txt_projekt_id->setVisible(false);
		ui->txt_stranka_id->setVisible(false);
		ui->txt_id_zapisa_2->setVisible(false);
		ui->txt_datum_placila_racuna->setVisible(false);
		ui->btn_racun->setVisible(false);
		ui->txt_vse_opombe->setHidden(true);
		ui->txt_vnesene_opombe->setHidden(true);

		ui->tab_racuni->removeTab(3);

		// napolni spustne sezname
			ui->txt_status_predracuna->addItem("");
			ui->txt_projekt->addItem("");
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

			sql_fill_combo.prepare("SELECT * FROM sif_opombe_pri_racunih");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text() + "," +
											prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ",");
			}
			sql_fill_combo.clear();

			ui->txt_banka->addItem("");
			sql_fill_combo.prepare("SELECT * FROM sif_banke ORDER BY ime_banke ASC");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_banka->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime_banke")).toString()));
			}
			sql_fill_combo.clear();

			ui->txt_koda_namena->addItem("");
			ui->txt_koda_namena_avans->addItem("");
			sql_fill_combo.prepare("SELECT * FROM sif_koda_namena ORDER BY koda ASC");
			sql_fill_combo.exec();
			while ( sql_fill_combo.next() ) {
				ui->txt_koda_namena->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("koda")).toString()) + " - " +
											 prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("opis_kratek")).toString()));
				ui->txt_koda_namena_avans->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("koda")).toString()) + " - " +
												   prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("opis_kratek")).toString()));
			}
			sql_fill_combo.clear();

		// nastavi privzete vrednosti
		ui->rb_predplacilo->setChecked(true);
		ui->rb_predracun->setChecked(true);
		ui->txt_status_placila->setCurrentIndex(0);
		ui->txt_status_racunovodstva->setCurrentIndex(0);

		ui->tab_racuni->setCurrentIndex(0);

		ui->btn_sprejmi->setText("Vnesi racun");

		napolni_vse_opombe();

}

racun::~racun()
{
	delete ui;
}

void racun::on_btn_izpisi_clicked() {

	if ( ui->txt_stevilka_racuna->text() != "" ) {
		// shranimo morebitne spremembe
		on_btn_sprejmi_clicked();

		// posljemo v tisk
		tiskanje *print = new tiskanje;
		print->show();
		QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
					 print , SLOT(prejem(QString, QString, QString)));
		tiskaj("izdani-racuni", ui->txt_id->text() + ",", "tisk"); // vrsta, id stevilke, format
	}
	else {
		QMessageBox sporocilo;
		sporocilo.setText("Racuna ne morete natisniti, dokler ga ne oznacite kot izdanega!");
		sporocilo.exec();
	}

}

void racun::on_btn_izhod_clicked() {

	close();

}

void racun::on_btn_racun_clicked() {

	QString stevilka_racuna_org = ui->txt_stevilka_racuna->text();

		// za potrebe vnosa racuna vnosno okno ustrezno oznacimo
		ui->rb_racun->setChecked(true);

		// na novo izracunamo stevilko racuna
//		stevilka_racuna();

		// doloci spremenljivke, ki jih ni v sami formi programa
		QString podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov,
				podjetje_naslov_stevilka, podjetje_naslov_posta, podjetje_naslov_postna_stevilka,
				podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv, podjetje_bic,
				podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip,
				izdajatelj_id, izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv, narocnik_id,
				narocnik_naziv, narocnik_naslov, narocnik_posta, narocnik_davcna;

				QSqlQuery sql_uporabnik;
				sql_uporabnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + vApp->id() + "'");
				sql_uporabnik.exec();
				if ( sql_uporabnik.next() ) {
					izdajatelj_id = sql_uporabnik.value(sql_uporabnik.record().indexOf("id")).toString();
					izdajatelj_ime = sql_uporabnik.value(sql_uporabnik.record().indexOf("ime")).toString();
					izdajatelj_priimek = sql_uporabnik.value(sql_uporabnik.record().indexOf("priimek")).toString();
					izdajatelj_naziv = sql_uporabnik.value(sql_uporabnik.record().indexOf("naziv")).toString();

					QSqlQuery sql_podjetje;
					sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + vApp->firm() + "'");
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
						podjetje_bic = pretvori(ui->txt_bic->text());
						podjetje_banka = pretvori(ui->txt_banka->currentText());
						podjetje_tekoci_racun = sql_podjetje.value(sql_podjetje.record().indexOf("tekoci_racun")).toString();
						if ( ui->rb_predracun->isChecked()) { // predracun
							podjetje_koda_namena = pretvori(ui->txt_koda_namena_avans->currentText().left(4));
						}
						else if ( ui->rb_racun->isChecked() ) { // racun
							podjetje_koda_namena = pretvori(ui->txt_koda_namena->currentText().left(4));
						}
						else { // predplacilni racun
							podjetje_koda_namena = "";
						}
						podjetje_logotip = sql_podjetje.value(sql_podjetje.record().indexOf("logotip")).toString();
					}
				}

				QSqlQuery sql_narocnik;
				sql_narocnik.prepare("SELECT * from stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
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

		// vnesemo podatke
		QSqlQuery sql_vnesi_projekt;
		sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
								  "datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
								  "status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa, opombe, "
								  "rok_placila, podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov_ulica, podjetje_naslov_stevilka, "
								  "podjetje_naslov_posta, podjetje_naslov_postna_stevilka, podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv, "
								  "podjetje_bic, podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip, izdajatelj_id, "
								  "izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv, narocnik_id, narocnik_naziv, narocnik_naslov, narocnik_posta, narocnik_davcna, "
								  "stevilka_starsa, stornacija) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
								  "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		sql_vnesi_projekt.bindValue(0, pretvori(""));
		sql_vnesi_projekt.bindValue(1, pretvori("3")); // predplacilo (2), racun (3)
		sql_vnesi_projekt.bindValue(2, pretvori(""));
		sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_stranka_id->text()));
		sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_projekt_id->text()));
		sql_vnesi_projekt.bindValue(5, pretvori(vApp->id()));
		sql_vnesi_projekt.bindValue(6, pretvori(ui->txt_datum_pricetka->text()));
		sql_vnesi_projekt.bindValue(7, pretvori(ui->txt_datum_zakljucka->text()));
		sql_vnesi_projekt.bindValue(8, pretvori(""));
		sql_vnesi_projekt.bindValue(9, pretvori(""));
		sql_vnesi_projekt.bindValue(10, pretvori(""));
		sql_vnesi_projekt.bindValue(11, pretvori(""));
		sql_vnesi_projekt.bindValue(12, pretvori(pretvori_v_double(ui->txt_avans->text())));
		sql_vnesi_projekt.bindValue(13, pretvori(pretvori_v_double(ui->txt_odstotek_avansa->text())));
		sql_vnesi_projekt.bindValue(14, pretvori(""));
		sql_vnesi_projekt.bindValue(15, pretvori(""));
		sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
		sql_vnesi_projekt.bindValue(17, pretvori(""));
		sql_vnesi_projekt.bindValue(18, pretvori(""));
		sql_vnesi_projekt.bindValue(19, pretvori(ui->txt_vnesene_opombe->text()));
		sql_vnesi_projekt.bindValue(20, pretvori(ui->txt_rok_placila->text()));
		sql_vnesi_projekt.bindValue(21, podjetje_id);
		sql_vnesi_projekt.bindValue(22, podjetje_kratki);
		sql_vnesi_projekt.bindValue(23, podjetje_polni);
		sql_vnesi_projekt.bindValue(24, podjetje_naslov);
		sql_vnesi_projekt.bindValue(25, podjetje_naslov_stevilka);
		sql_vnesi_projekt.bindValue(26, podjetje_naslov_posta);
		sql_vnesi_projekt.bindValue(27, podjetje_naslov_postna_stevilka);
		sql_vnesi_projekt.bindValue(28, podjetje_url);
		sql_vnesi_projekt.bindValue(29, podjetje_email);
		sql_vnesi_projekt.bindValue(30, podjetje_telefon);
		sql_vnesi_projekt.bindValue(31, podjetje_ddv);
		sql_vnesi_projekt.bindValue(32, podjetje_bic);
		sql_vnesi_projekt.bindValue(33, podjetje_banka);
		sql_vnesi_projekt.bindValue(34, podjetje_tekoci_racun);
		sql_vnesi_projekt.bindValue(35, podjetje_koda_namena);
		sql_vnesi_projekt.bindValue(36, podjetje_logotip);
		sql_vnesi_projekt.bindValue(37, izdajatelj_id);
		sql_vnesi_projekt.bindValue(38, izdajatelj_ime);
		sql_vnesi_projekt.bindValue(39, izdajatelj_priimek);
		sql_vnesi_projekt.bindValue(40, izdajatelj_naziv);
		sql_vnesi_projekt.bindValue(41, narocnik_id);
		sql_vnesi_projekt.bindValue(42, narocnik_naziv);
		sql_vnesi_projekt.bindValue(43, narocnik_naslov);
		sql_vnesi_projekt.bindValue(44, narocnik_posta);
		sql_vnesi_projekt.bindValue(45, narocnik_davcna);
		sql_vnesi_projekt.bindValue(46, ui->txt_id->text());
		sql_vnesi_projekt.bindValue(47, "0");
		sql_vnesi_projekt.exec();

		// poiscemo id pravkar vnesenega zapisa
		QSqlQuery sql_nov_id;
		QString nov_id = "";
		sql_nov_id.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori("3") + "' ORDER BY id ASC");
		sql_nov_id.exec();
		while ( sql_nov_id.next() ) {
			nov_id = prevedi(sql_nov_id.value(sql_nov_id.record().indexOf("id")).toString());
		}
		// kopira opravila iz predracuna v racun
		QSqlQuery sql_poisci_opravila;
		sql_poisci_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori("1") + "'");
		sql_poisci_opravila.exec();
		while ( sql_poisci_opravila.next() ) {
			QSqlQuery sql_kopiraj_opravila;
			sql_kopiraj_opravila.prepare("INSERT INTO opravila (stevilka_stranke, stevilka_projekta, stevilka_racuna, tip_racuna, opravilo_skupina, "
																	 "opravilo_storitev, urna_postavka_brez_ddv, urna_postavka_z_ddv, ddv, popust_fb1, popust_fb2, "
																	 "popust_komb1, popust_komb2, popust_stranka, popust_akcija, podrazitev_vikend, "
																	 "podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, "
																	 "tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, znesek_koncni, enota, opravilo_sklop, "
																	 "opravilo_rocno, vrstni_red, sifra) "
																	 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			sql_kopiraj_opravila.bindValue(0, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_stranke")).toString());
			sql_kopiraj_opravila.bindValue(1, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_projekta")).toString());
			sql_kopiraj_opravila.bindValue(2, pretvori(nov_id));
			sql_kopiraj_opravila.bindValue(3, pretvori("3")); // predplacilo (2), racun (3)
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
			sql_kopiraj_opravila.bindValue(14, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_akcija")).toString());
			sql_kopiraj_opravila.bindValue(15, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_vikend")).toString());
			sql_kopiraj_opravila.bindValue(16, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_hitrost")).toString());
			sql_kopiraj_opravila.bindValue(17, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_zapleti")).toString());
			sql_kopiraj_opravila.bindValue(18, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_vikend")).toString());
			sql_kopiraj_opravila.bindValue(19, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_hitrost")).toString());
			sql_kopiraj_opravila.bindValue(20, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_zapleti")).toString());
			sql_kopiraj_opravila.bindValue(21, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("tip_ur")).toString());
			sql_kopiraj_opravila.bindValue(22, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ur_dela")).toString());
			sql_kopiraj_opravila.bindValue(23, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("rocni_vnos_ur")).toString());
			sql_kopiraj_opravila.bindValue(24, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_popustov")).toString());
			sql_kopiraj_opravila.bindValue(25, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_ddv")).toString());
			sql_kopiraj_opravila.bindValue(26, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_koncni")).toString());
			sql_kopiraj_opravila.bindValue(27, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("enota")).toString());
			sql_kopiraj_opravila.bindValue(28, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_sklop")).toString());
			sql_kopiraj_opravila.bindValue(29, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_rocno")).toString());
			sql_kopiraj_opravila.bindValue(30, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("vrstni_red")).toString());
			sql_kopiraj_opravila.bindValue(31, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("sifra")).toString());
			sql_kopiraj_opravila.exec();
		}
		// izracunamo ddv avansa in ga vnesemo v novo nastali racun
		double znesek_ddv = 0.0;
		QSqlQuery sql_opravila;
		sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(nov_id) + "'");
		sql_opravila.exec();
		while ( sql_opravila.next() ) {
			znesek_ddv += prevedi(sql_opravila.value(sql_opravila.record().indexOf("znesek_ddv")).toString()).toDouble();
		}

		znesek_ddv = znesek_ddv * pretvori_v_double(ui->txt_odstotek_avansa->text()).toDouble() / 100;

		QSqlQuery sql_posodobi;
		sql_posodobi.prepare("UPDATE racuni SET avans_ddv = ? WHERE id LIKE '" + pretvori(nov_id) + "'");
		sql_posodobi.bindValue(0, pretvori(QString::number(znesek_ddv, 'f', 2)));
		sql_posodobi.exec();

		sql_posodobi.clear();
		sql_opravila.clear();

	// ponastavimo na predracun
	ui->rb_predracun->setChecked(true);
	ui->txt_stevilka_racuna->setText(stevilka_racuna_org);

	// onemogocimo gumb
	ui->btn_racun->setEnabled(false);

	// zapremo okno
	close();

}

void racun::on_btn_predplacilni_racun_clicked() {

	ui->txt_status_placila->setCurrentIndex(ui->txt_status_placila->findText("Pla", Qt::MatchStartsWith));

	QString stevilka_racuna_org = ui->txt_stevilka_racuna->text();

		// za potrebe vnosa racuna vnosno okno ustrezno oznacimo
		ui->rb_predplacilo->setChecked(true);

		// na novo izracunamo stevilko racuna
		stevilka_racuna();

		// doloci spremenljivke, ki jih ni v sami formi programa
		QString podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov,
				podjetje_naslov_stevilka, podjetje_naslov_posta, podjetje_naslov_postna_stevilka,
				podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv, podjetje_bic,
				podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip,
				izdajatelj_id, izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv, narocnik_id,
				narocnik_naziv, narocnik_naslov, narocnik_posta, narocnik_davcna;

				QSqlQuery sql_uporabnik;
				sql_uporabnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + vApp->id() + "'");
				sql_uporabnik.exec();
				if ( sql_uporabnik.next() ) {
					izdajatelj_id = sql_uporabnik.value(sql_uporabnik.record().indexOf("id")).toString();
					izdajatelj_ime = sql_uporabnik.value(sql_uporabnik.record().indexOf("ime")).toString();
					izdajatelj_priimek = sql_uporabnik.value(sql_uporabnik.record().indexOf("priimek")).toString();
					izdajatelj_naziv = sql_uporabnik.value(sql_uporabnik.record().indexOf("naziv")).toString();

					QSqlQuery sql_podjetje;
					sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + vApp->firm() + "'");
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
						podjetje_bic = pretvori(ui->txt_bic->text());
						podjetje_banka = pretvori(ui->txt_banka->currentText());
						podjetje_tekoci_racun = sql_podjetje.value(sql_podjetje.record().indexOf("tekoci_racun")).toString();
						if ( ui->rb_predracun->isChecked()) { // predracun
							podjetje_koda_namena = pretvori(ui->txt_koda_namena_avans->currentText().left(4));
						}
						else if ( ui->rb_racun->isChecked() || ui->rb_dobropis->isChecked() ) { // racun
							podjetje_koda_namena = pretvori(ui->txt_koda_namena->currentText().left(4));
						}
						else { // predplacilni racun
							podjetje_koda_namena = "";
						}
						podjetje_logotip = sql_podjetje.value(sql_podjetje.record().indexOf("logotip")).toString();
					}
				}

				QSqlQuery sql_narocnik;
				sql_narocnik.prepare("SELECT * from stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
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

		// vnesemo podatke
		QSqlQuery sql_vnesi_projekt;
		sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
								  "datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
								  "status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa, opombe, "
								  "rok_placila, podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov_ulica, podjetje_naslov_stevilka, "
								  "podjetje_naslov_posta, podjetje_naslov_postna_stevilka, podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv, "
								  "podjetje_bic, podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip, izdajatelj_id, "
								  "izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv, narocnik_id, narocnik_naziv, narocnik_naslov, narocnik_posta, narocnik_davcna, "
								  "stevilka_starsa) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
								  "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_racuna->text()));
		sql_vnesi_projekt.bindValue(1, pretvori("2")); // predplacilo (2), racun (3)
		sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_status_predracuna->currentText()));
		sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_stranka_id->text()));
		sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_projekt_id->text()));
		sql_vnesi_projekt.bindValue(5, pretvori(vApp->id()));
		sql_vnesi_projekt.bindValue(6, pretvori(ui->txt_datum_pricetka->text()));
		sql_vnesi_projekt.bindValue(7, pretvori(ui->txt_datum_zakljucka->text()));

		QDate datum = QDate::fromString(ui->txt_datum_placila_avansa->text(), "dd.MM.yyyy");
		QString mesec = datum.toString("MM");
		while ( datum.toString("MM") == mesec ) { // dokler smo v istem mesecu, dodamo en dan
			datum = datum.addDays(1);
		}
		datum = datum.addDays(-1); // zanka ena prevec
		sql_vnesi_projekt.bindValue(8, pretvori(datum.toString("dd.MM.yyyy")));

		sql_vnesi_projekt.bindValue(9, pretvori(ui->txt_rok_placila->text()));
		sql_vnesi_projekt.bindValue(10, pretvori(ui->txt_status_placila->currentText()));
		sql_vnesi_projekt.bindValue(11, pretvori(ui->txt_status_racunovodstva->currentText()));
		sql_vnesi_projekt.bindValue(12, pretvori(pretvori_v_double(ui->txt_avans->text())));
		sql_vnesi_projekt.bindValue(13, pretvori(pretvori_v_double(ui->txt_odstotek_avansa->text())));
		sql_vnesi_projekt.bindValue(14, pretvori(""));
		sql_vnesi_projekt.bindValue(15, pretvori(ui->txt_datum_oddaje_racuna->text()));
		sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
		sql_vnesi_projekt.bindValue(17, pretvori(ui->txt_sklic->text()));
		sql_vnesi_projekt.bindValue(18, pretvori(""));
		sql_vnesi_projekt.bindValue(19, pretvori(ui->txt_vnesene_opombe->text()));
		sql_vnesi_projekt.bindValue(20, pretvori(ui->txt_rok_placila->text()));
		sql_vnesi_projekt.bindValue(21, podjetje_id);
		sql_vnesi_projekt.bindValue(22, podjetje_kratki);
		sql_vnesi_projekt.bindValue(23, podjetje_polni);
		sql_vnesi_projekt.bindValue(24, podjetje_naslov);
		sql_vnesi_projekt.bindValue(25, podjetje_naslov_stevilka);
		sql_vnesi_projekt.bindValue(26, podjetje_naslov_posta);
		sql_vnesi_projekt.bindValue(27, podjetje_naslov_postna_stevilka);
		sql_vnesi_projekt.bindValue(28, podjetje_url);
		sql_vnesi_projekt.bindValue(29, podjetje_email);
		sql_vnesi_projekt.bindValue(30, podjetje_telefon);
		sql_vnesi_projekt.bindValue(31, podjetje_ddv);
		sql_vnesi_projekt.bindValue(32, podjetje_bic);
		sql_vnesi_projekt.bindValue(33, podjetje_banka);
		sql_vnesi_projekt.bindValue(34, podjetje_tekoci_racun);
		sql_vnesi_projekt.bindValue(35, podjetje_koda_namena);
		sql_vnesi_projekt.bindValue(36, podjetje_logotip);
		sql_vnesi_projekt.bindValue(37, izdajatelj_id);
		sql_vnesi_projekt.bindValue(38, izdajatelj_ime);
		sql_vnesi_projekt.bindValue(39, izdajatelj_priimek);
		sql_vnesi_projekt.bindValue(40, izdajatelj_naziv);
		sql_vnesi_projekt.bindValue(41, narocnik_id);
		sql_vnesi_projekt.bindValue(42, narocnik_naziv);
		sql_vnesi_projekt.bindValue(43, narocnik_naslov);
		sql_vnesi_projekt.bindValue(44, narocnik_posta);
		sql_vnesi_projekt.bindValue(45, narocnik_davcna);
		sql_vnesi_projekt.bindValue(46, ui->txt_id->text());
		sql_vnesi_projekt.exec();

		// poiscemo id pravkar vnesenega zapisa
		QSqlQuery sql_nov_id;
		QString nov_id = "";
		sql_nov_id.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stevilka_racuna->text()) +
											 "' AND tip_racuna LIKE '" + pretvori("2") + "'");
		sql_nov_id.exec();
		if ( sql_nov_id.next() ) {
			nov_id = prevedi(sql_nov_id.value(sql_nov_id.record().indexOf("id")).toString());
		}
		// kopira opravila iz predracuna v racun
		QSqlQuery sql_poisci_opravila;
		sql_poisci_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori("1") + "'");
		sql_poisci_opravila.exec();
		while ( sql_poisci_opravila.next() ) {
			QSqlQuery sql_kopiraj_opravila;
			sql_kopiraj_opravila.prepare("INSERT INTO opravila (stevilka_stranke, stevilka_projekta, stevilka_racuna, tip_racuna, opravilo_skupina, "
																	 "opravilo_storitev, urna_postavka_brez_ddv, urna_postavka_z_ddv, ddv, popust_fb1, popust_fb2, "
																	 "popust_komb1, popust_komb2, popust_stranka, popust_akcija, podrazitev_vikend, "
																	 "podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, "
																	 "tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, znesek_koncni, enota, opravilo_sklop, "
																	 "opravilo_rocno, vrstni_red, sifra) "
																	 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			sql_kopiraj_opravila.bindValue(0, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_stranke")).toString());
			sql_kopiraj_opravila.bindValue(1, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_projekta")).toString());
			sql_kopiraj_opravila.bindValue(2, pretvori(nov_id));
			sql_kopiraj_opravila.bindValue(3, pretvori("2")); // predplacilo (2), racun (3)
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
			sql_kopiraj_opravila.bindValue(14, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_akcija")).toString());
			sql_kopiraj_opravila.bindValue(15, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_vikend")).toString());
			sql_kopiraj_opravila.bindValue(16, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_hitrost")).toString());
			sql_kopiraj_opravila.bindValue(17, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_zapleti")).toString());
			sql_kopiraj_opravila.bindValue(18, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_vikend")).toString());
			sql_kopiraj_opravila.bindValue(19, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_hitrost")).toString());
			sql_kopiraj_opravila.bindValue(20, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_zapleti")).toString());
			sql_kopiraj_opravila.bindValue(21, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("tip_ur")).toString());
			sql_kopiraj_opravila.bindValue(22, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ur_dela")).toString());
			sql_kopiraj_opravila.bindValue(23, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("rocni_vnos_ur")).toString());
			sql_kopiraj_opravila.bindValue(24, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_popustov")).toString());
			sql_kopiraj_opravila.bindValue(25, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_ddv")).toString());
			sql_kopiraj_opravila.bindValue(26, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_koncni")).toString());
			sql_kopiraj_opravila.bindValue(27, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("enota")).toString());
			sql_kopiraj_opravila.bindValue(28, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_sklop")).toString());
			sql_kopiraj_opravila.bindValue(29, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_rocno")).toString());
			sql_kopiraj_opravila.bindValue(30, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("vrstni_red")).toString());
			sql_kopiraj_opravila.bindValue(31, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("sifra")).toString());
			sql_kopiraj_opravila.exec();
		}

		// izracunamo ddv avansa in ga vnesemo v novo nastali racun
		double znesek_ddv = 0.0;
		QSqlQuery sql_opravila;
		sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(nov_id) + "'");
		sql_opravila.exec();
		while ( sql_opravila.next() ) {
			znesek_ddv += prevedi(sql_opravila.value(sql_opravila.record().indexOf("znesek_ddv")).toString()).toDouble();
		}

		znesek_ddv = znesek_ddv * pretvori_v_double(ui->txt_odstotek_avansa->text()).toDouble() / 100;

		QSqlQuery sql_posodobi;
		sql_posodobi.prepare("UPDATE racuni SET avans_ddv = ? WHERE id LIKE '" + pretvori(nov_id) + "'");
		sql_posodobi.bindValue(0, pretvori(QString::number(znesek_ddv, 'f', 2)));
		sql_posodobi.exec();

		sql_posodobi.clear();
		sql_opravila.clear();

	// ponastavimo na predracun
	ui->rb_predracun->setChecked(true);
	ui->txt_stevilka_racuna->setText(stevilka_racuna_org);

	// onemogocimo gumb
	ui->btn_predplacilni_racun->setEnabled(false);

	// zapremo okno
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

// ne preverja obveznih polj
void racun::on_btn_sprejmi_clicked() {

	izracunaj();

	QString obstaja_racun = "NE";
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

			// doloci spremenljivke, ki jih ni v sami formi programa
			QString podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov,
					podjetje_naslov_stevilka, podjetje_naslov_posta, podjetje_naslov_postna_stevilka,
					podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv, podjetje_bic,
					podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip,
					izdajatelj_id, izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv, narocnik_id,
					narocnik_naziv, narocnik_naslov, narocnik_posta, narocnik_davcna, podjetje_maticna;

					QSqlQuery sql_uporabnik;
					sql_uporabnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + vApp->id() + "'");
					sql_uporabnik.exec();
					if ( sql_uporabnik.next() ) {
						izdajatelj_id = sql_uporabnik.value(sql_uporabnik.record().indexOf("id")).toString();
						izdajatelj_ime = sql_uporabnik.value(sql_uporabnik.record().indexOf("ime")).toString();
						izdajatelj_priimek = sql_uporabnik.value(sql_uporabnik.record().indexOf("priimek")).toString();
						izdajatelj_naziv = sql_uporabnik.value(sql_uporabnik.record().indexOf("naziv")).toString();

						QSqlQuery sql_podjetje;
						sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + vApp->firm() + "'");
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
							podjetje_bic = pretvori(ui->txt_bic->text());
							podjetje_banka = pretvori(ui->txt_banka->currentText());
							podjetje_tekoci_racun = sql_podjetje.value(sql_podjetje.record().indexOf("tekoci_racun")).toString();
							if ( ui->rb_predracun->isChecked()) { // predracun
								podjetje_koda_namena = pretvori(ui->txt_koda_namena_avans->currentText().left(4));
							}
							else if ( ui->rb_racun->isChecked() || ui->rb_dobropis->isChecked() ) { // racun
								podjetje_koda_namena = pretvori(ui->txt_koda_namena->currentText().left(4));
							}
							else { // predplacilni racun
								podjetje_koda_namena = "";
							}
							podjetje_logotip = sql_podjetje.value(sql_podjetje.record().indexOf("logotip")).toString();
							podjetje_maticna = sql_podjetje.value(sql_podjetje.record().indexOf("maticna_stevilka")).toString();
						}
					}

					QSqlQuery sql_narocnik;
					sql_narocnik.prepare("SELECT * from stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
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
							   narocnik_davcna = pretvori("SI") + sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString();
						   }
						   else {
							   narocnik_davcna = sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString();
						   }
						}
						narocnik_naslov = pretvori(prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("naslov")).toString()) + " " +
								prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("naslov_st")).toString()));
						narocnik_posta = pretvori(prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("postna_stevilka")).toString()) + " " +
								prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("posta")).toString()));
					}

			// doloci SQL query glede na stanje programa: vnesi nov vnos, popravi obstojeci vnos, kopiraj obstojeci vnos in vse pripadajoce opravke
			QSqlQuery sql_vnesi_projekt;
			if (ui->btn_sprejmi->text() == "Vnesi racun") { // nov vnos se neobstojecega (pred)racuna
				sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
										  "datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
										  "status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa, opombe, "
										  "rok_placila, podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov_ulica, podjetje_naslov_stevilka, "
										  "podjetje_naslov_posta, podjetje_naslov_postna_stevilka, podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv, "
										  "podjetje_bic, podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip, izdajatelj_id, "
										  "izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv, narocnik_id, narocnik_naziv, narocnik_naslov, narocnik_posta, "
										  "narocnik_davcna, stevilka_starsa, podjetje_maticna"
										  ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
										  "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojec vnos
				sql_vnesi_projekt.prepare("UPDATE racuni SET stevilka_racuna = ?, tip_racuna = ?, status_racuna = ?, stranka = ?, projekt = ?, "
										  "avtor_oseba = ?, datum_pricetka = ?, datum_konca = ?, datum_izdaje = ?, datum_placila = ?, "
										  "status_placila = ?, status_racunovodstva = ?, avans = ?, odstotek_avansa = ?, status_oddaje_racuna = ?, "
										  "datum_oddaje_racuna = ?, stara_stevilka_racuna = ?, sklic = ?, datum_placila_avansa = ?, opombe = ?, rok_placila = ?, "
										  "podjetje_id = ?, podjetje_kratki = ?, podjetje_polni = ?, podjetje_naslov_ulica = ?, "
										  "podjetje_naslov_stevilka = ?, podjetje_naslov_posta = ?, podjetje_naslov_postna_stevilka = ?, "
										  "podjetje_url = ?, podjetje_email = ?, podjetje_telefon = ?, podjetje_ddv = ?, podjetje_bic = ?, "
										  "podjetje_banka = ?, podjetje_tekoci_racun = ?, podjetje_koda_namena = ?, podjetje_logotip = ?, "
										  "izdajatelj_id = ?, izdajatelj_ime = ?, izdajatelj_priimek = ?, izdajatelj_naziv = ?, narocnik_id = ?, "
										  "narocnik_naziv = ?, narocnik_naslov = ?, narocnik_posta = ?, narocnik_davcna = ?, stevilka_starsa = ?, "
										  "podjetje_maticna = ? WHERE id LIKE '" + ui->txt_id->text() + "'");

				// izracunamo ddv avansa in ga vnesemo v novo nastali racun, a le na predracunu
				if ( ui->rb_predracun->isChecked() ) {
					double znesek_ddv = 0.0;
					QSqlQuery sql_opravila;
					sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "'");
					sql_opravila.exec();
					while ( sql_opravila.next() ) {
						znesek_ddv += prevedi(sql_opravila.value(sql_opravila.record().indexOf("znesek_ddv")).toString()).toDouble();
					}

					znesek_ddv = znesek_ddv * pretvori_v_double(ui->txt_odstotek_avansa->text()).toDouble() / 100;

					QSqlQuery sql_posodobi;
					sql_posodobi.prepare("UPDATE racuni SET avans_ddv = ? WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
					sql_posodobi.bindValue(0, pretvori(QString::number(znesek_ddv, 'f', 2)));
					sql_posodobi.exec();

					sql_posodobi.clear();
					sql_opravila.clear();
				}

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
			else if ( ui->rb_dobropis->isChecked() ) {
				sql_vnesi_projekt.bindValue(1, pretvori("4")); // dobropis
			}
			else {
				sql_vnesi_projekt.bindValue(1, pretvori("")); // prazno
			}
			sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_status_predracuna->currentText()));
			sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_stranka_id->text()));
			sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_projekt_id->text()));
			sql_vnesi_projekt.bindValue(5, pretvori(vApp->id()));
			sql_vnesi_projekt.bindValue(6, pretvori(ui->txt_datum_pricetka->text()));
			sql_vnesi_projekt.bindValue(7, pretvori(ui->txt_datum_zakljucka->text()));
			if ( ui->txt_stevilka_racuna->text() == "" && ui->txt_stara_stevilka_racuna->text() == "" ) { // racun je izdan
				sql_vnesi_projekt.bindValue(8, "");
			}
			else { // racun ni izdan
				sql_vnesi_projekt.bindValue(8, pretvori(ui->txt_datum_izdaje_racuna->text()));
			}
			sql_vnesi_projekt.bindValue(9, pretvori(ui->txt_datum_placila_racuna->text()));
			sql_vnesi_projekt.bindValue(10, pretvori(ui->txt_status_placila->currentText()));
			sql_vnesi_projekt.bindValue(11, pretvori(ui->txt_status_racunovodstva->currentText()));
			sql_vnesi_projekt.bindValue(12, pretvori(pretvori_v_double(ui->txt_avans->text())));
			sql_vnesi_projekt.bindValue(13, pretvori(pretvori_v_double(ui->txt_odstotek_avansa->text())));
			sql_vnesi_projekt.bindValue(14, pretvori(ui->txt_status_oddaje_racuna->currentText()));
			sql_vnesi_projekt.bindValue(15, pretvori(ui->txt_datum_oddaje_racuna->text()));
			sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
			sql_vnesi_projekt.bindValue(17, pretvori(ui->txt_sklic->text()));
			if ( ui->rb_predracun->isChecked() ) {
				sql_vnesi_projekt.bindValue(18, pretvori(ui->txt_datum_placila_avansa->text()));
			}
			else {
				sql_vnesi_projekt.bindValue(18, pretvori(""));
			}
			sql_vnesi_projekt.bindValue(19, pretvori(ui->txt_vnesene_opombe->text()));
			sql_vnesi_projekt.bindValue(20, pretvori(ui->txt_rok_placila->text()));

			sql_vnesi_projekt.bindValue(21, podjetje_id);
			sql_vnesi_projekt.bindValue(22, podjetje_kratki);
			sql_vnesi_projekt.bindValue(23, podjetje_polni);
			sql_vnesi_projekt.bindValue(24, podjetje_naslov);
			sql_vnesi_projekt.bindValue(25, podjetje_naslov_stevilka);
			sql_vnesi_projekt.bindValue(26, podjetje_naslov_posta);
			sql_vnesi_projekt.bindValue(27, podjetje_naslov_postna_stevilka);
			sql_vnesi_projekt.bindValue(28, podjetje_url);
			sql_vnesi_projekt.bindValue(29, podjetje_email);
			sql_vnesi_projekt.bindValue(30, podjetje_telefon);
			sql_vnesi_projekt.bindValue(31, podjetje_ddv);
			sql_vnesi_projekt.bindValue(32, podjetje_bic);
			sql_vnesi_projekt.bindValue(33, podjetje_banka);
			sql_vnesi_projekt.bindValue(34, podjetje_tekoci_racun);
			sql_vnesi_projekt.bindValue(35, podjetje_koda_namena);
			sql_vnesi_projekt.bindValue(36, podjetje_logotip);
			sql_vnesi_projekt.bindValue(37, izdajatelj_id);
			sql_vnesi_projekt.bindValue(38, izdajatelj_ime);
			sql_vnesi_projekt.bindValue(39, izdajatelj_priimek);
			sql_vnesi_projekt.bindValue(40, izdajatelj_naziv);
			sql_vnesi_projekt.bindValue(41, narocnik_id);
			sql_vnesi_projekt.bindValue(42, narocnik_naziv);
			sql_vnesi_projekt.bindValue(43, narocnik_naslov);
			sql_vnesi_projekt.bindValue(44, narocnik_posta);
			sql_vnesi_projekt.bindValue(45, narocnik_davcna);
			sql_vnesi_projekt.bindValue(46, ui->txt_stevilka_starsa->text());
			sql_vnesi_projekt.bindValue(47, podjetje_maticna);

			sql_vnesi_projekt.exec();

			// zapisi stevilko starsa v racun in predplacilni racun
			if ( !ui->rb_predracun->isChecked() && ui->txt_id->text() != "" ) {
				QSqlQuery sql_poisci_otroke;
				sql_poisci_otroke.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
				sql_poisci_otroke.exec();
				while ( sql_poisci_otroke.next() ) {
					QSqlQuery sql_spremeni_datum;
					sql_spremeni_datum.prepare("UPDATE racuni SET datum_placila_avansa = ? WHERE id LIKE '" +
																		 sql_poisci_otroke.value(sql_poisci_otroke.record().indexOf("stevilka_starsa")).toString() + "'");
					sql_spremeni_datum.bindValue(0, pretvori(ui->txt_datum_placila_avansa->text()));
					sql_spremeni_datum.exec();
					sql_spremeni_datum.clear();
				} // while ( sql_poisci_otroke.next() )
			} // if ( ui->rb_predracun->isChecked() && ui->txt_id->text() != "" )

			QSqlQuery sql_preveri_obstoj_racuna;
			sql_preveri_obstoj_racuna.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori("3") + "' AND stevilka_starsa LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_preveri_obstoj_racuna.exec();
			if ( sql_preveri_obstoj_racuna.next() ) { // predracun ima racun
			   obstaja_racun = "DA";
			}

		// send signal to reload widget
		poslji("racuni");

		// close this window
		close();

		// ce je status predracuna potrjen (predracun je placan), potem tvori racun (t.i. zacasni racun)
		if ( ui->rb_predracun->isChecked() && ui->txt_status_predracuna->currentText() == "Potrjen" && ui->txt_status_predracuna->isEnabled() && obstaja_racun == "NE" ) {
			on_btn_racun_clicked();
			QMessageBox sporocilo;
			sporocilo.setText("Tvorim racun");
			sporocilo.exec();
		}

	} // if napaka
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	} // else napaka

}

void racun::napolni() {

	ui->tbl_opravila->setSortingEnabled(false);

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
		naslov3->setText("Koncni znesek");
		naslov4->setText("Stevilo ur dela");
		naslov5->setText("Urna postavka");
		naslov6->setText("Popusti");
		naslov7->setText("DDV");

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
		else if ( ui->rb_dobropis->isChecked() ) {
			tip = "4";
		}

		sql_fill.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori(tip) +
						 "' ORDER BY vrstni_red ASC");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_opravila->insertRow(row);
			ui->tbl_opravila->setRowHeight(row, 20);

			int col = 0;
			int i = 0;
			QString polja[8] = {"id", "opravilo_skupina", "opravilo_storitev", "znesek_koncni", "ur_dela", "urna_postavka", "znesek_popustov", "znesek_ddv"};

			while (col <= 7) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "id" ) {
					celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
				}
				else if ( polja[col] == "opravilo_storitev" ) {
					QString storitev = "";
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("opravilo_rocno")).toString()) == "" ) {
						storitev = prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString());
					}
					else {
						storitev = prevedi(sql_fill.value(sql_fill.record().indexOf("opravilo_rocno")).toString());
					}
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
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ",") + " EUR");
				}
				else if ( polja[col] == "znesek_ddv" ) {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ",") + " EUR");
				}
				else if ( polja[col] == "znesek_koncni" ) {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ",") + " EUR");
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

void racun::prejem(QString besedilo) {

	if (besedilo.left(9) == "Nov racun") {
		ui->btn_sprejmi->setText("Vnesi racun");
		ui->btn_racun->setEnabled(false);
		ui->btn_predplacilni_racun->setEnabled(false);
		ui->txt_datum_izdaje_racuna->setDate(QDate::currentDate());
		ui->btn_izpisi->setEnabled(false);
		ui->tbl_opravila->setEnabled(false);
		ui->btn_opravilo->setEnabled(false);
		ui->btn_brisi_opravilo->setEnabled(false);

		if ( besedilo == "Nov racun4" ) {
			ui->rb_dobropis->setChecked(true);
		}
		else {
			ui->rb_predracun->setChecked(true);
		}

		ui->txt_status_oddaje_racuna->setEnabled(false);

		// from projekt id get stranka id
			QSqlQuery sql_stranka;
			sql_stranka.prepare("SELECT * FROM projekti WHERE stevilka_projekta LIKE '" + pretvori(besedilo.right(besedilo.length() - 9)) + "'");
			sql_stranka.exec();
			if ( sql_stranka.next() ) {
				ui->txt_stranka_id->setText(pretvori(sql_stranka.value(sql_stranka.record().indexOf("stranka")).toString()));
				ui->txt_projekt_id->setText(pretvori(sql_stranka.value(sql_stranka.record().indexOf("id")).toString()));
			}

			QSqlQuery sql_podjetje;
			sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + pretvori(vApp->firm()) + "'");
			sql_podjetje.exec();
			if ( sql_podjetje.next() ) {
				ui->txt_banka->setCurrentIndex(ui->txt_banka->findText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("banka")).toString())));
				ui->txt_bic->setText((prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("bic")).toString())));
				ui->txt_koda_namena_avans->setCurrentIndex(ui->txt_koda_namena_avans->findText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("koda_namena_avans")).toString()) + " - ", Qt::MatchStartsWith));
				ui->txt_koda_namena->setCurrentIndex(ui->txt_koda_namena->findText(prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("koda_namena")).toString()) + " - ", Qt::MatchStartsWith));
			}
	}
	else {
		ui->btn_sprejmi->setText("Odpiram");
		ui->btn_izpisi->setEnabled(true);
		ui->tbl_opravila->setEnabled(true);
		ui->btn_opravilo->setEnabled(true);
		ui->btn_brisi_opravilo->setEnabled(true);

		ui->btn_racun->setEnabled(false);
		ui->btn_predplacilni_racun->setEnabled(false);

		// besedilo nosi ID ze obstojeco stranko, potrebno je napolniti polja
			QSqlQuery sql_napolni;
			sql_napolni.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(besedilo) + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));

				// ce je racun storniran, vnesi lbl_storno napis STORNIRANO v rdecih crkah
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("stornacija")).toString()) == "1" ) {
					QPalette storno;
					storno.setColor(QPalette::WindowText, QColor(255,0,0)); // rdeca
					ui->lbl_storno->setPalette(storno);
					ui->lbl_storno->setText("STORNIRANO");
				}
				else {
					QPalette storno;
					storno.setColor(QPalette::WindowText, QColor(0,0,0)); // crna
					ui->lbl_storno->setPalette(storno);
					ui->lbl_storno->setText("");
				}

				// pripravi tip stranke, da bodo prave vrednosti v samem spustnem seznamu
				QSqlQuery sql_stranka;
				sql_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString() + "'");
				sql_stranka.exec();
				if ( sql_stranka.next() ) {
					if ( sql_stranka.value(sql_stranka.record().indexOf("tip")).toString() == "1" ) { // fizicna
						ui->rb_fizicna->setChecked(true);
						ui->rb_pravna->setChecked(false);
					}
					else { // pravna
						ui->rb_fizicna->setChecked(false);
						ui->rb_pravna->setChecked(true);
					}
				}
				sql_stranka.clear();

				ui->txt_stranka_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString()));
				ui->txt_stevilka_racuna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_racuna")).toString()));
				ui->txt_stevilka_starsa->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_starsa")).toString()));
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
				else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_racuna")).toString()) == "4") {
					ui->rb_dobropis->setChecked(true);
				}

				ui->txt_banka->setCurrentIndex(ui->txt_banka->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podjetje_banka")).toString())));
				ui->txt_bic->setText((prevedi(sql_napolni.value(sql_napolni.record().indexOf("podjetje_bic")).toString())));
				ui->txt_koda_namena_avans->setCurrentIndex(ui->txt_koda_namena_avans->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podjetje_koda_namena")).toString()) + " - ", Qt::MatchStartsWith));
				ui->txt_koda_namena->setCurrentIndex(ui->txt_koda_namena->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podjetje_koda_namena")).toString()) + " - ", Qt::MatchStartsWith));

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_pricetka")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_pricetka->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_konca")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_zakljucka->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_izdaje_racuna->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("rok_placila")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_rok_placila->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_oddaje_racuna")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_oddaje_racuna->setDate(datum);
				datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_placila")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_datum_placila_racuna->setDate(datum);

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()) == "" ) {
					ui->txt_datum_izdaje_racuna->setDate(QDate::currentDate());
				}

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

				ui->txt_avans->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("avans")).toString())) + " EUR");
				ui->txt_odstotek_avansa->setText(pretvori_iz_double(prevedi(pretvori_iz_double(sql_napolni.value(sql_napolni.record().indexOf("odstotek_avansa")).toString()))) + " %");

				ui->txt_sklic->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("sklic")).toString()));
				ui->txt_vnesene_opombe->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opombe")).toString()));

				// napolni datum placila avansa
				if ( sql_napolni.value(sql_napolni.record().indexOf("stevilka_starsa")).toString() == "" ) {
					datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_placila_avansa")).toString()), "dd'.'MM'.'yyyy");
					ui->txt_datum_placila_avansa->setDate(datum);
				}
				else {
					QSqlQuery sql_avans;
					sql_avans.prepare("SELECT * FROM racuni WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("stevilka_starsa")).toString() + "'");
					sql_avans.exec();
					if ( sql_avans.next() ) {
						datum = QDate::fromString(prevedi(sql_avans.value(sql_avans.record().indexOf("datum_placila_avansa")).toString()), "dd'.'MM'.'yyyy");
						ui->txt_datum_placila_avansa->setDate(datum);
					}
				}
			}

			// odpri izdelavo racuna in predplacilnega racuna, ce so pogoji za to ustrezni
			if ( ui->txt_status_predracuna->currentText() == "Potrjen" ) {
				ui->btn_racun->setEnabled(true);
				ui->btn_predplacilni_racun->setEnabled(true);
			}
			else {
				ui->btn_racun->setEnabled(false);
				ui->btn_predplacilni_racun->setEnabled(false);
			}

			// onemogoci izdajo racuna, ce le-ta nima opravil
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
			else if ( ui->rb_dobropis->isChecked() ) {
				tip_racuna = "4";
			}
			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) +
													 "' AND tip_racuna LIKE '" + pretvori(tip_racuna) + "'");
			sql_opravila.exec();
			if ( !sql_opravila.next() ) {
				ui->txt_status_oddaje_racuna->setEnabled(false);
			}
			else {
				ui->txt_status_oddaje_racuna->setEnabled(true);
			}

			// onemogoci shranjevanje podatkov v predplacilo
			if ( ui->rb_predplacilo->isChecked() ) {
//				ui->txt_stevilka_racuna->setEnabled(false);
				ui->cb_stara_stevilka_racuna->setEnabled(false);
				ui->txt_status_predracuna->setEnabled(false);
				ui->txt_stranka->setEnabled(false);
				ui->txt_projekt->setEnabled(false);
				ui->txt_datum_pricetka->setEnabled(false);
				ui->txt_datum_zakljucka->setEnabled(false);

				ui->txt_sklic->setEnabled(false);
				ui->txt_datum_izdaje_racuna->setEnabled(false);
				// ui->txt_rok_placila->setEnabled(false);
				ui->txt_status_oddaje_racuna->setEnabled(false);
				ui->txt_status_placila->setEnabled(false);
				ui->txt_status_racunovodstva->setEnabled(true);		//
				ui->txt_status_oddaje_racuna->setEnabled(true);		//
//				ui->txt_datum_oddaje_racuna->setEnabled(true);		//
				ui->txt_odstotek_avansa->setEnabled(false);
				ui->txt_datum_placila_avansa->setEnabled(false);

				ui->btn_izracunaj->setEnabled(false);
				ui->btn_sprejmi->setEnabled(true);								//
				ui->btn_opravilo->setEnabled(false);
				ui->btn_brisi_opravilo->setEnabled(false);

				// zapri celo polje o placilu
				ui->tab_racuni->removeTab(1);
			}
			// onemogoci polja pri racunu
			if ( ui->rb_racun->isChecked() ) {
				ui->txt_datum_placila_avansa->setEnabled(false);
			}

			// ce obstaja racun in predplacilni racun, onemogoci shranjevanje podatkov v predracun
			if ( ui->rb_predracun->isChecked() ) {

				// preverimo, ali za dan predracun obstaja kaksen racun
				QSqlQuery sql_preveri_starse;
				sql_preveri_starse.prepare("SELECT * FROM racuni WHERE stevilka_starsa LIKE '" + pretvori(ui->txt_id->text()) + "'");
				sql_preveri_starse.exec();
				sql_preveri_starse.clear();

				int i = 0;

				while ( sql_preveri_starse.next() ) {
//					ui->txt_stevilka_racuna->setEnabled(false);
					ui->cb_stara_stevilka_racuna->setEnabled(false);
					ui->txt_status_predracuna->setEnabled(false);
					ui->txt_stranka->setEnabled(false);
					ui->txt_projekt->setEnabled(false);
					ui->txt_datum_pricetka->setEnabled(false);
					ui->txt_datum_zakljucka->setEnabled(false);

					ui->txt_sklic->setEnabled(false);
					ui->txt_datum_izdaje_racuna->setEnabled(false);
					// ui->txt_rok_placila->setEnabled(false);
					ui->txt_status_oddaje_racuna->setEnabled(false);
					ui->txt_status_placila->setEnabled(false);
					ui->txt_status_racunovodstva->setEnabled(true);		//
//					ui->txt_datum_oddaje_racuna->setEnabled(false);
					ui->txt_odstotek_avansa->setEnabled(false);
				//		ui->txt_datum_placila_avansa->setEnabled(false);

					ui->btn_izracunaj->setEnabled(false);
				//		ui->btn_sprejmi->setEnabled(false);
					ui->btn_opravilo->setEnabled(false);
					ui->btn_brisi_opravilo->setEnabled(false);

					ui->btn_racun->setEnabled(true);
					ui->btn_predplacilni_racun->setEnabled(true);

					if ( prevedi(sql_preveri_starse.value(sql_preveri_starse.record().indexOf("tip_racuna")).toString()) == "3" ) {
						ui->btn_racun->setEnabled(false);
						i++;
					}
					if ( prevedi(sql_preveri_starse.value(sql_preveri_starse.record().indexOf("tip_racuna")).toString()) == "2" ) {
						ui->btn_predplacilni_racun->setEnabled(false);
						i++;
					}
					if ( i == 2 ) {
						ui->btn_racun->setEnabled(false);
						ui->btn_predplacilni_racun->setEnabled(false);
					}
				}
			}

		// preveri polje avans in glede na to polje uskladi z avansom povezana polja
		if ( ui->txt_odstotek_avansa->text() == "0,0 %" ) {
			ui->txt_datum_placila_avansa->setHidden(true);
			ui->lbl_avans_placan->setHidden(true);
		}
		else if ( ui->txt_odstotek_avansa->text() == "100,0 %" ) {
			ui->txt_datum_placila_racuna->setEnabled(false);
		}
		else {
			ui->txt_datum_placila_avansa->setHidden(false);
			ui->lbl_avans_placan->setHidden(false);
		}

		napolni();

		napolni_zapise();

		izracunaj(); // calculate the values

		ui->btn_sprejmi->setText("Popravi vnos");

		QString opomba = ui->txt_vnesene_opombe->text();

		QString nova_opomba = opomba + ",";
		QString id = "";
		if ( opomba != "" ) { // txt_vnesene_opombe ni prazen
			for ( int a = 0; a <= opomba.count(",,"); a++ ) { // zavrti zanko za vsak id
				id = nova_opomba.left(nova_opomba.indexOf(",,", 0)) + ",";
				nova_opomba = nova_opomba.right(nova_opomba.length() - id.length());
				ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text().remove(id));
			}
		}

	}

	izracunaj();

	napolni_vnesene_opombe();
	napolni_vse_opombe();

	// pri predplacilnem racunu nastavi status kot Placan
   // ui->txt_status_placila->setCurrentIndex(ui->txt_status_placila->findText("Pla", Qt::MatchStartsWith));

	/*
	if ( ui->txt_status_predracuna->currentText() == "Potrjen" ) {
		ui->btn_racun->setEnabled(true);
		ui->btn_predplacilni_racun->setEnabled(true);
	}
	else {
		ui->btn_racun->setEnabled(false);
		ui->btn_predplacilni_racun->setEnabled(false);
	}
	*/

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
		else if ( ui->rb_dobropis->isChecked() ) {
			tip = "4";
		}

		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori(tip) +
						  "' ORDER BY 'vrstni_red' ASC");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			popusti = popusti + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_popustov")).toString()).toDouble();
			brezddv = brezddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_koncni")).toString()).toDouble();
			ddv = ddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_ddv")).toString()).toDouble();
		}

	if ( !ui->rb_racun->isChecked() ) { // izvzamemo racun, da v primeru sprememb ne spreminjamo vrednosti ze placanega avansa
		ui->txt_avans->setText(pretvori_iz_double(QString::number(pretvori_v_double(ui->txt_znesek->text()).toDouble() *
																															pretvori_v_double(ui->txt_odstotek_avansa->text()).toDouble() / 100, 'f', 2)) + " EUR");
	}
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

		QSqlQuery sql_brisi;
		sql_brisi.prepare("DELETE FROM opravila WHERE id LIKE '" + id + "'");
		sql_brisi.exec();

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
	else if ( ui->rb_dobropis->isChecked() ) {
		tip_racuna = "4";
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
		vnesi_vrstni_red();
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
		ui->lbl_status_predracuna->setHidden(false);
		ui->txt_status_placila->setHidden(true);
		ui->lbl_status_placila->setHidden(true);
		ui->txt_status_racunovodstva->setHidden(false);
		ui->lbl_status_racunovodstva->setHidden(false);

		ui->txt_odstotek_avansa->setEnabled(true);
		ui->txt_datum_pricetka->setHidden(true);
		ui->lbl_datum_pricetka->setHidden(true);

		ui->lbl_stevilka_starsa->setVisible(false);
		ui->cb_stevilka_starsa->setVisible(false);
		ui->txt_stevilka_starsa->setVisible(false);

//        ui->btn_racun->setVisible(true);
		ui->btn_predplacilni_racun->setVisible(true);

		ui->lbl_avans_placan->setVisible(true);
		ui->txt_datum_placila_avansa->setVisible(true);

		ui->lbl_racun_placan->setVisible(false);
		ui->txt_datum_placila_racuna->setVisible(false);

		setWindowTitle(windowTitle().remove("predplacilnega ").replace(" ra", " predra"));
		ui->tab_podatki->setWindowTitle(ui->tab_podatki->windowTitle().remove("predplacilnega ").replace(" ra", " predra"));
		ui->lbl_stevilka_racuna->setText(ui->lbl_stevilka_racuna->text().remove("predplacilnega ").replace(" ra", " predra"));
		ui->lbl_stara_stevilka_racuna->setText(ui->lbl_stara_stevilka_racuna->text().remove("predplacilnega ").replace(" ra", " predra"));
		ui->lbl_datum_izdaje_racuna->setText(ui->lbl_datum_izdaje_racuna->text().remove("predplacilnega ").replace(" ra", " predra"));
		ui->lbl_datum_konca->setText("Predviden rok izvedbe");
		ui->lbl_status_oddaje_racuna->setText(ui->lbl_status_oddaje_racuna->text().remove("predplacilnega ").replace(" ra", " predra"));
		ui->lbl_datum_oddaje_racuna->setText(ui->lbl_datum_oddaje_racuna->text().remove("predplacilnega ").replace(" ra", " predra"));

		ui->txt_koda_namena->setVisible(false);
		ui->label_29->setVisible(false);
		ui->txt_koda_namena_avans->setVisible(true);
		ui->label_28->setVisible(true);
	}

}

void racun::on_rb_predplacilo_toggled() {

	if ( ui->rb_predplacilo->isChecked() ) {
		ui->txt_status_predracuna->setHidden(true);
		ui->lbl_status_predracuna->setHidden(true);
		ui->txt_status_placila->setHidden(false);
		ui->lbl_status_placila->setHidden(false);
		ui->txt_status_racunovodstva->setHidden(false);
		ui->lbl_status_racunovodstva->setHidden(false);

		ui->txt_odstotek_avansa->setEnabled(false);
		ui->txt_datum_pricetka->setHidden(true);
		ui->lbl_datum_pricetka->setHidden(true);

		ui->lbl_stevilka_starsa->setVisible(true);
		ui->cb_stevilka_starsa->setVisible(true);
		ui->txt_stevilka_starsa->setVisible(true);

//        ui->btn_racun->setVisible(false);
		ui->btn_predplacilni_racun->setVisible(false);

		ui->lbl_avans_placan->setVisible(true);
		ui->txt_datum_placila_avansa->setVisible(true);

		ui->lbl_racun_placan->setVisible(false);
		ui->txt_datum_placila_racuna->setVisible(false);

		setWindowTitle(windowTitle().remove("predplacilnega ").replace(" predra", " ra").replace(" ra", " predplacilnega ra"));
		ui->tab_podatki->setWindowTitle(ui->tab_podatki->windowTitle().remove("predplacilnega ").replace(" predra", " ra").replace(" ra", " predplacilnega ra"));
		ui->lbl_stevilka_racuna->setText(ui->lbl_stevilka_racuna->text().remove("predplacilnega ").replace(" predra", " ra").replace(" ra", " predplacilnega ra"));
		ui->lbl_stara_stevilka_racuna->setText(ui->lbl_stara_stevilka_racuna->text().remove("predplacilnega ").replace(" predra", " ra").replace(" ra", " predplacilnega ra"));
		ui->lbl_datum_izdaje_racuna->setText(ui->lbl_datum_izdaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra").replace(" ra", " predplacilnega ra"));
		ui->lbl_datum_konca->setText("Predviden rok izvedbe");
		ui->lbl_status_oddaje_racuna->setText(ui->lbl_status_oddaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra").replace(" ra", " predplacilnega ra"));
		ui->lbl_datum_oddaje_racuna->setText(ui->lbl_datum_oddaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra").replace(" ra", " predplacilnega ra"));

		ui->txt_koda_namena->setVisible(false);
		ui->label_29->setVisible(false);
		ui->txt_koda_namena_avans->setVisible(false);
		ui->label_28->setVisible(false);
	}

}

void racun::on_rb_racun_toggled() {

	if ( ui->rb_racun->isChecked() ) {
		ui->txt_status_predracuna->setHidden(true);
		ui->lbl_status_predracuna->setHidden(true);
		ui->txt_status_placila->setHidden(false);
		ui->lbl_status_placila->setHidden(false);
		ui->txt_status_racunovodstva->setHidden(false);
		ui->lbl_status_racunovodstva->setHidden(false);

		ui->txt_odstotek_avansa->setEnabled(false);
		ui->txt_datum_pricetka->setHidden(false);
		ui->lbl_datum_pricetka->setHidden(false);

		ui->lbl_stevilka_starsa->setVisible(true);
		ui->cb_stevilka_starsa->setVisible(true);
		ui->txt_stevilka_starsa->setVisible(true);

//        ui->btn_racun->setVisible(false);
		ui->btn_predplacilni_racun->setVisible(false);

		ui->lbl_avans_placan->setVisible(true);
		ui->txt_datum_placila_avansa->setVisible(true);

		ui->lbl_racun_placan->setVisible(true);
		ui->txt_datum_placila_racuna->setVisible(true);

		setWindowTitle(windowTitle().remove("predplacilnega ").replace(" predra", " ra"));
		ui->tab_podatki->setWindowTitle(ui->tab_podatki->windowTitle().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_stevilka_racuna->setText(ui->lbl_stevilka_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_stara_stevilka_racuna->setText(ui->lbl_stara_stevilka_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_datum_izdaje_racuna->setText(ui->lbl_datum_izdaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_datum_konca->setText("Datum zakljucka");
		ui->lbl_status_oddaje_racuna->setText(ui->lbl_status_oddaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_datum_oddaje_racuna->setText(ui->lbl_datum_oddaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));

		ui->txt_koda_namena->setVisible(true);
		ui->label_29->setVisible(true);
		ui->txt_koda_namena_avans->setVisible(false);
		ui->label_28->setVisible(false);
	}

}

void racun::on_rb_dobropis_toggled() {

	if ( ui->rb_dobropis->isChecked() ) {
		ui->txt_status_predracuna->setHidden(true);
		ui->lbl_status_predracuna->setHidden(true);
		ui->txt_status_placila->setHidden(false);
		ui->lbl_status_placila->setHidden(false);
		ui->txt_status_racunovodstva->setHidden(false);
		ui->lbl_status_racunovodstva->setHidden(false);

		ui->txt_odstotek_avansa->setEnabled(false);
		ui->txt_datum_pricetka->setHidden(false);
		ui->lbl_datum_pricetka->setHidden(false);

		ui->lbl_stevilka_starsa->setVisible(true);
		ui->cb_stevilka_starsa->setVisible(true);
		ui->txt_stevilka_starsa->setVisible(true);

//        ui->btn_racun->setVisible(false);
		ui->btn_predplacilni_racun->setVisible(false);

		ui->lbl_avans_placan->setVisible(true);
		ui->txt_datum_placila_avansa->setVisible(true);

		ui->lbl_racun_placan->setVisible(true);
		ui->txt_datum_placila_racuna->setVisible(true);

		setWindowTitle(windowTitle().remove("predplacilnega ").replace(" predra", " ra"));
		ui->tab_podatki->setWindowTitle(ui->tab_podatki->windowTitle().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_stevilka_racuna->setText(ui->lbl_stevilka_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_stara_stevilka_racuna->setText(ui->lbl_stara_stevilka_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_datum_izdaje_racuna->setText(ui->lbl_datum_izdaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_datum_konca->setText("Datum zakljucka");
		ui->lbl_status_oddaje_racuna->setText(ui->lbl_status_oddaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));
		ui->lbl_datum_oddaje_racuna->setText(ui->lbl_datum_oddaje_racuna->text().remove("predplacilnega ").replace(" predra", " ra"));

		ui->txt_koda_namena->setVisible(true);
		ui->label_29->setVisible(true);
		ui->txt_koda_namena_avans->setVisible(false);
		ui->label_28->setVisible(false);

		// napolni opravila pri dobropisu
		wid_dobropis *widdob = new wid_dobropis;
		ui->scr_dobropis->setWidget(widdob);

		// povezi signale pri opravilih dobropisa med seboj
		QObject::connect(this, SIGNAL(prenos(QString)),
				   widdob , SLOT(prejem(QString)));
		prenos("Dobropis" + ui->txt_id->text());
		this->disconnect();

		ui->tab_racuni->insertTab(3, ui->tab_dobropis, "Opravila");
		ui->tab_racuni->removeTab(2);
	}
	else {
		ui->tab_racuni->insertTab(2, ui->tab_opravila, "Opravila");
		ui->tab_racuni->removeTab(3);
	}

}

void racun::on_txt_odstotek_avansa_editingFinished() {

	ui->txt_odstotek_avansa->setText(pretvori_iz_double(pretvori_v_double(ui->txt_odstotek_avansa->text())) + " %");

	ui->txt_avans->setText(pretvori_iz_double(QString::number(pretvori_v_double(ui->txt_znesek->text()).toDouble() *
																														pretvori_v_double(ui->txt_odstotek_avansa->text()).toDouble() / 100, 'f', 2)) + " EUR");

	if ( ui->txt_odstotek_avansa->text() == "0,0 %" ) {
		ui->txt_datum_placila_avansa->setHidden(true);
		ui->lbl_avans_placan->setHidden(true);
	}
	else if ( ui->txt_odstotek_avansa->text() == "100,0 %" ) {
		ui->txt_datum_placila_racuna->setEnabled(false);
		ui->txt_datum_placila_racuna->setDate(ui->txt_datum_placila_avansa->date());
	}
	else {
		ui->txt_datum_placila_avansa->setHidden(false);
		ui->lbl_avans_placan->setHidden(false);
	}

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

void racun::on_cb_stevilka_starsa_toggled() {

	if ( ui->cb_stevilka_starsa->isChecked() ) {
		ui->txt_stevilka_starsa->setEnabled(true);
	}
	else {
		ui->txt_stevilka_starsa->setEnabled(false);
	}

}

void racun::stevilka_racuna() {

	if ( ui->btn_sprejmi->text() != "Odpiram" ) {

			QString leto = ui->txt_datum_izdaje_racuna->text().right(4);
			QString mesec = ui->txt_datum_izdaje_racuna->text().left(5).right(2);
			QString dan = ui->txt_datum_izdaje_racuna->text().left(2);

			int max_st_racuna = 0;

			bool tvori = true; // tvorimo stevilko naloga
			QString stara_stevilka_naloga = "";
			if ( ui->txt_id->text() != "" ) {
				if ( !ui->rb_predplacilo->isChecked() ) {
					QSqlQuery sql_stara_stevilka;
					sql_stara_stevilka.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
					sql_stara_stevilka.exec();
					if ( sql_stara_stevilka.next() ) {
						stara_stevilka_naloga = prevedi(sql_stara_stevilka.value(sql_stara_stevilka.record().indexOf("stevilka_racuna")).toString());
						if ( stara_stevilka_naloga.left(2) == leto.right(2) ) {
							ui->txt_stevilka_racuna->setText(stara_stevilka_naloga);
							tvori = false;
						}
					}
				}
			}

			// zapisi stevilko racuna
			if ( tvori == true ) {
				// izracunamo zaporedno stevilko racuna v tekocem letu
				QSqlQuery sql_stetje_racunov;
				QString tip_racuna = "";
				if ( ui->rb_predracun->isChecked() ) {
					sql_stetje_racunov.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%." + pretvori(leto) +
											   "' AND tip_racuna LIKE '" + pretvori("1") + "' ORDER BY stevilka_racuna ASC");
				}
				else if ( ui->rb_predplacilo->isChecked() ) {
					sql_stetje_racunov.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%." + pretvori(leto) +
											   "' AND tip_racuna LIKE '" + pretvori("2") + "' ORDER BY stevilka_racuna ASC");
				}
				else if ( ui->rb_racun->isChecked() ) {
					sql_stetje_racunov.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%." + pretvori(leto) +
											   "' AND tip_racuna LIKE '" + pretvori("3") + "' ORDER BY stevilka_racuna ASC");
				}
				else if ( ui->rb_dobropis->isChecked() ) {
					sql_stetje_racunov.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%." + pretvori(leto) +
											   "' AND tip_racuna LIKE '" + pretvori("4") + "' ORDER BY stevilka_racuna ASC");
				}


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

				QString predpona = "";

				if ( ui->rb_predracun->isChecked() ) {
					predpona = "P";
				}
				else if ( ui->rb_predplacilo->isChecked() ) {
					predpona = "A";
				}
				else if ( ui->rb_racun->isChecked() ) {
					predpona = "R";
				}
				else if ( ui->rb_dobropis->isChecked() ) {
					predpona = "D";
				}
				// imamo dovolj podatkov za tvorbo stevilke racuna

				ui->txt_stevilka_racuna->setText(predpona + leto.right(2) + st_racuna);
			}

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
				// gleda trenutni sklic, ce je prazen (predracun) povprasa o javnosti in zasebnosti, drugace privzame sklic iz predracuna
				if ( ui->txt_sklic->text() == "" ) { // predracun
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
				else { // ostalo, privzamemo vrednost prejsnjega polja
					// doloci vse tri cifre
					QString i_sklic = ui->txt_sklic->text();
					i_sklic = i_sklic.right(i_sklic.length() - 5); // odbijemo drzavo in model
					i_sklic = i_sklic.right(i_sklic.length() - 5); // odbijemo stevilko racuna
					int i_cifra_1 = i_sklic.left(1).toInt();
					i_sklic = i_sklic.right(i_sklic.length() - 3); // odbijemo cifro_1 in dan
					int i_cifra_2 = i_sklic.left(1).toInt();
					i_sklic = i_sklic.right(i_sklic.length() - 3); // odbijemo cifro_2 in mesec
					int i_cifra_3 = i_sklic.left(1).toInt();

					// iz prvih dveh izracunaj kontrolno stevilko
					int i_kontrolna = 0;

					int i_sestevek = 3 * i_cifra_1 + 2 * i_cifra_2;

					int i_ostanek = i_sestevek % 11;

					i_kontrolna = 11 - i_ostanek;

					if ( i_kontrolna >= 9 ) {
						i_kontrolna = 0;
					}

					// od cifre_3 odstej kontrolno stevilko
					// tako dobis 0 => racun je javen ali 1 => racun je zaseben
					int i_razlika = i_cifra_3 - i_kontrolna;

					if ( i_razlika == 1 ) {
						cifra_3 = kontrolna + 1;
					}
					else {
						cifra_3 = kontrolna;
					}
				}
			}
			else { // javen dostop, vsi lahko vidijo ta racun
				cifra_3 = kontrolna;
			}

			// 2) Zdruzimo podatke in nakljucna stevila
			QString sklic = "";
			sklic += ui->txt_stevilka_racuna->text().replace(QRegExp("[^0-9]"), "");
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

}

void racun::on_txt_datum_izdaje_racuna_dateChanged() {

	if ( ui->txt_status_oddaje_racuna->currentText() != "" ) {
		if ( ui->txt_stevilka_racuna->text() == "" ) {
			stevilka_racuna();
		}
	}

}

void racun::on_txt_status_oddaje_racuna_currentIndexChanged() {

	if ( ui->txt_status_oddaje_racuna->currentText() != "" ) {
		ui->txt_status_predracuna->setEnabled(true);
		ui->txt_datum_oddaje_racuna->setEnabled(true);
		ui->txt_status_racunovodstva->setEnabled(true);
		// ui->txt_rok_placila->setEnabled(true);
		ui->txt_odstotek_avansa->setEnabled(true);
		ui->txt_datum_placila_avansa->setEnabled(true);
		ui->txt_datum_izdaje_racuna->setEnabled(true);
		ui->txt_status_placila->setEnabled(true);
		if ( ui->txt_stevilka_racuna->text() == "" ) {
			stevilka_racuna();
		}
	}
	else {
		ui->txt_status_predracuna->setEnabled(false);
		ui->txt_datum_oddaje_racuna->setEnabled(false);
		ui->txt_status_racunovodstva->setEnabled(false);
		// ui->txt_rok_placila->setEnabled(false);
		ui->txt_odstotek_avansa->setEnabled(false);
		ui->txt_datum_placila_avansa->setEnabled(false);
		ui->txt_datum_izdaje_racuna->setEnabled(false);
		ui->txt_status_placila->setEnabled(false);
		ui->txt_stevilka_racuna->setText("");
		ui->txt_sklic->setText("");
	}

}

void racun::on_txt_status_predracuna_currentIndexChanged() {

	// preveriti, ali racun ze obstaja!!!

	QString nas_id = "";
	QString status = "Ne obstaja";

	// ali smo na predracunu?
	if ( ui->rb_predracun->isChecked() ) {
		// ali ima predracun ze dodeljeno stevilko predracuna?
		if ( ui->txt_stevilka_racuna->text() != "" ) {
			// preverimo, ali obstaja racun, ki ima za stevilko starsa stevilko nasega predracuna

				// pridobimo nas id
				QSqlQuery sql_nas_id;
				sql_nas_id.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stevilka_racuna->text()) + "'");
				sql_nas_id.exec();
				if ( sql_nas_id.next() ) { // stevilka nasega racuna je vpisana v bazi
					nas_id = prevedi(sql_nas_id.value(sql_nas_id.record().indexOf("id")).toString());
				}

				// preverimo, ali obstaja racun z nasim id-jem kot starsem
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori("3") + "' AND stevilka_starsa LIKE '" + pretvori(nas_id) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					status = "Obstaja";
				}
		}
	}

	if ( status == "Ne obstaja" ) { // nas predracun se nima racuna
		if ( ui->rb_predracun->isChecked() && ui->txt_status_predracuna->currentText() == "Potrjen" ) { // obvestimo o mozni tvorbi racuna
			ui->txt_status_placila->setCurrentIndex(ui->txt_status_placila->findText("Pla", Qt::MatchStartsWith));
			// ce je spustni seznam omogocen in ima status potrjen, potem obvesti, da ob shranjevanju tvori racun
			if ( ui->txt_status_predracuna->isEnabled() ) {
				QMessageBox sporocilo;
				sporocilo.setText("Ob pritisku na gumb Shrani bo predracun zaprt, \n"
								  "tvoril se bo nov t.i. zacasni racun, ki bo osnova \n"
								  "za koncni racun. Omogoceno bo vnasanje casovnic in komentarjev!");
				sporocilo.exec();

				// spremeni status pripadajocega projekta na "V delu", st.2
				QSqlQuery sql_status_projekta;
				sql_status_projekta.prepare("UPDATE projekti SET status_projekta = ? WHERE id LIKE '" + pretvori(ui->txt_projekt_id->text()) + "'");
				sql_status_projekta.bindValue(0, pretvori("2"));
				sql_status_projekta.exec();
			}
		}
		else if ( ui->rb_predracun->isChecked() && ui->txt_status_predracuna->currentText() == "Zavrnjen" ) { // status racunovodstva
			ui->txt_status_racunovodstva->setCurrentIndex(ui->txt_status_racunovodstva->findText("Ni za oddajo"));
		}
	// ce omogocimo, vpliva tudi na polnenje polj, kjer pa to ni zazeleno
	//    else {
	//        ui->txt_status_racunovodstva->setCurrentIndex(0);
	//    }
	}

}

void racun::on_btn_pocisti_2_clicked() {

	ui->txt_id_zapisa_2->setText("");
	ui->txt_datum_zapisa_2->setDateTime(QDateTime::currentDateTime());
	ui->txt_naslov_zapisa_2->setText("");
	ui->txt_opis_zapisa_2->clear();

	ui->btn_vnesi_zapis_2->setText("Vnesi zapis");

	napolni_zapise();

}

void racun::napolni_zapise() {

		// clear previous content
		ui->tbl_zapisi_2->clear();

		for (int i = 0; i <= 2; i++) {
			ui->tbl_zapisi_2->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM opombe");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_zapisi_2->removeRow(0);
		}

		// start filling the table
		ui->tbl_zapisi_2->insertColumn(0);
		ui->tbl_zapisi_2->insertColumn(1);
		ui->tbl_zapisi_2->insertColumn(2);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Datum zapisa");
		naslov2->setText("Naslov zapisa");

		ui->tbl_zapisi_2->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_zapisi_2->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_zapisi_2->setHorizontalHeaderItem(2, naslov2);

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
		else if ( ui->rb_dobropis->isChecked() ) {
			tip_racuna = "4";
		}

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM opombe WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori(tip_racuna) + "' ORDER BY id ASC");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_zapisi_2->insertRow(row);
			ui->tbl_zapisi_2->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[8] = {"id", "datum", "naslov"};

			while (col <= 2) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "id" ) {
					celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
				}
				else {
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				}
				ui->tbl_zapisi_2->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}

}

void racun::on_tbl_zapisi_2_clicked() {

	QString id = ui->tbl_zapisi_2->selectedItems().takeAt(0)->text();

		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM opombe WHERE id LIKE '" + pretvori(id) + "'");
		sql_napolni.exec();
		if ( sql_napolni.next() ) {
			ui->txt_id_zapisa_2->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
			QDateTime datum = QDateTime::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			ui->txt_datum_zapisa_2->setDateTime(datum);
			ui->txt_naslov_zapisa_2->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
			ui->txt_opis_zapisa_2->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("besedilo")).toString()));
		}

	ui->btn_vnesi_zapis_2->setText("Popravi zapis");

}


void racun::on_btn_izbris_zapisa_2_clicked() {

		// delete selected note
		QSqlQuery sql_notes;
		sql_notes.prepare("DELETE FROM opombe WHERE id LIKE '" + ui->txt_id_zapisa_2->text() + "'");
		sql_notes.exec();

	napolni_zapise();

}

void racun::on_btn_vnesi_zapis_2_clicked() {

		QSqlQuery sql_vnesi_zapis;
		if (ui->btn_vnesi_zapis_2->text() == "Vnesi zapis") {
			sql_vnesi_zapis.prepare("INSERT INTO opombe (stevilka_stranke, stevilka_projekta, stevilka_racuna, tip_racuna, datum, naslov, "
																"besedilo) VALUES (?, ?, ?, ?, ?, ?, ?)");
		}
		else {
			sql_vnesi_zapis.prepare("UPDATE opombe SET stevilka_stranke = ?, stevilka_projekta = ?, stevilka_racuna = ?, tip_racuna = ?, datum = ?, naslov = ?, "
															"besedilo = ? WHERE id LIKE '" + pretvori(ui->txt_id_zapisa_2->text()) + "'");
		}
		sql_vnesi_zapis.bindValue(0, pretvori(ui->txt_stranka_id->text()));
		sql_vnesi_zapis.bindValue(1, pretvori(ui->txt_projekt_id->text()));
		sql_vnesi_zapis.bindValue(2, pretvori(ui->txt_id->text()));
		if ( ui->rb_predracun->isChecked() ) {
			sql_vnesi_zapis.bindValue(3, pretvori("1"));
		}
		else if ( ui->rb_predplacilo->isChecked() ) {
			sql_vnesi_zapis.bindValue(3, pretvori("2"));
		}
		else if ( ui->rb_racun->isChecked() ) {
			sql_vnesi_zapis.bindValue(3, pretvori("3"));
		}
		else if ( ui->rb_dobropis->isChecked() ) {
			sql_vnesi_zapis.bindValue(3, pretvori("4"));
		}
		sql_vnesi_zapis.bindValue(4, pretvori(ui->txt_datum_zapisa_2->text()));
		sql_vnesi_zapis.bindValue(5, pretvori(ui->txt_naslov_zapisa_2->text()));
		sql_vnesi_zapis.bindValue(6, pretvori(ui->txt_opis_zapisa_2->toPlainText()));

		sql_vnesi_zapis.exec();

	ui->txt_id_zapisa_2->setText("");
	ui->txt_datum_zapisa_2->setDateTime(QDateTime::currentDateTime());
	ui->txt_naslov_zapisa_2->setText("");
	ui->txt_opis_zapisa_2->clear();

	ui->btn_vnesi_zapis_2->setText("Vnesi zapis");

	napolni_zapise();

}

void racun::on_txt_vse_opombe_textChanged() {

	QString zaporedje = ui->txt_vse_opombe->text();

	QString preostanek_opombe = zaporedje.left(zaporedje.length() - 1);

	// napolni seznam opomb
	QList<int> seznam_opomb;

	for ( int a = 0; a <= zaporedje.count(",,"); a++) {
		QString trenutni_id = "";
		trenutni_id = preostanek_opombe.left(preostanek_opombe.indexOf(",,")).right(preostanek_opombe.left(preostanek_opombe.indexOf(",,")).length() - 1);
		seznam_opomb.append(trenutni_id.toInt());
		preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - preostanek_opombe.indexOf(",,") - 1);
	}

	// uredi seznam opomb
	qSort(seznam_opomb.begin(), seznam_opomb.end());

	QString novo_zaporeje = "";
	for ( int b = 0; b < seznam_opomb.size(); b++ ) {
		novo_zaporeje += "," + QString::number(seznam_opomb.at(b), 10) + ",";
	}

	// ponastavi besedilo polja vseh opomb, ki je sedaj v pravem vrstnem redu in urejeno
	ui->txt_vse_opombe->setText(novo_zaporeje.remove(",0,"));

	if ( ui->btn_sprejmi->text() != "Odpiram" ) {
		napolni_vse_opombe();
	}
	seznam_opomb.clear();

}

void racun::on_txt_vnesene_opombe_textChanged() {

	if ( ui->btn_sprejmi->text() != "Odpiram" ) {
		napolni_vnesene_opombe();
	}

}

void racun::napolni_vse_opombe() {

		QString zaporedje = ui->txt_vse_opombe->text();

		// odstrani podvojene vejice
		while ( zaporedje.contains(",,") ) {
			zaporedje = zaporedje.replace(",,", ",");
		}

		// odstrani prvo vejico
		if ( zaporedje.left(1) == "," ) {
			zaporedje = zaporedje.right(zaporedje.length() - 1);
		}

		// odstrani zadnjo vejico
		if ( zaporedje.right(1) == "," ) {
			zaporedje = zaporedje.left(zaporedje.length() - 1);
		}

		QString preostanek_opombe = zaporedje;

		// napolni seznam opomb
		QStringList seznam_opomb;
		for ( int a = 0; a <= zaporedje.count(","); a++) {
			seznam_opomb.append(preostanek_opombe.left(preostanek_opombe.indexOf(",")));
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - preostanek_opombe.indexOf(",") - 1);
		}

		QString id = "";

		// clear previous content
		ui->tbl_vse_opombe->clear();

		for (int i = 0; i <= 1; i++) {
			ui->tbl_vse_opombe->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM sif_opombe_pri_racunih");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_vse_opombe->removeRow(0);
		}
		sql_clear.clear();

		// start filling the table
		ui->tbl_vse_opombe->insertColumn(0);
		ui->tbl_vse_opombe->insertColumn(1);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Naslov");

		ui->tbl_vse_opombe->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_vse_opombe->setHorizontalHeaderItem(1, naslov1);

		ui->tbl_vse_opombe->setColumnWidth(0, 35);
		int sirina = 290;
		if ( ui->tbl_vse_opombe->width() > sirina ) {
			sirina = ui->tbl_vse_opombe->width() - 65;
		}
		ui->tbl_vse_opombe->setColumnWidth(1, sirina);

		int row = 0;
		for ( int b = 0; b < seznam_opomb.count(); b++ ) {
			id = seznam_opomb.at(b); // dobimo id opombe

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_opombe_pri_racunih WHERE id LIKE '" + pretvori(id) + "' ORDER BY id ASC");
			sql_fill.exec();

			if (sql_fill.next()) {
				ui->tbl_vse_opombe->insertRow(row);
				ui->tbl_vse_opombe->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[2] = {"id", "naslov"};

				while (col <= 1) {

					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[i] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
					}
					else {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					}
					ui->tbl_vse_opombe->setItem(row, col, celica);

					col++;
					i++;

				}
			}
			row++;
		}

}

void racun::napolni_vnesene_opombe() {

		QString zaporedje = ui->txt_vnesene_opombe->text().right(ui->txt_vnesene_opombe->text().length() - 1);

		// odstrani podvojene vejice
		zaporedje = zaporedje.replace(",,", ",");

		// odstrani zadnjo vejico
		if ( zaporedje.right(1) == "," ) {
			zaporedje = zaporedje.left(zaporedje.length() - 1);
		}

		QString preostanek_opombe = zaporedje;

		// napolni seznam opomb
		QStringList seznam_opomb;
		for ( int a = 0; a <= zaporedje.count(","); a++) {
			seznam_opomb.append(preostanek_opombe.left(preostanek_opombe.indexOf(",")));
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - preostanek_opombe.indexOf(",") - 1);
		}

		QString id = "";

		// clear previous content
		ui->tbl_vnesene_opombe->clear();

		for (int i = 0; i <= 1; i++) {
			ui->tbl_vnesene_opombe->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM sif_opombe_pri_racunih");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_vnesene_opombe->removeRow(0);
		}

		// start filling the table
		ui->tbl_vnesene_opombe->insertColumn(0);
		ui->tbl_vnesene_opombe->insertColumn(1);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Naslov");

		ui->tbl_vnesene_opombe->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_vnesene_opombe->setHorizontalHeaderItem(1, naslov1);

		ui->tbl_vnesene_opombe->setColumnWidth(0, 35);
		int sirina = 290;
		if ( ui->tbl_vnesene_opombe->width() > sirina ) {
			sirina = ui->tbl_vnesene_opombe->width() - 65;
		}
		ui->tbl_vnesene_opombe->setColumnWidth(1, sirina);

		int row = 0;
		for ( int b = 0; b < seznam_opomb.count(); b++ ) {
			id = seznam_opomb.at(b); // dobimo id opombe

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_opombe_pri_racunih WHERE id LIKE '" + pretvori(id) + "' ORDER BY id ASC");
			sql_fill.exec();

			if (sql_fill.next()) {
				ui->tbl_vnesene_opombe->insertRow(row);
				ui->tbl_vnesene_opombe->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[2] = {"id", "naslov"};

				while (col <= 1) {

					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[i] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
					}
					else {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					}
					ui->tbl_vnesene_opombe->setItem(row, col, celica);

					col++;
					i++;

				}
			}
			row++;
		}

}

void racun::on_tbl_vse_opombe_doubleClicked() {

	QString id = "," + ui->tbl_vse_opombe->selectedItems().takeAt(0)->text() + ",";

	if ( ui->txt_vse_opombe->text().indexOf(id, 0) == -1 ) { // ce opomba se ni bila dodana, jo doda
		ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text() + id);
	}
	else { // jo odstrani
		ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text().remove(id));
	}

	if ( ui->txt_vnesene_opombe->text().indexOf(id, 0) == -1 ) { // ce opomba se ni bila dodana, jo doda
		ui->txt_vnesene_opombe->setText(ui->txt_vnesene_opombe->text() + id);
	}
	else { // jo odstrani
		ui->txt_vnesene_opombe->setText(ui->txt_vnesene_opombe->text().remove(id));
	}

}

void racun::on_tbl_vnesene_opombe_doubleClicked() {

	QString id = "," + ui->tbl_vnesene_opombe->selectedItems().takeAt(0)->text() + ",";

	if ( ui->txt_vse_opombe->text().indexOf(id, 0) == -1 ) { // ce opomba se ni bila dodana, jo doda
		ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text() + id);
	}
	else { // jo odstrani
		ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text().remove(id));
	}

	if ( ui->txt_vnesene_opombe->text().indexOf(id, 0) == -1 ) { // ce opomba se ni bila dodana, jo doda
		ui->txt_vnesene_opombe->setText(ui->txt_vnesene_opombe->text() + id);
	}
	else { // jo odstrani
		ui->txt_vnesene_opombe->setText(ui->txt_vnesene_opombe->text().remove(id));
	}

}

void racun::on_btn_gor_clicked() {

	if ( ui->tbl_vnesene_opombe->selectionModel()->isSelected(ui->tbl_vnesene_opombe->currentIndex()) ) {

		// nas seznam opomb vnesemo v listo
		QString zaporedje = ui->txt_vnesene_opombe->text().right(ui->txt_vnesene_opombe->text().length() - 1);

		// odstrani podvojene vejice
		zaporedje = zaporedje.replace(",,", ",");

		// odstrani zadnjo vejico
		if ( zaporedje.right(1) == "," ) {
			zaporedje = zaporedje.left(zaporedje.length() - 1);
		}

		QString preostanek_opombe = zaporedje;

		// napolni seznam opomb
		QStringList seznam_opomb;
		for ( int a = 0; a <= zaporedje.count(","); a++) {
			seznam_opomb.append(preostanek_opombe.left(preostanek_opombe.indexOf(",")));
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - preostanek_opombe.indexOf(",") - 1);
		}

		// dolocimo id vnosa, ki ga premikamo
		QString id = ui->tbl_vnesene_opombe->selectedItems().takeAt(0)->text();

		// dolocimo pozicijo vnosa, ki smo ga oznacili
		int pozicija = seznam_opomb.indexOf(id);

		// premaknemo pozicijo oznacenega polja za eno mesto visje na seznam
		if ( pozicija > 0 ) {
			seznam_opomb.swap(pozicija, pozicija - 1);
		}

		// ponovno tvorimo seznam
		zaporedje = "";
		for ( int b = 0; b < seznam_opomb.count(); b++ ) {
			zaporedje += "," + seznam_opomb.at(b) + ",";

		}

		// prepisemo seznam v nase polje
		ui->txt_vnesene_opombe->setText(zaporedje);

		// ponovno oznacimo premaknjeno vrstico
		ui->tbl_vnesene_opombe->selectRow(pozicija - 1);
	}

}

void racun::on_btn_dol_clicked() {

	if ( ui->tbl_vnesene_opombe->selectionModel()->isSelected(ui->tbl_vnesene_opombe->currentIndex()) ) {
		// nas seznam opomb vnesemo v listo
		QString zaporedje = ui->txt_vnesene_opombe->text().right(ui->txt_vnesene_opombe->text().length() - 1);

		// odstrani podvojene vejice
		zaporedje = zaporedje.replace(",,", ",");

		// odstrani zadnjo vejico
		if ( zaporedje.right(1) == "," ) {
			zaporedje = zaporedje.left(zaporedje.length() - 1);
		}

		QString preostanek_opombe = zaporedje;

		// napolni seznam opomb
		QStringList seznam_opomb;
		for ( int a = 0; a <= zaporedje.count(","); a++) {
			seznam_opomb.append(preostanek_opombe.left(preostanek_opombe.indexOf(",")));
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - preostanek_opombe.indexOf(",") - 1);
		}

		// dolocimo id vnosa, ki ga premikamo
		QString id = ui->tbl_vnesene_opombe->selectedItems().takeAt(0)->text();

		// dolocimo pozicijo vnosa, ki smo ga oznacili
		int pozicija = seznam_opomb.indexOf(id);

		// premaknemo pozicijo oznacenega polja za eno mesto visje na seznam
		if ( pozicija < seznam_opomb.count() - 1 ) {
			seznam_opomb.swap(pozicija, pozicija + 1);
		}

		// ponovno tvorimo seznam
		zaporedje = "";
		for ( int b = 0; b < seznam_opomb.count(); b++ ) {
			zaporedje += "," + seznam_opomb.at(b) + ",";

		}

		// prepisemo seznam v nase polje
		ui->txt_vnesene_opombe->setText(zaporedje);

		// ponovno oznacimo premaknjeno vrstico
		ui->tbl_vnesene_opombe->selectRow(pozicija + 1);
	}

}


void racun::on_btn_opravilo_gor_clicked() {

	// dolocimo polozaj izbrane vrstice
	int id = ui->tbl_opravila->selectedItems().takeAt(0)->row();

	if ( id != 0 ) {

		// dodamo novo vrstico
		ui->tbl_opravila->insertRow(0);

		// zakrozimo prek vseh stolpcev
		for ( int i = 0; i < ui->tbl_opravila->columnCount(); i++ ) {

			// prestavimo polje nad izbranim v na novo dodano vrstico
			ui->tbl_opravila->setItem(0, i, ui->tbl_opravila->takeItem(id, i));

			// prestavimo izbrano polje eno polje visje
			ui->tbl_opravila->setItem(id, i, ui->tbl_opravila->takeItem(id + 1, i));

			// prestavimo prvo prestavljena polja na mesto predhodno izbrane vrstice
			ui->tbl_opravila->setItem(id + 1, i, ui->tbl_opravila->takeItem(0, i));

		}

		// izbrisemo prazno vrstico
		ui->tbl_opravila->removeRow(0);

		// izberemo pred tem izbrano vrstico (premaknjeno eno vrstico visje)
		ui->tbl_opravila->selectRow(id - 1);
	}

	vnesi_vrstni_red();

}

void racun::on_btn_opravilo_dol_clicked() {

	// dolocimo polozaj izbrane vrstice
	int id = ui->tbl_opravila->selectedItems().takeAt(0)->row();

	if ( id != ui->tbl_opravila->rowCount() - 1 ) {

		// dodamo novo vrstico
		ui->tbl_opravila->insertRow(0);

		// zakrozimo prek vseh stolpcev
		for ( int i = 0; i < ui->tbl_opravila->columnCount(); i++ ) {

			// prestavimo polje pod izbranim v na novo dodano vrstico
			ui->tbl_opravila->setItem(0, i, ui->tbl_opravila->takeItem(id + 2, i));

			// prestavimo izbrano polje eno polje nizje
			ui->tbl_opravila->setItem(id + 2, i, ui->tbl_opravila->takeItem(id + 1, i));

			// prestavimo prvo prestavljena polja na mesto predhodno izbrane vrstice
			ui->tbl_opravila->setItem(id + 1, i, ui->tbl_opravila->takeItem(0, i));

		}

		// izbrisemo prazno vrstico
		ui->tbl_opravila->removeRow(0);

		// izberemo pred tem izbrano vrstico (premaknjeno eno vrstico nizje)
		ui->tbl_opravila->selectRow(id + 1);
	}

	vnesi_vrstni_red();

}

void racun::vnesi_vrstni_red() {

		QSqlQuery sql_vnesi_vrstni_red;

		// zaokrozi prek celotne tabele opravil
		for ( int i = 0; i < ui->tbl_opravila->rowCount(); i++ ) {
			sql_vnesi_vrstni_red.prepare("UPDATE opravila SET vrstni_red = ? WHERE id LIKE '" +
										 ui->tbl_opravila->item(i, 0)->text() + "'");
			sql_vnesi_vrstni_red.bindValue(0, QString::number(i, 10));
			sql_vnesi_vrstni_red.exec();
			sql_vnesi_vrstni_red.clear();
		}

}

void racun::on_txt_banka_currentIndexChanged() {

		// v bazi poiscemo pot do mesta shranjevanja podatkov
		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM sif_banke WHERE ime_banke LIKE '" + pretvori(ui->txt_banka->currentText()) + "'");
		sql_pot.exec();
		if ( sql_pot.next() ) {
			ui->txt_bic->setText(prevedi(sql_pot.value(sql_pot.record().indexOf("bic")).toString()));
		}

}

void racun::on_rb_fizicna_toggled() {

	if ( ui->rb_fizicna->isChecked() ) {
		ui->rb_fizicna->setChecked(true);
		ui->rb_pravna->setChecked(false);
	}
	else {
		ui->rb_fizicna->setChecked(false);
		ui->rb_pravna->setChecked(true);
	}

	napolni_stranke();

}

void racun::on_rb_pravna_toggled() {

	if ( ui->rb_fizicna->isChecked() ) {
		ui->rb_fizicna->setChecked(true);
		ui->rb_pravna->setChecked(false);
	}
	else {
		ui->rb_fizicna->setChecked(false);
		ui->rb_pravna->setChecked(true);
	}

	napolni_stranke();

}

void racun::napolni_stranke() {

	ui->txt_stranka->clear();
	ui->txt_stranka->addItem("");

		QSqlQuery sql_fill_combo;
		if ( ui->rb_fizicna->isChecked() ) {
			sql_fill_combo.prepare("SELECT * FROM stranke WHERE tip LIKE '1' ORDER BY priimek ASC");
		}
		else {
			sql_fill_combo.prepare("SELECT * FROM stranke WHERE tip LIKE '2' ORDER BY priimek ASC");
		}
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

}

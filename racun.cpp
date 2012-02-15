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
		ui->btn_sprejmi->setText("Odpiram");

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

		ui->txt_odstotek_avansa->setText("0,0 %");

		ui->tbl_opravila->clear();

		ui->tbl_vse_opombe->clear();
		ui->tbl_vnesene_opombe->clear();
		ui->txt_vse_opombe->setText("");
		ui->txt_vnesene_opombe->setText("");

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

		ui->btn_predplacilni_racun->setEnabled(false);
		ui->btn_racun->setEnabled(false);

		// skrij polja
		ui->txt_projekt_id->setVisible(false);
		ui->txt_stranka_id->setVisible(false);
		ui->txt_id_zapisa_2->setVisible(false);

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

			sql_fill_combo.prepare("SELECT * FROM sif_opombe_pri_racunih");
			sql_fill_combo.exec();
			while (sql_fill_combo.next()) {
				ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text() + "," +
																		prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString()) + ",");
			}
			sql_fill_combo.clear();

		}
		base.close();

		// nastavi privzete vrednosti
		ui->rb_predracun->setChecked(true);
		ui->txt_status_placila->setCurrentIndex(1);
		ui->txt_status_racunovodstva->setCurrentIndex(1);

		ui->tab_racuni->setCurrentIndex(0);

		ui->btn_sprejmi->setText("Vnesi racun");

		napolni_vse_opombe();

}

racun::~racun()
{
    delete ui;
}

void racun::on_btn_izpisi_clicked() {

	on_btn_sprejmi_clicked();

	print(ui->txt_id->text());

}

void racun::on_btn_izhod_clicked() {

	close();

}

void racun::on_btn_racun_clicked() {

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

		// za potrebe vnosa racuna vnosno okno ustrezno oznacimo
		ui->rb_racun->setChecked(true);

		// na novo izracunamo stevilko racuna
		stevilka_racuna();

		// vnesemo podatke
		QSqlQuery sql_vnesi_projekt;
		sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
															"datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
															"status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa, stevilka_starsa) "
															"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_racuna->text()));
		sql_vnesi_projekt.bindValue(1, pretvori("3")); // predplacilo (2), racun (3)
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
		sql_vnesi_projekt.bindValue(12, pretvori(pretvori_v_double(ui->txt_avans->text())));
		sql_vnesi_projekt.bindValue(13, pretvori(pretvori_v_double(ui->txt_odstotek_avansa->text())));
		sql_vnesi_projekt.bindValue(14, pretvori(ui->txt_status_oddaje_racuna->currentText()));
		sql_vnesi_projekt.bindValue(15, pretvori(ui->txt_datum_oddaje_racuna->text()));
		sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
		sql_vnesi_projekt.bindValue(17, pretvori(ui->txt_sklic->text()));
		sql_vnesi_projekt.bindValue(18, pretvori(ui->txt_datum_placila_avansa->text()));
		sql_vnesi_projekt.bindValue(19, pretvori(ui->txt_id->text()));

		sql_vnesi_projekt.exec();

		// poiscemo id pravkar vnesenega zapisa
		QSqlQuery sql_nov_id;
		QString nov_id = "";
		sql_nov_id.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stevilka_racuna->text()) +
											 "' AND tip_racuna LIKE '" + pretvori("3") + "'");
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
																	 "popust_komb1, popust_komb2, popust_stranka, popust_kupon, popust_akcija, podrazitev_vikend, "
																	 "podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, "
																	 "tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, znesek_koncni) "
																	 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
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
		}

	}
	base.close();

	// ponastavimo na predracun
	ui->rb_predracun->setChecked(true);

	// onemogocimo gumb
	ui->btn_racun->setEnabled(false);

}

void racun::on_btn_predplacilni_racun_clicked() {

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

		// za potrebe vnosa racuna vnosno okno ustrezno oznacimo
		ui->rb_predplacilo->setChecked(true);

		// na novo izracunamo stevilko racuna
		stevilka_racuna();

		// vnesemo podatke
		QSqlQuery sql_vnesi_projekt;
		sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
															"datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
															"status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa, stevilka_starsa) "
															"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_racuna->text()));
		sql_vnesi_projekt.bindValue(1, pretvori("2")); // predplacilo (2), racun (3)
		sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_status_predracuna->currentText()));
		sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_stranka_id->text()));
		sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_projekt_id->text()));
		sql_vnesi_projekt.bindValue(5, pretvori(vApp->id()));
		sql_vnesi_projekt.bindValue(6, pretvori(ui->txt_pricetek->text()));
		sql_vnesi_projekt.bindValue(7, pretvori(ui->txt_konec->text()));

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
		sql_vnesi_projekt.bindValue(14, pretvori(ui->txt_status_oddaje_racuna->currentText()));
		sql_vnesi_projekt.bindValue(15, pretvori(ui->txt_datum_oddaje_racuna->text()));
		sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
		sql_vnesi_projekt.bindValue(17, pretvori(ui->txt_sklic->text()));
		sql_vnesi_projekt.bindValue(18, pretvori(ui->txt_datum_placila_avansa->text()));
		sql_vnesi_projekt.bindValue(19, pretvori(ui->txt_id->text()));

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
																	 "popust_komb1, popust_komb2, popust_stranka, popust_kupon, popust_akcija, podrazitev_vikend, "
																	 "podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, "
																	 "tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, znesek_koncni) "
																	 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
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
		}

	}
	base.close();

	// ponastavimo na predracun
	ui->rb_predracun->setChecked(true);

	// onemogocimo gumb
	ui->btn_predplacilni_racun->setEnabled(false);

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
			// doloci SQL query glede na stanje programa: vnesi nov vnos, popravi obstojeci vnos, kopiraj obstojeci vnos in vse pripadajoce opravke
			QSqlQuery sql_vnesi_projekt;
			if (ui->btn_sprejmi->text() == "Vnesi racun") { // nov vnos se neobstojecega (pred)racuna
				sql_vnesi_projekt.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
																	"datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
																	"status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa, opombe) "
																	"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojec vnos
				sql_vnesi_projekt.prepare("UPDATE racuni SET stevilka_racuna = ?, tip_racuna = ?, status_racuna = ?, stranka = ?, projekt = ?, "
																	"avtor_oseba = ?, datum_pricetka = ?, datum_konca = ?, datum_izdaje = ?, datum_placila = ?, "
																	"status_placila = ?, status_racunovodstva = ?, avans = ?, odstotek_avansa = ?, status_oddaje_racuna = ?, "
																	"datum_oddaje_racuna = ?, stara_stevilka_racuna = ?, sklic = ?, datum_placila_avansa = ?, opombe = ? "
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
			sql_vnesi_projekt.bindValue(12, pretvori(pretvori_v_double(ui->txt_avans->text())));
			sql_vnesi_projekt.bindValue(13, pretvori(pretvori_v_double(ui->txt_odstotek_avansa->text())));
			sql_vnesi_projekt.bindValue(14, pretvori(ui->txt_status_oddaje_racuna->currentText()));
			sql_vnesi_projekt.bindValue(15, pretvori(ui->txt_datum_oddaje_racuna->text()));
			sql_vnesi_projekt.bindValue(16, pretvori(ui->txt_stara_stevilka_racuna->text()));
			sql_vnesi_projekt.bindValue(17, pretvori(ui->txt_sklic->text()));
			sql_vnesi_projekt.bindValue(18, pretvori(ui->txt_datum_placila_avansa->text()));
			sql_vnesi_projekt.bindValue(19, pretvori(ui->txt_vnesene_opombe->text()));

			sql_vnesi_projekt.exec();

			if ( ui->rb_predracun->isChecked() && ui->txt_id->text() != "" ) {
				QSqlQuery sql_poisci_otroke;
				sql_poisci_otroke.prepare("SELECT * FROM racuni WHERE stevilka_starsa LIKE '" + pretvori(ui->txt_id->text()) + "'");
				sql_poisci_otroke.exec();
				while ( sql_poisci_otroke.next() ) {
					QSqlQuery sql_spremeni_datum;
					sql_spremeni_datum.prepare("UPDATE racuni SET datum_placila_avansa = ? WHERE id LIKE '" +
																		 sql_poisci_otroke.value(sql_poisci_otroke.record().indexOf("id")).toString() + "'");
					sql_spremeni_datum.bindValue(0, pretvori(ui->txt_datum_placila_avansa->text()));
					sql_spremeni_datum.exec();
					sql_spremeni_datum.clear();
				} // while ( sql_poisci_otroke.next() )
			} // if ( ui->rb_predracun->isChecked() && ui->txt_id->text() != "" )

		} // else ( base.isOpen() )

		base.close();

		// send signal to reload widget
		poslji("racuni");

		// close this window
		close();

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

		sql_fill.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori(tip) + "'");
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
		ui->btn_racun->setEnabled(false);
		ui->btn_predplacilni_racun->setEnabled(false);
		ui->txt_datum_izdaje_racuna->setDate(QDate::currentDate());
		ui->btn_izpisi->setEnabled(false);
		ui->tbl_opravila->setEnabled(false);
		ui->btn_opravilo->setEnabled(false);
		ui->btn_brisi_opravilo->setEnabled(false);

		ui->rb_predplacilo->setEnabled(false);
		ui->rb_racun->setEnabled(false);
		ui->rb_predracun->setChecked(true);

		ui->txt_status_oddaje_racuna->setEnabled(false);

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
		ui->btn_sprejmi->setText("Odpiram");
		ui->btn_izpisi->setEnabled(true);
		ui->tbl_opravila->setEnabled(true);
		ui->btn_opravilo->setEnabled(true);
		ui->btn_brisi_opravilo->setEnabled(true);

		ui->btn_racun->setEnabled(false);
		ui->btn_predplacilni_racun->setEnabled(false);

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

				ui->txt_avans->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("avans")).toString())) + " EUR");
				ui->txt_odstotek_avansa->setText(pretvori_iz_double(prevedi(pretvori_iz_double(sql_napolni.value(sql_napolni.record().indexOf("odstotek_avansa")).toString()))) + " %");

				ui->txt_sklic->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("sklic")).toString()));
				ui->txt_vnesene_opombe->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opombe")).toString()));
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
				ui->txt_stevilka_racuna->setEnabled(false);
				ui->cb_stara_stevilka_racuna->setEnabled(false);
				ui->txt_status_predracuna->setEnabled(false);
				ui->txt_stranka->setEnabled(false);
				ui->txt_projekt->setEnabled(false);
				ui->txt_pricetek->setEnabled(false);
				ui->txt_konec->setEnabled(false);

				ui->txt_sklic->setEnabled(false);
				ui->txt_datum_izdaje_racuna->setEnabled(false);
				ui->txt_rok_placila->setEnabled(false);
				ui->txt_status_oddaje_racuna->setEnabled(false);
				ui->txt_status_placila->setEnabled(true);					//
				ui->txt_status_racunovodstva->setEnabled(true);		//
				ui->txt_status_oddaje_racuna->setEnabled(true);		//
				ui->txt_datum_oddaje_racuna->setEnabled(true);		//
				ui->txt_odstotek_avansa->setEnabled(false);
				ui->txt_datum_placila_avansa->setEnabled(false);

				ui->btn_izracunaj->setEnabled(false);
				ui->btn_sprejmi->setEnabled(true);								//
				ui->btn_opravilo->setEnabled(false);
				ui->btn_brisi_opravilo->setEnabled(false);
			}

			// ce obstaja racun in predplacilni racun, onemogoci shranjevanje podatkov v predracun
			if ( ui->rb_predracun->isChecked() ) {
				QSqlQuery sql_preveri_starse;
				sql_preveri_starse.prepare("SELECT * FROM racuni WHERE stevilka_starsa LIKE '" + pretvori(ui->txt_id->text()) + "'");
				sql_preveri_starse.exec();

				int i = 0;

				while ( sql_preveri_starse.next() ) {
					ui->txt_stevilka_racuna->setEnabled(false);
					ui->cb_stara_stevilka_racuna->setEnabled(false);
					ui->txt_status_predracuna->setEnabled(false);
					ui->txt_stranka->setEnabled(false);
					ui->txt_projekt->setEnabled(false);
					ui->txt_pricetek->setEnabled(false);
					ui->txt_konec->setEnabled(false);

					ui->txt_sklic->setEnabled(false);
					ui->txt_datum_izdaje_racuna->setEnabled(false);
					ui->txt_rok_placila->setEnabled(false);
					ui->txt_status_oddaje_racuna->setEnabled(false);
					ui->txt_status_placila->setEnabled(false);
					ui->txt_status_racunovodstva->setEnabled(true);		//
					ui->txt_datum_oddaje_racuna->setEnabled(false);
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

		}
		base.close();

		napolni();

		napolni_zapise();

		izracunaj(); // calculate the values

		ui->btn_sprejmi->setText("Popravi vnos");

		QString opomba = ui->txt_vnesene_opombe->text();
		QString nova_opomba = opomba + ",";
		QString id = "";
		for ( int a = 0; a <= opomba.count(",,"); a++ ) { // zavrti zanko za vsak id
			id = nova_opomba.left(nova_opomba.indexOf(",,", 0) + 1);
			nova_opomba = nova_opomba.right(nova_opomba.length() - id.length());

			ui->txt_vse_opombe->setText(ui->txt_vse_opombe->text().remove(id));
		}

	}

	izracunaj();

	napolni_vnesene_opombe();
	napolni_vse_opombe();

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

		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori(tip) + "'");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			popusti = popusti + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_popustov")).toString()).toDouble();
			brezddv = brezddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_koncni")).toString()).toDouble();
			ddv = ddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_ddv")).toString()).toDouble();
		}
	}
	base.close();

	if ( !ui->rb_racun->isChecked() ) {
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
		ui->txt_status_racunovodstva->setHidden(false);
		ui->label_7->setHidden(false);

		ui->txt_odstotek_avansa->setEnabled(true);

		ui->btn_racun->setVisible(true);
		ui->btn_predplacilni_racun->setVisible(true);

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

		ui->btn_racun->setVisible(false);
		ui->btn_predplacilni_racun->setVisible(false);

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

		ui->btn_racun->setVisible(false);
		ui->btn_predplacilni_racun->setVisible(false);

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

	if ( ui->btn_sprejmi->text() != "Odpiram" ) {
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "stevilka_racuna");
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

			sql_stetje_racunov.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%." + pretvori(leto) +
																 "' AND tip_racuna LIKE '" + pretvori(tip_racuna) + "' ORDER BY stevilka_racuna ASC");
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

}

void racun::on_txt_datum_izdaje_racuna_dateChanged() {

	stevilka_racuna();

}

void racun::on_txt_status_oddaje_racuna_currentIndexChanged() {

	if ( ui->txt_status_oddaje_racuna->currentText() != "" ) {
		ui->txt_status_predracuna->setEnabled(true);
		ui->txt_datum_oddaje_racuna->setEnabled(true);
	}
	else {
		ui->txt_status_predracuna->setEnabled(false);
		ui->txt_datum_oddaje_racuna->setEnabled(false);
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
				celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
				ui->tbl_zapisi_2->setItem(row, col, celica);

				col++;
				i++;

			}

			row++;

		}
	}
	base.close();

}

void racun::on_tbl_zapisi_2_doubleClicked() {

	QString id = ui->tbl_zapisi_2->selectedItems().takeAt(0)->text();

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
	}
	base.close();

	ui->btn_vnesi_zapis_2->setText("Popravi zapis");

}

void racun::on_btn_vnesi_zapis_2_clicked() {

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
		sql_vnesi_zapis.bindValue(4, pretvori(ui->txt_datum_zapisa_2->text()));
		sql_vnesi_zapis.bindValue(5, pretvori(ui->txt_naslov_zapisa_2->text()));
		sql_vnesi_zapis.bindValue(6, pretvori(ui->txt_opis_zapisa_2->toPlainText()));

		sql_vnesi_zapis.exec();
	}
	base.close();

	ui->txt_id_zapisa_2->setText("");
	ui->txt_datum_zapisa_2->setDateTime(QDateTime::currentDateTime());
	ui->txt_naslov_zapisa_2->setText("");
	ui->txt_opis_zapisa_2->clear();

	ui->btn_vnesi_zapis_2->setText("Vnesi zapis");

	napolni_zapise();

}

void racun::print(QString id) {

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

	QString narocnik = "";
	QString narocnik_naslov_ulica = "";
	QString narocnik_naslov_stevilka = "";
	QString narocnik_naslov_postna_stevilka = "";
	QString narocnik_naslov_posta = "";
	QString narocnik_davcna = "";

	QString racun_tip = "";
	QString racun_stevilka = "";
	QString racun_datum_izdaje = "";
	QString racun_datum_zacetka  = "";
	QString racun_rok_izvedbe = "";
	QString racun_avans = "";
	QString racun_znesek_avansa = "";
	QString racun_rok_placila = "";
	QString racun_stevilka_sklica = "";
	QString racun_opombe = "";

	QString storitev_ime = "";
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
	QString skupaj_ddv_avansa = "";
	QString skupaj_se_za_placati = "";
	QString skupaj_se_za_placati_ddv = "";

	double skupajbrezddv = 0.0;
	double skupajddvodosnove20 = 0.0;
	double skupajddvodosnove85 = 0.0;
	double skupajddvodosnove00 = 0.0;
	double skupajznesek = 0.0;
	double skupajznesekavansa = 0.0;
	double skupajddvavansa = 0.0;
	double skupajsezaplacati = 0.0;
	double skupajsezaplacatiddv = 0.0;

	// priprava baze in polnenje spremenljivk (razen storitve)
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
		// podjetje izvemo iz stevilke podjetja, ki ji pripada uporabnik, kateri tiska racun
		QSqlQuery sql_podjetje;
		sql_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + pretvori(vApp->firm()) + "'");
		sql_podjetje.exec();
		if ( sql_podjetje.next() ) {
			podjetje_logo = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("logotip")).toString());
			podjetje_kratki = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("ime")).toString());
			podjetje_polni = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("polnoime")).toString());
			podjetje_url = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("url")).toString());
			podjetje_email = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("email")).toString());
			podjetje_ddv = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("davcna")).toString());
			podjetje_naslov_ulica = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("naslov")).toString());
			podjetje_naslov_stevilka = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("naslov_st")).toString());
			podjetje_naslov_postna_stevilka = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("postna_stevilka")).toString());
			podjetje_naslov_posta = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("posta")).toString());
			podjetje_trr = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("tekoci_racun")).toString());
			podjetje_telefon = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("telefon")).toString());
			if ( podjetje_telefon == "+(0)/--" ) {
				podjetje_telefon = prevedi(sql_podjetje.value(sql_podjetje.record().indexOf("gsm")).toString());
			}

			// vstavi presledke med vsake stiri znake pri trr-ju
			podjetje_trr.insert(4, " ");
			podjetje_trr.insert(9, " ");
			podjetje_trr.insert(14, " ");
			podjetje_trr.insert(19, " ");
		}
		sql_podjetje.clear();

		// napolni vrednosti racuna
		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
		sql_racun.exec();
		if ( sql_racun.next() ) {
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
			racun_rok_placila = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_placila_avansa")).toString());
			racun_stevilka_sklica = prevedi(sql_racun.value(sql_racun.record().indexOf("sklic")).toString());
			racun_opombe = prevedi(sql_racun.value(sql_racun.record().indexOf("opombe")).toString());

			podjetje_oseba = prevedi(sql_racun.value(sql_racun.record().indexOf("avtor_oseba")).toString());
			narocnik = prevedi(sql_racun.value(sql_racun.record().indexOf("stranka")).toString());
		}

		// napolni vrednosti avtorja racuna
		QSqlQuery sql_uporabnik;
		sql_uporabnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + pretvori(podjetje_oseba) + "'");
		sql_uporabnik.exec();
		if ( sql_uporabnik.next() ) {
			podjetje_oseba = prevedi(sql_uporabnik.value(sql_uporabnik.record().indexOf("priimek")).toString()) + " " +
											 prevedi(sql_uporabnik.value(sql_uporabnik.record().indexOf("ime")).toString());
			podjetje_oseba_naziv = prevedi(sql_uporabnik.value(sql_uporabnik.record().indexOf("naziv")).toString());
		}

		// napolni vrednosti avtorjev naziv
		QSqlQuery sql_naziv;
		sql_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + pretvori(podjetje_oseba_naziv) + "'");
		sql_naziv.exec();
		if ( sql_naziv.next() ) {
			podjetje_oseba_naziv = prevedi(sql_naziv.value(sql_naziv.record().indexOf("naziv")).toString());
		}

		// napolni vrednosti narocnika
		QSqlQuery sql_narocnik;
		sql_narocnik.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(narocnik) + "'");
		sql_narocnik.exec();
		if ( sql_narocnik.next() ) {
			if ( prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("tip")).toString()) == "2" ) { // pravna
				narocnik = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("priimek")).toString());
				narocnik_davcna = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString());
			}
			else {
				narocnik = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("priimek")).toString()) + " " +
									 prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("ime")).toString());
			}
			narocnik_naslov_ulica = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("naslov")).toString());
			narocnik_naslov_stevilka = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("naslov_st")).toString());
			narocnik_naslov_postna_stevilka = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("postna_stevilka")).toString());
			narocnik_naslov_posta = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("posta")).toString());
		}

	}
	base.close();

	/**
		* Odpremo sifrant z besedilom potnega naloga
		* Besedilo bomo vztavljali z readLine
		**/

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

	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	QTextStream racun(&datoteka);

	// tiskanje
	QString besedilo = "";
	QString besedilo_noga = racun.readLine();

	QPrinter printer;

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Natisni racun"));
//	printer.setOutputFormat(QPrinter::PdfFormat);
//	printer.setOutputFileName("nonwritable.pdf");
	printer.setPaperSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

	 if (dialog->exec() == QDialog::Accepted) {
		QPainter painter;

		if (! painter.begin(&printer))  { // failed to open file
			qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
			return;
		}

		int i = 1;
		double pozicija = 0;
		double visina_vrstice = 0;
		double sirina_besedila = 0;
		double razmik_med_vrsticami = 0;
		double faktor_razmika_med_vrsticami_1 = 0.3; // pri tabelah
		double faktor_razmika_med_vrsticami_2 = 0.1; // pri besedilu

		// nastravitve
		QFont normalno("Arial", 10);
		QFont debelo("Arial", 10, QFont::Bold);
		QFont malo("Arial", 8);

		QPen *debel_svincnik = new QPen;
		QPen *tanek_svincnik = new QPen;
		debel_svincnik->setWidth(2.0);
		tanek_svincnik->setWidth(1.0);

// glava
	// logotip podjetja
		QImage logotip(podjetje_logo);
		painter.drawImage(0, pozicija, QPixmap::fromImage(logotip).scaledToHeight(73).toImage(), 0, 0, 0, 0, Qt::ColorOnly);

		pozicija = 10; // zaradi velikosti logotipa
	// ime in naslov podjetja
		painter.setFont(debelo);
		// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_kratki);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_kratki);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, polni naziv)
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni);
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_polni);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, ulica)
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, pos)
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
		// nova vrstica
		pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

		// crta pod glavo
		painter.setPen(*tanek_svincnik);
		painter.drawLine(0, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

	// noga
		// crta nad nogo
		painter.setPen(*tanek_svincnik);
		painter.drawLine(0, printer.height() - 20, printer.width(), printer.height() - 25);

		// podatki o podjetju
		painter.setFont(malo);
		// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
		velikost_besedila = painter.boundingRect(0, printer.height() - 15, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni +
																									 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " +
																									 podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, printer.height() - 15, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_polni +
										 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " + podjetje_naslov_postna_stevilka +
										 " " + podjetje_naslov_posta);

		// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
		velikost_besedila = painter.boundingRect(0, printer.height() - 15 + visina_vrstice, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_url +
																									 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, printer.height() - 15 + visina_vrstice, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_url +
										 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);

	// podatki o (pred)racunu
		// narocnik
		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine();
		double pozicija_zgoraj = pozicija;
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Narocnik: ")
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
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
		velikost_besedila = painter.boundingRect(0, 0, printer.width() / 2, 0, Qt::AlignJustify | Qt::TextWordWrap, narocnik);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width() /2 , visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo velikost kvadrata, ki ga tvori besedilo (naslov - ulica)
		velikost_besedila = painter.boundingRect(0, 0, printer.width() / 2, 0, Qt::AlignJustify | Qt::TextWordWrap, narocnik_naslov_ulica + " " + narocnik_naslov_stevilka);
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width() / 2, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_naslov_ulica + " " + narocnik_naslov_stevilka);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dolocimo velikost kvadrata, ki ga tvori besedilo (naslov - posta)
		velikost_besedila = painter.boundingRect(0, 0, printer.width() / 2, 0, Qt::AlignJustify | Qt::TextWordWrap, narocnik_naslov_postna_stevilka + " " + narocnik_naslov_posta);
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width() / 2, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_naslov_postna_stevilka + " " + narocnik_naslov_posta);
		// nova vrstica
		pozicija += visina_vrstice * 6 + razmik_med_vrsticami * 6;

		besedilo = racun.readLine();
		if ( narocnik_davcna != "" ) {
			painter.setFont(debelo);
			// dolocimo velikost kvadrata, ki ga tvori besedilo (davcna stevilka)
			velikost_besedila = painter.boundingRect(0, 0, printer.width() / 2, 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.drawText(QRectF(0, pozicija, printer.width() / 2, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			painter.setFont(normalno);
			painter.drawText(QRectF(velikost_besedila.width(), pozicija, printer.width() / 2, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, narocnik_davcna);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}

	// podatki o (pred)racunu
		pozicija = pozicija_zgoraj;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("(Pred)Racun st.: ")
		velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_stevilka);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Datum izdaje: ")
		velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_izdaje);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Kraj izdaje: ")
		velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_posta);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		if ( racun_tip != "3" ) {
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Predviden rok izvedbe: ")
			velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_izvedbe);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}
		else {
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Pricetek dela: ")
			velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_zacetka);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;

			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Zakljucek dela: ")
			velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_izvedbe);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Rok placila (avansa): ")
		velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_placila);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Nacin placila: ")
		velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		besedilo = racun.readLine();
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("TRR Abanka: ")
		velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_trr);
		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		if ( racun_tip != "2" ) { // samo pri predracunu in racunu
			// nastavi parametre
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// dolocimo velikost kvadrata, ki ga tvori besedilo ("Sklic: ")
			velikost_besedila = painter.boundingRect(printer.width() / 2, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// nastavimo parametre
			visina_vrstice = velikost_besedila.height();
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() / 2, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_stevilka_sklica);
			// nova vrstica
			pozicija += visina_vrstice + razmik_med_vrsticami;
		}
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta pod podatki
		painter.setPen(*debel_svincnik);
		painter.drawLine(0, pozicija, printer.width(), pozicija);

	// storitve
		// glava tabele
		// nastavi parametre
		painter.setPen(*tanek_svincnik);
		painter.setFont(debelo);

		double sirina_vecja = printer.width() / 8;
		double sirina_manjsa = printer.width() / 11;
		double crta_1 = 0;
		double crta_2 = printer.width() - sirina_vecja - sirina_manjsa * 5;
		double crta_3 = crta_2 + sirina_manjsa;
		double crta_4 = crta_3 + sirina_manjsa;
		double crta_5 = crta_4 + sirina_vecja;
		double crta_6 = crta_5 + sirina_manjsa;
		double crta_7 = crta_6 + sirina_manjsa;
		double crta_8 = printer.width();

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Storitev")
		velikost_besedila = painter.boundingRect(crta_1, pozicija, crta_2, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height() * 3 + razmik_med_vrsticami;
		// natisnemo besedilo
		painter.drawText(QRectF(crta_1, pozicija, crta_2, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Kolicina")
		velikost_besedila = painter.boundingRect(crta_2, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_2, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Enota")
		velikost_besedila = painter.boundingRect(crta_3, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_3, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Cena na enoto brez DDV")
		velikost_besedila = painter.boundingRect(crta_4, pozicija, sirina_vecja, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_4, pozicija, sirina_vecja, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Popust %")
		velikost_besedila = painter.boundingRect(crta_5, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_5, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("DDV %")
		velikost_besedila = painter.boundingRect(crta_6, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_6, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		besedilo = racun.readLine();
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Vrednost brez DDV")
		velikost_besedila = painter.boundingRect(crta_7, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// natisnemo besedilo
		painter.drawText(QRectF(crta_7, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, besedilo);

		// nova pozicija = nova vrstica v tabeli
		pozicija += visina_vrstice;

		// crta pod storitvami
		painter.setPen(*debel_svincnik);
		painter.drawLine(0, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += razmik_med_vrsticami / 2;

		// priprava baze in polnenje storitve
		QSqlDatabase base_1 = QSqlDatabase::addDatabase("QSQLITE");
		base_1.setDatabaseName(dbase_path);
		base_1.database();
		base_1.open();
		if(base_1.isOpen() != true){
			QMessageBox msgbox;
			msgbox.setText("Baze ni bilo moc odpreti");
			msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
			msgbox.exec();
		}
		else {
			// baza je odprta

			QSqlQuery sql_storitve;
			int trenutna_pozicija = 0;
			int koncna_pozicija = 0;

			sql_storitve.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id) + "' AND tip_racuna LIKE '" + pretvori(racun_tip) + "'");
			sql_storitve.exec();
			while ( sql_storitve.next() ) {
				koncna_pozicija++;
			}
			sql_storitve.clear();

			sql_storitve.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id) + "' AND tip_racuna LIKE '" + pretvori(racun_tip) + "'");
			sql_storitve.exec();
			while ( sql_storitve.next() ) {
				storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_skupina")).toString()) + ": " +
											 prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_storitev")).toString());

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
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_f2")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_komb1")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_komb2")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_stranka")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_kupon")).toString())).toDouble();
				popusti += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("popust_akcija")).toString())).toDouble();
				storitev_popust = QString::number(popusti, 'f', 1).replace(".", ",");

				// ceno storitve brez ddv
				double cena = pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesek_koncni")).toString())).toDouble();
				storitev_cena_brez_ddv = QString::number(cena, 'f', 2).replace(".", ",");

				// izracunaj zdruzke
				skupajbrezddv += cena;
				if ( storitev_odstotek_ddv == "20,0" ) {
					skupajddvodosnove20 += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesek_ddv")).toString())).toDouble();
				}
				if ( storitev_odstotek_ddv == "8,5" ) {
					skupajddvodosnove85 += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesek_ddv")).toString())).toDouble();
				}
				if ( storitev_odstotek_ddv == "0,0" ) {
					skupajddvodosnove00 += pretvori_v_double(prevedi(sql_storitve.value(sql_storitve.record().indexOf("znesek_ddv")).toString())).toDouble();
				}

				// preveri, ce je potrebna nova stran
				if( pozicija + 50 >= printer.height() ) {
					printer.newPage();
					pozicija = 0;
					i++;
					// glava
						// logotip podjetja
							QImage logotip(podjetje_logo);
							painter.drawImage(0, pozicija, QPixmap::fromImage(logotip).scaledToHeight(73).toImage(), 0, 0, 0, 0, Qt::ColorOnly);

							pozicija = 10;
						// ime in naslov podjetja
							painter.setFont(debelo);
							// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
							QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_kratki);
							// nastavimo parametre
							visina_vrstice = velikost_besedila.height();
							razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
							// natisnemo besedilo
							painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_kratki);
							// nova vrstica
							pozicija += visina_vrstice + razmik_med_vrsticami;

							// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, polni naziv)
							velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni);
							// natisnemo besedilo
							painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_polni);
							// nova vrstica
							pozicija += visina_vrstice + razmik_med_vrsticami;

							// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, ulica)
							velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
							// natisnemo besedilo
							painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
							// nova vrstica
							pozicija += visina_vrstice + razmik_med_vrsticami;

							// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, pos)
							velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
							// natisnemo besedilo
							painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
							// nova vrstica
							pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

							// crta pod glavo
							painter.setPen(*tanek_svincnik);
							painter.drawLine(0, pozicija, printer.width(), pozicija);
							// nova vrstica
							pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;
							// st. strani

							// dolocimo velikost kvadrata, ki ga tvori besedilo (st. strani)
							velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
							// natisnemo besedilo
							painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
							// nova vrstica
							pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

							// crta pod glavo
							painter.setPen(*tanek_svincnik);
							painter.drawLine(0, pozicija, printer.width(), pozicija);
							// nova vrstica
							pozicija += visina_vrstice + razmik_med_vrsticami * 2;

						// noga
							// crta nad nogo
							painter.setPen(*tanek_svincnik);
							painter.drawLine(0, printer.height() - 20, printer.width(), printer.height() - 25);

							// podatki o podjetju
							painter.setFont(malo);
							// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
							velikost_besedila = painter.boundingRect(0, printer.height() - 15, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni +
																														 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " +
																														 podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
							// nastavimo parametre
							visina_vrstice = velikost_besedila.height();
							razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
							// natisnemo besedilo
							painter.drawText(QRectF(0, printer.height() - 15, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_polni +
															 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " + podjetje_naslov_postna_stevilka +
															 " " + podjetje_naslov_posta);

							// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
							velikost_besedila = painter.boundingRect(0, printer.height() - 15 + visina_vrstice, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_url +
																														 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
							// nastavimo parametre
							visina_vrstice = velikost_besedila.height();
							razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
							// natisnemo besedilo
							painter.drawText(QRectF(0, printer.height() - 15 + visina_vrstice, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_url +
															 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
				}

				// tiskanje storitve
				painter.setFont(normalno);
				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Storitev")
				velikost_besedila = painter.boundingRect(crta_1, pozicija, crta_2, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_ime);
				// nastavimo parametre
				visina_vrstice = velikost_besedila.height() + razmik_med_vrsticami;
				// natisnemo besedilo
				painter.drawText(QRectF(crta_1, pozicija, crta_2, visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap | Qt::AlignVCenter, storitev_ime);

				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Kolicina")
				velikost_besedila = painter.boundingRect(crta_2, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_kolicina);
				// natisnemo besedilo
				painter.drawText(QRectF(crta_2, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_kolicina);

				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Enota")
				velikost_besedila = painter.boundingRect(crta_3, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_enota);
				// natisnemo besedilo
				painter.drawText(QRectF(crta_3, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_enota);

				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Cena na enoto brez DDV")
				velikost_besedila = painter.boundingRect(crta_4, pozicija, sirina_vecja, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_cena_brez_ddv_na_enoto);
				// natisnemo besedilo
				painter.drawText(QRectF(crta_4, pozicija, sirina_vecja, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_cena_brez_ddv_na_enoto);

				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Popust %")
				velikost_besedila = painter.boundingRect(crta_5, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_popust);
				// natisnemo besedilo
				painter.drawText(QRectF(crta_5, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_popust);

				// dolocimo velikost kvadrata, ki ga tvori besedilo ("DDV %")
				velikost_besedila = painter.boundingRect(crta_6, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_odstotek_ddv);
				// natisnemo besedilo
				painter.drawText(QRectF(crta_6, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_odstotek_ddv);

				// dolocimo velikost kvadrata, ki ga tvori besedilo ("Vrednost brez DDV")
				velikost_besedila = painter.boundingRect(crta_7, pozicija, sirina_manjsa, pozicija, Qt::AlignJustify | Qt::TextWordWrap, storitev_cena_brez_ddv);
				// natisnemo besedilo
				painter.drawText(QRectF(crta_7, pozicija, sirina_manjsa, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, storitev_cena_brez_ddv);

				// nova pozicija = nova vrstica v tabeli
				pozicija += visina_vrstice + razmik_med_vrsticami / 2;

				trenutna_pozicija++;
				if ( trenutna_pozicija < koncna_pozicija ) {
				// crta pod storitvami
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, pozicija, printer.width(), pozicija);
				}
		//		// nova vrstica
		//		pozicija += razmik_med_vrsticami;

			} // while ( sql_storitve.next() )
		} // base.isOpen()
		base_1.close();

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
		skupaj_ddv_od_osnove_20 = QString::number(skupajddvodosnove20, 'f', 2).replace(".", ",") + " EUR";
		skupaj_ddv_od_osnove_85 = QString::number(skupajddvodosnove85, 'f', 2).replace(".", ",") + " EUR";
		skupaj_ddv_od_osnove_00 = QString::number(skupajddvodosnove00, 'f', 2).replace(".", ",") + " EUR";
		skupajznesek = skupajbrezddv + skupajddvodosnove20 + skupajddvodosnove85 + skupajddvodosnove00;
		skupaj_znesek = QString::number(skupajznesek, 'f', 2).replace(".", ",") + " EUR";
		skupaj_znesek_avansa = racun_znesek_avansa.replace(".", ",") + " EUR";
		racun_avans = racun_avans.replace(".", ",") + " % ";
		skupajddvavansa = 16.66667 / 100 * pretvori_v_double(racun_znesek_avansa).toDouble();
		skupaj_ddv_avansa = QString::number(skupajddvavansa, 'f', 2).replace(".", ",") + " EUR";
		skupaj_se_za_placati = QString::number(skupajznesek - pretvori_v_double(racun_znesek_avansa).toDouble(), 'f', 2).replace(".", ",") + " EUR";
		skupaj_se_za_placati_ddv = QString::number(skupajddvodosnove20 - skupajddvavansa, 'f', 2).replace(".", ",") + " EUR";

		// preveri, ali je morda potreben preskok na novo stran
		if( pozicija + 250 >= printer.height() ) {
			printer.newPage();
			pozicija = 0;
			i++;
			// glava
				// logotip podjetja
					QImage logotip(podjetje_logo);
					painter.drawImage(0, pozicija, QPixmap::fromImage(logotip).scaledToHeight(73).toImage(), 0, 0, 0, 0, Qt::ColorOnly);

					pozicija = 10;
				// ime in naslov podjetja
					painter.setFont(debelo);
					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
					QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_kratki);
					// nastavimo parametre
					visina_vrstice = velikost_besedila.height();
					razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_kratki);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami;

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, polni naziv)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni);
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_polni);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami;

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, ulica)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami;

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, pos)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
					// nova vrstica
					pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

					// crta pod glavo
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, pozicija, printer.width(), pozicija);
					// nova vrstica
					pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

					// st. strani

					// dolocimo velikost kvadrata, ki ga tvori besedilo (st. strani)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
					// nova vrstica
					pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

					// crta pod glavo
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, pozicija, printer.width(), pozicija);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami * 2;

				// noga
					// crta nad nogo
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, printer.height() - 20, printer.width(), printer.height() - 25);

					// podatki o podjetju
					painter.setFont(malo);
					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
					velikost_besedila = painter.boundingRect(0, printer.height() - 15, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni +
																												 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " +
																												 podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
					// nastavimo parametre
					visina_vrstice = velikost_besedila.height();
					razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
					// natisnemo besedilo
					painter.drawText(QRectF(0, printer.height() - 15, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_polni +
													 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " + podjetje_naslov_postna_stevilka +
													 " " + podjetje_naslov_posta);

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
					velikost_besedila = painter.boundingRect(0, printer.height() - 15 + visina_vrstice, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_url +
																												 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
					// nastavimo parametre
					visina_vrstice = velikost_besedila.height();
					razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
					// natisnemo besedilo
					painter.drawText(QRectF(0, printer.height() - 15 + visina_vrstice, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_url +
													 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
		}

		// nastavi parametre
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo ("Skupaj brez DDV: ")
		velikost_besedila = painter.boundingRect(printer.width()  * 3 / 5, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "DDV 20,0 % od osnove: ");
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height() * 2;
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_brez_ddv);
		// nova vrstica * 2
		pozicija += visina_vrstice;

		// crta
		painter.setPen(*tanek_svincnik);
		painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += razmik_med_vrsticami;

		/**
			* Najprej preberemo vrstico datoteke, nato klicemo zanko
			* tako imamo lahko staticno datoteko z besedili in naknadno izlocimo
			* nepotrebne vrstice. V primeru besedilnih prevodov bo seveda to izpadlo.
			**/

		// nastavi parametre (20,0% DDV)
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		if ( skupaj_ddv_od_osnove_20 != "0,00 EUR" ) {
		// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_ddv_od_osnove_20);
			// nova vrstica * 2
			pozicija += visina_vrstice;

			// crta pod storitvami
			painter.setPen(*tanek_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;
		}

		// nastavi parametre (8,5% DDV)
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		if ( skupaj_ddv_od_osnove_85 != "0,00 EUR" ) {
		// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_ddv_od_osnove_85);
			// nova vrstica * 2
			pozicija += visina_vrstice;

			// crta
			painter.setPen(*tanek_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;
		}

		// nastavi parametre (0,0% DDV)
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		if ( skupaj_ddv_od_osnove_00 != "0,00 EUR" ) {
		// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_ddv_od_osnove_00);
			// nova vrstica * 2
			pozicija += visina_vrstice;

			// crta
			painter.setPen(*tanek_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;
		}

		// nastavi parametre ("Skupaj v EUR: ")
		painter.setFont(debelo);
		besedilo = racun.readLine() + " ";
		// natisnemo besedilo
		painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
		// natisnemo besedilo
		painter.setFont(normalno);
		painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_znesek);
		// nova vrstica * 2
		pozicija += visina_vrstice;

		// crta
		painter.setPen(*debel_svincnik);
		painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
		// nova vrstica
		pozicija += razmik_med_vrsticami;

		if ( racun_tip == "1" ) {
			// nastavi parametre ("% avans: ")
			painter.setFont(debelo);
			besedilo = racun_avans + " " + racun.readLine() + " ";
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija,velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_znesek_avansa);
			// nova vrstica * 2
			pozicija += visina_vrstice;

			// crta pod storitvami
			painter.setPen(*debel_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami * 2 + visina_vrstice;
		}
		if ( racun_tip != "1" ) { // predplacilo in racun
			// nastavi parametre ("Predplacilo brez DDV: ")
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_znesek_avansa);
			// nova vrstica
			pozicija += visina_vrstice;

			// crta pod storitvami
			painter.setPen(*tanek_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;

			// nastavi parametre ("DDV od predplacila: ")
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice * 2), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice * 2), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_ddv_avansa);
			// nova vrstica * 2
			pozicija += visina_vrstice * 2;

			// crta pod storitvami
			painter.setPen(*debel_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;
		}
		if ( racun_tip == "3" ) { // samo racun
			// nastavi parametre ("Se za placilo: ")
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_se_za_placati);
			// nova vrstica
			pozicija += visina_vrstice;

			// crta pod storitvami
			painter.setPen(*tanek_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami;

			// nastavi parametre ("Od tega DDV 20,0%: ")
			painter.setFont(debelo);
			besedilo = racun.readLine() + " ";
			// natisnemo besedilo
			painter.drawText(QRectF(printer.width() * 3 / 5, pozicija, velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap | Qt::AlignVCenter, besedilo);
			// natisnemo besedilo
			painter.setFont(normalno);
			painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_se_za_placati_ddv);
			// nova vrstica * 2
			pozicija += visina_vrstice;

			// crta pod storitvami
			painter.setPen(*debel_svincnik);
			painter.drawLine(printer.width()  * 3 / 5 - 10, pozicija, printer.width(), pozicija);
			// nova vrstica
			pozicija += razmik_med_vrsticami * 2 + visina_vrstice;
		}

	// opombe
		// priprava baze in polnenje storitve
		QSqlDatabase base_2 = QSqlDatabase::addDatabase("QSQLITE");
		base_2.setDatabaseName(dbase_path);
		base_2.database();
		base_2.open();
		if(base_2.isOpen() != true){
			QMessageBox msgbox;
			msgbox.setText("Baze ni bilo moc odpreti");
			msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
			msgbox.exec();
		}
		else {
			// baza je odprta

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
				// preveri, ce je morda potreben preskok na novo stran
				if( pozicija + 30 + visina_vrstice >= printer.height() ) {
						printer.newPage();
						pozicija = 0;
						i++;
						// glava
							// logotip podjetja
								QImage logotip(podjetje_logo);
								painter.drawImage(0, pozicija, QPixmap::fromImage(logotip).scaledToHeight(73).toImage(), 0, 0, 0, 0, Qt::ColorOnly);

								pozicija = 10;
							// ime in naslov podjetja
								painter.setFont(debelo);
								// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
								QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_kratki);
								// nastavimo parametre
								visina_vrstice = velikost_besedila.height();
								razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
								// natisnemo besedilo
								painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_kratki);
								// nova vrstica
								pozicija += visina_vrstice + razmik_med_vrsticami;

								// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, polni naziv)
								velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni);
								// natisnemo besedilo
								painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_polni);
								// nova vrstica
								pozicija += visina_vrstice + razmik_med_vrsticami;

								// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, ulica)
								velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
								// natisnemo besedilo
								painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
								// nova vrstica
								pozicija += visina_vrstice + razmik_med_vrsticami;

								// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, pos)
								velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
								// natisnemo besedilo
								painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
								// nova vrstica
								pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

								// crta pod glavo
								painter.setPen(*tanek_svincnik);
								painter.drawLine(0, pozicija, printer.width(), pozicija);
								// nova vrstica
								pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

								// st. strani

								// dolocimo velikost kvadrata, ki ga tvori besedilo (st. strani)
								velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
								// natisnemo besedilo
								painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
								// nova vrstica
								pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

								// crta pod glavo
								painter.setPen(*tanek_svincnik);
								painter.drawLine(0, pozicija, printer.width(), pozicija);
								// nova vrstica
								pozicija += visina_vrstice + razmik_med_vrsticami * 2;

								// podatki o podjetju
								painter.setFont(malo);
								// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
								velikost_besedila = painter.boundingRect(0, printer.height() - 15, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni +
																															 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " +
																															 podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
								// nastavimo parametre
								visina_vrstice = velikost_besedila.height();
								razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
								// natisnemo besedilo
								painter.drawText(QRectF(0, printer.height() - 15, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_polni +
																 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " + podjetje_naslov_postna_stevilka +
																 " " + podjetje_naslov_posta);

								// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
								velikost_besedila = painter.boundingRect(0, printer.height() - 15 + visina_vrstice, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_url +
																															 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
								// nastavimo parametre
								visina_vrstice = velikost_besedila.height();
								razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
								// natisnemo besedilo
								painter.drawText(QRectF(0, printer.height() - 15 + visina_vrstice, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_url +
																 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
					}
				// natisnemo besedilo
				painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, beseda);
				// nova vrstica
				pozicija += visina_vrstice + razmik_med_vrsticami;

			}

		}
		base_2.close();

		pozicija += visina_vrstice + 2 * razmik_med_vrsticami;

	// podpis
		// preveri, ali je morda potreben preskok na novo stran
		if( pozicija + 30 >= printer.height() ) {
			printer.newPage();
			pozicija = 0;
			i++;
			// glava
				// logotip podjetja
					QImage logotip(podjetje_logo);
					painter.drawImage(0, pozicija, QPixmap::fromImage(logotip).scaledToHeight(73).toImage(), 0, 0, 0, 0, Qt::ColorOnly);

					pozicija = 10;
				// ime in naslov podjetja
					painter.setFont(debelo);
					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
					QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_kratki);
					// nastavimo parametre
					visina_vrstice = velikost_besedila.height();
					razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_kratki);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami;

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, polni naziv)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni);
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_polni);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami;

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, ulica)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_ulica + " " + podjetje_naslov_stevilka);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami;

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, pos)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
					// nova vrstica
					pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

					// crta pod glavo
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, pozicija, printer.width(), pozicija);
					// nova vrstica
					pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

					// st. strani

					// dolocimo velikost kvadrata, ki ga tvori besedilo (st. strani)
					velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
					// natisnemo besedilo
					painter.drawText(QRectF(0, pozicija, printer.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, "Stran: " + QString::number(i, 10));
					// nova vrstica
					pozicija += visina_vrstice * 2 + razmik_med_vrsticami * 2;

					// crta pod glavo
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, pozicija, printer.width(), pozicija);
					// nova vrstica
					pozicija += visina_vrstice + razmik_med_vrsticami * 2;

				// noga
					// crta nad nogo
					painter.setPen(*tanek_svincnik);
					painter.drawLine(0, printer.height() - 20, printer.width(), printer.height() - 25);

					// podatki o podjetju
					painter.setFont(malo);
					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
					velikost_besedila = painter.boundingRect(0, printer.height() - 15, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_polni +
																												 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " +
																												 podjetje_naslov_postna_stevilka + " " + podjetje_naslov_posta);
					// nastavimo parametre
					visina_vrstice = velikost_besedila.height();
					razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
					// natisnemo besedilo
					painter.drawText(QRectF(0, printer.height() - 15, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_polni +
													 " | " + podjetje_kratki + " | " + podjetje_naslov_ulica + " " + podjetje_naslov_stevilka + ", " + podjetje_naslov_postna_stevilka +
													 " " + podjetje_naslov_posta);

					// dolocimo velikost kvadrata, ki ga tvori besedilo (podjetje, kratki naziv)
					velikost_besedila = painter.boundingRect(0, printer.height() - 15 + visina_vrstice, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_url +
																												 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
					// nastavimo parametre
					visina_vrstice = velikost_besedila.height();
					razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
					// natisnemo besedilo
					painter.drawText(QRectF(0, printer.height() - 15 + visina_vrstice, printer.width(), visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap, podjetje_url +
													 " | " + podjetje_telefon + " | " + podjetje_email + " | " + besedilo_noga + " " + podjetje_ddv);
		}

		// naziv
		painter.setFont(normalno);
		besedilo = racun.readLine() + " ";
		// dolocimo velikost kvadrata, ki ga tvori besedilo (naziv podpisnika)
		velikost_besedila = painter.boundingRect(0, pozicija, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, podjetje_oseba_naziv.toLower());
		// nastavimo parametre
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_2; // razmik med vrsticami, za lazje branje dokumenta
		// natisnemo besedilo
		painter.drawText(QRectF(0, pozicija, printer.width() - velikost_besedila.width(), visina_vrstice), Qt::AlignRight | Qt::TextWordWrap, besedilo);
		painter.drawText(QRectF(printer.width() - velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, podjetje_oseba_naziv.toLower());
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

}

void racun::on_txt_vse_opombe_textChanged() {

	if ( ui->btn_sprejmi->text() != "Odpiram" ) {
		napolni_vse_opombe();
	}

}

void racun::on_txt_vnesene_opombe_textChanged() {

	if ( ui->btn_sprejmi->text() != "Odpiram" ) {
		napolni_vnesene_opombe();
	}

}

void racun::napolni_vse_opombe() {

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

		QString opombe = ui->txt_vse_opombe->text().replace(",,", ",");
		QString preostanek_opombe = opombe;
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

		int row = 0;
		for ( int a = 0; a < opombe.count(","); a++ ) {
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - 1); // odstranimo zacetno vejico
			int vejica = preostanek_opombe.indexOf(",", 0); // dobimo pozicijo prve vejice
			id = preostanek_opombe.left(vejica); // dobimo id opombe
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - id.length() ); // opombe skrajsamo za dolzino enega id-ja

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
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					ui->tbl_vse_opombe->setItem(row, col, celica);

					col++;
					i++;

				}
			}
			row++;
		}
	}
	base.close();

}

void racun::napolni_vnesene_opombe() {

	QString opombe = ui->txt_vnesene_opombe->text().replace(",,", ",");
	QString preostanek_opombe = opombe;
	QString id = "";

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

		int row = 0;
		for ( int a = 0; a < opombe.count(","); a++ ) {
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - 1); // odstranimo zacetno vejico
			int vejica = preostanek_opombe.indexOf(",", 0); // dobimo pozicijo prve vejice
			id = preostanek_opombe.left(vejica); // dobimo id opombe
			preostanek_opombe = preostanek_opombe.right(preostanek_opombe.length() - id.length() ); // opombe skrajsamo za dolzino enega id-ja

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
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					ui->tbl_vnesene_opombe->setItem(row, col, celica);

					col++;
					i++;

				}
			}
			row++;
		}
	}
	base.close();

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

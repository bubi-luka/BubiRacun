#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QObject>

#include "opravila.h"
#include "ui_opravila.h"
#include "kodiranje.h"
#include "varnost.h"

opravila::opravila(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::opravila)
{
		ui->setupUi(this);

		// izbrisi polja
		ui->txt_id->setText("");
		ui->txt_id_stranka->setText("");
		ui->txt_stranka->setText("");
		ui->txt_id_projekt->setText("");
		ui->txt_projekt->setText("");
		ui->txt_id_racun->setText("");
		ui->txt_racun->setText("");
		ui->txt_id_tip->setText("");
		ui->txt_tip->setText("");
		ui->txt_rocni_vnos_storitve->setText("");

		ui->txt_sklop->clear();
		ui->txt_skupina->clear();
		ui->txt_storitev->clear();
		ui->txt_enota->clear();
		ui->txt_urna_postavka_brez_ddv->setText("");
		ui->txt_ddv->clear();
		ui->txt_urna_postavka->setText("");

		ui->cb_vikend->setText("");
		ui->cb_hitrost->setText("");
		ui->cb_zapleti->setText("");

		ui->txt_storitev_pribitek->setText("");
		ui->txt_pribitek->setText("");
		ui->txt_nova_urna_postavka_brez_ddv->setText("");
		ui->txt_novi_ddv->setText("");
		ui->txt_nova_urna_postavka->setText("");

		ui->rb_predracun->setText("");
		ui->rb_casovnice->setText("");
		ui->rb_rocni_vnos->setText("");
		ui->txt_predracun->setText("");
		ui->txt_casovnice->setText("");
		ui->txt_rocni_vnos->setText("");

		ui->txt_storitev_na_racunu->setText("");
		ui->txt_stevilo_opravljenih_ur->setText("");
		ui->txt_urna_postavka_na_racunu->setText("");
		ui->txt_stopnja_ddv->setText("");
		ui->txt_popusti->setText("");
		ui->txt_znesek_brez_ddv_na_racunu->setText("");
		ui->txt_znesek_ddv_na_racunu->setText("");
		ui->txt_znesek_na_racunu->setText("");

		ui->cb_popust_fb1->setText("");
		ui->cb_popust_fb2->setText("");
		ui->cb_popust_komb1->setText("");
		ui->cb_popust_komb2->setText("");
		ui->cb_popust_stalna->setText("");
		ui->cb_popust_kupon->setText("");
		ui->cb_popust_akcija->setText("");
		ui->cb_podrazitev_vikend->setText("");
		ui->cb_podrazitev_hitrost->setText("");
		ui->cb_podrazitev_zapleti->setText("");
		ui->txt_popust_fb1->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_fb2->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_komb1->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_komb2->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_stalna_stranka->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_kupon->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_akcija->setText(pretvori_iz_double("0,0 %"));
		ui->txt_vsi_popusti_facebook_2->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popusti_skupaj_2->setText(pretvori_iz_double("0,0 %"));

		ui->tbl_casovnice->clear();

		// onemogoci polja s popusti
		ui->txt_popust_fb1->setEnabled(false);
		ui->txt_popust_fb2->setEnabled(false);
		ui->txt_popust_komb1->setEnabled(false);
		ui->txt_popust_komb2->setEnabled(false);
		ui->txt_popust_stalna_stranka->setEnabled(false);
		ui->txt_popust_kupon->setEnabled(false);
		ui->txt_popust_akcija->setEnabled(false);
		ui->txt_vsi_popusti_facebook_1->setEnabled(false);
		ui->txt_vsi_popusti_facebook_2->setEnabled(false);
		ui->txt_popusti_skupaj_1->setEnabled(false);
		ui->txt_popusti_skupaj_2->setEnabled(false);
		ui->txt_podrazitev_vikend->setEnabled(false);
		ui->txt_podrazitev_hitrost->setEnabled(false);
		ui->txt_podrazitev_zapleti->setEnabled(false);

		// skrij potrebna polja
		ui->txt_id_stranka->setHidden(true);
		ui->txt_id_projekt->setHidden(true);
		ui->txt_id_racun->setHidden(true);
		ui->txt_id_tip->setHidden(true);
		ui->label_53->setHidden(true);
		ui->txt_rocni_vnos_storitve->setHidden(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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

			// napolni spustne sezname
			ui->txt_sklop->addItem("");
			ui->txt_enota->addItem("");
			ui->txt_ddv->addItem("");
			ui->txt_ddv->addItem("20,0 %");
			ui->txt_ddv->addItem("8,5 %");
			ui->txt_ddv->addItem("0,0 %");

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_storitve");
			sql_fill.exec();
			while ( sql_fill.next() ) {
				if ( ui->txt_sklop->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("sklop")).toString())) == -1 ) {
					ui->txt_sklop->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("sklop")).toString()));
				}
				QString enota = prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString());
				if ( ui->txt_enota->findText(enota) == -1 ) {
					ui->txt_enota->addItem(enota);
				}
			}
			sql_fill.clear();
			ui->txt_sklop->addItem("Ostalo");

			sql_fill.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '%" + pretvori("max_pop_facebook") + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				ui->txt_vsi_popusti_facebook_1->setText(pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString())) + " %");
			}
			sql_fill.clear();

			sql_fill.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '%" + pretvori("max_pop_vsi") + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				ui->txt_popusti_skupaj_1->setText(pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString())) + " %");
			}
			sql_fill.clear();

		}
		base.close();

}

opravila::~opravila()
{
    delete ui;
}

void opravila::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
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

void opravila::on_btn_izhod_clicked() {

	close();

}

void opravila::on_btn_brisi_clicked() { // dokler ne definiramo gumba, bo prazno

}

void opravila::on_btn_sprejmi_clicked() { // ne preverja polj

	QString napaka = "";

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
		if (napaka == "") {
			QSqlQuery sql_vnesi_opravilo;
			if (ui->btn_sprejmi->text() == "Vnesi novo opravilo") { // vnesi novega uporabnika
			sql_vnesi_opravilo.prepare("INSERT INTO opravila (stevilka_stranke, stevilka_projekta, stevilka_racuna, tip_racuna, opravilo_skupina, "
																 "opravilo_storitev, urna_postavka_brez_ddv, urna_postavka_z_ddv, ddv, popust_fb1, popust_fb2, "
																 "popust_komb1, popust_komb2, popust_stranka, popust_kupon, popust_akcija, podrazitev_vikend, "
																 "podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, "
																 "tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, znesek_koncni, enota, opravilo_sklop, opravilo_rocno) "
																 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_opravilo.prepare("UPDATE opravila SET stevilka_stranke = ?, stevilka_projekta = ?, stevilka_racuna = ?, tip_racuna = ?, "
																	 "opravilo_skupina = ?, opravilo_storitev = ?, urna_postavka_brez_ddv = ?, urna_postavka_z_ddv = ?, "
																	 "ddv = ?, popust_fb1 = ?, popust_fb2 = ?, popust_komb1 = ?, popust_komb2 = ?, popust_stranka = ?, "
																	 "popust_kupon = ?, popust_akcija = ?, podrazitev_vikend = ?, podrazitev_hitrost = ?, "
																	 "podrazitev_zapleti = ?, pribitek_vikend = ?, pribitek_hitrost = ?, pribitek_zapleti = ?, tip_ur = ?, "
																	 "ur_dela = ?, rocni_vnos_ur = ?, znesek_popustov = ?, znesek_ddv = ?, znesek_koncni = ?, enota = ?, "
																	 "opravilo_sklop = ?, opravilo_rocno = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_opravilo.bindValue(0, pretvori(ui->txt_id_stranka->text()));
			sql_vnesi_opravilo.bindValue(1, pretvori(ui->txt_id_projekt->text()));
			sql_vnesi_opravilo.bindValue(2, pretvori(ui->txt_id_racun->text()));
			sql_vnesi_opravilo.bindValue(3, pretvori(ui->txt_id_tip->text()));
			if ( ui->txt_sklop->currentText() != "Ostalo" && ui->txt_skupina->currentText() != "Ostalo" && ui->txt_storitev->currentText() != "Ostalo" ) {
				sql_vnesi_opravilo.bindValue(4, pretvori(ui->txt_skupina->currentText()));
				sql_vnesi_opravilo.bindValue(5, pretvori(ui->txt_storitev->currentText()));
			}
			else { // ce je katerikoli od spustnih seznamov "Ostalo"
				sql_vnesi_opravilo.bindValue(4, pretvori(ui->txt_skupina->currentText()));
				sql_vnesi_opravilo.bindValue(5, pretvori(ui->txt_storitev->currentText()));
			}
			sql_vnesi_opravilo.bindValue(6, pretvori(pretvori_v_double(ui->txt_urna_postavka_brez_ddv->text())));
			sql_vnesi_opravilo.bindValue(7, pretvori(pretvori_v_double(ui->txt_urna_postavka->text())));
			sql_vnesi_opravilo.bindValue(8, pretvori(pretvori_v_double(ui->txt_ddv->currentText())));
			sql_vnesi_opravilo.bindValue(9, pretvori(pretvori_v_double(ui->txt_popust_fb1->text())));
			sql_vnesi_opravilo.bindValue(10, pretvori(pretvori_v_double(ui->txt_popust_fb2->text())));
			sql_vnesi_opravilo.bindValue(11, pretvori(pretvori_v_double(ui->txt_popust_komb1->text())));
			sql_vnesi_opravilo.bindValue(12, pretvori(pretvori_v_double(ui->txt_popust_komb2->text())));
			sql_vnesi_opravilo.bindValue(13, pretvori(pretvori_v_double(ui->txt_popust_stalna_stranka->text())));
			sql_vnesi_opravilo.bindValue(14, pretvori(pretvori_v_double(ui->txt_popust_kupon->text())));
			sql_vnesi_opravilo.bindValue(15, pretvori(pretvori_v_double(ui->txt_popust_akcija->text())));
			sql_vnesi_opravilo.bindValue(16, pretvori(pretvori_v_double(ui->txt_podrazitev_vikend->text())));
			sql_vnesi_opravilo.bindValue(17, pretvori(pretvori_v_double(ui->txt_podrazitev_hitrost->text())));
			sql_vnesi_opravilo.bindValue(18, pretvori(pretvori_v_double(ui->txt_podrazitev_zapleti->text())));
			if ( ui->cb_vikend->isChecked() ) {
				sql_vnesi_opravilo.bindValue(19, pretvori("1"));
			}
			else {
				sql_vnesi_opravilo.bindValue(19, pretvori("0"));
			}
			if ( ui->cb_hitrost->isChecked() ) {
				sql_vnesi_opravilo.bindValue(20, pretvori("1"));
			}
			else {
				sql_vnesi_opravilo.bindValue(20, pretvori("0"));
			}
			if ( ui->cb_zapleti->isChecked() ) {
				sql_vnesi_opravilo.bindValue(21, pretvori("1"));
			}
			else {
				sql_vnesi_opravilo.bindValue(21, pretvori("0"));
			}
			if ( ui->rb_predracun->isChecked() ) {
				sql_vnesi_opravilo.bindValue(22, pretvori("predracun"));
				sql_vnesi_opravilo.bindValue(23, pretvori(pretvori_v_double(ui->txt_predracun->text())));
			}
			else if ( ui->rb_casovnice->isChecked() ){
				sql_vnesi_opravilo.bindValue(22, pretvori("casovnice"));
				sql_vnesi_opravilo.bindValue(23, pretvori(pretvori_v_double(ui->txt_casovnice->text())));
			}
			else if ( ui->rb_rocni_vnos->isChecked() ) {
				sql_vnesi_opravilo.bindValue(22, pretvori("rocno"));
				sql_vnesi_opravilo.bindValue(23, pretvori(pretvori_v_double(ui->txt_rocni_vnos->text())));
			}
			else {
				sql_vnesi_opravilo.bindValue(22, pretvori(""));
				sql_vnesi_opravilo.bindValue(23, pretvori(""));
			}
			sql_vnesi_opravilo.bindValue(24, pretvori(pretvori_v_double(ui->txt_rocni_vnos->text())));
			// izracuna znesek popustov
			double popusti = 0.0;
			popusti = pretvori_v_double(ui->txt_nova_urna_postavka_brez_ddv->text()).toDouble();
			popusti *= pretvori_v_double(ui->txt_stevilo_opravljenih_ur->text()).toDouble();
			popusti *= pretvori_v_double(ui->txt_popusti->text()).toDouble() / 100;
			//
			sql_vnesi_opravilo.bindValue(25, pretvori(pretvori_v_double(QString::number(popusti, 'f', 2))));
			sql_vnesi_opravilo.bindValue(26, pretvori(pretvori_v_double(ui->txt_znesek_ddv_na_racunu->text())));
			sql_vnesi_opravilo.bindValue(27, pretvori(pretvori_v_double(ui->txt_znesek_brez_ddv_na_racunu->text())));
			sql_vnesi_opravilo.bindValue(28, pretvori(ui->txt_enota->currentText()));
			sql_vnesi_opravilo.bindValue(29, pretvori(ui->txt_sklop->currentText()));
			sql_vnesi_opravilo.bindValue(30, pretvori(ui->txt_rocni_vnos_storitve->text()));
			sql_vnesi_opravilo.exec();

			// send signal to reload widget
			poslji("opravilo");

			// close this window
			close();
		}
	}
	base.close();

}

void opravila::prejem(QString beseda) {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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

		if ( beseda.left(13) == "Novo opravilo" ) { // nov vnos opravila
			ui->btn_sprejmi->setText("Vnesi novo opravilo");

			QString stevilka_racuna = beseda.right(beseda.length() - 13);
			stevilka_racuna = stevilka_racuna.left(stevilka_racuna.length() - 1);
			ui->txt_racun->setText(stevilka_racuna);
			ui->txt_id_tip->setText(beseda.right(1));

			QSqlQuery sql_napolni;
			sql_napolni.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(stevilka_racuna) + "'");
			sql_napolni.exec();
			if ( sql_napolni.next() ) {
				ui->txt_id_stranka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString()));
				ui->txt_id_projekt->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("projekt")).toString()));
				ui->txt_id_racun->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
			}

		}
		if ( beseda.left(14) == "Staro opravilo" ) { // pregled obstojecega opravila
			ui->btn_sprejmi->setText("Popravi opravilo");

			QString stevilka_opravila = beseda.right(beseda.length() - 14);
			ui->txt_id->setText(stevilka_opravila);

			QSqlQuery sql_napolni;
			sql_napolni.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(stevilka_opravila) + "'");
			sql_napolni.exec();
			if ( sql_napolni.next() ) {

				// vnos vnosnih polj
				ui->txt_id_stranka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_stranke")).toString()));
				ui->txt_id_projekt->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_projekta")).toString()));
				ui->txt_id_racun->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_racuna")).toString()));
				ui->txt_id_tip->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_racuna")).toString()));
				ui->txt_sklop->setCurrentIndex(ui->txt_sklop->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_sklop")).toString())));
				ui->txt_skupina->setCurrentIndex(ui->txt_skupina->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_skupina")).toString())));
				ui->txt_storitev->setCurrentIndex(ui->txt_storitev->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_storitev")).toString())));
				ui->txt_storitev->setCurrentIndex(ui->txt_storitev->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_storitev")).toString())));
				ui->txt_rocni_vnos_storitve->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_rocno")).toString()));
				ui->txt_urna_postavka_brez_ddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("urna_postavka_brez_ddv")).toString()).replace(".", ",") + " EUR");
				ui->txt_ddv->setCurrentIndex(ui->txt_ddv->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()).replace(".", ",") + " %"));
				ui->txt_urna_postavka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("urna_postavka_z_ddv")).toString()).replace(".", ",") + " EUR");
				ui->txt_rocni_vnos->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("rocni_vnos_ur")).toString()));
				ui->txt_enota->setCurrentIndex(ui->txt_enota->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("enota")).toString())));

				// izbira popustov
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_fb1")).toString()) != "0.0" ) {
					ui->cb_popust_fb1->setChecked(true);
				}
				else {
					ui->cb_popust_fb1->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_fb2")).toString()) != "0.0" ) {
					ui->cb_popust_fb2->setChecked(true);
				}
				else {
					ui->cb_popust_fb2->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_komb1")).toString()) != "0.0" ) {
					ui->cb_popust_komb1->setChecked(true);
				}
				else {
					ui->cb_popust_komb1->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_komb2")).toString()) != "0.0" ) {
					ui->cb_popust_komb2->setChecked(true);
				}
				else {
					ui->cb_popust_komb2->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_stranka")).toString()) != "0.0" ) {
					ui->cb_popust_stalna->setChecked(true);
				}
				else {
					ui->cb_popust_stalna->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_kupon")).toString()) != "0.0" ) {
					ui->cb_popust_kupon->setChecked(true);
				}
				else {
					ui->cb_popust_kupon->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_akcija")).toString()) != "0.0" ) {
					ui->cb_popust_akcija->setChecked(true);
				}
				else {
					ui->cb_popust_akcija->setChecked(false);
				}

				// vnos podrazitev
				ui->txt_podrazitev_vikend->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podrazitev_vikend")).toString()));
				ui->txt_podrazitev_hitrost->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podrazitev_hitrost")).toString()));
				ui->txt_podrazitev_zapleti->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podrazitev_zapleti")).toString()));

				// oznacitev podrazitev
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pribitek_vikend")).toString()) == "1" ) {
					ui->cb_vikend->setChecked(true);
				}
				else {
					ui->cb_vikend->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pribitek_hitrost")).toString()) == "1" ) {
					ui->cb_hitrost->setChecked(true);
				}
				else {
					ui->cb_hitrost->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("pribitek_zapleti")).toString()) == "1" ) {
					ui->cb_zapleti->setChecked(true);
				}
				else {
					ui->cb_zapleti->setChecked(false);
				}

				// oznacitev obracuna ur
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_ur")).toString()) == "predracun" ) {
					ui->rb_predracun->setChecked(true);
					ui->txt_predracun->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ur_dela")).toString()).replace(".", ","));
				}
				else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_ur")).toString()) == "casovnice" ) {
					ui->rb_casovnice->setChecked(true);
					ui->txt_casovnice->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ur_dela")).toString()).replace(".", ","));
				}
				else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("tip_ur")).toString()) == "rocno" ) {
					ui->rb_rocni_vnos->setChecked(true);
					ui->txt_rocni_vnos->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ur_dela")).toString()).replace(".", ","));
				}
			}
		}

		// ce je opravilo del predplacila, onemogoci shranjevanje
		if ( ui->txt_id_tip->text() == "2" ) {
			ui->btn_sprejmi->setEnabled(false);
			ui->btn_izracunaj->setEnabled(false);
		}

		// ce je opravilo del predracuna in obstaja racun, onemogoci shranjevanje
		if ( ui->txt_id_tip->text() == "1" ) {
			QSqlQuery sql_racun;
			sql_racun.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_racun->text()) +
												"' AND tip_racuna LIKE '" + pretvori("1") +
												"' AND status_racuna LIKE '" + pretvori("Potrjen") + "'");
			sql_racun.exec();
			if ( sql_racun.next() ) {
				ui->btn_sprejmi->setEnabled(false);
				ui->btn_izracunaj->setEnabled(false);
			}
		}
	}
	base.close();

}

void opravila::on_btn_izracunaj_clicked() {

	izracunaj_racun();

}

void opravila::on_txt_sklop_currentIndexChanged() {

	// pocisti storitve
	ui->txt_skupina->clear();
	ui->txt_storitev->clear();
	ui->txt_enota->setCurrentIndex(0);
	ui->label_53->setHidden(true);
	ui->txt_rocni_vnos_storitve->setHidden(true);
	ui->txt_rocni_vnos_storitve->setText("");

	if ( ui->txt_sklop->currentText() != "" && ui->txt_sklop->currentText() != "Ostalo" ) {
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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

			// napolni spustne sezname
			ui->txt_skupina->addItem("");

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_storitve WHERE sklop LIKE '" + pretvori(ui->txt_sklop->currentText()) + "'");
			sql_fill.exec();
			while ( sql_fill.next() ) {
				if ( ui->txt_skupina->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("skupina")).toString())) == -1 ) {
					ui->txt_skupina->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("skupina")).toString()));
				}
			}
			sql_fill.clear();

		}
		base.close();

		ui->txt_skupina->addItem("Ostalo");
	}
	else if ( ui->txt_sklop->currentText() == "Ostalo" ) {
		ui->label_53->setHidden(false);
		ui->txt_rocni_vnos_storitve->setHidden(false);
	}

}

void opravila::on_txt_skupina_currentIndexChanged() {

	// pocisti storitve
	ui->txt_storitev->clear();
	ui->txt_enota->setCurrentIndex(0);
	ui->label_53->setHidden(true);
	ui->txt_rocni_vnos_storitve->setHidden(true);
	ui->txt_rocni_vnos_storitve->setText("");

	if ( ui->txt_skupina->currentText() != "" && ui->txt_skupina->currentText() != "Ostalo" ) {
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			ui->txt_storitev->addItem("");

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina->currentText()) + "'");
			sql_fill.exec();
			while ( sql_fill.next() ) {
				if ( ui->txt_storitev->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("storitev")).toString())) == -1 ) {
					ui->txt_storitev->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("storitev")).toString()));
				}
			}
			sql_fill.clear();
		}
		base.close();

		ui->txt_storitev->addItem("Ostalo");
	}
	else if ( ui->txt_skupina->currentText() == "Ostalo" ) {
		ui->label_53->setHidden(false);
		ui->txt_rocni_vnos_storitve->setHidden(false);
	}

}

void opravila::on_txt_storitev_currentIndexChanged() {

	// pocisti polja
	ui->txt_enota->setCurrentIndex(0);
	ui->txt_urna_postavka_brez_ddv->setText("");
	ui->txt_ddv->setCurrentIndex(0);
	ui->txt_urna_postavka->setText("");
	ui->label_53->setHidden(true);
	ui->txt_rocni_vnos_storitve->setHidden(true);
	ui->txt_rocni_vnos_storitve->setText("");

	if ( ui->txt_storitev->currentText() != "" && ui->txt_storitev->currentText() != "Ostalo" ) {
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
		base.setDatabaseName(dbase_path);
		base.database();
		base.open();
		if(base.isOpen() != true){
			QMessageBox msgbox;
			msgbox.setText("111Baze ni bilo moc odpreti");
			msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
			msgbox.exec();
		}
		else {
			// baza je odprta

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina->currentText()) + "' AND "
											 "storitev LIKE '" + pretvori(ui->txt_storitev->currentText()) + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				ui->txt_enota->setCurrentIndex(ui->txt_enota->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString())));
				ui->txt_urna_postavka_brez_ddv->setText(pretvori_iz_double(QString::number(prevedi(sql_fill.value(sql_fill.record().indexOf("urna_postavka")).toString()).toDouble(), 'f', 2)) + " EUR");
				ui->txt_ddv->setCurrentIndex(ui->txt_ddv->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("ddv")).toString().replace(".", ",").append(" %"))));
			}
			sql_fill.clear();

		}
		base.close();
	}
	else if ( ui->txt_storitev->currentText() == "Ostalo" ) {
		ui->label_53->setHidden(false);
		ui->txt_rocni_vnos_storitve->setHidden(false);
	}

}

void opravila::izracunaj_urno_postavko_z_ddv() {

	double urna_postavka = pretvori_v_double(ui->txt_urna_postavka_brez_ddv->text()).toDouble();
	double ddv = pretvori_v_double(ui->txt_ddv->currentText()).toDouble();

	double urna_postavka_z_ddv = urna_postavka * ( 1 + ddv / 100 );

	ui->txt_urna_postavka->setText(pretvori_iz_double(QString::number(urna_postavka_z_ddv, 'f', 2)) + " EUR");

}

void opravila::on_txt_urna_postavka_brez_ddv_textChanged() {

	izracunaj_urno_postavko_z_ddv();

}

void opravila::on_txt_ddv_currentIndexChanged() {

	izracunaj_urno_postavko_z_ddv();

}

void opravila::on_txt_urna_postavka_textChanged() {

	napolni_polja();

}

void opravila::on_cb_vikend_toggled() {

	napolni_polja();

}

void opravila::on_cb_hitrost_toggled() {

	napolni_polja();

}

void opravila::on_cb_zapleti_toggled() {

	napolni_polja();

}

void opravila::napolni_polja() {

	// polja napolni samo, ce je urna postavka izpolnjena
	if ( ui->txt_urna_postavka->text() != "" ) {
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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

			// prepisi polja iz vnosnih polj
			ui->txt_storitev_pribitek->setText(ui->txt_skupina->currentText() + " - " +
																				 ui->txt_storitev->currentText());
			ui->txt_novi_ddv->setText(ui->txt_ddv->currentText());

			// ugotovi pribitke
			double pribitek = 0.0;

			QSqlQuery sql_pribitek;

			if ( ui->cb_vikend->isChecked() ) {
				sql_pribitek.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_vikend") + "'");
				sql_pribitek.exec();
				if ( sql_pribitek.next() ) {
					pribitek = pribitek + pretvori_v_double(ui->txt_podrazitev_vikend->text()).toDouble();
					ui->txt_storitev_pribitek->setText(ui->txt_storitev_pribitek->text() + " - Delo med vikendom");
				}
				sql_pribitek.clear();
			}

			if ( ui->cb_hitrost->isChecked() ) {
				sql_pribitek.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_hitrost") + "'");
				sql_pribitek.exec();
				if ( sql_pribitek.next() ) {
					pribitek = pribitek + pretvori_v_double(ui->txt_podrazitev_hitrost->text()).toDouble();
					ui->txt_storitev_pribitek->setText(ui->txt_storitev_pribitek->text() + " - Prednostna obdelava");
				}
				sql_pribitek.clear();
			}

			if ( ui->cb_zapleti->isChecked() ) {
				sql_pribitek.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_zapleti") + "'");
				sql_pribitek.exec();
				if ( sql_pribitek.next() ) {
					pribitek = pribitek + pretvori_v_double(ui->txt_podrazitev_zapleti->text()).toDouble();
					ui->txt_storitev_pribitek->setText(ui->txt_storitev_pribitek->text() + " - Zahtevna naloga");
				}
				sql_pribitek.clear();
			}

			ui->txt_pribitek->setText(pretvori_iz_double(QString::number(pribitek, 'f', 2)) + " %");
			double nova_urna_postavka = pretvori_v_double(ui->txt_urna_postavka_brez_ddv->text()).toDouble() * ( 1 + pribitek / 100 );
			ui->txt_nova_urna_postavka_brez_ddv->setText(pretvori_iz_double(QString::number(nova_urna_postavka, 'f', 2)) + " EUR");

			double ddv = pretvori_v_double(ui->txt_ddv->currentText()).toDouble();

			double nova_urna_postavka_z_ddv = nova_urna_postavka * ( 1 + ddv / 100 );

			ui->txt_nova_urna_postavka->setText(pretvori_iz_double(QString::number(nova_urna_postavka_z_ddv, 'f', 2)) + " EUR");

		}
		base.close();
	}
	else {
		ui->txt_storitev_pribitek->setText("");
		ui->txt_pribitek->setText("");
		ui->txt_nova_urna_postavka_brez_ddv->setText("");
		ui->txt_novi_ddv->setText("");
		ui->txt_nova_urna_postavka->setText("");
		ui->cb_vikend->setChecked(false);
		ui->cb_hitrost->setChecked(false);
		ui->cb_zapleti->setChecked(false);
	}

	izracunaj_racun();

}

void opravila::izracunaj_racun() {

	// napisi celoten naziv storitve
	ui->txt_storitev_na_racunu->setText(ui->txt_storitev_pribitek->text());

	// napisi urno postavko
	ui->txt_urna_postavka_na_racunu->setText(ui->txt_nova_urna_postavka_brez_ddv->text());

	// napisi ustrezno stevilo opravljenih ur
	if ( ui->rb_predracun->isChecked() ) {
		ui->txt_stevilo_opravljenih_ur->setText(ui->txt_predracun->text().replace(".", ","));
	}
	else if ( ui->rb_casovnice->isChecked() ) {
		ui->txt_stevilo_opravljenih_ur->setText(ui->txt_casovnice->text().replace(".", ","));
	}
	else if ( ui->rb_rocni_vnos->isChecked() ) {
		ui->txt_stevilo_opravljenih_ur->setText(ui->txt_rocni_vnos->text().replace(".", ","));
	}

	// napisi ustrezen ddv
	ui->txt_stopnja_ddv->setText(ui->txt_novi_ddv->text());

	// izracunaj skupni znesek popustov
	double popusti_skupaj = 0.0;
	popusti_skupaj += pretvori_v_double(ui->txt_popust_fb1->text()).toDouble();
	popusti_skupaj += pretvori_v_double(ui->txt_popust_fb2->text()).toDouble();
	popusti_skupaj += pretvori_v_double(ui->txt_popust_komb1->text()).toDouble();
	popusti_skupaj += pretvori_v_double(ui->txt_popust_komb2->text()).toDouble();
	popusti_skupaj += pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();
	popusti_skupaj += pretvori_v_double(ui->txt_popust_kupon->text()).toDouble();
	popusti_skupaj += pretvori_v_double(ui->txt_popust_akcija->text()).toDouble();

	ui->txt_popusti->setText(pretvori_iz_double(QString::number(popusti_skupaj, 'f', 1)) + " %");

	// izracunaj koncni znesek storitve
	double urna_postavka = pretvori_v_double(ui->txt_urna_postavka_na_racunu->text()).toDouble();
	double stevilo_ur = pretvori_v_double(ui->txt_stevilo_opravljenih_ur->text()).toDouble();
	double ddv = pretvori_v_double(ui->txt_stopnja_ddv->text()).toDouble();
	double popusti = pretvori_v_double(ui->txt_popusti->text()).toDouble();
	double znesek_brez_ddv = 0.0;
	double znesek_ddv = 0.0;
	double znesek = 0.0;

	znesek_brez_ddv = urna_postavka * stevilo_ur * (1 - popusti / 100 );
	znesek_ddv = znesek_brez_ddv * ddv / 100 ;
	znesek = znesek_brez_ddv + znesek_ddv;

	// napisi zneske na racunu
	ui->txt_znesek_brez_ddv_na_racunu->setText(pretvori_iz_double(QString::number(znesek_brez_ddv, 'f', 2)) + " EUR");
	ui->txt_znesek_ddv_na_racunu->setText(pretvori_iz_double(QString::number(znesek_ddv, 'f', 2)) + " EUR");
	ui->txt_znesek_na_racunu->setText(pretvori_iz_double(QString::number(znesek, 'f', 2)) + " EUR");

}

void opravila::on_rb_predracun_toggled() {

	izracunaj_racun();

}

void opravila::on_rb_casovnice_toggled() {

	izracunaj_racun();

}

void opravila::on_rb_rocni_vnos_toggled() {

	izracunaj_racun();

}

void opravila::on_txt_predracun_textChanged() {

	izracunaj_racun();

}

void opravila::on_txt_casovnice_textChanged() {

	izracunaj_racun();

}

void opravila::on_txt_rocni_vnos_textChanged() {

	izracunaj_racun();

}

void opravila::on_txt_id_stranka_textChanged() {

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

		QSqlQuery sql_poisci;
		sql_poisci.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
		sql_poisci.exec();
		if ( sql_poisci.next() ) {

			// napolni ime stranke
			if (prevedi(sql_poisci.value(sql_poisci.record().indexOf("tip")).toString()) == "1" ) {
				ui->txt_stranka->setText(prevedi(sql_poisci.value(sql_poisci.record().indexOf("priimek")).toString()) + " " +
																 prevedi(sql_poisci.value(sql_poisci.record().indexOf("ime")).toString()));
			}
			else {
				ui->txt_stranka->setText(prevedi(sql_poisci.value(sql_poisci.record().indexOf("ime")).toString()));
			}
			ui->txt_podrazitev_vikend->setText(pretvori_iz_double(prevedi(sql_poisci.value(sql_poisci.record().indexOf("pod_vikend")).toString())) + " %");
			ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(prevedi(sql_poisci.value(sql_poisci.record().indexOf("pod_hitrost")).toString())) + " %");
			ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(prevedi(sql_poisci.value(sql_poisci.record().indexOf("pod_zapleti")).toString())) + " %");
		}
		else {
			ui->txt_stranka->setText("");
		}

	}
	base.close();

}

void opravila::on_txt_id_projekt_textChanged() {

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

		QSqlQuery sql_poisci;
		sql_poisci.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
		sql_poisci.exec();
		if ( sql_poisci.next() ) {
			ui->txt_projekt->setText(prevedi(sql_poisci.value(sql_poisci.record().indexOf("stevilka_projekta")).toString()) + ": " +
															 prevedi(sql_poisci.value(sql_poisci.record().indexOf("naslov_projekta")).toString()));
		}
		else {
			ui->txt_projekt->setText("");
		}

	}
	base.close();

}

void opravila::on_txt_id_racun_textChanged() {

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

		QSqlQuery sql_poisci;
		sql_poisci.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(ui->txt_id_racun->text()) + "'");
		sql_poisci.exec();
		if ( sql_poisci.next() ) {
			ui->txt_racun->setText(prevedi(sql_poisci.value(sql_poisci.record().indexOf("stevilka_racuna")).toString()));
		}
		else {
			ui->txt_racun->setText("");
		}

	}
	base.close();

}

void opravila::on_txt_id_tip_textChanged() {

	if ( ui->txt_id_tip->text() == "1" ) {
		ui->txt_tip->setText("Predracun");
	}
	else if ( ui->txt_id_tip->text() == "2" ) {
		ui->txt_tip->setText("Predplacilo");
	}
	else if ( ui->txt_id_tip->text() == "3" ) {
		ui->txt_tip->setText("Racun");
	}

}

void opravila::izracunaj_popuste(double vrednost, int polje) {

	/*
		* racunali bomo po naslednjem principu
		* najprej izracunamo sestevek vseh vnosnih polj s popusti (razen akcijskega)
		* vrednost mora biti nujno manjsa od najvisje dovoljenje vrednosti
		* ce ni, takoj prekini racunanje in vrni sporocilo z napako
		* poglej, ali bi bil sestevek vseh polj vecji od najvisje dovoljene vrednosti
		* ce ni, vnesi stevilko v polje in popravi sestevke
		* ce je, vnesi najvisjo dovoljeno vrednost v polje in popravi sestevke ter
		* obvesti uporabnika s sporocilom
		*/

	/*
	* polje == 0 => fb1
	* polje == 1 => fb2
	* polje == 2 => komb1
	* polje == 3 => komb2
	* polje == 4 => stranka
	* polje == 5 => kupon
	*/

	// vsa vnosna polja s popustom pretvorimo v vrednost double, da bo mozno racunanje
	double popust[7];
	popust[0] = pretvori_v_double(ui->txt_popust_fb1->text()).toDouble();
	popust[1] = pretvori_v_double(ui->txt_popust_fb2->text()).toDouble();
	popust[2] = pretvori_v_double(ui->txt_popust_komb1->text()).toDouble();
	popust[3] = pretvori_v_double(ui->txt_popust_komb2->text()).toDouble();
	popust[4] = pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();
	popust[5] = pretvori_v_double(ui->txt_popust_kupon->text()).toDouble();
	popust[6] = pretvori_v_double(ui->txt_popust_akcija->text()).toDouble();

	// maksimalne vrednosti sestevka popustov
	double max_fb = pretvori_v_double(ui->txt_vsi_popusti_facebook_1->text()).toDouble();
	double max_vsi = pretvori_v_double(ui->txt_popusti_skupaj_1->text()).toDouble();

	double sestevek;
	sestevek = popust[0] + popust[1];

	if ( sestevek > max_fb ) {
		if ( polje == 0 ) {
			popust[0] = max_fb - popust[1];
			ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust[0], 'f', 1)) + " %");
		}
		else if ( polje == 1 ) {
			popust[1] = max_fb - popust[0];
			ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust[1], 'f', 1)) + " %");
		}
		QMessageBox msg_napaka;
		msg_napaka.setText("Vnesli ste previsoko vrednost. Vrednost je avtomatsko "
											 "popravljena na najvisjo dovoljena vrednost za to polje!");
		msg_napaka.exec();
		return;
	}

	sestevek = popust[0] + popust[1];
	ui->txt_vsi_popusti_facebook_2->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)) + " %");

	sestevek = 0.0;
	for ( int i = 0; i <= 5; i++ ) {
		sestevek += popust[i];
	}
	if ( sestevek > max_vsi ) {
		if ( polje == 0 ) {
			popust[0] = max_vsi - sestevek + popust[0];
			ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust[0], 'f', 1)) + " %");
		}
		else if ( polje == 1 ) {
			popust[1] = max_vsi - sestevek + popust[1];
			ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust[1], 'f', 1)) + " %");
		}
		else if ( polje == 2 ) {
			popust[2] = max_vsi - sestevek + popust[2];
			ui->txt_popust_komb1->setText(pretvori_iz_double(QString::number(popust[2], 'f', 1)) + " %");
		}
		else if ( polje == 3 ) {
			popust[3] = max_vsi - sestevek + popust[3];
			ui->txt_popust_komb2->setText(pretvori_iz_double(QString::number(popust[3], 'f', 1)) + " %");
		}
		else if ( polje == 4 ) {
			popust[4] = max_vsi - sestevek + popust[4];
			ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(QString::number(popust[4], 'f', 1)) + " %");
		}
		else if ( polje == 5 ) {
			popust[5] = max_vsi - sestevek + popust[5];
			ui->txt_popust_kupon->setText(pretvori_iz_double(QString::number(popust[5], 'f', 1)) + " %");
		}
		QMessageBox msg_napaka;
		msg_napaka.setText("Vnesli ste previsoko vrednost. Vrednost je avtomatsko "
											 "popravljena na najvisjo dovoljena vrednost za to polje!");
		msg_napaka.exec();
		return;
	}

	sestevek = 0.0;
	for ( int i = 0; i <= 5; i++ ) {
		sestevek += popust[i];
	}
	ui->txt_popusti_skupaj_2->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)) + " %");

	sestevek += popust[6];

	ui->txt_popusti->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)) + " %");

}

void opravila::on_cb_popust_fb1_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_fb1->setEnabled(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_fb1")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_fb1")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb1")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb1")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
				sql_stranke.exec();
				if ( sql_stranke.next() ) {
					if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_1")).toString()) != "0.0" ) { // stranka ima popust
						ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_1")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_facebook_stranka") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_popust_fb1->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_fb1->setEnabled(false);
	}

	ui->txt_popust_fb1->setText(ui->txt_popust_fb1->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_fb2_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_fb2->setEnabled(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_fb2")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_fb2")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb2")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb2")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
				sql_stranke.exec();
				if ( sql_stranke.next() ) {
					if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_2")).toString()) != "0.0" ) { // stranka ima popust
						ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_2")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_facebook_prijatelj") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_popust_fb2->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_fb2->setEnabled(false);
	}

	ui->txt_popust_fb2->setText(ui->txt_popust_fb2->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_komb1_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_komb1->setEnabled(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_komb1")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_komb1")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb1")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb1")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
				sql_stranke.exec();
				if ( sql_stranke.next() ) {
					if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_1")).toString()) != "0.0" ) { // stranka ima popust
						ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_1")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kombinacija_1") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_popust_komb1->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_komb1->setEnabled(false);
	}

	ui->txt_popust_komb1->setText(ui->txt_popust_komb1->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_komb2_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_komb2->setEnabled(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_komb2")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_komb2")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb2")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb2")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
				sql_stranke.exec();
				if ( sql_stranke.next() ) {
					if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_2")).toString()) != "0.0" ) { // stranka ima popust
						ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_2")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kombinacija_2") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_popust_komb2->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_komb2->setEnabled(false);
	}

	ui->txt_popust_komb2->setText(ui->txt_popust_komb2->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_stalna_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_stalna_stranka->setEnabled(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_stranka")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_stranka")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_stranka")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_stranka")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
				sql_stranke.exec();
				if ( sql_stranke.next() ) {
					if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_stranka")).toString()) != "0.0" ) { // stranka ima popust
						ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_stranka")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_stalna_stranka") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_popust_stalna_stranka->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_stalna_stranka->setEnabled(false);
	}

	ui->txt_popust_stalna_stranka->setText(ui->txt_popust_stalna_stranka->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_kupon_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_kupon->setEnabled(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_kupon")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_kupon")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_kupon")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_kupon")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
				sql_stranke.exec();
				if ( sql_stranke.next() ) {
					if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kupon")).toString()) != "0.0" ) { // stranka ima popust
						ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kupon")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kupon") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_popust_kupon->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_kupon->setEnabled(false);
	}

	ui->txt_popust_kupon->setText(ui->txt_popust_kupon->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_akcija_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_akcija->setEnabled(true);

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_akcija")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_akcija")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_akcija")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_akcija")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_id_stranka->text()) + "'");
				sql_stranke.exec();
				if ( sql_stranke.next() ) {
					if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString()) != "0.0" ) { // stranka ima popust
						ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_popust_akcija->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_akcija->setEnabled(false);
	}

	ui->txt_popust_akcija->setText(ui->txt_popust_akcija->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_podrazitev_vikend_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_podrazitev_vikend->setEnabled(true);
	}
	else {
		ui->txt_podrazitev_vikend->setEnabled(false);
	}

	ui->txt_podrazitev_vikend->setText(ui->txt_podrazitev_vikend->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_podrazitev_hitrost_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_podrazitev_hitrost->setEnabled(true);
	}
	else {
		ui->txt_podrazitev_hitrost->setEnabled(false);
	}

	ui->txt_podrazitev_hitrost->setText(ui->txt_podrazitev_hitrost->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_podrazitev_zapleti_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_podrazitev_zapleti->setEnabled(true);
	}
	else {
		ui->txt_podrazitev_zapleti->setEnabled(false);
	}

	ui->txt_podrazitev_zapleti->setText(ui->txt_podrazitev_zapleti->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_txt_popust_fb1_textChanged(QString besedilo) {

	izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 0);

}

void opravila::on_txt_popust_fb2_textChanged(QString besedilo) {

	izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 1);

}

void opravila::on_txt_popust_komb1_textChanged(QString besedilo) {

	izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 2);

}

void opravila::on_txt_popust_komb2_textChanged(QString besedilo) {

	izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 3);

}

void opravila::on_txt_popust_stalna_stranka_textChanged(QString besedilo) {

	izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 4);

}

void opravila::on_txt_popust_kupon_textChanged(QString besedilo) {

	izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 5);

}

void opravila::on_txt_popust_fb1_editingFinished() {

	ui->txt_popust_fb1->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_fb1->text())) + " %");

}

void opravila::on_txt_popust_fb2_editingFinished() {

	ui->txt_popust_fb2->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_fb2->text())) + " %");

}

void opravila::on_txt_popust_komb1_editingFinished() {

	ui->txt_popust_komb1->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_komb1->text())) + " %");

}

void opravila::on_txt_popust_komb2_editingFinished() {

	ui->txt_popust_komb2->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_komb2->text())) + " %");

}

void opravila::on_txt_popust_stalna_stranka_editingFinished() {

	ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_stalna_stranka->text())) + " %");

}

void opravila::on_txt_popust_kupon_editingFinished() {

	ui->txt_popust_kupon->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_kupon->text())) + " %");

}

void opravila::on_txt_popust_akcija_editingFinished() {

	ui->txt_popust_akcija->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_akcija->text())) + " %");

}

void opravila::on_txt_podrazitev_vikend_editingFinished() {

	ui->txt_podrazitev_vikend->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_vikend->text())) + " %");

}

void opravila::on_txt_podrazitev_hitrost_editingFinished() {

	ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_hitrost->text())) + " %");

}

void opravila::on_txt_podrazitev_zapleti_editingFinished() {

	ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_zapleti->text())) + " %");

}

QString opravila::pretvori_v_double(QString besedilo) {

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

QString opravila::pretvori_iz_double(QString besedilo) {

	besedilo.replace(".",",");

	return besedilo;

}

QString opravila::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString opravila::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

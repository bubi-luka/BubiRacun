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
		ui->txt_sifra->setText("");
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
		ui->cb_popust_priporocilo->setText("");
		ui->cb_popust_stalna->setText("");
		ui->cb_popust_akcija->setText("");
		ui->cb_podrazitev_vikend->setText("");
		ui->cb_podrazitev_hitrost->setText("");
		ui->cb_podrazitev_zapleti->setText("");
		ui->txt_popust_fb1->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_fb2->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_priporocilo->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_stalna_stranka->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popust_akcija->setText(pretvori_iz_double("0,0 %"));
		ui->txt_vsi_popusti_facebook_2->setText(pretvori_iz_double("0,0 %"));
		ui->txt_popusti_skupaj_2->setText(pretvori_iz_double("0,0 %"));

		ui->tbl_casovnice->clear();

		// onemogoci polja s popusti
		ui->txt_popust_fb1->setEnabled(false);
		ui->txt_popust_fb2->setEnabled(false);
		ui->txt_popust_priporocilo->setEnabled(false);
		ui->txt_popust_stalna_stranka->setEnabled(false);
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
		ui->gb_storitve_s_pribitkom->setHidden(true);
		ui->gb_urejanje_pribitkov_notranji->setHidden(true);
		ui->gb_najvisji_zneski_popustov_notranji->setHidden(true);

			// napolni spustne sezname
			ui->txt_sklop->addItem("");
			ui->txt_enota->addItem("");
			ui->txt_ddv->addItem("");

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_kategorije WHERE aktivnost LIKE '1' ORDER BY indeks ASC");
			sql_fill.exec();
			while ( sql_fill.next() ) {
			   ui->txt_sklop->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString()));
			}
			sql_fill.clear();

			sql_fill.prepare("SELECT * FROM sif_enote WHERE aktivnost LIKE '1'");
			sql_fill.exec();
			while ( sql_fill.next() ) {
			   ui->txt_enota->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString()));
			}
			sql_fill.clear();

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

			sql_fill.prepare("SELECT * FROM sif_ddv WHERE aktivnost LIKE '1'");
			sql_fill.exec();
			while ( sql_fill.next() ) {
				ui->txt_ddv->addItem(pretvori_iz_double(pretvori(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString())) + " %");
			}

		ui->tab_opravila->setCurrentIndex(0);
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

	izracunaj_racun();

	QString napaka = "";

		if (napaka == "") {
			QSqlQuery sql_vnesi_opravilo;
			if (ui->btn_sprejmi->text() == "Vnesi novo opravilo") { // vnesi novega uporabnika
				sql_vnesi_opravilo.prepare("INSERT INTO opravila (stevilka_stranke, stevilka_projekta, stevilka_racuna, tip_racuna, "
										   "opravilo_skupina, opravilo_storitev, urna_postavka_brez_ddv, urna_postavka_z_ddv, ddv, "
										   "popust_fb1, popust_fb2, popust_priporocilo, popust_stranka, "
										   "popust_akcija, podrazitev_vikend, podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, "
										   "pribitek_hitrost, pribitek_zapleti, tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, "
										   "znesek_koncni, enota, opravilo_sklop, sifra) VALUES "
										   "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_opravilo.prepare("UPDATE opravila SET stevilka_stranke = ?, stevilka_projekta = ?, stevilka_racuna = ?, tip_racuna = ?, "
																	 "opravilo_skupina = ?, opravilo_storitev = ?, urna_postavka_brez_ddv = ?, urna_postavka_z_ddv = ?, "
																	 "ddv = ?, popust_fb1 = ?, popust_fb2 = ?, popust_priporocilo = ?, popust_stranka = ?, "
																	 "popust_akcija = ?, podrazitev_vikend = ?, podrazitev_hitrost = ?, "
																	 "podrazitev_zapleti = ?, pribitek_vikend = ?, pribitek_hitrost = ?, pribitek_zapleti = ?, tip_ur = ?, "
																	 "ur_dela = ?, rocni_vnos_ur = ?, znesek_popustov = ?, znesek_ddv = ?, znesek_koncni = ?, enota = ?, "
																	 "opravilo_sklop = ?, sifra = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_opravilo.bindValue(0, pretvori(ui->txt_id_stranka->text()));
			sql_vnesi_opravilo.bindValue(1, pretvori(ui->txt_id_projekt->text()));
			sql_vnesi_opravilo.bindValue(2, pretvori(ui->txt_id_racun->text()));
			sql_vnesi_opravilo.bindValue(3, pretvori(ui->txt_id_tip->text()));
			sql_vnesi_opravilo.bindValue(4, pretvori(ui->txt_skupina->currentText()));
			if ( ui->cb_rocni->isChecked() ) {
				sql_vnesi_opravilo.bindValue(5, pretvori(ui->txt_rocni_vnos_storitve->text()));
			}
			else {
				sql_vnesi_opravilo.bindValue(5, pretvori(ui->txt_storitev->currentText()));
			}
			sql_vnesi_opravilo.bindValue(6, pretvori(pretvori_v_double(ui->txt_urna_postavka_brez_ddv->text())));
			sql_vnesi_opravilo.bindValue(7, pretvori(pretvori_v_double(ui->txt_urna_postavka->text())));
			sql_vnesi_opravilo.bindValue(8, pretvori(pretvori_v_double(ui->txt_ddv->currentText())));
			sql_vnesi_opravilo.bindValue(9, pretvori(pretvori_v_double(ui->txt_popust_fb1->text())));
			sql_vnesi_opravilo.bindValue(10, pretvori(pretvori_v_double(ui->txt_popust_fb2->text())));
			sql_vnesi_opravilo.bindValue(11, pretvori(pretvori_v_double(ui->txt_popust_priporocilo->text())));
			sql_vnesi_opravilo.bindValue(12, pretvori(pretvori_v_double(ui->txt_popust_stalna_stranka->text())));
			sql_vnesi_opravilo.bindValue(13, pretvori(pretvori_v_double(ui->txt_popust_akcija->text())));
			sql_vnesi_opravilo.bindValue(14, pretvori(pretvori_v_double(ui->txt_podrazitev_vikend->text())));
			sql_vnesi_opravilo.bindValue(15, pretvori(pretvori_v_double(ui->txt_podrazitev_hitrost->text())));
			sql_vnesi_opravilo.bindValue(16, pretvori(pretvori_v_double(ui->txt_podrazitev_zapleti->text())));
			if ( ui->cb_vikend->isChecked() ) {
				sql_vnesi_opravilo.bindValue(17, pretvori("1"));
			}
			else {
				sql_vnesi_opravilo.bindValue(17, pretvori("0"));
			}
			if ( ui->cb_hitrost->isChecked() ) {
				sql_vnesi_opravilo.bindValue(18, pretvori("1"));
			}
			else {
				sql_vnesi_opravilo.bindValue(18, pretvori("0"));
			}
			if ( ui->cb_zapleti->isChecked() ) {
				sql_vnesi_opravilo.bindValue(19, pretvori("1"));
			}
			else {
				sql_vnesi_opravilo.bindValue(19, pretvori("0"));
			}
			if ( ui->rb_predracun->isChecked() ) {
				sql_vnesi_opravilo.bindValue(20, pretvori("predracun"));
				sql_vnesi_opravilo.bindValue(21, pretvori(pretvori_v_double(ui->txt_predracun->text())));
			}
			else if ( ui->rb_casovnice->isChecked() ){
				sql_vnesi_opravilo.bindValue(20, pretvori("casovnice"));
				sql_vnesi_opravilo.bindValue(21, pretvori(pretvori_v_double(ui->txt_casovnice->text())));
			}
			else if ( ui->rb_rocni_vnos->isChecked() ) {
				sql_vnesi_opravilo.bindValue(20, pretvori("rocno"));
				sql_vnesi_opravilo.bindValue(21, pretvori(pretvori_v_double(ui->txt_rocni_vnos->text())));
			}
			else {
				sql_vnesi_opravilo.bindValue(20, pretvori(""));
				sql_vnesi_opravilo.bindValue(21, pretvori(""));
			}
			sql_vnesi_opravilo.bindValue(22, pretvori(pretvori_v_double(ui->txt_rocni_vnos->text())));
			// izracuna znesek popustov
			double popusti = 0.0;
			popusti = pretvori_v_double(ui->txt_nova_urna_postavka_brez_ddv->text()).toDouble();
			popusti *= pretvori_v_double(ui->txt_stevilo_opravljenih_ur->text()).toDouble();
			popusti *= pretvori_v_double(ui->txt_popusti->text()).toDouble() / 100;
			//
			sql_vnesi_opravilo.bindValue(23, pretvori(pretvori_v_double(QString::number(popusti, 'f', 2))));
			sql_vnesi_opravilo.bindValue(24, pretvori(pretvori_v_double(ui->txt_znesek_ddv_na_racunu->text())));
			sql_vnesi_opravilo.bindValue(25, pretvori(pretvori_v_double(ui->txt_znesek_brez_ddv_na_racunu->text())));
			sql_vnesi_opravilo.bindValue(26, pretvori(ui->txt_enota->currentText()));
			sql_vnesi_opravilo.bindValue(27, pretvori(ui->txt_sklop->currentText()));
			sql_vnesi_opravilo.bindValue(28, pretvori(ui->txt_sifra->text()));
			sql_vnesi_opravilo.exec();

			// send signal to reload widget
			poslji("opravilo");

			// close this window
			close();
		}

}

void opravila::on_cb_rocni_toggled() {

	if ( ui->cb_rocni->isChecked() ) {
		ui->txt_sifra->setText("999999");
		ui->txt_rocni_vnos_storitve->setVisible(true);
		ui->label_53->setVisible(true);
		ui->txt_storitev->setCurrentIndex(0);
	}
	else {
		ui->txt_sifra->setText("");
		ui->txt_rocni_vnos_storitve->setVisible(false);
		ui->label_53->setVisible(false);
		ui->txt_rocni_vnos_storitve->setText("");
	}

}

void opravila::prejem(QString beseda) {

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

			// izberi tip obracuna ur
			if ( ui->txt_id_tip->text() == "1") { // predracun
				ui->rb_rocni_vnos->setChecked(true);
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
				if ( ui->txt_sklop->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_sklop")).toString())) == -1 ) {
					ui->txt_sklop->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_sklop")).toString()));
				}
				ui->txt_sklop->setCurrentIndex(ui->txt_sklop->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_sklop")).toString())));
				if ( ui->txt_skupina->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_skupina")).toString())) == -1 ) {
					ui->txt_skupina->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_skupina")).toString()));
				}
				ui->txt_skupina->setCurrentIndex(ui->txt_skupina->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_skupina")).toString())));
				if ( ui->txt_storitev->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_storitev")).toString())) == -1 ) {
					ui->txt_storitev->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_storitev")).toString()));
				}
				ui->txt_storitev->setCurrentIndex(ui->txt_storitev->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_storitev")).toString())));
				ui->txt_rocni_vnos_storitve->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opravilo_rocno")).toString()));
				ui->txt_urna_postavka_brez_ddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("urna_postavka_brez_ddv")).toString()).replace(".", ",") + " EUR");
				if ( ui->txt_ddv->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()).replace(".", ",") + " %") == -1 ) {
					ui->txt_ddv->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()).replace(".", ",") + " %");
				}
				ui->txt_ddv->setCurrentIndex(ui->txt_ddv->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()).replace(".", ",") + " %"));
				ui->txt_urna_postavka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("urna_postavka_z_ddv")).toString()).replace(".", ",") + " EUR");
				ui->txt_rocni_vnos->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("rocni_vnos_ur")).toString()));
				if ( ui->txt_enota->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("enota")).toString())) == -1 ) {
					ui->txt_enota->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("enota")).toString()));
				}
				ui->txt_enota->setCurrentIndex(ui->txt_enota->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("enota")).toString())));
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("sifra")).toString()) == "" ) {
					ui->txt_sifra->setText("000000");
				}
				else {
					ui->txt_sifra->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("sifra")).toString()));
				}

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
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_priporocilo")).toString()) != "0.0" ) {
					ui->cb_popust_priporocilo->setChecked(true);
				}
				else {
					ui->cb_popust_priporocilo->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_stranka")).toString()) != "0.0" ) {
					ui->cb_popust_stalna->setChecked(true);
				}
				else {
					ui->cb_popust_stalna->setChecked(false);
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

		// napolni casovnice
		napolni_casovnice();

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

void opravila::on_btn_izracunaj_clicked() {

	izracunaj_racun();

}

void opravila::on_txt_sklop_currentIndexChanged() { // to so kategorije

	// pocisti storitve
	ui->txt_skupina->clear();
	ui->txt_storitev->clear();
	ui->txt_enota->setCurrentIndex(0);
	ui->label_53->setHidden(true);
	ui->txt_rocni_vnos_storitve->setHidden(true);
	ui->txt_rocni_vnos_storitve->setText("");

	if ( ui->txt_sklop->currentText() != "" ) {

			// napolni spustne sezname
			ui->txt_skupina->addItem("");

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_podkategorije WHERE kategorija LIKE '" + pretvori(ui->txt_sklop->currentText()) +
							 "' AND aktivnost LIKE '1' ORDER BY indeks ASC");
			sql_fill.exec();
			while ( sql_fill.next() ) {
				ui->txt_skupina->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString()));
			}
			sql_fill.clear();

	}

}

void opravila::on_txt_skupina_currentIndexChanged() { // podkategorije

	// pocisti storitve
	ui->txt_storitev->clear();
	ui->txt_enota->setCurrentIndex(0);
	ui->label_53->setHidden(true);
	ui->txt_rocni_vnos_storitve->setHidden(true);
	ui->txt_rocni_vnos_storitve->setText("");

	if ( ui->txt_skupina->currentText() != "" ) {
			ui->txt_storitev->addItem("");

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_storitve WHERE kategorija LIKE '" + pretvori(ui->txt_sklop->currentText()) + "' "
							 "AND podkategorija LIKE '" + pretvori(ui->txt_skupina->currentText()) + "' AND aktivnost LIKE '1' ORDER BY sifra ASC");
			sql_fill.exec();
			while ( sql_fill.next() ) {
				ui->txt_storitev->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("storitev")).toString()));
			}
			sql_fill.clear();

			// ce ni vnesene nobene storitve, vklopi rocni vnos storitev, drugace to izklopi
			if ( ui->txt_storitev->count() <= 1 ) {
				ui->cb_rocni->setChecked(true);
			}
			else {
				ui->cb_rocni->setChecked(false);
			}

	}

}

void opravila::on_txt_storitev_currentIndexChanged() {

	// pocisti polja
	ui->txt_enota->setCurrentIndex(0);
	ui->txt_urna_postavka_brez_ddv->setText("");
	ui->txt_ddv->setCurrentIndex(0);
	ui->txt_urna_postavka->setText("");

	if ( ui->txt_storitev->currentText() != "" ) {
		ui->cb_rocni->setChecked(false);

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM sif_storitve WHERE kategorija LIKE '" + pretvori(ui->txt_sklop->currentText()) + "' AND "
											 "podkategorija LIKE '" + pretvori(ui->txt_skupina->currentText()) + "' AND "
											 "storitev LIKE '" + pretvori(ui->txt_storitev->currentText()) + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				if ( ui->txt_enota->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString())) != -1 ) {
					ui->txt_enota->setCurrentIndex(ui->txt_enota->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString())));
				}
				else {
					ui->txt_enota->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString()));
					ui->txt_enota->setCurrentIndex(ui->txt_enota->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("enota")).toString())));
				}

				if ( ui->txt_ddv->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString().replace(".", ","))) != -1 ) {
					ui->txt_ddv->setCurrentIndex(ui->txt_ddv->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString().replace(".", ","))));
				}
				else {
					ui->txt_ddv->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString().replace(".", ",")));
					ui->txt_ddv->setCurrentIndex(ui->txt_ddv->findText(prevedi(sql_fill.value(sql_fill.record().indexOf("stopnja_ddv")).toString().replace(".", ","))));
				}

				ui->txt_urna_postavka_brez_ddv->setText(pretvori_iz_double(QString::number(pretvori_v_double(prevedi(sql_fill.value(sql_fill.record().indexOf("znesek_brez_ddv")).toString())).toDouble(), 'f', 2)) + " EUR");

				ui->txt_sifra->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("sifra")).toString()));
			}
			sql_fill.clear();

	}

}

void opravila::on_txt_rocni_vnos_storitve_textChanged() {

	ui->txt_storitev_na_racunu->setText("");

	if ( ui->txt_sifra->text() != "" && ui->txt_sifra->text() != "999999" ) {
		ui->txt_storitev_na_racunu->setText(ui->txt_rocni_vnos_storitve->text());
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

void opravila::napolni_casovnice() {

		// clear previous content
		ui->tbl_casovnice->clear();

		for (int i = 0; i <= 3; i++) {
			ui->tbl_casovnice->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM opravila");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_casovnice->removeRow(0);
		}

		// start filling the table
		ui->tbl_casovnice->insertColumn(0);
		ui->tbl_casovnice->insertColumn(1);
		ui->tbl_casovnice->insertColumn(2);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Datum");
		naslov2->setText("Ure");

		ui->tbl_casovnice->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_casovnice->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_casovnice->setHorizontalHeaderItem(2, naslov2);

		int row = 0;

		QString seznam_casovnic = "";
		QSqlQuery vnesi;
		vnesi.prepare("SELECT * FROM opravila WHERE id LIKE '" + ui->txt_id->text() + "' ORDER BY sifra ASC");
		vnesi.exec();
		if ( vnesi.next() ) {
			seznam_casovnic = vnesi.value(vnesi.record().indexOf("casovnice")).toString();

			int max_casovnice = seznam_casovnic.count(";");

			for ( int a = 1; a <= max_casovnice; a++ ) {
				ui->tbl_casovnice->insertRow(row);
				ui->tbl_casovnice->setRowHeight(row, 20);

				QString del_seznama = seznam_casovnic.left(seznam_casovnic.indexOf(";"));
				seznam_casovnic = seznam_casovnic.right(seznam_casovnic.length() - seznam_casovnic.indexOf(";") - 1);

				// del seznama razbijemo na datum in vrednost
				QString datum = del_seznama.left(del_seznama.indexOf(","));
				QString vrednost = del_seznama.right(del_seznama.length() - del_seznama.indexOf(",") - 1).replace(".", ",");

				QTableWidgetItem *polje1 = new QTableWidgetItem;
				polje1->setText(QString::number(a, 10));
				ui->tbl_casovnice->setItem(row, 0, polje1);

				QTableWidgetItem *polje2 = new QTableWidgetItem;
				polje2->setText(datum);
				ui->tbl_casovnice->setItem(row, 1, polje2);

				QTableWidgetItem *polje3 = new QTableWidgetItem;
				polje3->setText(vrednost);
				ui->tbl_casovnice->setItem(row, 2, polje3);

				row++;
			}
		}

	ui->tbl_casovnice->sortByColumn(1, Qt::AscendingOrder);

}

void opravila::napolni_polja() {

	// polja napolni samo, ce je urna postavka izpolnjena
	if ( ui->txt_urna_postavka->text() != "" ) {

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
	popusti_skupaj += pretvori_v_double(ui->txt_popust_priporocilo->text()).toDouble();
	popusti_skupaj += pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();
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

void opravila::on_txt_storitev_na_racunu_textChanged() {

	double ure_predracun = 0.0;
	double ure_casovnice = 0.0;

		if ( ui->txt_id_tip->text() == "3" ) { // gre za racun

			// poiscemo id stevilko predracuna, na katerega je vezan trenutni racun
			QString id_stars = "";
			QSqlQuery sql_id_starsa;
			sql_id_starsa.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(ui->txt_id_racun->text()) + "'");
			sql_id_starsa.exec();
			if ( sql_id_starsa.next() ) {
				id_stars = prevedi(sql_id_starsa.value(sql_id_starsa.record().indexOf("stevilka_starsa")).toString());

				// poiscemo prisotnost opravila v starsevskem predracunu, ki je enakovreden trenutnemu in prepisemo njegove ure kot ure predracuna
				QString sql_stavek = "SELECT * FROM opravila WHERE id LIKE '" + pretvori(id_stars) + "'";
				sql_stavek += " AND opravilo_sklop LIKE '" + pretvori(ui->txt_sklop->currentText()) + "'";
				sql_stavek += " AND opravilo_skupina LIKE '" + pretvori(ui->txt_skupina->currentText()) + "'";
				sql_stavek += " AND opravilo_storitev LIKE '" + pretvori(ui->txt_storitev->currentText()) + "'";
				sql_stavek += " AND opravilo_rocno LIKE '" + pretvori(ui->txt_rocni_vnos_storitve->text()) + "'";

				QString cb_vrednost = "0";
				if ( ui->cb_vikend->isChecked() ) {
					cb_vrednost = "1";
				}
				else {
					cb_vrednost = "0";
				}
				sql_stavek += " AND pribitek_vikend LIKE '" + pretvori(cb_vrednost) + "'";

				if ( ui->cb_hitrost->isChecked() ) {
					cb_vrednost = "1";
				}
				else {
					cb_vrednost = "0";
				}
				sql_stavek += " AND pribitek_hitrost LIKE '" + pretvori(cb_vrednost) + "'";

				if ( ui->cb_zapleti->isChecked() ) {
					cb_vrednost = "1";
				}
				else {
					cb_vrednost = "0";
				}
				sql_stavek += " AND pribitek_zapleti LIKE '" + pretvori(cb_vrednost) + "'";

				QSqlQuery sql_starsevsko_opravilo;
				sql_starsevsko_opravilo.prepare(sql_stavek);
				sql_starsevsko_opravilo.exec();
				while ( sql_starsevsko_opravilo.next() ) {
					ure_predracun += pretvori_v_double(prevedi(sql_starsevsko_opravilo.value(sql_starsevsko_opravilo.record().indexOf("rocni_vnos_ur")).toString())).toDouble();
				} //while ( sql_starsevsko_opravilo.next() )
			} // if ( sql_id_starsa.next() )

			QString seznam_casovnic = "";
			QSqlQuery sql_casovnice;
			sql_casovnice.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_casovnice.exec();
			if ( sql_casovnice.next() ) {
				seznam_casovnic = sql_casovnice.value(sql_casovnice.record().indexOf("casovnice")).toString();

				int max_casovnice = seznam_casovnic.count(";");

				for ( int a = 1; a <= max_casovnice; a++ ) {
					QString del_seznama = seznam_casovnic.left(seznam_casovnic.indexOf(";"));
					seznam_casovnic = seznam_casovnic.right(seznam_casovnic.length() - seznam_casovnic.indexOf(";") - 1);

					// del seznama razbijemo na datum in vrednost
					QString vrednost = del_seznama.right(del_seznama.length() - del_seznama.indexOf(",") - 1); // oblika "H:mm"
					double ure = vrednost.left(vrednost.length() - 3).toDouble();
					double minute = vrednost.right(2).toDouble() / 60;
					vrednost = QString::number(ure + minute, 'f', 2);

					ure_casovnice += pretvori_v_double(vrednost).toDouble();
				} // for ( int a = 1; a <= max_casovnice; a++ )
			} // if ( sql_casovnice.next() )
		} // if ( ui->txt_id_tip->text() == "3" )

	ui->txt_predracun->setText(pretvori_iz_double(QString::number(ure_predracun, 'f', 2)));
	ui->txt_casovnice->setText(pretvori_iz_double(QString::number(ure_casovnice, 'f', 2)));
	if ( ure_predracun != 0.0 ) {
		ui->rb_predracun->setChecked(true);
	}
	if ( ure_casovnice != 0.0 ) {
		ui->rb_casovnice->setChecked(true);
	}

}

void opravila::on_txt_id_stranka_textChanged() {

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

void opravila::on_txt_id_projekt_textChanged() {

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

void opravila::on_txt_id_racun_textChanged() {

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

void opravila::izracunaj_popuste(int polje) {

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
	* polje == 2 => priporocilo
	* polje == 4 => stranka
	*/

	// vsa vnosna polja s popustom pretvorimo v vrednost double, da bo mozno racunanje
	double popust[5];
	popust[0] = pretvori_v_double(ui->txt_popust_fb1->text()).toDouble();
	popust[1] = pretvori_v_double(ui->txt_popust_fb2->text()).toDouble();
	popust[2] = pretvori_v_double(ui->txt_popust_priporocilo->text()).toDouble();
	popust[3] = pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();
	popust[4] = pretvori_v_double(ui->txt_popust_akcija->text()).toDouble();

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
	for ( int i = 0; i <= 4; i++ ) {
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
			ui->txt_popust_priporocilo->setText(pretvori_iz_double(QString::number(popust[2], 'f', 1)) + " %");
		}
		else if ( polje == 3 ) {
			popust[3] = max_vsi - sestevek + popust[4];
			ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(QString::number(popust[4], 'f', 1)) + " %");
		}
		QMessageBox msg_napaka;
		msg_napaka.setText("Vnesli ste previsoko vrednost. Vrednost je avtomatsko "
											 "popravljena na najvisjo dovoljena vrednost za to polje!");
		msg_napaka.exec();
		return;
	}

	sestevek = 0.0;
	for ( int i = 0; i <= 4; i++ ) {
		sestevek += popust[i];
	}
	ui->txt_popusti_skupaj_2->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)) + " %");

	sestevek += popust[4];

	ui->txt_popusti->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)) + " %");

}

void opravila::on_cb_popust_fb1_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_fb1->setEnabled(true);
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
	else {
		ui->txt_popust_fb1->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_fb1->setEnabled(false);
	}

	ui->txt_popust_fb1->setText(ui->txt_popust_fb1->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_fb2_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_fb2->setEnabled(true);

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
	else {
		ui->txt_popust_fb2->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_fb2->setEnabled(false);
	}

	ui->txt_popust_fb2->setText(ui->txt_popust_fb2->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_priporocilo_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_priporocilo->setEnabled(true);

			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_priporocilo")).toString()) != "0.0" ) { // projekt ima popust
					ui->txt_popust_priporocilo->setText(pretvori_iz_double(prevedi(sql_opravila.value(sql_opravila.record().indexOf("popust_priporocilo")).toString())));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_projekti;
				sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id_projekt->text()) + "'");
				sql_projekti.exec();
				if ( sql_projekti.next() ) {
					if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_priporocilo")).toString()) != "0.0" ) { // projekt ima popust
						ui->txt_popust_priporocilo->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_priporocilo")).toString())));
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
						ui->txt_popust_priporocilo->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_1")).toString())));
						nipopusta = "false";
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kombinacija_1") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_popust_priporocilo->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
				}
			}
	}
	else {
		ui->txt_popust_priporocilo->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_priporocilo->setEnabled(false);
	}

	ui->txt_popust_priporocilo->setText(ui->txt_popust_priporocilo->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_stalna_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_stalna_stranka->setEnabled(true);

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
	else {
		ui->txt_popust_stalna_stranka->setText(pretvori_iz_double("0.0"));
		ui->txt_popust_stalna_stranka->setEnabled(false);
	}

	ui->txt_popust_stalna_stranka->setText(ui->txt_popust_stalna_stranka->text().remove(" %").replace(".", ",") + " %");

}

void opravila::on_cb_popust_akcija_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_popust_akcija->setEnabled(true);

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

void opravila::on_txt_popust_fb1_textChanged() {

	izracunaj_popuste(0);

}

void opravila::on_txt_popust_fb2_textChanged() {

	izracunaj_popuste(1);

}

void opravila::on_txt_popust_priporocilo_textChanged() {

	izracunaj_popuste(2);

}

void opravila::on_txt_popust_stalna_stranka_textChanged() {

	izracunaj_popuste(3);

}

void opravila::on_txt_popust_akcija_textChanged() {

	izracunaj_popuste(4);

}

void opravila::on_txt_popust_fb1_editingFinished() {

	double popust = pretvori_v_double(ui->txt_popust_fb1->text()).toDouble();

	if ( popust > 100.0 ) {
		popust = 100.0;
	}
	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_txt_popust_fb2_editingFinished() {

	double popust = pretvori_v_double(ui->txt_popust_fb2->text()).toDouble();

	if ( popust > 100.0 ) {
		popust = 100.0;
	}
	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_txt_popust_priporocilo_editingFinished() {

	double popust = pretvori_v_double(ui->txt_popust_priporocilo->text()).toDouble();

	if ( popust > 100.0 ) {
		popust = 100.0;
	}
	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_popust_priporocilo->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_txt_popust_stalna_stranka_editingFinished() {

	double popust = pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();

	if ( popust > 100.0 ) {
		popust = 100.0;
	}
	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_txt_popust_akcija_editingFinished() {

	double popust = pretvori_v_double(ui->txt_popust_akcija->text()).toDouble();

	if ( popust > 100.0 ) {
		popust = 100.0;
	}
	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_popust_akcija->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_txt_podrazitev_vikend_editingFinished() {

	double popust = pretvori_v_double(ui->txt_podrazitev_vikend->text()).toDouble();

	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_podrazitev_vikend->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_txt_podrazitev_hitrost_editingFinished() {

	double popust = pretvori_v_double(ui->txt_podrazitev_hitrost->text()).toDouble();

	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_txt_podrazitev_zapleti_editingFinished() {

	double popust = pretvori_v_double(ui->txt_podrazitev_zapleti->text()).toDouble();

	if ( popust < 0.0 ) {
		popust = 0.0;
	}

	ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(QString::number(popust, 'f', 1)) + " %");

}

void opravila::on_gb_urejanje_pribitkov_toggled() {

	if ( ui->gb_urejanje_pribitkov->isChecked() ) {
		ui->gb_urejanje_pribitkov_notranji->setVisible(true);
	}
	else {
		ui->gb_urejanje_pribitkov_notranji->setVisible(false);
	}

}


void opravila::on_gb_najvisji_zneski_popustov_toggled() {

	if ( ui->gb_najvisji_zneski_popustov->isChecked() ) {
		ui->gb_najvisji_zneski_popustov_notranji->setVisible(true);
	}
	else {
		ui->gb_najvisji_zneski_popustov_notranji->setVisible(false);
	}

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

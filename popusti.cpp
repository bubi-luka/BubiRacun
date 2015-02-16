#include <QtSql>
#include <QMessageBox>

#include "popusti.h"
#include "ui_popusti.h"
#include "kodiranje.h"


popusti::popusti(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::popusti)
{
	ui->setupUi(this);

	/*
		* Nastavimo validacijo polj
		* Polja vsebujejo eno ali dve celi stevilki, sledi vejica ali pika,
		* sledi ena stevilka, nato je lahko presledek in znak za odstotek
		* ali pa nic.
	*/
		QRegExp rx_procent("\\d{0,2}([(.|,)][(0-9)]){0,1}[ ]?[%]?");
		QValidator *v_procent = new QRegExpValidator(rx_procent, this);

		ui->txt_popust_fb1->setValidator(v_procent);
		ui->txt_popust_fb2->setValidator(v_procent);
		ui->txt_popust_komb1->setValidator(v_procent);
		ui->txt_popust_komb2->setValidator(v_procent);
		ui->txt_popust_stalna_stranka->setValidator(v_procent);
		ui->txt_popust_akcija->setValidator(v_procent);

		ui->txt_vsi_popusti_facebook->setValidator(v_procent);
		ui->txt_popusti_skupaj->setValidator(v_procent);

		ui->txt_podrazitev_vikend->setValidator(v_procent);
		ui->txt_podrazitev_hitrost->setValidator(v_procent);
		ui->txt_podrazitev_zapleti->setValidator(v_procent);

		// napolni vrednosti iz baze podatkov
		napolni();

}

popusti::~popusti()
{
	delete ui;
}

void popusti::on_btn_zapri_clicked() {

	close();

}

void popusti::on_txt_popust_fb1_editingFinished() {

	ui->txt_popust_fb1->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_fb1->text())));

}

void popusti::on_txt_popust_fb2_editingFinished() {

	ui->txt_popust_fb2->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_fb2->text())));

}

void popusti::on_txt_popust_komb1_editingFinished() {

	ui->txt_popust_komb1->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_komb1->text())));

}

void popusti::on_txt_popust_komb2_editingFinished() {

	ui->txt_popust_komb2->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_komb2->text())));

}

void popusti::on_txt_popust_stalna_stranka_editingFinished() {

	ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_stalna_stranka->text())));

}

void popusti::on_txt_popust_akcija_editingFinished() {

	ui->txt_popust_akcija->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popust_akcija->text())));

}

void popusti::on_txt_vsi_popusti_facebook_editingFinished() {

	ui->txt_vsi_popusti_facebook->setText(pretvori_iz_double(pretvori_v_double(ui->txt_vsi_popusti_facebook->text())));

}

void popusti::on_txt_popusti_skupaj_editingFinished() {

	ui->txt_popusti_skupaj->setText(pretvori_iz_double(pretvori_v_double(ui->txt_popusti_skupaj->text())));

}

void popusti::on_txt_podrazitev_vikend_editingFinished() {

	ui->txt_podrazitev_vikend->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_vikend->text())));

}

void popusti::on_txt_podrazitev_hitrost_editingFinished() {

	ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_hitrost->text())));

}

void popusti::on_txt_podrazitev_zapleti_editingFinished() {

	ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(pretvori_v_double(ui->txt_podrazitev_zapleti->text())));

}

void popusti::on_btn_vnesi_clicked() {

	QString str_vrednost[12];

	str_vrednost[0] = ui->txt_popust_fb1->text();
	str_vrednost[1] = ui->txt_popust_fb2->text();
	str_vrednost[2] = ui->txt_popust_komb1->text();
	str_vrednost[3] = ui->txt_popust_komb2->text();
	str_vrednost[4] = ui->txt_popust_stalna_stranka->text();
	str_vrednost[6] = ui->txt_popust_akcija->text();
	str_vrednost[7] = ui->txt_vsi_popusti_facebook->text();
	str_vrednost[8] = ui->txt_popusti_skupaj->text();
	str_vrednost[9] = ui->txt_podrazitev_vikend->text();
	str_vrednost[10] = ui->txt_podrazitev_hitrost->text();
	str_vrednost[11] = ui->txt_podrazitev_zapleti->text();

		for ( int i = 0; i <= 11; i++ ) {
			QSqlQuery sql_vrednost;
			sql_vrednost.prepare("UPDATE sif_popusti SET vrednost = ? WHERE id LIKE '" + QString::number(i + 1, 10) + "'");
			sql_vrednost.bindValue(0, pretvori(pretvori_v_double(str_vrednost[i])));
			sql_vrednost.exec();
		}

	napolni();

	QMessageBox msgbox;
	msgbox.setText("Popusti so uspesno posodobljeni");
	msgbox.exec();

}

void popusti::napolni() {

	QString str_napolni[12];
	int i = 0;

		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM sif_popusti ORDER BY id ASC");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			str_napolni[i] = prevedi(pretvori_iz_double(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString()));
			i++;
		}
		ui->txt_popust_fb1->setText(str_napolni[0]);
		ui->txt_popust_fb2->setText(str_napolni[1]);
		ui->txt_popust_komb1->setText(str_napolni[2]);
		ui->txt_popust_komb2->setText(str_napolni[3]);
		ui->txt_popust_stalna_stranka->setText(str_napolni[4]);
		ui->txt_popust_akcija->setText(str_napolni[5]);
		ui->txt_vsi_popusti_facebook->setText(str_napolni[6]);
		ui->txt_popusti_skupaj->setText(str_napolni[7]);
		ui->txt_podrazitev_vikend->setText(str_napolni[8]);
		ui->txt_podrazitev_hitrost->setText(str_napolni[9]);
		ui->txt_podrazitev_zapleti->setText(str_napolni[10]);

}

QString popusti::pretvori_v_double(QString besedilo) {

	besedilo.remove(" ");
	besedilo.remove("%");
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

QString popusti::pretvori_iz_double(QString besedilo) {

	besedilo.append(" %");
	besedilo.replace(".",",");

	return besedilo;
}

QString popusti::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString popusti::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

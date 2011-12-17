#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "potovanja.h"
#include "ui_potovanja.h"
#include "glavnookno.h"
#include "kodiranje.h"

potovanja::potovanja(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::potovanja)
{
	ui->setupUi(this);

	// zbrisi vsebino polj
	ui->txt_id->setText("");
	ui->txt_stevilka_naloga->setText("");
	ui->txt_kraj_odhoda->setText("");
	ui->txt_kraj_prihoda->setText("");
	ui->txt_naslov->setPlainText("");
	ui->txt_kilometri->setText("");
	ui->txt_povratek->setText("");

	// onemogoci polja
	ui->txt_id->setEnabled(false);
	ui->txt_stevilka_naloga->setEnabled(false);

	// nastavi datum
	ui->txt_datum_odhoda->setDateTime(QDateTime::currentDateTime());
	ui->txt_datum_prihoda->setDateTime(QDateTime::currentDateTime());

	// doloci dovoljeno vsebino
	QDoubleValidator *validator = new QDoubleValidator(0, 999999, 2, this);
	ui->txt_kilometri->setValidator(validator);
}

potovanja::~potovanja()
{
	delete ui;
}

void potovanja::on_btn_izhod_clicked() {

	close();

}

// prazno, dokler se ne dogovorimo o dejavnosti gumba
void potovanja::on_btn_brisi_clicked() {

}

// ne preverja obveznih polj
void potovanja::on_btn_sprejmi_clicked() {

	QString napaka = "";

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

	// check that departure time is not greater or equall than arrival time
	if ( ui->txt_datum_odhoda->dateTime().secsTo(ui->txt_datum_prihoda->dateTime()) <= 0) {
		napaka = napaka + "true";
		ui->label_2->setPalette(palette_error);
		ui->label_2->setFont(font_error);
		ui->txt_datum_prihoda->setFocus();
	}

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
			QSqlQuery sql_vnesi_pot;
			QSqlQuery sql_vnesi_potnazaj;
			if (ui->btn_sprejmi->text() == "Vnesi pot") { // vnesi novo pot
				sql_vnesi_pot.prepare("INSERT INTO potovanja (potni_nalog, kraj_odhoda, kraj_prihoda, cas_odhoda, cas_prihoda, kilometri, naslov) "
											 "VALUES (?, ?, ?, ?, ?, ?, ?) ");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_pot.prepare("UPDATE potovanja SET potni_nalog = ?, kraj_odhoda = ?, kraj_prihoda = ?, cas_odhoda = ?, cas_prihoda = ?, "
															"kilometri = ?, naslov = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_pot.bindValue(0, pretvori(ui->txt_stevilka_naloga->text()));
			sql_vnesi_pot.bindValue(1, pretvori(ui->txt_kraj_odhoda->text()));
			sql_vnesi_pot.bindValue(2, pretvori(ui->txt_kraj_prihoda->text()));
			sql_vnesi_pot.bindValue(3, pretvori(ui->txt_datum_odhoda->text()));
			sql_vnesi_pot.bindValue(4, pretvori(ui->txt_datum_prihoda->text()));
			sql_vnesi_pot.bindValue(5, pretvori(ui->txt_kilometri->text().replace(",", ".")));
			sql_vnesi_pot.bindValue(6, pretvori(ui->txt_naslov->toPlainText()));
			sql_vnesi_pot.exec();

	//		if ( ui->btn_sprejmi->text() == "Vnesi pot" && ui->txt_povratek->isChecked() ) {
			if ( ui->txt_povratek->isChecked() ) { // ce zelis, da vnese povratno pot tudi ob popravljanju
				sql_vnesi_potnazaj.prepare("INSERT INTO potovanja (potni_nalog, kraj_odhoda, kraj_prihoda, cas_odhoda, cas_prihoda, kilometri, naslov) "
																	 "VALUES (?, ?, ?, ?, ?, ?, ?) ");
				sql_vnesi_potnazaj.bindValue(0, pretvori(ui->txt_stevilka_naloga->text()));
				sql_vnesi_potnazaj.bindValue(1, pretvori(ui->txt_kraj_prihoda->text()));
				sql_vnesi_potnazaj.bindValue(2, pretvori(ui->txt_kraj_odhoda->text()));
				sql_vnesi_potnazaj.bindValue(3, pretvori(ui->txt_datum_prihoda->text()));
				sql_vnesi_potnazaj.bindValue(4, pretvori(ui->txt_datum_odhoda->text()));
				sql_vnesi_potnazaj.bindValue(5, pretvori(ui->txt_kilometri->text().replace(",", ".")));
				sql_vnesi_potnazaj.bindValue(6, pretvori(ui->txt_naslov->toPlainText()));
				sql_vnesi_potnazaj.exec();
			}

		}
		base.close();

		// send signal to reload widget
		poslji("potovanja");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void potovanja::keyPressEvent(QKeyEvent *event) {
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

// prazno
void potovanja::prejem(QString besedilo) {

	if (besedilo.left(8) == "Nova pot") {
		ui->btn_sprejmi->setText("Vnesi pot");
		ui->txt_stevilka_naloga->setText(besedilo.right(besedilo.length() - 8));
		ui->txt_povratek->setHidden(false);
		ui->label_6->setHidden(false);
	}
	else {
		ui->btn_sprejmi->setText("Popravi pot");
		ui->txt_povratek->setHidden(true);
		ui->label_6->setHidden(true);
		// besedilo nosi ID ze obstojecega uporabnika, potrebno je napolniti polja
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
			sql_napolni.prepare("SELECT * FROM potovanja WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_stevilka_naloga->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("potni_nalog")).toString()));
				ui->txt_kraj_odhoda->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kraj_odhoda")).toString()));
				ui->txt_kraj_prihoda->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kraj_prihoda")).toString()));
				ui->txt_kilometri->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kilometri")).toString()));
				ui->txt_naslov->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));

				QDateTime datum = QDateTime::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cas_odhoda")).toString()), "d'.'MM'.'yyyy' 'hh':'mm");
				ui->txt_datum_odhoda->setDateTime(datum);
				datum = QDateTime::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cas_prihoda")).toString()), "d'.'MM'.'yyyy' 'hh':'mm");
				ui->txt_datum_prihoda->setDateTime(datum);
			}
		}
		base.close();
	}
}

QString potovanja::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString potovanja::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

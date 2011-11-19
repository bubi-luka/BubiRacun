#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "projekti.h"
#include "ui_projekti.h"
#include "opravila.h"
#include "kodiranje.h"
#include "wid_racuni.h"

projekti::projekti(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::projekti)
{
    ui->setupUi(this);

	// set date for today
	ui->txt_pricetek->setDate(QDate::currentDate());
	ui->txt_konec->setDate(QDate::currentDate());

	// disable fields
	ui->txt_id->setEnabled(false);
	ui->txt_stranka_id->setEnabled(false);
	ui->txt_kupon1->setEnabled(false);
	ui->txt_kupon2->setEnabled(false);
	ui->txt_kupon3->setEnabled(false);
//	ui->txt_konec->setEnabled(false);
//	ui->txt_pricetek->setEnabled(false);
	ui->txt_znesekbrezddv->setEnabled(false);
	ui->txt_ddv->setEnabled(false);
	ui->txt_popusti->setEnabled(false);
	ui->txt_znesek->setEnabled(false);

	ui->txt_stranka_id->setHidden(true);

	// fill combo box and tables
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
		// insert project number
		QString leto = QDate::currentDate().toString("yyyy");
		int i = 1;
		QString stevilka = "";
		QString stevilka1 = "";
		QString stevilka2 = "";
		QString stevilka3 = "";

		QSqlQuery sql_insert_stnaloga;
		sql_insert_stnaloga.prepare("SELECT * FROM projekti WHERE stprojekta LIKE '" + pretvori("SP-" + leto) + "%'");
		sql_insert_stnaloga.exec();
		while (sql_insert_stnaloga.next()) {
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
		ui->txt_projekt->setText("SP-" + leto + "-" + stevilka);

		// fill combo box
		ui->txt_statusprojekta->addItem("");
		ui->txt_stranka->addItem("");

		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM sif_projekt");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_statusprojekta->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("projekt")).toString()));
		}

		sql_fill_combo.clear();
		sql_fill_combo.prepare("SELECT * FROM stranke");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			if (prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("tip")).toString()) == "fizicna" ) {
				ui->txt_stranka->addItem("(" + sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
										 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("priimek")).toString()) + ", "
										 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
			}
			else {
				ui->txt_stranka->addItem("(" + sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
										 + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
			}
		}

		// fill cupon numbers
		i = 1;
		QSqlQuery sql_insert_stkupona;
		sql_insert_stkupona.prepare("SELECT * FROM kuponi WHERE kupon LIKE '" + pretvori("KU-" + leto) + "%'");
		sql_insert_stkupona.exec();
		while (sql_insert_stkupona.next()) {
			i++;
		}

		if ( i < 10 ) {
			stevilka1 = "00" + QString::number(i, 10);
		}
		else if ( i < 100 ) {
			stevilka1 = "0" + QString::number(i, 10);
		}
		else {
			stevilka1 = "" + QString::number(i, 10);
		}

		i = i + 1;
		if ( i < 10 ) {
			stevilka2 = "00" + QString::number(i, 10);
		}
		else if ( i < 100 ) {
			stevilka2 = "0" + QString::number(i, 10);
		}
		else {
			stevilka2 = "" + QString::number(i, 10);
		}

		i = i + 1;
		if ( i < 10 ) {
			stevilka3 = "00" + QString::number(i, 10);
		}
		else if ( i < 100 ) {
			stevilka3 = "0" + QString::number(i, 10);
		}
		else {
			stevilka3 = "" + QString::number(i, 10);
		}

		ui->txt_kupon1->setText("KU-" + leto + "-" + stevilka1);
		ui->txt_kupon2->setText("KU-" + leto + "-" + stevilka2);
		ui->txt_kupon3->setText("KU-" + leto + "-" + stevilka3);

	}
	base.close();

}

projekti::~projekti()
{
	delete ui;

}

void projekti::keyPressEvent(QKeyEvent *event) {
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

// prazno, dokler ne definiramo funkcije gumba
void projekti::on_btn_brisi_clicked() {

}

void projekti::on_btn_izhod_clicked() {

	close();

}

void projekti::on_txt_stranka_currentIndexChanged(QString besedilo) {

	if (besedilo != "") {
		ui->txt_stranka_id->setText(besedilo.left(besedilo.indexOf(")", 1)));
		ui->txt_stranka_id->setText(ui->txt_stranka_id->text().right(ui->txt_stranka_id->text().length() - 1));
	}
	else {
		ui->txt_stranka_id->setText("");
	}

	wid_racuni *widr = new wid_racuni;
	ui->wid_racuni->setWidget(widr);

	QObject::connect(this, SIGNAL(prenos(QString)),
			   widr , SLOT(prejem(QString)));
	prenos(ui->txt_stranka_id->text());
	this->disconnect();

}

// ne preverja obveznih polj
void projekti::on_btn_sprejmi_clicked() {

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
			QSqlQuery sql_vnesi_projekt;
			if (ui->btn_sprejmi->text() == "Vnesi projekt") {
				sql_vnesi_projekt.prepare("INSERT INTO projekti (stprojekta, naziv, stranka, pricetek, konec, statusprojekta) "
										  "VALUES (?, ?, ?, ?, ?, ?)");
			}
			else {
				sql_vnesi_projekt.prepare("UPDATE projekti SET stprojekta = ?, naziv = ?, stranka = ?, pricetek = ?, konec = ?, statusprojekta = ? "
										  "WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_projekt->text()));
			sql_vnesi_projekt.bindValue(1, pretvori(ui->txt_naziv->text()));
			sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_stranka_id->text()));
			sql_vnesi_projekt.bindValue(3, pretvori(ui->txt_pricetek->text()));
			sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_konec->text()));
			sql_vnesi_projekt.bindValue(5, pretvori(ui->txt_statusprojekta->currentText()));

			sql_vnesi_projekt.exec();

			// shrani kupon
			if ( ui->btn_sprejmi->text() == "Vnesi projekt") {
				QSqlQuery sql_vnesi_kupon;
				sql_vnesi_kupon.prepare("INSERT INTO kuponi (kupon, projekt, prejemnik, datumprejema) VALUES (?, ?, ?, ?)");
				sql_vnesi_kupon.bindValue(0, pretvori(ui->txt_kupon1->text()));
				sql_vnesi_kupon.bindValue(1, pretvori(ui->txt_projekt->text()));
				sql_vnesi_kupon.bindValue(2, pretvori(ui->txt_stranka_id->text()));
				sql_vnesi_kupon.bindValue(3, pretvori(ui->txt_pricetek->text()));
				sql_vnesi_kupon.exec();
				sql_vnesi_kupon.clear();
				sql_vnesi_kupon.prepare("INSERT INTO kuponi (kupon, projekt, prejemnik, datumprejema) VALUES (?, ?, ?, ?)");
				sql_vnesi_kupon.bindValue(0, pretvori(ui->txt_kupon2->text()));
				sql_vnesi_kupon.bindValue(1, pretvori(ui->txt_projekt->text()));
				sql_vnesi_kupon.bindValue(2, pretvori(ui->txt_stranka_id->text()));
				sql_vnesi_kupon.bindValue(3, pretvori(ui->txt_pricetek->text()));
				sql_vnesi_kupon.exec();
				sql_vnesi_kupon.clear();
				sql_vnesi_kupon.prepare("INSERT INTO kuponi (kupon, projekt, prejemnik, datumprejema) VALUES (?, ?, ?, ?)");
				sql_vnesi_kupon.bindValue(0, pretvori(ui->txt_kupon3->text()));
				sql_vnesi_kupon.bindValue(1, pretvori(ui->txt_projekt->text()));
				sql_vnesi_kupon.bindValue(2, pretvori(ui->txt_stranka_id->text()));
				sql_vnesi_kupon.bindValue(3, pretvori(ui->txt_pricetek->text()));
				sql_vnesi_kupon.exec();
			}
		}

		base.close();

		// send signal to reload widget
		poslji("projekti");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void projekti::prejem(QString besedilo) {

	if (besedilo.left(11) == "Nov projekt") {
		ui->btn_sprejmi->setText("Vnesi projekt");
		ui->txt_stranka_id->setText(besedilo.right(besedilo.length() - 11));
	}
	else {
		ui->btn_sprejmi->setText("Popravi vnos");
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
			sql_napolni.prepare("SELECT * FROM projekti WHERE stprojekta LIKE '" + pretvori(besedilo) + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_projekt->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stprojekta")).toString()));
				ui->txt_naziv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()));
				ui->txt_stranka_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString()));

				QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pricetek")).toString()), "dd'.'MM'.'yyyy");
				ui->txt_pricetek->setDate(datum);
//				ui->txt_pricetek->setEnabled(true);

//				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("konec")).toString()) != "0" ) { // konec projekta je odvisen od stanja combo boxa
					datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("konec")).toString()), "dd'.'MM'.'yyyy");
					ui->txt_konec->setDate(datum);
//					ui->txt_konec->setEnabled(true);
//				}

				QSqlQuery sql_combo;
				bool ok;
				sql_combo.prepare("SELECT * FROM sif_projekt WHERE projekt LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("statusprojekta")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_statusprojekta->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}

				QSqlQuery sql_fill_kuponi;
				sql_fill_kuponi.prepare("SELECT * FROM kuponi WHERE projekt LIKE '" + pretvori(ui->txt_projekt->text()) + "'");
				sql_fill_kuponi.exec();
				if (sql_fill_kuponi.next()) {
					ui->txt_kupon1->setText(prevedi(sql_fill_kuponi.value(sql_fill_kuponi.record().indexOf("kupon")).toString()));
				}
				if (sql_fill_kuponi.next()) {
					ui->txt_kupon2->setText(prevedi(sql_fill_kuponi.value(sql_fill_kuponi.record().indexOf("kupon")).toString()));
				}
				if (sql_fill_kuponi.next()) {
					ui->txt_kupon3->setText(prevedi(sql_fill_kuponi.value(sql_fill_kuponi.record().indexOf("kupon")).toString()));
				}
			}
		}
		base.close();

		wid_racuni *widr = new wid_racuni;
		ui->wid_racuni->setWidget(widr);

		QObject::connect(this, SIGNAL(prenos(QString)),
				   widr , SLOT(prejem(QString)));
		prenos(ui->txt_projekt->text());
		this->disconnect();

		izracunaj(); // calculate the values

	}

}

QString projekti::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString projekti::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void projekti::on_txt_stranka_id_textChanged(QString besedilo) {

	int i = 0;

	while ( i <= ui->txt_stranka->count() ) {
		if ( ui->txt_stranka->itemText(i).left(besedilo.length() + 2) == ("(" + besedilo + ")") ) {
			ui->txt_stranka->setCurrentIndex(i);
		}
		i++;
	}

}

void projekti::izracunaj() {

	double d_znesekbrezddv = 0;
	double d_ddv = 0;
	double d_popusti = 0;
	double d_znesek = 0;

	bool *ok;

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
		sql_racun.prepare("SELECT * FROM racuni WHERE stranka LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
		sql_racun.exec();
		while ( sql_racun.next() ) {
			QSqlQuery sql_opravilo;
			sql_opravilo.prepare("SELECT * FROM opravila WHERE racun LIKE '" + sql_racun.value(sql_racun.record().indexOf("stracuna")).toString() + "'");
			sql_opravilo.exec();
			while ( sql_opravilo.next() ) {
				d_znesekbrezddv = d_znesekbrezddv + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesekbrezddv")).toString()).toDouble(ok);
				d_ddv = d_ddv + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesekddv")).toString()).toDouble(ok);
				d_popusti = d_popusti + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("popusti")).toString()).toDouble(ok);
				d_znesek = d_znesek + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesekskupaj")).toString()).toDouble(ok);
			}
		}

	}
	base.close();

	ui->txt_znesekbrezddv->setText(QString::number(d_znesekbrezddv, 'f', 2));
	ui->txt_ddv->setText(QString::number(d_ddv, 'f', 2));
	ui->txt_popusti->setText(QString::number(d_popusti, 'f', 2));
	ui->txt_znesek->setText(QString::number(d_znesek, 'f', 2));

}

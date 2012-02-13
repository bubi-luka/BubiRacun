#include <QtSql>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>

#include "wid_prejetiracuni.h"
#include "ui_wid_prejetiracuni.h"
#include "prejetiracuni.h"
#include "kodiranje.h"
#include "varnost.h"

wid_prejetiracuni::wid_prejetiracuni(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_prejetiracuni)
{
    ui->setupUi(this);

		// disable and hide
		ui->txt_stprojekta->setEnabled(false);
		ui->txt_stprojekta->setVisible(false);

		// napolni filtrirne spustne sezname
		QString gumb = ui->btn_nov->text();
		ui->btn_nov->setText("");

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

			// filtriraj po mesecu
			ui->cb_mesec->addItem("");
			ui->cb_mesec->addItem("01) Januar");
			ui->cb_mesec->addItem("02) Februar");
			ui->cb_mesec->addItem("03) Marec");
			ui->cb_mesec->addItem("04) April");
			ui->cb_mesec->addItem("05) Maj");
			ui->cb_mesec->addItem("06) Junij");
			ui->cb_mesec->addItem("07) Julij");
			ui->cb_mesec->addItem("08) Avgust");
			ui->cb_mesec->addItem("09) September");
			ui->cb_mesec->addItem("10) Oktober");
			ui->cb_mesec->addItem("11) November");
			ui->cb_mesec->addItem("12) December");

			// filtriraj po letu
			ui->cb_leto->addItem("");
			int min_leto = QDate::currentDate().year();
			int max_leto = QDate::currentDate().year();

			sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE avtor LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				if ( min_leto > prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt() ) {
					min_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt();
				}
				if ( max_leto < prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt() ) {
					max_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_prejema")).toString()).right(4).toInt();
				}
			}

			for ( int i = min_leto; i <= max_leto; i++ ) {
				ui->cb_leto->addItem(QString::number(i, 10));
			}
			sql_napolni.clear();

			// filtriraj po izdajatelju racuna
			ui->cb_izdajatelj->addItem("");
			sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE avtor LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				if ( ui->cb_izdajatelj->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("izdajatelj_kratki")).toString())) == -1 ) {
					ui->cb_izdajatelj->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("izdajatelj_kratki")).toString()));
				}
			}
			sql_napolni.clear();

			// filtriraj po statusu placila
			ui->cb_placilo->addItem("");
			sql_napolni.prepare("SELECT * FROM sif_status_placila");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_placilo->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString()));
			}
			sql_napolni.clear();

			// filtriraj po statusu racunovodstva
			ui->cb_racunovodstvo->addItem("");
			sql_napolni.prepare("SELECT * FROM sif_status_racunovodstva");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_racunovodstvo->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString()));
			}
			sql_napolni.clear();

		}
		base.close();

		napolni();

		ui->btn_nov->setText(gumb);

}

wid_prejetiracuni::~wid_prejetiracuni()
{
    delete ui;
}

void wid_prejetiracuni::on_cb_mesec_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_prejetiracuni::on_cb_leto_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_prejetiracuni::on_cb_izdajatelj_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_prejetiracuni::on_cb_placilo_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_prejetiracuni::on_cb_racunovodstvo_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_prejetiracuni::napolni() {

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
		ui->tbl_racuni->clear();

		for (int i = 0; i <= 6; i++) {
			ui->tbl_racuni->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM prejeti_racuni");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_racuni->removeRow(0);
		}

		// start filling the table
		ui->tbl_racuni->insertColumn(0);
		ui->tbl_racuni->insertColumn(1);
		ui->tbl_racuni->insertColumn(2);
		ui->tbl_racuni->insertColumn(3);
		ui->tbl_racuni->insertColumn(4);
		ui->tbl_racuni->insertColumn(5);
		ui->tbl_racuni->insertColumn(6);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;
		QTableWidgetItem *naslov6 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Stevilka racuna");
		naslov2->setText("Datum prejema");
		naslov3->setText("Izdajatelj");
		naslov4->setText("Rok placila");
		naslov5->setText("Status placila");
		naslov6->setText("Status racunovodstva");

		ui->tbl_racuni->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_racuni->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_racuni->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_racuni->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_racuni->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_racuni->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_racuni->setHorizontalHeaderItem(6, naslov6);

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QString stavek = "";

		if ( ui->cb_izdajatelj->currentText() != "" ) {
			stavek += " AND izdajatelj_kratki LIKE '" + pretvori(ui->cb_izdajatelj->currentText()) + "'";
		}
		if ( ui->cb_placilo->currentText() != "" ) {
			stavek += " AND status_placila LIKE '" + pretvori (ui->cb_placilo->currentText()) + "'";
		}
		if ( ui->cb_racunovodstvo->currentText() != "" ) {
			stavek += " AND status_racunovodstva LIKE '" + pretvori (ui->cb_racunovodstvo->currentText()) + "'";
		}

		if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
			stavek = " WHERE" + stavek.right(stavek.length() - 4);
		}

		QSqlQuery sql_fill;
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_projekta LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM prejeti_racuni" + stavek);
		}
		sql_fill.exec();

		int row = 0;

		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}

			if ( filter == "pozitivno" ) {
				ui->tbl_racuni->insertRow(row);
				ui->tbl_racuni->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[7] = {"id", "stevilka_racuna", "datum_prejema", "izdajatelj_kratki", "rok_placila", "status_placila",
														"status_racunovodstva"};

				while (col <= 6) {

					QTableWidgetItem *celica = new QTableWidgetItem;
					celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					ui->tbl_racuni->setItem(row, col, celica);

					col++;
					i++;
				}
				row++;
			}
		}
	}
	base.close();

}

void wid_prejetiracuni::on_btn_brisi_clicked() {

	QString id = ui->tbl_racuni->selectedItems().takeAt(0)->text();

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
		sql_brisi.prepare("DELETE FROM prejeti_racuni WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_racuni->removeRow(ui->tbl_racuni->selectedItems().takeAt(0)->row());
	osvezi("prejetiracuniracuni");

}

void wid_prejetiracuni::on_btn_nov_clicked() {

	prejetiracuni *uredi = new prejetiracuni;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov racun");
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_prejetiracuni::on_tbl_racuni_doubleClicked() {

	prejetiracuni *uredi = new prejetiracuni;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_racuni->selectedItems().takeAt(0)->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_prejetiracuni::osvezi(QString beseda) {

	if ( beseda == "racun" ) {
		napolni();
	}

}

QString wid_prejetiracuni::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_prejetiracuni::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void wid_prejetiracuni::prejem(QString besedilo) {

	ui->txt_stprojekta->setText(besedilo);

	napolni();

}

void wid_prejetiracuni::on_btn_refresh_clicked() {

	napolni();

}

void wid_prejetiracuni::on_btn_print_clicked() {

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

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QString stavek = "";

		if ( ui->cb_izdajatelj->currentText() != "" ) {
			stavek += " AND izdajatelj_kratki LIKE '" + pretvori(ui->cb_izdajatelj->currentText()) + "'";
		}
		if ( ui->cb_placilo->currentText() != "" ) {
			stavek += " AND status_placila LIKE '" + pretvori (ui->cb_placilo->currentText()) + "'";
		}
		if ( ui->cb_racunovodstvo->currentText() != "" ) {
			stavek += " AND status_racunovodstva LIKE '" + pretvori (ui->cb_racunovodstvo->currentText()) + "'";
		}

		if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
			stavek = " WHERE" + stavek.right(stavek.length() - 4);
		}

		QSqlQuery sql_fill;
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_projekta LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM prejeti_racuni" + stavek);
		}
		sql_fill.exec();

		int row = 0;

		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}

			if ( filter == "pozitivno" ) {
				print(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
			}
		}
	}
	base.close();

}

void wid_prejetiracuni::on_btn_print_pdf_clicked() {

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

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QString stavek = "";

		if ( ui->cb_izdajatelj->currentText() != "" ) {
			stavek += " AND izdajatelj_kratki LIKE '" + pretvori(ui->cb_izdajatelj->currentText()) + "'";
		}
		if ( ui->cb_placilo->currentText() != "" ) {
			stavek += " AND status_placila LIKE '" + pretvori (ui->cb_placilo->currentText()) + "'";
		}
		if ( ui->cb_racunovodstvo->currentText() != "" ) {
			stavek += " AND status_racunovodstva LIKE '" + pretvori (ui->cb_racunovodstvo->currentText()) + "'";
		}

		if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
			stavek = " WHERE" + stavek.right(stavek.length() - 4);
		}

		QSqlQuery sql_fill;
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_projekta LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM prejeti_racuni" + stavek);
		}
		sql_fill.exec();

		int row = 0;

		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_prejema")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}

			if ( filter == "pozitivno" ) {
				printpdf(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
			}
		}
	}
	base.close();

}

void wid_prejetiracuni::on_btn_print_seznam_clicked() {

}

void wid_prejetiracuni::print(QString id) {

	QPrinter printer;

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Natisni prejet racun"));
	printer.setPaperSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

	if (dialog->exec() == QDialog::Accepted) {
		QPainter painter;

		if (! painter.begin(&printer))  { // failed to open file
			qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
			return;
		}

		// nastavi pisavo
		painter.setFont(QFont("Arial", 12));

		// spremenljivke
		QString stevilka_vnosa = "";
		QString stevilka_racuna = "";
		QString izdajatelj_racuna = "";
		QString zadeva = "";
		QString placnik = "";
		QString projekt = "";
		QString datum_prejema = "";
		QString datum_placila = "";
		QString rok_placila = "";
		QString znesek_brez_ddv_00 = "";
		QString znesek_brez_ddv_85 = "";
		QString znesek_brez_ddv_20 = "";
		QString znesek_ddv = "";
		QString znesek = "";

		int pozicija = 0;

		// napolnimo spremenljivke z besedilom
		stevilka_vnosa = "Stevilka vnosa: ";
		stevilka_racuna = "Stevilka prejetega racuna: ";
		izdajatelj_racuna = "Izdajatelj racuna: ";
		zadeva = "Zadeva: ";
		placnik = "Placnik racuna: ";
		projekt = "Stevilka projekta: ";
		datum_prejema = "Datum prejema racuna: ";
		datum_placila = "Datum placila racuna: ";
		rok_placila = "Rok placila racuna: ";
		znesek_brez_ddv_00 = "Znesek brez DDV 0,0%: ";
		znesek_brez_ddv_85 = "Znesek brez DDV 8,5%: ";
		znesek_brez_ddv_20 = "Znesek brez DDV 20,0%: ";
		znesek_ddv = "Znesek DDV: ";
		znesek = "Koncni znesek: ";

		// odpremo bazo in napolnimo spremenljivke
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
			// baza je odprta

		/**
			* na zacetku zberemo podatke o potnem nalogu, izdajatelju potnega naloga
			* (podjetje, oseba), prejemniku potnega naloga ter opravljeni poti.
			**/

			// podatki o potnem nalogu
			QSqlQuery sql_prejeti_racuni;
			sql_prejeti_racuni.prepare("SELECT * FROM prejeti_racuni WHERE id LIKE '" + pretvori(id) + "'");
			sql_prejeti_racuni.exec();
			if ( sql_prejeti_racuni.next() ) {
				stevilka_vnosa += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_vnosa")).toString());
				stevilka_racuna += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_racuna")).toString());
				izdajatelj_racuna += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("izdajatelj_polni")).toString());
				zadeva += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("zadeva")).toString());
				datum_prejema += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("datum_prejema")).toString());;
				datum_placila += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("datum_placila")).toString());
				rok_placila += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("rok_placila")).toString());
				if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_00")).toString()) == "0.00" ) {
					znesek_brez_ddv_00 = "";
				}
				else {
					znesek_brez_ddv_00 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_00")).toString()).replace(".", ",") + " EUR";
				}
				if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_85")).toString()) == "0.00" ) {
					znesek_brez_ddv_85 = "";
				}
				else {
					znesek_brez_ddv_85 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_85")).toString()).replace(".", ",") + " EUR";
				}
				if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_20")).toString()) == "0.00" ) {
					znesek_brez_ddv_20 = "";
				}
				else {
					znesek_brez_ddv_20 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_20")).toString()).replace(".", ",") + " EUR";
				}
				znesek_ddv += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_ddv")).toString()).replace(".", ",") + " EUR";
				znesek += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek")).toString()).replace(".", ",") + " EUR";

				QSqlQuery sql_placnik;
				sql_placnik.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("placnik_podjetje")).toString() + "'");
				sql_placnik.exec();
				if ( sql_placnik.next() ) {
					placnik += prevedi(sql_placnik.value(sql_placnik.record().indexOf("polnoime")).toString());
				}

				QSqlQuery sql_projekt;
				sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_projekta")).toString() + "'");
				sql_projekt.exec();
				if ( sql_projekt.next() ) {
					projekt += prevedi(sql_projekt.value(sql_projekt.record().indexOf("stevilka_projekta")).toString());
				}

			}
		}
		base.close();

		// pripravimo dokument za tiskanje
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, stevilka_racuna);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, stevilka_racuna);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, izdajatelj_racuna);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, izdajatelj_racuna);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, zadeva);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, zadeva);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, placnik);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, placnik);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, projekt);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, projekt);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, datum_prejema);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, datum_prejema);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, datum_placila);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, datum_placila);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, rok_placila);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, rok_placila);
		pozicija += velikost_besedila.height();
		if ( znesek_brez_ddv_00 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_00);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_00);
			pozicija += velikost_besedila.height();
		}
		if ( znesek_brez_ddv_85 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_85);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_85);
			pozicija += velikost_besedila.height();
		}
		if ( znesek_brez_ddv_20 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_20);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_20);
			pozicija += velikost_besedila.height();
		}
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_ddv);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_ddv);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek);
		pozicija += velikost_besedila.height();

		painter.end();
	}

}

void wid_prejetiracuni::printpdf(QString id) {

	// spremenljivke
	QString stevilka_vnosa = "";
	QString stevilka_racuna = "";
	QString izdajatelj_racuna = "";
	QString zadeva = "";
	QString placnik = "";
	QString placnik_logotip = "";
	QString projekt = "";
	QString datum_prejema = "";
	QString datum_placila = "";
	QString rok_placila = "";
	QString znesek_brez_ddv_00 = "";
	QString znesek_brez_ddv_85 = "";
	QString znesek_brez_ddv_20 = "";
	QString znesek_ddv = "";
	QString znesek = "";

	int pozicija = 0;

	// napolnimo spremenljivke z besedilom
	stevilka_vnosa = "Stevilka vnosa: ";
	stevilka_racuna = "Stevilka prejetega racuna: ";
	izdajatelj_racuna = "Izdajatelj racuna: ";
	zadeva = "Zadeva: ";
	placnik = "Placnik racuna: ";
	projekt = "Stevilka projekta: ";
	datum_prejema = "Datum prejema racuna: ";
	datum_placila = "Datum placila racuna: ";
	rok_placila = "Rok placila racuna: ";
	znesek_brez_ddv_00 = "Znesek brez DDV 0,0%: ";
	znesek_brez_ddv_85 = "Znesek brez DDV 8,5%: ";
	znesek_brez_ddv_20 = "Znesek brez DDV 20,0%: ";
	znesek_ddv = "Znesek DDV: ";
	znesek = "Koncni znesek: ";

	// odpremo bazo in napolnimo spremenljivke
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
		// baza je odprta

	/**
		* na zacetku zberemo podatke o potnem nalogu, izdajatelju potnega naloga
		* (podjetje, oseba), prejemniku potnega naloga ter opravljeni poti.
		**/

		// podatki o potnem nalogu
		QSqlQuery sql_prejeti_racuni;
		sql_prejeti_racuni.prepare("SELECT * FROM prejeti_racuni WHERE id LIKE '" + pretvori(id) + "'");
		sql_prejeti_racuni.exec();
		if ( sql_prejeti_racuni.next() ) {
			stevilka_vnosa += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_vnosa")).toString());
			stevilka_racuna += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_racuna")).toString());
			izdajatelj_racuna += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("izdajatelj_polni")).toString());
			zadeva += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("zadeva")).toString());
			datum_prejema += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("datum_prejema")).toString());;
			datum_placila += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("datum_placila")).toString());
			rok_placila += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("rok_placila")).toString());
			if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_00")).toString()) == "0.00" ) {
				znesek_brez_ddv_00 = "";
			}
			else {
				znesek_brez_ddv_00 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_00")).toString()).replace(".", ",") + " EUR";
			}
			if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_85")).toString()) == "0.00" ) {
				znesek_brez_ddv_85 = "";
			}
			else {
				znesek_brez_ddv_85 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_85")).toString()).replace(".", ",") + " EUR";
			}
			if ( prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_20")).toString()) == "0.00" ) {
				znesek_brez_ddv_20 = "";
			}
			else {
				znesek_brez_ddv_20 += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_brez_ddv_20")).toString()).replace(".", ",") + " EUR";
			}
			znesek_ddv += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek_ddv")).toString()).replace(".", ",") + " EUR";
			znesek += prevedi(sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("znesek")).toString()).replace(".", ",") + " EUR";

			QSqlQuery sql_placnik;
			sql_placnik.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("placnik_podjetje")).toString() + "'");
			sql_placnik.exec();
			if ( sql_placnik.next() ) {
				placnik += prevedi(sql_placnik.value(sql_placnik.record().indexOf("polnoime")).toString());
				placnik_logotip = prevedi(sql_placnik.value(sql_placnik.record().indexOf("logotip")).toString());
			}

			QSqlQuery sql_projekt;
			sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_prejeti_racuni.value(sql_prejeti_racuni.record().indexOf("stevilka_projekta")).toString() + "'");
			sql_projekt.exec();
			if ( sql_projekt.next() ) {
				projekt += prevedi(sql_projekt.value(sql_projekt.record().indexOf("stevilka_projekta")).toString());
			}

		}
	}
	base.close();

	/**
		* Nastavimo podatke o tiskanju. Tu tiskamo v PDF dokument
		* velikost A4, kar po obliki ustreza tiskanju v koncni razlicici, ko
		* se bo tiskalo tako v PDF dokument kot direktno na tiskalnik
		*/

	// ustvariti pot do ustrezne mape
		QString mapa_za_shranjevanje = "";
		mapa_za_shranjevanje = placnik_logotip.left(placnik_logotip.lastIndexOf("/")); // izreze logotip
		mapa_za_shranjevanje = mapa_za_shranjevanje.left(mapa_za_shranjevanje.lastIndexOf("/")); // izreze mapo za logotip
		mapa_za_shranjevanje = QFileDialog::getExistingDirectory(this,
																														 "Izberite mapo za shranjevanje dokumentov",
																														 mapa_za_shranjevanje, QFileDialog::ShowDirsOnly);
		if ( mapa_za_shranjevanje == "" ) {
			return;
		}

		QDir mapa(mapa_za_shranjevanje);
		mapa.mkdir("prejeti-racuni");
		mapa.cd("prejeti-racuni");
		mapa.mkdir(datum_placila.right(4));
		mapa.cd(datum_placila.right(4));

	// podatki o printerju

	QPrinter printer;
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(mapa.path() + "/prejeti-racun-" + datum_placila.right(4) + "-" + stevilka_vnosa.right(11) + ".pdf");

//	QPrintDialog *dialog = new QPrintDialog(&printer, this);
//	dialog->setWindowTitle(tr("Natisni prejet racun"));
	printer.setPaperSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

//	if (dialog->exec() == QDialog::Accepted) {
		QPainter painter;

		if (! painter.begin(&printer))  { // failed to open file
			qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
			return;
		}

		// nastavi pisavo
		painter.setFont(QFont("Arial", 12));

		// pripravimo dokument za tiskanje
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, stevilka_vnosa);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, stevilka_racuna);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, stevilka_racuna);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, izdajatelj_racuna);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, izdajatelj_racuna);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, zadeva);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, zadeva);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, placnik);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, placnik);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, projekt);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, projekt);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, datum_prejema);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, datum_prejema);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, datum_placila);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, datum_placila);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, rok_placila);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, rok_placila);
		pozicija += velikost_besedila.height();
		if ( znesek_brez_ddv_00 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_00);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_00);
			pozicija += velikost_besedila.height();
		}
		if ( znesek_brez_ddv_85 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_85);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_85);
			pozicija += velikost_besedila.height();
		}
		if ( znesek_brez_ddv_20 != "" ) {
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_20);
			painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_brez_ddv_20);
			pozicija += velikost_besedila.height();
		}
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek_ddv);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek_ddv);
		pozicija += velikost_besedila.height();
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, znesek);
		painter.drawText(QRectF(0, pozicija, printer.width(), velikost_besedila.height()), Qt::AlignJustify | Qt::TextWordWrap, znesek);
		pozicija += velikost_besedila.height();

		painter.end();

//	}

}

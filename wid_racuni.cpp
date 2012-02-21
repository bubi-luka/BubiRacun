#include <QtSql>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QDir>
#include <QFileDialog>

#include "wid_racuni.h"
#include "ui_wid_racuni.h"
#include "racun.h"
#include "kodiranje.h"
#include "potninalogi.h"
#include "varnost.h"

wid_racuni::wid_racuni(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_racuni)
{
    ui->setupUi(this);

		// disable and hide
		ui->txt_stprojekta->setEnabled(false);
		ui->txt_stprojekta->setVisible(false);
		ui->btn_brisi->setEnabled(false);
		ui->btn_brisi->setVisible(false);

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

			// filtriraj po tipu racuna
			ui->cb_racun->addItem("");
			ui->cb_racun->addItem("1) Predracun");
			ui->cb_racun->addItem("2) Predplacilo");
			ui->cb_racun->addItem("3) Racun");

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

			sql_napolni.prepare("SELECT * FROM racuni WHERE avtor_oseba LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				if ( min_leto > prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()).right(4).toInt() ) {
					min_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()).right(4).toInt();
				}
				if ( max_leto < prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()).right(4).toInt() ) {
					max_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()).right(4).toInt();
				}
			}

			for ( int i = min_leto; i <= max_leto; i++ ) {
				ui->cb_leto->addItem(QString::number(i, 10));
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

wid_racuni::~wid_racuni()
{
	delete ui;
}

void wid_racuni::on_cb_racun_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_racuni::on_cb_mesec_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_racuni::on_cb_leto_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_racuni::on_cb_placilo_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_racuni::on_cb_racunovodstvo_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_racuni::napolni() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "wid_racuni");
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

		for (int i = 0; i <= 9; i++) {
			ui->tbl_racuni->removeColumn(0);
		}

		QSqlQuery sql_clear("wid_racuni");
		sql_clear.prepare("SELECT * FROM racuni");
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
		ui->tbl_racuni->insertColumn(7);
		ui->tbl_racuni->insertColumn(8);
		ui->tbl_racuni->insertColumn(9);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;
		QTableWidgetItem *naslov6 = new QTableWidgetItem;
		QTableWidgetItem *naslov7 = new QTableWidgetItem;
		QTableWidgetItem *naslov8 = new QTableWidgetItem;
		QTableWidgetItem *naslov9 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Tip racuna");
		naslov2->setText("St. racuna");
		naslov3->setText("Datum izdaje");
		naslov4->setText("Stranka");
		naslov5->setText("Projekt");
		naslov6->setText("Znesek za placilo");
		naslov7->setText("Se placati");
		naslov8->setText("Status placila");
		naslov9->setText("Status racunovodstva");

		ui->tbl_racuni->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_racuni->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_racuni->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_racuni->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_racuni->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_racuni->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_racuni->setHorizontalHeaderItem(6, naslov6);
		ui->tbl_racuni->setHorizontalHeaderItem(7, naslov7);
		ui->tbl_racuni->setHorizontalHeaderItem(8, naslov8);
		ui->tbl_racuni->setHorizontalHeaderItem(9, naslov9);

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QString stavek = "";

		if ( ui->cb_racun->currentText() != "" ) {
			stavek += " AND tip_racuna LIKE '" + pretvori(ui->cb_racun->currentText()).left(1) + "'";
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

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM racuni" + stavek);
		}
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			// filtriramo glede na javni, zasebni status racuna
			if ( vApp->state() != pretvori("private") ) {

				// doloci vse tri cifre
				QString sklic = prevedi(sql_fill.value(sql_fill.record().indexOf("sklic")).toString());
				sklic = sklic.right(sklic.length() - 5); // odbijemo drzavo in model
				sklic = sklic.right(sklic.length() - 5); // odbijemo stevilko racuna
				int cifra_1 = sklic.left(1).toInt();
				sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_1 in dan
				int cifra_2 = sklic.left(1).toInt();
				sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_2 in mesec
				int cifra_3 = sklic.left(1).toInt();

				// iz prvih dveh izracunaj kontrolno stevilko
				int kontrolna = 0;

				int sestevek = 3 * cifra_1 + 2 * cifra_2;

				int ostanek = sestevek % 11;

				kontrolna = 11 - ostanek;

				if ( kontrolna >= 9 ) {
					kontrolna = 0;
				}

				// od cifre_3 odstej kontrolno stevilko
				// tako dobis 0 => racun je javen ali 1 => racun je zaseben
				int razlika = cifra_3 - kontrolna;

				if ( razlika == 1 ) {
					filter = "negativno"; // racuna ne prikazi
				}

			}

			if ( filter == "pozitivno" ) {
				ui->tbl_racuni->insertRow(row);
				ui->tbl_racuni->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[10] = {"id", "tip_racuna", "stevilka_racuna", "datum_izdaje", "stranka", "projekt", "znesek_za_placilo",
														 "se_placati", "status_placila", "status_racunovodstva"};

				while (col <= 9) {
					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[i] == "tip_racuna" ) {
						if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "1" ) {
							celica->setText("Predracun");
						}
						else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "2" ) {
							celica->setText("Predplacilo");
						}
						else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "3" ) {
							celica->setText("Racun");
						}
					}
					else if ( polja[i] == "stranka" ) {
						QSqlQuery sql_find_stranka;
						sql_find_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
						sql_find_stranka.exec();
						if ( sql_find_stranka.next() ) {
							if ( prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("tip")).toString()) == "1" ) {
								celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("priimek")).toString()) + " " +
												prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
							}
							else {
								 celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
							}
						}
						else {
							celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
						}
					}
					else if ( polja[i] == "projekt" ) {
						QSqlQuery sql_kodiraj;
						sql_kodiraj.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("projekt")).toString() + "'");
						sql_kodiraj.exec();
						if ( sql_kodiraj.next() ) {
							celica->setText(prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("naslov_projekta")).toString()));
						}
					}
					else if ( polja[i] == "znesek_za_placilo" ) {
						QSqlQuery sql_kodiraj;
						sql_kodiraj.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() +
																"' AND tip_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString() + "'");
						sql_kodiraj.exec();
						double znesek = 0.0;
						while ( sql_kodiraj.next() ) {
							znesek += prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("znesek_koncni")).toString()).toDouble() * 1.2;
						}
						celica->setText(QString::number(znesek, 'f', 2).replace(".", ",") + " EUR");
					}
					else if ( polja[i] == "se_placati" ) {
						QSqlQuery sql_kodiraj;
						sql_kodiraj.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() +
																"' AND tip_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString() + "'");
						sql_kodiraj.exec();
						double znesek = 0.0;
						while ( sql_kodiraj.next() ) {
							znesek += prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("znesek_koncni")).toString()).toDouble() * 1.2;
						}
						znesek -= prevedi(sql_fill.value(sql_fill.record().indexOf("avans")).toString()).toDouble();
						celica->setText(QString::number(znesek, 'f', 2).replace(".", ",") + " EUR");
					}
					else {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					}
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

void wid_racuni::on_tbl_racuni_doubleClicked() {

	racun *uredi = new racun;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_racuni->selectedItems().takeAt(0)->text()); // ce racun ze obstaja, naprej posljemo id. racuna
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_racuni::on_btn_brisi_clicked() {

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
		sql_brisi.prepare("DELETE FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id) + "'");
		sql_brisi.exec();
		sql_brisi.clear();

		sql_brisi.prepare("DELETE FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_racuni->removeRow(ui->tbl_racuni->selectedItems().takeAt(0)->row());
	osvezi("racuni");

}

void wid_racuni::osvezi(QString beseda) {

	if ( beseda == "racuni" ) {
		napolni();
		prenesi();
	}

}

QString wid_racuni::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_racuni::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void wid_racuni::on_btn_nov_clicked() {

	racun *uredi = new racun;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov racun" + ui->txt_stprojekta->text()); // ce racun ne obstaja, naprej posljemo st. projekta
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_racuni::prejem(QString besedilo) {

	ui->txt_stprojekta->setText(besedilo);

	napolni();

}

void wid_racuni::on_btn_refresh_clicked() {

	napolni();

}

void wid_racuni::on_btn_print_clicked() {

	QModelIndexList selectedList = ui->tbl_racuni->selectionModel()->selectedRows();

	for( int i = 0; i < selectedList.count(); i++) {
		print(ui->tbl_racuni->item(selectedList.at(i).row(), 0)->text());
	}

	/*
	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "wid_racuni");
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

		if ( ui->cb_racun->currentText() != "" ) {
			stavek += " AND tip_racuna LIKE '" + pretvori(ui->cb_racun->currentText()).left(1) + "'";
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

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM racuni" + stavek);
		}
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			// filtriramo glede na javni, zasebni status racuna
			if ( vApp->state() != pretvori("private") ) {

				// doloci vse tri cifre
				QString sklic = prevedi(sql_fill.value(sql_fill.record().indexOf("sklic")).toString());
				sklic = sklic.right(sklic.length() - 5); // odbijemo drzavo in model
				sklic = sklic.right(sklic.length() - 5); // odbijemo stevilko racuna
				int cifra_1 = sklic.left(1).toInt();
				sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_1 in dan
				int cifra_2 = sklic.left(1).toInt();
				sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_2 in mesec
				int cifra_3 = sklic.left(1).toInt();

				// iz prvih dveh izracunaj kontrolno stevilko
				int kontrolna = 0;

				int sestevek = 3 * cifra_1 + 2 * cifra_2;

				int ostanek = sestevek % 11;

				kontrolna = 11 - ostanek;

				if ( kontrolna >= 9 ) {
					kontrolna = 0;
				}

				// od cifre_3 odstej kontrolno stevilko
				// tako dobis 0 => racun je javen ali 1 => racun je zaseben
				int razlika = cifra_3 - kontrolna;

				if ( razlika == 1 ) {
					filter = "negativno"; // racuna ne prikazi
				}

			}

			if ( filter == "pozitivno" ) {
				print(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
			}
		}
	}
	base.close();
*/
}

void wid_racuni::on_btn_print_pdf_clicked() {

	QModelIndexList selectedList = ui->tbl_racuni->selectionModel()->selectedRows();

	for( int i = 0; i < selectedList.count(); i++) {
		printpdf(ui->tbl_racuni->item(selectedList.at(i).row(), 0)->text());
	}

	QMessageBox konec;
	konec.setText("Shranjevanje dokumentov v PDF je koncano!");
	konec.setInformativeText("Shranjenih je " + QString::number(selectedList.count(), 10) + " dokumentov!");
	konec.exec();

	/*
	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "wid_racuni");
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

		if ( ui->cb_racun->currentText() != "" ) {
			stavek += " AND tip_racuna LIKE '" + pretvori(ui->cb_racun->currentText()).left(1) + "'";
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

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM racuni" + stavek);
		}
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			// filtriramo glede na javni, zasebni status racuna
			if ( vApp->state() != pretvori("private") ) {

				// doloci vse tri cifre
				QString sklic = prevedi(sql_fill.value(sql_fill.record().indexOf("sklic")).toString());
				sklic = sklic.right(sklic.length() - 5); // odbijemo drzavo in model
				sklic = sklic.right(sklic.length() - 5); // odbijemo stevilko racuna
				int cifra_1 = sklic.left(1).toInt();
				sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_1 in dan
				int cifra_2 = sklic.left(1).toInt();
				sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_2 in mesec
				int cifra_3 = sklic.left(1).toInt();

				// iz prvih dveh izracunaj kontrolno stevilko
				int kontrolna = 0;

				int sestevek = 3 * cifra_1 + 2 * cifra_2;

				int ostanek = sestevek % 11;

				kontrolna = 11 - ostanek;

				if ( kontrolna >= 9 ) {
					kontrolna = 0;
				}

				// od cifre_3 odstej kontrolno stevilko
				// tako dobis 0 => racun je javen ali 1 => racun je zaseben
				int razlika = cifra_3 - kontrolna;

				if ( razlika == 1 ) {
					filter = "negativno"; // racuna ne prikazi
				}

			}

			if ( filter == "pozitivno" ) {
				printpdf(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
			}
		}
	}
	base.close();
*/
}

QString wid_racuni::pretvori_v_double(QString besedilo) {

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

QString wid_racuni::pretvori_iz_double(QString besedilo) {

	besedilo.replace(".",",");

	return besedilo;

}

void wid_racuni::on_btn_print_seznam_clicked() {

}

void wid_racuni::print(QString id) {

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
	QString racun_datum_placila_avansa = "";
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
	QString skupaj_znesek_avansa_brez_ddv = "";
	QString skupaj_ddv_avansa = "";
	QString skupaj_se_za_placati = "";
	QString skupaj_se_za_placati_ddv = "";

	double skupajbrezddv = 0.0;
	double skupajddvodosnove20 = 0.0;
	double skupajddvodosnove85 = 0.0;
	double skupajddvodosnove00 = 0.0;
	double skupajznesek = 0.0;
	double skupajznesekavansabrezddv = 0.0;
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
			racun_rok_placila = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_placila")).toString());
			racun_datum_placila_avansa = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_placila_avansa")).toString());
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
				if ( prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("davcni_zavezanec")).toString()) == "0" ) { // ni davcni zavezanec
					narocnik_davcna = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString());
				}
				else {
					narocnik_davcna = "SI" + prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString());
				}
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
		 double pozicija_spodaj = pozicija; // postavimo višino, ki ustreza crti pod podatki (pred)racuna

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
		 if ( racun_tip != "2" ) {
			 painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_placila);
		 }
		 else {
			 painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_placila_avansa);
		 }
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

		 if ( pozicija < pozicija_spodaj ) {
			 pozicija = pozicija_spodaj + razmik_med_vrsticami;
		 }

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
				 if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_sklop")).toString()) == "Ostalo" ||
							prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_skupina")).toString()) == "Ostalo") {
					 storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_rocno")).toString());
				 }
				 else if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_storitev")).toString()) == "Ostalo" ) {
					 storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_skupina")).toString()) + ": " +
													prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_rocno")).toString());
				 }
				 else {
					 storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_skupina")).toString()) + ": " +
													prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_storitev")).toString());
				 }

				 if ( storitev_ime.right(2) == ": " ) {
					 storitev_ime = storitev_ime.left(storitev_ime.length() - 2);
				 }

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
		 skupajznesekavansabrezddv = pretvori_v_double(racun_znesek_avansa).toDouble() - skupajddvavansa;
		 skupaj_znesek_avansa_brez_ddv = QString::number(skupajznesekavansabrezddv, 'f', 2).replace(".", ",") + " EUR";
		 skupaj_se_za_placati = QString::number(skupajznesek - pretvori_v_double(racun_znesek_avansa).toDouble(), 'f', 2).replace(".", ",") + " EUR";
		 skupaj_se_za_placati_ddv = QString::number(skupajddvodosnove20 - skupajddvavansa, 'f', 2).replace(".", ",") + " EUR";
		 // vcasih poda negativni predznak pred zneskom 0,00
		 if ( skupaj_se_za_placati == "-0,00 EUR" ) {
			 skupaj_se_za_placati = "0,00 EUR";
		 }
		 if ( skupaj_se_za_placati_ddv == "-0,00 EUR" ) {
			 skupaj_se_za_placati_ddv = "0,00 EUR";
		 }

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
			 painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_znesek_avansa_brez_ddv);
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

void wid_racuni::printpdf(QString id) {

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
	QString racun_datum_placila_avansa = "";
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
	QString skupaj_znesek_avansa_brez_ddv = "";
	QString skupaj_ddv_avansa = "";
	QString skupaj_se_za_placati = "";
	QString skupaj_se_za_placati_ddv = "";

	double skupajbrezddv = 0.0;
	double skupajddvodosnove20 = 0.0;
	double skupajddvodosnove85 = 0.0;
	double skupajddvodosnove00 = 0.0;
	double skupajznesek = 0.0;
	double skupajznesekavansabrezddv = 0.0;
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
			racun_rok_placila = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_placila")).toString());
			racun_datum_placila_avansa = prevedi(sql_racun.value(sql_racun.record().indexOf("datum_placila_avansa")).toString());
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
				if ( prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("davcni_zavezanec")).toString()) == "0" ) { // ni davcni zavezanec
					narocnik_davcna = prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString());
				}
				else {
					narocnik_davcna = "SI" + prevedi(sql_narocnik.value(sql_narocnik.record().indexOf("davcna")).toString());
				}
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

	/**
		* Nastavimo podatke o tiskanju. Tu tiskamo v PDF dokument
		* velikost A4, kar po obliki ustreza tiskanju v koncni razlicici, ko
		* se bo tiskalo tako v PDF dokument kot direktno na tiskalnik
		*/

	// ustvariti pot do ustrezne mape
		QString mapa_za_shranjevanje = "";
		mapa_za_shranjevanje = podjetje_logo.left(podjetje_logo.lastIndexOf("/")); // izreze logotip
		mapa_za_shranjevanje = mapa_za_shranjevanje.left(mapa_za_shranjevanje.lastIndexOf("/")); // izreze mapo za logotip
//		mapa_za_shranjevanje = QFileDialog::getExistingDirectory(this,
//																														 "Izberite mapo za shranjevanje dokumentov",
//																														 mapa_za_shranjevanje, QFileDialog::ShowDirsOnly);
//		if ( mapa_za_shranjevanje == "" ) {
//			return;
//		}

		QDir mapa(mapa_za_shranjevanje);
		mapa.mkdir("izdani-predracuni");
		mapa.mkdir("izdani-predplacilni-racuni");
		mapa.mkdir("izdani-racuni");
		if ( racun_tip == "1" ) {
			mapa.cd("izdani-predracuni");
		}
		else if ( racun_tip == "2" ) {
			mapa.cd("izdani-predplacilni-racuni");
		}
		else if ( racun_tip == "3" ) {
			mapa.cd("izdani-racuni");
		}
		mapa.mkdir(racun_datum_izdaje.right(4));
		mapa.cd(racun_datum_izdaje.right(4));

	// podatki o printerju

	QPrinter printer;
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(mapa.path() + "/izdani-racun-" + racun_tip + "-" + racun_datum_izdaje.right(4) +
														"-" + racun_datum_izdaje.left(5).right(2) + "-" + racun_datum_izdaje.left(2) + "-" + racun_stevilka + ".pdf");
	printer.setPaperSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

//	 if (dialog->exec() == QDialog::Accepted) {
	QPainter painter;

	if (! painter.begin(&printer))  { // failed to open file
		qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
		return;
	}

	int i = 1;
	double pozicija = 0;
	double visina_vrstice = 0;
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
	double pozicija_spodaj = pozicija; // postavimo višino, ki ustreza crti pod podatki (pred)racuna

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
	if ( racun_tip != "2" ) {
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_rok_placila);
	}
	else {
		painter.drawText(QRectF(printer.width() / 2 + velikost_besedila.width(), pozicija, printer.width(), visina_vrstice), Qt::AlignJustify | Qt::TextWordWrap, racun_datum_placila_avansa);
	}
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

	if ( pozicija < pozicija_spodaj ) {
		pozicija = pozicija_spodaj + razmik_med_vrsticami;
	}

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
			if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_sklop")).toString()) == "Ostalo" ||
					 prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_skupina")).toString()) == "Ostalo") {
				storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_rocno")).toString());
			}
			else if ( prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_storitev")).toString()) == "Ostalo" ) {
				storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_skupina")).toString()) + ": " +
											 prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_rocno")).toString());
			}
			else {
				storitev_ime = prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_skupina")).toString()) + ": " +
											 prevedi(sql_storitve.value(sql_storitve.record().indexOf("opravilo_storitev")).toString());
			}

			if ( storitev_ime.right(2) == ": " ) {
				storitev_ime = storitev_ime.left(storitev_ime.length() - 2);
			}

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
	skupajznesekavansabrezddv = pretvori_v_double(racun_znesek_avansa).toDouble() - skupajddvavansa;
	skupaj_znesek_avansa_brez_ddv = QString::number(skupajznesekavansabrezddv, 'f', 2).replace(".", ",") + " EUR";
	skupaj_se_za_placati = QString::number(skupajznesek - pretvori_v_double(racun_znesek_avansa).toDouble(), 'f', 2).replace(".", ",") + " EUR";
	skupaj_se_za_placati_ddv = QString::number(skupajddvodosnove20 - skupajddvavansa, 'f', 2).replace(".", ",") + " EUR";
	// vcasih poda negativni predznak pred zneskom 0,00
	if ( skupaj_se_za_placati == "-0,00 EUR" ) {
		skupaj_se_za_placati = "0,00 EUR";
	}
	if ( skupaj_se_za_placati_ddv == "-0,00 EUR" ) {
		skupaj_se_za_placati_ddv = "0,00 EUR";
	}

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
		painter.drawText(QRectF(printer.width()  * 3 / 5 + velikost_besedila.width() + 10, pozicija, printer.width()  * 2 / 5 - velikost_besedila.width() - 10, visina_vrstice), Qt::AlignCenter | Qt::TextWordWrap | Qt::AlignVCenter, skupaj_znesek_avansa_brez_ddv);
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
//	}

}

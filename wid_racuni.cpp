#include <QtSql>
#include <QMessageBox>

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
/*					QSqlQuery sql_predracun;
					QString id_predracuna;
					sql_predracun.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("stevilka_racuna")).toString() +
																"' AND tip_racuna LIKE '" + pretvori("1") + "'");
					sql_predracun.exec();
					if ( sql_predracun.next() ) {
						id_predracuna = sql_predracun.value(sql_predracun.record().indexOf("id")).toString();
					}
					sql_predracun.clear();
*/
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
						celica->setText(QString::number(znesek, 'f', 2) + " EUR");
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
						celica->setText(QString::number(znesek, 'f', 2) + " EUR");
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
		QString tip_racuna = "";
		QString id_predracuna = "";

		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
		sql_racun.exec();
		if ( sql_racun.next() ) {
			tip_racuna = sql_racun.value(sql_racun.record().indexOf("tip_racuna")).toString();
			id_predracuna = sql_racun.value(sql_racun.record().indexOf("stevilka_racuna")).toString();
		}
		sql_racun.clear();
		sql_racun.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(id_predracuna) + "' AND tip_racuna LIKE '" + pretvori("1") + "'");
		sql_racun.exec();
		if ( sql_racun.next() ) {
			id_predracuna = sql_racun.value(sql_racun.record().indexOf("id")).toString();
		}
		sql_racun.clear();

		QSqlQuery sql_brisi;
		sql_brisi.prepare("DELETE FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id_predracuna) + "' AND tip_racuna LIKE '" + tip_racuna + "'");
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

void wid_racuni::on_btn_osvezi_clicked() {

	napolni();

}

void wid_racuni::prejem(QString besedilo) {

	ui->txt_stprojekta->setText(besedilo);

	napolni();

}

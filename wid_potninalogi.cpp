#include <QtSql>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QFileDialog>

#include "wid_potninalogi.h"
#include "ui_wid_potninalogi.h"
#include "potninalogi.h"
#include "kodiranje.h"
#include "varnost.h"

wid_potninalogi::wid_potninalogi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_potninalogi)
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

			sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE predlagatelj_oseba LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				if ( min_leto > prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_naloga")).toString()).right(4).toInt() ) {
					min_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_naloga")).toString()).right(4).toInt();
				}
				if ( max_leto < prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_naloga")).toString()).right(4).toInt() ) {
					max_leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_naloga")).toString()).right(4).toInt();
				}
			}

			for ( int i = min_leto; i <= max_leto; i++ ) {
				ui->cb_leto->addItem(QString::number(i, 10));
			}
			sql_napolni.clear();

			// filtriraj po namenu
			ui->cb_namen->addItem("");
			sql_napolni.prepare("SELECT * FROM sif_namen_potnega_naloga");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_namen->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()) + ") " +
															prevedi(sql_napolni.value(sql_napolni.record().indexOf("namen")).toString()));
			}
			sql_napolni.clear();

			// filtriraj po stranki
			ui->cb_stranka->addItem("");
			sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE predlagatelj_oseba LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_stranka->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv_ciljnega_podjetja")).toString()));
			}
			sql_napolni.clear();

			// filtriraj po kraju prihoda
			ui->cb_kraj->addItem("");
			sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE predlagatelj_oseba LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				QSqlQuery sql_kraj;
				sql_kraj.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("stevilka_naloga")).toString() + "'");
				sql_kraj.exec();
				while ( sql_kraj.next() ) {
					if ( ui->cb_kraj->findText(prevedi(sql_kraj.value(sql_kraj.record().indexOf("kraj_prihoda")).toString())) == -1 ) {
						ui->cb_kraj->addItem(prevedi(sql_kraj.value(sql_kraj.record().indexOf("kraj_prihoda")).toString()));
					}
				}
			}
			sql_napolni.clear();

			// filtriraj po prevoznem sredstvu
			ui->cb_prevoz->addItem("");
			sql_napolni.prepare("SELECT * FROM sif_prevozna_sredstva");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_prevoz->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()) + ") " +
															 prevedi(sql_napolni.value(sql_napolni.record().indexOf("prevoz")).toString()));
			}
			sql_napolni.clear();

		}
		base.close();

		napolni();

		ui->btn_nov->setText(gumb);

}

wid_potninalogi::~wid_potninalogi()
{
    delete ui;
}

void wid_potninalogi::on_btn_refresh_clicked() {

	napolni();

}

void wid_potninalogi::on_btn_print_clicked() {

	QModelIndexList selectedList = ui->tbl_potninalogi->selectionModel()->selectedRows();

	for( int i = 0; i < selectedList.count(); i++) {
		print(ui->tbl_potninalogi->item(selectedList.at(i).row(), 0)->text());
	}

/*
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

		if ( ui->cb_namen->currentText() != "" ) {
			stavek += " AND namen_naloga LIKE '" + pretvori(ui->cb_namen->currentText()).left(ui->cb_namen->currentText().indexOf(") ",0)) + "'";
		}

		if ( ui->cb_stranka->currentText() != "" ) {
			stavek += " AND naziv_ciljnega_podjetja LIKE '" + pretvori(ui->cb_stranka->currentText()) + "'";
		}

		if ( ui->cb_prevoz->currentText() != "" ) {
			stavek += " AND prevozno_sredstvo LIKE '" + pretvori(ui->cb_prevoz->currentText()).left(ui->cb_prevoz->currentText().indexOf(") ",0)) + "'";
		}

		if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
			stavek = " WHERE" + stavek.right(stavek.length() - 4);
		}

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM potni_nalogi WHERE stevilka_projekta LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM potni_nalogi" + stavek);
		}
		sql_fill.exec();

		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}

			if ( filter == "pozitivno" ) { // natisni
				print(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
			}
		}
	}
	base.close();
*/
}

void wid_potninalogi::on_btn_print_pdf_clicked() {

	QModelIndexList selectedList = ui->tbl_potninalogi->selectionModel()->selectedRows();

	for( int i = 0; i < selectedList.count(); i++) {
		printpdf(ui->tbl_potninalogi->item(selectedList.at(i).row(), 0)->text());
	}

	QMessageBox konec;
	konec.setText("Shranjevanje dokumentov v PDF je koncano!");
	konec.setInformativeText("Shranjenih je " + QString::number(selectedList.count(), 10) + " dokumentov!");
	konec.exec();

/*
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

		if ( ui->cb_namen->currentText() != "" ) {
			stavek += " AND namen_naloga LIKE '" + pretvori(ui->cb_namen->currentText()).left(ui->cb_namen->currentText().indexOf(") ",0)) + "'";
		}

		if ( ui->cb_stranka->currentText() != "" ) {
			stavek += " AND naziv_ciljnega_podjetja LIKE '" + pretvori(ui->cb_stranka->currentText()) + "'";
		}

		if ( ui->cb_prevoz->currentText() != "" ) {
			stavek += " AND prevozno_sredstvo LIKE '" + pretvori(ui->cb_prevoz->currentText()).left(ui->cb_prevoz->currentText().indexOf(") ",0)) + "'";
		}

		if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
			stavek = " WHERE" + stavek.right(stavek.length() - 4);
		}

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM potni_nalogi WHERE stevilka_projekta LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM potni_nalogi" + stavek);
		}
		sql_fill.exec();

		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}

			if ( filter == "pozitivno" ) { // natisni
				printpdf(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
			}
		}
	}
	base.close();
*/
}

void wid_potninalogi::on_cb_mesec_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_potninalogi::on_cb_leto_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_potninalogi::on_cb_namen_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_potninalogi::on_cb_stranka_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_potninalogi::on_cb_kraj_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_potninalogi::on_cb_prevoz_currentIndexChanged() {

	if ( ui->btn_nov->text() != "" ) {
		napolni();
	}

}

void wid_potninalogi::napolni() {

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
		ui->tbl_potninalogi->clear();

		for (int i = 0; i <= 11; i++) {
			ui->tbl_potninalogi->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM potni_nalogi");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_potninalogi->removeRow(0);
		}

		// start filling the table
		ui->tbl_potninalogi->insertColumn(0);
		ui->tbl_potninalogi->insertColumn(1);
		ui->tbl_potninalogi->insertColumn(2);
		ui->tbl_potninalogi->insertColumn(3);
		ui->tbl_potninalogi->insertColumn(4);
		ui->tbl_potninalogi->insertColumn(5);
		ui->tbl_potninalogi->insertColumn(6);
		ui->tbl_potninalogi->insertColumn(7);
		ui->tbl_potninalogi->insertColumn(8);
		ui->tbl_potninalogi->insertColumn(9);
		ui->tbl_potninalogi->insertColumn(10);
		ui->tbl_potninalogi->insertColumn(11);

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
		QTableWidgetItem *naslov10 = new QTableWidgetItem;
		QTableWidgetItem *naslov11 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Prejemnik");
		naslov2->setText("St.  naloga");
		naslov3->setText("Datum izdaje");
		naslov4->setText("Namen");
		naslov5->setText("Naziv ciljnega podjetja");
		naslov6->setText("Kraj prihoda");
		naslov7->setText("Prevozno sredstvo");
		naslov8->setText("St. kilometrov");
		naslov9->setText("St. dnevnic");
		naslov10->setText("Ostali stroski");
		naslov11->setText("Skupaj stroski");

		ui->tbl_potninalogi->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_potninalogi->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_potninalogi->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_potninalogi->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_potninalogi->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_potninalogi->setHorizontalHeaderItem(5, naslov5);
		ui->tbl_potninalogi->setHorizontalHeaderItem(6, naslov6);
		ui->tbl_potninalogi->setHorizontalHeaderItem(7, naslov7);
		ui->tbl_potninalogi->setHorizontalHeaderItem(8, naslov8);
		ui->tbl_potninalogi->setHorizontalHeaderItem(9, naslov9);
		ui->tbl_potninalogi->setHorizontalHeaderItem(10, naslov10);
		ui->tbl_potninalogi->setHorizontalHeaderItem(11, naslov11);

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QString stavek = "";

		if ( ui->cb_namen->currentText() != "" ) {
			stavek += " AND namen_naloga LIKE '" + pretvori(ui->cb_namen->currentText()).left(ui->cb_namen->currentText().indexOf(") ",0)) + "'";
		}

		if ( ui->cb_stranka->currentText() != "" ) {
			stavek += " AND naziv_ciljnega_podjetja LIKE '" + pretvori(ui->cb_stranka->currentText()) + "'";
		}

		if ( ui->cb_prevoz->currentText() != "" ) {
			stavek += " AND prevozno_sredstvo LIKE '" + pretvori(ui->cb_prevoz->currentText()).left(ui->cb_prevoz->currentText().indexOf(") ",0)) + "'";
		}

		if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
			stavek = " WHERE" + stavek.right(stavek.length() - 4);
		}

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM potni_nalogi WHERE stevilka_projekta LIKE '" + projekt + "'" + stavek);
		}
		else {
			sql_fill.prepare("SELECT * FROM potni_nalogi" + stavek);
		}
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";
			if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).right(4);
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_mesec->currentText() != "" ) {
				QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).left(5).right(2);
				if ( mesec != ui->cb_mesec->currentText().left(2) ) {
					filter = "negativno";
				}
			}
			else if ( ui->cb_leto->currentText() != "" ) {
				QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_naloga")).toString()).right(4);
				if ( leto != ui->cb_leto->currentText() ) {
					filter = "negativno";
				}
			}

			if ( filter == "pozitivno" ) {
				ui->tbl_potninalogi->insertRow(row);
				ui->tbl_potninalogi->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[12] = {"id", "prejemnik_oseba", "stevilka_naloga", "datum_naloga", "namen_naloga", "naziv_ciljnega_podjetja",
														 "kraj_prihoda", "prevozno_sredstvo", "skupaj_kilometri", "stevilo_dnevnic", "ostali_stroski",
														 "stroski_skupaj"};

				while (col <= 11) {

					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[i] == "prejemnik_oseba" ) {
						QSqlQuery sql_besedilo;
						sql_besedilo.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
						sql_besedilo.exec();
						if ( sql_besedilo.next() ) {
							celica->setText(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("priimek")).toString()) + " " +
															prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("ime")).toString()));
						}
					}
					else if ( polja[i] == "namen_naloga" ) {
						QSqlQuery sql_besedilo;
						sql_besedilo.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
						sql_besedilo.exec();
						if ( sql_besedilo.next() ) {
							celica->setText(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("namen")).toString()));
						}
					}
	//				else if ( polja[i] == "kraj_prihoda" ) {
	//					QSqlQuery sql_besedilo;
	//					sql_besedilo.prepare("SELECT * FROM sif_namen_naloga WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
	//					sql_besedilo.exec();
	//					if ( sql_besedilo.next() ) {
	//						celica->setText(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("namen")).toString()));
	//					}
	//				}
					else if ( polja[i] == "prevozno_sredstvo" ) {
						QSqlQuery sql_besedilo;
						sql_besedilo.prepare("SELECT * FROM sif_prevozna_sredstva WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
						sql_besedilo.exec();
						if ( sql_besedilo.next() ) {
							celica->setText(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("prevoz")).toString()));
						}
					}
					else if ( polja[i] == "stevilo_dnevnic" ) {
						double dnevnica = 0.0;
						dnevnica += prevedi(sql_fill.value(sql_fill.record().indexOf("dnevnica_6_8")).toString()).toDouble();
						dnevnica += prevedi(sql_fill.value(sql_fill.record().indexOf("dnevnica_8_12")).toString()).toDouble();
						dnevnica += prevedi(sql_fill.value(sql_fill.record().indexOf("dnevnica_12_24")).toString()).toDouble();
						celica->setText(QString::number(dnevnica, 'f', 0));
					}
					else if ( polja[i] == "ostali_stroski" ) {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ",") + " EUR");
					}
					else if ( polja[i] == "stroski_skupaj" ) {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ",") + " EUR");
					}
					else if ( polja[i] == "skupaj_kilometri" ) {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).replace(".", ","));
					}
					else {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					}
					ui->tbl_potninalogi->setItem(row, col, celica);

					col++;
					i++;

				}

				row++;

			}
		}
	}
	base.close();

}

void wid_potninalogi::on_tbl_potninalogi_doubleClicked() {

	potninalogi *uredi = new potninalogi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos(ui->tbl_potninalogi->selectedItems().takeAt(0)->text());
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_potninalogi::on_btn_brisi_clicked() {

	QString id = ui->tbl_potninalogi->selectedItems().takeAt(0)->text();
	QString stnaloga = ui->tbl_potninalogi->selectedItems().takeAt(2)->text();

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
		sql_brisi.prepare("DELETE FROM potovanja WHERE potni_nalog LIKE '" + pretvori(stnaloga) + "'");
		sql_brisi.exec();
		sql_brisi.clear();

		sql_brisi.prepare("DELETE FROM stroski WHERE potninalog LIKE '" + pretvori(stnaloga) + "'");
		sql_brisi.exec();
		sql_brisi.clear();

		sql_brisi.prepare("DELETE FROM potni_nalogi WHERE id LIKE '" + id + "'");
		sql_brisi.exec();
	}
	base.close();

	ui->tbl_potninalogi->removeRow(ui->tbl_potninalogi->selectedItems().takeAt(0)->row());
	osvezi("potninalogi");

}

void wid_potninalogi::on_btn_nov_clicked() {

	potninalogi *uredi = new potninalogi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("Nov nalog");
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

void wid_potninalogi::osvezi(QString beseda) {

	if ( beseda == "potninalog" ) {
		napolni();
		prenesi();
	}

}

QString wid_potninalogi::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_potninalogi::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString wid_potninalogi::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	return besedilo;

}

QString wid_potninalogi::pretvori_iz_double(QString besedilo) {

	/*
	* pretvarja stevilke v valuto, primerno za obdelavo naprej
	* ni nujno, da je vhodna stevilka resnicno double, lahko gre za drugacno obliko
	*/

	besedilo.replace(".", ","); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9,]")); // odstrani vse znake razen stevilk in decimalne vejice

	while ( besedilo.left(1) == "0" ) { // odstranimo vse vodilne nicle
		besedilo.remove(0,1);
	}
	if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0,00
		besedilo.append("0");
	}
	if ( besedilo.left(1) == "," ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
		besedilo.prepend("0");
	}
	if ( besedilo.right(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 00
		besedilo.append("00");
	}
	if ( besedilo.right(2).left(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 00
		besedilo.append("0");
	}
	if ( !besedilo.contains(",") ) { // ce je celo stevilo dodamo decimalno locilo in vrednost 00
		besedilo.append(",00");
	}

	besedilo.append(" EUR"); // doda oznako za evre

	return besedilo;

}

void wid_potninalogi::prejem(QString besedilo) {

	ui->txt_stprojekta->setText(besedilo);

	napolni();

}

void wid_potninalogi::print(QString id) {

	/**
		* id nosi id stevilko potnega naloga, s cimer lahko hkrati natisnemo
		* vec dokumentov, bodisi istih, bodisi razlicnih
		**/

	/**
		* Nastavimo spremenljivke, ki jih bomo uporabili v dokumentu
		**/
	// podatki o potnem nalogu
	QString stevilka_naloga = "";
	QString datum_naloga = "";
	QString namen_potnega_naloga = "";
	QString prevozno_sredstvo = "";
	QString cena_dnevnice_6_8 = "0.00";
	QString cena_dnevnice_8_12 = "0.00";
	QString cena_dnevnice_12_24 = "0.00";
	QString stevilo_dnevnic_6_8 = "0";
	QString stevilo_dnevnic_8_12 = "0";
	QString stevilo_dnevnic_12_24 = "0";
	QString stroski_skupaj = "";
	QString razdalja = "0";
	QString kilometrina = "";
	QString ostali_stroski = "";
	QString znesek_drugih_stroskov = "";
	QString zvisanje_dnevnic = "0";
	QString priloge = "";
	QString stevilo_dnevnic_1 = "0";
	QString stevilo_dnevnic_2 = "0";
	QString cena_dnevnic_1 = "0.00";
	QString cena_dnevnic_2 = "0.00";
	QString cena_dnevnic = "0.00";

	// podatki o predlagatelju - podjetje
	QString predlagatelj_podjetje_ime;
	QString predlagatelj_podjetje_polno_ime;
	QString predlagatelj_podjetje_naslov;
	QString predlagatelj_podjetje_naslov_st;
	QString predlagatelj_podjetje_postna_st;
	QString predlagatelj_podjetje_posta;

	// podatki o predlagatelju - oseba
	QString predlagatelj_oseba_ime;
	QString predlagatelj_oseba_priimek;
	QString predlagatelj_oseba_naslov;
	QString predlagatelj_oseba_naslov_stevilka;
	QString predlagatelj_oseba_postna_stevilka;
	QString predlagatelj_oseba_posta;

	// podatki o prejemniku
	QString prejemnik_ime;
	QString prejemnik_priimek;
	QString prejemnik_naziv;
	QString prejemnik_naslov;
	QString prejemnik_naslov_stevilka;
	QString prejemnik_postna_stevilka;
	QString prejemnik_posta;
	QString prejemnik_znamka_avtomobila;
	QString prejemnik_model_avtomobila;
	QString prejemnik_registrska_stevilka;

	// podatki o potovanju
	QString relacija;
	QString datum_odhoda;
	QString ura_odhoda;
	QString potovanje;
	QString datum_prihoda;
	QString ura_prihoda;
	QString dnevi_potovanja;
	QDateTime dt_trajanje;
	QDateTime dt_datum_odhoda;
	QDateTime dt_datum_prihoda;
	int minute;
	int ure;

	// odpri podatke o potnem nalogu, prejemniku, stroskih...
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
			QSqlQuery sql_potni_nalog;
			sql_potni_nalog.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + pretvori(id) + "'");
			sql_potni_nalog.exec();
			if ( sql_potni_nalog.next() ) {
				stevilka_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString());
				datum_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("datum_naloga")).toString());
				namen_potnega_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("namen_naloga")).toString());
//				cena_dnevnice = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnic")).toString());

				prevozno_sredstvo = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prevozno_sredstvo")).toString());

				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("priznana_dnevnica")).toString()) != "0" ) {
					cena_dnevnice_6_8 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_6_8")).toString());
					cena_dnevnice_8_12 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_8_12")).toString());
					cena_dnevnice_12_24 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_12_24")).toString());

					stevilo_dnevnic_6_8 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_6_8")).toString());
					stevilo_dnevnic_8_12 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_8_12")).toString());
					stevilo_dnevnic_12_24 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_12_24")).toString());
				}

				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("zajtrk_8_12")).toString()) != "0" ) {
					double cenadnevnice = 0.0;
					cenadnevnice = cena_dnevnice_8_12.toDouble() * 0.85;
					cena_dnevnice_8_12 = QString::number(cenadnevnice, 'f', 2);
				}
				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("zajtrk_12_24")).toString()) != "0" ) {
					double cenadnevnice = 0.0;
					cenadnevnice = cena_dnevnice_12_24.toDouble() * 0.90;
					cena_dnevnice_12_24 = QString::number(cenadnevnice, 'f', 2);
				}

				stroski_skupaj = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stroski_skupaj")).toString());
				razdalja = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("skupaj_kilometri")).toString()).replace(".", ",");
				kilometrina = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("kilometrina")).toString()).replace(".", ",");
				priloge =  prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("priloge")).toString()).replace("\n", ", ");

				// uredi dnevnice
				/**
					* ce obataja dnevnica/dnevnice za vec kot 12 ur, jih dodaj kot
					* dnevnica_2, ki jo bomo prikazali v spodnji vrstici
					* poglej, ce obstaja poleg dnevnice_2 se dnevnica_1, ki je lahko
					* polovicna ali tretjinska, ne moreta biti pa obe hkrati
					* dodaj jo kot dnevnica_1
					* ce ne obstaja celodnevna, potem kot dnevnica_2 dodaj ali polovicno
					* ali
					**/
				if ( stevilo_dnevnic_12_24 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_12_24;
					cena_dnevnic_2 = cena_dnevnice_12_24;
					if ( stevilo_dnevnic_6_8 != "0" ) {
						stevilo_dnevnic_1 = stevilo_dnevnic_6_8;
						cena_dnevnic_1 = cena_dnevnice_6_8;
					}
					else if ( stevilo_dnevnic_8_12 != "0" ) {
						stevilo_dnevnic_1 = stevilo_dnevnic_8_12;
						cena_dnevnic_1 = cena_dnevnice_8_12;
					}
				}
				else if ( stevilo_dnevnic_6_8 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_6_8;
					cena_dnevnic_2 = cena_dnevnice_6_8;
				}
				else if ( stevilo_dnevnic_8_12 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_8_12;
					cena_dnevnic_2 = cena_dnevnice_8_12;
				}

				// podatki o drugih stroskih
				QSqlQuery sql_stroski;
				sql_stroski.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(stevilka_naloga) + "'");
				sql_stroski.exec();
				double stroski = 0.0;
				while ( sql_stroski.next() ) {
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("strosek")).toString()) + ",";
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()) + ";";
					stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()).toDouble();
				}
				znesek_drugih_stroskov = QString::number(stroski, 'f', 2);

				// podatki o predlagatelju - podjetje
				QSqlQuery sql_predlagatelj_podjetje;
				sql_predlagatelj_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje")).toString() + "'");
				sql_predlagatelj_podjetje.exec();
				if ( sql_predlagatelj_podjetje.next() ) {
					predlagatelj_podjetje_ime = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("ime")).toString());
					predlagatelj_podjetje_polno_ime = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("polnoime")).toString());
					predlagatelj_podjetje_naslov = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("naslov")).toString());
					predlagatelj_podjetje_naslov_st = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("naslov_st")).toString());
					predlagatelj_podjetje_postna_st = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("postna_stevilka")).toString());
					predlagatelj_podjetje_posta = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("posta")).toString());
				}

				// podatki o predlagatelju - oseba
				QSqlQuery sql_predlagatelj_oseba;
				sql_predlagatelj_oseba.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba")).toString() + "'");
				sql_predlagatelj_oseba.exec();
				if ( sql_predlagatelj_oseba.next() ) {
					predlagatelj_oseba_ime = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("ime")).toString());
					predlagatelj_oseba_priimek = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("priimek")).toString());
					predlagatelj_oseba_naslov = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("naslov")).toString());
					predlagatelj_oseba_naslov_stevilka = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("naslov_stevilka")).toString());
					predlagatelj_oseba_postna_stevilka = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("postna_stevilka")).toString());
					predlagatelj_oseba_posta = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("posta")).toString());
				}

				// podatki o prejemniku
				QSqlQuery sql_prejemnik;
				sql_prejemnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba")).toString() + "'");
				sql_prejemnik.exec();
				if ( sql_prejemnik.next() ) {
					prejemnik_ime = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("ime")).toString());
					prejemnik_priimek = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("priimek")).toString());
					prejemnik_naziv = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naziv")).toString());
					prejemnik_naslov = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov")).toString());
					prejemnik_naslov_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov_stevilka")).toString());
					prejemnik_postna_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("postna_stevilka")).toString());
					prejemnik_posta = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("posta")).toString());
					prejemnik_znamka_avtomobila = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("avtomobil")).toString());
					prejemnik_model_avtomobila = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("model_avtomobila")).toString());
					prejemnik_registrska_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("registracija")).toString());
				}
				QSqlQuery sql_prejemnik_naziv;
				sql_prejemnik_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + pretvori(prejemnik_naziv) + "'");
				sql_prejemnik_naziv.exec();
				if ( sql_prejemnik_naziv.next() ) {
					prejemnik_naziv = prevedi(sql_prejemnik_naziv.value(sql_prejemnik_naziv.record().indexOf("naziv")).toString());
				}

				QSqlQuery sql_naloga;
				sql_naloga.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + pretvori(namen_potnega_naloga) + "'");
				sql_naloga.exec();
				if ( sql_naloga.next() ) {
					namen_potnega_naloga = prevedi(sql_naloga.value(sql_naloga.record().indexOf("namen")).toString());
				}

				QSqlQuery sql_prevoz;
				sql_prevoz.prepare("SELECT * FROM sif_prevozna_sredstva WHERE id LIKE '" + pretvori(prevozno_sredstvo) + "'");
				sql_prevoz.exec();
				if ( sql_prevoz.next() ) {
					prevozno_sredstvo = prevedi(sql_prevoz.value(sql_prevoz.record().indexOf("prevoz")).toString());
				}

				// podatki o potovanju
				QSqlQuery sql_pot;
				sql_pot.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString() + "'");
				sql_pot.exec();
				QString rel_start = "";
				QString rel_cilj = "";
				dt_datum_odhoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
				dt_datum_prihoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");

				while ( sql_pot.next() ) {

					// racunanje relacije potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString()) != rel_cilj ) {
						rel_start = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString());
					}
					rel_cilj = ""; // izpraznimo cilj, da se ne podvaja

					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString()) != rel_start ) {
						rel_cilj = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString());
					}
					if ( rel_start != "" && relacija != "" ) {
						rel_start = " - " + rel_start;
					}
					if ( rel_cilj != "" ) {
						rel_cilj = " - " + rel_cilj;
					}
					if ( relacija != "" ) {
						relacija = relacija + rel_cilj;
					}
					else {
						relacija = rel_start + rel_cilj;
					}
					rel_start = ""; // izpraznimo start, da se ne podvaja

					// racunanje koncnih tock potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()) != "" ) {
						potovanje = prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()).replace("\n", ", ") + "\n";
					}
			// uporabi samo, ce se bo pokazala potreba po odstranitvi zadnje vrstice
			// trenutno odstranjeno, saj ce zanji kraj nima vnesenega naslova, vseeno reze znake
			//		potovanje = potovanje.remove(potovanje.length() - 2, 2); // zadnji kraj ima tudi prelom v novo vrstico, ki pa tu ni potreben

					// racunanje podatkov o odhodu
					if ( dt_datum_odhoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_odhoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) < 0 ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_odhoda = dt_datum_odhoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					dt_trajanje = dt_datum_odhoda;
					ura_odhoda = datum_odhoda.right(5);
					datum_odhoda = datum_odhoda.left(datum_odhoda.length() - 6);

					if ( dt_datum_prihoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_prihoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) > 0 ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_prihoda = dt_datum_prihoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					ura_prihoda = datum_prihoda.right(5);
					datum_prihoda = datum_prihoda.left(datum_prihoda.length() - 6);
				}
				dnevi_potovanja = QString::number(dt_trajanje.daysTo(dt_datum_prihoda), 10);
				minute = (dt_trajanje.secsTo(dt_datum_prihoda) - dt_trajanje.daysTo(dt_datum_prihoda) * 24 * 60 * 60) / 60;
				ure = minute / 60;
				minute = minute - ure * 60;
			}
		}
		base.close();

	/**
		* Odpremo sifrant z besedilom potnega naloga
		* Besedilo bomo vztavljali z readLine
		**/

		QFile datoteka(app_path + "/potni-nalog-besedilo.csv");
		if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return;
		}
		QTextStream potni_nalog(&datoteka);

	/**
		* Nastavimo podatke o tiskanju. Tu tiskamo v PDF dokument
		* velikost A4, kar po obliki ustreza tiskanju v koncni razlicici, ko
		* se bo tiskalo tako v PDF dokument kot direktno na tiskalnik
		*/

	// podatki o printerju

	QPrinter printer;
	printer.setPaperSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Print Document"));

	if (dialog->exec() == QDialog::Accepted) {
		QPainter painter;

		if (! painter.begin(&printer))  { // failed to open file
			qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
			return;
		}


		double pozicija = 0;
		double visina_vrstice = 0;
		double sirina_besedila = 0;
		double razmik_med_vrsticami = 0;
		double faktor_razmika_med_vrsticami_1 = 0.3; // pri tabelah
		double faktor_razmika_med_vrsticami_2 = 0.0; // pri besedilu

		QString besedilo = "";

		QFont vstavljeno_besedilo("Arial", 10);
		QFont stalno_besedilo("Arial", 10, QFont::Bold);

	// narisemo glavo
		QPen *svincnik = new QPen;

		// dolocimo visino vrstice
		besedilo = "To je testno besedilo";
		painter.setFont(stalno_besedilo);
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsicami, za lazje branje dokumenta

		// crta zgoraj
		svincnik->setWidth(0.5);
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, 0, printer.width(), 0));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, printer.width() / 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_polno_ime;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, printer.width() / 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo odmik od crte
		pozicija = razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami + printer.width() / 2, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta pod naslovom naloga
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() / 2, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami + printer.width() / 2;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = stevilka_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dodamo okvir besedila
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, 0, 0, pozicija)); // crta levo
		painter.drawLine(QLine(printer.width() / 2, 0, printer.width() / 2, pozicija)); // crta sredina
		painter.drawLine(QLine(printer.width(), 0, printer.width(), pozicija)); // crta desno
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija)); // crta spodaj

	// podatki o prejemniku potnega naloga
		// nastavimo nov razmik med vrsticami
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_2;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_ime + " " + prejemnik_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naziv;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naslov + " " + prejemnik_naslov_stevilka + ", " + prejemnik_postna_stevilka + " " + prejemnik_posta;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potovanje;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = namen_potnega_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = "(" + prevozno_sredstvo + "): " + prejemnik_znamka_avtomobila + " " + prejemnik_model_avtomobila;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_polno_ime;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
//		besedilo = cena_dnevnice.replace(".", ",") + " EUR";
		besedilo = cena_dnevnice_12_24.replace(".", ",") + " EUR";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + 15;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// manjka besedilo posebnih dodatkov, dokler ne izvemo, kaj to je...
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

	// tabela predujma
		int prvotna_visina = pozicija; // ohranimo zacetno visino za prvo in drugo tretjino
		int visina_sklopa = 0; // najprej narisemo desno tretjino, ki nosi najvec podatkov in je tudi najvisja

		// prva vrstica je prazna
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() * 2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis odredbodajalca)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dve prazni vrstici
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() * 2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis prejemnika)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// dolocimo koncno visino
		visina_sklopa = pozicija;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo prvo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

	// glava obracuna potnih stroskov
		pozicija = visina_sklopa;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_1;
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() - razmik_med_vrsticami, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta sredina
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_ime + " " + predlagatelj_oseba_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_naslov + " " + predlagatelj_oseba_naslov_stevilka + ", " + predlagatelj_oseba_posta + " " + predlagatelj_oseba_postna_stevilka;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta sredina
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// crti levo in desno
		painter.drawLine(QLine(0, visina_sklopa, 0, pozicija)); // levo
		painter.drawLine(QLine(printer.width(), visina_sklopa, printer.width(), pozicija)); // desno

		// nastavimo odmik od crte
		pozicija += visina_vrstice + razmik_med_vrsticami;

	// obracun potnih stroskov
		// nastavitev zacetne pozicije
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_1;
		prvotna_visina = pozicija;
		double polje_1 = printer.width() * 7 / 8;
		double polje_2 = printer.width() / 8;
		double polje_3 = polje_2 / 2; // dnevi ne potrebujejo toliko prostora, ure/minute pa več

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odsotnost)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(3 * polje_2 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		if ( stevilo_dnevnic_1 == "0" ) { // ce ni dnevnic ali je samo ena, potem so napisi prek dveh vrstic, v nasprotnem primeru samo prek ene
			// nastavimo besedilo (dnevnice)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo novo pozicijo besedila
			pozicija += visina_vrstice + razmik_med_vrsticami;

			// crta zgoraj
			painter.setPen(*svincnik);
			painter.drawLine(QLine(0, pozicija, polje_2 * 5, pozicija));
		}
		else  { // obstajata dva razlicna tipa dnevnic, zato se naslov razpotega zgolj prek ene same vrstice
			// nastavimo besedilo (dnevnice)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo novo pozicijo besedila
			pozicija += visina_vrstice + razmik_med_vrsticami;

			// crta zgoraj
			painter.setPen(*svincnik);
			painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));
		}

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum odhoda)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila - polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila - polje_3, pozicija, 5 * polje_2 - sirina_besedila + polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		if ( stevilo_dnevnic_1 != "0" ) { // obstajata dva razlicna tipa dnevnic
			// nastavimo besedilo (dnevnice_1)
			besedilo = stevilo_dnevnic_1.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic_1)
			besedilo = cena_dnevnic_1.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = QString::number(pretvori_v_double(cena_dnevnic_1).toDouble() * pretvori_v_double(stevilo_dnevnic_1).toDouble(), 'f', 2).replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		}
		// razmik med crtami
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum prihoda)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila - polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = QString::number(ure, 10) + " ur " + QString::number(minute, 10) + " minut";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila - polje_3, pozicija, 5 * polje_2 - sirina_besedila + polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dnevnice_2)
		besedilo = stevilo_dnevnic_2.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (cena dnevnic_2)
		besedilo = cena_dnevnic_2.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (skupaj)
		besedilo = QString::number(pretvori_v_double(cena_dnevnic_2).toDouble() * pretvori_v_double(stevilo_dnevnic_2).toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// razmik med crtami
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// pokoncne crte
		painter.drawLine(QLine(3 * polje_2, prvotna_visina, 3 * polje_2, pozicija));
		painter.drawLine(QLine(5 * polje_2, prvotna_visina, 5 * polje_2, pozicija));
		painter.drawLine(QLine(6 * polje_2, prvotna_visina, 6 * polje_2, pozicija));

		prvotna_visina += visina_vrstice + razmik_med_vrsticami * 2;

		painter.drawLine(QLine(1 * polje_2, prvotna_visina, 1 * polje_2, pozicija));
		painter.drawLine(QLine(2 * polje_2, prvotna_visina, 2 * polje_2, pozicija));
		painter.drawLine(QLine(4 * polje_2 - polje_3, prvotna_visina, 4 * polje_2 - polje_3, pozicija));

		// vrnitev prvotne pozicije
		prvotna_visina = prvotna_visina - visina_vrstice - razmik_med_vrsticami * 2 + 1;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (% zvisanja dnevnic)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = zvisanje_dnevnic;
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		cena_dnevnic = QString::number(pretvori_v_double(cena_dnevnice_6_8).toDouble() * pretvori_v_double(stevilo_dnevnic_6_8).toDouble() +
																	 pretvori_v_double(cena_dnevnice_8_12).toDouble() * pretvori_v_double(stevilo_dnevnic_8_12).toDouble() +
																	 pretvori_v_double(cena_dnevnice_12_24).toDouble() * pretvori_v_double(stevilo_dnevnic_12_24).toDouble(), 'f', 2).replace(".", ",");
		besedilo = cena_dnevnic;
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (prevozni stroski)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 * 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo (št. km)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = razdalja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (km x )
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = kilometrina;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (EUR)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo (relacija)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = relacija;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo (registracija)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_registrska_stevilka;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(pretvori_v_double(kilometrina).toDouble() * pretvori_v_double(razdalja).toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (drugi stroski)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, polje_2 * 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// za vsak strosek dodamo novo vrstico
		if ( ostali_stroski != "" ) {
			do {
				QString strosek = ostali_stroski.left(ostali_stroski.indexOf(";", 0));
				ostali_stroski = ostali_stroski.right(ostali_stroski.length() - ostali_stroski.indexOf(";", 0) - 1);
				QString ime_stroska = strosek.left(strosek.indexOf(",", 0));
				QString vrednost_stroska = strosek.right(strosek.length() - strosek.indexOf(",", 0) - 1).replace(".", ",") + " EUR";
				// nastavimo tip pisave
				painter.setFont(vstavljeno_besedilo);
				// narisemo besedilo
				painter.drawText(polje_2 * 2, pozicija, polje_2 * 6, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, ime_stroska);
				// narisemo besedilo
				painter.drawText(polje_2 * 6, pozicija, polje_2 * 7, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, vrednost_stroska);
				// nastavimo razmik med crto
				pozicija += visina_vrstice + visina_vrstice * faktor_razmika_med_vrsticami_2;
			} while ( ostali_stroski.indexOf(";", 0) != -1 );
			// postavimo pozicijo na prejsnjo vrstico
			pozicija -= visina_vrstice - visina_vrstice * faktor_razmika_med_vrsticami_2;

			// nastavimo besedilo
			besedilo = znesek_drugih_stroskov.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = polje_1 + razmik_med_vrsticami;
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// narisemo besedilo
			painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		}

		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj v znesku)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, polje_1, 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnic.replace(",", ".").toDouble() + razdalja.toDouble() * kilometrina.toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (ostane za izplacilo)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnic.replace(",", ".").toDouble() + razdalja.toDouble() * kilometrina.toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// pokoncne crte
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, prvotna_visina, 0, pozicija)); // levo
		painter.drawLine(QLine(polje_1, prvotna_visina, polje_1, pozicija)); // sredina
		painter.drawLine(QLine(printer.width(), prvotna_visina, printer.width(), pozicija)); // desno

	// noga
		// nastavimo nov razmik
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_2;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Priloge)
		besedilo = potni_nalog.readLine() + ": ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = priloge;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// zapomnimo si zgornjo visino
		prvotna_visina = pozicija;

		// leva tretjina
		// nastavimo besedilo (V)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_posta + " ";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (dne)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Likvidator)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Blagajnik)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo tretjo tretjino
		pozicija = prvotna_visina;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Predlagatelj racuna)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj

		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odredbodajalec)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo odmik od crte
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Prejemnik)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		painter.end();

	}

}

void wid_potninalogi::printpdf(QString id) {

	/**
		* id nosi id stevilko potnega naloga, s cimer lahko hkrati natisnemo
		* vec dokumentov, bodisi istih, bodisi razlicnih
		**/

	/**
		* Nastavimo spremenljivke, ki jih bomo uporabili v dokumentu
		**/
	// podatki o potnem nalogu
	QString stevilka_naloga = "";
	QString datum_naloga = "";
	QString namen_potnega_naloga = "";
	QString prevozno_sredstvo = "";
	QString cena_dnevnice_6_8 = "0.00";
	QString cena_dnevnice_8_12 = "0.00";
	QString cena_dnevnice_12_24 = "0.00";
	QString stevilo_dnevnic_6_8 = "0";
	QString stevilo_dnevnic_8_12 = "0";
	QString stevilo_dnevnic_12_24 = "0";
	QString stroski_skupaj = "";
	QString razdalja = "0";
	QString kilometrina = "";
	QString ostali_stroski = "";
	QString znesek_drugih_stroskov = "";
	QString zvisanje_dnevnic = "0";
	QString priloge = "";
	QString stevilo_dnevnic_1 = "0";
	QString stevilo_dnevnic_2 = "0";
	QString cena_dnevnic_1 = "0.00";
	QString cena_dnevnic_2 = "0.00";
	QString cena_dnevnic = "0.00";

	// podatki o predlagatelju - podjetje
	QString predlagatelj_podjetje_ime;
	QString predlagatelj_podjetje_polno_ime;
	QString predlagatelj_podjetje_naslov;
	QString predlagatelj_podjetje_naslov_st;
	QString predlagatelj_podjetje_postna_st;
	QString predlagatelj_podjetje_posta;
	QString predlagatelj_podjetje_logotip;

	// podatki o predlagatelju - oseba
	QString predlagatelj_oseba_ime;
	QString predlagatelj_oseba_priimek;
	QString predlagatelj_oseba_naslov;
	QString predlagatelj_oseba_naslov_stevilka;
	QString predlagatelj_oseba_postna_stevilka;
	QString predlagatelj_oseba_posta;

	// podatki o prejemniku
	QString prejemnik_ime;
	QString prejemnik_priimek;
	QString prejemnik_naziv;
	QString prejemnik_naslov;
	QString prejemnik_naslov_stevilka;
	QString prejemnik_postna_stevilka;
	QString prejemnik_posta;
	QString prejemnik_znamka_avtomobila;
	QString prejemnik_model_avtomobila;
	QString prejemnik_registrska_stevilka;

	// podatki o potovanju
	QString relacija;
	QString datum_odhoda;
	QString ura_odhoda;
	QString potovanje;
	QString datum_prihoda;
	QString ura_prihoda;
	QString dnevi_potovanja;
	QDateTime dt_trajanje;
	QDateTime dt_datum_odhoda;
	QDateTime dt_datum_prihoda;
	int minute;
	int ure;

	// odpri podatke o potnem nalogu, prejemniku, stroskih...
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
			QSqlQuery sql_potni_nalog;
			sql_potni_nalog.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + pretvori(id) + "'");
			sql_potni_nalog.exec();
			if ( sql_potni_nalog.next() ) {
				stevilka_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString());
				datum_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("datum_naloga")).toString());
				namen_potnega_naloga = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("namen_naloga")).toString());
//				cena_dnevnice = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnic")).toString());

				prevozno_sredstvo = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prevozno_sredstvo")).toString());

				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("priznana_dnevnica")).toString()) != "0" ) {
					cena_dnevnice_6_8 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_6_8")).toString());
					cena_dnevnice_8_12 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_8_12")).toString());
					cena_dnevnice_12_24 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("cena_dnevnice_12_24")).toString());

					stevilo_dnevnic_6_8 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_6_8")).toString());
					stevilo_dnevnic_8_12 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_8_12")).toString());
					stevilo_dnevnic_12_24 = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("dnevnica_12_24")).toString());
				}

				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("zajtrk_8_12")).toString()) != "0" ) {
					double cenadnevnice = 0.0;
					cenadnevnice = cena_dnevnice_8_12.toDouble() * 0.85;
					cena_dnevnice_8_12 = QString::number(cenadnevnice, 'f', 2);
				}
				if ( prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("zajtrk_12_24")).toString()) != "0" ) {
					double cenadnevnice = 0.0;
					cenadnevnice = cena_dnevnice_12_24.toDouble() * 0.90;
					cena_dnevnice_12_24 = QString::number(cenadnevnice, 'f', 2);
				}

				stroski_skupaj = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stroski_skupaj")).toString());
				razdalja = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("skupaj_kilometri")).toString()).replace(".", ",");
				kilometrina = prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("kilometrina")).toString()).replace(".", ",");
				priloge =  prevedi(sql_potni_nalog.value(sql_potni_nalog.record().indexOf("priloge")).toString()).replace("\n", ", ");

				// uredi dnevnice
				/**
					* ce obataja dnevnica/dnevnice za vec kot 12 ur, jih dodaj kot
					* dnevnica_2, ki jo bomo prikazali v spodnji vrstici
					* poglej, ce obstaja poleg dnevnice_2 se dnevnica_1, ki je lahko
					* polovicna ali tretjinska, ne moreta biti pa obe hkrati
					* dodaj jo kot dnevnica_1
					* ce ne obstaja celodnevna, potem kot dnevnica_2 dodaj ali polovicno
					* ali
					**/
				if ( stevilo_dnevnic_12_24 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_12_24;
					cena_dnevnic_2 = cena_dnevnice_12_24;
					if ( stevilo_dnevnic_6_8 != "0" ) {
						stevilo_dnevnic_1 = stevilo_dnevnic_6_8;
						cena_dnevnic_1 = cena_dnevnice_6_8;
					}
					else if ( stevilo_dnevnic_8_12 != "0" ) {
						stevilo_dnevnic_1 = stevilo_dnevnic_8_12;
						cena_dnevnic_1 = cena_dnevnice_8_12;
					}
				}
				else if ( stevilo_dnevnic_6_8 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_6_8;
					cena_dnevnic_2 = cena_dnevnice_6_8;
				}
				else if ( stevilo_dnevnic_8_12 != "0" ) {
					stevilo_dnevnic_2 = stevilo_dnevnic_8_12;
					cena_dnevnic_2 = cena_dnevnice_8_12;
				}

				// podatki o drugih stroskih
				QSqlQuery sql_stroski;
				sql_stroski.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(stevilka_naloga) + "'");
				sql_stroski.exec();
				double stroski = 0.0;
				while ( sql_stroski.next() ) {
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("strosek")).toString()) + ",";
					ostali_stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()) + ";";
					stroski += prevedi(sql_stroski.value(sql_stroski.record().indexOf("cena")).toString()).toDouble();
				}
				znesek_drugih_stroskov = QString::number(stroski, 'f', 2);

				// podatki o predlagatelju - podjetje
				QSqlQuery sql_predlagatelj_podjetje;
				sql_predlagatelj_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_podjetje")).toString() + "'");
				sql_predlagatelj_podjetje.exec();
				if ( sql_predlagatelj_podjetje.next() ) {
					predlagatelj_podjetje_ime = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("ime")).toString());
					predlagatelj_podjetje_polno_ime = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("polnoime")).toString());
					predlagatelj_podjetje_naslov = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("naslov")).toString());
					predlagatelj_podjetje_naslov_st = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("naslov_st")).toString());
					predlagatelj_podjetje_postna_st = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("postna_stevilka")).toString());
					predlagatelj_podjetje_posta = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("posta")).toString());
					predlagatelj_podjetje_logotip = prevedi(sql_predlagatelj_podjetje.value(sql_predlagatelj_podjetje.record().indexOf("logotip")).toString());
				}

				// podatki o predlagatelju - oseba
				QSqlQuery sql_predlagatelj_oseba;
				sql_predlagatelj_oseba.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("predlagatelj_oseba")).toString() + "'");
				sql_predlagatelj_oseba.exec();
				if ( sql_predlagatelj_oseba.next() ) {
					predlagatelj_oseba_ime = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("ime")).toString());
					predlagatelj_oseba_priimek = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("priimek")).toString());
					predlagatelj_oseba_naslov = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("naslov")).toString());
					predlagatelj_oseba_naslov_stevilka = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("naslov_stevilka")).toString());
					predlagatelj_oseba_postna_stevilka = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("postna_stevilka")).toString());
					predlagatelj_oseba_posta = prevedi(sql_predlagatelj_oseba.value(sql_predlagatelj_oseba.record().indexOf("posta")).toString());
				}

				// podatki o prejemniku
				QSqlQuery sql_prejemnik;
				sql_prejemnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("prejemnik_oseba")).toString() + "'");
				sql_prejemnik.exec();
				if ( sql_prejemnik.next() ) {
					prejemnik_ime = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("ime")).toString());
					prejemnik_priimek = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("priimek")).toString());
					prejemnik_naziv = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naziv")).toString());
					prejemnik_naslov = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov")).toString());
					prejemnik_naslov_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("naslov_stevilka")).toString());
					prejemnik_postna_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("postna_stevilka")).toString());
					prejemnik_posta = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("posta")).toString());
					prejemnik_znamka_avtomobila = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("avtomobil")).toString());
					prejemnik_model_avtomobila = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("model_avtomobila")).toString());
					prejemnik_registrska_stevilka = prevedi(sql_prejemnik.value(sql_prejemnik.record().indexOf("registracija")).toString());
				}
				QSqlQuery sql_prejemnik_naziv;
				sql_prejemnik_naziv.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + pretvori(prejemnik_naziv) + "'");
				sql_prejemnik_naziv.exec();
				if ( sql_prejemnik_naziv.next() ) {
					prejemnik_naziv = prevedi(sql_prejemnik_naziv.value(sql_prejemnik_naziv.record().indexOf("naziv")).toString());
				}

				QSqlQuery sql_naloga;
				sql_naloga.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + pretvori(namen_potnega_naloga) + "'");
				sql_naloga.exec();
				if ( sql_naloga.next() ) {
					namen_potnega_naloga = prevedi(sql_naloga.value(sql_naloga.record().indexOf("namen")).toString());
				}

				QSqlQuery sql_prevoz;
				sql_prevoz.prepare("SELECT * FROM sif_prevozna_sredstva WHERE id LIKE '" + pretvori(prevozno_sredstvo) + "'");
				sql_prevoz.exec();
				if ( sql_prevoz.next() ) {
					prevozno_sredstvo = prevedi(sql_prevoz.value(sql_prevoz.record().indexOf("prevoz")).toString());
				}

				// podatki o potovanju
				QSqlQuery sql_pot;
				sql_pot.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_potni_nalog.value(sql_potni_nalog.record().indexOf("stevilka_naloga")).toString() + "'");
				sql_pot.exec();
				QString rel_start = "";
				QString rel_cilj = "";
				dt_datum_odhoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
				dt_datum_prihoda = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");

				while ( sql_pot.next() ) {

					// racunanje relacije potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString()) != rel_cilj ) {
						rel_start = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString());
					}
					rel_cilj = ""; // izpraznimo cilj, da se ne podvaja

					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString()) != rel_start ) {
						rel_cilj = prevedi(sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString());
					}
					if ( rel_start != "" && relacija != "" ) {
						rel_start = " - " + rel_start;
					}
					if ( rel_cilj != "" ) {
						rel_cilj = " - " + rel_cilj;
					}
					if ( relacija != "" ) {
						relacija = relacija + rel_cilj;
					}
					else {
						relacija = rel_start + rel_cilj;
					}
					rel_start = ""; // izpraznimo start, da se ne podvaja

					// racunanje koncnih tock potovanja
					if ( prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()) != "" ) {
						potovanje = prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()).replace("\n", ", ") + "\n";
					}
			// uporabi samo, ce se bo pokazala potreba po odstranitvi zadnje vrstice
			// trenutno odstranjeno, saj ce zanji kraj nima vnesenega naslova, vseeno reze znake
			//		potovanje = potovanje.remove(potovanje.length() - 2, 2); // zadnji kraj ima tudi prelom v novo vrstico, ki pa tu ni potreben

					// racunanje podatkov o odhodu
					if ( dt_datum_odhoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_odhoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) < 0 ) {
						dt_datum_odhoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_odhoda = dt_datum_odhoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					dt_trajanje = dt_datum_odhoda;
					ura_odhoda = datum_odhoda.right(5);
					datum_odhoda = datum_odhoda.left(datum_odhoda.length() - 6);

					if ( dt_datum_prihoda == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					if ( dt_datum_prihoda.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) > 0 ) {
						dt_datum_prihoda = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
					}
					datum_prihoda = dt_datum_prihoda.toString("dd'.'MM'.'yyyy' 'hh':'mm");
					ura_prihoda = datum_prihoda.right(5);
					datum_prihoda = datum_prihoda.left(datum_prihoda.length() - 6);
				}
				dnevi_potovanja = QString::number(dt_trajanje.daysTo(dt_datum_prihoda), 10);
				minute = (dt_trajanje.secsTo(dt_datum_prihoda) - dt_trajanje.daysTo(dt_datum_prihoda) * 24 * 60 * 60) / 60;
				ure = minute / 60;
				minute = minute - ure * 60;
			}
		}
		base.close();

	/**
		* Odpremo sifrant z besedilom potnega naloga
		* Besedilo bomo vztavljali z readLine
		**/

		QFile datoteka(app_path + "/potni-nalog-besedilo.csv");
		if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return;
		}
		QTextStream potni_nalog(&datoteka);

	/**
		* Nastavimo podatke o tiskanju. Tu tiskamo v PDF dokument
		* velikost A4, kar po obliki ustreza tiskanju v koncni razlicici, ko
		* se bo tiskalo tako v PDF dokument kot direktno na tiskalnik
		*/

	// ustvariti pot do ustrezne mape
		QString mapa_za_shranjevanje = "";
		mapa_za_shranjevanje = predlagatelj_podjetje_logotip.left(predlagatelj_podjetje_logotip.lastIndexOf("/")); // izreze logotip
		mapa_za_shranjevanje = mapa_za_shranjevanje.left(mapa_za_shranjevanje.lastIndexOf("/")); // izreze mapo za logotip
//		mapa_za_shranjevanje = QFileDialog::getExistingDirectory(this,
//																														 "Izberite mapo za shranjevanje dokumentov",
//																														 mapa_za_shranjevanje, QFileDialog::ShowDirsOnly);
//		if ( mapa_za_shranjevanje == "" ) {
//			return;
//		}

		QDir mapa(mapa_za_shranjevanje);
		mapa.mkdir("potni-nalogi");
		mapa.cd("potni-nalogi");
		mapa.mkdir(datum_naloga.right(4));
		mapa.cd(datum_naloga.right(4));

	// podatki o printerju

	QPrinter printer;
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(mapa.path() + "/potni-nalog-" + datum_naloga.right(4) +
														"-" + datum_naloga.left(5).right(2) + "-" + datum_naloga.left(2) + "-" + stevilka_naloga + ".pdf");
	printer.setPaperSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);

//	QPrintDialog *dialog = new QPrintDialog(&printer, this);
//	dialog->setWindowTitle(tr("Print Document"));

//	if (dialog->exec() == QDialog::Accepted) {
		QPainter painter;

		if (! painter.begin(&printer))  { // failed to open file
			qWarning("Datoteke ni mozno shraniti, prosim uredite dovoljenje za pisanje!");
			return;
		}

		double pozicija = 0;
		double visina_vrstice = 0;
		double sirina_besedila = 0;
		double razmik_med_vrsticami = 0;
		double faktor_razmika_med_vrsticami_1 = 0.3; // pri tabelah
		double faktor_razmika_med_vrsticami_2 = 0.0; // pri besedilu

		QString besedilo = "";

		QFont vstavljeno_besedilo("Arial", 10);
		QFont stalno_besedilo("Arial", 10, QFont::Bold);

	// narisemo glavo
		QPen *svincnik = new QPen;

		// dolocimo visino vrstice
		besedilo = "To je testno besedilo";
		painter.setFont(stalno_besedilo);
		QRect velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		visina_vrstice = velikost_besedila.height();
		razmik_med_vrsticami = velikost_besedila.height() * faktor_razmika_med_vrsticami_1; // razmik med vrsicami, za lazje branje dokumenta

		// crta zgoraj
		svincnik->setWidth(0.5);
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, 0, printer.width(), 0));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, printer.width() / 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_polno_ime;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, printer.width() / 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo odmik od crte
		pozicija = razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami + printer.width() / 2, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta pod naslovom naloga
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() / 2, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami + printer.width() / 2;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = stevilka_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dodamo okvir besedila
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, 0, 0, pozicija)); // crta levo
		painter.drawLine(QLine(printer.width() / 2, 0, printer.width() / 2, pozicija)); // crta sredina
		painter.drawLine(QLine(printer.width(), 0, printer.width(), pozicija)); // crta desno
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija)); // crta spodaj

	// podatki o prejemniku potnega naloga
		// nastavimo nov razmik med vrsticami
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_2;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_ime + " " + prejemnik_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naziv;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_naslov + " " + prejemnik_naslov_stevilka + ", " + prejemnik_postna_stevilka + " " + prejemnik_posta;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potovanje;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = namen_potnega_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = "(" + prevozno_sredstvo + "): " + prejemnik_znamka_avtomobila + " " + prejemnik_model_avtomobila;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_polno_ime;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
//		besedilo = cena_dnevnice.replace(".", ",") + " EUR";
		besedilo = cena_dnevnice_12_24.replace(".", ",") + " EUR";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + 15;
		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// manjka besedilo posebnih dodatkov, dokler ne izvemo, kaj to je...
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

	// tabela predujma
		int prvotna_visina = pozicija; // ohranimo zacetno visino za prvo in drugo tretjino
		int visina_sklopa = 0; // najprej narisemo desno tretjino, ki nosi najvec podatkov in je tudi najvisja

		// prva vrstica je prazna
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() * 2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis odredbodajalca)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// dve prazni vrstici
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() * 2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Podpis prejemnika)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice / 2 + razmik_med_vrsticami;

		// dolocimo koncno visino
		visina_sklopa = pozicija;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo prvo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

	// glava obracuna potnih stroskov
		pozicija = visina_sklopa;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_1;
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() - razmik_med_vrsticami, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta sredina
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_ime + " " + predlagatelj_oseba_priimek;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_oseba_naslov + " " + predlagatelj_oseba_naslov_stevilka + ", " + predlagatelj_oseba_posta + " " + predlagatelj_oseba_postna_stevilka;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta sredina
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// crti levo in desno
		painter.drawLine(QLine(0, visina_sklopa, 0, pozicija)); // levo
		painter.drawLine(QLine(printer.width(), visina_sklopa, printer.width(), pozicija)); // desno

		// nastavimo odmik od crte
		pozicija += visina_vrstice + razmik_med_vrsticami;

	// obracun potnih stroskov
		// nastavitev zacetne pozicije
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_1;
		prvotna_visina = pozicija;
		double polje_1 = printer.width() * 7 / 8;
		double polje_2 = printer.width() / 8;
		double polje_3 = polje_2 / 2; // dnevi ne potrebujejo toliko prostora, ure/minute pa več

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odsotnost)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(3 * polje_2 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		if ( stevilo_dnevnic_1 == "0" ) { // ce ni dnevnic ali je samo ena, potem so napisi prek dveh vrstic, v nasprotnem primeru samo prek ene
			// nastavimo besedilo (dnevnice)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice * 2, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo novo pozicijo besedila
			pozicija += visina_vrstice + razmik_med_vrsticami;

			// crta zgoraj
			painter.setPen(*svincnik);
			painter.drawLine(QLine(0, pozicija, polje_2 * 5, pozicija));
		}
		else  { // obstajata dva razlicna tipa dnevnic, zato se naslov razpotega zgolj prek ene same vrstice
			// nastavimo besedilo (dnevnice)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = potni_nalog.readLine() + " ";
			// nastavimo tip pisave
			painter.setFont(stalno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::AlignVCenter | Qt::TextWordWrap, besedilo);

			// nastavimo novo pozicijo besedila
			pozicija += visina_vrstice + razmik_med_vrsticami;

			// crta zgoraj
			painter.setPen(*svincnik);
			painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));
		}

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum odhoda)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_odhoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila - polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila - polje_3, pozicija, 5 * polje_2 - sirina_besedila + polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		if ( stevilo_dnevnic_1 != "0" ) { // obstajata dva razlicna tipa dnevnic
			// nastavimo besedilo (dnevnice_1)
			besedilo = stevilo_dnevnic_1.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (cena dnevnic_1)
			besedilo = cena_dnevnic_1.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

			// nastavimo besedilo (skupaj)
			besedilo = QString::number(pretvori_v_double(cena_dnevnic_1).toDouble() * pretvori_v_double(stevilo_dnevnic_1).toDouble(), 'f', 2).replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = razmik_med_vrsticami;
			// narisemo besedilo
			painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		}
		// razmik med crtami
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Datum prihoda)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo
		besedilo = datum_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 + sirina_besedila, pozicija, 2 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (Ob)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_2 * 2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = ura_prihoda;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, 3 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dni)
		besedilo = dnevi_potovanja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 3 + sirina_besedila, pozicija, 4 * polje_2 - sirina_besedila - polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (ur/minut)
		besedilo = QString::number(ure, 10) + " ur " + QString::number(minute, 10) + " minut";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(4 * polje_2 + sirina_besedila - polje_3, pozicija, 5 * polje_2 - sirina_besedila + polje_3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (dnevnice_2)
		besedilo = stevilo_dnevnic_2.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 5 + sirina_besedila, pozicija, 6 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (cena dnevnic_2)
		besedilo = cena_dnevnic_2.replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 6 + sirina_besedila, pozicija, 7 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nastavimo besedilo (skupaj)
		besedilo = QString::number(pretvori_v_double(cena_dnevnic_2).toDouble() * pretvori_v_double(stevilo_dnevnic_2).toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		// narisemo besedilo
		painter.drawText(polje_2 * 7 + sirina_besedila + 5, pozicija, 8 * polje_2 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// razmik med crtami
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// pokoncne crte
		painter.drawLine(QLine(3 * polje_2, prvotna_visina, 3 * polje_2, pozicija));
		painter.drawLine(QLine(5 * polje_2, prvotna_visina, 5 * polje_2, pozicija));
		painter.drawLine(QLine(6 * polje_2, prvotna_visina, 6 * polje_2, pozicija));

		prvotna_visina += visina_vrstice + razmik_med_vrsticami * 2;

		painter.drawLine(QLine(1 * polje_2, prvotna_visina, 1 * polje_2, pozicija));
		painter.drawLine(QLine(2 * polje_2, prvotna_visina, 2 * polje_2, pozicija));
		painter.drawLine(QLine(4 * polje_2 - polje_3, prvotna_visina, 4 * polje_2 - polje_3, pozicija));

		// vrnitev prvotne pozicije
		prvotna_visina = prvotna_visina - visina_vrstice - razmik_med_vrsticami * 2 + 1;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (% zvisanja dnevnic)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = zvisanje_dnevnic;
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		cena_dnevnic = QString::number(pretvori_v_double(cena_dnevnice_6_8).toDouble() * pretvori_v_double(stevilo_dnevnic_6_8).toDouble() +
																	 pretvori_v_double(cena_dnevnice_8_12).toDouble() * pretvori_v_double(stevilo_dnevnic_8_12).toDouble() +
																	 pretvori_v_double(cena_dnevnice_12_24).toDouble() * pretvori_v_double(stevilo_dnevnic_12_24).toDouble(), 'f', 2).replace(".", ",");
		besedilo = cena_dnevnic;
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (prevozni stroski)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_2 * 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo (št. km)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = razdalja;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (km x )
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = kilometrina;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (EUR)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;

		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo (relacija)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = relacija;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);

		// nova vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo (registracija)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 2 * polje_2 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = prejemnik_registrska_stevilka;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - sirina_besedila, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(pretvori_v_double(kilometrina).toDouble() * pretvori_v_double(razdalja).toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (drugi stroski)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(razmik_med_vrsticami, pozicija, polje_2 * 2, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// za vsak strosek dodamo novo vrstico
		if ( ostali_stroski != "" ) {
			do {
				QString strosek = ostali_stroski.left(ostali_stroski.indexOf(";", 0));
				ostali_stroski = ostali_stroski.right(ostali_stroski.length() - ostali_stroski.indexOf(";", 0) - 1);
				QString ime_stroska = strosek.left(strosek.indexOf(",", 0));
				QString vrednost_stroska = strosek.right(strosek.length() - strosek.indexOf(",", 0) - 1).replace(".", ",") + " EUR";
				// nastavimo tip pisave
				painter.setFont(vstavljeno_besedilo);
				// narisemo besedilo
				painter.drawText(polje_2 * 2, pozicija, polje_2 * 6, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, ime_stroska);
				// narisemo besedilo
				painter.drawText(polje_2 * 6, pozicija, polje_2 * 7, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, vrednost_stroska);
				// nastavimo razmik med crto
				pozicija += visina_vrstice + visina_vrstice * faktor_razmika_med_vrsticami_2;
			} while ( ostali_stroski.indexOf(";", 0) != -1 );
			// postavimo pozicijo na prejsnjo vrstico
			pozicija -= visina_vrstice - visina_vrstice * faktor_razmika_med_vrsticami_2;

			// nastavimo besedilo
			besedilo = znesek_drugih_stroskov.replace(".", ",");
			// nastavimo tip pisave
			painter.setFont(vstavljeno_besedilo);
			// nastavimo polozaj na listu, kjer zapisemo besedilo
			sirina_besedila = polje_1 + razmik_med_vrsticami;
			velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
			// narisemo besedilo
			painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		}

		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (skupaj v znesku)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, polje_1, 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnic.replace(",", ".").toDouble() + razdalja.toDouble() * kilometrina.toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// razmik med crtami
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (ostane za izplacilo)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, polje_1 - razmik_med_vrsticami * 2, visina_vrstice, Qt::AlignRight | Qt::TextWordWrap, besedilo);
		// nastavimo besedilo
		besedilo = QString::number(cena_dnevnic.replace(",", ".").toDouble() + razdalja.toDouble() * kilometrina.toDouble() + znesek_drugih_stroskov.replace(",", ".").toDouble(), 'f', 2).replace(".", ",");
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = polje_1 + razmik_med_vrsticami;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width(), pozicija));

		// pokoncne crte
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, prvotna_visina, 0, pozicija)); // levo
		painter.drawLine(QLine(polje_1, prvotna_visina, polje_1, pozicija)); // sredina
		painter.drawLine(QLine(printer.width(), prvotna_visina, printer.width(), pozicija)); // desno

	// noga
		// nastavimo nov razmik
		razmik_med_vrsticami = visina_vrstice * faktor_razmika_med_vrsticami_2;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Priloge)
		besedilo = potni_nalog.readLine() + ": ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = priloge;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// zapomnimo si zgornjo visino
		prvotna_visina = pozicija;

		// leva tretjina
		// nastavimo besedilo (V)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = predlagatelj_podjetje_posta + " ";
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo (dne)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo besedilo
		besedilo = datum_naloga;
		// nastavimo tip pisave
		painter.setFont(vstavljeno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		sirina_besedila += velikost_besedila.width() + razmik_med_vrsticami;
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// nastavimo polozaj na listu, kjer zapisemo besedilo
		sirina_besedila = 0;
		velikost_besedila = painter.boundingRect(0, 0, printer.width(), 0, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// narisemo besedilo
		painter.drawText(sirina_besedila, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Likvidator)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(0, pozicija, printer.width() / 3, pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Blagajnik)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width() / 3, visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo drugo tretjino
		pozicija = prvotna_visina;

		// nekaj praznih vrstic
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo besedilo
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(0, pozicija, printer.width(), visina_vrstice, Qt::AlignCenter | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// napisemo tretjo tretjino
		pozicija = prvotna_visina;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Predlagatelj racuna)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// prazna vrstica
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj

		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Odredbodajalec)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// nastavimo odmik od crte
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;
		pozicija += visina_vrstice + razmik_med_vrsticami;

		// crta zgoraj
		painter.setPen(*svincnik);
		painter.drawLine(QLine(printer.width() *2 / 3, pozicija, printer.width(), pozicija));

		// nastavimo odmik od crte
		pozicija += razmik_med_vrsticami;

		// nastavimo besedilo (Prejemnik)
		besedilo = potni_nalog.readLine() + " ";
		// nastavimo tip pisave
		painter.setFont(stalno_besedilo);
		// narisemo besedilo
		painter.drawText(printer.width() * 2 / 3, pozicija, printer.width(), visina_vrstice, Qt::AlignJustify | Qt::TextWordWrap, besedilo);
		// nastavimo novo pozicijo besedila
		pozicija += visina_vrstice + razmik_med_vrsticami;

		painter.end();


//	}

}

void wid_potninalogi::on_btn_print_seznam_clicked() {

}

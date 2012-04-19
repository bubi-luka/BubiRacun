#include <QtSql>
#include <QMessageBox>
#include <QSortFilterProxyModel>

#include "wid_casovnice.h"
#include "ui_wid_casovnice.h"
#include "varnost.h"
#include "kodiranje.h"


wid_casovnice::wid_casovnice(QWidget *parent) :
    QWidget(parent),
		ui(new Ui::wid_casovnice)
{
    ui->setupUi(this);

		// izbrisi vsa polja
		ui->tbl_casovnice->clear();

		ui->cb_aktivnost->setText("");

		ui->txt_leto->clear();
		ui->txt_mesec->clear();
		ui->txt_avtor->clear();
		ui->txt_stranka->clear();
		ui->txt_projekt->clear();
		ui->txt_racun->clear();

		// nastavi privzeto polnenje vseh racunov
		ui->cb_aktivnost->setChecked(false);

		// napolnimo podatke
		napolni_sezname();

}

wid_casovnice::~wid_casovnice()
{
    delete ui;
}

QString wid_casovnice::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_casovnice::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString wid_casovnice::pretvori_v_double(QString besedilo) {

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

	besedilo = QString::number(besedilo.toDouble(), 'f', 2);

	return besedilo;

}

QString wid_casovnice::pretvori_iz_double(QString besedilo) {

	besedilo.replace(".",",");

	return besedilo;

}

void wid_casovnice::on_cb_aktivnost_toggled() {

	napolni_sezname();

}

void wid_casovnice::on_txt_leto_currentIndexChanged() {

	if ( ui->txt_leto->currentText() != "" && ui->txt_mesec->currentText() != "" ) {
		napolni();
	}
	else if ( ui->txt_leto->currentText() == "" ) {
		ui->txt_mesec->setCurrentIndex(ui->txt_mesec->findText(""));
		napolni();
	}

}

void wid_casovnice::on_txt_mesec_currentIndexChanged() {

	if ( ui->txt_leto->currentText() != "" && ui->txt_mesec->currentText() != "" ) {
		napolni();
	}

}

void wid_casovnice::on_txt_avtor_currentIndexChanged() {

	if ( ui->txt_leto->currentText() != "" && ui->txt_mesec->currentText() != "" ) {
		napolni();
	}

}

void wid_casovnice::on_txt_stranka_currentIndexChanged() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_casovnice_projekte");
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

		// napolni spustni seznam pri projektih
		ui->txt_projekt->clear();
		ui->txt_projekt->addItem("");
		QSqlQuery projekt;
		if ( ui->txt_stranka->currentText() != "" ){
			projekt.prepare("SELECT * FROM projekti WHERE stranka LIKE '" + pretvori(ui->txt_stranka->currentText().left(ui->txt_stranka->currentText().indexOf(") "))) + "'");
		}
		projekt.exec();
		while ( projekt.next() ) {
			ui->txt_projekt->addItem(prevedi(projekt.value(projekt.record().indexOf("id")).toString()) + ") " +
															 prevedi(projekt.value(projekt.record().indexOf("naslov_projekta")).toString()));
		}
	}
	base.close();

	if ( ui->txt_leto->currentText() != "" && ui->txt_mesec->currentText() != "" ) {
		napolni();
	}

}

void wid_casovnice::on_txt_projekt_currentIndexChanged() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_casovnice_projekte");
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

		// napolni spustni seznam pri racunih
		ui->txt_racun->clear();
		ui->txt_racun->addItem("");
		QSqlQuery racun;
		QString stavek = "SELECT * FROM racuni WHERE tip_racuna LIKE '3'";

		if ( ui->txt_projekt->currentText() != "" ) {
			stavek += " AND projekt LIKE '" + pretvori(ui->txt_projekt->currentText().left(ui->txt_projekt->currentText().indexOf(") "))) + "'";
		}
		if ( ui->cb_aktivnost->isChecked() ) {
			stavek += " AND status_oddaje_racuna LIKE ''";
		}
		racun.prepare(stavek);
		racun.exec();
		while ( racun.next() ) {
			ui->txt_racun->addItem(prevedi(racun.value(racun.record().indexOf("id")).toString()) + ") " +
															 prevedi(racun.value(racun.record().indexOf("stevilka_racuna")).toString()));
		}
	}
	base.close();

	if ( ui->txt_leto->currentText() != "" && ui->txt_mesec->currentText() != "" ) {
		napolni();
	}

}

void wid_casovnice::on_txt_racun_currentIndexChanged() {

	if ( ui->txt_leto->currentText() != "" && ui->txt_mesec->currentText() != "" ) {
		napolni();
	}

}

void wid_casovnice::on_tbl_casovnice_cellChanged(int vrstica, int stolpec) {

	if ( ui->cb_aktivnost->text() != "Vnasam" ) {
			QString datum = ui->txt_leto->currentText() + "." + ui->txt_mesec->currentText().left(2) + ".";
			int dan = stolpec - 5;
			if ( dan < 10 ) {
				datum += "0";
			}
			datum += QString::number(dan, 10) + ".";
			QString vrednost = pretvori_v_double(ui->tbl_casovnice->item(vrstica, stolpec)->text());

			QString id = ui->tbl_casovnice->item(vrstica, 0)->text();

			QString app_path = QApplication::applicationDirPath();
			QString dbase_path = app_path + "/base.bz";

			QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "osvezi");
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

				// poiscemo vse zapise za casovnice za izbran zapis
				QString casovnice = "";

				QSqlQuery poisci;
				poisci.prepare("SELECT * FROM opravila WHERE id LIKE '" + id + "'");
				poisci.exec();
				if ( poisci.next() ) {
					casovnice = poisci.value(poisci.record().indexOf("casovnice")).toString();
				}

				// prestejemo vse zapise v tej casovnici
				int max_casovnic = casovnice.count(";");

				QString casovnice_new = datum + "," + pretvori_v_double(vrednost) + ";";

				// ce je to nasa prva casovnica, zakljucimo vnos
				// ce obstajajo se druge casovnice, jih dodamo v nas seznam
				if ( max_casovnic != 0 ) { // ce casovnice ze obstajajo nadaljujemo
					for ( int i = 1; i <= max_casovnic; i++ ) { // vsako ze obstojeco casovnico razbijemo

					// v seznam shranimo trenutno casovnico
						QString seznam = casovnice.left(casovnice.indexOf(";"));

					// v casovnice shranimo preostanek za naslednji cikelj
						casovnice = casovnice.right(casovnice.length() - casovnice.indexOf(";") - 1);

					// tenutno casovnico (seznam) razbijemo na datum in vrednost
						QString seznam_datum = seznam.left(seznam.indexOf(","));
						QString seznam_vrednost = pretvori_v_double(seznam.right(seznam.length() - seznam.indexOf(",") - 1));

					// ce datum ze obstaja, ga izbrisemo - popravljeno vrednost smo zapisali na zacetku
						if ( seznam_datum != datum) {
							casovnice_new += seznam_datum + "," + seznam_vrednost + ";";
						}
					}
				}

				// izbrisemo nulto vrednost
				if ( vrednost == "0" || vrednost == "" || vrednost == "0.0" || vrednost == "0.00") {
					casovnice_new.remove(datum + "," + vrednost + ";");
				}

				// vrednost zapisemo v bazo
				QSqlQuery popravi;
				popravi.prepare("UPDATE opravila SET casovnice = ? WHERE id LIKE '" + id + "'");
				popravi.bindValue(0, casovnice_new);
				popravi.exec();

			}
			base.close();

			// prepisi vrednost polja na prvotno stanje
			// da preprecimo morebitno ponavljanje zanke, vnesemo izkljucitveno besedilo, ki ga bomo na koncu izbrisali
			QString cb_aktivnost_besedilo = ui->cb_aktivnost->text();
			ui->cb_aktivnost->setText("Vnasam");

			// pretvorimo vrednost v ustrezno obliko
			vrednost = pretvori_iz_double(pretvori_v_double(vrednost));

			// vnesi novo vrednost v ustrezno polje
			QTableWidgetItem *t_vrednost = new QTableWidgetItem;
			t_vrednost->setText(vrednost);
			ui->tbl_casovnice->setItem(vrstica, stolpec, t_vrednost);

			// povrnemo predhodno nastavljeno besedilo na privzeto vrednost
			ui->cb_aktivnost->setText(cb_aktivnost_besedilo);

		napolni();

		preracunaj_casovnice(ui->tbl_casovnice->item(vrstica, 0)->text());

	}

}

void wid_casovnice::napolni_sezname() {

	// izbrisi spustne sezname
	ui->txt_leto->clear();
	ui->txt_mesec->clear();
	ui->txt_avtor->clear();
	ui->txt_stranka->clear();
	ui->txt_projekt->clear();
	ui->txt_racun->clear();

	// napolni spustne sezname
	ui->txt_mesec->addItem("");
	ui->txt_mesec->addItem("01) Januar");
	ui->txt_mesec->addItem("02) Februar");
	ui->txt_mesec->addItem("03) Marec");
	ui->txt_mesec->addItem("04) April");
	ui->txt_mesec->addItem("05) Maj");
	ui->txt_mesec->addItem("06) Junij");
	ui->txt_mesec->addItem("07) Julij");
	ui->txt_mesec->addItem("08) Avgust");
	ui->txt_mesec->addItem("09) September");
	ui->txt_mesec->addItem("10) Oktober");
	ui->txt_mesec->addItem("11) November");
	ui->txt_mesec->addItem("12) December");

	ui->txt_leto->addItem("");
	ui->txt_stranka->addItem("");

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_casovnice_zacetni");
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
		QSqlQuery vnesi;

		// napolni usluzbence
		ui->txt_avtor->addItem("");
		vnesi.prepare("SELECT * FROM uporabniki WHERE podjetje LIKE '" + vApp->firm() + "'");
		vnesi.exec();
		while ( vnesi.next() ) {
			ui->txt_avtor->addItem(prevedi(vnesi.value(vnesi.record().indexOf("id")).toString()) + ") " +
														 prevedi(vnesi.value(vnesi.record().indexOf("priimek")).toString()) + " " +
														 prevedi(vnesi.value(vnesi.record().indexOf("ime")).toString()));
		}
		vnesi.clear();

		// napolni spustne sezname, ki so povezani s projekti/racuni
		if ( ui->cb_aktivnost->isChecked() ) {
			vnesi.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND status_oddaje_racuna LIKE ''");
		}
		else {
			vnesi.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'");
		}
		vnesi.exec();
		while ( vnesi.next() ) {

			// napolni leta racunov
			if ( ui->txt_leto->findText(prevedi(vnesi.value(vnesi.record().indexOf("datum_izdaje")).toString()).right(4)) == -1 ) {
				ui->txt_leto->addItem(prevedi(vnesi.value(vnesi.record().indexOf("datum_izdaje")).toString()).right(4));
			}

			// napolni stranke
			QSqlQuery stranka;
			stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(vnesi.value(vnesi.record().indexOf("stranka")).toString()) + "'");
			stranka.exec();
			if ( stranka.next() ) {
				QString ime_stranke = "";
				if ( prevedi(stranka.value(stranka.record().indexOf("tip")).toString()) == "2" ) { // pravna
					ime_stranke = prevedi(stranka.value(stranka.record().indexOf("id")).toString()) + ") " +
							prevedi(stranka.value(stranka.record().indexOf("ime")).toString());
				}
				else { // fizicna
					ime_stranke = prevedi(stranka.value(stranka.record().indexOf("id")).toString()) + ") " +
							prevedi(stranka.value(stranka.record().indexOf("priimek")).toString()) + " " +
							prevedi(stranka.value(stranka.record().indexOf("ime")).toString());
				}

				if ( ui->txt_stranka->findText(ime_stranke) == -1 ) {
					ui->txt_stranka->addItem(ime_stranke);
				}
			}
		}
	}
	base.close();

	// razvrsti leta narascajoce
	QSortFilterProxyModel *proxy = new QSortFilterProxyModel(ui->txt_leto);
	proxy->setSourceModel(ui->txt_leto->model());
	ui->txt_leto->model()->setParent(proxy);
	ui->txt_leto->setModel(proxy);
	ui->txt_leto->model()->sort(0, Qt::AscendingOrder);


	// izberi trenuten datum
	ui->txt_leto->setCurrentIndex(ui->txt_leto->findText(QDate::currentDate().toString("yyyy")));
	if ( ui->txt_leto->currentText() != "" ) {
		ui->txt_mesec->setCurrentIndex(ui->txt_mesec->findText(QDate::currentDate().toString("MM") + ")", Qt::MatchStartsWith));
	}
	else {
		ui->txt_mesec->setCurrentIndex(ui->txt_mesec->findText(""));
	}

	ui->txt_avtor->setCurrentIndex(ui->txt_avtor->findText(vApp->id() + ") ", Qt::MatchStartsWith));

}

void wid_casovnice::napolni() {

	QString cb_aktivnost_besedilo = ui->cb_aktivnost->text();
	ui->cb_aktivnost->setText("Vnasam");

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_casovnice_tabela");
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

		// clear previous content
		ui->tbl_casovnice->clear();

		for (int i = 0; i <= 38; i++) {
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
		ui->tbl_casovnice->insertColumn(3);
		ui->tbl_casovnice->insertColumn(4);
		ui->tbl_casovnice->insertColumn(5);

		// set proper width to the first four columns

		ui->tbl_casovnice->setColumnWidth(0, 0);
		ui->tbl_casovnice->setColumnWidth(1, 0);
		ui->tbl_casovnice->setColumnWidth(2, 200);
		ui->tbl_casovnice->setColumnWidth(3, 400);
		ui->tbl_casovnice->setColumnWidth(4, 60);
		ui->tbl_casovnice->setColumnWidth(5, 60);

		// start filling the table
		QStringList naslovi;
		naslovi.append("ID");
		naslovi.append("St. racuna");
		naslovi.append("Projekt");
		naslovi.append("Opravilo");
		naslovi.append("Mesecno");
		naslovi.append("Skupaj");

		QString tekoce_leto = ui->txt_leto->currentText();
		QString tekoci_mesec = ui->txt_mesec->currentText().left(2);

		QDate pricetek = QDate::fromString("01." + tekoci_mesec + "." + tekoce_leto, "dd.MM.yyyy");

		for ( int a = 6; a <= 38; a++ ) {
			if ( pricetek.toString("MM") == tekoci_mesec ) {
				ui->tbl_casovnice->insertColumn(a);
				ui->tbl_casovnice->setColumnWidth(a, 45);
				naslovi.append(QString::number(a - 5, 10));
				pricetek = pricetek.addDays(1);
			}
		}

		ui->tbl_casovnice->setHorizontalHeaderLabels(naslovi);
		ui->tbl_casovnice->setVerticalHeaderLabels(naslovi);

		int row = 0;

		QSqlQuery racuni;
		QString stavek_racuni = "SELECT * FROM racuni WHERE tip_racuna LIKE '3'";
		if ( ui->txt_projekt->currentText() != "" ) {
			stavek_racuni += " AND projekt LIKE '" + pretvori(ui->txt_projekt->currentText().left(ui->txt_projekt->currentText().indexOf(") "))) + "'";
		}
		if ( ui->txt_stranka->currentText() != "" ) {
			stavek_racuni += " AND stranka LIKE '" + pretvori(ui->txt_stranka->currentText().left(ui->txt_stranka->currentText().indexOf(") "))) + "'";
		}
		if ( ui->txt_racun->currentText() != "" ) {
			stavek_racuni += " AND id LIKE '" + pretvori(ui->txt_racun->currentText().left(ui->txt_racun->currentText().indexOf(") "))) + "'";
		}
		if ( ui->cb_aktivnost->isChecked() ) {
			stavek_racuni += " AND status_oddaje_racuna LIKE ''";
		}
		racuni.prepare(stavek_racuni);
		racuni.exec();
		while ( racuni.next() ) {

			QSqlQuery vnesi;
			vnesi.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + racuni.value(racuni.record().indexOf("id")).toString() + "'");
			vnesi.exec();
			while ( vnesi.next() ) {
				ui->tbl_casovnice->insertRow(row);
				ui->tbl_casovnice->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[6] = {"id", "stevilka_racuna", "stevilka_projekta", "opravilo", "zdruzek", "casovnice"};

				double d_mesec = 0.0;
				double d_skupaj = 0.0;

				while (col <= 5) {

					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[i] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(vnesi.value(vnesi.record().indexOf(polja[i])).toString()).toInt());
					}
					else if ( polja[i] == "stevilka_projekta" ) {
						QSqlQuery vnesi_projekt;
						vnesi_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + vnesi.value(vnesi.record().indexOf(polja[i])).toString() + "'");
						vnesi_projekt.exec();
						if ( vnesi_projekt.next() ) {
							celica->setText(prevedi(vnesi_projekt.value(vnesi_projekt.record().indexOf("naslov_projekta")).toString()));
						}
					}
					else if ( polja[i] == "stevilka_racuna" ) {
						QSqlQuery vnesi_racun;
						vnesi_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + vnesi.value(vnesi.record().indexOf(polja[i])).toString() + "'");
						vnesi_racun.exec();
						if ( vnesi_racun.next() ) {
							celica->setText(prevedi(vnesi_racun.value(vnesi_racun.record().indexOf("stevilka_racuna")).toString()));
						}
					}
					else if ( polja[i] == "opravilo" ) {
						QString naslov = "";

						if ( vnesi.value(vnesi.record().indexOf("opravilo_skupina")).toString() == "" ||
								 vnesi.value(vnesi.record().indexOf("opravilo_skupina")).toString() == pretvori("Ostalo") ) {
							naslov = prevedi(vnesi.value(vnesi.record().indexOf("opravilo_rocno")).toString());
						}
						else {
							naslov = prevedi(vnesi.value(vnesi.record().indexOf("opravilo_skupina")).toString()) + " - ";

							if ( vnesi.value(vnesi.record().indexOf("opravilo_storitev")).toString() == "" ||
									 vnesi.value(vnesi.record().indexOf("opravilo_storitev")).toString() == pretvori("Ostalo") ) {
								naslov += prevedi(vnesi.value(vnesi.record().indexOf("opravilo_rocno")).toString());
							}
							else {
								naslov += prevedi(vnesi.value(vnesi.record().indexOf("opravilo_storitev")).toString());
							}
						}

						celica->setText(naslov);
					}
					else if ( polja[i] == "zdruzek" ) {
						QString seznam_casovnic = vnesi.value(vnesi.record().indexOf("casovnice")).toString();
						int max_casovnice = seznam_casovnic.count(";");

						// za vsako casovnico na seznamu vnesi njeno vrednost v ustrezno polje
						for ( int b = 1; b <= max_casovnice; b++ ) {
							QString del_seznama = seznam_casovnic.left(seznam_casovnic.indexOf(";"));
							seznam_casovnic = seznam_casovnic.right(seznam_casovnic.length() - seznam_casovnic.indexOf(";") - 1);

							// del seznama razbijemo na datum in vrednost
							QString datum = del_seznama.left(del_seznama.indexOf(","));
							QString vrednost = pretvori_iz_double(del_seznama.right(del_seznama.length() - del_seznama.indexOf(",") - 1));

							// datum razbijemo na leto, mesec in dan
							QString leto = datum.left(4);
							QString mesec = datum.right(6).left(2);

							if ( ui->txt_leto->currentText() == leto && ui->txt_mesec->currentText().left(2) == mesec ) {
								d_mesec += pretvori_v_double(vrednost).toDouble();
							}
							d_skupaj += pretvori_v_double(vrednost).toDouble();

						}

						QTableWidgetItem *t_mesec = new QTableWidgetItem;
						t_mesec->setText(QString::number(d_mesec, 'f', 2));
						ui->tbl_casovnice->setItem(row, 4, t_mesec);

						QTableWidgetItem *t_skupaj = new QTableWidgetItem;
						t_skupaj->setText(QString::number(d_skupaj, 'f', 2));
						ui->tbl_casovnice->setItem(row, 5, t_skupaj);

						celica->setText("");
					}
					else if ( polja[i] == "casovnice" ) {
						QString seznam_casovnic = vnesi.value(vnesi.record().indexOf(polja[i])).toString();
						int max_casovnice = seznam_casovnic.count(";");

						// za vsako casovnico na seznamu vnesi njeno vrednost v ustrezno polje
						for ( int b = 1; b <= max_casovnice; b++ ) {
							QString del_seznama = seznam_casovnic.left(seznam_casovnic.indexOf(";"));
							seznam_casovnic = seznam_casovnic.right(seznam_casovnic.length() - seznam_casovnic.indexOf(";") - 1);

							// del seznama razbijemo na datum in vrednost
							QString datum = del_seznama.left(del_seznama.indexOf(","));
							QString vrednost = pretvori_iz_double(del_seznama.right(del_seznama.length() - del_seznama.indexOf(",") - 1));

							// datum razbijemo na leto, mesec in dan
							QString leto = datum.left(4);
							QString mesec = datum.right(6).left(2);
							QString dan = datum.right(3).left(2);

							if ( ui->txt_leto->currentText() == leto && ui->txt_mesec->currentText().left(2) == mesec ) {
								QTableWidgetItem *polje_v_tabeli = new QTableWidgetItem;
								polje_v_tabeli->setText(vrednost);
								ui->tbl_casovnice->setItem(row, dan.toInt() + 5, polje_v_tabeli);
							}
						}
						celica->setText("");
					}
					else {
						celica->setText(vnesi.value(vnesi.record().indexOf(polja[i])).toString());
					}
					if ( celica->text() != "" ) {
						ui->tbl_casovnice->setItem(row, col, celica);
					}

					col++;
					i++;

				}

				row++;
			}
		}
	}
	base.close();

	ui->cb_aktivnost->setText(cb_aktivnost_besedilo);

}

void wid_casovnice::preracunaj_casovnice(QString opravilo) {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "preracunaj_casovnice");
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

		double sestevek_ur = 0.0;

		QSqlQuery sql_opravilo;
		sql_opravilo.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(opravilo) + "' AND tip_ur LIKE '" + pretvori("casovnice") + "'");
		sql_opravilo.exec();
		if ( sql_opravilo.next() ) {
			QString casovnice = sql_opravilo.value(sql_opravilo.record().indexOf("casovnice")).toString();

			// prestejemo vse zapise v tej casovnici
			int max_casovnic = casovnice.count(";");

			// ce je to nasa prva casovnica, zakljucimo vnos
			// ce obstajajo se druge casovnice, jih dodamo v nas seznam
			if ( max_casovnic != 0 ) { // ce casovnice ze obstajajo nadaljujemo
				for ( int i = 0; i <= max_casovnic; i++ ) { // vsako ze obstojeco casovnico razbijemo

				// v seznam shranimo trenutno casovnico
					QString seznam = casovnice.left(casovnice.indexOf(";"));

				// v casovnice shranimo preostanek za naslednji cikelj
					casovnice = casovnice.right(casovnice.length() - casovnice.indexOf(";") - 1);

				// tenutno casovnico (seznam) razbijemo na datum in vrednost
					QString seznam_datum = seznam.left(seznam.indexOf(","));
					QString seznam_vrednost = pretvori_v_double(seznam.right(seznam.length() - seznam.indexOf(",") - 1));

				// pristejemo trenutno vrednost h preteklim
					sestevek_ur += pretvori_v_double(seznam_vrednost).toDouble();
				}
			}

			// preracunaj in posodobi polja v tabeli opravil
			double urna_postavka = pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("urna_postavka_brez_ddv")).toString()).toDouble();
			double stopnja_ddv = pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("ddv")).toString()).toDouble();

			double pribitek = 0.0;

			double znesek_popustov = 0.0;
			double znesek_brez_ddv = 0.0;
			double znesek_ddv = 0.0;
			double znesek = 0.0;

			// izracunaj skupni znesek popustov
			double popusti_skupaj = 0.0;
			popusti_skupaj += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("popust_fb1")).toString()).toDouble();
			popusti_skupaj += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("popust_fb2")).toString()).toDouble();
			popusti_skupaj += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("popust_komb1")).toString()).toDouble();
			popusti_skupaj += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("popust_komb2")).toString()).toDouble();
			popusti_skupaj += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("popust_stranka")).toString()).toDouble();
			popusti_skupaj += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("popust_kupon")).toString()).toDouble();
			popusti_skupaj += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("popust_akcija")).toString()).toDouble();

			// pribitki
			pribitek += 1.0;
			pribitek += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("podrazitev_vikend")).toString()).toDouble() *
									pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("pribitek_vikend")).toString()).toDouble() / 100;
			pribitek += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("podrazitev_hitrost")).toString()).toDouble() *
									pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("pribitek_hitrost")).toString()).toDouble() / 100;
			pribitek += pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("podrazitev_zapleti")).toString()).toDouble() *
									pretvori_v_double(sql_opravilo.value(sql_opravilo.record().indexOf("pribitek_zapleti")).toString()).toDouble() / 100;

			// izracun zneskov
			znesek_brez_ddv = urna_postavka * sestevek_ur * (1 - popusti_skupaj / 100 ) * pribitek;
			znesek_popustov = urna_postavka * sestevek_ur * popusti_skupaj / 100;
			znesek_ddv = znesek_brez_ddv * stopnja_ddv / 100 ;
			znesek = znesek_brez_ddv + znesek_ddv;

			// vnesi podatke v bazo
			QSqlQuery sql_vnesi;
			sql_vnesi.prepare("UPDATE opravila SET ur_dela = ?, znesek_popustov = ?, znesek_ddv = ?, znesek_koncni = ? "
												"WHERE id LIKE '" + opravilo + "'");
			sql_vnesi.bindValue(0, pretvori(pretvori_v_double(QString::number(sestevek_ur, 'f', 1))));
			sql_vnesi.bindValue(1, pretvori(pretvori_v_double(QString::number(znesek_popustov, 'f', 2))));
			sql_vnesi.bindValue(2, pretvori(pretvori_v_double(QString::number(znesek_ddv, 'f', 2))));
			sql_vnesi.bindValue(3, pretvori(pretvori_v_double(QString::number(znesek_brez_ddv, 'f', 2))));
			sql_vnesi.exec();
		}
	}
	base.close();

}

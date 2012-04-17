#include <QtSql>
#include <QMessageBox>
#include <QDate>
#include <QSortFilterProxyModel>

#include "wid_prihodki.h"
#include "ui_wid_prihodki.h"
#include "kodiranje.h"

wid_prihodki::wid_prihodki(QWidget *parent) :
    QWidget(parent),
		ui(new Ui::wid_prihodki)
{
    ui->setupUi(this);

		// ocisti vnosna polja

		ui->txt_leto->clear();
		ui->txt_mesec->clear();

		ui->txt_leto_3->clear();

		ui->txt_skupaj_znesek->setText("");
		ui->txt_skupaj_odstotek->setText("");
		ui->txt_fizicne_osebe_znesek->setText("");
		ui->txt_fizicne_osebe_odstotek->setText("");
		ui->txt_pravne_osebe_znesek->setText("");
		ui->txt_pravne_osebe_odstotek->setText("");

		ui->txt_skupaj_znesek_3->setText("");
		ui->txt_skupaj_odstotek_3->setText("");
		ui->txt_fizicne_osebe_znesek_3->setText("");
		ui->txt_fizicne_osebe_odstotek_3->setText("");
		ui->txt_pravne_osebe_znesek_3->setText("");
		ui->txt_pravne_osebe_odstotek_3->setText("");

		ui->txt_skupaj_znesek_4->setText("");
		ui->txt_skupaj_odstotek_4->setText("");
		ui->txt_fizicne_osebe_znesek_4->setText("");
		ui->txt_fizicne_osebe_odstotek_4->setText("");
		ui->txt_pravne_osebe_znesek_4->setText("");
		ui->txt_pravne_osebe_odstotek_4->setText("");

		ui->cb_vsa_opravila->setText("");
		ui->cb_vsa_opravila_3->setText("");
		ui->cb_vsa_opravila_4->setText("");

		ui->txt_sklop->clear();
		ui->txt_skupina->clear();
		ui->txt_storitev->clear();

		ui->txt_sklop_3->clear();
		ui->txt_skupina_3->clear();
		ui->txt_storitev_3->clear();

		ui->txt_sklop_4->clear();
		ui->txt_skupina_4->clear();
		ui->txt_storitev_4->clear();

		ui->tbl_storitve->clear();

		ui->tbl_storitve_3->clear();

		ui->tbl_storitve_4->clear();

		// napolni spustne sezname
		QStringList seznam_mesecev;
		seznam_mesecev << ""
									 << "01) Januar"
									 << "02) Februar"
									 << "03) Marec"
									 << "04) April"
									 << "05) Maj"
									 << "06) Junij"
									 << "07) Julij"
									 << "08) Avgust"
									 << "09) September"
									 << "10) Oktober"
									 << "11) November"
									 << "12) December";
		ui->txt_mesec->addItems(seznam_mesecev);

		ui->txt_leto->addItem("");
		ui->txt_leto_3->addItem("");
		ui->txt_sklop->addItem("");
		ui->txt_sklop_3->addItem("");
		ui->txt_sklop_4->addItem("");

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-seznamov");
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

			QSqlQuery sql_leto;
			sql_leto.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND status_oddaje_racuna NOT LIKE ''");
			sql_leto.exec();
			while ( sql_leto.next() ) {
				QString leto = prevedi(sql_leto.value(sql_leto.record().indexOf("datum_izdaje")).toString()).right(4);
				if ( ui->txt_leto->findText(leto) == -1 ) {
					ui->txt_leto->addItem(leto);
					ui->txt_leto_3->addItem(leto);
				}
			}

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM sif_storitve");
			sql_opravila.exec();
			while ( sql_opravila.next() ) {
				if ( ui->txt_sklop->findText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("sklop")).toString())) == -1 ) {
					ui->txt_sklop->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("sklop")).toString()));
					ui->txt_sklop_3->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("sklop")).toString()));
					ui->txt_sklop_4->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("sklop")).toString()));
				}
			}

		}
		base.close();

		// razvrscanje let po vrsti
		QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->txt_leto);
		proxy->setSourceModel(ui->txt_leto->model());
		// spustni seznam prepisemo
		ui->txt_leto->model()->setParent(proxy);
		ui->txt_leto->setModel(proxy);
		// razvrsti
		ui->txt_leto->model()->sort(0);

		QSortFilterProxyModel* proxy_1 = new QSortFilterProxyModel(ui->txt_leto_3);
		proxy_1->setSourceModel(ui->txt_leto_3->model());
		// spustni seznam prepisemo
		ui->txt_leto_3->model()->setParent(proxy_1);
		ui->txt_leto_3->setModel(proxy_1);
		// razvrsti
		ui->txt_leto_3->model()->sort(0);

		ui->txt_leto->setCurrentIndex(ui->txt_leto->findText(QDate::currentDate().toString("yyyy")));
		ui->txt_leto_3->setCurrentIndex(ui->txt_leto->findText(QDate::currentDate().toString("yyyy")));
		ui->txt_mesec->setCurrentIndex(ui->txt_mesec->findText(QDate::currentDate().toString("MM") + ") ", Qt::MatchStartsWith));

		ui->cb_vsa_opravila->setChecked(true);
		ui->cb_vsa_opravila_3->setChecked(true);
		ui->cb_vsa_opravila_4->setChecked(true);

		ui->tab_prihodki->setCurrentIndex(0);

		napolni_mesec();
		napolni_letni();
		napolni_skupni();

}

wid_prihodki::~wid_prihodki()
{
    delete ui;
}

void wid_prihodki::on_txt_leto_currentIndexChanged() {

	napolni_mesec();

}

void wid_prihodki::on_txt_mesec_currentIndexChanged() {

	napolni_mesec();

}

void wid_prihodki::on_txt_sklop_currentIndexChanged() {

	// spraznimo skupine in storitve
	ui->txt_skupina->clear();
	ui->txt_storitev->clear();

	// dodamo prazno vrstico
	ui->txt_skupina->addItem("");

	if ( ui->txt_sklop->currentText() != "" ) {
		// napolnimo skupine
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-skupin-mesecni");
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

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM sif_storitve WHERE sklop LIKE '" + pretvori(ui->txt_sklop->currentText()) + "'");
			sql_opravila.exec();
			while ( sql_opravila.next() ) {
				if ( ui->txt_skupina->findText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("skupina")).toString())) == -1 ) {
					ui->txt_skupina->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("skupina")).toString()));
				}
			}

		}
		base.close();

	}

	// napolnimo tabelo
	napolni_mesec_tabela();

}

void wid_prihodki::on_txt_skupina_currentIndexChanged() {

	// sprazni spustni seznam
	ui->txt_storitev->clear();

	// dodaj prazno vrstico
	ui->txt_storitev->addItem("");

	if ( ui->txt_skupina->currentText() != "" ) {
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-storitev-mesecni");
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

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina->currentText()) + "'");
			sql_opravila.exec();
			while ( sql_opravila.next() ) {
				if ( ui->txt_storitev->findText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("storitev")).toString())) == -1 ) {
					ui->txt_storitev->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("storitev")).toString()));
				}
			}

		}
		base.close();
	}

	// napolni tabelo
	napolni_mesec_tabela();

}

void wid_prihodki::on_txt_storitev_currentIndexChanged() {

	// napolni tabelo
	napolni_mesec_tabela();

}

void wid_prihodki::on_cb_vsa_opravila_toggled() {

	// izprazni spustne sezname
	ui->txt_sklop->setCurrentIndex(0);
	ui->txt_skupina->setCurrentIndex(0);
	ui->txt_storitev->setCurrentIndex(0);

	if ( ui->cb_vsa_opravila->isChecked() ) {
		// onemogoci spustne sezname
		ui->txt_sklop->setEnabled(false);
		ui->txt_skupina->setEnabled(false);
		ui->txt_storitev->setEnabled(false);
	}
	else {
		// omogoci spustne sezname
		ui->txt_sklop->setEnabled(true);
		ui->txt_skupina->setEnabled(true);
		ui->txt_storitev->setEnabled(true);
	}

	// napolni tabelo
	napolni_mesec_tabela();

}

// letni pregled
void wid_prihodki::on_txt_leto_3_currentIndexChanged() {

	napolni_letni();

}

void wid_prihodki::on_txt_sklop_3_currentIndexChanged() {

	// spraznimo skupine in storitve
	ui->txt_skupina_3->clear();
	ui->txt_storitev_3->clear();

	// dodamo prazno vrstico
	ui->txt_skupina_3->addItem("");

	if ( ui->txt_sklop_3->currentText() != "" ) {
		// napolnimo skupine
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-skupin-letni");
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

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM sif_storitve WHERE sklop LIKE '" + pretvori(ui->txt_sklop_3->currentText()) + "'");
			sql_opravila.exec();
			while ( sql_opravila.next() ) {
				if ( ui->txt_skupina_3->findText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("skupina")).toString())) == -1 ) {
					ui->txt_skupina_3->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("skupina")).toString()));
				}
			}

		}
		base.close();

	}

	// napolnimo tabelo
	napolni_letni_tabela();

}

void wid_prihodki::on_txt_skupina_3_currentIndexChanged() {

	// sprazni spustni seznam
	ui->txt_storitev_3->clear();

	// dodaj prazno vrstico
	ui->txt_storitev_3->addItem("");

	if ( ui->txt_skupina_3->currentText() != "" ) {
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-storitev-letni");
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

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina_3->currentText()) + "'");
			sql_opravila.exec();
			while ( sql_opravila.next() ) {
				if ( ui->txt_storitev_3->findText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("storitev")).toString())) == -1 ) {
					ui->txt_storitev_3->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("storitev")).toString()));
				}
			}

		}
		base.close();

	}

	// napolni tabelo
	napolni_letni_tabela();

}

void wid_prihodki::on_txt_storitev_3_currentIndexChanged() {

	// napolni tabelo
	napolni_letni_tabela();

}

void wid_prihodki::on_cb_vsa_opravila_3_toggled() {

	// izprazni spustne sezname
	ui->txt_sklop_3->setCurrentIndex(0);
	ui->txt_skupina_3->setCurrentIndex(0);
	ui->txt_storitev_3->setCurrentIndex(0);

	if ( ui->cb_vsa_opravila_3->isChecked() ) {
		// onemogoci spustne sezname
		ui->txt_sklop_3->setEnabled(false);
		ui->txt_skupina_3->setEnabled(false);
		ui->txt_storitev_3->setEnabled(false);
	}
	else {
		// omogoci spustne sezname
		ui->txt_sklop_3->setEnabled(true);
		ui->txt_skupina_3->setEnabled(true);
		ui->txt_storitev_3->setEnabled(true);
	}

	// napolni tabelo
	napolni_letni_tabela();

}

// skupni pregled
void wid_prihodki::on_txt_sklop_4_currentIndexChanged() {

	// spraznimo skupine in storitve
	ui->txt_skupina_4->clear();
	ui->txt_storitev_4->clear();

	// dodamo prazno vrstico
	ui->txt_skupina_4->addItem("");

	if ( ui->txt_sklop_4->currentText() != "" ) {
		// napolnimo skupine
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-skupin-skupni");
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

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM sif_storitve WHERE sklop LIKE '" + pretvori(ui->txt_sklop_4->currentText()) + "'");
			sql_opravila.exec();
			while ( sql_opravila.next() ) {
				if ( ui->txt_skupina_4->findText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("skupina")).toString())) == -1 ) {
					ui->txt_skupina_4->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("skupina")).toString()));
				}
			}

		}
		base.close();

	}

	// napolnimo tabelo
	napolni_skupni_tabela();

}

void wid_prihodki::on_txt_skupina_4_currentIndexChanged() {

	// sprazni spustni seznam
	ui->txt_storitev_4->clear();

	// dodaj prazno vrstico
	ui->txt_storitev_4->addItem("");

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	if ( ui->txt_skupina_4->currentText() != "" ) {
		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-storitev-skupni");
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

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina_4->currentText()) + "'");
			sql_opravila.exec();
			while ( sql_opravila.next() ) {
				if ( ui->txt_storitev_4->findText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("storitev")).toString())) == -1 ) {
					ui->txt_storitev_4->addItem(prevedi(sql_opravila.value(sql_opravila.record().indexOf("storitev")).toString()));
				}
			}

		}
		base.close();

	}

	// napolni tabelo
	napolni_skupni_tabela();

}

void wid_prihodki::on_txt_storitev_4_currentIndexChanged() {

	// napolni tabelo
	napolni_skupni_tabela();

}

void wid_prihodki::on_cb_vsa_opravila_4_toggled() {

	// izprazni spustne sezname
	ui->txt_sklop_4->setCurrentIndex(0);
	ui->txt_skupina_4->setCurrentIndex(0);
	ui->txt_storitev_4->setCurrentIndex(0);

	if ( ui->cb_vsa_opravila_4->isChecked() ) {
		// onemogoci spustne sezname
		ui->txt_sklop_4->setEnabled(false);
		ui->txt_skupina_4->setEnabled(false);
		ui->txt_storitev_4->setEnabled(false);
	}
	else {
		// omogoci spustne sezname
		ui->txt_sklop_4->setEnabled(true);
		ui->txt_skupina_4->setEnabled(true);
		ui->txt_storitev_4->setEnabled(true);
	}

	// napolni tabelo
	napolni_skupni_tabela();

}

// polnenje tabel

// napolni mesecni pregled
void wid_prihodki::napolni_mesec() {

	double znesek_avans = 0.0;
	double znesek_avans_fizicne = 0.0;
	double znesek_avans_pravne = 0.0;

	double znesek_ostanek = 0.0;
	double znesek_ostanek_fizicne = 0.0;
	double znesek_ostanek_pravne = 0.0;

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-podatkov-mesecni");
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

		// poisci, kateri avansi so bili placani v danem mesecu
		QSqlQuery sql_avans;
		sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND datum_placila_avansa LIKE '%." +
										 pretvori(ui->txt_mesec->currentText().left(2) + "." + ui->txt_leto->currentText()) + "'");
		sql_avans.exec();
		while ( sql_avans.next() ) {
			znesek_avans += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();

			// razvrsti placila glede na pravne in fizicne osebe
			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_avans.value(sql_avans.record().indexOf("stranka")).toString() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "1" ) {
					znesek_avans_fizicne += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
				}
				else if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "2" ) {
					znesek_avans_pravne += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
				}
			}
		}

		// poisci preostanek placila
		QSqlQuery sql_preostanek;
		sql_preostanek.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
													 " AND datum_placila LIKE '%." + pretvori(ui->txt_mesec->currentText().left(2) + "." + ui->txt_leto->currentText()) + "'"
													 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
		sql_preostanek.exec();
		while ( sql_preostanek.next() ) {
			QSqlQuery sql_opravilo;
			sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
			sql_opravilo.exec();
			while ( sql_opravilo.next() ) {
				znesek_ostanek += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
													pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
			}
			sql_opravilo.clear();
			// odstejemo ze placan avans
			znesek_ostanek -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();

			// razvrsti placila glede na pravne in fizicne osebe
			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("stranka")).toString() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "1" ) {
					sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
					sql_opravilo.exec();
					while ( sql_opravilo.next() ) {
						znesek_ostanek_fizicne += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
																			pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
					}
					sql_opravilo.clear();
					// odstejemo ze placan avans
					znesek_ostanek_fizicne -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
				}
				else if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "2" ) {
					sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
					sql_opravilo.exec();
					while ( sql_opravilo.next() ) {
						znesek_ostanek_pravne += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
																		 pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
					}
					sql_opravilo.clear();
					// odstejemo ze placan avans
					znesek_ostanek_pravne -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
				}
			}
		}

	}
	base.close();

	double znesek_skupaj = znesek_avans + znesek_ostanek;
	double znesek_skupaj_fizicne = znesek_avans_fizicne + znesek_ostanek_fizicne;
	double znesek_skupaj_pravne = znesek_avans_pravne + znesek_ostanek_pravne;

	ui->txt_skupaj_znesek->setText(pretvori_iz_double(QString::number(znesek_skupaj, 'f', 2)) + " EUR");
	ui->txt_fizicne_osebe_znesek->setText(pretvori_iz_double(QString::number(znesek_skupaj_fizicne, 'f', 2)) + " EUR");
	ui->txt_pravne_osebe_znesek->setText(pretvori_iz_double(QString::number(znesek_skupaj_pravne, 'f', 2)) + " EUR");

	if ( znesek_skupaj > 0.0 ) {
		ui->txt_skupaj_odstotek->setText(pretvori_iz_double(QString::number(znesek_skupaj / znesek_skupaj * 100, 'f', 2)) + " %");
		ui->txt_fizicne_osebe_odstotek->setText(pretvori_iz_double(QString::number(znesek_skupaj_fizicne / znesek_skupaj * 100, 'f', 2)) + " %");
		ui->txt_pravne_osebe_odstotek->setText(pretvori_iz_double(QString::number(znesek_skupaj_pravne / znesek_skupaj * 100, 'f', 2)) + " %");
	}
	else {
		ui->txt_skupaj_odstotek->setText("");
		ui->txt_fizicne_osebe_odstotek->setText("");
		ui->txt_pravne_osebe_odstotek->setText("");
	}

	napolni_mesec_tabela();

}

void wid_prihodki::napolni_mesec_tabela() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-tabela-storitev-mesecni");
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
		ui->tbl_storitve->clear();

		for (int i = 0; i <= 6; i++) {
			ui->tbl_storitve->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM sif_storitve");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_storitve->removeRow(0);
		}

		// start filling the table
		ui->tbl_storitve->insertColumn(0);
		ui->tbl_storitve->insertColumn(1);
		ui->tbl_storitve->insertColumn(2);
		ui->tbl_storitve->insertColumn(3);
		ui->tbl_storitve->insertColumn(4);
		ui->tbl_storitve->insertColumn(5);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Sklop");
		naslov2->setText("Skupina");
		naslov3->setText("Storitev");
		naslov4->setText("Oddelanih ur");
		naslov5->setText("Prihodek");

		ui->tbl_storitve->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_storitve->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_storitve->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_storitve->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_storitve->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_storitve->setHorizontalHeaderItem(5, naslov5);

		ui->tbl_storitve->setColumnWidth(0, 35);

		/**
			* 1) poiscemo racune, ki spadajo v dani okvir
			* 2) shranimo id-je racunov v seznam
			* 3) glede na spustne sezname dolocimo opravila, ki jih je potrebno prikazati
			* 4) poiscemo tem racunom pripadajoca opravila
			* 5) razvrstimo opravljena opravila glede na prikazana opravila
			* 6) sestejemo ure in zneske opravljenih opravil
			* 7) sestevke prikazemo v tabeli
			**/

		// 1) poiscemo racune in jih 2) shranimo v seznam
		QStringList seznam_id_racunov;
		QSqlQuery sql_racuni;
		sql_racuni.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
											 " AND datum_placila LIKE '%." + pretvori(ui->txt_mesec->currentText().left(2) + "." + ui->txt_leto->currentText()) + "'"
											 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
		sql_racuni.exec();
		while ( sql_racuni.next() ) {
			seznam_id_racunov.append(prevedi(sql_racuni.value(sql_racuni.record().indexOf("id")).toString()));
		}

		// 3) glede na spustne sezname dolocimo opravila, ki jih je potrebno prikazati
		QSqlQuery sql_storitve; // za vse situacije

		// prikazi vse storitve
		if ( ui->cb_vsa_opravila->isChecked() ) {
			sql_storitve.prepare("SELECT * FROM sif_storitve");
		}
		else {
			if ( ui->txt_storitev->currentText() != "" ) { // imamo izbrano storitev, torej prikazemo samo to storitev
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE storitev LIKE '" + pretvori(ui->txt_storitev->currentText()) + "'");
			}
			else if ( ui->txt_skupina->currentText() != "" ) { // imamo izbrano skupino, prikazemo vse storitve v tej skupini
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina->currentText()) + "'");
			}
			else if ( ui->txt_sklop->currentText() != "" ) { // imamo izbran sklop, prikazemo vse skupine v tem sklopu, ne pa storitev
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE sklop LIKE '" + pretvori(ui->txt_sklop->currentText()) + "'");
			}
			else { // nic ni izbrano, prikazemo vse sklope
				sql_storitve.prepare("SELECT * FROM sif_storitve");
			}
		}

		sql_storitve.exec(); // izvrsi zgodnji sql stavek

		// prikazi storitve v tabeli
		// ce so samo sklopi, skrij storitve in skupine
		// ce so samo skupine, skrij storitve
		// za vsako storitev, skupino, sklop sestej opravila, ki jih ima posamezen id
		// prikazemo ure in zneske

		// nastavimo st. vrstice in st. stolpca
		int row = 0;
		QString stavek_sestevek = "";
		while ( sql_storitve.next() ) {
			QString preverjanje = ""; // ce string ni prazen, potem vnesemo vrstico

			if ( ui->cb_vsa_opravila->isChecked() ) {	// prikaze vse storitve, ki so v bazi
				preverjanje = "polno";
				ui->tbl_storitve->setColumnHidden(0, false); // ID
				ui->tbl_storitve->setColumnHidden(1, false); // sklop
				ui->tbl_storitve->setColumnHidden(2, false); // skupina
				ui->tbl_storitve->setColumnHidden(3, false); // storitev
				ui->tbl_storitve->setColumnHidden(4, false); // ure
				ui->tbl_storitve->setColumnHidden(5, false); // znesek

				// pripravi sql stavek za primer prikaza vseh storitev
				stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
											 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
											 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
			}
			else {
				if ( ui->txt_storitev->currentText() != "" ) { // imamo izbrano storitev, torej prikazemo samo to storitev
					if ( ui->txt_storitev->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()) ) {
						if ( ui->txt_skupina->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()) ) {
							if ( ui->txt_sklop->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
								preverjanje = "polno";
								ui->tbl_storitve->setColumnHidden(0, false); // ID
								ui->tbl_storitve->setColumnHidden(1, false); // sklop
								ui->tbl_storitve->setColumnHidden(2, false); // skupina
								ui->tbl_storitve->setColumnHidden(3, false); // storitev
								ui->tbl_storitve->setColumnHidden(4, false); // ure
								ui->tbl_storitve->setColumnHidden(5, false); // znesek

								// pripravi sql stavek za primer prikaza izbrane storitve
								stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
															 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
															 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
							}
						}
					}
				}
				else if ( ui->txt_skupina->currentText() != "" ) { // imamo izbrano skupino, prikazemo vse storitve v tej skupini
					if ( ui->tbl_storitve->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()), Qt::MatchExactly).count() < 1 ) {
						if ( ui->txt_skupina->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()) ) {
							if ( ui->txt_sklop->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
								preverjanje = "polno";
								ui->tbl_storitve->setColumnHidden(0, false); // ID
								ui->tbl_storitve->setColumnHidden(1, false); // sklop
								ui->tbl_storitve->setColumnHidden(2, false); // skupina
								ui->tbl_storitve->setColumnHidden(3, false); // storitev
								ui->tbl_storitve->setColumnHidden(4, false); // ure
								ui->tbl_storitve->setColumnHidden(5, false); // znesek

								// pripravi sql stavek za primer prikaza vseh storitev v izbrani skupini
								stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
															 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
															 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
							}
						}
					}
				}
				else if ( ui->txt_sklop->currentText() != "" ) { // imamo izbran sklop, prikazemo vse skupine v tem sklopu, ne pa storitev
					if ( ui->tbl_storitve->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()), Qt::MatchExactly).count() < 1 ) {
						if ( ui->txt_sklop->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
							preverjanje = "polno";
							ui->tbl_storitve->setColumnHidden(0, false); // ID
							ui->tbl_storitve->setColumnHidden(1, false); // sklop
							ui->tbl_storitve->setColumnHidden(2, false); // skupina
							ui->tbl_storitve->setColumnHidden(3, true); // storitev
							ui->tbl_storitve->setColumnHidden(4, false); // ure
							ui->tbl_storitve->setColumnHidden(5, false); // znesek

							// pripravi sql stavek za primer prikaza vseh skupin
							stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
														 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "'";

						}
					}
				}
				else { // nic ni izbrano, prikazemo vse sklope
					if ( ui->tbl_storitve->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()), Qt::MatchExactly).count() < 1 ) {
						preverjanje = "polno";
						ui->tbl_storitve->setColumnHidden(0, false); // ID
						ui->tbl_storitve->setColumnHidden(1, false); // sklop
						ui->tbl_storitve->setColumnHidden(2, true); // skupina
						ui->tbl_storitve->setColumnHidden(3, true); // storitev
						ui->tbl_storitve->setColumnHidden(4, false); // ure
						ui->tbl_storitve->setColumnHidden(5, false); // znesek

						// pripravi sql stavek za primer prikaza vseh sklopov
						stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "'";
					}
				}
			}

		//	if ( ui->tbl_storitve->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()), Qt::MatchExactly).count() < 1 ) {
		//		preverjanje = "polno";
		//	}

			if ( preverjanje != "" ) {
				ui->tbl_storitve->insertRow(row);
				ui->tbl_storitve->setRowHeight(row, 20);

				int col = 0;
				QString polja[5] = {"id", "sklop", "skupina", "storitev", "racun"};

				while ( col < 5 ) {
					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[col] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_storitve.value(sql_storitve.record().indexOf(polja[col])).toString()).toInt());
					}
					else {
						celica->setText(prevedi(sql_storitve.value(sql_storitve.record().indexOf(polja[col])).toString()));
					}
					ui->tbl_storitve->setItem(row, col, celica);
					col++;
				}

				// sestej vsa opravila v danem mesecu (ki so del predhodno izbranih racunov), katerih storitev pripada dani vrstici
				double sestevek_ure = 0.0;
				double sestevek_znesek = 0.0;

				// pojdi skozi vsa opravila pri predhodno izbranih racunih in pri vsakem poisci storitev, ki jo trenutno potrebujemo
				for ( int a = 0; a < seznam_id_racunov.count(); a++ ) {
					QSqlQuery sql_sestevek;
					sql_sestevek.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + seznam_id_racunov.value(a) + "' " + stavek_sestevek);
					sql_sestevek.exec();

					while ( sql_sestevek.next() ) {

						sestevek_ure += prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("ur_dela")).toString()).toDouble();
						sestevek_znesek += prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("znesek_koncni")).toString()).toDouble() +
															 prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("znesek_ddv")).toString()).toDouble();
					}

					sql_sestevek.clear();

				}

				QTableWidgetItem *ure = new QTableWidgetItem;
				ure->setText(QString::number(sestevek_ure, 'f', 2) + " ur");
				ui->tbl_storitve->setItem(row, 4, ure);

				QTableWidgetItem *znesek = new QTableWidgetItem;
				znesek->setText(QString::number(sestevek_znesek, 'f', 2) + " EUR");
				ui->tbl_storitve->setItem(row, 5, znesek);

				// skrij polja, kjer je stevilo opravljenih ur 0
				if ( sestevek_ure == 0.0 ) {
					ui->tbl_storitve->setRowHidden(row, true);
				}
				else {
					ui->tbl_storitve->setRowHidden(row, false);
				}

				row++;
			}

		}

	}

	base.close();

}

// napolni letni pregled
void wid_prihodki::napolni_letni() {

	double znesek_avans = 0.0;
	double znesek_avans_fizicne = 0.0;
	double znesek_avans_pravne = 0.0;

	double znesek_ostanek = 0.0;
	double znesek_ostanek_fizicne = 0.0;
	double znesek_ostanek_pravne = 0.0;

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-podatkov-letni");
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

		// poisci, kateri avansi so bili placani v danem mesecu
		QSqlQuery sql_avans;
		sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND datum_placila_avansa LIKE '%." +
										 pretvori(ui->txt_leto_3->currentText()) + "'");
		sql_avans.exec();
		while ( sql_avans.next() ) {
			znesek_avans += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();

			// razvrsti placila glede na pravne in fizicne osebe
			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_avans.value(sql_avans.record().indexOf("stranka")).toString() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "1" ) {
					znesek_avans_fizicne += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
				}
				else if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "2" ) {
					znesek_avans_pravne += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
				}
			}
		}

		// poisci preostanek placila
		QSqlQuery sql_preostanek;
		sql_preostanek.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
													 " AND datum_placila LIKE '%." + pretvori(ui->txt_leto_3->currentText()) + "'"
													 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
		sql_preostanek.exec();
		while ( sql_preostanek.next() ) {
			QSqlQuery sql_opravilo;
			sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
			sql_opravilo.exec();
			while ( sql_opravilo.next() ) {
				znesek_ostanek += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
													pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
			}
			sql_opravilo.clear();
			// odstejemo ze placan avans
			znesek_ostanek -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();

			// razvrsti placila glede na pravne in fizicne osebe
			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("stranka")).toString() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "1" ) {
					sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
					sql_opravilo.exec();
					while ( sql_opravilo.next() ) {
						znesek_ostanek_fizicne += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
																			pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
					}
					sql_opravilo.clear();
					// odstejemo ze placan avans
					znesek_ostanek_fizicne -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
				}
				else if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "2" ) {
					sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
					sql_opravilo.exec();
					while ( sql_opravilo.next() ) {
						znesek_ostanek_pravne += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
																		 pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
					}
					sql_opravilo.clear();
					// odstejemo ze placan avans
					znesek_ostanek_pravne -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
				}
			}
		}

	}
	base.close();

	double znesek_skupaj = znesek_avans + znesek_ostanek;
	double znesek_skupaj_fizicne = znesek_avans_fizicne + znesek_ostanek_fizicne;
	double znesek_skupaj_pravne = znesek_avans_pravne + znesek_ostanek_pravne;

	ui->txt_skupaj_znesek_3->setText(pretvori_iz_double(QString::number(znesek_skupaj, 'f', 2)) + " EUR");
	ui->txt_fizicne_osebe_znesek_3->setText(pretvori_iz_double(QString::number(znesek_skupaj_fizicne, 'f', 2)) + " EUR");
	ui->txt_pravne_osebe_znesek_3->setText(pretvori_iz_double(QString::number(znesek_skupaj_pravne, 'f', 2)) + " EUR");

	if ( znesek_skupaj > 0.0 ) {
		ui->txt_skupaj_odstotek_3->setText(pretvori_iz_double(QString::number(znesek_skupaj / znesek_skupaj * 100, 'f', 2)) + " %");
		ui->txt_fizicne_osebe_odstotek_3->setText(pretvori_iz_double(QString::number(znesek_skupaj_fizicne / znesek_skupaj * 100, 'f', 2)) + " %");
		ui->txt_pravne_osebe_odstotek_3->setText(pretvori_iz_double(QString::number(znesek_skupaj_pravne / znesek_skupaj * 100, 'f', 2)) + " %");
	}
	else {
		ui->txt_skupaj_odstotek_3->setText("");
		ui->txt_fizicne_osebe_odstotek_3->setText("");
		ui->txt_pravne_osebe_odstotek_3->setText("");
	}

	napolni_letni_tabela();

}

void wid_prihodki::napolni_letni_tabela() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-tabela-storitev-letni");
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
		ui->tbl_storitve_3->clear();

		for (int i = 0; i <= 6; i++) {
			ui->tbl_storitve_3->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM sif_storitve");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_storitve_3->removeRow(0);
		}

		// start filling the table
		ui->tbl_storitve_3->insertColumn(0);
		ui->tbl_storitve_3->insertColumn(1);
		ui->tbl_storitve_3->insertColumn(2);
		ui->tbl_storitve_3->insertColumn(3);
		ui->tbl_storitve_3->insertColumn(4);
		ui->tbl_storitve_3->insertColumn(5);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Sklop");
		naslov2->setText("Skupina");
		naslov3->setText("Storitev");
		naslov4->setText("Oddelanih ur");
		naslov5->setText("Prihodek");

		ui->tbl_storitve_3->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_storitve_3->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_storitve_3->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_storitve_3->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_storitve_3->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_storitve_3->setHorizontalHeaderItem(5, naslov5);

		ui->tbl_storitve_3->setColumnWidth(0, 35);

		/**
			* 1) poiscemo racune, ki spadajo v dani okvir
			* 2) shranimo id-je racunov v seznam
			* 3) glede na spustne sezname dolocimo opravila, ki jih je potrebno prikazati
			* 4) poiscemo tem racunom pripadajoca opravila
			* 5) razvrstimo opravljena opravila glede na prikazana opravila
			* 6) sestejemo ure in zneske opravljenih opravil
			* 7) sestevke prikazemo v tabeli
			**/

		// 1) poiscemo racune in jih 2) shranimo v seznam
		QStringList seznam_id_racunov;
		QSqlQuery sql_racuni;
		sql_racuni.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
											 " AND datum_placila LIKE '%." + pretvori(ui->txt_leto_3->currentText()) + "'"
											 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
		sql_racuni.exec();
		while ( sql_racuni.next() ) {
			seznam_id_racunov.append(prevedi(sql_racuni.value(sql_racuni.record().indexOf("id")).toString()));
		}

		// 3) glede na spustne sezname dolocimo opravila, ki jih je potrebno prikazati
		QSqlQuery sql_storitve; // za vse situacije

		// prikazi vse storitve
		if ( ui->cb_vsa_opravila_3->isChecked() ) {
			sql_storitve.prepare("SELECT * FROM sif_storitve");
		}
		else {
			if ( ui->txt_storitev_3->currentText() != "" ) { // imamo izbrano storitev, torej prikazemo samo to storitev
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE storitev LIKE '" + pretvori(ui->txt_storitev_3->currentText()) + "'");
			}
			else if ( ui->txt_skupina_3->currentText() != "" ) { // imamo izbrano skupino, prikazemo vse storitve v tej skupini
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina_3->currentText()) + "'");
			}
			else if ( ui->txt_sklop_3->currentText() != "" ) { // imamo izbran sklop, prikazemo vse skupine v tem sklopu, ne pa storitev
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE sklop LIKE '" + pretvori(ui->txt_sklop_3->currentText()) + "'");
			}
			else { // nic ni izbrano, prikazemo vse sklope
				sql_storitve.prepare("SELECT * FROM sif_storitve");
			}
		}

		sql_storitve.exec(); // izvrsi zgodnji sql stavek

		// prikazi storitve v tabeli
		// ce so samo sklopi, skrij storitve in skupine
		// ce so samo skupine, skrij storitve
		// za vsako storitev, skupino, sklop sestej opravila, ki jih ima posamezen id
		// prikazemo ure in zneske

		// nastavimo st. vrstice in st. stolpca
		int row = 0;
		QString stavek_sestevek = "";
		while ( sql_storitve.next() ) {
			QString preverjanje = ""; // ce string ni prazen, potem vnesemo vrstico

			if ( ui->cb_vsa_opravila_3->isChecked() ) {	// prikaze vse storitve, ki so v bazi
				preverjanje = "polno";
				ui->tbl_storitve_3->setColumnHidden(0, false); // ID
				ui->tbl_storitve_3->setColumnHidden(1, false); // sklop
				ui->tbl_storitve_3->setColumnHidden(2, false); // skupina
				ui->tbl_storitve_3->setColumnHidden(3, false); // storitev
				ui->tbl_storitve_3->setColumnHidden(4, false); // ure
				ui->tbl_storitve_3->setColumnHidden(5, false); // znesek

				// pripravi sql stavek za primer prikaza vseh storitev
				stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
											 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
											 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
			}
			else {
				if ( ui->txt_storitev_3->currentText() != "" ) { // imamo izbrano storitev, torej prikazemo samo to storitev
					if ( ui->txt_storitev_3->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()) ) {
						if ( ui->txt_skupina_3->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()) ) {
							if ( ui->txt_sklop_3->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
								preverjanje = "polno";
								ui->tbl_storitve_3->setColumnHidden(0, false); // ID
								ui->tbl_storitve_3->setColumnHidden(1, false); // sklop
								ui->tbl_storitve_3->setColumnHidden(2, false); // skupina
								ui->tbl_storitve_3->setColumnHidden(3, false); // storitev
								ui->tbl_storitve_3->setColumnHidden(4, false); // ure
								ui->tbl_storitve_3->setColumnHidden(5, false); // znesek

								// pripravi sql stavek za primer prikaza izbrane storitve
								stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
															 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
															 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
							}
						}
					}
				}
				else if ( ui->txt_skupina_3->currentText() != "" ) { // imamo izbrano skupino, prikazemo vse storitve v tej skupini
					if ( ui->tbl_storitve_3->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()), Qt::MatchExactly).count() < 1 ) {
						if ( ui->txt_skupina_3->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()) ) {
							if ( ui->txt_sklop_3->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
								preverjanje = "polno";
								ui->tbl_storitve_3->setColumnHidden(0, false); // ID
								ui->tbl_storitve_3->setColumnHidden(1, false); // sklop
								ui->tbl_storitve_3->setColumnHidden(2, false); // skupina
								ui->tbl_storitve_3->setColumnHidden(3, false); // storitev
								ui->tbl_storitve_3->setColumnHidden(4, false); // ure
								ui->tbl_storitve_3->setColumnHidden(5, false); // znesek

								// pripravi sql stavek za primer prikaza vseh storitev v izbrani skupini
								stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
															 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
															 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
							}
						}
					}
				}
				else if ( ui->txt_sklop_3->currentText() != "" ) { // imamo izbran sklop, prikazemo vse skupine v tem sklopu, ne pa storitev
					if ( ui->tbl_storitve_3->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()), Qt::MatchExactly).count() < 1 ) {
						if ( ui->txt_sklop_3->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
							preverjanje = "polno";
							ui->tbl_storitve_3->setColumnHidden(0, false); // ID
							ui->tbl_storitve_3->setColumnHidden(1, false); // sklop
							ui->tbl_storitve_3->setColumnHidden(2, false); // skupina
							ui->tbl_storitve_3->setColumnHidden(3, true); // storitev
							ui->tbl_storitve_3->setColumnHidden(4, false); // ure
							ui->tbl_storitve_3->setColumnHidden(5, false); // znesek

							// pripravi sql stavek za primer prikaza vseh skupin
							stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
														 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "'";

						}
					}
				}
				else { // nic ni izbrano, prikazemo vse sklope
					if ( ui->tbl_storitve_3->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()), Qt::MatchExactly).count() < 1 ) {
						preverjanje = "polno";
						ui->tbl_storitve_3->setColumnHidden(0, false); // ID
						ui->tbl_storitve_3->setColumnHidden(1, false); // sklop
						ui->tbl_storitve_3->setColumnHidden(2, true); // skupina
						ui->tbl_storitve_3->setColumnHidden(3, true); // storitev
						ui->tbl_storitve_3->setColumnHidden(4, false); // ure
						ui->tbl_storitve_3->setColumnHidden(5, false); // znesek

						// pripravi sql stavek za primer prikaza vseh sklopov
						stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "'";
					}
				}
			}

		//	if ( ui->tbl_storitve_3->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()), Qt::MatchExactly).count() < 1 ) {
		//		preverjanje = "polno";
		//	}

			if ( preverjanje != "" ) {
				ui->tbl_storitve_3->insertRow(row);
				ui->tbl_storitve_3->setRowHeight(row, 20);

				int col = 0;
				QString polja[4] = {"id", "sklop", "skupina", "storitev"};

				while ( col < 4 ) {
					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[col] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_storitve.value(sql_storitve.record().indexOf(polja[col])).toString()).toInt());
					}
					else {
						celica->setText(prevedi(sql_storitve.value(sql_storitve.record().indexOf(polja[col])).toString()));
					}
					ui->tbl_storitve_3->setItem(row, col, celica);
					col++;
				}

				// sestej vsa opravila v danem mesecu (ki so del predhodno izbranih racunov), katerih storitev pripada dani vrstici

				// pojdi skozi vsa opravila pri predhodno izbranih racunih in pri vsakem poisci storitev, ki jo trenutno potrebujemo
				double sestevek_ure = 0.0;
				double sestevek_znesek = 0.0;
				for ( int a = 0; a < seznam_id_racunov.count(); a++ ) {

					QSqlQuery sql_sestevek;
					sql_sestevek.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + seznam_id_racunov.value(a) + "' " + stavek_sestevek);
					sql_sestevek.exec();

					while ( sql_sestevek.next() ) {

						sestevek_ure += prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("ur_dela")).toString()).toDouble();
						sestevek_znesek += prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("znesek_koncni")).toString()).toDouble() +
															 prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("znesek_ddv")).toString()).toDouble();
					}

					sql_sestevek.clear();
				}

				QTableWidgetItem *ure = new QTableWidgetItem;
				ure->setText(QString::number(sestevek_ure, 'f', 2) + " ur");
				ui->tbl_storitve_3->setItem(row, 4, ure);

				QTableWidgetItem *znesek = new QTableWidgetItem;
				znesek->setText(QString::number(sestevek_znesek, 'f', 2) + " EUR");
				ui->tbl_storitve_3->setItem(row, 5, znesek);

				// skrij polja, kjer je stevilo opravljenih ur 0
				if ( sestevek_ure == 0.0 ) {
					ui->tbl_storitve_3->setRowHidden(row, true);
				}
				else {
					ui->tbl_storitve_3->setRowHidden(row, false);
				}

				row++;
			}

		}

	}

	base.close();

}

// napolni skupni pregled
void wid_prihodki::napolni_skupni() {

	double znesek_avans = 0.0;
	double znesek_avans_fizicne = 0.0;
	double znesek_avans_pravne = 0.0;

	double znesek_ostanek = 0.0;
	double znesek_ostanek_fizicne = 0.0;
	double znesek_ostanek_pravne = 0.0;

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-polnenje-podatkov-skupni");
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

		// poisci, kateri avansi so bili placani v danem mesecu
		QSqlQuery sql_avans;
		sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'");
		sql_avans.exec();
		while ( sql_avans.next() ) {
			znesek_avans += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();

			// razvrsti placila glede na pravne in fizicne osebe
			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_avans.value(sql_avans.record().indexOf("stranka")).toString() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "1" ) {
					znesek_avans_fizicne += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
				}
				else if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "2" ) {
					znesek_avans_pravne += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
				}
			}
		}

		// poisci preostanek placila
		QSqlQuery sql_preostanek;
		sql_preostanek.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
													 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
		sql_preostanek.exec();
		while ( sql_preostanek.next() ) {
			QSqlQuery sql_opravilo;
			sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
			sql_opravilo.exec();
			while ( sql_opravilo.next() ) {
				znesek_ostanek += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
													pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
			}
			sql_opravilo.clear();
			// odstejemo ze placan avans
			znesek_ostanek -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();

			// razvrsti placila glede na pravne in fizicne osebe
			QSqlQuery sql_stranke;
			sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("stranka")).toString() + "'");
			sql_stranke.exec();
			if ( sql_stranke.next() ) {
				if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "1" ) {
					sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
					sql_opravilo.exec();
					while ( sql_opravilo.next() ) {
						znesek_ostanek_fizicne += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
																			pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
					}
					sql_opravilo.clear();
					// odstejemo ze placan avans
					znesek_ostanek_fizicne -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
				}
				else if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("tip")).toString()) == "2" ) {
					sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
					sql_opravilo.exec();
					while ( sql_opravilo.next() ) {
						znesek_ostanek_pravne += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
																		 pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
					}
					sql_opravilo.clear();
					// odstejemo ze placan avans
					znesek_ostanek_pravne -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
				}
			}
		}

	}
	base.close();

	double znesek_skupaj = znesek_avans + znesek_ostanek;
	double znesek_skupaj_fizicne = znesek_avans_fizicne + znesek_ostanek_fizicne;
	double znesek_skupaj_pravne = znesek_avans_pravne + znesek_ostanek_pravne;

	ui->txt_skupaj_znesek_4->setText(pretvori_iz_double(QString::number(znesek_skupaj, 'f', 2)) + " EUR");
	ui->txt_fizicne_osebe_znesek_4->setText(pretvori_iz_double(QString::number(znesek_skupaj_fizicne, 'f', 2)) + " EUR");
	ui->txt_pravne_osebe_znesek_4->setText(pretvori_iz_double(QString::number(znesek_skupaj_pravne, 'f', 2)) + " EUR");

	if ( znesek_skupaj > 0.0 ) {
		ui->txt_skupaj_odstotek_4->setText(pretvori_iz_double(QString::number(znesek_skupaj / znesek_skupaj * 100, 'f', 2)) + " %");
		ui->txt_fizicne_osebe_odstotek_4->setText(pretvori_iz_double(QString::number(znesek_skupaj_fizicne / znesek_skupaj * 100, 'f', 2)) + " %");
		ui->txt_pravne_osebe_odstotek_4->setText(pretvori_iz_double(QString::number(znesek_skupaj_pravne / znesek_skupaj * 100, 'f', 2)) + " %");
	}
	else {
		ui->txt_skupaj_odstotek_4->setText("");
		ui->txt_fizicne_osebe_odstotek_4->setText("");
		ui->txt_pravne_osebe_odstotek_4->setText("");
	}

	napolni_skupni_tabela();

}

void wid_prihodki::napolni_skupni_tabela() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prihodki-tabela-storitev-letni");
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
		ui->tbl_storitve_4->clear();

		for (int i = 0; i <= 6; i++) {
			ui->tbl_storitve_4->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM sif_storitve");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_storitve_4->removeRow(0);
		}

		// start filling the table
		ui->tbl_storitve_4->insertColumn(0);
		ui->tbl_storitve_4->insertColumn(1);
		ui->tbl_storitve_4->insertColumn(2);
		ui->tbl_storitve_4->insertColumn(3);
		ui->tbl_storitve_4->insertColumn(4);
		ui->tbl_storitve_4->insertColumn(5);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;
		QTableWidgetItem *naslov5 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Sklop");
		naslov2->setText("Skupina");
		naslov3->setText("Storitev");
		naslov4->setText("Oddelanih ur");
		naslov5->setText("Prihodek");

		ui->tbl_storitve_4->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_storitve_4->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_storitve_4->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_storitve_4->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_storitve_4->setHorizontalHeaderItem(4, naslov4);
		ui->tbl_storitve_4->setHorizontalHeaderItem(5, naslov5);

		ui->tbl_storitve_4->setColumnWidth(0, 35);

		/**
			* 1) poiscemo racune, ki spadajo v dani okvir
			* 2) shranimo id-je racunov v seznam
			* 3) glede na spustne sezname dolocimo opravila, ki jih je potrebno prikazati
			* 4) poiscemo tem racunom pripadajoca opravila
			* 5) razvrstimo opravljena opravila glede na prikazana opravila
			* 6) sestejemo ure in zneske opravljenih opravil
			* 7) sestevke prikazemo v tabeli
			**/

		// 1) poiscemo racune in jih 2) shranimo v seznam
		QStringList seznam_id_racunov;
		QSqlQuery sql_racuni;
		sql_racuni.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
											 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
		sql_racuni.exec();
		while ( sql_racuni.next() ) {
			seznam_id_racunov.append(prevedi(sql_racuni.value(sql_racuni.record().indexOf("id")).toString()));
		}

		// 3) glede na spustne sezname dolocimo opravila, ki jih je potrebno prikazati
		QSqlQuery sql_storitve; // za vse situacije

		// prikazi vse storitve
		if ( ui->cb_vsa_opravila_4->isChecked() ) {
			sql_storitve.prepare("SELECT * FROM sif_storitve");
		}
		else {
			if ( ui->txt_storitev_4->currentText() != "" ) { // imamo izbrano storitev, torej prikazemo samo to storitev
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE storitev LIKE '" + pretvori(ui->txt_storitev_4->currentText()) + "'");
			}
			else if ( ui->txt_skupina_4->currentText() != "" ) { // imamo izbrano skupino, prikazemo vse storitve v tej skupini
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE skupina LIKE '" + pretvori(ui->txt_skupina_4->currentText()) + "'");
			}
			else if ( ui->txt_sklop_4->currentText() != "" ) { // imamo izbran sklop, prikazemo vse skupine v tem sklopu, ne pa storitev
				sql_storitve.prepare("SELECT * FROM sif_storitve WHERE sklop LIKE '" + pretvori(ui->txt_sklop_4->currentText()) + "'");
			}
			else { // nic ni izbrano, prikazemo vse sklope
				sql_storitve.prepare("SELECT * FROM sif_storitve");
			}
		}

		sql_storitve.exec(); // izvrsi zgodnji sql stavek

		// prikazi storitve v tabeli
		// ce so samo sklopi, skrij storitve in skupine
		// ce so samo skupine, skrij storitve
		// za vsako storitev, skupino, sklop sestej opravila, ki jih ima posamezen id
		// prikazemo ure in zneske

		// nastavimo st. vrstice in st. stolpca
		int row = 0;
		QString stavek_sestevek = "";
		while ( sql_storitve.next() ) {
			QString preverjanje = ""; // ce string ni prazen, potem vnesemo vrstico

			if ( ui->cb_vsa_opravila_4->isChecked() ) {	// prikaze vse storitve, ki so v bazi
				preverjanje = "polno";
				ui->tbl_storitve_4->setColumnHidden(0, false); // ID
				ui->tbl_storitve_4->setColumnHidden(1, false); // sklop
				ui->tbl_storitve_4->setColumnHidden(2, false); // skupina
				ui->tbl_storitve_4->setColumnHidden(3, false); // storitev
				ui->tbl_storitve_4->setColumnHidden(4, false); // ure
				ui->tbl_storitve_4->setColumnHidden(5, false); // znesek

				// pripravi sql stavek za primer prikaza vseh storitev
				stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
											 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
											 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
			}
			else {
				if ( ui->txt_storitev_4->currentText() != "" ) { // imamo izbrano storitev, torej prikazemo samo to storitev
					if ( ui->txt_storitev_4->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()) ) {
						if ( ui->txt_skupina_4->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()) ) {
							if ( ui->txt_sklop_4->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
								preverjanje = "polno";
								ui->tbl_storitve_4->setColumnHidden(0, false); // ID
								ui->tbl_storitve_4->setColumnHidden(1, false); // sklop
								ui->tbl_storitve_4->setColumnHidden(2, false); // skupina
								ui->tbl_storitve_4->setColumnHidden(3, false); // storitev
								ui->tbl_storitve_4->setColumnHidden(4, false); // ure
								ui->tbl_storitve_4->setColumnHidden(5, false); // znesek

								// pripravi sql stavek za primer prikaza izbrane storitve
								stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
															 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
															 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
							}
						}
					}
				}
				else if ( ui->txt_skupina_4->currentText() != "" ) { // imamo izbrano skupino, prikazemo vse storitve v tej skupini
					if ( ui->tbl_storitve_4->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString()), Qt::MatchExactly).count() < 1 ) {
						if ( ui->txt_skupina_4->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()) ) {
							if ( ui->txt_sklop_4->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
								preverjanje = "polno";
								ui->tbl_storitve_4->setColumnHidden(0, false); // ID
								ui->tbl_storitve_4->setColumnHidden(1, false); // sklop
								ui->tbl_storitve_4->setColumnHidden(2, false); // skupina
								ui->tbl_storitve_4->setColumnHidden(3, false); // storitev
								ui->tbl_storitve_4->setColumnHidden(4, false); // ure
								ui->tbl_storitve_4->setColumnHidden(5, false); // znesek

								// pripravi sql stavek za primer prikaza vseh storitev v izbrani skupini
								stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
															 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "' "
															 "AND opravilo_storitev LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("storitev")).toString() + "'";
							}
						}
					}
				}
				else if ( ui->txt_sklop_4->currentText() != "" ) { // imamo izbran sklop, prikazemo vse skupine v tem sklopu, ne pa storitev
					if ( ui->tbl_storitve_4->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString()), Qt::MatchExactly).count() < 1 ) {
						if ( ui->txt_sklop_4->currentText() == prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()) ) {
							preverjanje = "polno";
							ui->tbl_storitve_4->setColumnHidden(0, false); // ID
							ui->tbl_storitve_4->setColumnHidden(1, false); // sklop
							ui->tbl_storitve_4->setColumnHidden(2, false); // skupina
							ui->tbl_storitve_4->setColumnHidden(3, true); // storitev
							ui->tbl_storitve_4->setColumnHidden(4, false); // ure
							ui->tbl_storitve_4->setColumnHidden(5, false); // znesek

							// pripravi sql stavek za primer prikaza vseh skupin
							stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "' "
														 "AND opravilo_skupina LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("skupina")).toString() + "'";

						}
					}
				}
				else { // nic ni izbrano, prikazemo vse sklope
					if ( ui->tbl_storitve_4->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()), Qt::MatchExactly).count() < 1 ) {
						preverjanje = "polno";
						ui->tbl_storitve_4->setColumnHidden(0, false); // ID
						ui->tbl_storitve_4->setColumnHidden(1, false); // sklop
						ui->tbl_storitve_4->setColumnHidden(2, true); // skupina
						ui->tbl_storitve_4->setColumnHidden(3, true); // storitev
						ui->tbl_storitve_4->setColumnHidden(4, false); // ure
						ui->tbl_storitve_4->setColumnHidden(5, false); // znesek

						// pripravi sql stavek za primer prikaza vseh sklopov
						stavek_sestevek = "AND opravilo_sklop LIKE '" + sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString() + "'";
					}
				}
			}

		//	if ( ui->tbl_storitve_4->findItems(prevedi(sql_storitve.value(sql_storitve.record().indexOf("sklop")).toString()), Qt::MatchExactly).count() < 1 ) {
		//		preverjanje = "polno";
		//	}

			if ( preverjanje != "" ) {
				ui->tbl_storitve_4->insertRow(row);
				ui->tbl_storitve_4->setRowHeight(row, 20);

				int col = 0;
				QString polja[4] = {"id", "sklop", "skupina", "storitev"};

				while ( col < 4 ) {
					QTableWidgetItem *celica = new QTableWidgetItem;
					if ( polja[col] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_storitve.value(sql_storitve.record().indexOf(polja[col])).toString()).toInt());
					}
					else {
						celica->setText(prevedi(sql_storitve.value(sql_storitve.record().indexOf(polja[col])).toString()));
					}
					ui->tbl_storitve_4->setItem(row, col, celica);
					col++;
				}

				// sestej vsa opravila v danem mesecu (ki so del predhodno izbranih racunov), katerih storitev pripada dani vrstici

				// pojdi skozi vsa opravila pri predhodno izbranih racunih in pri vsakem poisci storitev, ki jo trenutno potrebujemo
				double sestevek_ure = 0.0;
				double sestevek_znesek = 0.0;
				for ( int a = 0; a < seznam_id_racunov.count(); a++ ) {

					QSqlQuery sql_sestevek;
					sql_sestevek.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + seznam_id_racunov.value(a) + "' " + stavek_sestevek);
					sql_sestevek.exec();

					while ( sql_sestevek.next() ) {

						sestevek_ure += prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("ur_dela")).toString()).toDouble();
						sestevek_znesek += prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("znesek_koncni")).toString()).toDouble() +
															 prevedi(sql_sestevek.value(sql_sestevek.record().indexOf("znesek_ddv")).toString()).toDouble();
					}

					sql_sestevek.clear();
				}

				QTableWidgetItem *ure = new QTableWidgetItem;
				ure->setText(QString::number(sestevek_ure, 'f', 2) + " ur");
				ui->tbl_storitve_4->setItem(row, 4, ure);

				QTableWidgetItem *znesek = new QTableWidgetItem;
				znesek->setText(QString::number(sestevek_znesek, 'f', 2) + " EUR");
				ui->tbl_storitve_4->setItem(row, 5, znesek);

				// skrij polja, kjer je stevilo opravljenih ur 0
				if ( sestevek_ure == 0.0 ) {
					ui->tbl_storitve_4->setRowHidden(row, true);
				}
				else {
					ui->tbl_storitve_4->setRowHidden(row, false);
				}

				row++;
			}

		}

	}

	base.close();

}

// splosno
QString wid_prihodki::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_prihodki::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString wid_prihodki::pretvori_v_double(QString besedilo) {

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

QString wid_prihodki::pretvori_iz_double(QString besedilo) {

	besedilo.replace(".",",");

	return besedilo;

}

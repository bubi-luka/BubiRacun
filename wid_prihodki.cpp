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

			QSqlQuery sql_leto;
			sql_leto.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND status_oddaje_racuna NOT LIKE ''"
							 " AND stornacija NOT LIKE '1'");
			sql_leto.exec();
			while ( sql_leto.next() ) {
				QString leto = prevedi(sql_leto.value(sql_leto.record().indexOf("datum_izdaje")).toString()).right(4);
				if ( ui->txt_leto->findText(leto) == -1 ) {
					ui->txt_leto->addItem(leto);
					ui->txt_leto_3->addItem(leto);
				}
			}

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

// letni pregled
void wid_prihodki::on_txt_leto_3_currentIndexChanged() {

	napolni_letni();

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
/*
	double znesek_storno = 0.0;
	double znesek_storno_fizicne = 0.0;
	double znesek_storno_pravne = 0.0;
*/

		// poisci, kateri avansi so bili placani v danem mesecu (1==predracun)
		QSqlQuery sql_avans;
		sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '1' AND datum_placila_avansa LIKE '%." +
										 pretvori(ui->txt_mesec->currentText().left(2) + "." + ui->txt_leto->currentText()) +
										 "' AND status_racuna LIKE '" + pretvori("Potrjen") + "'");
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
		// poisci preostanek placila (3==racun)
		QSqlQuery sql_preostanek;
		sql_preostanek.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
							   " AND datum_placila LIKE '%." + pretvori(ui->txt_mesec->currentText().left(2) + "." + ui->txt_leto->currentText()) + "'"
							   " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'"
							   " AND stornacija NOT LIKE '1'");
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

}


// napolni letni pregled
void wid_prihodki::napolni_letni() {

	double znesek_avans = 0.0;
	double znesek_avans_fizicne = 0.0;
	double znesek_avans_pravne = 0.0;

	double znesek_ostanek = 0.0;
	double znesek_ostanek_fizicne = 0.0;
	double znesek_ostanek_pravne = 0.0;
/*
	double znesek_storno = 0.0;
	double znesek_storno_fizicne = 0.0;
	double znesek_storno_pravne = 0.0;
*/

		// poisci, kateri avansi so bili placani v danem mesecu
		QSqlQuery sql_avans;
		sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '1' AND datum_placila_avansa LIKE '%." +
										 pretvori(ui->txt_leto->currentText()) +
										 "' AND status_racuna LIKE '" + pretvori("Potrjen") + "'");
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
													 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'"
							   " AND stornacija NOT LIKE '1'");
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

}

// napolni skupni pregled
void wid_prihodki::napolni_skupni() {

	double znesek_avans = 0.0;
	double znesek_avans_fizicne = 0.0;
	double znesek_avans_pravne = 0.0;

	double znesek_ostanek = 0.0;
	double znesek_ostanek_fizicne = 0.0;
	double znesek_ostanek_pravne = 0.0;
/*
	double znesek_storno = 0.0;
	double znesek_storno_fizicne = 0.0;
	double znesek_storno_pravne = 0.0;
*/

		// poisci, kateri avansi so bili placani v danem mesecu
		QSqlQuery sql_avans;
		sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '1' AND status_racuna LIKE '" + pretvori("Potrjen") + "'");
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
													 " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'"
							   " AND stornacija NOT LIKE '1'");
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

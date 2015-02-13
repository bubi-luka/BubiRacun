#include <QtSql>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QFileDialog>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QTextEdit>

#include "wid_potninalogi.h"
#include "ui_wid_potninalogi.h"
#include "potninalogi.h"
#include "kodiranje.h"
#include "varnost.h"
#include "tiskanje.h"
#include "datum.h"

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

			// privzeto izberi trenutno leto
			ui->cb_leto->setCurrentIndex(ui->cb_leto->findText(QDate::currentDate().toString("yyyy")));

			// filtriraj po namenu
			QString s_povratek = "";
			ui->cb_namen->addItem("");
			sql_napolni.prepare("SELECT * FROM sif_namen_potnega_naloga");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_namen->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()) + ") " +
															prevedi(sql_napolni.value(sql_napolni.record().indexOf("namen")).toString()));
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("namen")).toString()) == "Povratek" ) {
					s_povratek = prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString());
				}
			}
			sql_napolni.clear();

			// filtriraj po stranki
			ui->cb_stranka->addItem("");
			sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE predlagatelj_oseba LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				QSqlQuery sql_stranke;
				sql_stranke.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("id")).toString() + "'");
				sql_stranke.exec();
				while ( sql_stranke.next() ) {
					if ( ui->cb_stranka->findText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("naziv_ciljnega_podjetja")).toString())) == -1 ) {
						ui->cb_stranka->addItem(prevedi(sql_stranke.value(sql_stranke.record().indexOf("naziv_ciljnega_podjetja")).toString()));
					}
				}
			}
			sql_napolni.clear();

			// razvrscanje strank po vrsti
			QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->cb_stranka);
			proxy->setSourceModel(ui->cb_stranka->model());
			// spustni seznam prepisemo
			ui->cb_stranka->model()->setParent(proxy);
			ui->cb_stranka->setModel(proxy);
			// razvrsti
			ui->cb_stranka->model()->sort(0);

			// filtriraj po kraju prihoda
			ui->cb_kraj->addItem("");
			sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE predlagatelj_oseba LIKE '" + pretvori(vApp->id()) + "'");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				QSqlQuery sql_kraj;
				sql_kraj.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("id")).toString() + "'");
				sql_kraj.exec();
				while ( sql_kraj.next() ) {
					if ( ui->cb_kraj->findText(prevedi(sql_kraj.value(sql_kraj.record().indexOf("kraj_prihoda")).toString()), Qt::MatchFixedString) == -1 ) {
						if ( prevedi(sql_kraj.value(sql_kraj.record().indexOf("namen_potovanja")).toString()) != s_povratek ) {
							ui->cb_kraj->addItem(prevedi(sql_kraj.value(sql_kraj.record().indexOf("kraj_prihoda")).toString()));
						}
					}
				}
			}
			sql_napolni.clear();

			// razvrscanje strank po vrsti
			QSortFilterProxyModel* proxy1 = new QSortFilterProxyModel(ui->cb_kraj);
			proxy1->setSourceModel(ui->cb_kraj->model());
			// spustni seznam prepisemo
			ui->cb_kraj->model()->setParent(proxy1);
			ui->cb_kraj->setModel(proxy1);
			// razvrsti
			ui->cb_kraj->model()->sort(0);

			// filtriraj po prevoznem sredstvu
			ui->cb_prevoz->addItem("");
			sql_napolni.prepare("SELECT * FROM sif_prevozna_sredstva");
			sql_napolni.exec();
			while ( sql_napolni.next() ) {
				ui->cb_prevoz->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()) + ") " +
															 prevedi(sql_napolni.value(sql_napolni.record().indexOf("prevoz")).toString()));
			}
			sql_napolni.clear();

		napolni();

		ui->btn_nov->setText(gumb);

		if ( vApp->permission() == pretvori("Administrator") ) {
			ui->btn_brisi->setVisible(true);
			ui->btn_brisi->setEnabled(true);
		}
		else {
			ui->btn_brisi->setVisible(false);
			ui->btn_brisi->setEnabled(false);
		}

		ui->tbl_potninalogi->sortByColumn(2, Qt::AscendingOrder);

}

wid_potninalogi::~wid_potninalogi()
{
	delete ui;
}

void wid_potninalogi::on_btn_refresh_clicked() {

	napolni();

}

void wid_potninalogi::on_btn_print_clicked() {

	QString seznam_id = "";
	QModelIndexList selectedList = ui->tbl_potninalogi->selectionModel()->selectedRows();

	for( int i = 0; i < selectedList.count(); i++) {
		seznam_id.append(ui->tbl_potninalogi->item(selectedList.at(i).row(), 0)->text() + ",");
	}

	// posljemo v tisk
	tiskanje *print = new tiskanje;
	print->show();
	QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
				 print , SLOT(prejem(QString, QString, QString)));
	tiskaj("potni-nalogi", seznam_id, "tisk"); // vrsta, id stevilke, format

}

void wid_potninalogi::on_btn_print_pdf_clicked() {

	QString seznam_id = "";
	QModelIndexList selectedList = ui->tbl_potninalogi->selectionModel()->selectedRows();

	for( int i = 0; i < selectedList.count(); i++) {
		seznam_id.append(ui->tbl_potninalogi->item(selectedList.at(i).row(), 0)->text() + ",");
	}

	// posljemo v tisk
	tiskanje *print = new tiskanje;
	print->show();
	QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
				 print , SLOT(prejem(QString, QString, QString)));
	tiskaj("potni-nalogi", seznam_id, "pdf"); // vrsta, id stevilke, format

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

	int izbranec = 0;
	int razvrsti = 0;

	if ( ui->tbl_potninalogi->selectedItems().count() > 0 ) {
		izbranec = ui->tbl_potninalogi->selectedItems().takeAt(0)->row();
	}

	razvrsti = ui->tbl_potninalogi->horizontalHeader()->sortIndicatorSection();

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

		ui->tbl_potninalogi->setColumnWidth(0, 0);

		datum *delegate = new datum(this);
		ui->tbl_potninalogi->setItemDelegateForColumn(3, delegate);

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QString stavek = "";

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

			// filtriramo glede na kraj prihoda
			if ( filter == "pozitivno" ) { // filtriramo samo, ce datumi ustrezajo
				// filtriraj glede na kraj prihoda
				if ( ui->cb_kraj->currentText() != "" ) {
					filter = "negativno"; // privzeto nastavimo na negativno vrednost, tako pozitivni zadetki lahko omogocijo pozitivni filter

					QStringList seznam_krajev;
					seznam_krajev.clear();

					// napolnimo seznam krajev s seznamom nasih krajev prihoda (ciljev)
					QSqlQuery sql_filter;
					sql_filter.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_fill.value(sql_fill.record().indexOf("stevilka_naloga")).toString() + "'");
					sql_filter.exec();
					while ( sql_filter.next() ) {
//                        if ( !seznam_krajev.contains(prevedi(sql_filter.value(sql_filter.record().indexOf("kraj_prihoda")).toString()).toLower()) ) {
							seznam_krajev.append(prevedi(sql_filter.value(sql_filter.record().indexOf("kraj_prihoda")).toString()).toLower());
//                        }
					}

					// izbrisi zadnji kraj prihoda (cilj) v seznamu krajev, razen, ce je samo en kraj (samo izhodisce)
					if ( seznam_krajev.count() > 1 ) {
						seznam_krajev.removeLast();
					}

					// ce seznam krajev vsebuje iskani kraj, potem obdrzimo filter "pozitivno", drugace ga zapremo
					if ( seznam_krajev.contains(ui->cb_kraj->currentText().toLower()) ) {
						filter = "pozitivno";
					}
				}
			}

			// filtriramo glede na namen potovanja
			if ( filter == "pozitivno" ) { // filtriramo samo, ce datumi in kraj prihoda ustrezajo
				// filtriraj glede na kraj prihoda
				if ( ui->cb_namen->currentText() != "" ) {
					filter = "negativno"; // privzeto nastavimo na negativno vrednost, tako pozitivni zadetki lahko omogocijo pozitivni filter
					QSqlQuery sql_filter;
					sql_filter.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_fill.value(sql_fill.record().indexOf("stevilka_naloga")).toString() + "'");
					sql_filter.exec();
					while ( sql_filter.next() ) {
						if ( ui->cb_namen->currentText().startsWith(prevedi(sql_filter.value(sql_filter.record().indexOf("namen_potovanja")).toString()) + ") ") ) {
							filter = "pozitivno";
						}
					}
				}
			}

			// filtriramo glede na stranko
			if ( filter == "pozitivno" ) { // filtriramo samo, ce datumi in kraj prihoda ustrezajo
				// filtriraj glede na kraj prihoda
				if ( ui->cb_stranka->currentText() != "" ) {
					filter = "negativno"; // privzeto nastavimo na negativno vrednost, tako pozitivni zadetki lahko omogocijo pozitivni filter
					QSqlQuery sql_filter;
					sql_filter.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_fill.value(sql_fill.record().indexOf("stevilka_naloga")).toString() + "'");
					sql_filter.exec();
					while ( sql_filter.next() ) {
						if ( prevedi(sql_filter.value(sql_filter.record().indexOf("naziv_ciljnega_podjetja")).toString()).toLower() == ui->cb_stranka->currentText().toLower() ) {
							filter = "pozitivno";
						}
					}
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
					if ( polja[i] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
					}
					else if ( polja[i] == "datum_naloga" ) {
						celica->setData(Qt::DisplayRole, QDate::fromString(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()), "dd'.'MM'.'yyyy"));
					}
					else if ( polja[i] == "prejemnik_oseba" ) {
						QSqlQuery sql_besedilo;
						sql_besedilo.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() + "'");
						sql_besedilo.exec();
						if ( sql_besedilo.next() ) {
							celica->setText(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("prejemnik_oseba_priimek")).toString()) + " " +
															prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("prejemnik_oseba_ime")).toString()));
						}
					}
					else if ( polja[i] == "namen_naloga" ) {
						QString s_namen_naloga = "";

						QSqlQuery sql_zdruzek;
						sql_zdruzek.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() + "'");
						sql_zdruzek.exec();
						while ( sql_zdruzek.next() ) {
							QSqlQuery sql_besedilo;
							sql_besedilo.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE id LIKE '" + sql_zdruzek.value(sql_zdruzek.record().indexOf("namen_potovanja")).toString() + "'");
							sql_besedilo.exec();
							if ( sql_besedilo.next() ) {
								if ( !s_namen_naloga.contains(prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("namen")).toString()) + ", ") ) {
									if ( prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("namen")).toString()) != "Povratek" ) {
										s_namen_naloga += prevedi(sql_besedilo.value(sql_besedilo.record().indexOf("namen")).toString()) + ", ";
									}
								}
							}
						}
						celica->setText(s_namen_naloga.left(s_namen_naloga.length() - 2));
					}
					else if ( polja[i] == "kraj_prihoda" ) {
						QString s_kraj_prihoda = "";
						QString s_povratek = "";

						QSqlQuery sql_povratek;
						sql_povratek.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE namen LIKE '" + pretvori("Povratek") + "'");
						sql_povratek.exec();
						if ( sql_povratek.next() ) {
							s_povratek = prevedi(sql_povratek.value(sql_povratek.record().indexOf("id")).toString());
						}

						QSqlQuery sql_zdruzek;
						sql_zdruzek.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() + "'");
						sql_zdruzek.exec();
						while ( sql_zdruzek.next() ) {
							if ( !s_kraj_prihoda.contains(prevedi(sql_zdruzek.value(sql_zdruzek.record().indexOf("kraj_prihoda")).toString()) + ", ") ) {
								if ( prevedi(sql_zdruzek.value(sql_zdruzek.record().indexOf("namen_potovanja")).toString()) != s_povratek ) {
									s_kraj_prihoda += prevedi(sql_zdruzek.value(sql_zdruzek.record().indexOf("kraj_prihoda")).toString()) + ", ";
								}
							}
						}
						celica->setText(s_kraj_prihoda.left(s_kraj_prihoda.length() - 2));
					}
					else if ( polja[i] == "naziv_ciljnega_podjetja" ) {
						QString s_podjetje = "";
						QString s_povratek = "";

						QSqlQuery sql_povratek;
						sql_povratek.prepare("SELECT * FROM sif_namen_potnega_naloga WHERE namen LIKE '" + pretvori("Povratek") + "'");
						sql_povratek.exec();
						if ( sql_povratek.next() ) {
							s_povratek = prevedi(sql_povratek.value(sql_povratek.record().indexOf("id")).toString());
						}

						QSqlQuery sql_zdruzek;
						sql_zdruzek.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() + "'");
						sql_zdruzek.exec();
						while ( sql_zdruzek.next() ) {
							if ( !s_podjetje.contains(prevedi(sql_zdruzek.value(sql_zdruzek.record().indexOf("naziv_ciljnega_podjetja")).toString()) + ", ") ) {
								if ( prevedi(sql_zdruzek.value(sql_zdruzek.record().indexOf("namen_potovanja")).toString()) != s_povratek ) {
									s_podjetje += prevedi(sql_zdruzek.value(sql_zdruzek.record().indexOf("naziv_ciljnega_podjetja")).toString()) + ", ";
								}
							}
						}
						celica->setText(s_podjetje.left(s_podjetje.length() - 2));
					}
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

	ui->tbl_potninalogi->selectRow(izbranec);
	ui->tbl_potninalogi->sortByColumn(razvrsti, Qt::AscendingOrder);

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

		QSqlQuery sql_brisi;
		sql_brisi.prepare("DELETE FROM potovanja WHERE potni_nalog LIKE '" + id + "'");
		sql_brisi.exec();
		sql_brisi.clear();

		sql_brisi.prepare("DELETE FROM stroski WHERE potninalog LIKE '" + id + "'");
		sql_brisi.exec();
		sql_brisi.clear();

		sql_brisi.prepare("DELETE FROM potni_nalogi WHERE id LIKE '" + id + "'");
		sql_brisi.exec();

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

void wid_potninalogi::on_btn_prestevilci_clicked() {

	QString ime_gumba = ui->btn_prestevilci->text();
	QString trenutni_vnos = "";
	int i_trenutni_vnos = 0;
	QString stevilo_vseh_vnosov = "";

		ui->btn_prestevilci->setText("... priprave na prestevilcevanje ...");
		QSqlQuery sql_vnosi;
		sql_vnosi.prepare("SELECT * FROM potni_nalogi");
		sql_vnosi.exec();
		int st_vnosov = 0;
		while ( sql_vnosi.next() ) {
			st_vnosov++;
		}
		stevilo_vseh_vnosov = QString::number(st_vnosov, 10);

		// poisci vsa leta; dobimo seznam vseh let, v katerih smo uradovali
		QStringList leta;

		QSqlQuery sql_leta;
		sql_leta.prepare("SELECT * FROM potni_nalogi");
		sql_leta.exec();
		while ( sql_leta.next() ) {
			if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_naloga")).toString()).right(4)) ) {
				leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_naloga")).toString()).right(4));

			}
		}

		// razvrsti leta narascajoce
		leta.sort();

		// za vsako leto poisci mesece, dneve in id-je ter jih zapisi v nov seznam
		for ( int i_leta = 0; i_leta < leta.count(); i_leta++ ) {

			// ustvari nov seznam id-jev, mesecev in zaporednih stevilk
			QStringList seznam_vnosov;
			QStringList meseci;
			int zaporedna_stevilka = 0;

			// poisci vse mesece, dobimo seznam mesecev v danem letu
			QSqlQuery sql_meseci;
			sql_meseci.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + leta.value(i_leta) + "'");
			sql_meseci.exec();
			while ( sql_meseci.next() ) {
				if ( !meseci.contains(prevedi(sql_meseci.value(sql_meseci.record().indexOf("datum_naloga")).toString()).right(7).left(2)) ) {
					meseci.append(prevedi(sql_meseci.value(sql_meseci.record().indexOf("datum_naloga")).toString()).right(7).left(2));
				}
			}

			// razvrsti po mesecu
			meseci.sort();

			// za vsak mesec poiscemo dneve, idje ter jih zapisemo v nov seznam
			for ( int i_meseci = 0; i_meseci < meseci.count(); i_meseci++ ) {

				// ustvarimo nov seznam dnevov
				QStringList dnevi;

				// poisci vse dneve, dobimo seznam dnevov v danem letu in danem mesecu
				QSqlQuery sql_dnevi;
				sql_dnevi.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + meseci.value(i_meseci) + "." + leta.value(i_leta) + "'");
				sql_dnevi.exec();
				while ( sql_dnevi.next() ) {
					if ( !dnevi.contains(prevedi(sql_dnevi.value(sql_dnevi.record().indexOf("datum_naloga")).toString()).left(2)) ) {
						dnevi.append(prevedi(sql_dnevi.value(sql_dnevi.record().indexOf("datum_naloga")).toString()).left(2));
					}
				}

				// razvrsti po dnevih
				dnevi.sort();

				// za vsak dan poisci id-je ter jih zapisi v nov seznam
				for ( int i_dnevi = 0; i_dnevi < dnevi.count(); i_dnevi++ ) {

					// poisci vse id-je, dobimo seznam id-jev v danem letu in danem mesecu in danem dnevu
					QSqlQuery sql_id_ji;
					sql_id_ji.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '" + dnevi.value(i_dnevi) + "." + meseci.value(i_meseci) + "." + leta.value(i_leta) + "'");
					sql_id_ji.exec();
					while ( sql_id_ji.next() ) {
						if ( !seznam_vnosov.contains(prevedi(sql_id_ji.value(sql_id_ji.record().indexOf("id")).toString())) ) {
							seznam_vnosov.append(prevedi(sql_id_ji.value(sql_id_ji.record().indexOf("id")).toString()));
						}
					}

					sql_id_ji.clear();

				} // for ( int i_dnevi = 0; i_dnevi < dnevi.count(); i_dnevi++ )

				dnevi.clear();
				sql_dnevi.clear();

			} // for ( int i_meseci = 0; i_meseci < meseci.count(); i_meseci++ )

			vApp->processEvents();

			ui->btn_prestevilci->setText("... prestevilcujem ...");
			// pojdi cez cel seznam vnosov (notri so IDji po vrstnem redu) in vsakega izpisi ( kasneje popravi )
			for ( int i_seznam_vnosov = 0; i_seznam_vnosov < seznam_vnosov.count(); i_seznam_vnosov++ ) {

				vApp->processEvents();
				i_trenutni_vnos++;
				trenutni_vnos = QString::number(i_trenutni_vnos, 10);
				ui->btn_prestevilci->setText(trenutni_vnos + "/" + stevilo_vseh_vnosov);

				vApp->processEvents();

				zaporedna_stevilka++;
				QString zaporedna = "";
				if ( zaporedna_stevilka < 10 ) {
					zaporedna = "00" + QString::number(zaporedna_stevilka, 10);
				}
				else if ( zaporedna_stevilka < 100 ) {
					zaporedna = "0" + QString::number(zaporedna_stevilka, 10);
				}
				else {
					zaporedna = "" + QString::number(zaporedna_stevilka, 10);
				}

				vApp->processEvents();

				// prestevilci potni nalog - stevilka naloga
				QSqlQuery sql_prestevilci;
				sql_prestevilci.prepare("UPDATE potni_nalogi SET stevilka_naloga = ? WHERE id LIKE '" + pretvori(seznam_vnosov.value(i_seznam_vnosov)) + "'");
				sql_prestevilci.bindValue(0, pretvori("PN-" + leta.value(i_leta) + "-" + zaporedna));
				sql_prestevilci.exec();

			} // for ( int i_seznam_vnosov = 0; i_seznam_vnosov < seznam_vnosov.count(); i_seznam_vnosov++ )

			meseci.clear();
			sql_meseci.clear();
			seznam_vnosov.clear();

		} // for ( int i_leta = 0; i_leta < leta.count(); i_leta++ )

		leta.clear();
		sql_leta.clear();

	ui->btn_prestevilci->setText("... koncujem ...");
	// sporocilo ob zakljucku prestevilcevanja
	QMessageBox zakljucek;
	zakljucek.setText("Prestevilcenje zakljuceno");
	zakljucek.setInformativeText("Prestevilcenje zahtevanih vnosov je bilo uspesno zakljuceno.");
	zakljucek.exec();

	napolni();

	ui->btn_prestevilci->setText(ime_gumba);

}

void wid_potninalogi::on_btn_kopiraj_clicked() {

	QClipboard *odlozisce = QApplication::clipboard();

	QModelIndexList selectedList = ui->tbl_potninalogi->selectionModel()->selectedRows();

	QString html_besedilo = "<table>";
	html_besedilo += "<tr>";
	html_besedilo += "<th>ID</th>";
	html_besedilo += "<th>Prejemnik</th>";
	html_besedilo += "<th>St. naloga</th>";
	html_besedilo += "<th>Datum izdaje</th>";
	html_besedilo += "<th>Namen</th>";
	html_besedilo += "<th>Naziv ciljnega podjetja</th>";
	html_besedilo += "<th>Kraj prihoda</th>";
	html_besedilo += "<th>Prevozno sredstvo</th>";
	html_besedilo += "<th>St. kilometrov</th>";
	html_besedilo += "<th>St. dnevnic</th>";
	html_besedilo += "<th>Ostali stroski</th>";
	html_besedilo += "<th>Skupaj stroski</th>";
	html_besedilo += "</tr>";
	for( int i = 0; i < selectedList.count(); i++) {
		html_besedilo += "<tr>";
		for ( int a = 0; a < 12; a++ ) {
				html_besedilo += "<td>";
				html_besedilo += ui->tbl_potninalogi->item(selectedList.at(i).row(), a)->text();
				html_besedilo += "</td>";
		}
		html_besedilo += "</tr>";
	}

	html_besedilo += "</table>";

	QTextEdit *textedit = new QTextEdit;

	textedit->setHtml(html_besedilo);
	html_besedilo = textedit->toHtml();

	odlozisce->clear();

	QMimeData *mimeData = new QMimeData();
	mimeData->setData("text/html", html_besedilo.toUtf8());
	odlozisce->setMimeData(mimeData, QClipboard::Clipboard);

}

void wid_potninalogi::on_btn_duplikat_clicked() {

	QString id = "";

		// doloci id naloga, ki ga zelimo kopirati
		QString zap_st = ui->tbl_potninalogi->selectedItems().takeAt(0)->text();

		// preberi oznacen nalog in ga kopiraj v nov nalog
		QSqlQuery sql_preberi;
		sql_preberi.prepare("SELECT * FROM potni_nalogi WHERE id LIKE '" + pretvori(zap_st) + "'");
		sql_preberi.exec();
		if ( sql_preberi.next() ) {

			// tvori novo stevilko potnega naloga
			QString st_naloga = "";

			QString leto = QDate::currentDate().toString("yyyy");

			QSqlQuery sql_stevilka;
			sql_stevilka.prepare("SELECT * FROM potni_nalogi WHERE stevilka_naloga LIKE '" + pretvori("PN-" + leto) + "%' ORDER BY stevilka_naloga ASC");
			sql_stevilka.exec();
			while ( sql_stevilka.next() ) {
				st_naloga = prevedi(sql_stevilka.value(sql_stevilka.record().indexOf("stevilka_naloga")).toString());
			}

			int zaporedna = st_naloga.right(3).toInt();
			zaporedna++;

			if ( zaporedna < 10 ) {
				st_naloga = "00" + QString::number(zaporedna, 10);
			}
			else if ( zaporedna < 100 ) {
				st_naloga = "0" + QString::number(zaporedna, 10);
			}
			else {
				st_naloga = "" + QString::number(zaporedna, 10);
			}

			st_naloga ="PN-" + leto + "-" + st_naloga;

			// vnesi kopijo naloga
			QSqlQuery sql_vnesi;
			sql_vnesi.prepare("INSERT INTO potni_nalogi (stevilka_naloga, datum_naloga, "
							  "stevilka_projekta, opombe, cena_prevoza, cena_dnevnic, ostali_stroski, stroski_skupaj, skupaj_kilometri, "
							  "kilometrina, skupaj_dnevi, skupaj_ure, priznana_dnevnica, cena_dnevnice_6_8, cena_dnevnice_8_12, cena_dnevnice_12_24, dnevnica_6_8, "
							  "dnevnica_8_12, dnevnica_12_24, zajtrk_8_12, zajtrk_12_24, predlagatelj_podjetje_id, predlagatelj_podjetje_dolgi, "
							  "predlagatelj_podjetje_kratki, predlagatelj_podjetje_naslov_ulica, predlagatelj_podjetje_naslov_stevilka, "
							  "predlagatelj_podjetje_naslov_posta, predlagatelj_podjetje_naslov_postna_stevilka, predlagatelj_podjetje_logotip, "
							  "predlagatelj_oseba_id, predlagatelj_oseba_ime, predlagatelj_oseba_priimek, predlagatelj_oseba_naziv, predlagatelj_oseba_naslov_ulica, "
							  "predlagatelj_oseba_naslov_stevilka, predlagatelj_oseba_naslov_posta, predlagatelj_oseba_naslov_postna_stevilka, "
							  "prejemnik_oseba_id, prejemnik_oseba_ime, prejemnik_oseba_priimek, prejemnik_oseba_naziv, prejemnik_oseba_naslov_ulica, "
							  "prejemnik_oseba_naslov_stevilka, prejemnik_oseba_naslov_posta, prejemnik_oseba_naslov_postna_stevilka, "
							  "prevozno_sredstvo, proizvajalec, znamka, tip, registrska_stevilka, priloge, stevilka_dokumenta) VALUES "
							  "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
							  "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			sql_vnesi.bindValue(0, pretvori(st_naloga));
			sql_vnesi.bindValue(1, pretvori(QDate::currentDate().toString("dd.MM.yyyy")));
			sql_vnesi.bindValue(2, sql_preberi.value(sql_preberi.record().indexOf("stevilka_projekta")).toString());
			sql_vnesi.bindValue(3, sql_preberi.value(sql_preberi.record().indexOf("opombe")).toString());
			sql_vnesi.bindValue(4, sql_preberi.value(sql_preberi.record().indexOf("cena_prevoza")).toString());
			sql_vnesi.bindValue(5, sql_preberi.value(sql_preberi.record().indexOf("cena_dnevnic")).toString());
			sql_vnesi.bindValue(6, sql_preberi.value(sql_preberi.record().indexOf("ostali_stroski")).toString());
			sql_vnesi.bindValue(7, sql_preberi.value(sql_preberi.record().indexOf("stroski_skupaj")).toString());
			sql_vnesi.bindValue(8, sql_preberi.value(sql_preberi.record().indexOf("skupaj_kilometri")).toString());
			sql_vnesi.bindValue(9, sql_preberi.value(sql_preberi.record().indexOf("kilometrina")).toString());
			sql_vnesi.bindValue(10, sql_preberi.value(sql_preberi.record().indexOf("skupaj_dnevi")).toString());
			sql_vnesi.bindValue(11, sql_preberi.value(sql_preberi.record().indexOf("skupaj_ure")).toString());
			sql_vnesi.bindValue(12, sql_preberi.value(sql_preberi.record().indexOf("priznana_dnevnica")).toString());
			sql_vnesi.bindValue(13, sql_preberi.value(sql_preberi.record().indexOf("cena_dnevnice_6_8")).toString());
			sql_vnesi.bindValue(14, sql_preberi.value(sql_preberi.record().indexOf("cena_dnevnice_8_12")).toString());
			sql_vnesi.bindValue(15, sql_preberi.value(sql_preberi.record().indexOf("cena_dnevnice_12_24")).toString());
			sql_vnesi.bindValue(16, sql_preberi.value(sql_preberi.record().indexOf("dnevnica_6_8")).toString());
			sql_vnesi.bindValue(17, sql_preberi.value(sql_preberi.record().indexOf("dnevnica_8_12")).toString());
			sql_vnesi.bindValue(18, sql_preberi.value(sql_preberi.record().indexOf("dnevnica_12_24")).toString());
			sql_vnesi.bindValue(19, sql_preberi.value(sql_preberi.record().indexOf("zajtrk_8_12")).toString());
			sql_vnesi.bindValue(20, sql_preberi.value(sql_preberi.record().indexOf("zajtrk_12_24")).toString());
			sql_vnesi.bindValue(21, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_id")).toString());
			sql_vnesi.bindValue(22, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_dolgi")).toString());
			sql_vnesi.bindValue(23, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_kratki")).toString());
			sql_vnesi.bindValue(24, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_naslov_ulica")).toString());
			sql_vnesi.bindValue(25, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_naslov_stevilka")).toString());
			sql_vnesi.bindValue(26, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_naslov_posta")).toString());
			sql_vnesi.bindValue(27, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_naslov_postna_stevilka")).toString());
			sql_vnesi.bindValue(28, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_podjetje_logotip")).toString());
			sql_vnesi.bindValue(29, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_id")).toString());
			sql_vnesi.bindValue(30, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_ime")).toString());
			sql_vnesi.bindValue(31, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_priimek")).toString());
			sql_vnesi.bindValue(32, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_naziv")).toString());
			sql_vnesi.bindValue(33, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_naslov_ulica")).toString());
			sql_vnesi.bindValue(34, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_naslov_stevilka")).toString());
			sql_vnesi.bindValue(35, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_naslov_posta")).toString());
			sql_vnesi.bindValue(36, sql_preberi.value(sql_preberi.record().indexOf("predlagatelj_oseba_naslov_postna_stevilka")).toString());
			sql_vnesi.bindValue(37, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_id")).toString());
			sql_vnesi.bindValue(38, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_ime")).toString());
			sql_vnesi.bindValue(39, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_priimek")).toString());
			sql_vnesi.bindValue(40, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_naziv")).toString());
			sql_vnesi.bindValue(41, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_naslov_ulica")).toString());
			sql_vnesi.bindValue(42, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_naslov_stevilka")).toString());
			sql_vnesi.bindValue(43, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_naslov_posta")).toString());
			sql_vnesi.bindValue(44, sql_preberi.value(sql_preberi.record().indexOf("prejemnik_oseba_naslov_postna_stevilka")).toString());
			sql_vnesi.bindValue(45, sql_preberi.value(sql_preberi.record().indexOf("prevozno_sredstvo")).toString());
			sql_vnesi.bindValue(46, sql_preberi.value(sql_preberi.record().indexOf("proizvajalec")).toString());
			sql_vnesi.bindValue(47, sql_preberi.value(sql_preberi.record().indexOf("znamka")).toString());
			sql_vnesi.bindValue(48, sql_preberi.value(sql_preberi.record().indexOf("tip")).toString());
			sql_vnesi.bindValue(49, sql_preberi.value(sql_preberi.record().indexOf("registrska_stevilka")).toString());
			sql_vnesi.bindValue(50, sql_preberi.value(sql_preberi.record().indexOf("priloge")).toString());
			sql_vnesi.bindValue(51, sql_preberi.value(sql_preberi.record().indexOf("stevilka_dokumenta")).toString());
			sql_vnesi.exec();

			sql_vnesi.clear();

			// dobi id zadnje vnesenega potnega naloga
			sql_vnesi.prepare("SELECT * FROM potni_nalogi");
			sql_vnesi.exec();
			while ( sql_vnesi.next() ) {
				id = prevedi(sql_vnesi.value(sql_vnesi.record().indexOf("id")).toString());
			}
			sql_vnesi.clear();

			// poisci vse vnose v stroskih za star id in jih vnesi kot nov id
			QSqlQuery sql_stroski;
			sql_stroski.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" +  pretvori(zap_st) + "'");
			sql_stroski.exec();
			while ( sql_stroski.next() ) {
				sql_vnesi.prepare("INSERT INTO stroski (potninalog, strosek, cena) VALUES (?, ?, ?)");
				sql_vnesi.bindValue(0, pretvori(id));
				sql_vnesi.bindValue(1, sql_stroski.value(sql_stroski.record().indexOf("strosek")).toString());
				sql_vnesi.bindValue(2, sql_stroski.value(sql_stroski.record().indexOf("cena")).toString());
				sql_vnesi.exec();
				sql_vnesi.clear();
			}
			sql_stroski.clear();

			// poisci vse postanke na poti za star id in jih vnesi za nov id
			QSqlQuery sql_pot;
			sql_pot.prepare("SELECT * FROM potovanja WHERE potni_nalog LIKE '" +  pretvori(zap_st) + "'");
			sql_pot.exec();
			while ( sql_pot.next() ) {
				sql_vnesi.prepare("INSERT INTO potovanja (potni_nalog, kraj_odhoda, kraj_prihoda, cas_odhoda, cas_prihoda, kilometri, naslov_ulica, naslov_stevilka, "
								  "naslov_postna_stevilka, naslov_posta, naziv_ciljnega_podjetja, namen_potovanja) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
				sql_vnesi.bindValue(0, pretvori(id));
				sql_vnesi.bindValue(1, sql_pot.value(sql_pot.record().indexOf("kraj_odhoda")).toString());
				sql_vnesi.bindValue(2, sql_pot.value(sql_pot.record().indexOf("kraj_prihoda")).toString());
				sql_vnesi.bindValue(3, sql_pot.value(sql_pot.record().indexOf("cas_odhoda")).toString());
				sql_vnesi.bindValue(4, sql_pot.value(sql_pot.record().indexOf("cas_prihoda")).toString());
				sql_vnesi.bindValue(5, sql_pot.value(sql_pot.record().indexOf("kilometri")).toString());
				sql_vnesi.bindValue(6, sql_pot.value(sql_pot.record().indexOf("naslov_ulica")).toString());
				sql_vnesi.bindValue(7, sql_pot.value(sql_pot.record().indexOf("naslov_stevilka")).toString());
				sql_vnesi.bindValue(8, sql_pot.value(sql_pot.record().indexOf("naslov_postna_stevilka")).toString());
				sql_vnesi.bindValue(9, sql_pot.value(sql_pot.record().indexOf("naslov_posta")).toString());
				sql_vnesi.bindValue(10, sql_pot.value(sql_pot.record().indexOf("naziv_ciljnega_podjetja")).toString());
				sql_vnesi.bindValue(11, sql_pot.value(sql_pot.record().indexOf("namen_potovanja")).toString());
				sql_vnesi.exec();
				sql_vnesi.clear();
			}
			sql_stroski.clear();
		}

	osvezi("potninalog");

	potninalogi *uredi = new potninalogi;
	uredi->show();
	QObject::connect(this, SIGNAL(prenos(QString)),
			   uredi , SLOT(prejem(QString)));
	prenos("kopiraj" + id);
	this->disconnect();

	// receive signal to refresh table
	QObject::connect(uredi, SIGNAL(poslji(QString)),
			   this , SLOT(osvezi(QString)));

}

#include "wid_casovnice.h"
#include "ui_wid_casovnice.h"

wid_casovnice::wid_casovnice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_casovnice)
{
    ui->setupUi(this);

		// vnesi naslov stolpca - dan v mesecu

		// privzeto nastavi, da ima mesec 31 dni
		int n = 30;

		// clear previous content
		ui->tbl_casovnice->clear();

		for (int i = 1; i < n; i++) {
			ui->tbl_casovnice->removeColumn(0);
		}
/*
		QSqlQuery sql_clear("wid_racuni");
		sql_clear.prepare("SELECT * FROM racuni");
		sql_clear.exec();
		while (sql_clear.next()) {
			ui->tbl_racuni->removeRow(0);
		}
*/
		// start filling the table
		QStringList naslovi;

		for ( int i = 0; i < 10; i++ ) {
			ui->tbl_casovnice->insertRow(i);
			ui->tbl_casovnice->setRowHeight(i, 20);
		}

		for ( int i = 0; i < n; i++ ) {
			ui->tbl_casovnice->insertColumn(i);
			ui->tbl_casovnice->setColumnWidth(i, 30);
			naslovi.append(QString::number(i + 1, 10));
		}
		ui->tbl_casovnice->setHorizontalHeaderLabels(naslovi);
		ui->tbl_casovnice->setVerticalHeaderLabels(naslovi);

/*

		QString projekt = "";

		QSqlQuery sql_projekt;
		sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
		sql_projekt.exec();
		if ( sql_projekt.next() ) {
			projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
		}

		QSqlQuery sql_fill("wid_racuni");
		if ( ui->txt_stprojekta->text() != "*" ) {
			sql_fill.prepare("SELECT * FROM racuni WHERE stprojekta LIKE '" + projekt + "'");
		}
		else {
			sql_fill.prepare("SELECT * FROM racuni");
		}
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			ui->tbl_racuni->insertRow(row);
			ui->tbl_racuni->setRowHeight(row, 20);
			int col = 0;
			int i = 0;
			QString polja[9] = {"id", "stevilka_racuna", "projekt", "stranka", "datum_pricetka", "datum_konca", "status_placila", "status_racunovodstva", "tip_racuna"};

			while (col <= 8) {

				QTableWidgetItem *celica = new QTableWidgetItem;
				if ( polja[i] == "stranka" ) {
					QSqlQuery sql_find_stranka;
					sql_find_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
					sql_find_stranka.exec();
					if ( sql_find_stranka.next() ) {
						if ( prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("tip")).toString()) == "1" ) {
							celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()) + " " +
											prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("priimek")).toString()));
						}
						else {
							 celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
						}
					}
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
	base.close();
	*/
}

wid_casovnice::~wid_casovnice()
{
    delete ui;
}

#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"

#include "dobropis.h"
#include "ui_dobropis.h"

dobropis::dobropis(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::dobropis)
{
	ui->setupUi(this);

	ui->txt_id_projekt->setVisible(false);
	ui->txt_id_racuna->setVisible(false);
	ui->txt_id_stranka->setVisible(false);

}

dobropis::~dobropis()
{
	delete ui;
}

void dobropis::on_btn_izhod_clicked() {

	close();

}

void dobropis::on_btn_izprazni_clicked() {

	ui->tbl_storitve->clearSelection();

}

void dobropis::prejem(QString besedilo) {

	if ( besedilo.left(12) == "Nov dobropis" ) {
		ui->txt_id_racuna->setText(besedilo.right(besedilo.length() - 12));
		napolni_racun();
		napolni_tabelo();
	}

}

void dobropis::napolni_racun() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_racune");
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

		QSqlQuery sql_racun;
		sql_racun.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(ui->txt_id_racuna->text()) + "'");
		sql_racun.exec();
		if ( sql_racun.next() ) {
			ui->txt_id_stranka->setText(prevedi(sql_racun.value(sql_racun.record().indexOf("stranka")).toString()));
			ui->txt_id_projekt->setText(prevedi(sql_racun.value(sql_racun.record().indexOf("projekt")).toString()));
		}
	}
	base.close();
}

void dobropis::napolni_tabelo() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni-tabelo-racunov");
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

		for (int i = 0; i <= 4; i++) {
			ui->tbl_storitve->removeColumn(0);
		}

		QSqlQuery sql_clear;
		sql_clear.prepare("SELECT * FROM racuni");
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

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;

		naslov0->setText("ID");
		naslov1->setText("Ime storitve");
		naslov2->setText("St. ur");
		naslov3->setText("Del dobropisa");
		naslov4->setText("St. ur dobropisa");

		ui->tbl_storitve->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_storitve->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_storitve->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_storitve->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_storitve->setHorizontalHeaderItem(4, naslov4);

		ui->tbl_storitve->setColumnWidth(0, 0);
		ui->tbl_storitve->setColumnWidth(1, 200);

		QSqlQuery sql_fill;
		sql_fill.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_id_racuna->text()) + "'");
		sql_fill.exec();

		int row = 0;
		while (sql_fill.next()) {
			// filtriramo glede na mesec in leto
			QString filter = "pozitivno";

			if ( filter == "pozitivno" ) {
				ui->tbl_storitve->insertRow(row);
				ui->tbl_storitve->setRowHeight(row, 20);
				int col = 0;
				int i = 0;
				QString polja[5] = {"id", "opravilo_storitev", "ur_dela", "dobropis", "ur_dobropisa"};

				while (col <= 4) {
					QTableWidgetItem *celica = new QTableWidgetItem;

					// storniranim vrsticam spremenimo barvo ozadja in besedila
					if ( prevedi(sql_fill.value(sql_fill.record().indexOf("stornacija")).toString()) == "1" ) {
						celica->setBackgroundColor(Qt::red);
						celica->setTextColor(Qt::black);
					}

					if ( polja[i] == "id" ) {
						celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
					}
					if ( polja[i] == "stornacija" ) {
						if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) == "1" ) { // racun je storniran
							celica->setText("DA");
						}
						else if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) == "0" ) { // racun ni storniran
							celica->setText("NE");
						}
						else { // ne gre za racun ampak za predracun, predplacilni racun ali stornacijo
							celica->setText("");
						}
					}
					else {
						celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
					}

					ui->tbl_storitve->setItem(row, col, celica);
					col++;
					i++;
				}
				row++;
			}
		}
	}
	base.close();

}

QString dobropis::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString dobropis::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void dobropis::on_tbl_storitve_itemSelectionChanged() {

	if ( ui->tbl_storitve->selectedItems().count() == 0 ) {
		ui->txt_id_storitve->setText("");
		ui->txt_ime_storitve->setText("");
		ui->txt_st_enot_na_dobropisu->setText("");
		ui->txt_st_enot_na_racunu->setText("");
		ui->cb_aktivirana->setChecked(false);
	}
	else {
		ui->txt_id_storitve->setText(ui->tbl_storitve->selectedItems().takeAt(0)->text());

		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni-tabelo-racunov");
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

			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id_storitve->text()) + "'");
			sql_fill.exec();
			if ( sql_fill.next() ) {
				ui->txt_ime_storitve->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("opravilo_storitev")).toString()));
				ui->txt_st_enot_na_racunu->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("ur_dela")).toString()));
				ui->txt_st_enot_na_dobropisu->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("")).toString()));
				if ( prevedi(sql_fill.value(sql_fill.record().indexOf("")).toString()) == "1" ) {
					ui->cb_aktivirana->setChecked(true);
				}
				else {
					ui->cb_aktivirana->setChecked(false);
				}
			}
			else {
				ui->txt_id_storitve->setText("");
				ui->txt_ime_storitve->setText("");
				ui->txt_st_enot_na_dobropisu->setText("");
				ui->txt_st_enot_na_racunu->setText("");
				ui->cb_aktivirana->setChecked(false);
			}

		}
		base.close();
	}

}

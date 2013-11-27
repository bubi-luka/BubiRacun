#include <QtSql>
#include <QMessageBox>
#include <QKeyEvent>

#include "kodiranje.h"

#include "razlog_stornacije.h"
#include "ui_razlog_stornacije.h"

razlog_stornacije::razlog_stornacije(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::razlog_stornacije)
{
	ui->setupUi(this);

	ui->txt_id->setVisible(false);
	ui->txt_razlog->setPlainText("");

}

razlog_stornacije::~razlog_stornacije()
{
	delete ui;
}

void razlog_stornacije::on_btn_brisi_clicked() {

	ui->txt_razlog->setPlainText("");

}

void razlog_stornacije::on_btn_shrani_clicked() {

	if ( ui->txt_razlog->toPlainText() != "" ) { // razlog je vpisan
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
			// baza je odprta

			// spremenimo status racuna na storinirano
			QSqlQuery sql_vnesi;
			sql_vnesi.prepare("UPDATE racuni SET razlog_stornacije = ?, stornacija = ? WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'"
							  " AND tip_racuna LIKE '" + pretvori("3") + "'");
			sql_vnesi.bindValue(0, pretvori(ui->txt_razlog->toPlainText()));
			sql_vnesi.bindValue(1, pretvori("1"));
			sql_vnesi.exec();
			sql_vnesi.clear();

			// poiscemo starsevski predracun
			QString stars_id = "";
			sql_vnesi.prepare("SELECT * from racuni WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'"
							  " AND tip_racuna LIKE '" + pretvori("3") + "'");
			sql_vnesi.exec();
			if ( sql_vnesi.next() ) {
				stars_id = prevedi(sql_vnesi.value(sql_vnesi.record().indexOf("stevilka_starsa")).toString());
			}
			sql_vnesi.clear();

			// popravimo starsevski predracun na odprt - omogoca izdajo novega racuna vezanega na isti predracun
			sql_vnesi.prepare("UPDATE racuni SET status_racuna = '' WHERE id LIKE '" + pretvori(stars_id) + "' AND tip_racuna LIKE '" + pretvori("1") + "'");
			sql_vnesi.exec();
			sql_vnesi.clear();

		}
		base.close();

		// send signal to reload widget
		poslji("racuni");

		close();

	}
	else { // razlog ni vpisan, je pa nujen
		QMessageBox msgbox;
		msgbox.setText("Razlog stornacije ni naveden");
		msgbox.setInformativeText("Razlog stornacije je nujen podatek. Brez njega ni mozno izvesti stornacije. Prosim, vpisite ga!");
		msgbox.exec();
	}

}

void razlog_stornacije::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_shrani_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_shrani_clicked();
	}
	else if ((event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::AltModifier))
	{
		this->on_btn_brisi_clicked();
	}

}

void razlog_stornacije::prejem(QString besedilo) {

	ui->txt_id->setText(besedilo);

}

QString razlog_stornacije::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString razlog_stornacije::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

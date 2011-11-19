#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "stroski.h"
#include "ui_stroski.h"
#include "glavnookno.h"
#include "kodiranje.h"

stroski::stroski(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::stroski)
{
	ui->setupUi(this);

	// set certain fields as disabled
	ui->txt_id->setDisabled(true);
	ui->txt_nalog->setDisabled(true);

	// validate numbers
	QDoubleValidator *validator = new QDoubleValidator(0, 999999, 2, this);
	ui->txt_cena->setValidator(validator);
}

stroski::~stroski()
{
	delete ui;
}

void stroski::on_btn_izhod_clicked() {

	close();

}

// prazno, dokler se ne dogovorimo o dejavnosti gumba
void stroski::on_btn_brisi_clicked() {

}

// ne preverja obveznih polj
void stroski::on_btn_sprejmi_clicked() {

	QString napaka = "";
	bool *ok;
	ui->txt_cena->setText(QString::number(ui->txt_cena->text().replace(",", ".").toDouble(ok), 'f', 2));
/*
	// nastavitev polja za napako
	QFont font_error;
	font_error.setBold(true);
	font_error.setUnderline(true);

	QFont font_normal;
	font_normal.setBold(false);
	font_normal.setUnderline(false);

	QPalette palette_error;
	QPalette palette_normal;

	QBrush brush_error(QColor(255, 0, 0, 255));
	brush_error.setStyle(Qt::SolidPattern);

	QBrush brush_normal(QColor(0, 0, 0, 255));
	brush_normal.setStyle(Qt::SolidPattern);

	palette_error.setBrush(QPalette::Active, QPalette::WindowText, brush_error);
	palette_error.setBrush(QPalette::Inactive, QPalette::WindowText, brush_error);
	palette_normal.setBrush(QPalette::Active, QPalette::WindowText, brush_normal);
	palette_normal.setBrush(QPalette::Inactive, QPalette::WindowText, brush_normal);

	// preveri uporabnisko ime, ce ze obstaja
	if (ui->txt_uporabnik->text() == "") {
		ui->label_4->setPalette(palette_error);
		ui->label_4->setFont(font_error);
		napaka = "true";
		ui->txt_uporabnik->setFocus();
	}
	*/

	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {
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
			QSqlQuery sql_vnesi_kupon;
			if (ui->btn_sprejmi->text() == "Vnesi strosek") { // vnesi novega uporabnika
				sql_vnesi_kupon.prepare("INSERT INTO stroski (potninalog, strosek, cena) "
											 "VALUES (?, ?, ?) ");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_kupon.prepare("UPDATE stroski SET potninalog = ?, strosek = ?, cena = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_kupon.bindValue(0, pretvori(ui->txt_nalog->text()));
			sql_vnesi_kupon.bindValue(1, pretvori(ui->txt_strosek->text()));
			sql_vnesi_kupon.bindValue(2, pretvori(ui->txt_cena->text()));
			sql_vnesi_kupon.exec();
		}
		base.close();

		// send signal to reload widget
		poslji("stroski");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void stroski::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}
	else if ((event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::AltModifier))
	{
		this->on_btn_brisi_clicked();
	}
}

void stroski::prejem(QString besedilo) {

	if (besedilo.left(11) == "Nov strosek") {
		ui->btn_sprejmi->setText("Vnesi strosek");
		ui->txt_nalog->setText(besedilo.right(besedilo.length() - 11));
	}
	else {
		ui->btn_sprejmi->setText("Popravi strosek");
		// besedilo nosi ID ze obstojecega uporabnika, potrebno je napolniti polja
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
			QSqlQuery sql_napolni;
			sql_napolni.prepare("SELECT * FROM stroski WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_nalog->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("potninalog")).toString()));
				ui->txt_strosek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("strosek")).toString()));
				ui->txt_cena->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena")).toString()));
			}
		}
		base.close();
	}
}

QString stroski::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString stroski::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

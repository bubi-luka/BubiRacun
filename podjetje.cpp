#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QFileDialog>
#include <QDir>

#include "podjetje.h"
#include "ui_podjetje.h"
#include "prijava.h"
#include "kodiranje.h"

podjetje::podjetje(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::podjetje)
{
    ui->setupUi(this);

	// hide fields
	ui->txt_logo->setVisible(false);

	// set input mask
	ui->txt_davcna->setInputMask("99999999;_");
	ui->txt_trr->setInputMask("SI56 9999 9999 9999 999;_");
	ui->txt_gsm->setInputMask("+990 (\\0)99/999-999;_");
	ui->txt_telefon->setInputMask("+990 (\\0)9/99-99-999;_");

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
		// insert data for combo box
		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM sif_nazivi");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_naziv->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naziv")).toString()));
		}
	}
	base.close();

}

podjetje::~podjetje() {
    delete ui;
}

void podjetje::on_btn_izhod_clicked() {
	close();
}

void podjetje::on_btn_potrdi_clicked() {

	QString napaka = "";

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

	// preveri ime podjetja, ce ze obstaja
	if (ui->txt_ime->text() == "") {
		ui->label_5->setPalette(palette_error);
		ui->label_5->setFont(font_error);
		napaka = "true";
		ui->txt_ime->setFocus();
	}
	else {
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
			QSqlQuery sql_preveri_podjetje;
			sql_preveri_podjetje.prepare("SELECT * FROM podjetja WHERE ime LIKE '" + ui->txt_ime->text() + "'");
			sql_preveri_podjetje.exec();
			if (sql_preveri_podjetje.next()) {
				if (ui->txt_id->text() != prevedi(sql_preveri_podjetje.value(sql_preveri_podjetje.record().indexOf("id")).toString())) {
					ui->label_5->setPalette(palette_error);
					ui->label_5->setFont(font_error);
					napaka = "true";
					ui->txt_ime->setFocus();
				}
				else {
					ui->label_5->setPalette(palette_normal);
					ui->label_5->setFont(font_normal);
				}
			}
			else {
				ui->label_5->setPalette(palette_normal);
				ui->label_5->setFont(font_normal);
			}
		}
		base.close();
	}

	// preveri davcno stevilko
	QString davcna = ui->txt_davcna->text();
	if (ui->txt_zavezanec->isChecked()) {
		davcna = davcna.right(davcna.length()-2);
	}
	if (davcna.length() < 8 && davcna != "") { // dolzina
		ui->label_8->setPalette(palette_error);
		ui->label_8->setFont(font_error);
		napaka = "true";
		ui->txt_davcna->setFocus();
	}
	else { // pravilnost zapisa
		QString tax = davcna;
		QString tax_error = "";
		bool ok;

		int i_sum = 8*tax.left(1).toInt(&ok, 10) +
					7*tax.left(2).right(1).toInt(&ok, 10) +
					6*tax.left(3).right(1).toInt(&ok, 10) +
					5*tax.left(4).right(1).toInt(&ok, 10) +
					4*tax.left(5).right(1).toInt(&ok, 10) +
					3*tax.left(6).right(1).toInt(&ok, 10) +
					2*tax.left(7).right(1).toInt(&ok, 10);
		int i_mod = i_sum % 11;
		int i_control = 11 - i_mod;
		if (i_control == 10) {
			i_control = 0;
		}
		if (i_control == 11) {
			tax_error = "Error";
		}

		if (i_control != tax.right(1).toInt(&ok, 10)) {
			tax_error = "Error";
		}

		if (tax_error == "Error") {
			ui->label_8->setPalette(palette_error);
			ui->label_8->setFont(font_error);
			napaka = "true";
			ui->txt_davcna->setFocus();
		}
		else {
			ui->label_8->setPalette(palette_normal);
			ui->label_8->setFont(font_normal);
		}
	}

	// preveri trr
	// ne najdem algoritma

	// preveri email naslov
	ui->txt_email->setText(ui->txt_email->text().toLower());
	QRegExp rx_email("^[a-z0-9\\-\\_\\.]*[@][a-z0-9\\-\\_\\.]*[\\.][a-z]{2,3}$");

	if (!rx_email.exactMatch(ui->txt_email->text())) {
		ui->label_11->setPalette(palette_error);
		ui->label_11->setFont(font_error);
		napaka = "true";
		ui->txt_email->setFocus();
	}
	else {
		ui->label_11->setPalette(palette_normal);
		ui->label_11->setFont(font_normal);
	}

	// preveri url
	if (ui->txt_url->text() != "") {
		if (!(ui->txt_url->text().startsWith("http") || ui->txt_url->text().contains("://"))) {
			ui->txt_url->setText("http://" + ui->txt_url->text());
		}
		if (!(ui->txt_url->text().endsWith("/") || ui->txt_url->text().endsWith(".htm") || ui->txt_url->text().endsWith(".html"))) {
			ui->txt_url->setText(ui->txt_url->text().append("/"));
		}
	}

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
			QSqlQuery sql_vnesi;
			QString zavezanec = "NE";
			if (ui->txt_zavezanec->isChecked()) {
				zavezanec = "DA";
			}
			if (ui->btn_potrdi->text() == "Vnesi podjetje") {
			sql_vnesi.prepare("INSERT INTO podjetje (ime, polnoime, naslov, ddvzavezanec, davcnastevilka, logo, email, spletnastran, maticnastevilka, "
							  "trr, skd, lastnik, odgovornaoseba, nazivodgovorneosebe, gsm, telefon)"
							  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ");
			}
			else {
			sql_vnesi.prepare("UPDATE podjetje SET ime = ?, polnoime = ?, naslov = ?, ddvzavezanec = ?, davcnastevilka = ?, logo = ?, email = ?, "
							  "spletnastran = ?, maticnastevilka = ?, trr = ?, skd = ?, lastnik = ?, odgovornaoseba = ?, "
							  "nazivodgovorneosebe = ?, gsm = ?, telefon = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi.bindValue(0, pretvori(ui->txt_ime->text()));
			sql_vnesi.bindValue(1, pretvori(ui->txt_polnoime->text()));
			sql_vnesi.bindValue(2, pretvori(ui->txt_naslov->toPlainText()));
			sql_vnesi.bindValue(3, pretvori(zavezanec));
			sql_vnesi.bindValue(4, pretvori(ui->txt_davcna->text().remove(" ")));
			sql_vnesi.bindValue(5, pretvori(ui->txt_logo->text()));
			sql_vnesi.bindValue(6, pretvori(ui->txt_email->text()));
			sql_vnesi.bindValue(7, pretvori(ui->txt_url->text()));
			sql_vnesi.bindValue(8, pretvori(ui->txt_maticnastevilka->text()));
			sql_vnesi.bindValue(9, pretvori(ui->txt_trr->text().remove(" ")));
			sql_vnesi.bindValue(10, pretvori(ui->txt_skd->text()));
			sql_vnesi.bindValue(11, pretvori(ui->txt_lastnik->text()));
			sql_vnesi.bindValue(12, pretvori(ui->txt_odgovorni->text()));
			sql_vnesi.bindValue(13, pretvori(ui->txt_naziv->currentText()));
			sql_vnesi.bindValue(14, pretvori(ui->txt_gsm->text().remove(" ")));
			sql_vnesi.bindValue(15, pretvori(ui->txt_telefon->text().remove(" ")));
			sql_vnesi.exec();
		}
		base.close();

		// send signal to reload widget
		poslji("podjetje");

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}
}

void podjetje::on_txt_zavezanec_stateChanged() {

	if (ui->txt_zavezanec->isChecked()) {
		ui->txt_davcna->setInputMask("SI99999999;_");
	}
	else {
		ui->txt_davcna->setInputMask("99999999;_");
	}
}

void podjetje::on_btn_brisi_clicked() {

	ui->txt_davcna->setText("");
	ui->txt_email->setText("");
	ui->txt_ime->setText("");
	ui->txt_lastnik->setText("");
	ui->txt_logo->setText("");
	ui->txt_maticnastevilka->setText("");
	ui->txt_odgovorni->setText("");
	ui->txt_polnoime->setText("");
	ui->txt_skd->setText("");
	ui->txt_trr->setText("");
	ui->txt_url->setText("");
	ui->txt_telefon->setText("");
	ui->txt_gsm->setText("");

	ui->txt_naslov->clear();

	ui->txt_naziv->setCurrentIndex(0);

	ui->txt_zavezanec->setChecked(false);

}

void podjetje::on_btn_logo_clicked() {

	QString potdoslike = QFileDialog::getOpenFileName(this,
					tr("Izberite logotip"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));

	ui->txt_logo->setText(potdoslike);

	QImage slika(potdoslike);
	ui->gv_logo->setPixmap(QPixmap::fromImage(slika));

}

void podjetje::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_potrdi_clicked();
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

void podjetje::prejem(QString besedilo) {

	if (besedilo == "Novo podjetje") {
		ui->btn_potrdi->setText("Vnesi podjetje");
	}
	else {
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
			ui->btn_potrdi->setText("Popravi podjetje");
			QSqlQuery sql_fill;
			sql_fill.prepare("SELECT * FROM podjetje WHERE id LIKE '" + besedilo + "'");
			sql_fill.exec();
			if (sql_fill.next()) {
				if (prevedi(sql_fill.value(sql_fill.record().indexOf("ddvzavezanec")).toString()) == "DA") {
					ui->txt_zavezanec->setChecked(true);
				}
				else {
					ui->txt_zavezanec->setChecked(false);
				}
				ui->txt_id->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()));
				ui->txt_ime->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("ime")).toString()));
				ui->txt_polnoime->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("polnoime")).toString()));
				ui->txt_naslov->setPlainText(prevedi(sql_fill.value(sql_fill.record().indexOf("naslov")).toString()));
				ui->txt_davcna->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("davcnastevilka")).toString()));
				ui->txt_logo->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("logo")).toString()));

				QImage slika(ui->txt_logo->text());
				ui->gv_logo->setPixmap(QPixmap::fromImage(slika));

				ui->txt_email->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("email")).toString()));
				ui->txt_url->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("spletnastran")).toString()));
				ui->txt_maticnastevilka->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("maticnastevilka")).toString()));
				ui->txt_trr->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("trr")).toString()));
				ui->txt_skd->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("skd")).toString()));
				ui->txt_lastnik->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("lastnik")).toString()));
				ui->txt_odgovorni->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("odgovornaoseba")).toString()));

				bool ok;
				QSqlQuery sql_combo_naziv;
				sql_combo_naziv.prepare("SELECT * FROM sif_nazivi WHERE naziv LIKE '" + sql_fill.value(sql_fill.record().indexOf("nazivodgovorneosebe")).toString() + "'");
				sql_combo_naziv.exec();
				if (sql_combo_naziv.next()) {
					ui->txt_naziv->setCurrentIndex(prevedi(sql_combo_naziv.value(sql_combo_naziv.record().indexOf("id")).toString()).toInt(&ok, 10) - 1);
				}

				ui->txt_gsm->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("gsm")).toString()));
				ui->txt_telefon->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("telefon")).toString()));
			}
		}
		base.close();
	}

}

QString podjetje::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString podjetje::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

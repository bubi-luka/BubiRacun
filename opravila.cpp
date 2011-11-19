#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "opravila.h"
#include "ui_opravila.h"
#include "kodiranje.h"

opravila::opravila(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::opravila)
{
    ui->setupUi(this);

	// disable fields
	ui->txt_id->setEnabled(false);
	ui->txt_racun->setEnabled(false);

	ui->txt_s_znesek->setEnabled(false);
	ui->txt_popravljena_cena_ure->setEnabled(false);

	ui->txt_p_akcija->setEnabled(false);
	ui->txt_p_blog->setEnabled(false);
	ui->txt_p_facebook->setEnabled(false);
	ui->txt_p_forum->setEnabled(false);
	ui->txt_p_google->setEnabled(false);
	ui->txt_p_obrazec->setEnabled(false);
	ui->txt_p_sfacebook->setEnabled(false);
	ui->txt_p_skuponi->setEnabled(false);
	ui->txt_p_stranka->setEnabled(false);
	ui->txt_p_stwitter->setEnabled(false);
	ui->txt_p_twitter->setEnabled(false);
	ui->txt_p_vip->setEnabled(false);

	ui->txt_k_komunikacija->setEnabled(false);
	ui->txt_k_kratek->setEnabled(false);
	ui->txt_k_neumnosti->setEnabled(false);
	ui->txt_k_skupaj->setEnabled(false);
	ui->txt_k_vikend->setEnabled(false);
	ui->txt_k_zahtevnost->setEnabled(false);

	ui->txt_popust->setEnabled(false);
	ui->txt_p_skupaj->setEnabled(false);
	ui->txt_s_znesek->setEnabled(false);
	ui->txt_s_ddv->setEnabled(false);

	// fill combo box and tables
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

		// fill combo box
		ui->txt_storitev->addItem("");
		ui->txt_ddv->addItem("");

		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM sif_opravila");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_storitev->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("opravilo")).toString()));
		}

		ui->txt_ddv->addItem("0,0%");
		ui->txt_ddv->addItem("8,5%");
		ui->txt_ddv->addItem("20,0%");

	}
	base.close();
}

opravila::~opravila()
{
    delete ui;
}

void opravila::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_vnesi_clicked();
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

// prazno, dokler ne definiramo funkcije gumba
void opravila::on_btn_brisi_clicked() {

}

void opravila::on_btn_izhod_clicked() {

	close();

}

void opravila::on_btn_izracunaj_clicked() {

	double rezultat = 0;

	// popusti
	ui->txt_p_facebook->setText(ui->txt_p_facebook->text().remove("%").replace(",", "."));
	if (ui->txt_p_facebook->text() == "") {
		ui->txt_p_facebook->setText("0");
	}
	if (!ui->txt_p_facebook->text().contains(".")) {
		ui->txt_p_facebook->setText(ui->txt_p_facebook->text() + ".00");
	}
	ui->txt_p_twitter->setText(ui->txt_p_twitter->text().remove("%").replace(",", "."));
	if (ui->txt_p_twitter->text() == "") {
		ui->txt_p_twitter->setText("0");
	}
	if (!ui->txt_p_twitter->text().contains(".")) {
		ui->txt_p_twitter->setText(ui->txt_p_twitter->text() + ".00");
	}
	ui->txt_p_google->setText(ui->txt_p_google->text().remove("%").replace(",", "."));
	if (ui->txt_p_google->text() == "") {
		ui->txt_p_google->setText("0");
	}
	if (!ui->txt_p_google->text().contains(".")) {
		ui->txt_p_google->setText(ui->txt_p_google->text() + ".00");
	}
	ui->txt_p_blog->setText(ui->txt_p_blog->text().remove("%").replace(",", "."));
	if (ui->txt_p_blog->text() == "") {
		ui->txt_p_blog->setText("0");
	}
	if (!ui->txt_p_blog->text().contains(".")) {
		ui->txt_p_blog->setText(ui->txt_p_blog->text() + ".00");
	}
	ui->txt_p_forum->setText(ui->txt_p_forum->text().remove("%").replace(",", "."));
	if (ui->txt_p_forum->text() == "") {
		ui->txt_p_forum->setText("0");
	}
	if (!ui->txt_p_forum->text().contains(".")) {
		ui->txt_p_forum->setText(ui->txt_p_forum->text() + ".00");
	}
	ui->txt_p_sfacebook->setText(ui->txt_p_sfacebook->text().remove("%").replace(",", "."));
	if (ui->txt_p_sfacebook->text() == "") {
		ui->txt_p_sfacebook->setText("0");
	}
	if (!ui->txt_p_sfacebook->text().contains(".")) {
		ui->txt_p_sfacebook->setText(ui->txt_p_sfacebook->text() + ".00");
	}
	ui->txt_p_stwitter->setText(ui->txt_p_stwitter->text().remove("%").replace(",", "."));
	if (ui->txt_p_stwitter->text() == "") {
		ui->txt_p_stwitter->setText("0");
	}
	if (!ui->txt_p_stwitter->text().contains(".")) {
		ui->txt_p_stwitter->setText(ui->txt_p_stwitter->text() + ".00");
	}
	ui->txt_p_skuponi->setText(ui->txt_p_skuponi->text().remove("%").replace(",", "."));
	if (ui->txt_p_skuponi->text() == "") {
		ui->txt_p_skuponi->setText("0");
	}
	if (!ui->txt_p_skuponi->text().contains(".")) {
		ui->txt_p_skuponi->setText(ui->txt_p_skuponi->text() + ".00");
	}
	ui->txt_p_obrazec->setText(ui->txt_p_obrazec->text().remove("%").replace(",", "."));
	if (ui->txt_p_obrazec->text() == "") {
		ui->txt_p_obrazec->setText("0");
	}
	if (!ui->txt_p_obrazec->text().contains(".")) {
		ui->txt_p_obrazec->setText(ui->txt_p_obrazec->text() + ".00");
	}
	ui->txt_p_vip->setText(ui->txt_p_vip->text().remove("%").replace(",", "."));
	if (ui->txt_p_vip->text() == "") {
		ui->txt_p_vip->setText("0");
	}
	if (!ui->txt_p_vip->text().contains(".")) {
		ui->txt_p_vip->setText(ui->txt_p_vip->text() + ".00");
	}
	ui->txt_p_akcija->setText(ui->txt_p_akcija->text().remove("%").replace(",", "."));
	if (ui->txt_p_akcija->text() == "") {
		ui->txt_p_akcija->setText("0");
	}
	if (!ui->txt_p_akcija->text().contains(".")) {
		ui->txt_p_akcija->setText(ui->txt_p_akcija->text() + ".00");
	}
	ui->txt_p_stranka->setText(ui->txt_p_stranka->text().remove("%").replace(",", "."));
	if (ui->txt_p_stranka->text() == "") {
		ui->txt_p_stranka->setText("0");
	}
	if (!ui->txt_p_stranka->text().contains(".")) {
		ui->txt_p_stranka->setText(ui->txt_p_stranka->text() + ".00");
	}

	ui->txt_p_skupaj->setText(QString::number(ui->txt_p_facebook->text().toDouble() + ui->txt_p_twitter->text().toDouble() + ui->txt_p_google->text().toDouble() +
												  ui->txt_p_blog->text().toDouble() + ui->txt_p_forum->text().toDouble() + ui->txt_p_sfacebook->text().toDouble() +
												  ui->txt_p_stwitter->text().toDouble() + ui->txt_p_skuponi->text().toDouble() + ui->txt_p_obrazec->text().toDouble() +
												  ui->txt_p_vip->text().toDouble() + ui->txt_p_akcija->text().toDouble() +
												  ui->txt_p_stranka->text().toDouble(), 'f', 2));

	// podrazitve
	rezultat = 0;

	ui->txt_k_vikend->setText(ui->txt_k_vikend->text().remove("%").replace(",", "."));
	if (ui->txt_k_vikend->text() == "") {
		ui->txt_k_vikend->setText("0");
	}
	if (!ui->txt_k_vikend->text().contains(".")) {
		ui->txt_k_vikend->setText(ui->txt_k_vikend->text() + ".00");
	}
	ui->txt_k_kratek->setText(ui->txt_k_kratek->text().remove("%").replace(",", "."));
	if (ui->txt_k_kratek->text() == "") {
		ui->txt_k_kratek->setText("0");
	}
	if (!ui->txt_k_kratek->text().contains(".")) {
		ui->txt_k_kratek->setText(ui->txt_k_kratek->text() + ".00");
	}
	ui->txt_k_neumnosti->setText(ui->txt_k_neumnosti->text().remove("%").replace(",", "."));
	if (ui->txt_k_neumnosti->text() == "") {
		ui->txt_k_neumnosti->setText("0");
	}
	if (!ui->txt_k_neumnosti->text().contains(".")) {
		ui->txt_k_neumnosti->setText(ui->txt_k_neumnosti->text() + ".00");
	}
	ui->txt_k_zahtevnost->setText(ui->txt_k_zahtevnost->text().remove("%").replace(",", "."));
	if (ui->txt_k_zahtevnost->text() == "") {
		ui->txt_k_zahtevnost->setText("0");
	}
	if (!ui->txt_k_zahtevnost->text().contains(".")) {
		ui->txt_k_zahtevnost->setText(ui->txt_k_zahtevnost->text() + ".00");
	}
	ui->txt_k_komunikacija->setText(ui->txt_k_komunikacija->text().remove("%").replace(",", "."));
	if (ui->txt_k_komunikacija->text() == "") {
		ui->txt_k_komunikacija->setText("0");
	}
	if (!ui->txt_k_komunikacija->text().contains(".")) {
		ui->txt_k_komunikacija->setText(ui->txt_k_komunikacija->text() + ".00");
	}

	ui->txt_k_skupaj->setText(QString::number(ui->txt_k_vikend->text().toDouble() + ui->txt_k_kratek->text().toDouble() +
											  ui->txt_k_neumnosti->text().toDouble() + ui->txt_k_zahtevnost->text().toDouble() +
											  ui->txt_k_komunikacija->text().toDouble(), 'f', 2));

	// izracun za racun
	rezultat = 0;

	ui->txt_cena->setText(ui->txt_cena->text().replace(",", "."));
	ui->txt_ure->setText(ui->txt_ure->text().replace(",", "."));
	ui->txt_popravljena_cena_ure->setText(QString::number(ui->txt_cena->text().toDouble() * (1 + ui->txt_k_skupaj->text().toDouble()/100), 'f', 2));

	ui->txt_znesekbrezddv->setText(QString::number(ui->txt_popravljena_cena_ure->text().toDouble() * ui->txt_ure->text().toDouble(), 'f', 2));
	ui->txt_popust->setText(QString::number(ui->txt_znesekbrezddv->text().toDouble() * ui->txt_p_skupaj->text().toDouble()/100 , 'f', 2));
	ui->txt_s_ddv->setText(QString::number((ui->txt_znesekbrezddv->text().toDouble() - ui->txt_popust->text().toDouble()) * ui->txt_ddv->currentText().left(ui->txt_ddv->currentText().length() - 1).replace(",", ".").toDouble()/100, 'f', 2));
	ui->txt_s_znesek->setText(QString::number(ui->txt_znesekbrezddv->text().toDouble() - ui->txt_popust->text().toDouble() + ui->txt_s_ddv->text().toDouble(), 'f', 2));

}

void opravila::on_btn_vnesi_clicked() {

	on_btn_izracunaj_clicked();

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
		QSqlQuery sql_vnesi_opravilo;
		if (ui->btn_vnesi->text() == "Vnesi opravilo") { // vnesi novega uporabnika
			sql_vnesi_opravilo.prepare("INSERT INTO opravila (racun, storitev, ure, cena_ure, ddv, p_facebook, p_twitter, p_google, p_blog, "
									   "p_forum, p_sfacebook, p_stwitter, p_skuponi, p_obrazec, p_kupon, p_akcija, p_vip, popusti, k_vikend, "
									   "k_kratekrok, k_zahtevnost, k_neumnosti, k_komunikacija, kontrapopusti, znesekbrezddv, znesekddv, znesekpopust, znesekskupaj) "
									   "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

		}
		else { // popravi ze obstojeci vnos
			sql_vnesi_opravilo.prepare("UPDATE opravila SET racun = ?, storitev = ?, ure = ?, cena_ure = ?, ddv = ?, p_facebook = ?, p_twitter = ?, "
									   "p_google = ?, p_blog = ?, p_forum = ?, p_sfacebook = ?, p_stwitter = ?, p_skuponi = ?, p_obrazec = ?, p_kupon = ?, "
									   "p_akcija = ?, p_vip = ?, popusti = ?, k_vikend = ?, k_kratekrok = ?, k_zahtevnost = ?, k_neumnosti = ?, "
									   "k_komunikacija = ?, kontrapopusti = ?, znesekbrezddv = ?, znesekddv = ?, znesekpopust, = ?, znesekskupaj = ? "
									   "WHERE id LIKE '" + ui->txt_id->text() + "'");
		}
		sql_vnesi_opravilo.bindValue(0, pretvori(ui->txt_racun->text()));
		sql_vnesi_opravilo.bindValue(1, pretvori(ui->txt_storitev->currentText()));
		sql_vnesi_opravilo.bindValue(2, pretvori(ui->txt_ure->text()));
		sql_vnesi_opravilo.bindValue(3, pretvori(ui->txt_cena->text()));
		sql_vnesi_opravilo.bindValue(4, pretvori(ui->txt_ddv->currentText()));
		sql_vnesi_opravilo.bindValue(5, pretvori(ui->txt_p_facebook->text()));
		sql_vnesi_opravilo.bindValue(6, pretvori(ui->txt_p_twitter->text()));
		sql_vnesi_opravilo.bindValue(7, pretvori(ui->txt_p_google->text()));
		sql_vnesi_opravilo.bindValue(8, pretvori(ui->txt_p_blog->text()));
		sql_vnesi_opravilo.bindValue(9, pretvori(ui->txt_p_forum->text()));
		sql_vnesi_opravilo.bindValue(10, pretvori(ui->txt_p_sfacebook->text()));
		sql_vnesi_opravilo.bindValue(11, pretvori(ui->txt_p_stwitter->text()));
		sql_vnesi_opravilo.bindValue(12, pretvori(ui->txt_p_skuponi->text()));
		sql_vnesi_opravilo.bindValue(13, pretvori(ui->txt_p_obrazec->text()));
		sql_vnesi_opravilo.bindValue(14, pretvori(ui->txt_p_stranka->text()));
		sql_vnesi_opravilo.bindValue(15, pretvori(ui->txt_p_akcija->text()));
		sql_vnesi_opravilo.bindValue(16, pretvori(ui->txt_p_vip->text()));
		sql_vnesi_opravilo.bindValue(17, pretvori(ui->txt_p_skupaj->text()));
		sql_vnesi_opravilo.bindValue(18, pretvori(ui->txt_k_vikend->text()));
		sql_vnesi_opravilo.bindValue(19, pretvori(ui->txt_k_kratek->text()));
		sql_vnesi_opravilo.bindValue(20, pretvori(ui->txt_k_zahtevnost->text()));
		sql_vnesi_opravilo.bindValue(21, pretvori(ui->txt_k_neumnosti->text()));
		sql_vnesi_opravilo.bindValue(22, pretvori(ui->txt_k_komunikacija->text()));
		sql_vnesi_opravilo.bindValue(23, pretvori(ui->txt_k_skupaj->text()));
		sql_vnesi_opravilo.bindValue(24, pretvori(ui->txt_znesekbrezddv->text()));
		sql_vnesi_opravilo.bindValue(25, pretvori(ui->txt_s_ddv->text()));
		sql_vnesi_opravilo.bindValue(26, pretvori(ui->txt_popust->text()));
		sql_vnesi_opravilo.bindValue(27, pretvori(ui->txt_s_znesek->text()));
		sql_vnesi_opravilo.exec();
	}

	base.close();

	// send signal to reload widget
	poslji("opravila");

	// close this window
	close();

}

void opravila::on_txt_storitev_currentIndexChanged(QString besedilo) {

	if (ui->btn_vnesi->text() != "Vnesi") {
		if (besedilo != "") {
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
				sql_napolni.prepare("SELECT * FROM sif_opravila WHERE opravilo LIKE '" + pretvori(besedilo) + "'");
				sql_napolni.exec();
				if (sql_napolni.next()) {
					int i = 0;
					if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()) == "0.0") {
						i = 1;
					}
					else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()) == "8.5") {
						i = 2;
					}
					else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()) == "20.0") {
						i = 3;
					}
					ui->txt_ddv->setCurrentIndex(i);
					ui->txt_cena->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena")).toString()));
				}
			}
			base.close();
		}
		else {
			ui->txt_ddv->setCurrentIndex(0);
		}
	}

}

void opravila::prejem(QString besedilo) {

	QString id = "";

	if (besedilo.contains("Novo opravilo")) {
		ui->btn_vnesi->setText("Vnesi opravilo");
		ui->txt_racun->setText(besedilo.right(besedilo.length() - 13));
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
			QSqlQuery sql_stranka;
			sql_stranka.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
			sql_stranka.exec();
			if ( sql_stranka.next() ) {
				id = prevedi(sql_stranka.value(sql_stranka.record().indexOf("stranka")).toString());
			}

			QSqlQuery sql_popusti;
			sql_popusti.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(id) + "'");
			sql_popusti.exec();
			if (sql_popusti.next()) {
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("facebook")).toString()) == "facebook" ) {
					ui->txt_p_facebook->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_facebook")).toString()));
					ui->cb_facebook->setChecked(true);
				}
				else {
					ui->cb_facebook->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("twitter")).toString()) == "twitter" ) {
					ui->txt_p_twitter->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_twitter")).toString()));
					ui->cb_twitter->setChecked(true);
				}
				else {
					ui->cb_twitter->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("google")).toString()) == "google" ) {
					ui->txt_p_google->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_google")).toString()));
					ui->cb_google->setChecked(true);
				}
				else {
					ui->cb_google->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("blog")).toString()) == "blog" ) {
					ui->txt_p_blog->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_blog")).toString()));
					ui->cb_blog->setChecked(true);
				}
				else {
					ui->cb_blog->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("forum")).toString()) == "forum" ) {
					ui->txt_p_forum->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_forum")).toString()));
					ui->cb_forum->setChecked(true);
				}
				else {
					ui->cb_forum->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("s_facebook")).toString()) == "sfacebook" ) {
					ui->txt_p_sfacebook->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_s_facebook")).toString()));
					ui->cb_sfacebook->setChecked(true);
				}
				else {
					ui->cb_sfacebook->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("s_twitter")).toString()) == "stwitter" ) {
					ui->txt_p_stwitter->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_s_twitter")).toString()));
					ui->cb_stwitter->setChecked(true);
				}
				else {
					ui->cb_stwitter->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("s_kuponi")).toString()) == "skuponi" ) {
					ui->txt_p_skuponi->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_s_kuponi")).toString()));
					ui->cb_skuponi->setChecked(true);
				}
				else {
					ui->cb_skuponi->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("obrazec")).toString()) == "obrazec" ) {
					ui->txt_p_obrazec->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_obrazec")).toString()));
					ui->cb_obrazec->setChecked(true);
				}
				else {
					ui->cb_obrazec->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("vip")).toString()) == "vip" ) {
					ui->txt_p_vip->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_vip")).toString()));
					ui->cb_vip->setChecked(true);
				}
				else {
					ui->cb_vip->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("akcija")).toString()) == "akcija" ) {
					ui->txt_p_akcija->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_akcija")).toString()));
					ui->cb_akcija->setChecked(true);
				}
				else {
					ui->cb_akcija->setChecked(false);
				}
				if ( prevedi(sql_popusti.value(sql_popusti.record().indexOf("kuponi")).toString()) == "kuponi" ) {
					ui->txt_p_stranka->setText(prevedi(sql_popusti.value(sql_popusti.record().indexOf("pop_kuponi")).toString()));
					ui->cb_kupon->setChecked(true);
				}
				else {
					ui->cb_kupon->setChecked(false);
				}
			}
		}
		base.close();
	}
	else {
		// besedilo nosi ID ze obstojeco stranko, potrebno je napolniti polja
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
			sql_napolni.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(besedilo) + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
				ui->txt_racun->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("racun")).toString()));

				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM sif_opravila WHERE opravilo LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("storitev")).toString() + "'");
				sql_combo.exec();
				if ( sql_combo.next() ) {
					ui->txt_storitev->setCurrentIndex(prevedi(sql_combo.value(sql_combo.record().indexOf("id")).toString()).toInt());
				}

				ui->txt_ure->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ure")).toString()));
				ui->txt_cena->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena_ure")).toString()));

				int i = 0;
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()) == "20.0%" ) {
					i = 3;
				}
				else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()) == "8.5%" ) {
					i = 2;
				}
				else if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("ddv")).toString()) == "0.0%" ) {
					i = 1;
				}

				ui->txt_ddv->setCurrentIndex(i);

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_facebook")).toString()) != "0.00" ) {
					ui->txt_p_facebook->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_facebook")).toString()));
					ui->cb_facebook->setChecked(true);
				}
				else {
					ui->cb_facebook->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_twitter")).toString()) != "0.00" ) {
					ui->txt_p_twitter->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_twitter")).toString()));
					ui->cb_twitter->setChecked(true);
				}
				else {
					ui->cb_twitter->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_google")).toString()) != "0.00" ) {
					ui->txt_p_google->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_google")).toString()));
					ui->cb_google->setChecked(true);
				}
				else {
					ui->cb_google->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_blog")).toString()) != "0.00" ) {
					ui->txt_p_blog->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_blog")).toString()));
					ui->cb_blog->setChecked(true);
				}
				else {
					ui->cb_blog->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_forum")).toString()) != "0.00" ) {
					ui->txt_p_forum->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_forum")).toString()));
					ui->cb_forum->setChecked(true);
				}
				else {
					ui->cb_forum->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_sfacebook")).toString()) != "0.00" ) {
					ui->txt_p_sfacebook->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_sfacebook")).toString()));
					ui->cb_sfacebook->setChecked(true);
				}
				else {
					ui->cb_sfacebook->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_stwitter")).toString()) != "0.00" ) {
					ui->txt_p_stwitter->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_stwitter")).toString()));
					ui->cb_stwitter->setChecked(true);
				}
				else {
					ui->cb_stwitter->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_skuponi")).toString()) != "0.00" ) {
					ui->txt_p_skuponi->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_skuponi")).toString()));
					ui->cb_skuponi->setChecked(true);
				}
				else {
					ui->cb_skuponi->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_obrazec")).toString()) != "0.00" ) {
					ui->txt_p_obrazec->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_obrazec")).toString()));
					ui->cb_obrazec->setChecked(true);
				}
				else {
					ui->cb_obrazec->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_kupon")).toString()) != "0.00" ) {
					ui->txt_p_stranka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_kupon")).toString()));
					ui->cb_kupon->setChecked(true);
				}
				else {
					ui->cb_kupon->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_akcija")).toString()) != "0.00" ) {
					ui->txt_p_akcija->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_akcija")).toString()));
					ui->cb_akcija->setChecked(true);
				}
				else {
					ui->cb_akcija->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_vip")).toString()) != "0.00" ) {
					ui->txt_p_vip->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("p_vip")).toString()));
					ui->cb_vip->setChecked(true);
				}
				else {
					ui->cb_vip->setChecked(false);
				}

				ui->txt_p_skupaj->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popusti")).toString()));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_vikend")).toString()) != "0.00" ) {
					ui->txt_k_vikend->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_vikend")).toString()));
					ui->cb_k_vikend->setChecked(true);
				}
				else {
					ui->cb_k_vikend->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_kratekrok")).toString()) != "0.00" ) {
					ui->txt_k_kratek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_kratekrok")).toString()));
					ui->cb_k_kratek->setChecked(true);
				}
				else {
					ui->cb_k_kratek->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_zahtevnost")).toString()) != "0.00" ) {
					ui->txt_k_zahtevnost->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_zahtevnost")).toString()));
					ui->cb_k_zahtevnost->setChecked(true);
				}
				else {
					ui->cb_k_zahtevnost->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_neumnosti")).toString()) != "0.00" ) {
					ui->txt_k_neumnosti->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_neumnosti")).toString()));
					ui->cb_k_neumnost->setChecked(true);
				}
				else {
					ui->cb_k_neumnost->setChecked(false);
				}
				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_komunikacija")).toString()) != "0.00" ) {
					ui->txt_k_komunikacija->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("k_komunikacija")).toString()));
					ui->cb_k_komunikacija->setChecked(true);
				}
				else {
					ui->cb_k_komunikacija->setChecked(false);
				}

				ui->txt_k_skupaj->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kontrapopusti")).toString()));

				ui->txt_znesekbrezddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesekbrezddv")).toString()));
				ui->txt_s_ddv->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesekddv")).toString()));
				ui->txt_s_znesek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesekskupaj")).toString()));
			}
		}
		base.close();

		ui->btn_vnesi->setText("Popravi opravilo");

		on_btn_izracunaj_clicked();

	}
}

QString opravila::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString opravila::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void opravila::on_cb_facebook_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_p_facebook->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_facebook")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_facebook->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_facebook")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_facebook->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Facebook osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_facebook->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_facebook->setText("0.00");
		ui->txt_p_facebook->setEnabled(false);
	}

}

void opravila::on_cb_twitter_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_twitter->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_twitter")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_twitter->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_twitter")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_twitter")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_twitter->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_twitter")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Twitter osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_twitter->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_twitter->setText("0.00");
		ui->txt_p_twitter->setEnabled(false);
	}

}

void opravila::on_cb_google_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_google->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_google")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_google->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_google")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_google")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_google->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_google")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Google plus osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_google->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_google->setText("0.00");
		ui->txt_p_google->setEnabled(false);
	}

}

void opravila::on_cb_blog_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_blog->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_blog")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_blog->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_blog")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_blog")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_blog->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_blog")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Blog osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_blog->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_blog->setText("0.00");
		ui->txt_p_blog->setEnabled(false);
	}

}

void opravila::on_cb_forum_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_forum->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_forum")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_forum->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_forum")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_forum")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_forum->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_forum")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Forum osebni") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_forum->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_forum->setText("0.00");
		ui->txt_p_forum->setEnabled(false);
	}
}

void opravila::on_cb_sfacebook_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_sfacebook->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_sfacebook")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_sfacebook->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_sfacebook")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_facebook")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_sfacebook->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_facebook")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Facebook sirjenje") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_sfacebook->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_sfacebook->setText("0.00");
		ui->txt_p_sfacebook->setEnabled(false);
	}

}

void opravila::on_cb_stwitter_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_stwitter->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_stwitter")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_stwitter->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_stwitter")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_twitter")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_stwitter->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_twitter")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Twitter sirjenje") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_stwitter->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_stwitter->setText("0.00");
		ui->txt_p_stwitter->setEnabled(false);
	}

}

void opravila::on_cb_skuponi_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_skuponi->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_skuponi")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_skuponi->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_skuponi")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_kuponi")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_skuponi->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_s_kuponi")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Kuponi - pridobil stranke") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_skuponi->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_skuponi->setText("0.00");
		ui->txt_p_skuponi->setEnabled(false);
	}
}

void opravila::on_cb_obrazec_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_obrazec->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_obrazec")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_obrazec->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_obrazec")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_obrazec")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_obrazec->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_obrazec")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Obrazec akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_obrazec->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_obrazec->setText("0.00");
		ui->txt_p_obrazec->setEnabled(false);
	}

}

void opravila::on_cb_kupon_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_stranka->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_kupon")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_stranka->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_kupon")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kuponi")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_stranka->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kuponi")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Kupon akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_stranka->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_stranka->setText("0.00");
		ui->txt_p_stranka->setEnabled(false);
	}

}

void opravila::on_cb_akcija_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_akcija->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_akcija")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_akcija->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_akcija")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_akcija->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Akcijski popust akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_akcija->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_akcija->setText("0.00");
		ui->txt_p_akcija->setEnabled(false);
	}

}

void opravila::on_cb_vip_toggled(bool stanje){

	if ( stanje == true ) {
		ui->txt_p_vip->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_vip")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_p_vip->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("p_vip")).toString()));
					nipopusta = "false";
				}
			}
			else {
				QSqlQuery sql_racun;
				sql_racun.prepare("SELECT * FROM racuni WHERE stracuna LIKE '" + pretvori(ui->txt_racun->text()) + "'");
				sql_racun.exec();
				if ( sql_racun.next() ) {
					QSqlQuery sql_stranke;
					sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_racun.value(sql_racun.record().indexOf("stranka")).toString() + "'");
					sql_stranke.exec();
					if ( sql_stranke.next() ) {
						if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_vip")).toString()) != "0.00" ) { // opravilo ima popust
							ui->txt_p_vip->setText(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_vip")).toString()));
							nipopusta = "false";
						}
					}
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Stalna stranka akcija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_p_vip->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_p_vip->setText("0.00");
		ui->txt_p_vip->setEnabled(false);
	}

}

void opravila::on_cb_k_vikend_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_k_vikend->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_vikend")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_k_vikend->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_vikend")).toString()));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Vikend") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_k_vikend->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_k_vikend->setText("0.00");
		ui->txt_k_vikend->setEnabled(false);
	}

}

void opravila::on_cb_k_kratek_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_k_kratek->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_kratekrok")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_k_kratek->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_kratekrok")).toString()));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Kratek rok") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_k_kratek->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_k_kratek->setText("0.00");
		ui->txt_k_kratek->setEnabled(false);
	}

}

void opravila::on_cb_k_zahtevnost_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_k_zahtevnost->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_zahtevnost")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_k_zahtevnost->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_zahtevnost")).toString()));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Zahtevna stranka") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_k_zahtevnost->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_k_zahtevnost->setText("0.00");
		ui->txt_k_zahtevnost->setEnabled(false);
	}

}

void opravila::on_cb_k_neumnost_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_k_neumnosti->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_neumnosti")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_k_neumnosti->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_neumnosti")).toString()));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Neumnosti") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_k_neumnosti->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_k_neumnosti->setText("0.00");
		ui->txt_k_neumnosti->setEnabled(false);
	}

}

void opravila::on_cb_k_komunikacija_toggled(bool stanje) {

	if ( stanje == true ) {
		ui->txt_k_komunikacija->setEnabled(true);

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
			QString nipopusta = "true";

			QSqlQuery sql_opravila;
			sql_opravila.prepare("SELECT * FROM opravila WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
			sql_opravila.exec();
			if ( sql_opravila.next() ) {
				if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_komunikacija")).toString()) != "0.00" ) { // opravilo ima popust
					ui->txt_k_komunikacija->setText(prevedi(sql_opravila.value(sql_opravila.record().indexOf("k_komunikacija")).toString()));
					nipopusta = "false";
				}
			}

			if ( nipopusta == "true" ) {
				QSqlQuery sql_osnova;
				sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("Komunikacija") + "'");
				sql_osnova.exec();
				if ( sql_osnova.next() ) {
					ui->txt_k_komunikacija->setText(prevedi(sql_osnova.value(sql_osnova.record().indexOf("procent")).toString()));
				}
			}

		}
		base.close();
	}
	else {
		ui->txt_k_komunikacija->setText("0.00");
		ui->txt_k_komunikacija->setEnabled(false);
	}

}

#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>

#include "baza.h"
#include "setup.h"

QString baza::str_name = "";
QString baza::str_path = "";
QString baza::str_type = "";

baza::baza() {
}

baza::~baza() {
}

// Getters
QString baza::get_name() {
	return str_name;
}

QString baza::get_path() {
	return str_path;
}

QString baza::get_type() {
	return str_type;
}

// Setters
void baza::set_name(QString name) {
	str_name = name;
}

void baza::set_path(QString path) {
	str_path = path;
}

void baza::set_type(QString type) {
	str_type = type;
}

// Methods
void baza::new_database() {

	QMessageBox msgbox;
	msgbox.setText("Kreiranje nove baze");
	msgbox.setInformativeText("Kreiranje in posodabljanje nove baze lahko traja nekaj minut. V tem casu je program lahko neodziven. Prosimo za potrpljenje.");
	msgbox.exec();

	QString path = QFileDialog::getSaveFileName(0, "Izberi ime baze podatkov", QApplication::applicationDirPath(), "Baza (*.bz);;Arhiv baze (*.bz.bck)");
	if ( path.right(3) != ".bz" && path.right(7) != ".bz.bck" ) {
		path.append(".bz");
	}

	if ( path != "" ) {
		set_name("osnovna-baza");
		set_path(path);
		set_type("QSQLITE");

		QSettings nastavitve("BubiTech", "BubiRacun");
		nastavitve.setValue("name", get_name());
		nastavitve.setValue("path", get_path());
		nastavitve.setValue("type", get_type());

		open_database();

		// dodamo prvo tabelo, da se baza podatkov tudi shrani na disk
		setup().start_first_run();

	} // if
	else {
		exit(0);
	} // else

}

void baza::ask_for_database() {

	QString path = QFileDialog::getOpenFileName(0, "Izberite bazo podatkov", QApplication::applicationDirPath(), "Baza (*.bz);;Arhiv baze (*.bz.bck)");

	if ( path != "" ) {
		set_name("osnovna-baza");
		set_path(path);
		set_type("QSQLITE");

		QSettings nastavitve("BubiTech", "BubiRacun");
		nastavitve.setValue("name", get_name());
		nastavitve.setValue("path", get_path());
		nastavitve.setValue("type", get_type());

		open_database();

	} // if
	else {
		exit(0);
	} // else
}

void baza::open_database() {

	QSqlDatabase nova_baza = QSqlDatabase::addDatabase(get_type());
	nova_baza.setDatabaseName(get_path());
	nova_baza.database();
	nova_baza.open();
	if(nova_baza.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}

	//	emit database_opened();

}

void baza::close_database() {

	QString ime_baze = QSqlDatabase::database().connectionName();

	QSqlDatabase::database().close();
	QSqlDatabase::database().removeDatabase(ime_baze);

// emit database_closed();

}

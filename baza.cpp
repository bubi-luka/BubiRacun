#include <QtSql>
#include <QMessageBox>

#include "baza.h"

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
void baza::open_database() {

	QSqlDatabase base = QSqlDatabase::addDatabase(get_type());
	base.setDatabaseName(get_path());
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
//	emit database_opened();

}

void baza::close_database() {

	QString ime_baze = "";
	ime_baze = QSqlDatabase::database().connectionName();

	QSqlDatabase::database().close();
	QSqlDatabase::database().removeDatabase(ime_baze);

// emit database_closed();

}

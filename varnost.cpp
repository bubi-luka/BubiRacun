#include <QtSql>
#include <QMessageBox>

#include "varnost.h"

varnost::varnost(int &argc, char *argv[]) :
QApplication(argc, argv) {

}

varnost::~varnost() {

}

// Getters
QString varnost::id() const {

	return m_id;

}

QString varnost::name() const {

	return m_name;
}

QString varnost::surname() const {

	return m_surname;

}

QString varnost::permission() const {

	return m_permission;

}

QString varnost::state() const {

	return m_state;

}

//Setters
void varnost::set_id(const QString &id) {

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
		// the database is opened
		QSqlQuery sql_set_user_data;
		sql_set_user_data.prepare("SELECT * FROM uporabniki WHERE user_name LIKE '" + id + "'");
		sql_set_user_data.exec();
		if ( sql_set_user_data.next() ) {
			m_id = sql_set_user_data.value(sql_set_user_data.record().indexOf("id")).toString();
			m_name = sql_set_user_data.value(sql_set_user_data.record().indexOf("ime")).toString();
			m_surname = sql_set_user_data.value(sql_set_user_data.record().indexOf("priimek")).toString();
		//	set_permission(sql_set_user_data.value(sql_set_user_data.record().indexOf("dovoljenje")).toString());
			set_permission("Direktor");
		}
	}
	base.close();


	emit id_changed();

}

void varnost::set_permission(const QString &permission)	{

	m_permission = permission;
	emit permission_changed();

}

void varnost::set_state(const QString &state) {

	m_state = state;
	emit state_changed();

}

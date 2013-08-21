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

QString varnost::firm() const {

    return m_firm;

}

QString varnost::state() const {

    return m_state;

}

//Setters
void varnost::set_id(const QString &id) {

    if ( id != "" ) {
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "varnost-set-id");
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
            sql_set_user_data.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + id + "'");
            sql_set_user_data.exec();
            if ( sql_set_user_data.next() ) {
                m_id = sql_set_user_data.value(sql_set_user_data.record().indexOf("id")).toString();
                m_name = sql_set_user_data.value(sql_set_user_data.record().indexOf("ime")).toString();
                m_surname = sql_set_user_data.value(sql_set_user_data.record().indexOf("priimek")).toString();
                m_firm = sql_set_user_data.value(sql_set_user_data.record().indexOf("podjetje")).toString();

                QSqlQuery sql_set_permission_data;
                sql_set_permission_data.prepare("SELECT * FROM sif_dovoljenja WHERE id LIKE '" + sql_set_user_data.value(sql_set_user_data.record().indexOf("dovoljenje")).toString() + "'");
                sql_set_permission_data.exec();
                if ( sql_set_permission_data.next() ) {
                    set_permission(sql_set_permission_data.value(sql_set_permission_data.record().indexOf("dovoljenje")).toString());
                }
            }
        }
        base.close();
    }
    else {
        m_id = "";
        m_name = "";
        m_surname = "";
        m_firm = "";
        m_permission = "";
        m_state = "";
    }

    emit id_changed();

}

void varnost::set_permission(const QString &permission)	{

    m_permission = permission;
    emit permission_changed();

}

void varnost::set_state(const QString &state) {

    /**
        *	stanje "public" je odprto za vse radovedneze
        *	stanje "private" je odprto samo za zaposlene, da vse podatke
        */

    m_state = state;
    emit state_changed();

}

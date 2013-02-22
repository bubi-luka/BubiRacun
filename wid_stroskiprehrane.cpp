#include <QtSql>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QTextEdit>

#include "kodiranje.h"
#include "varnost.h"
#include "wid_stroskiprehrane.h"
#include "ui_wid_stroskiprehrane.h"

wid_stroskiprehrane::wid_stroskiprehrane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_stroskiprehrane)
{
    ui->setupUi(this);

    izbrisi_tabelo();
    izbrisi_leta();

    napolni_leta();

}

wid_stroskiprehrane::~wid_stroskiprehrane()
{
    delete ui;
}

void wid_stroskiprehrane::izbrisi_leta() {

    ui->cb_leto->clear();

}

void wid_stroskiprehrane::izbrisi_tabelo() {

}

void wid_stroskiprehrane::napolni_leta() {

    // filtriraj po letu
    ui->cb_leto->addItem("");
    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prehrana-napolni-leta");
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

        QSqlQuery sql_napolni;
        sql_napolni.prepare("SELECT * FROM stroski_prehrane WHERE izplacilo_dni_" + pretvori(vApp->id()) + " IS NOT NULL");
        sql_napolni.exec();
        while ( sql_napolni.next() ) {
            QString leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("leto")).toString());
            if ( ui->cb_leto->findText(leto) == -1 ) {
                ui->cb_leto->addItem(leto);
            }
        }
        sql_napolni.clear();

        // razvrscanje let po vrsti
        QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->cb_leto);
        proxy->setSourceModel(ui->cb_leto->model());
        // spustni seznam prepisemo
        ui->cb_leto->model()->setParent(proxy);
        ui->cb_leto->setModel(proxy);
        // razvrsti
        ui->cb_leto->model()->sort(0);

        // privzeto izberi trenutno leto
        ui->cb_leto->setCurrentIndex(ui->cb_leto->findText(QDate::currentDate().toString("yyyy")));
    }
    base.close();

}

void wid_stroskiprehrane::napolni_tabelo() {

}

QString wid_stroskiprehrane::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_stroskiprehrane::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

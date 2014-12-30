#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"
#include "varnost.h"
#include "sif_delavniki.h"
#include "ui_sif_delavniki.h"

sif_delavniki::sif_delavniki(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sif_delavniki)
{
    ui->setupUi(this);

    ui->txt_status->clear();
    napolni_leta();

}

sif_delavniki::~sif_delavniki()
{
    delete ui;
}

void sif_delavniki::on_btn_izhod_clicked() {

    close();

}

void sif_delavniki::on_btn_brisi_clicked() {

    if ( ui->txt_leto->currentText() != "" ) {
        // nastavi polja iz baze
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "brisi_leto");
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
            sql_napolni.prepare("DELETE FROM stroski_prehrane WHERE leto = '" + pretvori(ui->txt_leto->currentText()) + "'");
            sql_napolni.exec();

            QMessageBox msgbox;
            msgbox.setText("Operacija koncana");
            msgbox.setInformativeText("Podatki za leto " + ui->txt_leto->currentText() + " so izbrisani!");
            msgbox.exec();

            base.close();

        }
    }
    napolni_leta();

}

void sif_delavniki::napolni_leta() {

    ui->txt_leto->clear();
    ui->txt_leto->addItem("");

    // nastavi polja iz baze
    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "vnesi-leta");
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

        QSqlQuery sql_leta;
        sql_leta.prepare("SELECT * FROM stroski_prehrane");
        sql_leta.exec();
        while ( sql_leta.next() ) {
            QString leto = prevedi(sql_leta.value(sql_leta.record().indexOf("leto")).toString());
            if ( ui->txt_leto->findText(leto) == -1 ) {
                ui->txt_leto->addItem(leto);
            }
        }
        base.close();
    }

    // urejevanje vrstenga reda zapisov
    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->txt_leto);
    proxy->setSourceModel(ui->txt_leto->model());
    ui->txt_leto->model()->setParent(proxy);
    ui->txt_leto->setModel(proxy);
    ui->txt_leto->model()->sort(0, Qt::DescendingOrder);

}

void sif_delavniki::on_btn_osvezi_clicked() {

    ui->txt_status->appendPlainText("Pričenjam");

    // nastavi pot do spletnega naslova
    QString pot;

    pot = "http://www.bubi.si/bubiracun/delavniki.html";

    // povezi s spletnim naslovom in pocakaj na odziv
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(konec_odziva(QNetworkReply*)));

    ui->txt_status->appendPlainText("Pricenjam povezovanje");
    manager->get(QNetworkRequest(QUrl(pot)));

}

void sif_delavniki::konec_odziva(QNetworkReply *odgovor) {

    if ( odgovor->error() == 0 ) {
        ui->txt_status->appendPlainText("Podatki pridobljeni!");
        QString celotna_stran = odgovor->readAll();

        int polozaj = 0;

        // v bazo zabelezimo vsa leta, za katera imamo podatke in jih se ni v bazi
        for ( int i = 0; i < celotna_stran.count("<h2>"); i++ ) {
            QString besedilo = "";

            polozaj = celotna_stran.indexOf("<h2>", polozaj + 4);

            besedilo = celotna_stran.right(celotna_stran.length() - celotna_stran.indexOf("<h2>", polozaj) - 4);
            besedilo = besedilo.left(besedilo.indexOf("<h2>"));

            if ( besedilo.contains("<table") ) {
                QString leto = "";
                leto = besedilo.left(4);

                // izrezemo vse pred <table in za </table>
                besedilo = besedilo.left(besedilo.indexOf("</table>"));
                besedilo = besedilo.right(besedilo.length() - besedilo.indexOf("<table"));

                besedilo.replace(QRegExp("<table(.)[^>]*>"), "");
                besedilo.replace(QRegExp("<(.)[^>]*>"), ";");
                besedilo.replace("\n;;", "");
                besedilo.replace("\t", "");

                // odstranimo prvo vrstico
                besedilo = besedilo.right(besedilo.length() - besedilo.indexOf("\n"));

                //odstranimo prvi stolpec
                QStringList seznam;
                seznam = besedilo.split(";;");

                besedilo = "";

                for ( int i = 0; i < 12; i++ ) {
                    besedilo = besedilo + ";;";
                    for ( int j = 0; j < 6; j++ ) {
                        if ( j != 0 && j != 1 ) {
                            besedilo = besedilo + seznam.at(0) + ";;";
                        }
                        if ( !seznam.empty() ) {
                            seznam.removeFirst();
                        }
                        if ( j == 5 ) {
                            besedilo = besedilo + "\n";
                        }
                    }
                }
                besedilo.remove(" ");

                ui->txt_status->appendPlainText("Podatki razclenjeni: " + leto + "!");

                {
                    // nastavi polja iz baze
                    QString app_path = QApplication::applicationDirPath();
                    QString dbase_path = app_path + "/base.bz";

                    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "vnesi-delavnike");
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

                        ui->txt_status->appendPlainText("Vnasam podatke: " + leto + "!");
                        QSqlQuery sql_preveri;
                        sql_preveri.prepare("SELECT * FROM stroski_prehrane WHERE leto LIKE '" + pretvori(leto) + "'");
                        sql_preveri.exec();
                        if ( !sql_preveri.next() ) {
                            for ( int i = 0; i < 12; i++ ) {
                                QString izlusceno_besedilo = "";
                                QSqlQuery sql_vnesi;
                                sql_vnesi.prepare("INSERT INTO stroski_prehrane (leto, mesec, delavniki, prazniki, skupaj, ure_na_mesec) "
                                                  "VALUES (?, ?, ?, ?, ?, ?)");
                                sql_vnesi.bindValue(0, pretvori(leto));
                                for ( int j = 0; j < 5; j++ ) {
                                    izlusceno_besedilo = besedilo.left(besedilo.indexOf(";;"));
                                    besedilo = besedilo.right(besedilo.length() - besedilo.indexOf(";;") - 2);
                                    if ( j == 0 ) {
                                        izlusceno_besedilo = QString::number(i + 1, 10);
                                        if ( izlusceno_besedilo.length() == 1 ) {
                                            izlusceno_besedilo = "0" + izlusceno_besedilo;
                                        }
                                    }
                                    sql_vnesi.bindValue(j + 1, izlusceno_besedilo);
                                    QCoreApplication::processEvents();
                                }
                                sql_vnesi.exec();
                                sql_vnesi.clear();
                            }
                            ui->txt_status->appendPlainText("Podatki za leto " + leto + " so vneseni!");
                        }
                        else {
                            ui->txt_status->appendPlainText("Podatki za leto " + leto + " obstajajo!");
                        }
                        sql_preveri.clear();

                    }
                    base.close();
                }
                QSqlDatabase::removeDatabase("vnesi-delavnike");

            }

        }
        ui->txt_status->appendPlainText("Podatki osvezeni!");
    }
    else {
        ui->txt_status->appendPlainText("Napaka pri prenosu!");
    }

    napolni_leta();

}

// pretvori v in iz kodirane oblike
QString sif_delavniki::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString sif_delavniki::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

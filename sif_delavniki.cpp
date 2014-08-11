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

    ui->txt_status->setText("");
}

sif_delavniki::~sif_delavniki()
{
    delete ui;
}

void sif_delavniki::on_btn_izhod_clicked() {

    close();

}

void sif_delavniki::on_btn_osvezi_clicked() {

    ui->txt_status->setText("Pricetek!");

    // nastavi pot do spletnega naslova
    // QString pot = "http://www.racunovodja.com/mdokumenti/delure2002.asp"; // pre 2014
    QString pot = "http://www.racunovodja.com/clanki.asp?clanek=6469/%8Atevilo_delovnih_dni_za_leto_2014_%2840-urni_delovni_teden%29";

    // nastavi polja iz baze
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
        QSqlQuery sql_napolni;
        sql_napolni.prepare("SELECT * FROM nastavitve");
        sql_napolni.exec();
        while ( sql_napolni.next() ) {
            if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString()) == "delavniki" ) {
                pot = prevedi(sql_napolni.value(sql_napolni.record().indexOf("vrednost")).toString());
                ui->txt_status->setText("Poznan URL!");
            }
        }
    }
    base.close();

    // povezi s spletnim naslovom in pocakaj na odziv
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(konec_odziva(QNetworkReply*)));

    ui->txt_status->setText("Pricenjam povezovanje");
    manager->get(QNetworkRequest(QUrl(pot)));

}

void sif_delavniki::konec_odziva(QNetworkReply *odgovor) {

    if ( odgovor->readAll().contains("tevilo delovnih dni za leto") ) {
        ui->txt_status->setText("Podatki pridobljeni!");
        QString celotna_stran = odgovor->readAll();

        // pridobimo odsek s podatki za nase leto
        celotna_stran = celotna_stran.right(celotna_stran.length() - celotna_stran.indexOf("tevilo delovnih dni za leto " + QDate::currentDate().toString("yyyy")));
        celotna_stran = celotna_stran.left(celotna_stran.indexOf("tevilo delovnih dni za leto " + QDate::currentDate().addYears(-1).toString("yyyy")));

        // izrezemo vse pred <table in za </table>
        celotna_stran = celotna_stran.left(celotna_stran.indexOf("</table>"));
        celotna_stran = celotna_stran.right(celotna_stran.length() - celotna_stran.indexOf("<table"));

        celotna_stran.replace(QRegExp("<table(.)[^>]*>"), "");
        celotna_stran.replace(QRegExp("<(.)[^>]*>"), ";");
        celotna_stran.replace("\n;;", "");
        celotna_stran.replace("\t", "");

        ui->txt_status->setText("Podatki razclenjeni!");

        // izbrisemo naslovno vrstico tabele
        for ( int j = 0; j < 5; j++ ) {
            celotna_stran = celotna_stran.right(celotna_stran.length() - celotna_stran.indexOf(";;") - 2);
        }

        // nastavi polja iz baze
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

            ui->txt_status->setText("Vnasam podatke!");
            QSqlQuery sql_preveri;
            sql_preveri.prepare("SELECT * FROM stroski_prehrane WHERE leto LIKE '" + pretvori(QDate::currentDate().toString("yyyy")) + "'");
            sql_preveri.exec();
            if ( !sql_preveri.next() ) {
                for ( int i = 0; i < 12; i++ ) {
                    QString izlusceno_besedilo = "";
                    QSqlQuery sql_vnesi;
                    sql_vnesi.prepare("INSERT INTO stroski_prehrane (leto, mesec, delavniki, prazniki, skupaj, ure_na_mesec) "
                                      "VALUES (?, ?, ?, ?, ?, ?)");
                    sql_vnesi.bindValue(0, pretvori(QDate::currentDate().toString("yyyy")));
                    for ( int j = 0; j < 5; j++ ) {
                        izlusceno_besedilo = celotna_stran.left(celotna_stran.indexOf(";;"));
                        celotna_stran = celotna_stran.right(celotna_stran.length() - celotna_stran.indexOf(";;") - 2);
                        if ( j == 0 ) {
                            izlusceno_besedilo = QString::number(i + 1, 10);
                            if ( izlusceno_besedilo.length() == 1 ) {
                                izlusceno_besedilo = "0" + izlusceno_besedilo;
                            }
                        }
                        sql_vnesi.bindValue(j + 1, pretvori(izlusceno_besedilo));
                    }
                    sql_vnesi.exec();
                    sql_vnesi.clear();
                    ui->txt_status->setText("Podatki vneseni!");
                }
            }
            sql_preveri.clear();
        }
        base.close();

        ui->txt_status->setText("Podatki osvezeni!");
    }
    else {
        ui->txt_status->setText("Napaka pri prenosu!");
    }

}

// pretvori v in iz kodirane oblike
QString sif_delavniki::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString sif_delavniki::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

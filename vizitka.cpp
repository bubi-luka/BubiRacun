#include <QObject>
#include <QImage>
#include <QtSql>
#include <QMessageBox>
#include <QFile>

#include "vizitka.h"
#include "ui_vizitka.h"

Vizitka::Vizitka(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Vizitka)
{
    ui->setupUi(this);

    /*
    QImage slika(QApplication::applicationDirPath() + "/bubiji.png");
    ui->logo->setPixmap(QPixmap::fromImage(slika));
    */

    // fill data from files to the form
    insert_program_info();
    insert_news();
    insert_licence();

    // fill data from the tables to the form
    insert_base();
}

Vizitka::~Vizitka()
{
    delete ui;
}

void Vizitka::insert_base() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "podatki_o_programu");
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

        QSqlQuery sql_fill;

        sql_fill.prepare("SELECT * FROM glavna WHERE parameter LIKE 'Verzija programa'");
        sql_fill.exec();
        if ( sql_fill.next() ) {
            ui->txt_razlicica_programa->setText(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString());
        }
        sql_fill.clear();

        sql_fill.prepare("SELECT * FROM glavna WHERE parameter LIKE 'Verzija baze'");
        sql_fill.exec();
        if ( sql_fill.next() ) {
            ui->txt_razlicica_baze->setText(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString());
        }
        sql_fill.clear();

        sql_fill.prepare("SELECT * FROM glavna WHERE parameter LIKE 'Ime programa'");
        sql_fill.exec();
        if ( sql_fill.next() ) {
            ui->txt_ime_programa->setText(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString());
        }
        sql_fill.clear();

        sql_fill.prepare("SELECT * FROM glavna WHERE parameter LIKE 'Avtor programa'");
        sql_fill.exec();
        if ( sql_fill.next() ) {
            ui->txt_avtor_programa->setText(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString());
        }
        sql_fill.clear();

        sql_fill.prepare("SELECT * FROM glavna WHERE parameter LIKE 'Datum spremembe'");
        sql_fill.exec();
        if ( sql_fill.next() ) {
            ui->txt_datum_posodobitve_programa->setText(sql_fill.value(sql_fill.record().indexOf("vrednost")).toString());
        }
        sql_fill.clear();

    }
    base.close();

}

void Vizitka::insert_licence() {

    QString app_path = QApplication::applicationDirPath();
    QFile datoteka(app_path + "/licence.txt");
    if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream besedilo(&datoteka);
    while (!besedilo.atEnd()) {
        ui->txt_pogoji_uporabe->appendPlainText(besedilo.readLine());
    }

}

void Vizitka::insert_news() {

    QString app_path = QApplication::applicationDirPath();
    QFile datoteka(app_path + "/news.txt");
    if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream besedilo(&datoteka);
    while (!besedilo.atEnd()) {
        ui->txt_novosti->appendPlainText(besedilo.readLine());
    }

}

void Vizitka::insert_program_info() {

    QString app_path = QApplication::applicationDirPath();
    QFile datoteka(app_path + "/readme.txt");
    if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream besedilo(&datoteka);
    while (!besedilo.atEnd()) {
        ui->txt_predstavitev_programa->appendPlainText(besedilo.readLine());
    }

}

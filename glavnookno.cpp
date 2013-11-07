#include <QDateTime>
#include <QTimer>
#include <QWidget>
#include <QtWidgets>
#include <QKeyEvent>
#include <QtSql>
#include <QMessageBox>
#include <QObject>

#include "glavnookno.h"
#include "ui_glavnookno.h"
#include "sif_dnevnice.h"
#include "sif_delavniki.h"
#include "sif_kilometrina.h"
#include "wid_uporabnik.h"
#include "wid_podjetje.h"
#include "wid_potninalogi.h"
#include "wid_prejetiracuni.h"
#include "wid_stranke.h"
#include "wid_projekti.h"
#include "prijava.h"
#include "wid_kuponi.h"
#include "wid_racuni.h"
#include "wid_opombepriracunih.h"
#include "vizitka.h"
#include "varnost.h"
#include "kodiranje.h"
#include "prijava.h"
#include "nastavitve.h"
#include "wid_casovnice.h"
#include "wid_prihodki.h"
#include "wid_izdatki.h"
#include "wid_poslovanje.h"
#include "wid_osnovni_pogled.h"
#include "wid_avtomobili.h"
#include "wid_stroskiprehrane.h"
#include "sif_cenamalice.h"
#include "wid_ddv.h"
#include "wid_storitve.h"
#include "kategorije.h"
#include "podkategorije.h"

#include "prijava.h"
#include "podjetje.h"
#include "uporabnik.h"
#include "setup.h"

GlavnoOkno::GlavnoOkno(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GlavnoOkno)
{
    ui->setupUi(this);

    vApp->processEvents();

    // povecaj cez cel ekran
    showMaximized();

    // skrijemo polja, ki jih ne potrebujemo
    ui->txt_pozicija->setVisible(false);
    ui->txt_uporabnik->setVisible(false);

    // first run
    ui->lbl_pozdrav->setText("Poteka posodabljanje baze. To opravilo obicajno zahteva nekaj minut! Hvala za razumevanje!");
    vApp->processEvents();

    setup().start_first_run();
    zagon();

    vApp->processEvents();
}

void GlavnoOkno::zagon() {

    vApp->set_id("");

    // disable menu and buttons
    ui->menubar->setEnabled(false);
    ui->btn_home->setEnabled(false);

    // set labels to empty
    ui->txt_pozicija->setText("");
    ui->txt_uporabnik->setText("");
    ui->lbl_pozdrav->clear();
    ui->lbl_pozicija->clear();
    ui->lbl_pozdrav->setText("");
    ui->lbl_pozicija->setText("");

    // has the program been used before (do we have at least one firm and one user)?
    QString first_use = "";

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "zagonska");
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
        QSqlQuery sql_check;

        //check, if we have a valid firm
        sql_check.prepare("SELECT * FROM podjetje");
        sql_check.exec();
        if ( !sql_check.next() ) {
            first_use = "firm";
        }
        sql_check.clear();

        //check, if we have a valid user
        if ( first_use == "" ) {
            sql_check.prepare("SELECT * FROM uporabniki");
            sql_check.exec();
            if ( !sql_check.next() ) {
                first_use = "user";
            }
        }
        sql_check.clear();

        // check if we have all the necessary settings in place
//        if ( first_use == "" ) {
//            sql_check.prepare("SELECT * FROM uporabniki");
//            sql_check.exec();
//            if ( !sql_check.next() ) {
//               first_use = "user";
//            }
//        }
//        sql_check.clear();

        // open correct widget

        if ( first_use == "firm" ) {
            podjetje *widpodjetje = new podjetje;
            ui->scrollArea->setWidget(widpodjetje);
            ui->lbl_pozicija->setText("Vnesite podatke o podjetju!");
            setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Podatki o podjetju");

            QObject::connect(this, SIGNAL(prenos(QString)),
                       widpodjetje , SLOT(prejem(QString)));
            prenos("Novo podjetje");
            this->disconnect();

            // receive signal to refresh table
            QObject::connect(widpodjetje, SIGNAL(poslji(QString)),
                       this , SLOT(osvezi(QString)));
            this->disconnect();
        }
        else if ( first_use == "user" ) {
            uporabnik *widuporabnik = new uporabnik;
            ui->scrollArea->setWidget(widuporabnik);
            ui->lbl_pozicija->setText("Vnesite podatke o uporabniku!");
            setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Podatki o uporabniku");

            QObject::connect(this, SIGNAL(prenos(QString)),
                       widuporabnik , SLOT(prejem(QString)));
            prenos("Nov zaposleni");
            this->disconnect();

            // receive signal to refresh table
            QObject::connect(widuporabnik, SIGNAL(poslji(QString)),
                       this , SLOT(osvezi(QString)));
            this->disconnect();
        }
/*        else if ( first_use == "settings" ) {

        }
     */   else {
            prijava *widprijava = new prijava;
            ui->scrollArea->setWidget(widprijava);
            widprijava->show();
            widprijava->setFocus();

            ui->lbl_pozicija->setText("Vnesite prijavne podatke!");
            setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Prijava");

            // receive signal to refresh table
            QObject::connect(widprijava, SIGNAL(poslji(QString)),
                     this , SLOT(osvezi(QString)));
        }

    }
    base.close();

    zacetek();

}

void GlavnoOkno::osvezi(QString besedilo) {

    if ( besedilo == "loginok" ) {
        osnovni_pogled();
    }
    else {
        zagon();
    }

}

GlavnoOkno::~GlavnoOkno()
{
    delete ui;
}


void GlavnoOkno::on_btn_home_clicked() {

    osnovni_pogled();

}

void GlavnoOkno::sekundnik() {

    QString datum = QDate::currentDate().toString("dd.MM.yyyy");
    QString ura = QTime::currentTime().toString("HH:mm:ss");

    ui->lbl_datum->setText("Danes je: " + datum + " " + ura);
    podatki();

}

void GlavnoOkno::osnovni_pogled() {

    wid_osnovni_pogled *widop = new wid_osnovni_pogled;
    ui->scrollArea->setWidget(widop);
    ui->lbl_pozicija->setText("Nahajate se na Osnovnem Pogledu!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Osnovni Pogled");

}

void GlavnoOkno::on_actionUporabniki_triggered() {
    wid_uporabnik *widup = new wid_uporabnik;
    ui->scrollArea->setWidget(widup);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Zaposlenih!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Zaposleni");
}

void GlavnoOkno::on_actionPodjetje_triggered() {
    wid_podjetje *widpod = new wid_podjetje;
    ui->scrollArea->setWidget(widpod);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Podjetja!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Podjetja");
}

void GlavnoOkno::on_actionPotni_nalogi_triggered() {
    wid_potninalogi *widpot = new wid_potninalogi;
    ui->scrollArea->setWidget(widpot);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Potni nalogi!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Potni nalogi");

    QObject::connect(this, SIGNAL(prenos(QString)),
                 widpot , SLOT(prejem(QString)));
    prenos("*");
    this->disconnect();

}

void GlavnoOkno::on_actionPrejeti_ra_uni_triggered() {
    wid_prejetiracuni *widrac = new wid_prejetiracuni;
    ui->scrollArea->setWidget(widrac);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Prejeti racuni!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Prejeti racuni");

    QObject::connect(this, SIGNAL(prenos(QString)),
                 widrac , SLOT(prejem(QString)));
    prenos("*");
    this->disconnect();

}

void GlavnoOkno::on_actionStranke_triggered() {
    wid_stranke *widst = new wid_stranke;
    ui->scrollArea->setWidget(widst);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Stranke!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Stranke");
}

void GlavnoOkno::on_actionProjekti_triggered() {
    wid_projekti *widpr = new wid_projekti;
    ui->scrollArea->setWidget(widpr);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Projekti!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Projekti");
}

void GlavnoOkno::on_actionKuponi_triggered() {
    wid_kuponi *widkup = new wid_kuponi;
    ui->scrollArea->setWidget(widkup);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Kuponi!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Kuponi");
}

void GlavnoOkno::on_actionVizitka_triggered() {

    Vizitka *widviz = new Vizitka;
    ui->scrollArea->setWidget(widviz);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Vizitka!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Vizitka");

}

void GlavnoOkno::on_action_asovnice_triggered() {

    wid_casovnice *widcas = new wid_casovnice;
    ui->scrollArea->setWidget(widcas);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Casovnice!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Casovnice");

}

void GlavnoOkno::on_actionOpombe_pri_ra_unih_triggered() {

    wid_opombepriracunih *widopo = new wid_opombepriracunih;
    ui->scrollArea->setWidget(widopo);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Opombe pri racunih!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Opombe pri racunih");

}

void GlavnoOkno::on_actionPrihodek_triggered() {

    wid_prihodki *widpri = new wid_prihodki;
    ui->scrollArea->setWidget(widpri);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Pregled prihodkov poslovanja!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Pregled prihodkov poslovanja");

}

void GlavnoOkno::on_actionIzdatek_triggered() {

    wid_izdatki *widizd = new wid_izdatki;
    ui->scrollArea->setWidget(widizd);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Pregled poslovnih izdatkov!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Pregled poslovnih izdatkov");

}

void GlavnoOkno::on_actionPoslovanje_triggered() {

    wid_poslovanje *widpos = new wid_poslovanje;
    ui->scrollArea->setWidget(widpos);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Pregled poslovanja!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Pregled poslovanja");

}

void GlavnoOkno::on_actionKilometrina_triggered() {

    sif_kilometrina *odpri = new sif_kilometrina;
    odpri->show();

}

void GlavnoOkno::on_actionDnevnice_triggered() {

    sif_dnevnice *odpri = new sif_dnevnice;
    odpri->show();

}

void GlavnoOkno::on_actionDelavniki_triggered() {

    sif_delavniki *odpri = new sif_delavniki;
    odpri->show();

}

void GlavnoOkno::on_actionCena_malice_triggered() {

    sif_cenamalice *odpri = new sif_cenamalice;
    odpri->show();

}

void GlavnoOkno::on_actionNastavitve_triggered() {

    nastavitve *odpri = new nastavitve;
    odpri->show();

}

void GlavnoOkno::on_actionAvtomobili_triggered() {

    wid_avtomobili *widavto = new wid_avtomobili;
    ui->scrollArea->setWidget(widavto);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Pregled avtomobilov!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Pregled avtomobilov");

}

void GlavnoOkno::on_actionDopusti_bolni_ke_triggered() {

    wid_stroskiprehrane *widavto = new wid_stroskiprehrane;
    ui->scrollArea->setWidget(widavto);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Vnos dopustov, bolniskih dni, malic!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Dopusti, bolniske, malice");

}

void GlavnoOkno::on_actionPredra_uni_triggered() {

    wid_racuni *widrac = new wid_racuni;
    ui->scrollArea->setWidget(widrac);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Izdani racuni!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Izdani racuni");

    QObject::connect(this, SIGNAL(prenos(QString)),
               widrac , SLOT(tip(QString)));
    prenos("1");
    this->disconnect();

}

void GlavnoOkno::on_actionPredpla_ilni_ra_uni_triggered() {

    wid_racuni *widrac = new wid_racuni;
    ui->scrollArea->setWidget(widrac);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Izdani racuni!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Izdani racuni");

    QObject::connect(this, SIGNAL(prenos(QString)),
               widrac , SLOT(tip(QString)));
    prenos("2");
    this->disconnect();

}

void GlavnoOkno::on_actionRa_uni_triggered() {

    wid_racuni *widrac = new wid_racuni;
    ui->scrollArea->setWidget(widrac);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Izdani racuni!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Izdani racuni");

    QObject::connect(this, SIGNAL(prenos(QString)),
               widrac , SLOT(tip(QString)));
    prenos("3");
    this->disconnect();

}

void GlavnoOkno::on_actionStornirani_ra_uni_triggered() {

    wid_racuni *widrac = new wid_racuni;
    ui->scrollArea->setWidget(widrac);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Stornirani racuni!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Stornirani racuni");

    QObject::connect(this, SIGNAL(prenos(QString)),
               widrac , SLOT(tip(QString)));
    prenos("4");
    this->disconnect();

}

void GlavnoOkno::on_actionVrednost_DDV_triggered() {

    wid_ddv *widddv = new wid_ddv;
    ui->scrollArea->setWidget(widddv);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Sifrant DDV!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Sifrant DDV");

}

void GlavnoOkno::on_actionStoritve_triggered() {

    wid_storitve *widstoritev = new wid_storitve;
    ui->scrollArea->setWidget(widstoritev);
    ui->lbl_pozicija->setText("Nahajate se na tabeli Sifrant storitev!");
    setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Sifrant storitev");

}

void GlavnoOkno::on_actionKategorije_triggered() {

    kategorije *okno = new kategorije;
    okno->show();

}

void GlavnoOkno::on_actionPodkategorije_triggered() {

    podkategorije *okno = new podkategorije;
    okno->show();

}

void GlavnoOkno::varnost_id_changed() {

    if ( vApp->id() == "" ) {
        ui->menubar->setEnabled(false);
        ui->btn_home->setEnabled(false);
    }
    else {
        ui->menubar->setEnabled(true);
        ui->btn_home->setEnabled(true);
    }

    podatki();

}

QString GlavnoOkno::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString GlavnoOkno::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void GlavnoOkno::keyPressEvent(QKeyEvent *event) {

    if ( event->key() == Qt::Key_Escape ) {
        if ( vApp->id() != "" ) {
            osnovni_pogled();
        }
    }
    else if ( (event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::AltModifier) ) {
        zagon();
    }
    else if ( (event->key() == Qt::Key_S) && (event->modifiers() == Qt::AltModifier) ) {

        if (vApp->state() == pretvori("public") ) {
            vApp->set_state(pretvori("private"));
            ui->txt_pozicija->setVisible(false);
        }
        else {
            vApp->set_state(pretvori("public"));
            ui->txt_pozicija->setVisible(true);
        }

    }

    ui->txt_pozicija->setText(prevedi(vApp->state()));

}

void GlavnoOkno::podatki() {

    QString pozdrav = "";

    {
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki-pozdrav");
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
            // baza je odprta

            QSqlQuery sql_firma;
            sql_firma.prepare("SELECT * FROM podjetje WHERE id LIKE '" + vApp->firm() + "'");
            sql_firma.exec();
            if ( sql_firma.next() ) {
                pozdrav = prevedi(sql_firma.value(sql_firma.record().indexOf("ime")).toString());
            }
        }
        base.close();
    }
    QSqlDatabase::removeDatabase("uporabniki-pozdrav");

    if ( vApp->id() != "" ) {
        pozdrav = "Pozdravljeni " + prevedi(vApp->name()) + " "  + prevedi(vApp->surname()) + " (" +  prevedi(vApp->permission()) + "), v podjetju " + pozdrav + "!";
        ui->lbl_pozdrav->setText(pozdrav);
        ui->lbl_pozdrav->update();
    }


    ui->txt_uporabnik->setText(vApp->id());
    ui->txt_pozicija->setText(prevedi(vApp->state()));

}

void GlavnoOkno::zacetek() {

/*   QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(sekundnik()));
   timer->start(1000);

   QString datum = QDate::currentDate().toString("dd.MM.yyyy");
   QString ura = QTime::currentTime().toString("HH:mm:ss");

       ui->lbl_datum->setText("Danes je: " + datum + " " + ura);
*/
}

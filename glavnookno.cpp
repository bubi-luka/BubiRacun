#include <QDateTime>
#include <QTimer>
#include <QWidget>
#include <QKeyEvent>
#include <QtSql>
#include <QMessageBox>

#include "glavnookno.h"
#include "ui_glavnookno.h"
#include "wid_uporabnik.h"
#include "wid_podjetje.h"
#include "wid_potninalogi.h"
#include "wid_prejetiracuni.h"
#include "wid_stranke.h"
#include "wid_projekti.h"
#include "wid_kuponi.h"
#include "wid_racuni.h"
#include "vizitka.h"
#include "varnost.h"
#include "kodiranje.h"
#include "prijava.h"
#include "popusti.h"

GlavnoOkno::GlavnoOkno(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GlavnoOkno)
{
    ui->setupUi(this);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(sekundnik()));
	timer->start(1000);

	QString datum = QDate::currentDate().toString("dd.MM.yyyy");
	QString ura = QTime::currentTime().toString("HH:mm:ss");

	ui->lbl_datum->setText("Danes je: " + datum + " " + ura);

	QString pozdrav;
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
		// baza je odprta
		QSqlQuery sql_firma;
		sql_firma.prepare("SELECT * FROM podjetje WHERE id LIKE '" + vApp->firm() + "'");
		sql_firma.exec();
		if ( sql_firma.next() ) {
			pozdrav = prevedi(sql_firma.value(sql_firma.record().indexOf("ime")).toString());
		}
	}
	base.close();

	pozdrav = "Pozdravljeni " + prevedi(vApp->name()) + " "  + prevedi(vApp->surname()) + " (" +  prevedi(vApp->permission()) + "), v podjetju " + pozdrav + "!";
	ui->lbl_pozdrav->setText(pozdrav);

	ui->txt_uporabnik->setText(vApp->id());
	ui->txt_pozicija->setText(prevedi(vApp->state()));

	ui->actionPoslovanje->setEnabled(false);
	ui->actionPrihodek->setEnabled(false);
	ui->actionIzdatek->setEnabled(false);

}

GlavnoOkno::~GlavnoOkno()
{
    delete ui;
}

void GlavnoOkno::sekundnik() {
	QString datum = QDate::currentDate().toString("dd.MM.yyyy");
	QString ura = QTime::currentTime().toString("HH:mm:ss");

	ui->lbl_datum->setText("Danes je: " + datum + " " + ura);
}

void GlavnoOkno::on_actionUporabniki_triggered() {
	wid_uporabnik *widup = new wid_uporabnik;
	ui->scrollArea->setWidget(widup);
	ui->lbl_pozicija->setText("Nahajate se na tabeli Uporabniki!");
	setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Uporabniki");
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
}

void GlavnoOkno::on_actionPrejeti_ra_uni_triggered() {
	wid_prejetiracuni *widrac = new wid_prejetiracuni;
	ui->scrollArea->setWidget(widrac);
	ui->lbl_pozicija->setText("Nahajate se na tabeli Prejeti racuni!");
	setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Prejeti racuni");
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

void GlavnoOkno::on_actionIzdani_ra_uni_triggered() {

	wid_racuni *widrac = new wid_racuni;
	ui->scrollArea->setWidget(widrac);
	ui->lbl_pozicija->setText("Nahajate se na tabeli Izdani racuni!");
	setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Izdani racuni");

	QObject::connect(this, SIGNAL(prenos(QString)),
			   widrac , SLOT(prejem(QString)));
	prenos("*");
	this->disconnect();

}

void GlavnoOkno::on_actionVizitka_triggered() {
	Vizitka *widviz = new Vizitka;
	ui->scrollArea->setWidget(widviz);
	ui->lbl_pozicija->setText("Nahajate se na tabeli Vizitka!");
	setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Vizitka");
}


void GlavnoOkno::on_actionPopusti_triggered() {

	/*
	* Za enkrat se popusti odprejo in urejajo zgolj v oknu, widget/tabela še ni predvidena
	*/
//	Vizitka *widviz = new Vizitka;
//	ui->scrollArea->setWidget(widviz);
//	ui->lbl_pozicija->setText("Nahajate se na tabeli Vizitka!");
//	setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)) + " - Vizitka");

	popusti *okno_popusti = new popusti;
	okno_popusti->show();

}

void GlavnoOkno::varnost_id_changed() {



}

QString GlavnoOkno::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString GlavnoOkno::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void GlavnoOkno::keyPressEvent(QKeyEvent *event) {

	if ( event->key() == Qt::Key_Escape ) {
		ui->scrollArea->widget()->close();
		setWindowTitle(windowTitle().left(windowTitle().indexOf(" - ", 0)));
		ui->lbl_pozicija->setText("");
	}
	else if ( (event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::AltModifier) ) {
		prijava *okno_prijava = new prijava;
		okno_prijava->show();
		GlavnoOkno::close();
	}
	else if ( (event->key() == Qt::Key_S) && (event->modifiers() == Qt::AltModifier) ) {

		if (vApp->state() == pretvori("public") ) {
			vApp->set_state(pretvori("private"));
		}
		else {
			vApp->set_state(pretvori("public"));
		}

	}

}

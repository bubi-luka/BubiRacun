#include <QDateTime>
#include <QTimer>
#include <QWidget>

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
	ui->lbl_pozdrav->setText("Pozdravljeni!");
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
}

void GlavnoOkno::on_actionPodjetje_triggered() {
	wid_podjetje *widpod = new wid_podjetje;
	ui->scrollArea->setWidget(widpod);
}

void GlavnoOkno::on_actionPotni_nalogi_triggered() {
	wid_potninalogi *widpot = new wid_potninalogi;
	ui->scrollArea->setWidget(widpot);
}

void GlavnoOkno::on_actionPrejeti_ra_uni_triggered() {
	wid_prejetiracuni *widrac = new wid_prejetiracuni;
	ui->scrollArea->setWidget(widrac);
}

void GlavnoOkno::on_actionStranke_triggered() {
	wid_stranke *widst = new wid_stranke;
	ui->scrollArea->setWidget(widst);
}

void GlavnoOkno::on_actionProjekti_triggered() {
	wid_projekti *widpr = new wid_projekti;
	ui->scrollArea->setWidget(widpr);
}

void GlavnoOkno::on_actionKuponi_triggered() {
	wid_kuponi *widkup = new wid_kuponi;
	ui->scrollArea->setWidget(widkup);
}

void GlavnoOkno::on_actionIzdani_ra_uni_triggered() {

	wid_racuni *widrac = new wid_racuni;
	ui->scrollArea->setWidget(widrac);

	QObject::connect(this, SIGNAL(prenos(QString)),
			   widrac , SLOT(prejem(QString)));
	prenos("*");
	this->disconnect();

}

void GlavnoOkno::on_actionVizitka_triggered() {
	Vizitka *widviz = new Vizitka;
	ui->scrollArea->setWidget(widviz);
}

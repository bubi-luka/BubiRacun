#include <QMessageBox>
#include <QtSql>
#include <QDate>
#include <QKeyEvent>

#include "varnost.h"
#include "kodiranje.h"
#include "sif_dnevnice.h"
#include "ui_sif_dnevnice.h"

sif_dnevnice::sif_dnevnice(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::sif_dnevnice)
{
	ui->setupUi(this);

		// pocistimo polja
		ui->txt_dnevnica_1->setText("");
		ui->txt_dnevnica_2->setText("");
		ui->txt_dnevnica_3->setText("");
		ui->txt_datum->setDate(QDate::currentDate());

		// onemogoci gumbe
		ui->btn_sprejmi->setEnabled(false);

		// napolni polja
		prejem();

}

sif_dnevnice::~sif_dnevnice()
{
	delete ui;
}

void sif_dnevnice::on_btn_izhod_clicked() {

	close();

}

// ne preverja napak
void sif_dnevnice::on_btn_sprejmi_clicked() {

	ui->txt_datum->setDate(QDate::currentDate());
	QString napaka = "";

	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {
			QSqlQuery sql_vnesi;
			sql_vnesi.prepare("INSERT INTO sif_dnevnice (dnevnica_1, dnevnica_2, dnevnica_3, datum, avtor_oseba) VALUES (?, ?, ?, ?, ?)");
			sql_vnesi.bindValue(0, pretvori(pretvori_v_double(ui->txt_dnevnica_1->text())));
			sql_vnesi.bindValue(1, pretvori(pretvori_v_double(ui->txt_dnevnica_2->text())));
			sql_vnesi.bindValue(2, pretvori(pretvori_v_double(ui->txt_dnevnica_3->text())));
			sql_vnesi.bindValue(3, pretvori(ui->txt_datum->text()));
			sql_vnesi.bindValue(4, pretvori(vApp->id()));
			sql_vnesi.exec();

		// close this window
		close();
	}
	else {
		QMessageBox msgbox;
		msgbox.setText("Dolocena polja niso pravilno izpolnjena");
		msgbox.exec();
	}

}

void sif_dnevnice::on_txt_dnevnica_1_textChanged() {

	ui->btn_sprejmi->setEnabled(true);

}

void sif_dnevnice::on_txt_dnevnica_2_textChanged() {

	ui->btn_sprejmi->setEnabled(true);

}

void sif_dnevnice::on_txt_dnevnica_3_textChanged() {

	ui->btn_sprejmi->setEnabled(true);

}

QString sif_dnevnice::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString sif_dnevnice::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString sif_dnevnice::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	return besedilo;

}

QString sif_dnevnice::pretvori_iz_double(QString besedilo) {

	/*
	* pretvarja stevilke v valuto, primerno za obdelavo naprej
	* ni nujno, da je vhodna stevilka resnicno double, lahko gre za drugacno obliko
	*/

	besedilo.replace(".", ","); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9,]")); // odstrani vse znake razen stevilk in decimalne vejice

	while ( besedilo.left(1) == "0" ) { // odstranimo vse vodilne nicle
		besedilo.remove(0,1);
	}
	if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0,00
		besedilo.append("0");
	}
	if ( besedilo.left(1) == "," ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
		besedilo.prepend("0");
	}
	if ( besedilo.right(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 00
		besedilo.append("00");
	}
	if ( besedilo.right(2).left(1) == "," ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 00
		besedilo.append("0");
	}
	if ( !besedilo.contains(",") ) { // ce je celo stevilo dodamo decimalno locilo in vrednost 00
		besedilo.append(",00");
	}

	besedilo.append(" EUR"); // doda oznako za evre

	return besedilo;

}

void sif_dnevnice::prejem() {

		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM sif_dnevnice");
		sql_napolni.exec();
		while (sql_napolni.next()) {
			ui->txt_dnevnica_1->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("dnevnica_1")).toString())));
			ui->txt_dnevnica_2->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("dnevnica_2")).toString())));
			ui->txt_dnevnica_3->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("dnevnica_3")).toString())));
			QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum")).toString()), "dd.MM.yyyy");
			ui->txt_datum->setDate(datum);
		}

	// blokiraj gumb za spreminjanje
	ui->btn_sprejmi->setEnabled(false);

}

void sif_dnevnice::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}

}

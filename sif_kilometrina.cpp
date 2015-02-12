#include <QMessageBox>
#include <QtSql>
#include <QDate>
#include <QKeyEvent>

#include "varnost.h"
#include "kodiranje.h"
#include "sif_kilometrina.h"
#include "ui_sif_kilometrina.h"

sif_kilometrina::sif_kilometrina(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::sif_kilometrina)
{
	ui->setupUi(this);

		// ponastavi vsa polja
		ui->txt_datum->setDate(QDate::currentDate());
		ui->txt_kilometrina->setText("");

		// blokiraj polja in gumbe
		ui->btn_sprejmi->setEnabled(false);

		// napolni polja
		prejem();

}

sif_kilometrina::~sif_kilometrina()
{
	delete ui;
}

void sif_kilometrina::on_btn_izhod_clicked() {

	close();

}

// ne preverja napak
void sif_kilometrina::on_btn_sprejmi_clicked() {

	ui->txt_datum->setDate(QDate::currentDate());
	QString napaka = "";

	// javi napake, ce ni napak vnesi v bazo
	if (napaka == "") {

			QSqlQuery sql_vnesi;
			sql_vnesi.prepare("INSERT INTO sif_kilometrina (kilometrina, datum, avtor_oseba) VALUES (?, ?, ?)");
			sql_vnesi.bindValue(0, pretvori(pretvori_v_double(ui->txt_kilometrina->text())));
			sql_vnesi.bindValue(1, pretvori(ui->txt_datum->text()));
			sql_vnesi.bindValue(2, pretvori(vApp->id()));
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

void sif_kilometrina::on_txt_kilometrina_textChanged() {

	ui->btn_sprejmi->setEnabled(true);

}

QString sif_kilometrina::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString sif_kilometrina::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString sif_kilometrina::pretvori_v_double(QString besedilo) {

	/*
	* pretvarja znake v format double
	* prejme poljubni format, vrne double
	*/

	besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
	besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

	return besedilo;

}

QString sif_kilometrina::pretvori_iz_double(QString besedilo) {

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

void sif_kilometrina::prejem() {

		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM sif_kilometrina");
		sql_napolni.exec();
		while (sql_napolni.next()) {
			ui->txt_kilometrina->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kilometrina")).toString())));
			QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum")).toString()), "dd.MM.yyyy");
			ui->txt_datum->setDate(datum);
		}

	// zapri gumb za spreminjanje
	ui->btn_sprejmi->setEnabled(false);

}

void sif_kilometrina::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}

}

#include <QKeyEvent>
#include <QtSql>
#include <QMessageBox>

#include "varnost.h"
#include "kodiranje.h"
#include "opombepriracunih.h"
#include "ui_opombepriracunih.h"

opombepriracunih::opombepriracunih(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::opombepriracunih)
{
	ui->setupUi(this);

		// izbrisi vnosna polja
		ui->txt_id->setText("");
		ui->txt_naslov->setText("");
		ui->txt_besedilo->clear();

}

opombepriracunih::~opombepriracunih()
{
	delete ui;
}

void opombepriracunih::on_btn_izhod_clicked() {

	close();

}

void opombepriracunih::on_btn_sprejmi_clicked() {

		QSqlQuery sql_vnesi;

		if (ui->btn_sprejmi->text() == "Vnesi opombo") {
			sql_vnesi.prepare("INSERT INTO sif_opombe_pri_racunih (naslov, besedilo)"
												"VALUES (?, ?) ");
		}
		else {
			sql_vnesi.prepare("UPDATE sif_opombe_pri_racunih SET naslov = ?, besedilo = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
		}
		sql_vnesi.bindValue(0, pretvori(ui->txt_naslov->text()));
		sql_vnesi.bindValue(1, pretvori(ui->txt_besedilo->toPlainText()));
		sql_vnesi.exec();

	// vrni signal
	poslji("opombe");

	// zapri okno
	close();

}

void opombepriracunih::keyPressEvent(QKeyEvent *event) {

	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}

}

QString opombepriracunih::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString opombepriracunih::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void opombepriracunih::prejem(QString besedilo) {

	if (besedilo == "Nova opomba") {
		ui->btn_sprejmi->setText("Vnesi opombo");
	}
	else {
		ui->btn_sprejmi->setText("Popravi opombo");
			QSqlQuery sql_vnesi;
			sql_vnesi.prepare("SELECT * FROM sif_opombe_pri_racunih WHERE id LIKE '" + pretvori(besedilo) + "'");
			sql_vnesi.exec();
			if ( sql_vnesi.next() ) {
				ui->txt_id->setText(prevedi(sql_vnesi.value(sql_vnesi.record().indexOf("id")).toString()));
				ui->txt_naslov->setText(prevedi(sql_vnesi.value(sql_vnesi.record().indexOf("naslov")).toString()));
				ui->txt_besedilo->setPlainText(prevedi(sql_vnesi.value(sql_vnesi.record().indexOf("besedilo")).toString()));
			}

	}

}

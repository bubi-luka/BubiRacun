#include <QtSql>
#include <QMessageBox>
#include <QDate>
#include <QSortFilterProxyModel>

#include "kodiranje.h"
#include "wid_izdatki.h"
#include "ui_wid_izdatki.h"

wid_izdatki::wid_izdatki(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_izdatki)
{
    ui->setupUi(this);

		// izprazni vsa polja
		ui->tbl_letni_pregled->clear();
		// trenutno zaradi mase polj ne delam

		// napolni spustne sezname
		// mesec
		ui->txt_mesec->addItem("");
		ui->txt_mesec->addItem("01) Januar");
		ui->txt_mesec->addItem("02) Februar");
		ui->txt_mesec->addItem("03) Marec");
		ui->txt_mesec->addItem("04) April");
		ui->txt_mesec->addItem("05) Maj");
		ui->txt_mesec->addItem("06) Junij");
		ui->txt_mesec->addItem("07) Julij");
		ui->txt_mesec->addItem("08) Avgust");
		ui->txt_mesec->addItem("09) September");
		ui->txt_mesec->addItem("10) Oktober");
		ui->txt_mesec->addItem("11) November");
		ui->txt_mesec->addItem("12) December");

		// leto
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_izdatki_spustni_seznami");
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

			QSqlQuery sql_leta;
			sql_leta.prepare("SELECT * FROM prejeti_racuni");
			sql_leta.exec();
			while ( sql_leta.next() ) {
				if ( ui->txt_leto->findText(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) == -1 ) {
					ui->txt_leto->addItem(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
				}
			}
		}
		base.close();

		// razvrscanje let po vrsti
		QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->txt_leto);
		proxy->setSourceModel(ui->txt_leto->model());
		// spustni seznam prepisemo
		ui->txt_leto->model()->setParent(proxy);
		ui->txt_leto->setModel(proxy);
		// razvrsti
		ui->txt_leto->model()->sort(0);

		ui->txt_leto->setCurrentIndex(ui->txt_leto->findText(QDate::currentDate().toString("yyyy")));
		ui->txt_mesec->setCurrentIndex(ui->txt_mesec->findText(QDate::currentDate().toString("MM") + ") ", Qt::MatchStartsWith));

		napolni_izbrani_mesec();
		napolni_trenutni_mesec();

		napolni_leto();

		napolni_izbrani_skupni();
		napolni_trenutni_skupni();

		ui->tab_izdatki->setCurrentIndex(0);

}

wid_izdatki::~wid_izdatki()
{
    delete ui;
}

// podatki za mesec
void wid_izdatki::on_txt_leto_currentIndexChanged() {

	napolni_izbrani_mesec();

}

void wid_izdatki::on_txt_mesec_currentIndexChanged() {

	napolni_izbrani_mesec();

}

void wid_izdatki::napolni_izbrani_mesec() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_izdatki_izbrani_mesec");
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

		double skupaj = 0.0;
		double projekti = 0.0;

		// izracunaj prejete racune
		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE datum_placila LIKE '%." + pretvori("01." +
																																															 ui->txt_mesec->currentText().left(2) + "." +
																																															 ui->txt_leto->currentText()).right(7) + "'");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			skupaj += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString())).toDouble();
			if ( sql_napolni.value(sql_napolni.record().indexOf("stevilka_projekta")).toString() != "" ) {
				projekti += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString())).toDouble();
			}
		}
		sql_napolni.clear();

		// pristej potne naloge
		double potni_nalog = 0.0;
		sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + pretvori("01." +
																																														ui->txt_mesec->currentText().left(2) + "." +
																																														ui->txt_leto->currentText()).right(7) + "'");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			skupaj += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stroski_skupaj")).toString())).toDouble();
			potni_nalog += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stroski_skupaj")).toString())).toDouble();
		}

		ui->txt_skupaj_znesek->setText(QString::number(skupaj, 'f', 2).replace(".", ",") + " EUR");
		ui->txt_projektni_znesek->setText(QString::number(projekti, 'f', 2).replace(".", ",") + " EUR");
		ui->txt_prosti_znesek->setText(QString::number(skupaj - projekti - potni_nalog, 'f', 2).replace(".", ",") + " EUR");
		ui->txt_potni_nalog_znesek->setText(QString::number(potni_nalog, 'f', 2).replace(".", ",") + " EUR");

		if ( skupaj == 0.0 ) {
			ui->txt_skupaj_odstotek->setText("0,0 %");
			ui->txt_projektni_odstotek->setText("0,0 %");
			ui->txt_prosti_odstotek->setText("0,0 %");
			ui->txt_potni_nalog_odstotek->setText("0,0 %");
		}
		else {
			ui->txt_skupaj_odstotek->setText(QString::number(skupaj / skupaj * 100, 'f', 2).replace(".", ",") + " %");
			ui->txt_projektni_odstotek->setText(QString::number(projekti / skupaj * 100, 'f', 2).replace(".", ",") + " %");
			ui->txt_prosti_odstotek->setText(QString::number(( skupaj - projekti - potni_nalog ) / skupaj * 100, 'f', 2).replace(".", ",") + " %");
			ui->txt_potni_nalog_odstotek->setText(QString::number(potni_nalog / skupaj * 100, 'f', 2).replace(".", ",") + " %");
		}

	}
	base.close();

}

void wid_izdatki::napolni_trenutni_mesec() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_izdatki_trenutni_mesec");
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
		QString trenutno_leto = QDate::currentDate().toString("yyyy");

		QString meseci[12] = {"01", "02", "03", "04", "05", "06",
															"07", "08", "09", "10", "11", "12"};

		for ( int a = trenutno_leto.toInt(); a >= trenutno_leto.toInt() - 1; a-- ) {
			QString vrednosti[12];
			for ( int b = 0; b < 12; b++ ) {

				// prejeti racuni
				QSqlQuery sql_meseci;
				sql_meseci.prepare("SELECT * FROM prejeti_racuni WHERE datum_placila LIKE '%." + pretvori(meseci[b] + "." + meseci[b] + "." + QString::number(a, 10)).right(7) + "'");
				sql_meseci.exec();

				double vrednost = 0.0;

				while ( sql_meseci.next() ) {
					vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("znesek")).toString())).toDouble();
				} // while ( sql_meseci.next() )

				sql_meseci.clear();

				// potni nalogi
				sql_meseci.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + pretvori(meseci[b] + "." + meseci[b] + "." + QString::number(a, 10)).right(7) + "'");
				sql_meseci.exec();
				while ( sql_meseci.next() ) {
					vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("stroski_skupaj")).toString())).toDouble();
				} // while ( sql_meseci.next() )

				vrednosti[b] = QString::number(vrednost, 'f', 2);
			} // for ( int b = 0; b < 12; b++ )

			if ( a == trenutno_leto.toInt() ) {
				ui->txt_januar_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[0])) + " EUR");
				ui->txt_februar_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[1])) + " EUR");
				ui->txt_marec_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[2])) + " EUR");
				ui->txt_april_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[3])) + " EUR");
				ui->txt_maj_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[4])) + " EUR");
				ui->txt_junij_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[5])) + " EUR");
				ui->txt_julij_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[6])) + " EUR");
				ui->txt_avgust_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[7])) + " EUR");
				ui->txt_september_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[8])) + " EUR");
				ui->txt_oktober_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[9])) + " EUR");
				ui->txt_november_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[10])) + " EUR");
				ui->txt_december_tekoce->setText(pretvori_iz_double(pretvori_v_double(vrednosti[11])) + " EUR");
			} // if ( a == trenutno_leto.toInt() )
			else {
				ui->txt_januar_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[0])) + " EUR");
				ui->txt_februar_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[1])) + " EUR");
				ui->txt_marec_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[2])) + " EUR");
				ui->txt_april_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[3])) + " EUR");
				ui->txt_maj_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[4])) + " EUR");
				ui->txt_junij_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[5])) + " EUR");
				ui->txt_julij_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[6])) + " EUR");
				ui->txt_avgust_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[7])) + " EUR");
				ui->txt_september_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[8])) + " EUR");
				ui->txt_oktober_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[9])) + " EUR");
				ui->txt_november_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[10])) + " EUR");
				ui->txt_december_preteklo->setText(pretvori_iz_double(pretvori_v_double(vrednosti[11])) + " EUR");
			} // else => if ( a == trenutno_leto.toInt() )

		} // for ( int a = trenutno_leto.toInt(); a > trenutno_leto.toInt() - 1; a-- )
	}
	base.close();

	QPalette pozitivno;
	QPalette negativno;
	QPalette nevtralno;

	pozitivno.setColor(QPalette::Base, QColor(200,255,200)); // zelena
	negativno.setColor(QPalette::Base, QColor(255,200,200)); // rdeca
	nevtralno.setColor(QPalette::Base, QColor(255,255,200)); // rumena

	// januar
	if ( ui->txt_januar_preteklo->text() != "0,00 EUR" && ui->txt_januar_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_januar_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_januar_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_januar_preteklo->text()).toDouble() * 100;
		ui->txt_januar_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_januar_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_januar_razlika->setPalette(negativno);
		}
		else {
			ui->txt_januar_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_januar_preteklo->text() == "0,00 EUR" && ui->txt_januar_tekoce->text() != "0,00 EUR" ) {
		ui->txt_januar_razlika->setText("100,0 %");
		ui->txt_januar_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_januar_preteklo->text() != "0,00 EUR" && ui->txt_januar_tekoce->text() == "0,00 EUR" ) {
		ui->txt_januar_razlika->setText("-100,0 %");
		ui->txt_januar_razlika->setPalette(negativno);
	}
	else {
		ui->txt_januar_razlika->setText("0,0 %");
		ui->txt_januar_razlika->setPalette(nevtralno);
	}

	// februar
	if ( ui->txt_februar_preteklo->text() != "0,00 EUR" && ui->txt_februar_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_februar_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_februar_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_februar_preteklo->text()).toDouble() * 100;
		ui->txt_februar_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");

		qDebug(QString::number(razlika, 'f', 2).toAscii());
		if ( razlika > 10 ) {
			ui->txt_februar_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_februar_razlika->setPalette(negativno);
		}
		else {
			ui->txt_februar_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_februar_preteklo->text() == "0,00 EUR" && ui->txt_februar_tekoce->text() != "0,00 EUR" ) {
		ui->txt_februar_razlika->setText("100,0 %");
		ui->txt_februar_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_februar_preteklo->text() != "0,00 EUR" && ui->txt_februar_tekoce->text() == "0,00 EUR" ) {
		ui->txt_februar_razlika->setText("-100,0 %");
		ui->txt_februar_razlika->setPalette(negativno);
	}
	else {
		ui->txt_februar_razlika->setText("0,0 %");
		ui->txt_februar_razlika->setPalette(nevtralno);
	}

	// marec
	if ( ui->txt_marec_preteklo->text() != "0,00 EUR" && ui->txt_marec_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_marec_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_marec_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_marec_preteklo->text()).toDouble() * 100;
		ui->txt_marec_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_marec_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_marec_razlika->setPalette(negativno);
		}
		else {
			ui->txt_marec_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_marec_preteklo->text() == "0,00 EUR" && ui->txt_marec_tekoce->text() != "0,00 EUR" ) {
		ui->txt_marec_razlika->setText("100,0 %");
		ui->txt_marec_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_marec_preteklo->text() != "0,00 EUR" && ui->txt_marec_tekoce->text() == "0,00 EUR" ) {
		ui->txt_marec_razlika->setText("-100,0 %");
		ui->txt_marec_razlika->setPalette(negativno);
	}
	else {
		ui->txt_marec_razlika->setText("0,0 %");
		ui->txt_marec_razlika->setPalette(nevtralno);
	}

	// april
	if ( ui->txt_april_preteklo->text() != "0,00 EUR" && ui->txt_april_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_april_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_april_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_april_preteklo->text()).toDouble() * 100;
		ui->txt_april_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_april_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_april_razlika->setPalette(negativno);
		}
		else {
			ui->txt_april_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_april_preteklo->text() == "0,00 EUR" && ui->txt_april_tekoce->text() != "0,00 EUR" ) {
		ui->txt_april_razlika->setText("100,0 %");
		ui->txt_april_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_april_preteklo->text() != "0,00 EUR" && ui->txt_april_tekoce->text() == "0,00 EUR" ) {
		ui->txt_april_razlika->setText("-100,0 %");
		ui->txt_april_razlika->setPalette(negativno);
	}
	else {
		ui->txt_april_razlika->setText("0,0 %");
		ui->txt_april_razlika->setPalette(nevtralno);
	}

	// maj
	if ( ui->txt_maj_preteklo->text() != "0,00 EUR" && ui->txt_maj_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_maj_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_maj_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_maj_preteklo->text()).toDouble() * 100;
		ui->txt_maj_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_maj_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_maj_razlika->setPalette(negativno);
		}
		else {
			ui->txt_maj_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_maj_preteklo->text() == "0,00 EUR" && ui->txt_maj_tekoce->text() != "0,00 EUR" ) {
		ui->txt_maj_razlika->setText("0,0 %");
		ui->txt_maj_razlika->setPalette(nevtralno);
	}
	else if ( ui->txt_maj_preteklo->text() != "0,00 EUR" && ui->txt_maj_tekoce->text() == "0,00 EUR" ) {
		ui->txt_maj_razlika->setText("-100,0 %");
		ui->txt_maj_razlika->setPalette(negativno);
	}
	else {
		ui->txt_maj_razlika->setText("0,0 %");
		ui->txt_maj_razlika->setPalette(nevtralno);
	}

	// junij
	if ( ui->txt_junij_preteklo->text() != "0,00 EUR" && ui->txt_junij_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_junij_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_junij_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_junij_preteklo->text()).toDouble() * 100;
		ui->txt_junij_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_junij_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_junij_razlika->setPalette(negativno);
		}
		else {
			ui->txt_junij_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_junij_preteklo->text() == "0,00 EUR" && ui->txt_junij_tekoce->text() != "0,00 EUR" ) {
		ui->txt_junij_razlika->setText("100,0 %");
		ui->txt_junij_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_junij_preteklo->text() != "0,00 EUR" && ui->txt_junij_tekoce->text() == "0,00 EUR" ) {
		ui->txt_junij_razlika->setText("-100,0 %");
		ui->txt_junij_razlika->setPalette(negativno);
	}
	else {
		ui->txt_junij_razlika->setText("0,0 %");
		ui->txt_junij_razlika->setPalette(nevtralno);
	}

	// julij
	if ( ui->txt_julij_preteklo->text() != "0,00 EUR" && ui->txt_julij_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_julij_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_julij_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_julij_preteklo->text()).toDouble() * 100;
		ui->txt_julij_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_julij_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_julij_razlika->setPalette(negativno);
		}
		else {
			ui->txt_julij_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_julij_preteklo->text() == "0,00 EUR" && ui->txt_julij_tekoce->text() != "0,00 EUR" ) {
		ui->txt_julij_razlika->setText("100,0 %");
		ui->txt_julij_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_julij_preteklo->text() != "0,00 EUR" && ui->txt_julij_tekoce->text() == "0,00 EUR" ) {
		ui->txt_julij_razlika->setText("-100,0 %");
		ui->txt_julij_razlika->setPalette(negativno);
	}
	else {
		ui->txt_julij_razlika->setText("0,0 %");
		ui->txt_julij_razlika->setPalette(nevtralno);
	}

	// avgust
	if ( ui->txt_avgust_preteklo->text() != "0,00 EUR" && ui->txt_avgust_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_avgust_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_avgust_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_avgust_preteklo->text()).toDouble() * 100;
		ui->txt_avgust_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_avgust_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_avgust_razlika->setPalette(negativno);
		}
		else {
			ui->txt_avgust_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_avgust_preteklo->text() == "0,00 EUR" && ui->txt_avgust_tekoce->text() != "0,00 EUR" ) {
		ui->txt_avgust_razlika->setText("100,0 %");
		ui->txt_avgust_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_avgust_preteklo->text() != "0,00 EUR" && ui->txt_avgust_tekoce->text() == "0,00 EUR" ) {
		ui->txt_avgust_razlika->setText("-100,0 %");
		ui->txt_avgust_razlika->setPalette(negativno);
	}
	else {
		ui->txt_avgust_razlika->setText("0,0 %");
		ui->txt_avgust_razlika->setPalette(nevtralno);
	}

	// september
	if ( ui->txt_september_preteklo->text() != "0,00 EUR" && ui->txt_september_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_september_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_september_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_september_preteklo->text()).toDouble() * 100;
		ui->txt_september_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_september_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_september_razlika->setPalette(negativno);
		}
		else {
			ui->txt_september_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_september_preteklo->text() == "0,00 EUR" && ui->txt_september_tekoce->text() != "0,00 EUR" ) {
		ui->txt_september_razlika->setText("100,0 %");
		ui->txt_september_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_september_preteklo->text() != "0,00 EUR" && ui->txt_september_tekoce->text() == "0,00 EUR" ) {
		ui->txt_september_razlika->setText("-100,0 %");
		ui->txt_september_razlika->setPalette(negativno);
	}
	else {
		ui->txt_september_razlika->setText("0,0 %");
		ui->txt_september_razlika->setPalette(nevtralno);
	}

	// oktober
	if ( ui->txt_oktober_preteklo->text() != "0,00 EUR" && ui->txt_oktober_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_oktober_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_oktober_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_oktober_preteklo->text()).toDouble() * 100;
		ui->txt_oktober_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_oktober_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_oktober_razlika->setPalette(negativno);
		}
		else {
			ui->txt_oktober_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_oktober_preteklo->text() == "0,00 EUR" && ui->txt_oktober_tekoce->text() != "0,00 EUR" ) {
		ui->txt_oktober_razlika->setText("100,0 %");
		ui->txt_oktober_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_oktober_preteklo->text() != "0,00 EUR" && ui->txt_oktober_tekoce->text() == "0,00 EUR" ) {
		ui->txt_oktober_razlika->setText("-100,0 %");
		ui->txt_oktober_razlika->setPalette(negativno);
	}
	else {
		ui->txt_oktober_razlika->setText("0,0 %");
		ui->txt_oktober_razlika->setPalette(nevtralno);
	}

	// november
	if ( ui->txt_november_preteklo->text() != "0,00 EUR" && ui->txt_november_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_november_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_november_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_november_preteklo->text()).toDouble() * 100;
		ui->txt_november_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_november_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_november_razlika->setPalette(negativno);
		}
		else {
			ui->txt_november_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_november_preteklo->text() == "0,00 EUR" && ui->txt_november_tekoce->text() != "0,00 EUR" ) {
		ui->txt_november_razlika->setText("100,0 %");
		ui->txt_november_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_november_preteklo->text() != "0,00 EUR" && ui->txt_november_tekoce->text() == "0,00 EUR" ) {
		ui->txt_november_razlika->setText("-100,0 %");
		ui->txt_november_razlika->setPalette(negativno);
	}
	else {
		ui->txt_november_razlika->setText("0,0 %");
		ui->txt_november_razlika->setPalette(nevtralno);
	}

	// december
	if ( ui->txt_december_preteklo->text() != "0,00 EUR" && ui->txt_december_tekoce->text() != "0,00 EUR" ) {
		double razlika = pretvori_v_double(ui->txt_december_tekoce->text()).toDouble() -
										 pretvori_v_double(ui->txt_december_preteklo->text()).toDouble();
		razlika = razlika / pretvori_v_double(ui->txt_december_preteklo->text()).toDouble() * 100;
		ui->txt_december_razlika->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
		if ( razlika > 10 ) {
			ui->txt_december_razlika->setPalette(pozitivno);
		}
		else if ( razlika < -10 ) {
			ui->txt_december_razlika->setPalette(negativno);
		}
		else {
			ui->txt_december_razlika->setPalette(nevtralno);
		}
	}
	else if ( ui->txt_december_preteklo->text() == "0,00 EUR" && ui->txt_december_tekoce->text() != "0,00 EUR" ) {
		ui->txt_december_razlika->setText("100,0 %");
		ui->txt_december_razlika->setPalette(pozitivno);
	}
	else if ( ui->txt_december_preteklo->text() != "0,00 EUR" && ui->txt_december_tekoce->text() == "0,00 EUR" ) {
		ui->txt_december_razlika->setText("-100,0 %");
		ui->txt_december_razlika->setPalette(negativno);
	}
	else {
		ui->txt_december_razlika->setText("0,0 %");
		ui->txt_december_razlika->setPalette(nevtralno);
	}

}

// podatki za leto
void wid_izdatki::napolni_leto() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "izdatki-letna-tabela");
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

		QStringList leta;
		QSqlQuery sql_leta;
		sql_leta.prepare("SELECT * FROM prejeti_racuni");
		sql_leta.exec();
		while (sql_leta.next()) {
			if ( !leta.count(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) ) {
				leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
			}
		}

		// clear previous content
		ui->tbl_letni_pregled->clear();

		for ( int i = 0; i <= 5; i++ ) {
			ui->tbl_letni_pregled->removeColumn(0);
		}

		for ( int i = 0; i < leta.count(); i++ ) {
			ui->tbl_letni_pregled->removeRow(0);
		}

		// set the table
		ui->tbl_letni_pregled->insertColumn(0);
		ui->tbl_letni_pregled->insertColumn(1);
		ui->tbl_letni_pregled->insertColumn(2);
		ui->tbl_letni_pregled->insertColumn(3);
		ui->tbl_letni_pregled->insertColumn(4);

		QTableWidgetItem *naslov0 = new QTableWidgetItem;
		QTableWidgetItem *naslov1 = new QTableWidgetItem;
		QTableWidgetItem *naslov2 = new QTableWidgetItem;
		QTableWidgetItem *naslov3 = new QTableWidgetItem;
		QTableWidgetItem *naslov4 = new QTableWidgetItem;

		naslov0->setText("Leto");
		naslov1->setText("Skupaj");
		naslov2->setText("Projektni");
		naslov3->setText("Prosti");
		naslov4->setText("Potni nalogi");

		ui->tbl_letni_pregled->setHorizontalHeaderItem(0, naslov0);
		ui->tbl_letni_pregled->setHorizontalHeaderItem(1, naslov1);
		ui->tbl_letni_pregled->setHorizontalHeaderItem(2, naslov2);
		ui->tbl_letni_pregled->setHorizontalHeaderItem(3, naslov3);
		ui->tbl_letni_pregled->setHorizontalHeaderItem(4, naslov4);

		// start collecting data
		for ( int i = 0; i < leta.count(); i++ ) {

			ui->tbl_letni_pregled->insertRow(i);
			ui->tbl_letni_pregled->setRowHeight(i, 20);

			QSqlQuery sql_izdatki;
			sql_izdatki.prepare("SELECT * FROM prejeti_racuni WHERE datum_placila LIKE '%." + pretvori("01.01." + leta.at(i)).right(4) + "'");
			sql_izdatki.exec();

			double skupaj = 0.0;
			double projektni = 0.0;

			while ( sql_izdatki.next() ) {

				skupaj += pretvori_v_double(prevedi(sql_izdatki.value(sql_izdatki.record().indexOf("znesek")).toString())).toDouble();

				if ( prevedi(sql_izdatki.value(sql_izdatki.record().indexOf("stevilka_projekta")).toString()) != "" ) {
					projektni += pretvori_v_double(prevedi(sql_izdatki.value(sql_izdatki.record().indexOf("znesek")).toString())).toDouble();
				}

			} // while ( sql_izdatki.next() )

			sql_izdatki.clear();

			sql_izdatki.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + pretvori("01.01." + leta.at(i)).right(4) + "'");
			sql_izdatki.exec();

			double potni_nalog = 0.0;

			while ( sql_izdatki.next() ) {

				skupaj += pretvori_v_double(prevedi(sql_izdatki.value(sql_izdatki.record().indexOf("stroski_skupaj")).toString())).toDouble();
				potni_nalog += pretvori_v_double(prevedi(sql_izdatki.value(sql_izdatki.record().indexOf("stroski_skupaj")).toString())).toDouble();

			} // while ( sql_izdatki.next() )

			// start filling the table

			QTableWidgetItem *celica0 = new QTableWidgetItem;
			QTableWidgetItem *celica1 = new QTableWidgetItem;
			QTableWidgetItem *celica2 = new QTableWidgetItem;
			QTableWidgetItem *celica3 = new QTableWidgetItem;
			QTableWidgetItem *celica4 = new QTableWidgetItem;

			celica0->setText(leta.at(i));
			celica1->setText(QString::number(skupaj, 'f', 2).replace(".", ",") + " EUR");
			celica2->setText(QString::number(projektni, 'f', 2).replace(".", ",") + " EUR");
			celica3->setText(QString::number(skupaj - projektni - potni_nalog, 'f', 2).replace(".", ",") + " EUR");
			celica4->setText(QString::number(potni_nalog, 'f', 2).replace(".", ",") + " EUR");

			ui->tbl_letni_pregled->setItem(i, 0, celica0);
			ui->tbl_letni_pregled->setItem(i, 1, celica1);
			ui->tbl_letni_pregled->setItem(i, 2, celica2);
			ui->tbl_letni_pregled->setItem(i, 3, celica3);
			ui->tbl_letni_pregled->setItem(i, 4, celica4);

		} // for ( int i = 0; i < leta.count(); i++ )

	}
	base.close();

}

// podatki za skupaj
void wid_izdatki::napolni_izbrani_skupni() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_izdatki_izbrani_skupni");
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

		double skupaj = 0.0;
		double projekti = 0.0;

		// izracunaj prejete racune
		QSqlQuery sql_napolni;
		sql_napolni.prepare("SELECT * FROM prejeti_racuni");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			skupaj += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString())).toDouble();
			if ( sql_napolni.value(sql_napolni.record().indexOf("stevilka_projekta")).toString() != "" ) {
				projekti += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString())).toDouble();
			}
		}
		sql_napolni.clear();

		// pristej potne naloge
		double potni_nalog = 0.0;
		sql_napolni.prepare("SELECT * FROM potni_nalogi");
		sql_napolni.exec();
		while ( sql_napolni.next() ) {
			skupaj += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stroski_skupaj")).toString())).toDouble();
			potni_nalog += pretvori_v_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stroski_skupaj")).toString())).toDouble();
		}

		ui->txt_skupaj_znesek_2->setText(QString::number(skupaj, 'f', 2).replace(".", ",") + " EUR");
		ui->txt_projektni_znesek_2->setText(QString::number(projekti, 'f', 2).replace(".", ",") + " EUR");
		ui->txt_prosti_znesek_2->setText(QString::number(skupaj - projekti - potni_nalog, 'f', 2).replace(".", ",") + " EUR");
		ui->txt_potni_nalog_znesek_2->setText(QString::number(potni_nalog, 'f', 2).replace(".", ",") + " EUR");

		if ( skupaj == 0.0 ) {
			ui->txt_skupaj_odstotek_2->setText("0,0 %");
			ui->txt_projektni_odstotek_2->setText("0,0 %");
			ui->txt_prosti_odstotek_2->setText("0,0 %");
			ui->txt_potni_nalog_odstotek_2->setText("0,0 %");
		}
		else {
			ui->txt_skupaj_odstotek_2->setText(QString::number(skupaj / skupaj * 100, 'f', 2).replace(".", ",") + " %");
			ui->txt_projektni_odstotek_2->setText(QString::number(projekti / skupaj * 100, 'f', 2).replace(".", ",") + " %");
			ui->txt_prosti_odstotek_2->setText(QString::number(( skupaj - projekti - potni_nalog ) / skupaj * 100, 'f', 2).replace(".", ",") + " %");
			ui->txt_potni_nalog_odstotek_2->setText(QString::number(potni_nalog / skupaj * 100, 'f', 2).replace(".", ",") + " %");
		}

	}
	base.close();

}

void wid_izdatki::napolni_trenutni_skupni() {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_izdatki_trenutni_mesec");
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

		// poisci vsa leta, v katerih so izdani racuni
		QStringList leta;

		QSqlQuery sql_leta;
		sql_leta.prepare("SELECT * FROM prejeti_racuni");
		sql_leta.exec();

		while ( sql_leta.next() ) {
			if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) ) {
				leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
			}
		}

		QString meseci[12] = {"01", "02", "03", "04", "05", "06",
															"07", "08", "09", "10", "11", "12"};

		QString vrednosti[12];
		for ( int b = 0; b < 12; b++ ) {

			double vrednost = 0.0;

			// zaokrozimo skozi vsa leta (zaradi tega lahko tvorimo tocen datum *.MM.yyyy, samo z mesecem bi dobili *.MM.* in MM.*)
			for ( int c = 0; c < leta.count(); c++ ) {

				// prejeti racuni
				QSqlQuery sql_meseci;
				sql_meseci.prepare("SELECT * FROM prejeti_racuni WHERE datum_placila LIKE '%." + pretvori(meseci[b] + "." + leta.at(c)) + "'");
				sql_meseci.exec();

				while ( sql_meseci.next() ) {
					vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("znesek")).toString())).toDouble();
				} // while ( sql_meseci.next() )

				sql_meseci.clear();

				// potni nalogi
				sql_meseci.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + pretvori(meseci[b] + "." + leta.at(c)) + "'");
				sql_meseci.exec();
				while ( sql_meseci.next() ) {
					vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("stroski_skupaj")).toString())).toDouble();
				} // while ( sql_meseci.next() )

			} // for ( int c = 0; c < leta.count(); c++ )

			vrednosti[b] = QString::number(vrednost, 'f', 2);
		} // for ( int b = 0; b < 12; b++ )

		// skupni zneski po mesecih
		ui->txt_januar_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[0])) + " EUR");
		ui->txt_februar_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[1])) + " EUR");
		ui->txt_marec_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[2])) + " EUR");
		ui->txt_april_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[3])) + " EUR");
		ui->txt_maj_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[4])) + " EUR");
		ui->txt_junij_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[5])) + " EUR");
		ui->txt_julij_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[6])) + " EUR");
		ui->txt_avgust_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[7])) + " EUR");
		ui->txt_september_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[8])) + " EUR");
		ui->txt_oktober_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[9])) + " EUR");
		ui->txt_november_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[10])) + " EUR");
		ui->txt_december_tekoce_2->setText(pretvori_iz_double(pretvori_v_double(vrednosti[11])) + " EUR");

		// povprecni zneski po mesecih
		ui->txt_januar_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[0]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_februar_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[1]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_marec_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[2]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_april_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[3]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_maj_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[4]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_junij_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[5]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_julij_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[6]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_avgust_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[7]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_september_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[8]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_oktober_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[9]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_november_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[10]).toDouble() / leta.count(), 'f', 2)) + " EUR");
		ui->txt_december_preteklo_2->setText(pretvori_iz_double(QString::number(pretvori_v_double( vrednosti[11]).toDouble() / leta.count(), 'f', 2)) + " EUR");

	}
	base.close();

}

// splosno
QString wid_izdatki::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString wid_izdatki::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

QString wid_izdatki::pretvori_v_double(QString besedilo) {

	besedilo.remove(" ");
	besedilo.remove("%");
	besedilo.remove("EUR");
	besedilo.replace(",", ".");

	if ( besedilo.left(1) == "0" ) {
		besedilo.remove(0,1);
	}
	if ( besedilo == "" ) { // ce je polje prazno, dodamo vrednost 0.0
		besedilo.append("0.0");
	}
	if ( besedilo.left(1) == "." ) { // ce besedilo nima vodilne nicle, pa je pricakovana, jo dodamo
		besedilo.prepend("0");
	}
	if ( besedilo.right(1) == "." ) { // ce ima besedilo decimalno locilo, za njim pa nic, dodamo 0
		besedilo.append("0");
	}
	if ( !besedilo.contains(".") ) { // ce je celo stevilo dodamo decimalno locilo in vrednost 0
		besedilo.append(".0");
	}

	return besedilo;

}

QString wid_izdatki::pretvori_iz_double(QString besedilo) {

	besedilo.replace(".",",");

	return besedilo;

}

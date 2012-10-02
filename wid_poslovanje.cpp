#include <QtSql>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QDate>

#include "wid_poslovanje.h"
#include "ui_wid_poslovanje.h"
#include "kodiranje.h"

wid_poslovanje::wid_poslovanje(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_poslovanje)
{
    ui->setupUi(this);

        // leto
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_poslovanje_spustni_seznami");
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
            // najprej pregledamo prejete racune
            sql_leta.prepare("SELECT * FROM prejeti_racuni");
            sql_leta.exec();
            while ( sql_leta.next() ) {
                if ( ui->txt_leto->findText(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) == -1 ) {
                    ui->txt_leto->addItem(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
                }
            }
            sql_leta.clear();
            // nato pregledano izdane racune
            sql_leta.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND status_oddaje_racuna NOT LIKE ''");
            sql_leta.exec();
            while ( sql_leta.next() ) {
                QString leto = prevedi(sql_leta.value(sql_leta.record().indexOf("datum_izdaje")).toString()).right(4);
                if ( ui->txt_leto->findText(leto) == -1 ) {
                    ui->txt_leto->addItem(leto);
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

        napolni_skupni();

        napolni_mesec();

        napolni_leto();

        ui->tab_poslovanje->setCurrentIndex(0);

}

wid_poslovanje::~wid_poslovanje()
{
    delete ui;
}

// podatki za mesec
void wid_poslovanje::on_txt_leto_currentIndexChanged() {

    napolni_mesec();

}

void wid_poslovanje::napolni_mesec() {

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
        QString trenutno_leto = ui->txt_leto->currentText();

        QString meseci[12] = {"01", "02", "03", "04", "05", "06",
                                                            "07", "08", "09", "10", "11", "12"};

        QString vrednost_izdanih[12];
        QString vrednost_prejetih[12];
        QString vrednost_poslovanja[12];

        for ( int b = 0; b < 12; b++ ) {

            // prejeti racuni
            QSqlQuery sql_meseci;
            sql_meseci.prepare("SELECT * FROM prejeti_racuni WHERE datum_placila LIKE '%." + pretvori(meseci[b] + "." + meseci[b] + "." + trenutno_leto).right(7) + "'");
            sql_meseci.exec();

            double vrednost = 0.0;

            while ( sql_meseci.next() ) {
                vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("znesek")).toString())).toDouble();
            } // while ( sql_meseci.next() )

            sql_meseci.clear();

            // potni nalogi
            sql_meseci.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + pretvori(meseci[b] + "." + meseci[b] + "." + trenutno_leto).right(7) + "'");
            sql_meseci.exec();
            while ( sql_meseci.next() ) {
                vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("stroski_skupaj")).toString())).toDouble();
            } // while ( sql_meseci.next() )

            vrednost_izdanih[b] = QString::number(vrednost, 'f', 2);

            // izdani racuni
            // poisci, kateri avansi so bili placani v danem mesecu
            QSqlQuery sql_avans;
            double znesek_avans = 0.0;
            double znesek_ostanek = 0.0;
            sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND datum_placila_avansa LIKE '%." +
                                                pretvori("01." + meseci[b] + "." + trenutno_leto).right(7) + "'");
            sql_avans.exec();
            while ( sql_avans.next() ) {
                znesek_avans += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
            }

            // poisci preostanek placila
            QSqlQuery sql_preostanek;
            sql_preostanek.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
                                                         " AND datum_placila LIKE '%." + pretvori("01." + meseci[b] + "." + trenutno_leto).right(7) + "'"
                                                         " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
            sql_preostanek.exec();
            while ( sql_preostanek.next() ) {
                QSqlQuery sql_opravilo;
                sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
                sql_opravilo.exec();
                while ( sql_opravilo.next() ) {
                    znesek_ostanek += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
                                                        pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
                }
                sql_opravilo.clear();
                // odstejemo ze placan avans
                znesek_ostanek -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
            }

            vrednost_prejetih[b] = QString::number(znesek_avans + znesek_ostanek, 'f', 2);

            // poslovanje
            vrednost_poslovanja[b] = QString::number(znesek_avans + znesek_ostanek - vrednost, 'f', 2);

        } // for ( int b = 0; b < 12; b++ )

        // prihodki
        ui->txt_tp_januar->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[0])) + " EUR");
        ui->txt_tp_februar->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[1])) + " EUR");
        ui->txt_tp_marec->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[2])) + " EUR");
        ui->txt_tp_april->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[3])) + " EUR");
        ui->txt_tp_maj->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[4])) + " EUR");
        ui->txt_tp_junij->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[5])) + " EUR");
        ui->txt_tp_julij->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[6])) + " EUR");
        ui->txt_tp_avgust->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[7])) + " EUR");
        ui->txt_tp_september->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[8])) + " EUR");
        ui->txt_tp_oktober->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[9])) + " EUR");
        ui->txt_tp_november->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[10])) + " EUR");
        ui->txt_tp_december->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[11])) + " EUR");
        // izdatki
        ui->txt_ti_januar->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[0])) + " EUR");
        ui->txt_ti_februar->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[1])) + " EUR");
        ui->txt_ti_marec->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[2])) + " EUR");
        ui->txt_ti_april->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[3])) + " EUR");
        ui->txt_ti_maj->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[4])) + " EUR");
        ui->txt_ti_junij->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[5])) + " EUR");
        ui->txt_ti_julij->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[6])) + " EUR");
        ui->txt_ti_avgust->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[7])) + " EUR");
        ui->txt_ti_september->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[8])) + " EUR");
        ui->txt_ti_oktober->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[9])) + " EUR");
        ui->txt_ti_november->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[10])) + " EUR");
        ui->txt_ti_december->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[11])) + " EUR");
        // poslovanje
        ui->txt_tpos_januar->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[0])) + " EUR");
        ui->txt_tpos_februar->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[1])) + " EUR");
        ui->txt_tpos_marec->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[2])) + " EUR");
        ui->txt_tpos_april->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[3])) + " EUR");
        ui->txt_tpos_maj->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[4])) + " EUR");
        ui->txt_tpos_junij->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[5])) + " EUR");
        ui->txt_tpos_julij->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[6])) + " EUR");
        ui->txt_tpos_avgust->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[7])) + " EUR");
        ui->txt_tpos_september->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[8])) + " EUR");
        ui->txt_tpos_oktober->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[9])) + " EUR");
        ui->txt_tpos_november->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[10])) + " EUR");
        ui->txt_tpos_december->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[11])) + " EUR");
        // povprecje
        ui->txt_pp_januar->setText(ui->txt_pp_januar_2->text());
        ui->txt_pp_februar->setText(ui->txt_pp_februar_2->text());
        ui->txt_pp_marec->setText(ui->txt_pp_marec_2->text());
        ui->txt_pp_april->setText(ui->txt_pp_april_2->text());
        ui->txt_pp_maj->setText(ui->txt_pp_maj_2->text());
        ui->txt_pp_junij->setText(ui->txt_pp_junij_2->text());
        ui->txt_pp_julij->setText(ui->txt_pp_julij_2->text());
        ui->txt_pp_avgust->setText(ui->txt_pp_avgust_2->text());
        ui->txt_pp_september->setText(ui->txt_pp_september_2->text());
        ui->txt_pp_oktober->setText(ui->txt_pp_oktober_2->text());
        ui->txt_pp_november->setText(ui->txt_pp_november_2->text());
        ui->txt_pp_december->setText(ui->txt_pp_december_2->text());

        QPalette pozitivno;
        QPalette negativno;
        QPalette nevtralno;

        pozitivno.setColor(QPalette::Base, QColor(200,255,200)); // zelena
        negativno.setColor(QPalette::Base, QColor(255,200,200)); // rdeca
        nevtralno.setColor(QPalette::Base, QColor(255,255,200)); // rumena

        // barvanje pri poslovanju
        // januar
        if ( ui->txt_tpos_januar->text() == "0,00 EUR" ) {
            ui->txt_tpos_januar->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_januar->text().left(1) == "-" ) {
            ui->txt_tpos_januar->setPalette(negativno);
        }
        else {
            ui->txt_tpos_januar->setPalette(pozitivno);
        }
        // februar
        if ( ui->txt_tpos_februar->text() == "0,00 EUR" ) {
            ui->txt_tpos_februar->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_februar->text().left(1) == "-" ) {
            ui->txt_tpos_februar->setPalette(negativno);
        }
        else {
            ui->txt_tpos_februar->setPalette(pozitivno);
        }
        // marec
        if ( ui->txt_tpos_marec->text() == "0,00 EUR" ) {
            ui->txt_tpos_marec->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_marec->text().left(1) == "-" ) {
            ui->txt_tpos_marec->setPalette(negativno);
        }
        else {
            ui->txt_tpos_marec->setPalette(pozitivno);
        }
        // april
        if ( ui->txt_tpos_april->text() == "0,00 EUR" ) {
            ui->txt_tpos_april->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_april->text().left(1) == "-" ) {
            ui->txt_tpos_april->setPalette(negativno);
        }
        else {
            ui->txt_tpos_april->setPalette(pozitivno);
        }
        // maj
        if ( ui->txt_tpos_maj->text() == "0,00 EUR" ) {
            ui->txt_tpos_maj->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_maj->text().left(1) == "-" ) {
            ui->txt_tpos_maj->setPalette(negativno);
        }
        else {
            ui->txt_tpos_maj->setPalette(pozitivno);
        }
        // junij
        if ( ui->txt_tpos_junij->text() == "0,00 EUR" ) {
            ui->txt_tpos_junij->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_junij->text().left(1) == "-" ) {
            ui->txt_tpos_junij->setPalette(negativno);
        }
        else {
            ui->txt_tpos_junij->setPalette(pozitivno);
        }
        // julij
        if ( ui->txt_tpos_julij->text() == "0,00 EUR" ) {
            ui->txt_tpos_julij->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_julij->text().left(1) == "-" ) {
            ui->txt_tpos_julij->setPalette(negativno);
        }
        else {
            ui->txt_tpos_julij->setPalette(pozitivno);
        }
        // avgust
        if ( ui->txt_tpos_avgust->text() == "0,00 EUR" ) {
            ui->txt_tpos_avgust->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_avgust->text().left(1) == "-" ) {
            ui->txt_tpos_avgust->setPalette(negativno);
        }
        else {
            ui->txt_tpos_avgust->setPalette(pozitivno);
        }
        // september
        if ( ui->txt_tpos_september->text() == "0,00 EUR" ) {
            ui->txt_tpos_september->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_september->text().left(1) == "-" ) {
            ui->txt_tpos_september->setPalette(negativno);
        }
        else {
            ui->txt_tpos_september->setPalette(pozitivno);
        }
        // oktober
        if ( ui->txt_tpos_oktober->text() == "0,00 EUR" ) {
            ui->txt_tpos_oktober->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_oktober->text().left(1) == "-" ) {
            ui->txt_tpos_oktober->setPalette(negativno);
        }
        else {
            ui->txt_tpos_oktober->setPalette(pozitivno);
        }
        // november
        if ( ui->txt_tpos_november->text() == "0,00 EUR" ) {
            ui->txt_tpos_november->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_november->text().left(1) == "-" ) {
            ui->txt_tpos_november->setPalette(negativno);
        }
        else {
            ui->txt_tpos_november->setPalette(pozitivno);
        }
        // december
        if ( ui->txt_tpos_december->text() == "0,00 EUR" ) {
            ui->txt_tpos_december->setPalette(nevtralno);
        }
        else if ( ui->txt_tpos_december->text().left(1) == "-" ) {
            ui->txt_tpos_december->setPalette(negativno);
        }
        else {
            ui->txt_tpos_december->setPalette(pozitivno);
        }

        // razlika
        // januar
        if ( ui->txt_pp_januar->text() != "0,00 EUR" && ui->txt_tpos_januar->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_januar->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_januar->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_januar->text()).toDouble() * 100;
            ui->txt_razlika_januar->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_januar->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_januar->setPalette(negativno);
            }
            else {
                ui->txt_razlika_januar->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_januar->text() == "0,00 EUR" && ui->txt_tpos_januar->text() != "0,00 EUR" ) {
            ui->txt_razlika_januar->setText("100,0 %");
            ui->txt_razlika_januar->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_januar->text() != "0,00 EUR" && ui->txt_tpos_januar->text() == "0,00 EUR" ) {
            ui->txt_razlika_januar->setText("-100,0 %");
            ui->txt_razlika_januar->setPalette(negativno);
        }
        else {
            ui->txt_razlika_januar->setText("0,0 %");
            ui->txt_razlika_januar->setPalette(nevtralno);
        }

        // februar
        if ( ui->txt_pp_februar->text() != "0,00 EUR" && ui->txt_tpos_februar->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_februar->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_februar->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_februar->text()).toDouble() * 100;
            ui->txt_razlika_februar->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_februar->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_februar->setPalette(negativno);
            }
            else {
                ui->txt_razlika_februar->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_februar->text() == "0,00 EUR" && ui->txt_tpos_februar->text() != "0,00 EUR" ) {
            ui->txt_razlika_februar->setText("100,0 %");
            ui->txt_razlika_februar->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_februar->text() != "0,00 EUR" && ui->txt_tpos_februar->text() == "0,00 EUR" ) {
            ui->txt_razlika_februar->setText("-100,0 %");
            ui->txt_razlika_februar->setPalette(negativno);
        }
        else {
            ui->txt_razlika_februar->setText("0,0 %");
            ui->txt_razlika_februar->setPalette(nevtralno);
        }

        // marec
        if ( ui->txt_pp_marec->text() != "0,00 EUR" && ui->txt_tpos_marec->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_marec->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_marec->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_marec->text()).toDouble() * 100;
            ui->txt_razlika_marec->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_marec->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_marec->setPalette(negativno);
            }
            else {
                ui->txt_razlika_marec->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_marec->text() == "0,00 EUR" && ui->txt_tpos_marec->text() != "0,00 EUR" ) {
            ui->txt_razlika_marec->setText("100,0 %");
            ui->txt_razlika_marec->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_marec->text() != "0,00 EUR" && ui->txt_tpos_marec->text() == "0,00 EUR" ) {
            ui->txt_razlika_marec->setText("-100,0 %");
            ui->txt_razlika_marec->setPalette(negativno);
        }
        else {
            ui->txt_razlika_marec->setText("0,0 %");
            ui->txt_razlika_marec->setPalette(nevtralno);
        }

        // april
        if ( ui->txt_pp_april->text() != "0,00 EUR" && ui->txt_tpos_april->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_april->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_april->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_april->text()).toDouble() * 100;
            ui->txt_razlika_april->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_april->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_april->setPalette(negativno);
            }
            else {
                ui->txt_razlika_april->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_april->text() == "0,00 EUR" && ui->txt_tpos_april->text() != "0,00 EUR" ) {
            ui->txt_razlika_april->setText("100,0 %");
            ui->txt_razlika_april->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_april->text() != "0,00 EUR" && ui->txt_tpos_april->text() == "0,00 EUR" ) {
            ui->txt_razlika_april->setText("-100,0 %");
            ui->txt_razlika_april->setPalette(negativno);
        }
        else {
            ui->txt_razlika_april->setText("0,0 %");
            ui->txt_razlika_april->setPalette(nevtralno);
        }

        // maj
        if ( ui->txt_pp_maj->text() != "0,00 EUR" && ui->txt_tpos_maj->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_maj->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_maj->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_maj->text()).toDouble() * 100;
            ui->txt_razlika_maj->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_maj->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_maj->setPalette(negativno);
            }
            else {
                ui->txt_razlika_maj->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_maj->text() == "0,00 EUR" && ui->txt_tpos_maj->text() != "0,00 EUR" ) {
            ui->txt_razlika_maj->setText("0,0 %");
            ui->txt_razlika_maj->setPalette(nevtralno);
        }
        else if ( ui->txt_pp_maj->text() != "0,00 EUR" && ui->txt_tpos_maj->text() == "0,00 EUR" ) {
            ui->txt_razlika_maj->setText("-100,0 %");
            ui->txt_razlika_maj->setPalette(negativno);
        }
        else {
            ui->txt_razlika_maj->setText("0,0 %");
            ui->txt_razlika_maj->setPalette(nevtralno);
        }

        // junij
        if ( ui->txt_pp_junij->text() != "0,00 EUR" && ui->txt_tpos_junij->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_junij->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_junij->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_junij->text()).toDouble() * 100;
            ui->txt_razlika_junij->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_junij->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_junij->setPalette(negativno);
            }
            else {
                ui->txt_razlika_junij->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_junij->text() == "0,00 EUR" && ui->txt_tpos_junij->text() != "0,00 EUR" ) {
            ui->txt_razlika_junij->setText("100,0 %");
            ui->txt_razlika_junij->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_junij->text() != "0,00 EUR" && ui->txt_tpos_junij->text() == "0,00 EUR" ) {
            ui->txt_razlika_junij->setText("-100,0 %");
            ui->txt_razlika_junij->setPalette(negativno);
        }
        else {
            ui->txt_razlika_junij->setText("0,0 %");
            ui->txt_razlika_junij->setPalette(nevtralno);
        }

        // julij
        if ( ui->txt_pp_julij->text() != "0,00 EUR" && ui->txt_tpos_julij->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_julij->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_julij->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_julij->text()).toDouble() * 100;
            ui->txt_razlika_julij->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_julij->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_julij->setPalette(negativno);
            }
            else {
                ui->txt_razlika_julij->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_julij->text() == "0,00 EUR" && ui->txt_tpos_julij->text() != "0,00 EUR" ) {
            ui->txt_razlika_julij->setText("100,0 %");
            ui->txt_razlika_julij->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_julij->text() != "0,00 EUR" && ui->txt_tpos_julij->text() == "0,00 EUR" ) {
            ui->txt_razlika_julij->setText("-100,0 %");
            ui->txt_razlika_julij->setPalette(negativno);
        }
        else {
            ui->txt_razlika_julij->setText("0,0 %");
            ui->txt_razlika_julij->setPalette(nevtralno);
        }

        // avgust
        if ( ui->txt_pp_avgust->text() != "0,00 EUR" && ui->txt_tpos_avgust->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_avgust->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_avgust->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_avgust->text()).toDouble() * 100;
            ui->txt_razlika_avgust->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_avgust->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_avgust->setPalette(negativno);
            }
            else {
                ui->txt_razlika_avgust->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_avgust->text() == "0,00 EUR" && ui->txt_tpos_avgust->text() != "0,00 EUR" ) {
            ui->txt_razlika_avgust->setText("100,0 %");
            ui->txt_razlika_avgust->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_avgust->text() != "0,00 EUR" && ui->txt_tpos_avgust->text() == "0,00 EUR" ) {
            ui->txt_razlika_avgust->setText("-100,0 %");
            ui->txt_razlika_avgust->setPalette(negativno);
        }
        else {
            ui->txt_razlika_avgust->setText("0,0 %");
            ui->txt_razlika_avgust->setPalette(nevtralno);
        }

        // september
        if ( ui->txt_pp_september->text() != "0,00 EUR" && ui->txt_tpos_september->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_september->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_september->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_september->text()).toDouble() * 100;
            ui->txt_razlika_september->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_september->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_september->setPalette(negativno);
            }
            else {
                ui->txt_razlika_september->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_september->text() == "0,00 EUR" && ui->txt_tpos_september->text() != "0,00 EUR" ) {
            ui->txt_razlika_september->setText("100,0 %");
            ui->txt_razlika_september->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_september->text() != "0,00 EUR" && ui->txt_tpos_september->text() == "0,00 EUR" ) {
            ui->txt_razlika_september->setText("-100,0 %");
            ui->txt_razlika_september->setPalette(negativno);
        }
        else {
            ui->txt_razlika_september->setText("0,0 %");
            ui->txt_razlika_september->setPalette(nevtralno);
        }

        // oktober
        if ( ui->txt_pp_oktober->text() != "0,00 EUR" && ui->txt_tpos_oktober->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_oktober->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_oktober->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_oktober->text()).toDouble() * 100;
            ui->txt_razlika_oktober->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_oktober->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_oktober->setPalette(negativno);
            }
            else {
                ui->txt_razlika_oktober->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_oktober->text() == "0,00 EUR" && ui->txt_tpos_oktober->text() != "0,00 EUR" ) {
            ui->txt_razlika_oktober->setText("100,0 %");
            ui->txt_razlika_oktober->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_oktober->text() != "0,00 EUR" && ui->txt_tpos_oktober->text() == "0,00 EUR" ) {
            ui->txt_razlika_oktober->setText("-100,0 %");
            ui->txt_razlika_oktober->setPalette(negativno);
        }
        else {
            ui->txt_razlika_oktober->setText("0,0 %");
            ui->txt_razlika_oktober->setPalette(nevtralno);
        }

        // november
        if ( ui->txt_pp_november->text() != "0,00 EUR" && ui->txt_tpos_november->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_november->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_november->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_november->text()).toDouble() * 100;
            ui->txt_razlika_november->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_november->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_november->setPalette(negativno);
            }
            else {
                ui->txt_razlika_november->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_november->text() == "0,00 EUR" && ui->txt_tpos_november->text() != "0,00 EUR" ) {
            ui->txt_razlika_november->setText("100,0 %");
            ui->txt_razlika_november->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_november->text() != "0,00 EUR" && ui->txt_tpos_november->text() == "0,00 EUR" ) {
            ui->txt_razlika_november->setText("-100,0 %");
            ui->txt_razlika_november->setPalette(negativno);
        }
        else {
            ui->txt_razlika_november->setText("0,0 %");
            ui->txt_razlika_november->setPalette(nevtralno);
        }

        // december
        if ( ui->txt_pp_december->text() != "0,00 EUR" && ui->txt_tpos_december->text() != "0,00 EUR" ) {
            double razlika = pretvori_v_double(ui->txt_tpos_december->text()).toDouble() -
                                             pretvori_v_double(ui->txt_pp_december->text()).toDouble();
            razlika = razlika / pretvori_v_double(ui->txt_pp_december->text()).toDouble() * 100;
            ui->txt_razlika_december->setText(QString::number(razlika, 'f', 1).replace(".", ",") + " %");
            if ( razlika > 10 ) {
                ui->txt_razlika_december->setPalette(pozitivno);
            }
            else if ( razlika < -10 ) {
                ui->txt_razlika_december->setPalette(negativno);
            }
            else {
                ui->txt_razlika_december->setPalette(nevtralno);
            }
        }
        else if ( ui->txt_pp_december->text() == "0,00 EUR" && ui->txt_tpos_december->text() != "0,00 EUR" ) {
            ui->txt_razlika_december->setText("100,0 %");
            ui->txt_razlika_december->setPalette(pozitivno);
        }
        else if ( ui->txt_pp_december->text() != "0,00 EUR" && ui->txt_tpos_december->text() == "0,00 EUR" ) {
            ui->txt_razlika_december->setText("-100,0 %");
            ui->txt_razlika_december->setPalette(negativno);
        }
        else {
            ui->txt_razlika_december->setText("0,0 %");
            ui->txt_razlika_december->setPalette(nevtralno);
        }


    }
    base.close();

}

// podatki za letni
void wid_poslovanje::napolni_leto() {

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_poslovanje->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_poslovanje->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_poslovanje->horizontalHeader()->sortIndicatorSection();

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

        // dolocimo stevilo let, skozi katere mora aplikacija iti
        QStringList leta;

        QSqlQuery sql_leta;
        sql_leta.prepare("SELECT * FROM prejeti_racuni");
        sql_leta.exec();

        while ( sql_leta.next() ) {
            if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) ) {
                leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
            }
        }
        sql_leta.clear();

        sql_leta.prepare("SELECT * FROM potni_nalogi");
        sql_leta.exec();

        while ( sql_leta.next() ) {
            if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_naloga")).toString()).right(4)) ) {
                leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_naloga")).toString()).right(4));
            }
        }
        sql_leta.clear();

        sql_leta.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
        sql_leta.exec();

        while ( sql_leta.next() ) {
            if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) ) {
                leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
            }
        }

        // clear previous content
        ui->tbl_poslovanje->clear();

        for ( int i = 0; i <= 4; i++ ) {
            ui->tbl_poslovanje->removeColumn(0);
        }

        for ( int i = 0; i < leta.count(); i++ ) {
            ui->tbl_poslovanje->removeRow(0);
        }

        // set the table
        ui->tbl_poslovanje->insertColumn(0);
        ui->tbl_poslovanje->insertColumn(1);
        ui->tbl_poslovanje->insertColumn(2);
        ui->tbl_poslovanje->insertColumn(3);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;

        naslov0->setText("Leto");
        naslov1->setText("Poslovanje");
        naslov2->setText("Izdatki");
        naslov3->setText("Prejemki");

        ui->tbl_poslovanje->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_poslovanje->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_poslovanje->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_poslovanje->setHorizontalHeaderItem(3, naslov3);

        // start collecting data
        for ( int i = 0; i < leta.count(); i++ ) {

            ui->tbl_poslovanje->insertRow(i);
            ui->tbl_poslovanje->setRowHeight(i, 20);

            // vrednosti
            double izdatki = 0.0;
            double prejemki = 0.0;

            // izdatki
            QSqlQuery sql_izdatki;
            sql_izdatki.prepare("SELECT * FROM prejeti_racuni WHERE datum_placila LIKE '%." + pretvori("01.01." + leta.at(i)).right(4) + "'");
            sql_izdatki.exec();

            while ( sql_izdatki.next() ) {

                izdatki += pretvori_v_double(prevedi(sql_izdatki.value(sql_izdatki.record().indexOf("znesek")).toString())).toDouble();

            } // while ( sql_izdatki.next() )

            sql_izdatki.clear();

            sql_izdatki.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + pretvori("01.01." + leta.at(i)).right(4) + "'");
            sql_izdatki.exec();

            while ( sql_izdatki.next() ) {

                izdatki += pretvori_v_double(prevedi(sql_izdatki.value(sql_izdatki.record().indexOf("stroski_skupaj")).toString())).toDouble();

            } // while ( sql_izdatki.next() )

            // prejemki
            // poisci, kateri avansi so bili placani v danem mesecu
            double znesek_avans = 0.0;
            double znesek_ostanek = 0.0;

            QSqlQuery sql_avans;
            sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND datum_placila LIKE '%." + pretvori("01.01." + leta.at(i)).right(4) + "'");
            sql_avans.exec();
            while ( sql_avans.next() ) {
                znesek_avans += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
            }

            // poisci preostanek placila
            QSqlQuery sql_preostanek;
            sql_preostanek.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
                                                         " AND datum_placila LIKE '%." + pretvori("01.01." + leta.at(i)).right(4) + "'"
                                                         " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
            sql_preostanek.exec();
            while ( sql_preostanek.next() ) {
                QSqlQuery sql_opravilo;
                sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
                sql_opravilo.exec();
                while ( sql_opravilo.next() ) {
                    znesek_ostanek += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
                                                        pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
                }
                sql_opravilo.clear();
                // odstejemo ze placan avans
                znesek_ostanek -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
            }

            prejemki = znesek_avans + znesek_ostanek;

            // start filling the table

            QTableWidgetItem *celica0 = new QTableWidgetItem;
            QTableWidgetItem *celica1 = new QTableWidgetItem;
            QTableWidgetItem *celica2 = new QTableWidgetItem;
            QTableWidgetItem *celica3 = new QTableWidgetItem;

            celica0->setText(leta.at(i));
            celica1->setText(QString::number(prejemki - izdatki, 'f', 2).replace(".", ",") + " EUR");
            celica2->setText(QString::number(izdatki, 'f', 2).replace(".", ",") + " EUR");
            celica3->setText(QString::number(prejemki, 'f', 2).replace(".", ",") + " EUR");

            ui->tbl_poslovanje->setItem(i, 0, celica0);
            ui->tbl_poslovanje->setItem(i, 1, celica1);
            ui->tbl_poslovanje->setItem(i, 2, celica2);
            ui->tbl_poslovanje->setItem(i, 3, celica3);

        } // for ( int i = 0; i < leta.count(); i++ )

    }
    base.close();

    ui->tbl_poslovanje->selectRow(izbranec);
    ui->tbl_poslovanje->sortByColumn(razvrsti, Qt::AscendingOrder);

}

// podatki za skupni
void wid_poslovanje::napolni_skupni() {

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

        // dolocimo stevilo let, skozi katere mora aplikacija iti
        QStringList leta;

        QSqlQuery sql_leta;
        sql_leta.prepare("SELECT * FROM prejeti_racuni");
        sql_leta.exec();

        while ( sql_leta.next() ) {
            if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) ) {
                leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
            }
        }
        sql_leta.clear();

        sql_leta.prepare("SELECT * FROM potni_nalogi");
        sql_leta.exec();

        while ( sql_leta.next() ) {
            if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_naloga")).toString()).right(4)) ) {
                leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_naloga")).toString()).right(4));
            }
        }
        sql_leta.clear();

        sql_leta.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
        sql_leta.exec();

        while ( sql_leta.next() ) {
            if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4)) ) {
                leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_placila")).toString()).right(4));
            }
        }

        QString meseci[12] = {"01", "02", "03", "04", "05", "06",
                                                            "07", "08", "09", "10", "11", "12"};

        QString vrednost_izdanih[12];
        QString vrednost_prejetih[12];
        QString vrednost_poslovanja[12];
        QString povprecna_vrednost_poslovanja[12];

        for ( int b = 0; b < 12; b++ ) {

            double vrednost = 0.0;
            double znesek_avans = 0.0;
            double znesek_ostanek = 0.0;

            // zanka gre skozi vsa leta
            for ( int c = 0; c < leta.count(); c++ ) {

                // prejeti racuni
                QSqlQuery sql_meseci;
                sql_meseci.prepare("SELECT * FROM prejeti_racuni WHERE datum_placila LIKE '%." + pretvori(meseci[b] + "." + meseci[b] + "." + leta.at(c)).right(7) + "'");
                sql_meseci.exec();


                while ( sql_meseci.next() ) {
                    vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("znesek")).toString())).toDouble();
                } // while ( sql_meseci.next() )

                sql_meseci.clear();

                // potni nalogi
                sql_meseci.prepare("SELECT * FROM potni_nalogi WHERE datum_naloga LIKE '%." + pretvori(meseci[b] + "." + meseci[b] + "." + leta.at(c)).right(7) + "'");
                sql_meseci.exec();
                while ( sql_meseci.next() ) {
                    vrednost += pretvori_v_double(prevedi(sql_meseci.value(sql_meseci.record().indexOf("stroski_skupaj")).toString())).toDouble();
                } // while ( sql_meseci.next() )


                // izdani racuni
                // poisci, kateri avansi so bili placani v danem mesecu
                QSqlQuery sql_avans;
                sql_avans.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3' AND datum_placila_avansa LIKE '%." +
                                                    pretvori("01." + meseci[b] + "." + leta.at(c)).right(7) + "'");
                sql_avans.exec();
                while ( sql_avans.next() ) {
                    znesek_avans += pretvori_v_double(prevedi(sql_avans.value(sql_avans.record().indexOf("avans")).toString())).toDouble();
                }

                // poisci preostanek placila
                QSqlQuery sql_preostanek;
                sql_preostanek.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '3'"
                                                             " AND datum_placila LIKE '%." + pretvori("01." + meseci[b] + "." + leta.at(c)).right(7) + "'"
                                                             " AND status_placila LIKE '" + pretvori("Pla") + "%" + pretvori("Placano").right(3) + "'");
                sql_preostanek.exec();
                while ( sql_preostanek.next() ) {
                    QSqlQuery sql_opravilo;
                    sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_preostanek.value(sql_preostanek.record().indexOf("id")).toString() + "'");
                    sql_opravilo.exec();
                    while ( sql_opravilo.next() ) {
                        znesek_ostanek += pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString())).toDouble() +
                                                            pretvori_v_double(prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString())).toDouble();
                    }
                    sql_opravilo.clear();
                    // odstejemo ze placan avans
                    znesek_ostanek -= pretvori_v_double(prevedi(sql_preostanek.value(sql_preostanek.record().indexOf("avans")).toString())).toDouble();
                }

            } // for ( int c = 0; c < leta.count(); c++ )

            vrednost_izdanih[b] = QString::number(vrednost, 'f', 2);
            vrednost_prejetih[b] = QString::number(znesek_avans + znesek_ostanek, 'f', 2);
            vrednost_poslovanja[b] = QString::number(znesek_avans + znesek_ostanek - vrednost, 'f', 2);
            povprecna_vrednost_poslovanja[b] = QString::number(( znesek_avans + znesek_ostanek - vrednost ) / leta.count(), 'f', 2);

        } // for ( int b = 0; b < 12; b++ )

        // prihodki
        ui->txt_tp_januar_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[0])) + " EUR");
        ui->txt_tp_februar_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[1])) + " EUR");
        ui->txt_tp_marec_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[2])) + " EUR");
        ui->txt_tp_april_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[3])) + " EUR");
        ui->txt_tp_maj_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[4])) + " EUR");
        ui->txt_tp_junij_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[5])) + " EUR");
        ui->txt_tp_julij_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[6])) + " EUR");
        ui->txt_tp_avgust_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[7])) + " EUR");
        ui->txt_tp_september_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[8])) + " EUR");
        ui->txt_tp_oktober_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[9])) + " EUR");
        ui->txt_tp_november_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[10])) + " EUR");
        ui->txt_tp_december_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_prejetih[11])) + " EUR");
        // izdatki
        ui->txt_ti_januar_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[0])) + " EUR");
        ui->txt_ti_februar_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[1])) + " EUR");
        ui->txt_ti_marec_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[2])) + " EUR");
        ui->txt_ti_april_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[3])) + " EUR");
        ui->txt_ti_maj_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[4])) + " EUR");
        ui->txt_ti_junij_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[5])) + " EUR");
        ui->txt_ti_julij_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[6])) + " EUR");
        ui->txt_ti_avgust_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[7])) + " EUR");
        ui->txt_ti_september_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[8])) + " EUR");
        ui->txt_ti_oktober_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[9])) + " EUR");
        ui->txt_ti_november_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[10])) + " EUR");
        ui->txt_ti_december_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_izdanih[11])) + " EUR");
        // poslovanje
        ui->txt_tpos_januar_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[0])) + " EUR");
        ui->txt_tpos_februar_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[1])) + " EUR");
        ui->txt_tpos_marec_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[2])) + " EUR");
        ui->txt_tpos_april_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[3])) + " EUR");
        ui->txt_tpos_maj_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[4])) + " EUR");
        ui->txt_tpos_junij_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[5])) + " EUR");
        ui->txt_tpos_julij_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[6])) + " EUR");
        ui->txt_tpos_avgust_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[7])) + " EUR");
        ui->txt_tpos_september_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[8])) + " EUR");
        ui->txt_tpos_oktober_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[9])) + " EUR");
        ui->txt_tpos_november_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[10])) + " EUR");
        ui->txt_tpos_december_2->setText(pretvori_iz_double(pretvori_v_double(vrednost_poslovanja[11])) + " EUR");
        // povprecje
        ui->txt_pp_januar_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[0])) + " EUR");
        ui->txt_pp_februar_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[1])) + " EUR");
        ui->txt_pp_marec_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[2])) + " EUR");
        ui->txt_pp_april_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[3])) + " EUR");
        ui->txt_pp_maj_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[4])) + " EUR");
        ui->txt_pp_junij_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[5])) + " EUR");
        ui->txt_pp_julij_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[6])) + " EUR");
        ui->txt_pp_avgust_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[7])) + " EUR");
        ui->txt_pp_september_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[8])) + " EUR");
        ui->txt_pp_oktober_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[9])) + " EUR");
        ui->txt_pp_november_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[10])) + " EUR");
        ui->txt_pp_december_2->setText(pretvori_iz_double(pretvori_v_double(povprecna_vrednost_poslovanja[11])) + " EUR");
    }
    base.close();

    // barvanje pri poslovanju
    QPalette pozitivno;
    QPalette negativno;
    QPalette nevtralno;

    pozitivno.setColor(QPalette::Base, QColor(200,255,200)); // zelena
    negativno.setColor(QPalette::Base, QColor(255,200,200)); // rdeca
    nevtralno.setColor(QPalette::Base, QColor(255,255,200)); // rumena

    // januar
    if ( ui->txt_tpos_januar_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_januar_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_januar_2->text().left(1) == "-" ) {
        ui->txt_tpos_januar_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_januar_2->setPalette(pozitivno);
    }
    // februar
    if ( ui->txt_tpos_februar_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_februar_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_februar_2->text().left(1) == "-" ) {
        ui->txt_tpos_februar_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_februar_2->setPalette(pozitivno);
    }
    // marec
    if ( ui->txt_tpos_marec_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_marec_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_marec_2->text().left(1) == "-" ) {
        ui->txt_tpos_marec_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_marec_2->setPalette(pozitivno);
    }
    // april
    if ( ui->txt_tpos_april_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_april_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_april_2->text().left(1) == "-" ) {
        ui->txt_tpos_april_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_april_2->setPalette(pozitivno);
    }
    // maj
    if ( ui->txt_tpos_maj_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_maj_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_maj_2->text().left(1) == "-" ) {
        ui->txt_tpos_maj_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_maj_2->setPalette(pozitivno);
    }
    // junij
    if ( ui->txt_tpos_junij_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_junij_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_junij_2->text().left(1) == "-" ) {
        ui->txt_tpos_junij_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_junij_2->setPalette(pozitivno);
    }
    // julij
    if ( ui->txt_tpos_julij_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_julij_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_julij_2->text().left(1) == "-" ) {
        ui->txt_tpos_julij_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_julij_2->setPalette(pozitivno);
    }
    // avgust
    if ( ui->txt_tpos_avgust_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_avgust_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_avgust_2->text().left(1) == "-" ) {
        ui->txt_tpos_avgust_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_avgust_2->setPalette(pozitivno);
    }
    // september
    if ( ui->txt_tpos_september_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_september_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_september_2->text().left(1) == "-" ) {
        ui->txt_tpos_september_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_september_2->setPalette(pozitivno);
    }
    // oktober
    if ( ui->txt_tpos_oktober_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_oktober_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_oktober_2->text().left(1) == "-" ) {
        ui->txt_tpos_oktober_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_oktober_2->setPalette(pozitivno);
    }
    // november
    if ( ui->txt_tpos_november_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_november_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_november_2->text().left(1) == "-" ) {
        ui->txt_tpos_november_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_november_2->setPalette(pozitivno);
    }
    // december
    if ( ui->txt_tpos_december_2->text() == "0,00 EUR" ) {
        ui->txt_tpos_december_2->setPalette(nevtralno);
    }
    else if ( ui->txt_tpos_december_2->text().left(1) == "-" ) {
        ui->txt_tpos_december_2->setPalette(negativno);
    }
    else {
        ui->txt_tpos_december_2->setPalette(pozitivno);
    }

}

// splosno
QString wid_poslovanje::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_poslovanje::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

QString wid_poslovanje::pretvori_v_double(QString besedilo) {

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

QString wid_poslovanje::pretvori_iz_double(QString besedilo) {

    besedilo.replace(".",",");

    return besedilo;

}

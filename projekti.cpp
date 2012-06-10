#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "projekti.h"
#include "ui_projekti.h"
#include "opravila.h"
#include "kodiranje.h"
#include "wid_racuni.h"
#include "wid_prejetiracuni.h"
#include "wid_potninalogi.h"
#include "varnost.h"

projekti::projekti(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::projekti)
{
    ui->setupUi(this);

        // izprazni vsa polja
        ui->txt_id->setText("");
        ui->txt_stevilka_projekta->setText("");
        ui->txt_naziv_projekta->setText("");
        ui->txt_stranka->clear();
        ui->txt_stranka_id->setText("");
        ui->txt_pricetek->setDate(QDate::currentDate());
        ui->txt_konec->setDate(QDate::currentDate());
        ui->txt_status_projekta->clear();
        ui->txt_st_izdanih_racunov->setText("");
        ui->txt_st_prejetih_racunov->setText("");
        ui->txt_znesek_izdanih_racunov->setText("");
        ui->txt_znesek_prejetih_racunov->setText("");
        ui->txt_znesek_skupaj->setText("");
        ui->txt_st_potnih_nalogov->setText("");
        ui->txt_st_ur_dela->setText("");

        ui->txt_id_zapisa->setText("");
        ui->tbl_zapisi->clear();
        ui->txt_datum_zapisa->setDateTime(QDateTime::currentDateTime());
        ui->txt_naslov_zapisa->setText("");
        ui->txt_opis_zapisa->setPlainText("");

        ui->txt_izdani_brez_ddv->setText("");
        ui->txt_izdani_ddv->setText("");
        ui->txt_izdani_popusti->setText("");
        ui->txt_izdani_placilo->setText("");

        ui->txt_prejeti_brez_ddv->setText("");
        ui->txt_prejeti_ddv->setText("");
        ui->txt_prejeti_placilo->setText("");

        ui->cb_popust_fb1->setText("");
        ui->cb_popust_fb2->setText("");
        ui->cb_popust_komb1->setText("");
        ui->cb_popust_komb2->setText("");
        ui->cb_popust_stalna->setText("");
        ui->cb_popust_kupon->setText("");
        ui->cb_popust_akcija->setText("");
        ui->cb_podrazitev_vikend->setText("");
        ui->cb_podrazitev_hitrost->setText("");
        ui->cb_podrazitev_zapleti->setText("");
        ui->txt_popust_fb1->setText(pretvori_iz_double("0"));
        ui->txt_popust_fb2->setText(pretvori_iz_double("0"));
        ui->txt_popust_komb1->setText(pretvori_iz_double("0"));
        ui->txt_popust_komb2->setText(pretvori_iz_double("0"));
        ui->txt_popust_stalna_stranka->setText(pretvori_iz_double("0"));
        ui->txt_popust_kupon->setText(pretvori_iz_double("0"));
        ui->txt_popust_akcija->setText(pretvori_iz_double("0"));
        ui->txt_vsi_popusti_facebook_2->setText(pretvori_iz_double("0"));
        ui->txt_popusti_skupaj_2->setText(pretvori_iz_double("0"));

        ui->txt_cas_racun_1->clear();
        ui->txt_cas_opravilo_1->clear();
        ui->txt_cas_racun_2->clear();
        ui->txt_cas_opravilo_2->clear();
        ui->txt_cas_od->setTime(QTime::fromString("00:00", "HH:mm"));
        ui->txt_cas_do->setTime(QTime::fromString("00:00", "HH:mm"));
        ui->txt_cas_trajanje->setTime(QTime::fromString("00:00", "HH:mm"));
        ui->txt_datum->setDate(QDate::currentDate());
        ui->tbl_casovnice->clear();

        // onemogoci polja s popusti
        ui->txt_popust_fb1->setEnabled(false);
        ui->txt_popust_fb2->setEnabled(false);
        ui->txt_popust_komb1->setEnabled(false);
        ui->txt_popust_komb2->setEnabled(false);
        ui->txt_popust_stalna_stranka->setEnabled(false);
        ui->txt_popust_kupon->setEnabled(false);
        ui->txt_popust_akcija->setEnabled(false);
        ui->txt_vsi_popusti_facebook_1->setEnabled(false);
        ui->txt_vsi_popusti_facebook_2->setEnabled(false);
        ui->txt_popusti_skupaj_1->setEnabled(false);
        ui->txt_popusti_skupaj_2->setEnabled(false);
        ui->txt_podrazitev_vikend->setEnabled(false);
        ui->txt_podrazitev_hitrost->setEnabled(false);
        ui->txt_podrazitev_zapleti->setEnabled(false);
        ui->tab_casovnice->setEnabled(false);
        ui->tab_zapisi->setEnabled(false);

        ui->tab_projekti->setCurrentIndex(0);

    // onemogoci polja
        ui->txt_id->setEnabled(false);
        ui->txt_stevilka_projekta->setEnabled(false);
        ui->txt_stranka_id->setEnabled(false);
//	ui->txt_konec->setEnabled(false);
//	ui->txt_pricetek->setEnabled(false);
        ui->txt_izdani_brez_ddv->setEnabled(false);
        ui->txt_izdani_ddv->setEnabled(false);
        ui->txt_izdani_popusti->setEnabled(false);
        ui->txt_izdani_placilo->setEnabled(false);
        ui->txt_prejeti_brez_ddv->setEnabled(false);
        ui->txt_prejeti_ddv->setEnabled(false);
        ui->txt_prejeti_placilo->setEnabled(false);

        ui->txt_stranka_id->setHidden(true);
        ui->txt_id_zapisa->setHidden(true);

    // skrij polja z urami dela
    ui->label_47->setVisible(false);
    ui->txt_st_ur_dela->setVisible(false);

    // napolni tabele in spustne sezname
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
        // baza je odprta

        // napolni spustne sezname
        ui->txt_status_projekta->addItem("");
        ui->txt_stranka->addItem("");

        QSqlQuery sql_fill_combo;
        sql_fill_combo.prepare("SELECT * FROM sif_status_projekta");
        sql_fill_combo.exec();
        while (sql_fill_combo.next()) {
            ui->txt_status_projekta->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("status")).toString()));
        }

        sql_fill_combo.clear();
        sql_fill_combo.prepare("SELECT * FROM stranke");
        sql_fill_combo.exec();
        while (sql_fill_combo.next()) {
            if (prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("tip")).toString()) == "1" ) {
                ui->txt_stranka->addItem(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
                                         + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("priimek")).toString()) + " "
                                         + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
            }
            else {
                ui->txt_stranka->addItem(sql_fill_combo.value(sql_fill_combo.record().indexOf("id")).toString() + ") "
                                         + prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
            }
        }

        // napolni max popuste
        QSqlQuery sql_popusti;
        sql_popusti.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("max_pop_vsi") + "'");
        sql_popusti.exec();
        if ( sql_popusti.next() ) {
            ui->txt_popusti_skupaj_1->setText(prevedi(pretvori_iz_double(sql_popusti.value(sql_popusti.record().indexOf("vrednost")).toString())));
        }

        sql_popusti.clear();
        sql_popusti.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("max_pop_facebook") + "'");
        sql_popusti.exec();
        if ( sql_popusti.next() ) {
            ui->txt_vsi_popusti_facebook_1->setText(pretvori_iz_double(prevedi(sql_popusti.value(sql_popusti.record().indexOf("vrednost")).toString())));
        }

        sql_popusti.clear();
        sql_popusti.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_vikend") + "'");
        sql_popusti.exec();
        if ( sql_popusti.next() ) {
            ui->txt_podrazitev_vikend->setText(pretvori_iz_double(prevedi(sql_popusti.value(sql_popusti.record().indexOf("vrednost")).toString())));
        }

        sql_popusti.clear();
        sql_popusti.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_hitrost") + "'");
        sql_popusti.exec();
        if ( sql_popusti.next() ) {
            ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(prevedi(sql_popusti.value(sql_popusti.record().indexOf("vrednost")).toString())));
        }

        sql_popusti.clear();
        sql_popusti.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pod_zapleti") + "'");
        sql_popusti.exec();
        if ( sql_popusti.next() ) {
            ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(prevedi(sql_popusti.value(sql_popusti.record().indexOf("vrednost")).toString())));
        }

        // napolni casovnice
    }
    base.close();

    napolni_podatke();

//	stevilka_racuna();

}

projekti::~projekti()
{
    delete ui;

}

void projekti::on_txt_status_projekta_currentIndexChanged() {

    if ( ui->txt_status_projekta->currentText().left(6) == "Zaklju" ) { // Zakljuceno
        ui->label_6->setText("Datum konca dela:");
    }
    else {
        ui->label_6->setText("Predviden datum konca dela:");
    }

}

void projekti::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Return)
    {
        this->on_btn_sprejmi_clicked();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        this->on_btn_izhod_clicked();
    }
    else if ((event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::AltModifier))
    {
        this->on_btn_brisi_clicked();
    }

}

// prazno, dokler ne definiramo funkcije gumba
void projekti::on_btn_brisi_clicked() {

}

void projekti::on_btn_izhod_clicked() {

    close();

}

// ne preverja obveznih polj
void projekti::on_btn_sprejmi_clicked() {

    QString napaka = "";
/*
    // nastavitev polja za napako
    QFont font_error;
    font_error.setBold(true);
    font_error.setUnderline(true);

    QFont font_normal;
    font_normal.setBold(false);
    font_normal.setUnderline(false);

    QPalette palette_error;
    QPalette palette_normal;

    QBrush brush_error(QColor(255, 0, 0, 255));
    brush_error.setStyle(Qt::SolidPattern);

    QBrush brush_normal(QColor(0, 0, 0, 255));
    brush_normal.setStyle(Qt::SolidPattern);

    palette_error.setBrush(QPalette::Active, QPalette::WindowText, brush_error);
    palette_error.setBrush(QPalette::Inactive, QPalette::WindowText, brush_error);
    palette_normal.setBrush(QPalette::Active, QPalette::WindowText, brush_normal);
    palette_normal.setBrush(QPalette::Inactive, QPalette::WindowText, brush_normal);
*/
    // javi napake, ce ni napak vnesi v bazo
    if (napaka == "") {

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
            QSqlQuery sql_status_projekta;
            sql_status_projekta.prepare("SELECT * FROM sif_status_projekta WHERE status LIKE '" + pretvori(ui->txt_status_projekta->currentText()) + "'");
            QString status_projekta = "0";
            sql_status_projekta.exec();
            if ( sql_status_projekta.next() ) {
                status_projekta = sql_status_projekta.value(sql_status_projekta.record().indexOf("id")).toString();
            }

            QSqlQuery sql_vnesi_projekt;
            if (ui->btn_sprejmi->text() == "Vnesi projekt") {
                sql_vnesi_projekt.prepare("INSERT INTO projekti (stevilka_projekta, naslov_projekta, stranka, avtor_oseba, pricetek_dela, konec_dela, "
                                                                    "status_projekta, popust_fb1, popust_fb2, popust_komb1, popust_komb2, popust_stranka, popust_kupon, "
                                                                    "popust_akcija, podrazitev_vikend, podrazitev_hitrost, podrazitev_zapleti) "
                                                                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            }
            else {
                sql_vnesi_projekt.prepare("UPDATE projekti SET stevilka_projekta = ?, naslov_projekta = ?, stranka = ?, avtor_oseba = ?, pricetek_dela = ?, "
                                                                    "konec_dela = ?, status_projekta = ?, popust_fb1 = ?, popust_fb2 = ?, popust_komb1 = ?, "
                                                                    "popust_komb2 = ?, popust_stranka = ?, popust_kupon = ?, popust_akcija = ?, podrazitev_vikend = ?, "
                                                                    "podrazitev_hitrost = ?, podrazitev_zapleti = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
            }
            sql_vnesi_projekt.bindValue(0, pretvori(ui->txt_stevilka_projekta->text()));
            sql_vnesi_projekt.bindValue(1, pretvori(ui->txt_naziv_projekta->text()));
            sql_vnesi_projekt.bindValue(2, pretvori(ui->txt_stranka_id->text()));
            sql_vnesi_projekt.bindValue(3, pretvori(vApp->id()));
            sql_vnesi_projekt.bindValue(4, pretvori(ui->txt_pricetek->text()));
            sql_vnesi_projekt.bindValue(5, pretvori(ui->txt_konec->text()));
            sql_vnesi_projekt.bindValue(6, pretvori(status_projekta));
            sql_vnesi_projekt.bindValue(7, pretvori(pretvori_v_double(ui->txt_popust_fb1->text())));
            sql_vnesi_projekt.bindValue(8, pretvori(pretvori_v_double(ui->txt_popust_fb2->text())));
            sql_vnesi_projekt.bindValue(9, pretvori(pretvori_v_double(ui->txt_popust_komb1->text())));
            sql_vnesi_projekt.bindValue(10, pretvori(pretvori_v_double(ui->txt_popust_komb2->text())));
            sql_vnesi_projekt.bindValue(11, pretvori(pretvori_v_double(ui->txt_popust_stalna_stranka->text())));
            sql_vnesi_projekt.bindValue(12, pretvori(pretvori_v_double(ui->txt_popust_kupon->text())));
            sql_vnesi_projekt.bindValue(13, pretvori(pretvori_v_double(ui->txt_popust_akcija->text())));
            sql_vnesi_projekt.bindValue(14, pretvori(pretvori_v_double(ui->txt_podrazitev_vikend->text())));
            sql_vnesi_projekt.bindValue(15, pretvori(pretvori_v_double(ui->txt_podrazitev_hitrost->text())));
            sql_vnesi_projekt.bindValue(16, pretvori(pretvori_v_double(ui->txt_podrazitev_zapleti->text())));

            sql_vnesi_projekt.exec();
        }

        base.close();

        // send signal to reload widget
        poslji("projekti");

        // close this window
        close();
    }
    else {
        QMessageBox msgbox;
        msgbox.setText("Dolocena polja niso pravilno izpolnjena");
        msgbox.exec();
    }

}

void projekti::prejem(QString besedilo) {

    if (besedilo.left(11) == "Nov projekt") {
        ui->btn_sprejmi->setText("Vnesi projekt");
        ui->txt_stranka_id->setText(besedilo.right(besedilo.length() - 11));
        ui->wid_prejeti_racuni->setEnabled(false);
        ui->wid_izdani_racuni->setEnabled(false);
        ui->wid_potni_nalogi->setEnabled(false);
        ui->txt_pricetek->setEnabled(true);
    }
    else {
        ui->btn_sprejmi->setText("Polnim");
        ui->wid_prejeti_racuni->setEnabled(true);
        ui->wid_izdani_racuni->setEnabled(true);
        ui->wid_potni_nalogi->setEnabled(true);
        ui->txt_pricetek->setEnabled(false);
        // besedilo nosi ID ze obstojeco stranko, potrebno je napolniti polja
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE");
        base.setDatabaseName(dbase_path);
        base.database();
        base.open();
        if ( base.isOpen() != true ) {
            QMessageBox msgbox;
            msgbox.setText("Baze ni bilo moc odpreti");
            msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
            msgbox.exec();
        }
        else {
            QString status_projekta = "";
            QString stranka = "";

            QSqlQuery sql_napolni;
            sql_napolni.prepare("SELECT * FROM projekti WHERE stevilka_projekta LIKE '" + pretvori(besedilo) + "'");
            sql_napolni.exec();
            if ( sql_napolni.next() ) {
                ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
                ui->txt_stevilka_projekta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilka_projekta")).toString()));
                ui->txt_naziv_projekta->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_projekta")).toString()));
                stranka = prevedi(sql_napolni.value(sql_napolni.record().indexOf("stranka")).toString()); // zamik zaradi veckratnega odpiranja/zapiranja baze

                QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("pricetek_dela")).toString()), "dd'.'MM'.'yyyy");
                ui->txt_pricetek->setDate(datum);
                datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("konec_dela")).toString()), "dd'.'MM'.'yyyy");
                ui->txt_konec->setDate(datum);

                status_projekta = sql_napolni.value(sql_napolni.record().indexOf("status_projekta")).toString();

                // napolni popuste
                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_fb1")).toString()) != "0.0") {
                    ui->cb_popust_fb1->setCheckState(Qt::Checked);
                    ui->txt_popust_fb1->setEnabled(true);
                }
                else {
                    ui->cb_popust_fb1->setCheckState(Qt::Unchecked);
                    ui->txt_popust_fb1->setEnabled(false);
                }
                ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_fb1")).toString())));

                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_fb2")).toString()) != "0.0") {
                    ui->cb_popust_fb2->setCheckState(Qt::Checked);
                    ui->txt_popust_fb2->setEnabled(true);
                }
                else {
                    ui->cb_popust_fb2->setCheckState(Qt::Unchecked);
                    ui->txt_popust_fb2->setEnabled(false);
                }
                ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_fb2")).toString())));

                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_komb1")).toString()) != "0.0") {
                    ui->cb_popust_komb1->setCheckState(Qt::Checked);
                    ui->txt_popust_komb1->setEnabled(true);
                }
                else {
                    ui->cb_popust_komb1->setCheckState(Qt::Unchecked);
                    ui->txt_popust_komb1->setEnabled(false);
                }
                ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_komb1")).toString())));

                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_komb2")).toString()) != "0.0") {
                    ui->cb_popust_komb2->setCheckState(Qt::Checked);
                    ui->txt_popust_komb2->setEnabled(true);
                }
                else {
                    ui->cb_popust_komb2->setCheckState(Qt::Unchecked);
                    ui->txt_popust_komb2->setEnabled(false);
                }
                ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_komb2")).toString())));

                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_stranka")).toString()) != "0.0") {
                    ui->cb_popust_stalna->setCheckState(Qt::Checked);
                    ui->txt_popust_stalna_stranka->setEnabled(true);
                }
                else {
                    ui->cb_popust_stalna->setCheckState(Qt::Unchecked);
                    ui->txt_popust_stalna_stranka->setEnabled(false);
                }
                ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_stranka")).toString())));

                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_kupon")).toString()) != "0.0") {
                    ui->cb_popust_kupon->setCheckState(Qt::Checked);
                    ui->txt_popust_kupon->setEnabled(true);
                }
                else {
                    ui->cb_popust_kupon->setCheckState(Qt::Unchecked);
                    ui->txt_popust_kupon->setEnabled(false);
                }
                ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_kupon")).toString())));

                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_akcija")).toString()) != "0.0") {
                    ui->cb_popust_akcija->setCheckState(Qt::Checked);
                    ui->txt_popust_akcija->setEnabled(true);
                }
                else {
                    ui->cb_popust_akcija->setCheckState(Qt::Unchecked);
                    ui->txt_popust_akcija->setEnabled(false);
                }
                ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("popust_akcija")).toString())));

                ui->txt_podrazitev_vikend->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podrazitev_vikend")).toString())));
                ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podrazitev_hitrost")).toString())));
                ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("podrazitev_zapleti")).toString())));
            }

            sql_napolni.clear();
            sql_napolni.prepare("SELECT * FROM sif_status_projekta WHERE id LIKE '" + status_projekta + "'");
            sql_napolni.exec();
            if ( sql_napolni.next() ) {
                ui->txt_status_projekta->setCurrentIndex(ui->txt_status_projekta->findText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString())));
            }
            ui->txt_stranka_id->setText(stranka);

        }
        base.close();

        // widget pri izdanih racunih
        wid_racuni *widizd = new wid_racuni;
        ui->wid_izdani_racuni->setWidget(widizd);

        QObject::connect(this, SIGNAL(prenos(QString)),
                     widizd , SLOT(prejem(QString)));
        prenos(ui->txt_id->text());
        this->disconnect();

        // widget pri prejetih racunih
        wid_prejetiracuni *widpre = new wid_prejetiracuni;
        ui->wid_prejeti_racuni->setWidget(widpre);

        QObject::connect(this, SIGNAL(prenos(QString)),
                     widpre , SLOT(prejem(QString)));
        prenos(ui->txt_id->text());
        this->disconnect();

        // widget pri potnih nalogih
        wid_potninalogi *widpot = new wid_potninalogi;
        ui->wid_potni_nalogi->setWidget(widpot);

        QObject::connect(this, SIGNAL(prenos(QString)),
                     widpot , SLOT(prejem(QString)));
        prenos(ui->txt_id->text());
        this->disconnect();

        izracunaj(); // calculate the values

        napolni_zapise(); // napolni opombe in zapise

        napolni_casovnice(); // napolni casovnice

        ui->btn_sprejmi->setText("Popravi vnos");
    }

}

QString projekti::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString projekti::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void projekti::on_txt_stranka_currentIndexChanged(QString besedilo) {

    if (besedilo != "") {
        ui->txt_stranka_id->setText(besedilo.left(besedilo.indexOf(") ", 1)));
        ui->txt_stranka_id->setText(ui->txt_stranka_id->text().right(ui->txt_stranka_id->text().length()));
    }
    else {
        ui->txt_stranka_id->setText("");
    }

}

void projekti::on_txt_stranka_id_textChanged(QString besedilo) {

    int i = 0;

    while ( i <= ui->txt_stranka->count() ) {
        if ( ui->txt_stranka->itemText(i).left(besedilo.length() + 2) == (besedilo + ") ") ) {
            ui->txt_stranka->setCurrentIndex(i);
        }
        i++;
    }

    // widget pri izdanih racunih
    wid_racuni *widizd = new wid_racuni;
    ui->wid_izdani_racuni->setWidget(widizd);

    QObject::connect(this, SIGNAL(prenos(QString)),
                 widizd , SLOT(prejem(QString)));
    prenos(ui->txt_stranka_id->text());

    // widget pri prejetih racunih
    wid_prejetiracuni *widpre = new wid_prejetiracuni;
    ui->wid_prejeti_racuni->setWidget(widpre);

    QObject::connect(this, SIGNAL(prenos(QString)),
                 widpre , SLOT(prejem(QString)));
    prenos(ui->txt_stranka_id->text());

    // widget pri potnih nalogih
    wid_potninalogi *widpot = new wid_potninalogi;
    ui->wid_potni_nalogi->setWidget(widpot);

    QObject::connect(this, SIGNAL(prenos(QString)),
                 widpot , SLOT(prejem(QString)));
    prenos(ui->txt_stranka_id->text());

    izracunaj();

}

void projekti::izracunaj() {

    double d_znesekbrezddv = 0;
    double d_ddv = 0;
    double d_popusti = 0;
    double d_znesek = 0;

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
        QSqlQuery sql_racun;

        // vsota izdanih racunov
        sql_racun.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori("3") + "'");
        sql_racun.exec();
        while ( sql_racun.next() ) {
            QSqlQuery sql_opravilo;
            sql_opravilo.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_racun.value(sql_racun.record().indexOf("id")).toString() + "'");
            sql_opravilo.exec();
            while ( sql_opravilo.next() ) {
                d_znesekbrezddv = d_znesekbrezddv + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_koncni")).toString()).toDouble();
                d_ddv = d_ddv + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_ddv")).toString()).toDouble();
                d_popusti = d_popusti + prevedi(sql_opravilo.value(sql_opravilo.record().indexOf("znesek_popustov")).toString()).toDouble();
            }
        }
        ui->txt_izdani_brez_ddv->setText(QString::number(d_znesekbrezddv, 'f', 2));
        ui->txt_izdani_ddv->setText(QString::number(d_ddv, 'f', 2));
        ui->txt_izdani_popusti->setText(QString::number(d_popusti, 'f', 2));
        ui->txt_izdani_placilo->setText(QString::number(d_znesekbrezddv + d_ddv, 'f', 2));

        // vsota prejetih racunov

        d_znesekbrezddv = 0;
        d_ddv = 0;
        d_popusti = 0;
        d_znesek = 0;

        sql_racun.clear();
        sql_racun.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_racuna LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
        sql_racun.exec();
        while ( sql_racun.next() ) {
            d_znesekbrezddv = d_znesekbrezddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_brez_ddv")).toString()).toDouble();
            d_ddv = d_ddv + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek_ddv")).toString()).toDouble();
            d_znesek = d_znesek + prevedi(sql_racun.value(sql_racun.record().indexOf("znesek")).toString()).toDouble();
        }
        ui->txt_prejeti_brez_ddv->setText(QString::number(d_znesekbrezddv, 'f', 2));
        ui->txt_prejeti_ddv->setText(QString::number(d_ddv, 'f', 2));
        ui->txt_prejeti_placilo->setText(QString::number(d_znesek, 'f', 2));
    }
    base.close();

    napolni_podatke();

}

void projekti::izracunaj_popuste(double vrednost, int polje) {

    /*
        * racunali bomo po naslednjem principu
        * najprej izracunamo sestevek vseh vnosnih polj s popusti (razen akcijskega)
        * vrednost mora biti nujno manjsa od najvisje dovoljenje vrednosti
        * ce ni, takoj prekini racunanje in vrni sporocilo z napako
        * poglej, ali bi bil sestevek vseh polj vecji od najvisje dovoljene vrednosti
        * ce ni, vnesi stevilko v polje in popravi sestevke
        * ce je, vnesi najvisjo dovoljeno vrednost v polje in popravi sestevke ter
        * obvesti uporabnika s sporocilom
        */

    /*
    * polje == 0 => fb1
    * polje == 1 => fb2
    * polje == 2 => komb1
    * polje == 3 => komb2
    * polje == 4 => stranka
    * polje == 5 => kupon
    */

    // vsa vnosna polja s popustom pretvorimo v vrednost double, da bo mozno racunanje
    double popust[6];
    popust[0] = pretvori_v_double(ui->txt_popust_fb1->text()).toDouble();
    popust[1] = pretvori_v_double(ui->txt_popust_fb2->text()).toDouble();
    popust[2] = pretvori_v_double(ui->txt_popust_komb1->text()).toDouble();
    popust[3] = pretvori_v_double(ui->txt_popust_komb2->text()).toDouble();
    popust[4] = pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();
    popust[5] = pretvori_v_double(ui->txt_popust_kupon->text()).toDouble();

    // maksimalne vrednosti sestevka popustov
    double max_fb = pretvori_v_double(ui->txt_vsi_popusti_facebook_1->text()).toDouble();
    double max_vsi = pretvori_v_double(ui->txt_popusti_skupaj_1->text()).toDouble();

    double sestevek;
    sestevek = popust[0] + popust[1];

    if ( sestevek > max_fb ) {
        if ( polje == 0 ) {
            popust[0] = max_fb - popust[1];
            ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust[0], 'f', 1)));
        }
        else if ( polje == 1 ) {
            popust[1] = max_fb - popust[0];
            ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust[1], 'f', 1)));
        }
        QMessageBox msg_napaka;
        msg_napaka.setText("Vnesli ste previsoko vrednost. Vrednost je avtomatsko "
                                             "popravljena na najvisjo dovoljena vrednost za to polje!");
        msg_napaka.exec();
        return;
    }

    sestevek = popust[0] + popust[1];
    ui->txt_vsi_popusti_facebook_2->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)));

    sestevek = 0.0;
    for ( int i = 0; i <= 5; i++ ) {
        sestevek += popust[i];
    }
    if ( sestevek > max_vsi ) {
        if ( polje == 0 ) {
            popust[0] = max_vsi - sestevek + popust[0];
            ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust[0], 'f', 1)));
        }
        else if ( polje == 1 ) {
            popust[1] = max_vsi - sestevek + popust[1];
            ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust[1], 'f', 1)));
        }
        else if ( polje == 2 ) {
            popust[2] = max_vsi - sestevek + popust[2];
            ui->txt_popust_komb1->setText(pretvori_iz_double(QString::number(popust[2], 'f', 1)));
        }
        else if ( polje == 3 ) {
            popust[3] = max_vsi - sestevek + popust[3];
            ui->txt_popust_komb2->setText(pretvori_iz_double(QString::number(popust[3], 'f', 1)));
        }
        else if ( polje == 4 ) {
            popust[4] = max_vsi - sestevek + popust[4];
            ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(QString::number(popust[4], 'f', 1)));
        }
        else if ( polje == 5 ) {
            popust[5] = max_vsi - sestevek + popust[5];
            ui->txt_popust_kupon->setText(pretvori_iz_double(QString::number(popust[5], 'f', 1)));
        }
        QMessageBox msg_napaka;
        msg_napaka.setText("Vnesli ste previsoko vrednost. Vrednost je avtomatsko "
                                             "popravljena na najvisjo dovoljena vrednost za to polje!");
        msg_napaka.exec();
        return;
    }

    sestevek = 0.0;
    for ( int i = 0; i <= 5; i++ ) {
        sestevek += popust[i];
    }
    ui->txt_popusti_skupaj_2->setText(pretvori_iz_double(QString::number(sestevek, 'f', 1)));

}

void projekti::napolni_podatke() {

    /*	Metoda pregleda vse izdane in prejete racune in potne naloge, ki se
    *		nanasajo na dani projekt. Sestevek zneskov in stevilk vpise v pripravljena
    *		polja. Metodo klicemo na zagonu programa, verjetno pa tudi (ce se bo dalo,
    *		ko se bo napravila kaksna sprememba v bazi podatkov.
    */

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
        // baza je odprta
        QSqlQuery sql_napolni;
        int i = 0; // steje stevilo racunov
        double znesek = 0.0; // steje koncne zneske na racunih
        double znesek_ddv = 0.0;
        double znesek_brez_ddv = 0.0;
        double ure = 0.0; // steje ure dela na projektu

        // preveri prejete racune
        sql_napolni.prepare("SELECT * FROM prejeti_racuni WHERE stevilka_projekta LIKE '" + pretvori(ui->txt_id->text()) + "'");
        sql_napolni.exec();
        while ( sql_napolni.next() ) {
            i++;
            znesek += prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek")).toString()).toDouble();
            znesek_ddv += prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_ddv")).toString()).toDouble();
            znesek_brez_ddv += prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_brez_ddv_00")).toString()).toDouble();
            znesek_brez_ddv += prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_brez_ddv_85")).toString()).toDouble();
            znesek_brez_ddv += prevedi(sql_napolni.value(sql_napolni.record().indexOf("znesek_brez_ddv_20")).toString()).toDouble();
        }

        // vnesi prejete racune
        ui->txt_st_prejetih_racunov->setText(QString::number(i, 10));
        ui->txt_znesek_prejetih_racunov->setText(QString::number(znesek, 'f', 2) + " EUR");
        ui->txt_prejeti_placilo->setText(QString::number(znesek, 'f', 2) + " EUR");
        ui->txt_prejeti_ddv->setText(QString::number(znesek_ddv, 'f', 2) + " EUR");
        ui->txt_prejeti_brez_ddv->setText(QString::number(znesek_brez_ddv, 'f', 2) + " EUR");

        sql_napolni.clear();
        i = 0;
        znesek = 0.0;
        ure = 0.0;
        double potni_stoski = 0.0;

        // preveri potne naloge
        sql_napolni.prepare("SELECT * FROM potni_nalogi WHERE stevilka_projekta LIKE '" + pretvori(ui->txt_id->text()) + "'");
        sql_napolni.exec();
        while ( sql_napolni.next() ) {
            i++;
            potni_stoski += prevedi(sql_napolni.value(sql_napolni.record().indexOf("stroski_skupaj")).toString()).toDouble();
        }

        // vnesi potne naloge
        ui->txt_st_potnih_nalogov->setText(QString::number(i, 10));
        ui->txt_potni_stroski->setText(QString::number(potni_stoski, 'f', 2) + " EUR");

        sql_napolni.clear();
        i = 0;
        znesek = 0.0;
        ure = 0.0;

        // preveri izdane racune
        sql_napolni.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" + pretvori("3") + "'");
        sql_napolni.exec();
        while ( sql_napolni.next() ) {
            i++;
            QSqlQuery sql_racuni;
            sql_racuni.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("id")).toString() + "'");
            sql_racuni.exec();
            while ( sql_racuni.next() ) {
                znesek += prevedi(sql_racuni.value(sql_racuni.record().indexOf("znesek_koncni")).toString()).toDouble() +
                                    prevedi(sql_racuni.value(sql_racuni.record().indexOf("znesek_ddv")).toString()).toDouble();
                ure += prevedi(sql_racuni.value(sql_racuni.record().indexOf("ur_dela")).toString()).toDouble();
            }
        }

        // vnesi izdane racune
        ui->txt_st_izdanih_racunov->setText(QString::number(i, 10));
        ui->txt_znesek_izdanih_racunov->setText(QString::number(znesek, 'f', 2) + " EUR");
        ui->txt_st_ur_dela->setText(QString::number(ure, 'f', 0));

        // izracunaj skupen zasluzek na projektu
        znesek = 0.0;
        znesek = ui->txt_znesek_izdanih_racunov->text().left(ui->txt_znesek_izdanih_racunov->text().length() - 4).toDouble();
        znesek -= ui->txt_znesek_prejetih_racunov->text().left(ui->txt_znesek_prejetih_racunov->text().length() - 4).toDouble();
        znesek -= ui->txt_potni_stroski->text().left(ui->txt_potni_stroski->text().length() - 4).toDouble();

        ui->txt_znesek_skupaj->setText(QString::number(znesek, 'f', 2) + " EUR");
    }
    base.close();

}

void projekti::on_cb_popust_fb1_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_popust_fb1->setEnabled(true);

        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
            QString nipopusta = "true";

            QSqlQuery sql_projekti;
            sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
            sql_projekti.exec();
            if ( sql_projekti.next() ) {
                if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb1")).toString()) != "0.0" ) { // projekt ima popust
                    ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb1")).toString())));
                    nipopusta = "false";
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_stranke;
                sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
                sql_stranke.exec();
                if ( sql_stranke.next() ) {
                    if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_1")).toString()) != "0.0" ) { // stranka ima popust
                        ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_1")).toString())));
                        nipopusta = "false";
                    }
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_osnova;
                sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_facebook_stranka") + "'");
                sql_osnova.exec();
                if ( sql_osnova.next() ) {
                    ui->txt_popust_fb1->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
                }
            }

        }
        base.close();
    }
    else {
        ui->txt_popust_fb1->setText(pretvori_iz_double("0.0"));
        ui->txt_popust_fb1->setEnabled(false);
    }

}

void projekti::on_cb_popust_fb2_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_popust_fb2->setEnabled(true);

        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
            QString nipopusta = "true";

            QSqlQuery sql_projekti;
            sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
            sql_projekti.exec();
            if ( sql_projekti.next() ) {
                if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb2")).toString()) != "0.0" ) { // projekt ima popust
                    ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_fb2")).toString())));
                    nipopusta = "false";
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_stranke;
                sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
                sql_stranke.exec();
                if ( sql_stranke.next() ) {
                    if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_2")).toString()) != "0.0" ) { // stranka ima popust
                        ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_facebook_2")).toString())));
                        nipopusta = "false";
                    }
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_osnova;
                sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_facebook_prijatelj") + "'");
                sql_osnova.exec();
                if ( sql_osnova.next() ) {
                    ui->txt_popust_fb2->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
                }
            }

        }
        base.close();
    }
    else {
        ui->txt_popust_fb2->setText(pretvori_iz_double("0.0"));
        ui->txt_popust_fb2->setEnabled(false);
    }

}

void projekti::on_cb_popust_komb1_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_popust_komb1->setEnabled(true);

        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
            QString nipopusta = "true";

            QSqlQuery sql_projekti;
            sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
            sql_projekti.exec();
            if ( sql_projekti.next() ) {
                if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb1")).toString()) != "0.0" ) { // projekt ima popust
                    ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb1")).toString())));
                    nipopusta = "false";
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_stranke;
                sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
                sql_stranke.exec();
                if ( sql_stranke.next() ) {
                    if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_1")).toString()) != "0.0" ) { // stranka ima popust
                        ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_1")).toString())));
                        nipopusta = "false";
                    }
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_osnova;
                sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kombinacija_1") + "'");
                sql_osnova.exec();
                if ( sql_osnova.next() ) {
                    ui->txt_popust_komb1->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
                }
            }

        }
        base.close();
    }
    else {
        ui->txt_popust_komb1->setText(pretvori_iz_double("0.0"));
        ui->txt_popust_komb1->setEnabled(false);
    }

}

void projekti::on_cb_popust_komb2_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_popust_komb2->setEnabled(true);

        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
            QString nipopusta = "true";

            QSqlQuery sql_projekti;
            sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
            sql_projekti.exec();
            if ( sql_projekti.next() ) {
                if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb2")).toString()) != "0.0" ) { // projekt ima popust
                    ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_komb2")).toString())));
                    nipopusta = "false";
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_stranke;
                sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
                sql_stranke.exec();
                if ( sql_stranke.next() ) {
                    if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_2")).toString()) != "0.0" ) { // stranka ima popust
                        ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kombinacija_2")).toString())));
                        nipopusta = "false";
                    }
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_osnova;
                sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kombinacija_2") + "'");
                sql_osnova.exec();
                if ( sql_osnova.next() ) {
                    ui->txt_popust_komb2->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
                }
            }

        }
        base.close();
    }
    else {
        ui->txt_popust_komb2->setText(pretvori_iz_double("0.0"));
        ui->txt_popust_komb2->setEnabled(false);
    }

}

void projekti::on_cb_popust_stalna_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_popust_stalna_stranka->setEnabled(true);

        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
            QString nipopusta = "true";

            QSqlQuery sql_projekti;
            sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
            sql_projekti.exec();
            if ( sql_projekti.next() ) {
                if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_stranka")).toString()) != "0.0" ) { // projekt ima popust
                    ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_stranka")).toString())));
                    nipopusta = "false";
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_stranke;
                sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
                sql_stranke.exec();
                if ( sql_stranke.next() ) {
                    if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_stranka")).toString()) != "0.0" ) { // stranka ima popust
                        ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_stranka")).toString())));
                        nipopusta = "false";
                    }
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_osnova;
                sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_stalna_stranka") + "'");
                sql_osnova.exec();
                if ( sql_osnova.next() ) {
                    ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
                }
            }

        }
        base.close();
    }
    else {
        ui->txt_popust_stalna_stranka->setText(pretvori_iz_double("0.0"));
        ui->txt_popust_stalna_stranka->setEnabled(false);
    }

}

void projekti::on_cb_popust_kupon_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_popust_kupon->setEnabled(true);

        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
            QString nipopusta = "true";

            QSqlQuery sql_projekti;
            sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
            sql_projekti.exec();
            if ( sql_projekti.next() ) {
                if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_kupon")).toString()) != "0.0" ) { // projekt ima popust
                    ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_kupon")).toString())));
                    nipopusta = "false";
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_stranke;
                sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
                sql_stranke.exec();
                if ( sql_stranke.next() ) {
                    if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kupon")).toString()) != "0.0" ) { // stranka ima popust
                        ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_kupon")).toString())));
                        nipopusta = "false";
                    }
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_osnova;
                sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_kupon") + "'");
                sql_osnova.exec();
                if ( sql_osnova.next() ) {
                    ui->txt_popust_kupon->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
                }
            }

        }
        base.close();
    }
    else {
        ui->txt_popust_kupon->setText(pretvori_iz_double("0.0"));
        ui->txt_popust_kupon->setEnabled(false);
    }

}

void projekti::on_cb_popust_akcija_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_popust_akcija->setEnabled(true);

        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "popusti");
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
            QString nipopusta = "true";

            QSqlQuery sql_projekti;
            sql_projekti.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
            sql_projekti.exec();
            if ( sql_projekti.next() ) {
                if ( prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_akcija")).toString()) != "0.0" ) { // projekt ima popust
                    ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_projekti.value(sql_projekti.record().indexOf("popust_akcija")).toString())));
                    nipopusta = "false";
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_stranke;
                sql_stranke.prepare("SELECT * FROM stranke WHERE id LIKE '" + pretvori(ui->txt_stranka_id->text()) + "'");
                sql_stranke.exec();
                if ( sql_stranke.next() ) {
                    if ( prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString()) != "0.0" ) { // stranka ima popust
                        ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_stranke.value(sql_stranke.record().indexOf("pop_akcija")).toString())));
                        nipopusta = "false";
                    }
                }
            }

            if ( nipopusta == "true" ) {
                QSqlQuery sql_osnova;
                sql_osnova.prepare("SELECT * FROM sif_popusti WHERE popust LIKE '" + pretvori("pop_akcija") + "'");
                sql_osnova.exec();
                if ( sql_osnova.next() ) {
                    ui->txt_popust_akcija->setText(pretvori_iz_double(prevedi(sql_osnova.value(sql_osnova.record().indexOf("vrednost")).toString())));
                }
            }

        }
        base.close();
    }
    else {
        ui->txt_popust_akcija->setText(pretvori_iz_double("0.0"));
        ui->txt_popust_akcija->setEnabled(false);
    }

}

void projekti::on_cb_podrazitev_vikend_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_podrazitev_vikend->setEnabled(true);
    }
    else {
        ui->txt_podrazitev_vikend->setEnabled(false);
    }

}

void projekti::on_cb_podrazitev_hitrost_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_podrazitev_hitrost->setEnabled(true);
    }
    else {
        ui->txt_podrazitev_hitrost->setEnabled(false);
    }

}

void projekti::on_cb_podrazitev_zapleti_toggled(bool stanje) {

    if ( stanje == true ) {
        ui->txt_podrazitev_zapleti->setEnabled(true);
    }
    else {
        ui->txt_podrazitev_zapleti->setEnabled(false);
    }

}

void projekti::on_txt_popust_fb1_textChanged(QString besedilo) {

    izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 0);

}

void projekti::on_txt_popust_fb2_textChanged(QString besedilo) {

    izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 1);

}

void projekti::on_txt_popust_komb1_textChanged(QString besedilo) {

    izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 2);

}

void projekti::on_txt_popust_komb2_textChanged(QString besedilo) {

    izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 3);

}

void projekti::on_txt_popust_stalna_stranka_textChanged(QString besedilo) {

    izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 4);

}

void projekti::on_txt_popust_kupon_textChanged(QString besedilo) {

    izracunaj_popuste(pretvori_v_double(besedilo).toDouble(), 5);

}

void projekti::on_txt_popust_fb1_editingFinished() {

    double popust = pretvori_v_double(ui->txt_popust_fb1->text()).toDouble();

    if ( popust > 100.0 ) {
        popust = 100.0;
    }
    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_popust_fb1->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_popust_fb2_editingFinished() {

    double popust = pretvori_v_double(ui->txt_popust_fb2->text()).toDouble();

    if ( popust > 100.0 ) {
        popust = 100.0;
    }
    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_popust_fb2->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_popust_komb1_editingFinished() {

    double popust = pretvori_v_double(ui->txt_popust_komb1->text()).toDouble();

    if ( popust > 100.0 ) {
        popust = 100.0;
    }
    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_popust_komb1->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_popust_komb2_editingFinished() {

    double popust = pretvori_v_double(ui->txt_popust_komb2->text()).toDouble();

    if ( popust > 100.0 ) {
        popust = 100.0;
    }
    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_popust_komb2->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_popust_stalna_stranka_editingFinished() {

    double popust = pretvori_v_double(ui->txt_popust_stalna_stranka->text()).toDouble();

    if ( popust > 100.0 ) {
        popust = 100.0;
    }
    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_popust_stalna_stranka->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_popust_kupon_editingFinished() {

    double popust = pretvori_v_double(ui->txt_popust_kupon->text()).toDouble();

    if ( popust > 100.0 ) {
        popust = 100.0;
    }
    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_popust_kupon->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_popust_akcija_editingFinished() {

    double popust = pretvori_v_double(ui->txt_popust_akcija->text()).toDouble();

    if ( popust > 100.0 ) {
        popust = 100.0;
    }
    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_popust_akcija->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_podrazitev_vikend_editingFinished() {

    double popust = pretvori_v_double(ui->txt_podrazitev_vikend->text()).toDouble();

    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_podrazitev_vikend->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_podrazitev_hitrost_editingFinished() {

    double popust = pretvori_v_double(ui->txt_podrazitev_hitrost->text()).toDouble();

    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_podrazitev_hitrost->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

void projekti::on_txt_podrazitev_zapleti_editingFinished() {

    double popust = pretvori_v_double(ui->txt_podrazitev_zapleti->text()).toDouble();

    if ( popust < 0.0 ) {
        popust = 0.0;
    }

    ui->txt_podrazitev_zapleti->setText(pretvori_iz_double(QString::number(popust, 'f', 1)));

}

QString projekti::pretvori_v_double(QString besedilo) {

    besedilo.remove(" ");
    besedilo.remove("%");
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

QString projekti::pretvori_iz_double(QString besedilo) {

    besedilo.append(" %");
    besedilo.replace(".",",");

    return besedilo;

}

void projekti::on_btn_vnesi_zapis_clicked() {

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
        QSqlQuery sql_vnesi_zapis;
        if (ui->btn_vnesi_zapis->text() == "Vnesi zapis") {
            sql_vnesi_zapis.prepare("INSERT INTO opombe (stevilka_stranke, stevilka_projekta, datum, naslov, "
                                                                "besedilo) VALUES (?, ?, ?, ?, ?)");
        }
        else {
            sql_vnesi_zapis.prepare("UPDATE opombe SET stevilka_stranke = ?, stevilka_projekta = ?, datum = ?, naslov = ?, "
                                                            "besedilo = ? WHERE id LIKE '" + pretvori(ui->txt_id_zapisa->text()) + "'");
        }
        sql_vnesi_zapis.bindValue(0, pretvori(ui->txt_stranka_id->text()));
        sql_vnesi_zapis.bindValue(1, pretvori(ui->txt_id->text()));
        sql_vnesi_zapis.bindValue(2, pretvori(ui->txt_datum_zapisa->text()));
        sql_vnesi_zapis.bindValue(3, pretvori(ui->txt_naslov_zapisa->text()));
        sql_vnesi_zapis.bindValue(4, pretvori(ui->txt_opis_zapisa->toPlainText()));

        sql_vnesi_zapis.exec();
    }
    base.close();

    ui->txt_id_zapisa->setText("");
    ui->txt_datum_zapisa->setDateTime(QDateTime::currentDateTime());
    ui->txt_naslov_zapisa->setText("");
    ui->txt_opis_zapisa->clear();

    ui->btn_vnesi_zapis->setText("Vnesi zapis");

    napolni_zapise();

}

void projekti::on_tbl_zapisi_doubleClicked() {

    QString id = ui->tbl_zapisi->selectedItems().takeAt(0)->text();

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
        sql_napolni.prepare("SELECT * FROM opombe WHERE id LIKE '" + pretvori(id) + "'");
        sql_napolni.exec();
        if ( sql_napolni.next() ) {
            ui->txt_id_zapisa->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
            QDateTime datum = QDateTime::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
            ui->txt_datum_zapisa->setDateTime(datum);
            ui->txt_naslov_zapisa->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
            ui->txt_opis_zapisa->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("besedilo")).toString()));
        }
    }
    base.close();

    ui->btn_vnesi_zapis->setText("Popravi zapis");

}

void projekti::napolni_zapise() {

    ui->tab_zapisi->setEnabled(true);

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

        // clear previous content
        ui->tbl_zapisi->clear();

        for (int i = 0; i <= 2; i++) {
            ui->tbl_zapisi->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM opombe");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_zapisi->removeRow(0);
        }

        // start filling the table
        ui->tbl_zapisi->insertColumn(0);
        ui->tbl_zapisi->insertColumn(1);
        ui->tbl_zapisi->insertColumn(2);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Datum zapisa");
        naslov2->setText("Naslov zapisa");

        ui->tbl_zapisi->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_zapisi->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_zapisi->setHorizontalHeaderItem(2, naslov2);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM opombe WHERE stevilka_projekta LIKE '" + pretvori(ui->txt_id->text()) + "' ORDER BY id ASC");
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_zapisi->insertRow(row);
            ui->tbl_zapisi->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[8] = {"id", "datum", "naslov"};

            while (col <= 2) {

                QTableWidgetItem *celica = new QTableWidgetItem;
                celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                ui->tbl_zapisi->setItem(row, col, celica);

                col++;
                i++;

            }

            row++;

        }
    }
    base.close();

}

void projekti::on_btn_pocisti_clicked() {

    ui->txt_id_zapisa->setText("");
    ui->txt_datum_zapisa->setDateTime(QDateTime::currentDateTime());
    ui->txt_naslov_zapisa->setText("");
    ui->txt_opis_zapisa->clear();

    ui->btn_vnesi_zapis->setText("Vnesi zapis");

    napolni_zapise();

}

void projekti::napolni_casovnice() {

    ui->tab_casovnice->setEnabled(true);

    ui->txt_cas_racun_1->clear();
    ui->txt_cas_racun_1->addItem("");

    ui->txt_cas_racun_2->clear();
    ui->txt_cas_racun_2->addItem("");

    // preveri, ali obstajajo racuni
    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_casovnice");
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

        QSqlQuery racuni;
        racuni.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + pretvori(ui->txt_id->text()) + "' AND tip_racuna LIKE '" +
                       pretvori("3") + "'");
        racuni.exec();
        // napolni jih v ustrezni spustni seznam (racun_1)
        while ( racuni.next() ) {
            ui->txt_cas_racun_1->addItem(prevedi(racuni.value(racuni.record().indexOf("id")).toString()) + ") " +
                                         prevedi(racuni.value(racuni.record().indexOf("stevilka_racuna")).toString()));

            // preveri, ali obstajajo racuni, ki se trajajo
            if ( racuni.value(racuni.record().indexOf("status_oddaje_racuna")).toString() == "" ) {
                ui->txt_cas_racun_2->addItem(prevedi(racuni.value(racuni.record().indexOf("id")).toString()) + ") " +
                                             prevedi(racuni.value(racuni.record().indexOf("stevilka_racuna")).toString()));
            }
        }
        racuni.clear();
    }
    base.close();

    // ce je samo 1 racun potem nastavi spustni seznam, da ima tega oznacenega
    if ( ui->txt_cas_racun_1->count() == 2 ) {
        ui->txt_cas_racun_1->setCurrentIndex(1);
    }

    if ( ui->txt_cas_racun_2->count() == 2 ) {
        ui->txt_cas_racun_2->setCurrentIndex(1);
    }

    napolni_casovnice_tabelo();

}

void projekti::napolni_casovnice_tabelo() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_tabelo_casovnic");
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

        QString sql_stavek = "SELECT * FROM opravila WHERE stevilka_projekta LIKE '" + pretvori(ui->txt_id->text()) + "'";

        // dodaj pogoj za morebitni racun
        if ( ui->txt_cas_racun_1->currentText() != "" ) { // izbran je tocno dolocen racun
            sql_stavek += " AND stevilka_racuna LIKE '" + pretvori(ui->txt_cas_racun_1->currentText().left(ui->txt_cas_racun_1->currentText().indexOf(") "))) + "'";
        }

        // dodaj pogoj za morebitno opravilo
        if ( ui->txt_cas_opravilo_1->currentText() != "" ) { // izbran je tocno doloceno opravilo
            sql_stavek += " AND id LIKE '" + pretvori(ui->txt_cas_opravilo_1->currentText().left(ui->txt_cas_opravilo_1->currentText().indexOf(") "))) + "'";
        }

        // in samo, ce imajo prisotne casovnice
        sql_stavek += " AND casovnice NOT LIKE ''";

        // clear previous content
        ui->tbl_casovnice->clear();

        for (int i = 0; i <= 6; i++) {
            ui->tbl_casovnice->removeColumn(0);
        }

        QSqlQuery sql_clear;
        sql_clear.prepare("SELECT * FROM opravila");
        sql_clear.exec();
        while (sql_clear.next()) {
            int i = prevedi(sql_clear.value(sql_clear.record().indexOf("casovnice")).toString()).count(";");
            for ( int j = 0; j <= i; j++ ) {
                ui->tbl_casovnice->removeRow(0);
            }
        }

        // start filling the table
        ui->tbl_casovnice->insertColumn(0);
        ui->tbl_casovnice->insertColumn(1);
        ui->tbl_casovnice->insertColumn(2);
        ui->tbl_casovnice->insertColumn(3);
        ui->tbl_casovnice->insertColumn(4);
        ui->tbl_casovnice->insertColumn(5);
        ui->tbl_casovnice->insertColumn(6);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;
        QTableWidgetItem *naslov5 = new QTableWidgetItem;
        QTableWidgetItem *naslov6 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Racun");
        naslov2->setText("Sklop");
        naslov3->setText("Skupina");
        naslov4->setText("Storitev");
        naslov5->setText("Datum");
        naslov6->setText("Opravljenih ur");

        ui->tbl_casovnice->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_casovnice->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_casovnice->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_casovnice->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_casovnice->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_casovnice->setHorizontalHeaderItem(5, naslov5);
        ui->tbl_casovnice->setHorizontalHeaderItem(6, naslov6);

 /*       ui->tbl_casovnice->setColumnWidth(0, 5);
        ui->tbl_casovnice->setColumnWidth(1, 30);
        ui->tbl_casovnice->setColumnWidth(2, 50);
        ui->tbl_casovnice->setColumnWidth(3, 50);
        ui->tbl_casovnice->setColumnWidth(4, 50);
        ui->tbl_casovnice->setColumnWidth(5, 30);
        ui->tbl_casovnice->setColumnWidth(6, 20);
*/
        QSqlQuery sql_fill;
        sql_fill.prepare(sql_stavek);
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {

            // za vsako opravilo, ki vsebuje casovnice, razbij te casovnice in vsako vnesi v svojo vrstico
            QString casovnice = prevedi(sql_fill.value(sql_fill.record().indexOf("casovnice")).toString());
            QString nove_casovnice = casovnice;

            for ( int i = 0; i < casovnice.count(";"); i++ ) {
                QString datum = nove_casovnice.left(nove_casovnice.indexOf(",", 0));
                QString ure = nove_casovnice.left(nove_casovnice.indexOf(";"));
                ure = ure.right(ure.length() - datum.length() - 1);
                nove_casovnice = nove_casovnice.right(nove_casovnice.length() - nove_casovnice.indexOf(";") - 1);

                ui->tbl_casovnice->insertRow(row);
                ui->tbl_casovnice->setRowHeight(row, 20);
                int col = 0;
                int i = 0;
                QString polja[7] = {"id", "racun", "opravilo_sklop", "opravilo_skupina", "opravilo_storitev", "datum", "ure"};

                while (col <= 6) {

                    QTableWidgetItem *celica = new QTableWidgetItem;

                    if ( polja[i] == "opravilo_sklop" || polja[i] == "opravilo_skupina" || polja[i] == "opravilo_storitev") {
                        if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) == "Ostalo" ) {
                            celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf("opravilo_rocno")).toString()));
                        }
                        else {
                            celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                        }
                    }
                    else if ( polja[i] == "racun" ) {
                        QString racun = "";
                        QSqlQuery st_racuna;
                        st_racuna.prepare("SELECT * FROM racuni WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("stevilka_racuna")).toString() + "'");
                        st_racuna.exec();
                        if ( st_racuna.next() ) {
                            racun = prevedi(st_racuna.value(st_racuna.record().indexOf("id")).toString()) + ") " +
                                    prevedi(st_racuna.value(st_racuna.record().indexOf("stevilka_racuna")).toString());
                        }
                        celica->setText(racun);
                    }
                    else if ( polja[i] == "datum" ) {
                        celica->setText(datum);
                    }
                    else if ( polja[i] == "ure" ) {
                        celica->setText(ure);
                    }
                    else {
                        celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    }

                    ui->tbl_casovnice->setItem(row, col, celica);

                    col++;
                    i++;

                }

                row++;

            }

        }
    }

    base.close();

}

void projekti::on_gb_trajanje_od_do_toggled() {

    if ( ui->gb_trajanje_od_do->isChecked() ) {
        ui->gb_trajanje_skupaj->setChecked(false);
    }
    else {
        ui->gb_trajanje_skupaj->setChecked(true);
    }

}

void projekti::on_gb_trajanje_skupaj_toggled() {

    if ( ui->gb_trajanje_skupaj->isChecked() ) {
        ui->gb_trajanje_od_do->setChecked(false);
    }
    else {
        ui->gb_trajanje_od_do->setChecked(true);
    }

}

void projekti::on_txt_cas_racun_1_currentIndexChanged() {

    ui->txt_cas_opravilo_1->clear();

    if ( ui->txt_cas_racun_1->currentText() != "" ) { // naredi, ce ni izbrano prazno polje
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "sprememba_racuna");
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

            ui->txt_cas_opravilo_1->addItem("");
            QString st_racuna = ui->txt_cas_racun_1->currentText().left(ui->txt_cas_racun_1->currentText().indexOf(")", 0));

            QSqlQuery sql_opravila;
            sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(st_racuna) + "'");
            sql_opravila.exec();
            while ( sql_opravila.next() ) {
                QString opravilo = "";
                opravilo = prevedi(sql_opravila.value(sql_opravila.record().indexOf("id")).toString()) + ") ";

                // izpise celoten naziv opravila, glede na hierarhijo in upostevajoc moznost "Ostalo"
                if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_sklop")).toString()) == "Ostalo") { // sklop
                    opravilo += prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_rocno")).toString());
                }
                else {
                    opravilo += prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_sklop")).toString());
                    if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_skupina")).toString()) == "Ostalo") { // skupina
                        opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_rocno")).toString());
                    }
                    else {
                        opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_skupina")).toString());
                        if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_storitev")).toString()) == "Ostalo") { // storitev
                            opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_rocno")).toString());
                        }
                        else {
                            opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_storitev")).toString());
                        }
                    }
                }

                ui->txt_cas_opravilo_1->addItem(opravilo);
            }

        }
        base.close();
    }

    if ( ui->txt_cas_opravilo_1->count() == 2 ) { // v primeru samo enega opravila ga avtomatsko izberi
        ui->txt_cas_opravilo_1->setCurrentIndex(1);
    }

    napolni_casovnice_tabelo();

}

void projekti::on_txt_cas_racun_2_currentIndexChanged() {

    ui->txt_cas_opravilo_2->clear();

    if ( ui->txt_cas_racun_2->currentText() != "" ) { // naredi, ce ni izbrano prazno polje
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
            // baza je odprta

            ui->txt_cas_opravilo_2->addItem("");
            QString st_racuna = ui->txt_cas_racun_2->currentText().left(ui->txt_cas_racun_2->currentText().indexOf(")", 0));

            QSqlQuery sql_opravila;
            sql_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(st_racuna) + "'");
            sql_opravila.exec();
            while ( sql_opravila.next() ) {
                QString opravilo = "";
                opravilo = prevedi(sql_opravila.value(sql_opravila.record().indexOf("id")).toString()) + ") ";

                // izpise celoten naziv opravila, glede na hierarhijo in upostevajoc moznost "Ostalo"
                if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_sklop")).toString()) == "Ostalo") { // sklop
                    opravilo += prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_rocno")).toString());
                }
                else {
                    opravilo += prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_sklop")).toString());
                    if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_skupina")).toString()) == "Ostalo") { // skupina
                        opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_rocno")).toString());
                    }
                    else {
                        opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_skupina")).toString());
                        if ( prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_storitev")).toString()) == "Ostalo") { // storitev
                            opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_rocno")).toString());
                        }
                        else {
                            opravilo += " - " + prevedi(sql_opravila.value(sql_opravila.record().indexOf("opravilo_storitev")).toString());
                        }
                    }
                }

                ui->txt_cas_opravilo_2->addItem(opravilo);
            }

        }
        base.close();
    }

    if ( ui->txt_cas_opravilo_2->count() == 2 ) { // v primeru samo enega opravila ga avtomatsko izberi
        ui->txt_cas_opravilo_2->setCurrentIndex(1);
    }

}

void projekti::on_txt_cas_opravilo_1_currentIndexChanged() {

    napolni_casovnice_tabelo();

}

void projekti::on_txt_cas_opravilo_2_currentIndexChanged() {

    // placeholder for some future functions

}

void projekti::on_btn_pocisti_casovnico_clicked() {

    ui->txt_cas_opravilo_2->clear();
    ui->txt_cas_od->setTime(QTime::fromString("00:00", "HH:mm"));
    ui->txt_cas_do->setTime(QTime::fromString("00:00", "HH:mm"));
    ui->txt_cas_trajanje->setTime(QTime::fromString("00:00", "HH:mm"));
    ui->txt_datum->setDate(QDate::currentDate());

}














/////////////////////////////////////////////////////////
void projekti::on_btn_vnesi_casovnico_clicked() {

    napolni_casovnice_tabelo();

}

/////////////////////////////////////////////////////////














void projekti::on_txt_pricetek_dateChanged() {

    stevilka_racuna();

}

void projekti::stevilka_racuna() {

    if ( ui->btn_sprejmi->text() != "Polnim" ) {
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
            // baza je odprta

            // vnesi stevilko projekta
            QString leto = ui->txt_pricetek->text().right(4);

            bool tvori = true; // tvorimo stevilko naloga
            QString stara_stevilka_naloga = "";
            if ( ui->txt_id->text() != "") {
                QSqlQuery sql_stara_stevilka;
                sql_stara_stevilka.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_id->text()) + "'");
                sql_stara_stevilka.exec();
                if ( sql_stara_stevilka.next() ) {
                    stara_stevilka_naloga = prevedi(sql_stara_stevilka.value(sql_stara_stevilka.record().indexOf("stevilka_projekta")).toString());
                    if ( stara_stevilka_naloga.left(7).right(4) == leto ) {
                        ui->txt_stevilka_projekta->setText(stara_stevilka_naloga);
                        tvori = false;
                    }
                }
            }

            // zapisi stevilko potnega naloga
            if ( tvori == true ) {
                int i = 0;
                QString stevilka = "";

                QSqlQuery sql_insert_stnaloga;
                sql_insert_stnaloga.prepare("SELECT * FROM projekti WHERE stevilka_projekta LIKE '" + pretvori("SP-" + leto) + "%'");
                sql_insert_stnaloga.exec();
                while (sql_insert_stnaloga.next()) {
                    i = pretvori(sql_insert_stnaloga.value(sql_insert_stnaloga.record().indexOf("stevilka_projekta")).toString()).right(3).toInt();
                }

                i++;

                if ( i < 10 ) {
                    stevilka = "00" + QString::number(i, 10);
                }
                else if ( i < 100 ) {
                    stevilka = "0" + QString::number(i, 10);
                }
                else {
                    stevilka = "" + QString::number(i, 10);
                }
                ui->txt_stevilka_projekta->setText("SP-" + leto + "-" + stevilka);
            }

        }
        base.close();
    }

}

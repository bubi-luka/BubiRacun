#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>

#include "uporabnik.h"
#include "ui_uporabnik.h"
#include "prijava.h"
#include "glavnookno.h"
#include "kodiranje.h"

uporabnik::uporabnik(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uporabnik)
{
    ui->setupUi(this);

    // onemogoci polja
    ui->txt_odpustitev->setEnabled(false);
    ui->txt_zaposlen->setChecked(false);

    // brisemo vsebino, ki je morda ostala v kaksnem polju
    on_btn_brisi_clicked();
    ui->txt_posta->clear();
    ui->txt_dovoljenja->clear();
    ui->txt_podjetje->clear();
    ui->txt_naziv->clear();
    ui->txt_pogodba->clear();

    // nastavimo obliko polja
    ui->txt_emso->setInputMask("99 99 999 999 999;_");
    ui->txt_davcna->setInputMask("99999999;_");
    ui->txt_tekoci_racun->setInputMask("SI56 9999 9999 9999 999;_");
    ui->txt_gsm->setInputMask("+990 (\\0)99/999-999;_");
    ui->txt_telefon->setInputMask("+990 (\\0)9/99-99-999;_");

    // napolnimo spustne sezname
    ui->txt_posta->addItem("");
    ui->txt_dovoljenja->addItem("");
    ui->txt_podjetje->addItem("");
    ui->txt_naziv->addItem("");
    ui->txt_pogodba->addItem("");

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
        QSqlQuery sql_fill_combo;
        sql_fill_combo.prepare("SELECT * FROM sif_posta");
        sql_fill_combo.exec();
        while (sql_fill_combo.next()) {
            ui->txt_posta->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("posta")).toString()));
        }
        sql_fill_combo.clear();
         sql_fill_combo;
        sql_fill_combo.prepare("SELECT * FROM sif_dovoljenja");
        sql_fill_combo.exec();
        while (sql_fill_combo.next()) {
            ui->txt_dovoljenja->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("dovoljenje")).toString()));
        }
        sql_fill_combo.clear();
        sql_fill_combo.prepare("SELECT * FROM podjetje");
        sql_fill_combo.exec();
        while (sql_fill_combo.next()) {
            ui->txt_podjetje->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
        }
        sql_fill_combo.clear();
        sql_fill_combo.prepare("SELECT * FROM sif_naziv");
        sql_fill_combo.exec();
        while (sql_fill_combo.next()) {
            ui->txt_naziv->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naziv")).toString()));
        }
        sql_fill_combo.clear();
        sql_fill_combo.prepare("SELECT * FROM sif_pogodbe");
        sql_fill_combo.exec();
        while (sql_fill_combo.next()) {
            ui->txt_pogodba->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("pogodba")).toString()));
        }
    }
    base.close();

}

uporabnik::~uporabnik()
{
    delete ui;
}

void uporabnik::on_btn_izhod_clicked() {

    close();

}

void uporabnik::on_btn_brisi_clicked() {

    ui->txt_ime->setText("");
    ui->txt_priimek->setText("");
    ui->txt_naslov->setText("");
    ui->txt_naslov_st->setText("");
    ui->txt_postna_stevilka->setText("");
    ui->txt_telefon->setText("");
    ui->txt_gsm->setText("");
    ui->txt_email->setText("");
    ui->txt_url->setText("");

    ui->txt_id->setText("");
    ui->txt_uporabnik->setText("");
    ui->txt_geslo->setText("");
    ui->txt_ponovnogeslo->setText("");
    ui->txt_davcna->setText("");
    ui->txt_emso->setText("");
    ui->txt_tekoci_racun->setText("");

    ui->txt_rojstni->setDate(QDate::currentDate());
    ui->txt_zaposlitev->setDate(QDate::currentDate());
    ui->txt_odpustitev->setDate(QDate::currentDate());

    ui->txt_zaposlen->setChecked(false);
    ui->txt_zaposlen->setText("");

    ui->txt_dovoljenja->setCurrentIndex(0);
    ui->txt_podjetje->setCurrentIndex(0);
    ui->txt_naziv->setCurrentIndex(0);
    ui->txt_pogodba->setCurrentIndex(0);

}
// ne preveri polj
void uporabnik::on_btn_sprejmi_clicked() {

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

    // preveri uporabnisko ime, ce ze obstaja
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // preveri le v primeru, da je NOV vnos in ne popravilo OBSTOJECEGA
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (ui->txt_uporabnik->text() == "") {
        ui->label_4->setPalette(palette_error);
        ui->label_4->setFont(font_error);
        napaka = "true";
        ui->txt_uporabnik->setFocus();
    }
    else {
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
            QSqlQuery sql_preveri_uporabnika;
            sql_preveri_uporabnika.prepare("SELECT * FROM uporabniki WHERE user_name LIKE '" + ui->txt_uporabnik->text() + "'");
            sql_preveri_uporabnika.exec();
            if (sql_preveri_uporabnika.next()) {
                if (ui->txt_id->text() != prevedi(sql_preveri_uporabnika.value(sql_preveri_uporabnika.record().indexOf("id")).toString())) {
                    ui->label_4->setPalette(palette_error);
                    ui->label_4->setFont(font_error);
                    napaka = "true";
                    ui->txt_uporabnik->setFocus();
                }
                else {
                    ui->label_4->setPalette(palette_normal);
                    ui->label_4->setFont(font_normal);
                }
            }
            else {
            ui->label_4->setPalette(palette_normal);
            ui->label_4->setFont(font_normal);
            }
        }
        base.close();
    }

    // preveri veljavnost gesla
    if (ui->txt_geslo->text() == "") {
        ui->label_5->setPalette(palette_error);
        ui->label_5->setFont(font_error);
        napaka = "true";
        ui->txt_geslo->setFocus();
    }
    else if (ui->txt_ponovnogeslo->text() == "") {
        ui->label_6->setPalette(palette_error);
        ui->label_6->setFont(font_error);
        napaka = "true";
        ui->txt_ponovnogeslo->setFocus();
    }
    else if (ui->txt_geslo->text() != ui->txt_ponovnogeslo->text()) {
        ui->label_5->setPalette(palette_error);
        ui->label_5->setFont(font_error);
        ui->label_6->setPalette(palette_error);
        ui->label_6->setFont(font_error);
        napaka = "true";
        ui->txt_geslo->setText("");
        ui->txt_ponovnogeslo->setText("");
        ui->txt_geslo->setFocus();
    }
    else {
        ui->label_5->setPalette(palette_normal);
        ui->label_5->setFont(font_normal);
        ui->label_6->setPalette(palette_normal);
        ui->label_6->setFont(font_normal);
    }

    // preveri davcno stevilko
    if (ui->txt_davcna->text().length() < 8 && ui->txt_davcna->text() != "") { // dolzina
        ui->label_14->setPalette(palette_error);
        ui->label_14->setFont(font_error);
        napaka = "true";
        ui->txt_davcna->setFocus();
    }
    else { // pravilnost zapisa
        QString tax = ui->txt_davcna->text();
        QString tax_error = "";
        bool ok;

        int i_sum = 8*tax.left(1).toInt(&ok, 10) +
                    7*tax.left(2).right(1).toInt(&ok, 10) +
                    6*tax.left(3).right(1).toInt(&ok, 10) +
                    5*tax.left(4).right(1).toInt(&ok, 10) +
                    4*tax.left(5).right(1).toInt(&ok, 10) +
                    3*tax.left(6).right(1).toInt(&ok, 10) +
                    2*tax.left(7).right(1).toInt(&ok, 10);
        int i_mod = i_sum % 11;
        int i_control = 11 - i_mod;
        if (i_control == 10) {
            i_control = 0;
        }
        if (i_control == 11) {
            tax_error = "Error";
        }

        if (i_control != tax.right(1).toInt(&ok, 10)) {
            tax_error = "Error";
        }

        if (tax_error == "Error") {
            ui->label_14->setPalette(palette_error);
            ui->label_14->setFont(font_error);
            napaka = "true";
            ui->txt_davcna->setFocus();
        }
        else {
            ui->label_14->setPalette(palette_normal);
            ui->label_14->setFont(font_normal);
        }
    }

    // preveri emso
    if (ui->txt_emso->text().length() < 17 && ui->txt_emso->text() != "") { // dolzina
        ui->label_15->setPalette(palette_error);
        ui->label_15->setFont(font_error);
        napaka = "true";
        ui->txt_emso->setFocus();
    }
    else { //pravilnost zapisa
        QString EMSO = ui->txt_emso->text().remove(" ");
        QString EMSO_error = "";
        bool ok;

        int i_sum = 7*EMSO.left(1).toInt(&ok, 10) +
                    6*EMSO.left(2).right(1).toInt(&ok, 10) +
                    5*EMSO.left(3).right(1).toInt(&ok, 10) +
                    4*EMSO.left(4).right(1).toInt(&ok, 10) +
                    3*EMSO.left(5).right(1).toInt(&ok, 10) +
                    2*EMSO.left(6).right(1).toInt(&ok, 10) +
                    7*EMSO.left(7).right(1).toInt(&ok, 10) +
                    6*EMSO.left(8).right(1).toInt(&ok, 10) +
                    5*EMSO.left(9).right(1).toInt(&ok, 10) +
                    4*EMSO.left(10).right(1).toInt(&ok, 10) +
                    3*EMSO.left(11).right(1).toInt(&ok, 10) +
                    2*EMSO.left(12).right(1).toInt(&ok, 10);
        int i_mod = i_sum % 11;
        if (i_mod == 10) {
            EMSO_error = "Error";
        }
        if (i_mod != 0) {
            i_mod = 11 - i_mod;
        }

        if (i_mod != EMSO.right(1).toInt(&ok, 10)) {
            EMSO_error = "Error";
        }

        if (EMSO_error == "Error") {
            ui->label_15->setPalette(palette_error);
            ui->label_15->setFont(font_error);
            napaka = "true";
            ui->txt_emso->setFocus();
        }
        else {
            ui->label_15->setPalette(palette_normal);
            ui->label_15->setFont(font_normal);
        }
    }

    // preveri trr
    // ne najdem algoritma

    // preveri email naslov
    ui->txt_email->setText(ui->txt_email->text().toLower());
    QRegExp rx_email("^[a-z0-9\\-\\_\\.]*[@][a-z0-9\\-\\_\\.]*[\\.][a-z]{2,3}$");

    if (!rx_email.exactMatch(ui->txt_email->text())) {
        ui->label_10->setPalette(palette_error);
        ui->label_10->setFont(font_error);
        napaka = "true";
        ui->txt_email->setFocus();
    }
    else {
        ui->label_10->setPalette(palette_normal);
        ui->label_10->setFont(font_normal);
    }

    // preveri url
    if (ui->txt_url->text() != "") {
        if (!(ui->txt_url->text().startsWith("http") || ui->txt_url->text().contains("://"))) {
            ui->txt_url->setText("http://" + ui->txt_url->text());
        }
        if (!(ui->txt_url->text().endsWith("/") || ui->txt_url->text().endsWith(".htm") || ui->txt_url->text().endsWith(".html"))) {
            ui->txt_url->setText(ui->txt_url->text().append("/"));
        }
    }

    // ponovno preveri obvezno izpolnjena polja
    if (ui->txt_ime->text() == "") {
        napaka = "true";
        ui->label_2->setPalette(palette_error);
        ui->label_2->setFont(font_error);
    }
    else {
        ui->label_2->setPalette(palette_normal);
        ui->label_2->setFont(font_normal);
    }
    if (ui->txt_priimek->text() == "") {
        napaka = "true";
        ui->label_3->setPalette(palette_error);
        ui->label_3->setFont(font_error);
    }
    else {
       ui->label_3->setPalette(palette_normal);
       ui->label_3->setFont(font_normal);
    }
    if (ui->txt_naziv->currentText() == "") {
        napaka = "true";
        ui->label_13->setPalette(palette_error);
        ui->label_13->setFont(font_error);
    }
    else {
        ui->label_13->setPalette(palette_normal);
        ui->label_13->setFont(font_normal);
    }
    if (ui->txt_pogodba->currentText() == "") {
        napaka = "true";
        ui->label_18->setPalette(palette_error);
        ui->label_18->setFont(font_error);
    }
    else {
        ui->label_18->setPalette(palette_normal);
        ui->label_18->setFont(font_normal);
    }
    if (ui->txt_tekoci_racun->text() == "") {
        napaka = "true";
        ui->label_16->setPalette(palette_error);
        ui->label_16->setFont(font_error);
    }
    else {
        ui->label_16->setPalette(palette_normal);
        ui->label_16->setFont(font_normal);
    }
*/
    // javi napake, ce ni napak vnesi v bazo
    if (napaka == "") {

        // vnesi ali popravi uporabnika
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

            // ker v bazo vnasamo indekse (zmanjsamo podvajanje dela in kolicine podatkov),
            // pridobimo indekse vrednosti spustnih seznamov
            QString dovoljenje;
            QString podjetje;
            QString naziv;
            QString pogodba;
            QString zaposlen;

            QSqlQuery sql_id;
            sql_id.prepare("SELECT * FROM sif_dovoljenja WHERE dovoljenje LIKE '" + pretvori(ui->txt_dovoljenja->currentText()) + "'");
            sql_id.exec();
            if ( sql_id.next() ) {
                dovoljenje = prevedi(sql_id.value(sql_id.record().indexOf("id")).toString());
            }
            sql_id.clear();

            sql_id.prepare("SELECT * FROM podjetje WHERE ime LIKE '" + pretvori(ui->txt_podjetje->currentText()) + "'");
            sql_id.exec();
            if ( sql_id.next() ) {
                podjetje = prevedi(sql_id.value(sql_id.record().indexOf("id")).toString());
            }
            sql_id.clear();

            sql_id.prepare("SELECT * FROM sif_naziv WHERE naziv LIKE '" + pretvori(ui->txt_naziv->currentText()) + "'");
            sql_id.exec();
            if ( sql_id.next() ) {
                naziv = prevedi(sql_id.value(sql_id.record().indexOf("id")).toString());
            }
            sql_id.clear();

            sql_id.prepare("SELECT * FROM sif_pogodbe WHERE pogodba LIKE '" + pretvori(ui->txt_pogodba->currentText()) + "'");
            sql_id.exec();
            if ( sql_id.next() ) {
                pogodba = prevedi(sql_id.value(sql_id.record().indexOf("id")).toString());
            }
            sql_id.clear();

            if ( ui->txt_zaposlen->isChecked() ) {
                zaposlen = "1";
            }
            else {
                zaposlen = "0";
            }

            QSqlQuery sql_vnesi_uporabnika;

            if (ui->btn_sprejmi->text() == "Vnesi zaposlenega") { // vnesi novega uporabnika
            sql_vnesi_uporabnika.prepare("INSERT INTO uporabniki (ime, priimek, user_name, geslo, naslov, naslov_stevilka, posta, postna_stevilka, "
                                         "telefon, gsm, email, rojstni_datum, spletna_stran, naziv, davcna_stevilka, emso, tekoci_racun, "
                                         "zaposlen, datum_zaposlitve, konec_zaposlitve, pogodba, dovoljenje, podjetje) "
                                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            }
            else { // popravi ze obstojeci vnos
                sql_vnesi_uporabnika.prepare("UPDATE uporabniki SET ime = ?, priimek = ?, user_name = ?, geslo = ?, naslov = ?, naslov_stevilka = ?, "
                                                                         "posta = ?, postna_stevilka = ?, telefon = ?, gsm = ?, email = ?, rojstni_datum = ?, spletna_stran = ?, "
                                                                         "naziv = ?, davcna_stevilka = ?, emso = ?, tekoci_racun = ?, zaposlen = ?, datum_zaposlitve = ?, "
                                                                         "konec_zaposlitve = ?, pogodba = ?, "
                                                                         "dovoljenje = ?, podjetje = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
            }
            sql_vnesi_uporabnika.bindValue(0, pretvori(ui->txt_ime->text()));
            sql_vnesi_uporabnika.bindValue(1, pretvori(ui->txt_priimek->text()));
            sql_vnesi_uporabnika.bindValue(2, pretvori(ui->txt_uporabnik->text()));
            sql_vnesi_uporabnika.bindValue(3, pretvori(ui->txt_geslo->text()));
            sql_vnesi_uporabnika.bindValue(4, pretvori(ui->txt_naslov->text()));
            sql_vnesi_uporabnika.bindValue(5, pretvori(ui->txt_naslov_st->text()));
            sql_vnesi_uporabnika.bindValue(6, pretvori(ui->txt_posta->currentText()));
            sql_vnesi_uporabnika.bindValue(7, pretvori(ui->txt_postna_stevilka->text()));
            sql_vnesi_uporabnika.bindValue(8, pretvori(ui->txt_telefon->text().remove(" ")));
            sql_vnesi_uporabnika.bindValue(9, pretvori(ui->txt_gsm->text().remove(" ")));
            sql_vnesi_uporabnika.bindValue(10, pretvori(ui->txt_email->text()));
            sql_vnesi_uporabnika.bindValue(11, pretvori(ui->txt_rojstni->text()));
            sql_vnesi_uporabnika.bindValue(12, pretvori(ui->txt_url->text()));
            sql_vnesi_uporabnika.bindValue(13, pretvori(naziv));
            sql_vnesi_uporabnika.bindValue(14, pretvori(ui->txt_davcna->text().remove(" ")));
            sql_vnesi_uporabnika.bindValue(15, pretvori(ui->txt_emso->text().remove(" ")));
            sql_vnesi_uporabnika.bindValue(16, pretvori(ui->txt_tekoci_racun->text().remove(" ")));
            sql_vnesi_uporabnika.bindValue(17, pretvori(zaposlen));
            sql_vnesi_uporabnika.bindValue(18, pretvori(ui->txt_zaposlitev->text()));
            sql_vnesi_uporabnika.bindValue(19, pretvori(ui->txt_odpustitev->text()));
            sql_vnesi_uporabnika.bindValue(20, pretvori(pogodba));
            sql_vnesi_uporabnika.bindValue(21, pretvori(dovoljenje));
            sql_vnesi_uporabnika.bindValue(22, pretvori(podjetje));
            sql_vnesi_uporabnika.exec();

        }
        base.close();

        // send signal to reload widget
        poslji("uporabnik");
        vApp->set_id(vApp->id());

        // close this window
        close();
    }
    else {
        QMessageBox msgbox;
        msgbox.setText("Dolocena polja niso pravilno izpolnjena");
        msgbox.exec();
    }
}

void uporabnik::keyPressEvent(QKeyEvent *event) {
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

void uporabnik::prejem(QString besedilo) {

    if (besedilo == "Nov zaposleni") {
        ui->btn_sprejmi->setText("Vnesi zaposlenega");
    }
    else {
        ui->btn_sprejmi->setText("Popravi vnos");
        // besedilo nosi ID ze obstojecega uporabnika, potrebno je napolniti polja
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
            QSqlQuery sql_napolni;
            sql_napolni.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + besedilo + "'");
            sql_napolni.exec();
            if (sql_napolni.next()) {
                ui->txt_ime->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("ime")).toString()));
                ui->txt_priimek->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("priimek")).toString()));

                QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("rojstni_datum")).toString()), "dd'.'MM'.'yyyy");
                ui->txt_rojstni->setDate(datum);
                ui->txt_naslov->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
                ui->txt_naslov_st->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov_stevilka")).toString()));
                ui->txt_postna_stevilka->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("postna_stevilka")).toString()));
                ui->txt_telefon->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("telefon")).toString()));
                ui->txt_gsm->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("gsm")).toString()));
                ui->txt_email->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("email")).toString()));
                ui->txt_url->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("spletna_stran")).toString()));

                ui->txt_id->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("id")).toString()));
                ui->txt_uporabnik->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("user_name")).toString()));
                ui->txt_geslo->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("geslo")).toString()));
                ui->txt_ponovnogeslo->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("geslo")).toString()));

                QSqlQuery sql_combo;
                sql_combo.prepare("SELECT * FROM sif_dovoljenja WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("dovoljenje")).toString() + "'");
                sql_combo.exec();
                if (sql_combo.next()) {
                    ui->txt_dovoljenja->setCurrentIndex(ui->txt_dovoljenja->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("dovoljenje")).toString())));
                }
                sql_combo.clear();

                sql_combo.prepare("SELECT * FROM podjetje WHERE id LIKE '" + pretvori(sql_napolni.value(sql_napolni.record().indexOf("podjetje")).toString()) + "'");
                sql_combo.exec();
                if ( sql_combo.next() ) {
                    ui->txt_podjetje->setCurrentIndex(ui->txt_podjetje->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("ime")).toString())));
                }
                sql_combo.clear();

                sql_combo.prepare("SELECT * FROM sif_naziv WHERE id LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString() + "'");
                sql_combo.exec();
                if (sql_combo.next()) {
                    ui->txt_naziv->setCurrentIndex(ui->txt_naziv->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("naziv")).toString())));
                }
                sql_combo.clear();

                datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_zaposlitve")).toString()), "dd'.'MM'.'yyyy");
                ui->txt_zaposlitev->setDate(datum);

                sql_combo.prepare("SELECT * FROM sif_pogodbe WHERE id LIKE '" + pretvori(sql_napolni.value(sql_napolni.record().indexOf("pogodba")).toString()) + "'");
                sql_combo.exec();
                if ( sql_combo.next() ) {
                    ui->txt_pogodba->setCurrentIndex(ui->txt_pogodba->findText(prevedi(sql_combo.value(sql_combo.record().indexOf("pogodba")).toString())));
                }
                sql_combo.clear();

                if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("zaposlen")).toString()) == "1" ) {
                    ui->txt_zaposlen->setChecked(true);
                    ui->txt_odpustitev->setEnabled(false);
                }
                else {
                    ui->txt_zaposlen->setChecked(false);
                    ui->txt_odpustitev->setEnabled(true);

                }

                datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("konec_zaposlitve")).toString()), "dd'.'MM'.'yyyy");
                ui->txt_odpustitev->setDate(datum);
                ui->txt_davcna->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("davcna_stevilka")).toString()));
                ui->txt_emso->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("emso")).toString()));
                ui->txt_tekoci_racun->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("tekoci_racun")).toString()));

                napolni_avtomobile();

            }
        }
        base.close();
    }
}

void uporabnik::on_txt_zaposlen_toggled(bool stanje) {

    if ( stanje == true ) { // oseba je zaposlena
         ui->txt_odpustitev->setEnabled(false);
    }
    else {
        ui->txt_odpustitev->setEnabled(true);
    }

}

void uporabnik::on_btn_geslo_clicked() {

    if ( ui->btn_geslo->text() == "Skrij" ){
        ui->btn_geslo->setText("Prikazi");
        ui->txt_geslo->setEchoMode(QLineEdit::Password);
        ui->txt_ponovnogeslo->setEchoMode(QLineEdit::Password);
    }
    else {
        ui->btn_geslo->setText("Skrij");
        ui->txt_geslo->setEchoMode(QLineEdit::Normal);
        ui->txt_ponovnogeslo->setEchoMode(QLineEdit::Normal);
    }

}

void uporabnik::on_txt_posta_currentIndexChanged(QString besedilo) {

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
        QSqlQuery sql_najdi;
        sql_najdi.prepare("SELECT * FROM sif_posta WHERE posta LIKE '" + pretvori(ui->txt_posta->currentText()) + "'");
        sql_najdi.exec();
        if ( sql_najdi.next() ) {
            ui->txt_postna_stevilka->setText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("postna_stevilka")).toString()));
        }
    }
    base.close();

}

void uporabnik::on_txt_postna_stevilka_textChanged(QString besedilo) {

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
        QSqlQuery sql_najdi;
        sql_najdi.prepare("SELECT * FROM sif_posta WHERE postna_stevilka LIKE '" + pretvori(besedilo) + "'");
        sql_najdi.exec();
        if ( sql_najdi.next() ) {
            ui->txt_posta->setCurrentIndex(ui->txt_posta->findText(prevedi(sql_najdi.value(sql_najdi.record().indexOf("posta")).toString())));
        }
    }
    base.close();

}

void uporabnik::napolni_avtomobile() {

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

        // izprazni tabelo
        QSqlQuery sql_brisi;
        sql_brisi.prepare("SELECT * FROM avtomobili");
        sql_brisi.exec();
        while ( sql_brisi.next() ) {
            ui->tbl_avto->removeRow(0);
        }

        for ( int i = 0; i < 3; i++ ) {
            ui->tbl_avto->removeColumn(0);
        }

        // start filling the table
        ui->tbl_avto->insertColumn(0);
        ui->tbl_avto->insertColumn(1);
        ui->tbl_avto->insertColumn(2);
        ui->tbl_avto->insertColumn(3);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Proizvajalec");
        naslov2->setText("Znamka");
        naslov3->setText("Registrska st.");

        ui->tbl_avto->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_avto->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_avto->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_avto->setHorizontalHeaderItem(3, naslov3);

        ui->tbl_avto->setColumnWidth(0, 35);
        ui->tbl_avto->setColumnWidth(1, 100);
        ui->tbl_avto->setColumnWidth(2, 100);
        ui->tbl_avto->setColumnWidth(3, 100);

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM avtomobili");
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {

            // napreduj le ce je avto last podjetja, kateremu pripada tudi uporabnik ALI ce je avto last uporabniku
            QString napreduj = "";
            if ( prevedi(sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString()) == "0" ) { // podjetje
                napreduj = "true";
            }
            else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString()) == "1" ) { // privat
                if ( prevedi(sql_fill.value(sql_fill.record().indexOf("lastnik")).toString()) == ui->txt_id->text() ) {
                    napreduj = "true";
                }
                else {
                    napreduj = "false";
                }
            }
            else {
                napreduj = "false";
            }

            if ( napreduj == "true" ) {
                ui->tbl_avto->insertRow(row);
                ui->tbl_avto->setRowHeight(row, 20);
                int col = 0;
                int i = 0;
                QString polja[4] = {"id", "proizvajalec", "znamka", "registrska_stevilka"};

                while (col <= 3) {

                    QTableWidgetItem *celica = new QTableWidgetItem;
                    if ( polja[i] == "id" ) {
                        celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                    }
                    else if ( polja[i] == "lastnik" ) {
                        QSqlQuery sql_lastnik;
                        if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "1" ) { // zasebnik
                            sql_lastnik.prepare("SELECT * FROM uporabniki WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("lastnik")).toString() + "'");
                        }
                        else if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "0" ) { // podjetje
                            sql_lastnik.prepare("SELECT * FROM podjetje WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("lastnik")).toString() + "'");
                        }
                        sql_lastnik.exec();
                        if ( sql_lastnik.next() ) {
                            if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "1" ) { // zasebnik
                                celica->setText(prevedi(sql_lastnik.value(sql_lastnik.record().indexOf("ime")).toString()) + " " +
                                                prevedi(sql_lastnik.value(sql_lastnik.record().indexOf("priimek")).toString()));
                            }
                            else if ( sql_fill.value(sql_fill.record().indexOf("lastnistvo")).toString() == "0" ) { // podjetje
                                celica->setText(prevedi(sql_lastnik.value(sql_lastnik.record().indexOf("ime")).toString()));
                            }
                        }
                    }
                    else {
                        celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    }
                    ui->tbl_avto->setItem(row, col, celica);

                    col++;
                    i++;

                }

                row++;

            }
        }
    }
    base.close();

}

QString uporabnik::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString uporabnik::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

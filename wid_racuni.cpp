#include <QtSql>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QDir>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QTextEdit>

#include "wid_racuni.h"
#include "ui_wid_racuni.h"
#include "racun.h"
#include "kodiranje.h"
#include "potninalogi.h"
#include "varnost.h"
#include "tiskanje.h"
#include "datum.h"

wid_racuni::wid_racuni(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_racuni)
{
    ui->setupUi(this);

        // disable and hide
        ui->txt_stprojekta->setEnabled(false);
        ui->txt_stprojekta->setVisible(false);
        ui->btn_brisi->setEnabled(false);
        ui->btn_brisi->setVisible(false);

        // napolni filtrirne spustne sezname
        QString gumb = ui->btn_nov->text();
        ui->btn_nov->setText("");

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

            // filtriraj po tipu racuna
            ui->cb_racun->addItem("");
            ui->cb_racun->addItem("1) Predracun");
            ui->cb_racun->addItem("2) Predplacilo");
            ui->cb_racun->addItem("3) Racun");
            ui->cb_racun->addItem("4) Stornacija");

            // filtriraj po mesecu
            ui->cb_mesec->addItem("");
            ui->cb_mesec->addItem("01) Januar");
            ui->cb_mesec->addItem("02) Februar");
            ui->cb_mesec->addItem("03) Marec");
            ui->cb_mesec->addItem("04) April");
            ui->cb_mesec->addItem("05) Maj");
            ui->cb_mesec->addItem("06) Junij");
            ui->cb_mesec->addItem("07) Julij");
            ui->cb_mesec->addItem("08) Avgust");
            ui->cb_mesec->addItem("09) September");
            ui->cb_mesec->addItem("10) Oktober");
            ui->cb_mesec->addItem("11) November");
            ui->cb_mesec->addItem("12) December");
            ui->cb_mesec->addItem("Ni vnosa");

            // filtriraj po letu
            ui->cb_leto->addItem("");
            ui->cb_leto->addItem("Ni vnosa");

            sql_napolni.prepare("SELECT * FROM racuni WHERE avtor_oseba LIKE '" + pretvori(vApp->id()) + "'");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                QString leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum_izdaje")).toString()).right(4);
                if ( ui->cb_leto->findText(leto) == -1 ) {
                    ui->cb_leto->addItem(leto);
                }
            }
            sql_napolni.clear();

            // razvrscanje let po vrsti
            QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->cb_leto);
            proxy->setSourceModel(ui->cb_leto->model());
            // spustni seznam prepisemo
            ui->cb_leto->model()->setParent(proxy);
            ui->cb_leto->setModel(proxy);
            // razvrsti
            ui->cb_leto->model()->sort(0);

            // privzeto izberi trenutno leto
            ui->cb_leto->setCurrentIndex(ui->cb_leto->findText(QDate::currentDate().toString("yyyy")));

            // filtriraj po statusu placila
            ui->cb_placilo->addItem("");
            sql_napolni.prepare("SELECT * FROM sif_status_placila");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                ui->cb_placilo->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString()));
            }
            sql_napolni.clear();

            // filtriraj po statusu racunovodstva
            ui->cb_racunovodstvo->addItem("");
            sql_napolni.prepare("SELECT * FROM sif_status_racunovodstva");
            sql_napolni.exec();
            while ( sql_napolni.next() ) {
                ui->cb_racunovodstvo->addItem(prevedi(sql_napolni.value(sql_napolni.record().indexOf("status")).toString()));
            }
            sql_napolni.clear();

        }
        base.close();

        napolni();

        ui->btn_nov->setText(gumb);

        if ( vApp->permission() == pretvori("Administrator") ) {
            ui->btn_brisi->setVisible(true);
            ui->btn_brisi->setEnabled(true);
        }
        else {
            ui->btn_brisi->setVisible(false);
            ui->btn_brisi->setEnabled(false);
        }
}

wid_racuni::~wid_racuni()
{
    delete ui;
}

void wid_racuni::on_cb_racun_currentIndexChanged() {

    if ( ui->cb_racun->currentText().left(3) == "3) " ) {
        ui->btn_storno->setEnabled(true);
    }
    else {
        ui->btn_storno->setEnabled(false);
    }

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_racuni::on_cb_mesec_currentIndexChanged() {

    if (ui->cb_mesec->currentText() == "Ni vnosa" ) {
        ui->cb_leto->setCurrentText("Ni vnosa");
    }
    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_racuni::on_cb_leto_currentIndexChanged() {

    if (ui->cb_leto->currentText() == "Ni vnosa" ) {
        ui->cb_mesec->setCurrentText("Ni vnosa");
    }
    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_racuni::on_cb_placilo_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_racuni::on_cb_racunovodstvo_currentIndexChanged() {

    if ( ui->btn_nov->text() != "" ) {
        napolni();
    }

}

void wid_racuni::napolni() {

    ui->btn_print->setText("Polnim");

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_racuni->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_racuni->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_racuni->horizontalHeader()->sortIndicatorSection();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "wid_racuni");
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
        ui->tbl_racuni->clear();

        for (int i = 0; i <= 10; i++) {
            ui->tbl_racuni->removeColumn(0);
        }

        QSqlQuery sql_clear("wid_racuni");
        sql_clear.prepare("SELECT * FROM racuni");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_racuni->removeRow(0);
        }

        // start filling the table
        ui->tbl_racuni->insertColumn(0);
        ui->tbl_racuni->insertColumn(1);
        ui->tbl_racuni->insertColumn(2);
        ui->tbl_racuni->insertColumn(3);
        ui->tbl_racuni->insertColumn(4);
        ui->tbl_racuni->insertColumn(5);
        ui->tbl_racuni->insertColumn(6);
        ui->tbl_racuni->insertColumn(7);
        ui->tbl_racuni->insertColumn(8);
        ui->tbl_racuni->insertColumn(9);
        ui->tbl_racuni->insertColumn(10);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;
        QTableWidgetItem *naslov5 = new QTableWidgetItem;
        QTableWidgetItem *naslov6 = new QTableWidgetItem;
        QTableWidgetItem *naslov7 = new QTableWidgetItem;
        QTableWidgetItem *naslov8 = new QTableWidgetItem;
        QTableWidgetItem *naslov9 = new QTableWidgetItem;
        QTableWidgetItem *naslov10 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Tip racuna");
        naslov2->setText("St. racuna");
        naslov3->setText("Datum izdaje");
        naslov4->setText("Stranka");
        naslov5->setText("Projekt");
        naslov6->setText("Znesek za placilo");
        naslov7->setText("Se placati");
        naslov8->setText("Status placila");
        naslov9->setText("Status racunovodstva");
        naslov10->setText("Stornacija");

        ui->tbl_racuni->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_racuni->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_racuni->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_racuni->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_racuni->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_racuni->setHorizontalHeaderItem(5, naslov5);
        ui->tbl_racuni->setHorizontalHeaderItem(6, naslov6);
        ui->tbl_racuni->setHorizontalHeaderItem(7, naslov7);
        ui->tbl_racuni->setHorizontalHeaderItem(8, naslov8);
        ui->tbl_racuni->setHorizontalHeaderItem(9, naslov9);
        ui->tbl_racuni->setHorizontalHeaderItem(10, naslov10);

        ui->tbl_racuni->setColumnWidth(0, 35);

        datum *delegate = new datum(this);
        ui->tbl_racuni->setItemDelegateForColumn(3, delegate);

        QString projekt = "";

        QSqlQuery sql_projekt;
        sql_projekt.prepare("SELECT * FROM projekti WHERE id LIKE '" + pretvori(ui->txt_stprojekta->text()) + "'");
        sql_projekt.exec();
        if ( sql_projekt.next() ) {
            projekt = sql_projekt.value(sql_projekt.record().indexOf("id")).toString();
        }

        QString stavek = "";

        if ( ui->cb_racun->currentText() != "" ) {
            stavek += " AND tip_racuna LIKE '" + pretvori(ui->cb_racun->currentText()).left(1) + "'";
        }
        if ( ui->cb_placilo->currentText() != "" ) {
            stavek += " AND status_placila LIKE '" + pretvori (ui->cb_placilo->currentText()) + "'";
        }
        if ( ui->cb_racunovodstvo->currentText() != "" ) {
            stavek += " AND status_racunovodstva LIKE '" + pretvori (ui->cb_racunovodstvo->currentText()) + "'";
        }

        if ( stavek != "" && ui->txt_stprojekta->text() == "*" && stavek.indexOf(" WHERE") == -1 ) {
            stavek = " WHERE" + stavek.right(stavek.length() - 4);
        }

        QSqlQuery sql_fill("wid_racuni");
        if ( ui->txt_stprojekta->text() != "*" ) {
            sql_fill.prepare("SELECT * FROM racuni WHERE projekt LIKE '" + projekt + "'" + stavek);
        }
        else {
            sql_fill.prepare("SELECT * FROM racuni" + stavek);
        }
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            // filtriramo glede na mesec in leto
            QString filter = "pozitivno";
            if ( ui->cb_mesec->currentText() == "Ni vnosa" && ui->cb_leto->currentText() == "Ni vnosa" ) {
                if ( sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString() != "" ) {
                    filter = "negativno";
                }
            }
            else if ( ui->cb_mesec->currentText() != "" && ui->cb_leto->currentText() != "" ) {
                QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
                QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
                if ( mesec != ui->cb_mesec->currentText().left(2) || leto != ui->cb_leto->currentText() ) {
                    filter = "negativno";
                }
            }
            else if ( ui->cb_mesec->currentText() != "" ) {
                QString mesec = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).left(5).right(2);
                if ( mesec != ui->cb_mesec->currentText().left(2) ) {
                    filter = "negativno";
                }
            }
            else if ( ui->cb_leto->currentText() != "" ) {
                QString leto = prevedi(sql_fill.value(sql_fill.record().indexOf("datum_izdaje")).toString()).right(4);
                if ( leto != ui->cb_leto->currentText() ) {
                    filter = "negativno";
                }
            }
            // filtriramo glede na javni, zasebni status racuna
            if ( vApp->state() != pretvori("private") ) {

                // doloci vse tri cifre
                QString sklic = prevedi(sql_fill.value(sql_fill.record().indexOf("sklic")).toString());
                sklic = sklic.right(sklic.length() - 5); // odbijemo drzavo in model
                sklic = sklic.right(sklic.length() - 5); // odbijemo stevilko racuna
                int cifra_1 = sklic.left(1).toInt();
                sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_1 in dan
                int cifra_2 = sklic.left(1).toInt();
                sklic = sklic.right(sklic.length() - 3); // odbijemo cifro_2 in mesec
                int cifra_3 = sklic.left(1).toInt();

                // iz prvih dveh izracunaj kontrolno stevilko
                int kontrolna = 0;

                int sestevek = 3 * cifra_1 + 2 * cifra_2;

                int ostanek = sestevek % 11;

                kontrolna = 11 - ostanek;

                if ( kontrolna >= 9 ) {
                    kontrolna = 0;
                }

                // od cifre_3 odstej kontrolno stevilko
                // tako dobis 0 => racun je javen ali 1 => racun je zaseben
                int razlika = cifra_3 - kontrolna;

                if ( razlika == 1 ) {
                    filter = "negativno"; // racuna ne prikazi
                }

            }

            if ( filter == "pozitivno" ) {
                ui->tbl_racuni->insertRow(row);
                ui->tbl_racuni->setRowHeight(row, 20);
                int col = 0;
                int i = 0;
                QString polja[11] = {"id", "tip_racuna", "stevilka_racuna", "datum_izdaje", "stranka", "projekt", "znesek_za_placilo",
                                     "se_placati", "status_placila", "status_racunovodstva", "stornacija"};

                while (col <= 10) {
                    QTableWidgetItem *celica = new QTableWidgetItem;
                    if ( polja[i] == "id" ) {
                        celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                    }
                    if ( polja[i] == "stornacija" ) {
                        if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) == "1" ) { // racun je storniran
                            celica->setText("DA");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) == "0" ) { // racun ni storniran
                            celica->setText("NE");
                        }
                        else { // ne gre za racun ampak za predracun, predplacilni racun ali stornacijo
                            celica->setText("");
                        }
                    }
                    else if ( polja[i] == "datum_izdaje" ) {
                        celica->setData(Qt::DisplayRole, QDate::fromString(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()), "dd'.'MM'.'yyyy"));
                    }
                    else if ( polja[i] == "tip_racuna" ) {
                        if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "1" ) {
                            celica->setText("Predracun");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "2" ) {
                            celica->setText("Predplacilo");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "3" ) {
                            celica->setText("Racun");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "4" ) {
                            celica->setText("Stornacija");
                        }
                    }
                    else if ( polja[i] == "stranka" ) {
                        QSqlQuery sql_find_stranka;
                        sql_find_stranka.prepare("SELECT * FROM stranke WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf(polja[i])).toString() + "'");
                        sql_find_stranka.exec();
                        if ( sql_find_stranka.next() ) {
                            if ( prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("tip")).toString()) == "1" ) {
                                celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("priimek")).toString()) + " " +
                                                prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
                            }
                            else {
                                 celica->setText(prevedi(sql_find_stranka.value(sql_find_stranka.record().indexOf("ime")).toString()));
                            }
                        }
                        else {
                            celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                        }
                    }
                    else if ( polja[i] == "projekt" ) {
                        QSqlQuery sql_kodiraj;
                        sql_kodiraj.prepare("SELECT * FROM projekti WHERE id LIKE '" + sql_fill.value(sql_fill.record().indexOf("projekt")).toString() + "'");
                        sql_kodiraj.exec();
                        if ( sql_kodiraj.next() ) {
                            celica->setText(prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("naslov_projekta")).toString()));
                        }
                    }
                    else if ( polja[i] == "znesek_za_placilo" ) {
                        QSqlQuery sql_kodiraj;
                        sql_kodiraj.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() +
                                                                "' AND tip_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString() + "'");
                        sql_kodiraj.exec();
                        double znesek = 0.0;
                        while ( sql_kodiraj.next() ) {
                            znesek += prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("znesek_koncni")).toString()).toDouble() *
                                    ( prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("ddv")).toString()).toDouble() + 100.0 ) / 100.0;
                        }
                        celica->setText(QString::number(znesek, 'f', 2).replace(".", ",") + " EUR");
                    }
                    else if ( polja[i] == "se_placati" ) {
                        QSqlQuery sql_kodiraj;
                        sql_kodiraj.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("id")).toString() +
                                                                "' AND tip_racuna LIKE '" + sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString() + "'");
                        sql_kodiraj.exec();
                        double znesek = 0.0;
                        while ( sql_kodiraj.next() ) {
                            znesek += prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("znesek_koncni")).toString()).toDouble() *
                                      ( prevedi(sql_kodiraj.value(sql_kodiraj.record().indexOf("ddv")).toString()).toDouble() + 100.0 ) / 100.0;
                        }
                        znesek -= prevedi(sql_fill.value(sql_fill.record().indexOf("avans")).toString()).toDouble();
                        celica->setText(QString::number(znesek, 'f', 2).replace(".", ",") + " EUR");
                    }
                    else {
                        celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    }
                    ui->tbl_racuni->setItem(row, col, celica);
                    col++;
                    i++;
                }
                row++;
            }
        }
    }
    base.close();

    ui->tbl_racuni->selectRow(izbranec);
    ui->tbl_racuni->sortByColumn(razvrsti, Qt::AscendingOrder);

    ui->btn_print->setText("Natisni");

}

void wid_racuni::napolni_sorodnike() {

    int izbranec = 0;
    int razvrsti = 0;

    if ( ui->tbl_sorodniki->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_sorodniki->selectedItems().takeAt(0)->row();
    }

    razvrsti = ui->tbl_sorodniki->horizontalHeader()->sortIndicatorSection();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "wid_sorodniki");
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
        ui->tbl_sorodniki->clear();

        for (int i = 0; i <= 3; i++) {
            ui->tbl_sorodniki->removeColumn(0);
        }

        QSqlQuery sql_clear("wid_sorodniki");
        sql_clear.prepare("SELECT * FROM racuni");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_sorodniki->removeRow(0);
        }

        // start filling the table
        ui->tbl_sorodniki->insertColumn(0);
        ui->tbl_sorodniki->insertColumn(1);
        ui->tbl_sorodniki->insertColumn(2);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Tip racuna");
        naslov2->setText("St. racuna");

        ui->tbl_sorodniki->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_sorodniki->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_sorodniki->setHorizontalHeaderItem(2, naslov2);

        ui->tbl_sorodniki->setColumnWidth(0, 35);
        ui->tbl_sorodniki->setColumnWidth(1, 70);
        ui->tbl_sorodniki->setColumnWidth(2, 70);

        // dolocimo ID, po katerem iscemo sorodnike ter ID, ki je trenutno izbran
        QString id_starsa = "";
        QString id_izbran = "";

        id_izbran = ui->tbl_racuni->selectedItems().takeAt(0)->text();

        if ( ui->tbl_racuni->selectedItems().takeAt(1)->text().left(6) == "Predra" ) { // predracun, id_izbran == id_starsa
            id_starsa = id_izbran;
        }
        else { // predplacilni racun ali racun
            QSqlQuery sql_stars;
            sql_stars.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(id_izbran) + "'");
            sql_stars.exec();
            if ( sql_stars.next() ) {
                id_starsa = prevedi(sql_stars.value(sql_stars.record().indexOf("stevilka_starsa")).toString());
            }
        }

        QString sql_stavek = "SELECT * FROM racuni WHERE";
        sql_stavek.append(" stevilka_starsa LIKE '" + pretvori(id_starsa) + "'");
        sql_stavek.append(" OR id LIKE '" + pretvori(id_starsa) + "'");

        QSqlQuery sql_fill("wid_sorodniki");
        sql_fill.prepare(sql_stavek);
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            // izlocimo trenutno izbran racun
            if ( id_izbran != prevedi(sql_fill.value(sql_fill.record().indexOf("id")).toString()) ) {
                ui->tbl_sorodniki->insertRow(row);
                ui->tbl_sorodniki->setRowHeight(row, 20);
                int col = 0;
                int i = 0;
                QString polja[3] = {"id", "tip_racuna", "stevilka_racuna"};

                while (col <= 2) {
                    QTableWidgetItem *celica = new QTableWidgetItem;
                    if ( polja[i] == "id" ) {
                        celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                    }
                    else if ( polja[i] == "tip_racuna" ) {
                        if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "1" ) {
                            celica->setText("Predracun");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "2" ) {
                            celica->setText("Predplacilo");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "3" ) {
                            celica->setText("Racun");
                        }
                        else if ( prevedi(sql_fill.value(sql_fill.record().indexOf("tip_racuna")).toString()) == "4" ) {
                            celica->setText("Stornacija");
                        }
                    }
                    else {
                        celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    }
                    ui->tbl_sorodniki->setItem(row, col, celica);
                    col++;
                    i++;
                }
                row++;
            }
        }
    }
    base.close();

    ui->tbl_sorodniki->selectRow(izbranec);
    ui->tbl_sorodniki->sortByColumn(razvrsti, Qt::AscendingOrder);

}

void wid_racuni::on_tbl_racuni_doubleClicked() {

    racun *uredi = new racun;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_racuni->selectedItems().takeAt(0)->text()); // ce racun ze obstaja, naprej posljemo id. racuna
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_racuni::on_tbl_racuni_itemSelectionChanged() {

    if ( ui->btn_print->text() != "Polnim" ) {
        if ( ui->tbl_racuni->selectedItems().count() == 11 ) { // pri enem oznacenem polju omogoci gumbe, pri vecih pa ne
            napolni_sorodnike();

            // stornacije so mozne samo na racunih, ne pa na predracunih, stornacijah in predplacilnih racunih
            if ( ui->tbl_racuni->selectedItems().takeAt(10)->text() != "NE" ) {
                ui->btn_storno->setEnabled(false);
            }
            else {
                ui->btn_storno->setEnabled(true);
            }
        }
        else {
            ui->tbl_sorodniki->clear();
            ui->btn_storno->setEnabled(false);
        }
    }

}

void wid_racuni::on_tbl_sorodniki_doubleClicked() {

    racun *uredi = new racun;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos(ui->tbl_sorodniki->selectedItems().takeAt(0)->text()); // ce racun ze obstaja, naprej posljemo id. racuna
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_racuni::on_btn_brisi_clicked() {

    QString id = ui->tbl_racuni->selectedItems().takeAt(0)->text();

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
        QSqlQuery sql_brisi;

        // izbrisi opravila
        sql_brisi.prepare("DELETE FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id) + "'");
        sql_brisi.exec();
        sql_brisi.clear();

        // izbrisi zapise
        sql_brisi.prepare("DELETE FROM opombe WHERE stevilka_racuna LIKE '" + pretvori(id) + "'");
        sql_brisi.exec();
        sql_brisi.clear();

        // izbrisi racun
        sql_brisi.prepare("DELETE FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
        sql_brisi.exec();
    }
    base.close();

    ui->tbl_racuni->removeRow(ui->tbl_racuni->selectedItems().takeAt(0)->row());
    osvezi("racuni");

}

void wid_racuni::on_btn_kopiraj_clicked() {

    QClipboard *odlozisce = QApplication::clipboard();

    QModelIndexList selectedList = ui->tbl_racuni->selectionModel()->selectedRows();

    QString html_besedilo = "<table>";
    html_besedilo += "<tr>";
    html_besedilo += "<th>ID</th>";
    html_besedilo += "<th>Tip racuna</th>";
    html_besedilo += "<th>Stevilka racuna</th>";
    html_besedilo += "<th>Datum izdaje</th>";
    html_besedilo += "<th>Stranka</th>";
    html_besedilo += "<th>Projekt</th>";
    html_besedilo += "<th>Znesek za placilo</th>";
    html_besedilo += "<th>Se placati</th>";
    html_besedilo += "<th>Status placila</th>";
    html_besedilo += "<th>Status racunovodstva</th>";
    html_besedilo += "</tr>";

    for( int i = 0; i < selectedList.count(); i++) {
        html_besedilo += "<tr>";
        for ( int a = 0; a < 10; a++ ) {
            html_besedilo += "<td>";
            html_besedilo += ui->tbl_racuni->item(selectedList.at(i).row(), a)->text();
            html_besedilo += "</td>";

        }
        html_besedilo += "</tr>";
    }

    html_besedilo += "</table>";

    QTextEdit *textedit = new QTextEdit;

    textedit->setHtml(html_besedilo);
    html_besedilo = textedit->toHtml();

    odlozisce->clear();

    QMimeData *mimeData = new QMimeData();
    mimeData->setData("text/html", html_besedilo.toUtf8());
    odlozisce->setMimeData(mimeData, QClipboard::Clipboard);


}

void wid_racuni::on_btn_storno_clicked() {

    if ( ui->tbl_racuni->selectedItems().takeAt(10)->text() == "DA" ) {
        ui->btn_storno->setEnabled(false);
    }
    else {
        qDebug("Storno je bil pritisnjen");
        QString id = ui->tbl_racuni->selectedItems().takeAt(0)->text();

        qDebug("Oznacen je racun st.: " + id.toUtf8());
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

        // oznaci trenutni racun kot storniran
            qDebug("Oznacujem racun kot storniran");
            QSqlQuery oznaci;
            oznaci.prepare("UPDATE racuni SET stornacija = '1' WHERE id LIKE '" + pretvori(id) + "'");
            oznaci.exec();
            qDebug("Racun je storniran");

        // prepisi trenutni racun v bazo storniranih, zneski so negativni

            // na novo izracunamo stevilko racuna
            qDebug("Iscem novo stevilko racuna");
            QString st_racuna = stevilka_racuna();
            qDebug("Nova stevilka racuna je " + st_racuna.toUtf8());

            // doloci spremenljivke, ki jih ni v sami formi programa
            qDebug("Prepisujem podatke iz racuna v stornacijo racuna");

            QSqlQuery izberi;
            izberi.prepare("SELECT * FROM racuni WHERE id LIKE '" + pretvori(id) + "'");
            izberi.exec();
            if ( izberi.next() ) {
                qDebug("Racun, ki ga zelimo stornirati, obstaja!");
                QSqlQuery prepisi;
                prepisi.prepare("INSERT INTO racuni (stevilka_racuna, tip_racuna, status_racuna, stranka, projekt, avtor_oseba, datum_pricetka, "
                                "datum_konca, datum_izdaje, datum_placila, status_placila, status_racunovodstva, avans, odstotek_avansa, "
                                "status_oddaje_racuna, datum_oddaje_racuna, stara_stevilka_racuna, sklic, datum_placila_avansa, opombe, "
                                "rok_placila, podjetje_id, podjetje_kratki, podjetje_polni, podjetje_naslov_ulica, podjetje_naslov_stevilka, "
                                "podjetje_naslov_posta, podjetje_naslov_postna_stevilka, podjetje_url, podjetje_email, podjetje_telefon, podjetje_ddv, "
                                "podjetje_bic, podjetje_banka, podjetje_tekoci_racun, podjetje_koda_namena, podjetje_logotip, izdajatelj_id, "
                                "izdajatelj_ime, izdajatelj_priimek, izdajatelj_naziv, narocnik_id, narocnik_naziv, narocnik_naslov, narocnik_posta, "
                                "narocnik_davcna, stevilka_starsa"
                                ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
                                "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                prepisi.bindValue(0, pretvori(st_racuna));
                prepisi.bindValue(1, pretvori("4"));
                prepisi.bindValue(2, izberi.value(izberi.record().indexOf("status_racuna")).toString());
                prepisi.bindValue(3, izberi.value(izberi.record().indexOf("stranka")).toString());
                prepisi.bindValue(4, izberi.value(izberi.record().indexOf("projekt")).toString());
                prepisi.bindValue(5, izberi.value(izberi.record().indexOf("avtor_oseba")).toString());
                prepisi.bindValue(6, izberi.value(izberi.record().indexOf("datum_pricetka")).toString());
                prepisi.bindValue(7, izberi.value(izberi.record().indexOf("datum_konca")).toString());
                prepisi.bindValue(8, izberi.value(izberi.record().indexOf("datum_izdaje")).toString());
                prepisi.bindValue(9, izberi.value(izberi.record().indexOf("datum_placila")).toString());
                prepisi.bindValue(10, izberi.value(izberi.record().indexOf("status_placila")).toString());
                prepisi.bindValue(11, izberi.value(izberi.record().indexOf("status_racunovodstva")).toString());
                prepisi.bindValue(12, izberi.value(izberi.record().indexOf("avans")).toString());
                prepisi.bindValue(13, izberi.value(izberi.record().indexOf("odstotek_avansa")).toString());
                prepisi.bindValue(14, izberi.value(izberi.record().indexOf("status_oddaje_racuna")).toString());
                prepisi.bindValue(15, izberi.value(izberi.record().indexOf("datum_oddaje_racuna")).toString());
                prepisi.bindValue(16, izberi.value(izberi.record().indexOf("stara_stevilka_racuna")).toString());
                prepisi.bindValue(17, izberi.value(izberi.record().indexOf("sklic")).toString());
                prepisi.bindValue(18, izberi.value(izberi.record().indexOf("datum_placila_avansa")).toString());
                prepisi.bindValue(19, izberi.value(izberi.record().indexOf("opombe")).toString());
                prepisi.bindValue(20, izberi.value(izberi.record().indexOf("rok_placila")).toString());
                prepisi.bindValue(21, izberi.value(izberi.record().indexOf("podjetje_id")).toString());
                prepisi.bindValue(22, izberi.value(izberi.record().indexOf("podjetje_kratki")).toString());
                prepisi.bindValue(23, izberi.value(izberi.record().indexOf("podjetje_polni")).toString());
                prepisi.bindValue(24, izberi.value(izberi.record().indexOf("podjetje_naslov_ulica")).toString());
                prepisi.bindValue(25, izberi.value(izberi.record().indexOf("podjetje_naslov_stevilka")).toString());
                prepisi.bindValue(26, izberi.value(izberi.record().indexOf("podjetje_naslov_posta")).toString());
                prepisi.bindValue(27, izberi.value(izberi.record().indexOf("podjetje_naslov_postna_stevilka")).toString());
                prepisi.bindValue(28, izberi.value(izberi.record().indexOf("podjetje_url")).toString());
                prepisi.bindValue(29, izberi.value(izberi.record().indexOf("podjetje_email")).toString());
                prepisi.bindValue(30, izberi.value(izberi.record().indexOf("podjetje_telefon")).toString());
                prepisi.bindValue(31, izberi.value(izberi.record().indexOf("podjetje_ddv")).toString());
                prepisi.bindValue(32, izberi.value(izberi.record().indexOf("podjetje_bic")).toString());
                prepisi.bindValue(33, izberi.value(izberi.record().indexOf("podjetje_banka")).toString());
                prepisi.bindValue(34, izberi.value(izberi.record().indexOf("podjetje_tekoci_racun")).toString());
                prepisi.bindValue(35, izberi.value(izberi.record().indexOf("podjetje_koda_namena")).toString());
                prepisi.bindValue(36, izberi.value(izberi.record().indexOf("podjetje_logotip")).toString());
                prepisi.bindValue(37, izberi.value(izberi.record().indexOf("izdajatelj_id")).toString());
                prepisi.bindValue(38, izberi.value(izberi.record().indexOf("izdajatelj_ime")).toString());
                prepisi.bindValue(39, izberi.value(izberi.record().indexOf("izdajatelj_priimek")).toString());
                prepisi.bindValue(40, izberi.value(izberi.record().indexOf("izdajatelj_naziv")).toString());
                prepisi.bindValue(41, izberi.value(izberi.record().indexOf("narocnik_id")).toString());
                prepisi.bindValue(42, izberi.value(izberi.record().indexOf("narocnik_naziv")).toString());
                prepisi.bindValue(43, izberi.value(izberi.record().indexOf("narocnik_naslov")).toString());
                prepisi.bindValue(44, izberi.value(izberi.record().indexOf("narocnik_posta")).toString());
                prepisi.bindValue(45, izberi.value(izberi.record().indexOf("narocnik_davcna")).toString());
                prepisi.bindValue(46, izberi.value(izberi.record().indexOf("stevilka_starsa")).toString());
                prepisi.exec();
            }
            qDebug("Podatki o racunu prepisani");

        // poiscemo ID pravkar vnesenega zapisa
            qDebug("Iscemo nov id pravkar zapisanega racuna");
            QString nov_id = "";
            QSqlQuery sql_nov_id;
            sql_nov_id.prepare("SELECT * FROM racuni WHERE stevilka_racuna LIKE '" + pretvori(st_racuna) + "' AND tip_racuna LIKE '4'");
            sql_nov_id.exec();
            if ( sql_nov_id.next() ) {
                nov_id = prevedi(sql_nov_id.value(sql_nov_id.record().indexOf("id")).toString());
            }
            qDebug("Na novo vpisan storniran racun ima id: " + nov_id.toUtf8());

        // kopira opravila iz racuna v storno
            QSqlQuery sql_poisci_opravila;
            sql_poisci_opravila.prepare("SELECT * FROM opravila WHERE stevilka_racuna LIKE '" + pretvori(id) + "' AND tip_racuna LIKE '" + pretvori("3") + "'");
            sql_poisci_opravila.exec();
            while ( sql_poisci_opravila.next() ) {
                QSqlQuery sql_kopiraj_opravila;
                sql_kopiraj_opravila.prepare("INSERT INTO opravila (stevilka_stranke, stevilka_projekta, stevilka_racuna, tip_racuna, opravilo_skupina, "
                                                                         "opravilo_storitev, urna_postavka_brez_ddv, urna_postavka_z_ddv, ddv, popust_fb1, popust_fb2, "
                                                                         "popust_komb1, popust_komb2, popust_stranka, popust_kupon, popust_akcija, podrazitev_vikend, "
                                                                         "podrazitev_hitrost, podrazitev_zapleti, pribitek_vikend, pribitek_hitrost, pribitek_zapleti, "
                                                                         "tip_ur, ur_dela, rocni_vnos_ur, znesek_popustov, znesek_ddv, znesek_koncni, enota, opravilo_sklop, "
                                                                         "opravilo_rocno, vrstni_red) "
                                                                         "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                sql_kopiraj_opravila.bindValue(0, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_stranke")).toString());
                sql_kopiraj_opravila.bindValue(1, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("stevilka_projekta")).toString());
                sql_kopiraj_opravila.bindValue(2, pretvori(nov_id));
                sql_kopiraj_opravila.bindValue(3, pretvori("4")); // storno == 4
                sql_kopiraj_opravila.bindValue(4, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_skupina")).toString());
                sql_kopiraj_opravila.bindValue(5, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_storitev")).toString());
                sql_kopiraj_opravila.bindValue(6, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("urna_postavka_brez_ddv")).toString());
                sql_kopiraj_opravila.bindValue(7, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("urna_postavka_z_ddv")).toString());
                sql_kopiraj_opravila.bindValue(8, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ddv")).toString());
                sql_kopiraj_opravila.bindValue(9, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_fb1")).toString());
                sql_kopiraj_opravila.bindValue(10, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_fb2")).toString());
                sql_kopiraj_opravila.bindValue(11, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_komb1")).toString());
                sql_kopiraj_opravila.bindValue(12, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_komb2")).toString());
                sql_kopiraj_opravila.bindValue(13, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_stranka")).toString());
                sql_kopiraj_opravila.bindValue(14, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_kupon")).toString());
                sql_kopiraj_opravila.bindValue(15, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("popust_akcija")).toString());
                sql_kopiraj_opravila.bindValue(16, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_vikend")).toString());
                sql_kopiraj_opravila.bindValue(17, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_hitrost")).toString());
                sql_kopiraj_opravila.bindValue(18, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("podrazitev_zapleti")).toString());
                sql_kopiraj_opravila.bindValue(19, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_vikend")).toString());
                sql_kopiraj_opravila.bindValue(20, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_hitrost")).toString());
                sql_kopiraj_opravila.bindValue(21, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("pribitek_zapleti")).toString());
                sql_kopiraj_opravila.bindValue(22, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("tip_ur")).toString());
                sql_kopiraj_opravila.bindValue(23, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("ur_dela")).toString());
                sql_kopiraj_opravila.bindValue(24, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("rocni_vnos_ur")).toString());
                sql_kopiraj_opravila.bindValue(25, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_popustov")).toString());
                sql_kopiraj_opravila.bindValue(26, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_ddv")).toString());
                sql_kopiraj_opravila.bindValue(27, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("znesek_koncni")).toString());
                sql_kopiraj_opravila.bindValue(28, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("enota")).toString());
                sql_kopiraj_opravila.bindValue(29, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_sklop")).toString());
                sql_kopiraj_opravila.bindValue(30, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("opravilo_rocno")).toString());
                sql_kopiraj_opravila.bindValue(31, sql_poisci_opravila.value(sql_poisci_opravila.record().indexOf("vrstni_red")).toString());
                sql_kopiraj_opravila.exec();
            }
        }

        base.close();

        qDebug("Osvezujem tabelo racunov");
        osvezi("racuni");
        qDebug("Tabela racunov je osvezena");
        napolni_sorodnike();
    }

}

QString wid_racuni::stevilka_racuna() {

    QString st_racuna = "";
    qDebug("Trenutna stevilka racuna je " + st_racuna.toUtf8());

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "stevilka_racuna");
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
        QString leto = QDate::currentDate().toString("yyyy");
        QString mesec = QDate::currentDate().toString("MM");
        QString dan = QDate::currentDate().toString("dd");

        qDebug("Danes smo:");
        qDebug("dan: " + dan.toUtf8());
        qDebug("mesec: " + mesec.toUtf8());
        qDebug("leto: " + leto.toUtf8());

        int max_st_racuna = 0;

        // izracunamo zaporedno stevilko racuna v tekocem letu
        QSqlQuery sql_stetje_racunov;
        sql_stetje_racunov.prepare("SELECT * FROM racuni WHERE datum_izdaje LIKE '%." + pretvori(leto) +
                                   "' AND ( tip_racuna LIKE '" + pretvori("3") +
                                   "' OR tip_racuna LIKE '" + pretvori("4") + "' ) ORDER BY stevilka_racuna ASC");
        sql_stetje_racunov.exec();
        while ( sql_stetje_racunov.next() ) {
            int st_racuna = 0;
            st_racuna = prevedi(sql_stetje_racunov.value(sql_stetje_racunov.record().indexOf("stevilka_racuna")).toString()).right(3).toInt();
            if ( st_racuna > max_st_racuna ) {
                max_st_racuna = st_racuna;
            }
        }

        qDebug("Najvisja stevilka obstojecega racuna je: " + QString::number(max_st_racuna, 10).toUtf8());
        max_st_racuna = max_st_racuna + 1;
        st_racuna = QString::number(max_st_racuna, 10);
        qDebug("Najvisja stevilka obstojecega racuna je: " + st_racuna.toUtf8());

        // iz stevilke racuna ustvarimo tromestno stevilko, pretvorjeno v besedo
        if ( st_racuna.length() == 1 ) {
            st_racuna = "00" + st_racuna;
        }
        else if ( st_racuna.length() == 2 ) {
            st_racuna = "0" + st_racuna;
        }
        st_racuna = QDate::currentDate().toString("yyyy").right(2) + st_racuna;
        qDebug("Nova stevilka racuna je: " + st_racuna.toUtf8());
    }

    base.close();

    return st_racuna;

}

void wid_racuni::osvezi(QString beseda) {

    if ( beseda == "racuni" ) {
        napolni();
        prenesi();
    }

}

QString wid_racuni::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_racuni::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

void wid_racuni::on_btn_nov_clicked() {

    racun *uredi = new racun;
    uredi->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               uredi , SLOT(prejem(QString)));
    prenos("Nov racun" + ui->txt_stprojekta->text()); // ce racun ne obstaja, naprej posljemo st. projekta
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(uredi, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_racuni::prejem(QString besedilo) {

    ui->txt_stprojekta->setText(besedilo);

    napolni();

}

void wid_racuni::tip(QString besedilo) {

    ui->txt_stprojekta->setText("*");

    ui->cb_racun->setCurrentIndex(ui->cb_racun->findText(besedilo + ") ", Qt::MatchStartsWith));

//    napolni();

}

void wid_racuni::on_btn_refresh_clicked() {

    napolni();

}

void wid_racuni::on_btn_print_clicked() {

    QString seznam_id = "";
    QModelIndexList selectedList = ui->tbl_racuni->selectionModel()->selectedRows();

    for( int i = 0; i < selectedList.count(); i++) {
        if ( ui->tbl_racuni->item(selectedList.at(i).row(), 2)->text() != "" ) {
            seznam_id.append(ui->tbl_racuni->item(selectedList.at(i).row(), 0)->text() + ",");
        }
    }

    // posljemo v tisk
    tiskanje *print = new tiskanje;
    print->show();
    QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
                 print , SLOT(prejem(QString, QString, QString)));
    tiskaj("izdani-racuni", seznam_id, "tisk"); // vrsta, id stevilke, format

}

void wid_racuni::on_btn_print_pdf_clicked() {

    QString seznam_id = "";
    QModelIndexList selectedList = ui->tbl_racuni->selectionModel()->selectedRows();

    for( int i = 0; i < selectedList.count(); i++) {
        if ( ui->tbl_racuni->item(selectedList.at(i).row(), 2)->text() != "" ) {
            seznam_id.append(ui->tbl_racuni->item(selectedList.at(i).row(), 0)->text() + ",");
        }
    }

    // posljemo v tisk
    tiskanje *print = new tiskanje;
    print->show();
    QObject::connect(this, SIGNAL(tiskaj(QString, QString, QString)),
                 print , SLOT(prejem(QString, QString, QString)));
    tiskaj("izdani-racuni", seznam_id, "pdf"); // vrsta, id stevilke, format

}

QString wid_racuni::pretvori_v_double(QString besedilo) {

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

QString wid_racuni::pretvori_iz_double(QString besedilo) {

    besedilo.replace(".",",");

    return besedilo;

}

void wid_racuni::on_btn_print_seznam_clicked() {

}

void wid_racuni::on_btn_prestevilci_clicked() {

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prestevilci-zapise");
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

        // naredi prestevilcenje za vse tipe racunov
        for ( int i_tip_racuna = 1; i_tip_racuna <= 3; i_tip_racuna++ ) {

            // poisci vsa leta; dobimo seznam vseh let, v katerih smo uradovali
            QStringList leta;

            QSqlQuery sql_leta;
            sql_leta.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori(QString::number(i_tip_racuna, 10)) +
                             "' AND stevilka_racuna NOT LIKE ''");
            sql_leta.exec();
            while ( sql_leta.next() ) {
                if ( !leta.contains(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_izdaje")).toString()).right(4)) ) {
                    leta.append(prevedi(sql_leta.value(sql_leta.record().indexOf("datum_izdaje")).toString()).right(4));
                }
            }

            // razvrsti leta narascajoce
            leta.sort();

            // za vsako leto poisci mesece, dneve in id-je ter jih zapisi v nov seznam
            for ( int i_leta = 0; i_leta < leta.count(); i_leta++ ) {
                // ustvari nov seznam id-jev, mesecev in zaporednih stevilk
                QStringList seznam_vnosov;
                QStringList meseci;
                int zaporedna_stevilka = 0;

                // poisci vse mesece, dobimo seznam mesecev v danem letu
                QSqlQuery sql_meseci;
                sql_meseci.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori(QString::number(i_tip_racuna, 10)) +
                                   "' AND stevilka_racuna NOT LIKE '" +
                                   "' AND datum_izdaje LIKE '%." + leta.value(i_leta) + "'");
                sql_meseci.exec();
                while ( sql_meseci.next() ) {
                    if ( !meseci.contains(prevedi(sql_meseci.value(sql_meseci.record().indexOf("datum_izdaje")).toString()).right(7).left(2)) ) {
                        meseci.append(prevedi(sql_meseci.value(sql_meseci.record().indexOf("datum_izdaje")).toString()).right(7).left(2));
                    }
                }

                // razvrsti po mesecu
                meseci.sort();

                // za vsak mesec poiscemo dneve, idje ter jih zapisemo v nov seznam
                for ( int i_meseci = 0; i_meseci < meseci.count(); i_meseci++ ) {

                    // ustvarimo nov seznam dnevov
                    QStringList dnevi;

                    // poisci vse dneve, dobimo seznam dnevov v danem letu in danem mesecu
                    QSqlQuery sql_dnevi;
                    sql_dnevi.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori(QString::number(i_tip_racuna, 10)) +
                                      "' AND stevilka_racuna NOT LIKE '" +
                                      "' AND datum_izdaje LIKE '%." + meseci.value(i_meseci) + "." + leta.value(i_leta) + "'");
                    sql_dnevi.exec();
                    while ( sql_dnevi.next() ) {
                        if ( !dnevi.contains(prevedi(sql_dnevi.value(sql_dnevi.record().indexOf("datum_izdaje")).toString()).left(2)) ) {
                            dnevi.append(prevedi(sql_dnevi.value(sql_dnevi.record().indexOf("datum_izdaje")).toString()).left(2));
                        }
                    }

                    // razvrsti po dnevih
                    dnevi.sort();

                    // za vsak dan poisci id-je ter jih zapisi v nov seznam
                    for ( int i_dnevi = 0; i_dnevi < dnevi.count(); i_dnevi++ ) {

                        // poisci vse id-je, dobimo seznam id-jev v danem letu in danem mesecu in danem dnevu
                        QSqlQuery sql_id_ji;
                        sql_id_ji.prepare("SELECT * FROM racuni WHERE tip_racuna LIKE '" + pretvori(QString::number(i_tip_racuna, 10)) +
                                          "' AND stevilka_racuna NOT LIKE '" +
                                          "' AND datum_izdaje LIKE '" + dnevi.value(i_dnevi) + "." + meseci.value(i_meseci) + "." + leta.value(i_leta) + "'");
                        sql_id_ji.exec();
                        while ( sql_id_ji.next() ) {
                            if ( !seznam_vnosov.contains(prevedi(sql_id_ji.value(sql_id_ji.record().indexOf("id")).toString())) ) {
                                seznam_vnosov.append(prevedi(sql_id_ji.value(sql_id_ji.record().indexOf("id")).toString()));
                            }
                        }

                        sql_id_ji.clear();

                    } // for ( int i_dnevi = 0; i_dnevi < dnevi.count(); i_dnevi++ )

                    dnevi.clear();
                    sql_dnevi.clear();

                } // for ( int i_meseci = 0; i_meseci < meseci.count(); i_meseci++ )

                // pojdi cez cel seznam vnosov (notri so IDji po vrstnem redu) in vsakega izpisi ( kasneje popravi )
                for ( int i_seznam_vnosov = 0; i_seznam_vnosov < seznam_vnosov.count(); i_seznam_vnosov++ ) {

                    zaporedna_stevilka++;
                    QString zaporedna = "";
                    if ( zaporedna_stevilka < 10 ) {
                        zaporedna = "00" + QString::number(zaporedna_stevilka, 10);
                    }
                    else if ( zaporedna_stevilka < 100 ) {
                        zaporedna = "0" + QString::number(zaporedna_stevilka, 10);
                    }
                    else {
                        zaporedna = "" + QString::number(zaporedna_stevilka, 10);
                    }

                    QSqlQuery sql_prestevilci;
                    sql_prestevilci.prepare("UPDATE racuni SET stevilka_racuna = ? WHERE id LIKE '" + pretvori(seznam_vnosov.value(i_seznam_vnosov)) + "'");
                    sql_prestevilci.bindValue(0, pretvori(leta.value(i_leta).right(2) + zaporedna));
                    sql_prestevilci.exec();

                } // for ( int i_seznam_vnosov = 0; i_seznam_vnosov < seznam_vnosov.count(); i_seznam_vnosov++ )

                meseci.clear();
                sql_meseci.clear();
                seznam_vnosov.clear();

            } // for ( int i_leta = 0; i_leta < leta.count(); i_leta++ )

            leta.clear();
            sql_leta.clear();

        } // for ( int i_tip_racuna = 1; i_tip_racuna <= 3; i_tip_racuna++ )

    }

    base.close();

    // sporocilo ob zakljucku prestevilcevanja
    QMessageBox zakljucek;
    zakljucek.setText("Prestevilcenje zakljuceno");
    zakljucek.setInformativeText("Prestevilcenje zahtevanih vnosov je bilo uspesno zakljuceno.");
    zakljucek.exec();

    napolni();

}

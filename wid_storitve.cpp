#include <QtSql>
#include <QMessageBox>

#include "kodiranje.h"
#include "storitve.h"

#include "wid_storitve.h"
#include "ui_wid_storitve.h"

wid_storitve::wid_storitve(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_storitve)
{
    ui->setupUi(this);

    napolni_kategorije();

    napolni_aktivnosti();

}

wid_storitve::~wid_storitve()
{
    delete ui;
}

void wid_storitve::on_btn_nov_clicked() {

    storitve *nova = new storitve;
    nova->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               nova , SLOT(prejem(QString)));
    prenos("Nova storitev"); // ko vnasamo novo storitev, ne prenesemo nobenega ID-ja
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(nova, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_storitve::on_tbl_storitve_doubleClicked() {

    storitve *nova = new storitve;
    nova->show();
    QObject::connect(this, SIGNAL(prenos(QString)),
               nova , SLOT(prejem(QString)));
    prenos(ui->tbl_storitve->selectedItems().takeAt(0)->text()); // v novo okno prenesemo zaporedni ID storitve, ki jo popravljamo
    this->disconnect();

    // receive signal to refresh table
    QObject::connect(nova, SIGNAL(poslji(QString)),
               this , SLOT(osvezi(QString)));

}

void wid_storitve::osvezi(QString beseda) {

    if ( beseda == "storitve" ) {
        on_btn_osvezi_clicked();
    }

}

void wid_storitve::napolni_kategorije() {

    ui->cb_kategorija->clear();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_kategorije");
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

        ui->cb_kategorija->addItem("");

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT* FROM sif_kategorije WHERE aktivnost LIKE '1' ORDER BY indeks ASC");
        sql_fill.exec();
        while ( sql_fill.next() ) {
            ui->cb_kategorija->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("kategorija")).toString()));
        }
    }
    base.close();

}

void wid_storitve::napolni_podkategorije() {

    ui->cb_podkategorija->clear();

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_podkategorije");
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

        ui->cb_podkategorija->addItem("");

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT* FROM sif_podkategorije WHERE aktivnost LIKE '1' AND kategorija LIKE '" + pretvori(ui->cb_kategorija->currentText()) + "' ORDER BY indeks ASC");
        sql_fill.exec();
        while ( sql_fill.next() ) {
            ui->cb_podkategorija->addItem(prevedi(sql_fill.value(sql_fill.record().indexOf("podkategorija")).toString()));
        }
    }
    base.close();

}

void wid_storitve::napolni_aktivnosti() {

    ui->cb_aktivnost->clear();
    ui->cb_aktivnost->addItem("");
    ui->cb_aktivnost->addItem("Aktivne");
    ui->cb_aktivnost->addItem("Neaktivne");

}

void wid_storitve::on_btn_pobrisi_filtre_clicked() {

    ui->txt_iskalnik->setText("");
    ui->cb_aktivnost->setCurrentIndex(ui->cb_aktivnost->findText(""));
    ui->cb_kategorija->setCurrentIndex(ui->cb_kategorija->findText(""));
    ui->cb_podkategorija->setCurrentIndex(ui->cb_podkategorija->findText(""));

}

void wid_storitve::on_btn_osvezi_clicked() {

    napolni();

}

void wid_storitve::on_btn_aktiviraj_clicked() {

    QString seznam_id = "";
    QModelIndexList selectedList = ui->tbl_storitve->selectionModel()->selectedRows();

    for( int i = 0; i < selectedList.count(); i++ ) {
        if ( ui->tbl_storitve->item(selectedList.at(i).row(), 0)->text() != "" ) {

            QString aktivnost = "1"; // neaktivno bomo spremenili v aktnivno!!!
            if ( ui->tbl_storitve->item(selectedList.at(i).row(), 10)->text() == "Aktivna" ) {
                aktivnost = "0"; // aktivno bomo spremenili v neaktivno!!!
            }

            QString app_path = QApplication::applicationDirPath();
            QString dbase_path = app_path + "/base.bz";

            QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_tabelo");
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
                QSqlQuery sql_change;
                sql_change.prepare("UPDATE sif_storitve SET aktivnost = ? WHERE id LIKE '" + ui->tbl_storitve->item(selectedList.at(i).row(), 0)->text() + "'");
                sql_change.bindValue(0, aktivnost);
                sql_change.exec();
            }
            base.close();
        }
    }

    on_btn_osvezi_clicked();

}

void wid_storitve::napolni() {

    ui->btn_nov->setText("Polnim");

    int izbranec = 0;
    int razvrsti = 1;

    if ( ui->tbl_storitve->selectedItems().count() > 0 ) {
        izbranec = ui->tbl_storitve->selectedItems().takeAt(0)->row();
        razvrsti = ui->tbl_storitve->horizontalHeader()->sortIndicatorSection();
    }


    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "napolni_tabelo");
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
        ui->tbl_storitve->clear();

        for (int i = 0; i <= 10; i++) {
            ui->tbl_storitve->removeColumn(0);
        }

        QSqlQuery sql_clear("wid_racuni");
        sql_clear.prepare("SELECT * FROM sif_storitve");
        sql_clear.exec();
        while (sql_clear.next()) {
            ui->tbl_storitve->removeRow(0);
        }

        // start filling the table
        ui->tbl_storitve->insertColumn(0);
        ui->tbl_storitve->insertColumn(1);
        ui->tbl_storitve->insertColumn(2);
        ui->tbl_storitve->insertColumn(3);
        ui->tbl_storitve->insertColumn(4);
        ui->tbl_storitve->insertColumn(5);
        ui->tbl_storitve->insertColumn(6);
        ui->tbl_storitve->insertColumn(7);
        ui->tbl_storitve->insertColumn(8);
        ui->tbl_storitve->insertColumn(9);
        ui->tbl_storitve->insertColumn(10);

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
        naslov1->setText("Sifra");
        naslov2->setText("Kategorija");
        naslov3->setText("Podkategorija");
        naslov4->setText("Storitev");
        naslov5->setText("Enota");
        naslov6->setText("Stopnja DDV");
        naslov7->setText("Znesek brez DDV");
        naslov8->setText("Znesek DDV");
        naslov9->setText("Znesek z DDV");
        naslov10->setText("Aktivnost");

        ui->tbl_storitve->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_storitve->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_storitve->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_storitve->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_storitve->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_storitve->setHorizontalHeaderItem(5, naslov5);
        ui->tbl_storitve->setHorizontalHeaderItem(6, naslov6);
        ui->tbl_storitve->setHorizontalHeaderItem(7, naslov7);
        ui->tbl_storitve->setHorizontalHeaderItem(8, naslov8);
        ui->tbl_storitve->setHorizontalHeaderItem(9, naslov9);
        ui->tbl_storitve->setHorizontalHeaderItem(10, naslov10);

        ui->tbl_storitve->setColumnWidth(0, 35);

        // pripravimo morebitne pogoje za filtriranje tabele
        QString stavek = "";

        if ( ui->cb_aktivnost->currentText() != "" ) {
            if ( stavek != "" ) {
                stavek += " AND ";
            }
            if ( ui->cb_aktivnost->currentText() == "Aktivne" ) {
                stavek += "aktivnost LIKE '1'";
            }
            else {
                stavek += "aktivnost LIKE '0'";
            }
        }

        if ( ui->cb_kategorija->currentText() != "" ) {
            if ( stavek != "" ) {
                stavek += " AND ";
            }
            stavek += "kategorija LIKE '" + pretvori(ui->cb_kategorija->currentText()) + "'";
        }

        if ( ui->cb_podkategorija->currentText() != "" ) {
            if ( stavek != "" ) {
                stavek += " AND ";
            }
            stavek += "podkategorija LIKE '" + pretvori(ui->cb_podkategorija->currentText()) + "'";
        }

        if ( ui->txt_iskalnik->text() != "" ) {
            if ( stavek != "" ) {
                stavek += " AND ";
            }
            stavek += "( storitev LIKE '%" + pretvori(ui->txt_iskalnik->text()) + "%' OR ";
            stavek += "sifra LIKE '%" + pretvori(ui->txt_iskalnik->text()) + "%' OR ";
            stavek += "kategorija LIKE '" + pretvori(ui->txt_iskalnik->text()) + "%' OR ";
            stavek += "podkategorija LIKE '" + pretvori(ui->txt_iskalnik->text()) + "%' )";
        }

        if ( stavek != "" ) {
            stavek = " WHERE " + stavek;
        }

        QSqlQuery sql_fill;
        sql_fill.prepare("SELECT * FROM sif_storitve" + stavek);
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_storitve->insertRow(row);
            ui->tbl_storitve->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[11] = {"id", "sifra", "kategorija", "podkategorija", "storitev", "enota", "stopnja_ddv", "znesek_brez_ddv",
                                 "znesek_ddv", "znesek_z_ddv", "aktivnost"};

            while (col <= 10) {
                QTableWidgetItem *celica = new QTableWidgetItem;
                if ( polja[i] == "id" ) {
                    celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                }
                else if ( polja[i] == "aktivnost" ) {
                    if ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()) == "1" ) { // storitev je aktivna
                        celica->setText("Aktivna");
                    }
                    else { // storitev ni aktivna, status je 0
                        celica->setText("Ni aktivna");
                    }
                }
                else if ( polja[i] == "stopnja_ddv" ) {
                    celica->setText(pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString())) + "%");
                }
                else if ( polja[i] == "znesek_brez_ddv" || polja[i] == "znesek_ddv" || polja[i] == "znesek_z_ddv" ) {
                    celica->setText(pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString())));
                }
                else {
                    celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                }
                ui->tbl_storitve->setItem(row, col, celica);
                col++;
                i++;
            }
            row++;
        }
    }
    base.close();

    ui->tbl_storitve->selectRow(izbranec);
    ui->tbl_storitve->sortByColumn(razvrsti, Qt::AscendingOrder);

    ui->btn_nov->setText("Nova");

}

void wid_storitve::on_cb_kategorija_currentIndexChanged() {

    napolni_podkategorije();
    napolni();

}

void wid_storitve::on_cb_podkategorija_currentIndexChanged() {

    napolni();

}

void wid_storitve::on_cb_aktivnost_currentIndexChanged() {

    napolni();

}

void wid_storitve::on_txt_iskalnik_textChanged() {

    napolni();

}

QString wid_storitve::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_storitve::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

QString wid_storitve::pretvori_v_double(QString besedilo) {

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

QString wid_storitve::pretvori_iz_double(QString besedilo) {

    besedilo.replace(".",",");

    return besedilo;

}

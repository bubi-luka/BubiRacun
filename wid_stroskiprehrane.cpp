#include <QtSql>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QModelIndexList>
#include <QTextEdit>
#include <QTimer>

#include "kodiranje.h"
#include "varnost.h"
#include "wid_stroskiprehrane.h"
#include "ui_wid_stroskiprehrane.h"

wid_stroskiprehrane::wid_stroskiprehrane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wid_stroskiprehrane)
{
    ui->setupUi(this);

    izbrisi_tabelo();
    izbrisi_leta();

    napolni_leta();

}

wid_stroskiprehrane::~wid_stroskiprehrane()
{
    delete ui;
}

void wid_stroskiprehrane::izbrisi_leta() {

    ui->cb_leto->clear();

}

void wid_stroskiprehrane::izbrisi_tabelo() {

    ui->tbl_stroski_prehrane->clear();

    while ( ui->tbl_stroski_prehrane->rowCount() > 0 ) {
        ui->tbl_stroski_prehrane->removeRow(0);
    }

    while ( ui->tbl_stroski_prehrane->columnCount() > 0 ) {
        ui->tbl_stroski_prehrane->removeColumn(0);
    }

}

void wid_stroskiprehrane::napolni_leta() {

    // filtriraj po letu
    ui->cb_leto->addItem("");

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prehrana-napolni-leta");
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
        sql_napolni.prepare("SELECT * FROM stroski_prehrane WHERE izplacilo_dni_" + pretvori(vApp->id()) + " IS NOT NULL");
        sql_napolni.exec();
        while ( sql_napolni.next() ) {
            QString leto = prevedi(sql_napolni.value(sql_napolni.record().indexOf("leto")).toString());
            if ( ui->cb_leto->findText(leto) == -1 ) {
                ui->cb_leto->addItem(leto);
            }
        }
        sql_napolni.clear();

        if ( ui->cb_leto->findText(QDate::currentDate().toString("yyyy")) == -1 ) {
            ui->cb_leto->addItem(QDate::currentDate().toString("yyyy"));
        }

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
    }
    base.close();

}

void wid_stroskiprehrane::napolni_tabelo() {

    ui->btn_kopiraj->setText("Vnasam");

    double strosek_za_malico = 0.00;

    QString app_path = QApplication::applicationDirPath();
    QString dbase_path = app_path + "/base.bz";

    QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prehrana-napolni-leta");
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
        // start filling the table
        ui->tbl_stroski_prehrane->insertColumn(0);
        ui->tbl_stroski_prehrane->insertColumn(1);
        ui->tbl_stroski_prehrane->insertColumn(2);
        ui->tbl_stroski_prehrane->insertColumn(3);
        ui->tbl_stroski_prehrane->insertColumn(4);
        ui->tbl_stroski_prehrane->insertColumn(5);
        ui->tbl_stroski_prehrane->insertColumn(6);
        ui->tbl_stroski_prehrane->insertColumn(7);

        QTableWidgetItem *naslov0 = new QTableWidgetItem;
        QTableWidgetItem *naslov1 = new QTableWidgetItem;
        QTableWidgetItem *naslov2 = new QTableWidgetItem;
        QTableWidgetItem *naslov3 = new QTableWidgetItem;
        QTableWidgetItem *naslov4 = new QTableWidgetItem;
        QTableWidgetItem *naslov5 = new QTableWidgetItem;
        QTableWidgetItem *naslov6 = new QTableWidgetItem;
        QTableWidgetItem *naslov7 = new QTableWidgetItem;

        naslov0->setText("ID");
        naslov1->setText("Mesec");
        naslov2->setText("St. delovnih dni");
        naslov3->setText("Dopust");
        naslov4->setText("Bolniska");
        naslov5->setText("Delovni dnevi");
        naslov6->setText("Cena malice");
        naslov7->setText("Skupen znesek");

        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(0, naslov0);
        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(1, naslov1);
        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(2, naslov2);
        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(3, naslov3);
        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(4, naslov4);
        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(5, naslov5);
        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(6, naslov6);
        ui->tbl_stroski_prehrane->setHorizontalHeaderItem(7, naslov7);
        ui->tbl_stroski_prehrane->setColumnWidth(0, 35);
        ui->tbl_stroski_prehrane->setColumnWidth(1, 100);
        ui->tbl_stroski_prehrane->setColumnWidth(2, 100);
        ui->tbl_stroski_prehrane->setColumnWidth(3, 100);
        ui->tbl_stroski_prehrane->setColumnWidth(4, 100);
        ui->tbl_stroski_prehrane->setColumnWidth(5, 100);
        ui->tbl_stroski_prehrane->setColumnWidth(6, 100);
        ui->tbl_stroski_prehrane->setColumnWidth(7, 150);

        QSqlQuery sql_fill;
        if ( ui->cb_leto->currentText() == "" ) {
            sql_fill.prepare("SELECT * FROM stroski_prehrane"); // prikazi za vsa leta
        }
        else {
            sql_fill.prepare("SELECT * FROM stroski_prehrane WHERE leto LIKE '" + pretvori(ui->cb_leto->currentText()) + "'");
        }
        sql_fill.exec();

        int row = 0;
        while (sql_fill.next()) {
            ui->tbl_stroski_prehrane->insertRow(row);
            ui->tbl_stroski_prehrane->setRowHeight(row, 20);
            int col = 0;
            int i = 0;
            QString polja[8] = {"id", "mesec", "delavniki", "dopust_" + pretvori(vApp->id()),
                                "bolezen_" + pretvori(vApp->id()), "izplacilo_dni_" + pretvori(vApp->id()),
                                "cena_malice", "izplacilo_znesek_" + pretvori(vApp->id())};

            while (col <= 7) {

                QTableWidgetItem *celica = new QTableWidgetItem;
                if ( polja[i] == "id" ) {
                    celica->setData(Qt::DisplayRole, prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt());
                    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                }
                else if ( polja[i] == "mesec" ) {
                    QString str_mesec = "";
                    switch ( prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()).toInt() ) {
                        case 1:
                        str_mesec = "Januar";
                        break;
                        case 2:
                        str_mesec = "Februar";
                        break;
                        case 3:
                        str_mesec = "Marec";
                        break;
                        case 4:
                        str_mesec = "April";
                        break;
                        case 5:
                        str_mesec = "Maj";
                        break;
                        case 6:
                        str_mesec = "Junij";
                        break;
                        case 7:
                        str_mesec = "Julij";
                        break;
                        case 8:
                        str_mesec = "Avgust";
                        break;
                        case 9:
                        str_mesec = "September";
                        break;
                        case 10:
                        str_mesec = "Oktober";
                        break;
                        case 11:
                        str_mesec = "November";
                        break;
                        case 12:
                        str_mesec = "December";
                        break;
                        default:
                        str_mesec = "";
                        break;
                    }
                    if ( ui->cb_leto->currentText() == "" ) {
                        str_mesec += " " + prevedi(sql_fill.value(sql_fill.record().indexOf("leto")).toString());
                    }
                    celica->setText(str_mesec);
                    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                }
                else if ( polja[i] == "delavniki" ) {
                    celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                }
                else if ( polja[i] == "cena_malice" ) {
                    QString str_malica = "";
                    str_malica = pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    if ( str_malica == "0,00 EUR" ) {
                        QSqlQuery sql_malica;
                        sql_malica.prepare("SELECT * FROM sif_cena_malice");
                        sql_malica.exec();
                        while ( sql_malica.next() ) {
                            str_malica =  pretvori_iz_double(prevedi(sql_malica.value(sql_malica.record().indexOf("cena")).toString()));
                            celica->setFlags(Qt::ItemIsSelectable);
                        }
                    }
                    celica->setText(str_malica);
                    str_malica = "";
                }
                else if ( polja[i] == "izplacilo_dni_" + pretvori(vApp->id()) ) {
                    celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                }
                else if ( polja[i] == "izplacilo_znesek_" + pretvori(vApp->id()) ) {
                    celica->setText(pretvori_iz_double(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString())));
                    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    strosek_za_malico += pretvori_v_double(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString())).toDouble();
                }
                else {
                    celica->setText(prevedi(sql_fill.value(sql_fill.record().indexOf(polja[i])).toString()));
                }
                ui->tbl_stroski_prehrane->setItem(row, col, celica);

                col++;
                i++;

            }

            row++;

        }

    }
    base.close();

    ui->btn_kopiraj->setText("Copy - Paste");

    ui->lbl_prehrana->setText(pretvori_iz_double(QString::number(strosek_za_malico, 'f', 2)));

    QTimer::singleShot(1, this, SLOT(preracunaj()));

}

void wid_stroskiprehrane::preracunaj() {

    for ( int i = 0; i < 12; i++ ) {
        if ( ui->tbl_stroski_prehrane->item(i, 7)->text() == "" ) {
            izracunaj_delavne_dni(i, 0);
        }
    }
}

void wid_stroskiprehrane::izracunaj_delavne_dni(int vrstica, int status) {

    int vsi_dnevi = 0;
    int dopusti = 0;
    int bolniske = 0;
    int dejanski_dnevi = 0;

    if ( ui->tbl_stroski_prehrane->item(vrstica, 2)->text() != "" ) {
        vsi_dnevi = ui->tbl_stroski_prehrane->item(vrstica, 2)->text().toInt();
    }
    if ( ui->tbl_stroski_prehrane->item(vrstica, 3)->text() != "" ) {
        dopusti = ui->tbl_stroski_prehrane->item(vrstica, 3)->text().toInt();
    }
    if ( ui->tbl_stroski_prehrane->item(vrstica, 4)->text() != "" ) {
        bolniske = ui->tbl_stroski_prehrane->item(vrstica, 4)->text().toInt();
    }

    dejanski_dnevi = vsi_dnevi - dopusti - bolniske;

    ui->btn_kopiraj->setText("Vnasam");

    QTableWidgetItem *celica = new QTableWidgetItem;
    celica->setText(QString::number(dejanski_dnevi, 10));
    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->tbl_stroski_prehrane->setItem(vrstica, 5, celica);

    ui->btn_kopiraj->setText("Copy - Paste");

    izracunaj_znesek_malice(vrstica, status);

}

void wid_stroskiprehrane::izracunaj_znesek_malice(int vrstica, int status) {

    double cena = 0.00;
    double dnevi = 0;
    double znesek = 0.00;

    if ( ui->tbl_stroski_prehrane->item(vrstica, 5)->text() != "" ) {
        dnevi = ui->tbl_stroski_prehrane->item(vrstica, 5)->text().toDouble();
    }

    cena = pretvori_v_double(ui->tbl_stroski_prehrane->item(vrstica, 6)->text()).toDouble();

    znesek = dnevi * cena;

    ui->btn_kopiraj->setText("Vnasam");

    QTableWidgetItem *celica = new QTableWidgetItem;
    celica->setText(pretvori_iz_double(QString::number(znesek, 'f', 2)));
    celica->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->tbl_stroski_prehrane->setItem(vrstica, 7, celica);

    ui->btn_kopiraj->setText("Copy - Paste");

    shrani_vrstico(vrstica, status);

}

void wid_stroskiprehrane::shrani_vrstico(int vrstica, int status) {

    if ( status == 1 ) {
        QString app_path = QApplication::applicationDirPath();
        QString dbase_path = app_path + "/base.bz";

        QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "prehrana-shrani");
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

            QSqlQuery sql_vnesi;
            sql_vnesi.prepare("UPDATE stroski_prehrane SET cena_malice = ?, 'bolezen_" + vApp->id() + "' = ?, "
                              "'dopust_" + vApp->id() + "' = ?, 'izplacilo_dni_" + vApp->id() + "' = ?, "
                              "'izplacilo_znesek_" + vApp->id() + "' = ? WHERE id LIKE '" +
                              ui->tbl_stroski_prehrane->item(vrstica, 0)->text() + "'");
            sql_vnesi.bindValue(0, pretvori(pretvori_v_double(ui->tbl_stroski_prehrane->item(vrstica, 6)->text())));
            sql_vnesi.bindValue(1, pretvori(ui->tbl_stroski_prehrane->item(vrstica, 4)->text()));
            sql_vnesi.bindValue(2, pretvori(ui->tbl_stroski_prehrane->item(vrstica, 3)->text()));
            sql_vnesi.bindValue(3, pretvori(ui->tbl_stroski_prehrane->item(vrstica, 5)->text()));
            sql_vnesi.bindValue(4, pretvori(pretvori_v_double(ui->tbl_stroski_prehrane->item(vrstica, 7)->text())));
            sql_vnesi.exec();
        }
        base.close();
    }

}

void wid_stroskiprehrane::on_tbl_stroski_prehrane_itemChanged(QTableWidgetItem *celica) {

    if ( ui->btn_kopiraj->text() != "Vnasam" ) {
        if ( celica->text() == celica->text().remove(QRegExp("[^0-9]*")) &&
             ui->tbl_stroski_prehrane->item(celica->row(), 2)->text().toInt() + 1 >
             ( ui->tbl_stroski_prehrane->item(celica->row(), 3)->text().toInt() +
               ui->tbl_stroski_prehrane->item(celica->row(), 4)->text().toInt() ) ) {
            izracunaj_delavne_dni(celica->row(), 1);
        }
        else {
            QTableWidgetItem *polje = new QTableWidgetItem;
            polje->setText("");
            ui->tbl_stroski_prehrane->setItem(celica->row(), celica->column(), polje);
        }
    }

}

void wid_stroskiprehrane::on_btn_osvezi_clicked() {

    izbrisi_leta();
    izbrisi_tabelo();
    napolni_leta();

}

void wid_stroskiprehrane::on_btn_kopiraj_clicked() {

    QClipboard *odlozisce = QApplication::clipboard();

    QModelIndexList selectedList = ui->tbl_stroski_prehrane->selectionModel()->selectedRows();

    QString html_besedilo = "<table>";
    html_besedilo += "<tr>";
    html_besedilo += "<th>ID</th>";
    html_besedilo += "<th>Mesec</th>";
    html_besedilo += "<th>St. delovnih dni</th>";
    html_besedilo += "<th>Dopust</th>";
    html_besedilo += "<th>Bolniska</th>";
    html_besedilo += "<th>Delovni dnevi</th>";
    html_besedilo += "<th>Cena malice</th>";
    html_besedilo += "<th>Skupen znesek</th>";
    html_besedilo += "</tr>";

    for( int i = 0; i < selectedList.count(); i++) {
        html_besedilo += "<tr>";
        for ( int a = 0; a < 8; a++ ) {
            html_besedilo += "<td>";
            html_besedilo += ui->tbl_stroski_prehrane->item(selectedList.at(i).row(), a)->text();
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

void wid_stroskiprehrane::on_cb_leto_currentIndexChanged() {

    izbrisi_tabelo();
    napolni_tabelo();

}

QString wid_stroskiprehrane::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString wid_stroskiprehrane::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

QString wid_stroskiprehrane::pretvori_v_double(QString besedilo) {

    /*
    * pretvarja znake v format double
    * prejme poljubni format, vrne double
    */

    besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
    besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

    return besedilo;

}

QString wid_stroskiprehrane::pretvori_iz_double(QString besedilo) {

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

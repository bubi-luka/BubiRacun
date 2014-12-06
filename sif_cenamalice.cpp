#include <QMessageBox>
#include <QtSql>
#include <QDate>
#include <QKeyEvent>

#include "varnost.h"
#include "kodiranje.h"
#include "sif_cenamalice.h"
#include "ui_sif_cenamalice.h"

sif_cenamalice::sif_cenamalice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sif_cenamalice)
{
    ui->setupUi(this);

    // ponastavi vsa polja
    ui->txt_datum->setDate(QDate::currentDate());
    ui->txt_cena->setText("");

    // blokiraj polja in gumbe
    ui->btn_sprejmi->setEnabled(false);

    // napolni polja
    prejem();

}

sif_cenamalice::~sif_cenamalice()
{
    delete ui;
}

void sif_cenamalice::on_btn_izhod_clicked() {

    close();

}

// ne preverja napak
void sif_cenamalice::on_btn_sprejmi_clicked() {

    ui->txt_datum->setDate(QDate::currentDate());
    QString napaka = "";

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
            QSqlQuery sql_vnesi;
            sql_vnesi.prepare("INSERT INTO sif_cena_malice (cena, datum, avtor_oseba) VALUES (?, ?, ?)");
            sql_vnesi.bindValue(0, pretvori(pretvori_v_double(ui->txt_cena->text())));
            sql_vnesi.bindValue(1, pretvori(ui->txt_datum->text()));
            sql_vnesi.bindValue(2, pretvori(vApp->id()));
            sql_vnesi.exec();
        }
        base.close();

        // close this window
        close();
    }
    else {
        QMessageBox msgbox;
        msgbox.setText("Dolocena polja niso pravilno izpolnjena");
        msgbox.exec();
    }

}

void sif_cenamalice::on_txt_cena_textChanged() {

    ui->btn_sprejmi->setEnabled(true);

}

QString sif_cenamalice::pretvori(QString besedilo) {

    return kodiranje().zakodiraj(besedilo);

}

QString sif_cenamalice::prevedi(QString besedilo) {

    return kodiranje().odkodiraj(besedilo);

}

QString sif_cenamalice::pretvori_v_double(QString besedilo) {

    /*
    * pretvarja znake v format double
    * prejme poljubni format, vrne double
    */

    besedilo.replace(",", "."); // zamenja decimalno piko (double) za vejiso (SI)
    besedilo.remove(QRegExp("[^0-9.]")); // odstrani vse znake razen stevilk in decimalne vejice

    return besedilo;

}

QString sif_cenamalice::pretvori_iz_double(QString besedilo) {

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

void sif_cenamalice::prejem() {

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
        sql_napolni.prepare("SELECT * FROM sif_cena_malice");
        sql_napolni.exec();
        while (sql_napolni.next()) {
            ui->txt_cena->setText(pretvori_iz_double(prevedi(sql_napolni.value(sql_napolni.record().indexOf("cena")).toString())));
            QDate datum = QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum")).toString()), "dd.MM.yyyy");
            ui->txt_datum->setDate(datum);
        }
    }
    base.close();

    // zapri gumb za spreminjanje
    ui->btn_sprejmi->setEnabled(false);

}

void sif_cenamalice::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Return)  {
        this->on_btn_sprejmi_clicked();
    }
    else if (event->key() == Qt::Key_Escape) {
        this->on_btn_izhod_clicked();
    }

}

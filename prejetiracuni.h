#ifndef PREJETIRACUNI_H
#define PREJETIRACUNI_H

#include <QDialog>
#include <QDate>
#include <QTableWidgetItem>

namespace Ui {
    class prejetiracuni;
}

class prejetiracuni : public QDialog
{
    Q_OBJECT

public:
    explicit prejetiracuni(QWidget *parent = 0);
    ~prejetiracuni();

private slots:
    void on_btn_izhod_clicked();
    void on_btn_izpisi_clicked();
    void on_btn_sprejmi_clicked();

    void keyPressEvent(QKeyEvent *event);

    void on_txt_izberi_osebo_currentIndexChanged(QString besedilo);
    void on_txt_izberi_projekt_currentIndexChanged(QString besedilo);

    void on_cb_oseba_toggled(bool stanje);
    void on_cb_projekt_toggled(bool stanje);

    void on_txt_posta_currentIndexChanged(QString besedilo);
    void on_txt_postna_stevilka_textChanged(QString besedilo);

    void on_txt_naziv_podjetja_kratki_editingFinished();

    void on_txt_datum_prejema_dateChanged();

    void on_btn_dodaj_ddv_clicked();
    void on_tbl_ddv_itemChanged(QTableWidgetItem *celica);
    void napolni_ddv();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_iz_double(QString besedilo);
    QString pretvori_v_double(QString besedilo);

    void prejem(QString besedilo);
    void stevilka_racuna();
    void dodaj(QString besedilo);
    void izracunaj();

signals:
    void poslji(QString beseda);
    void tiskaj(QString vrsta, QString stevilke, QString format);
    void prenos(QString beseda);

private:
    Ui::prejetiracuni *ui;
};

#endif // PREJETIRACUNI_H

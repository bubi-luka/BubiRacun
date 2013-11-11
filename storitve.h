#ifndef STORITVE_H
#define STORITVE_H

#include <QDialog>

namespace Ui {
class storitve;
}

class storitve : public QDialog
{
    Q_OBJECT

public:
    explicit storitve(QWidget *parent = 0);
    ~storitve();

public slots:
    void on_btn_izhod_clicked();
    void on_btn_izbrisi_clicked();
    void on_btn_vnesi_clicked();
    void on_btn_sifra_clicked();

    void on_cb_kategorija_currentIndexChanged();

    void napolni_kategorije();
    void napolni_podkategorije();
    void napolni_enote();
    void napolni_davke();

    void tvori_sifro();
    void izracunaj_cene();

    void prejem(QString besedilo);

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_v_double(QString besedilo);
    QString pretvori_iz_double(QString besedilo);

signals:
    void poslji(QString beseda);

private:
    Ui::storitve *ui;
};

#endif // STORITVE_H

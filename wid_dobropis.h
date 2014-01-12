#ifndef WID_DOBROPIS_H
#define WID_DOBROPIS_H

#include <QWidget>

namespace Ui {
class wid_dobropis;
}

class wid_dobropis : public QWidget
{
    Q_OBJECT

public:
    explicit wid_dobropis(QWidget *parent = 0);
    ~wid_dobropis();

private slots:
    void prejem(QString besedilo);

    void on_btn_izhod_clicked();
    void on_btn_izprazni_clicked();
    void on_btn_potrdi_clicked();

    void on_tbl_storitve_itemSelectionChanged();
    void on_txt_st_enot_na_dobropisu_editingFinished();
    void on_cb_aktivirana_toggled();

    void napolni_racun();
    void napolni_tabelo();
    void izracunaj();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_v_double(QString besedilo);
    QString pretvori_iz_double(QString besedilo);

signals:
    void poslji(QString beseda);
    void prenos(QString beseda);

private:
    Ui::wid_dobropis *ui;
};

#endif // WID_DOBROPIS_H

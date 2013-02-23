#ifndef WID_STROSKIPREHRANE_H
#define WID_STROSKIPREHRANE_H

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
class wid_stroskiprehrane;
}

class wid_stroskiprehrane : public QWidget
{
    Q_OBJECT
    
public:
    explicit wid_stroskiprehrane(QWidget *parent = 0);
    ~wid_stroskiprehrane();
    
private slots:
    void izbrisi_tabelo();
    void izbrisi_leta();
    void napolni_leta();
    void napolni_tabelo();
    void preracunaj();

    void izracunaj_delavne_dni(int vrstica, int status);
    void izracunaj_znesek_malice(int vrstica, int status);
    void shrani_vrstico(int vrstica, int status);

    void on_btn_osvezi_clicked();
    void on_btn_kopiraj_clicked();
    void on_cb_leto_currentIndexChanged();

    void on_tbl_stroski_prehrane_itemChanged(QTableWidgetItem *celica);

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_v_double(QString besedilo);
    QString pretvori_iz_double(QString besedilo);

private:
    Ui::wid_stroskiprehrane *ui;
};

#endif // WID_STROSKIPREHRANE_H

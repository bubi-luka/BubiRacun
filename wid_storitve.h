#ifndef WID_STORITVE_H
#define WID_STORITVE_H

#include <QWidget>

namespace Ui {
class wid_storitve;
}

class wid_storitve : public QWidget
{
    Q_OBJECT

public:
    explicit wid_storitve(QWidget *parent = 0);
    ~wid_storitve();


private slots:
    void napolni_kategorije();
    void napolni_podkategorije();
    void napolni_aktivnosti();
    void napolni();

    void on_cb_kategorija_currentIndexChanged();
    void on_cb_podkategorija_currentIndexChanged();
    void on_cb_aktivnost_currentIndexChanged();

    void on_btn_pobrisi_filtre_clicked();

/*
    void on_tbl_storitve_doubleClicked();
    void on_btn_osvezi_clicked();
   */ void on_btn_nov_clicked();/*
    void on_btn_aktiviraj_clicked();

    void osvezi(QString beseda);
    void prejem(QString besedilo);
    void tip(QString besedilo);*/

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_v_double(QString besedilo);
    QString pretvori_iz_double(QString besedilo);

signals:
    void prenos(QString beseda);/*
    void prenesi();
    void tiskaj(QString vrsta, QString stevilke, QString format);
*/
private:
    Ui::wid_storitve *ui;
};

#endif // WID_STORITVE_H

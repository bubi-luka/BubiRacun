#ifndef WID_OSNOVNI_POGLED_H
#define WID_OSNOVNI_POGLED_H

#include <QWidget>

namespace Ui {
class wid_osnovni_pogled;
}

class wid_osnovni_pogled : public QWidget
{
    Q_OBJECT

public:
    explicit wid_osnovni_pogled(QWidget *parent = 0);
    ~wid_osnovni_pogled();

private slots:
    void napolni_projekte();
    void napolni_stranke();
    void shrani_prioriteto();
    void napolni_potne_naloge();
    void napolni_prejete_racune();
    void napolni_izdane_racune();
    void napolni_predracune();

    void on_tbl_stranke_doubleClicked();
    void on_tbl_potni_nalogi_doubleClicked();
    void on_tbl_prejeti_racuni_doubleClicked();
    void on_tbl_predracun_doubleClicked();

    void on_btn_prioriteta_gor_clicked();
    void on_btn_prioriteta_dol_clicked();
    void on_btn_osvezi_stranke_clicked();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

    void osvezi(QString beseda);

signals:
    void prenos(QString beseda);

private:
    Ui::wid_osnovni_pogled *ui;
};

#endif // WID_OSNOVNI_POGLED_H

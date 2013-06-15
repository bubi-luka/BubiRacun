#ifndef PRIJAVA_H
#define PRIJAVA_H

#include <QDialog>

#include "varnost.h"

namespace Ui {
    class prijava;
}

class prijava : public QDialog
{
    Q_OBJECT

public:
    explicit prijava(QWidget *parent = 0);
    ~prijava();

private slots:
    void on_btn_izhod_clicked();
    void on_btn_brisi_clicked();
    void on_btn_prijavi_clicked();
    void keyPressEvent(QKeyEvent *event);

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

    // ustvari varnostno kopijo
    void varnostna_kopija();
    void glavna_tabela();

    // ustvari tabele
    void tabela_podjetje();
    void tabela_uporabnik();
    void tabela_potni_nalogi();
    void tabela_potovanja();
    void tabela_stroski();
    void tabela_prejeti_racuni();
    void tabela_stranke();
    void tabela_projekti();
    void tabela_racuni();
    void tabela_opravila();
    void tabela_opombe();
    void tabela_nastavitve();
    void tabela_avtomobili();
    void tabela_stroski_prehrane();

    // ustvari tabele sifrantov
    void tabela_skd();
    void tabela_posta();
    void tabela_nazivi();
    void tabela_pogodbe();
    void tabela_dovoljenja();
    void tabela_status_projekta();
    void tabela_status_racuna();
    void tabela_status_placila();
    void tabela_status_racunovodstva();
    void tabela_popusti();
    void tabela_viri_strank();
    void tabela_namen_potnega_naloga();
    void tabela_prevoz();
    void tabela_predracuni();
    void tabela_storitev();
    void tabela_oddaje_racuna();
    void tabela_opombe_pri_racunih();
    void tabela_dnevnice();
    void tabela_kilometrina();
    void tabela_cenamalice();
    void tabela_banke();
    void tabela_koda_namena();

    // vnese podatke v tabele
    void vnesi_skd();
    void vnesi_posta();
    void vnesi_nazive();
    void vnesi_pogodbe();
    void vnesi_dovoljenja();
    void vnesi_status_projekta();
    void vnesi_status_racuna();
    void vnesi_status_placila();
    void vnesi_status_racunovodstva();
    void vnesi_popuste();
    void vnesi_vire_strank();
    void vnesi_namen_potnega_naloga();
    void vnesi_prevoz();
    void vnesi_predracune();
    void vnesi_storitve();
    void vnesi_oddaja_racuna();
    void vnesi_nastavitve();
    void vnesi_banke();
    void vnesi_koda_namena();
    void vnesi_stroski_prehrane();

    // posodobitev baze
    void posodobi_bazo();

signals:
    void prenos1(QString besedilo);
    void prenos2(QString besedilo);
    void secret(QString pass);
    void obvestilo(QString obvestilo);

private:
    Ui::prijava *ui;
};

#endif // PRIJAVA_H

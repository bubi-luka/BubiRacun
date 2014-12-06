#ifndef PREJETIRACUNI_DODAJDDV_H
#define PREJETIRACUNI_DODAJDDV_H

#include <QDialog>

namespace Ui {
class prejetiracuni_dodajddv;
}

class prejetiracuni_dodajddv : public QDialog
{
    Q_OBJECT

public:
    explicit prejetiracuni_dodajddv(QWidget *parent = 0);
    ~prejetiracuni_dodajddv();

private slots:
    void on_btn_izhod_clicked();
    void on_btn_sprejmi_clicked();

    void keyPressEvent(QKeyEvent *event);

    void on_rb_aktivni_toggled();
    void on_rb_vsi_toggled();

    void on_tbl_ddv_doubleClicked();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_iz_double(QString besedilo);
    QString pretvori_v_double(QString besedilo);

    void napolni();

    void prejem(QString besedilo);

signals:
    void poslji(QString beseda);

private:
    Ui::prejetiracuni_dodajddv *ui;
};

#endif // PREJETIRACUNI_DODAJDDV_H

#ifndef SIF_DDV_H
#define SIF_DDV_H

#include <QDialog>

namespace Ui {
class sif_ddv;
}

class sif_ddv : public QDialog
{
    Q_OBJECT

public:
    explicit sif_ddv(QWidget *parent = 0);
    ~sif_ddv();

private slots:
    void on_btn_izhod_clicked();
    void on_btn_sprejmi_clicked();

    void keyPressEvent(QKeyEvent *event);

    void on_rb_aktiven_toggled();
    void on_rb_neaktiven_toggled();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_iz_double(QString besedilo);
    QString pretvori_v_double(QString besedilo);

    void prejem(QString besedilo);

signals:
    void poslji(QString beseda);

private:
    Ui::sif_ddv *ui;
};

#endif // SIF_DDV_H

#ifndef SIF_ENOTE_H
#define SIF_ENOTE_H

#include <QDialog>

namespace Ui {
class sif_enote;
}

class sif_enote : public QDialog
{
    Q_OBJECT

public:
    explicit sif_enote(QWidget *parent = 0);
    ~sif_enote();

public slots:
    void on_btn_vnesi_clicked();
    void on_btn_izbrisi_clicked();
    void on_btn_izhod_clicked();

    void on_rb_vsi_toggled();
    void on_rb_aktivni_toggled();

    void on_tbl_enote_itemSelectionChanged();

    void napolni_polja();
    void osvezi_tabelo();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

private:
    Ui::sif_enote *ui;
};

#endif // SIF_ENOTE_H

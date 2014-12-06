#ifndef PODKATEGORIJE_H
#define PODKATEGORIJE_H

#include <QDialog>

namespace Ui {
class podkategorije;
}

class podkategorije : public QDialog
{
    Q_OBJECT

public:
    explicit podkategorije(QWidget *parent = 0);
    ~podkategorije();

private slots:
    void napolni_tabelo_podkategorij();
    void napolni_kategorije();

    void on_btn_izhod_clicked();
    void on_btn_shrani_clicked();
    void on_btn_zbrisi_clicked();

    void on_rb_vsi_toggled();
    void on_rb_aktivni_toggled();
    void on_cb_filter_kategorije_currentIndexChanged();

    void on_tbl_podkategorije_itemSelectionChanged();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

private:
    Ui::podkategorije *ui;
};

#endif // PODKATEGORIJE_H

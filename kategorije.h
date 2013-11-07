#ifndef KATEGORIJE_H
#define KATEGORIJE_H

#include <QDialog>

namespace Ui {
class kategorije;
}

class kategorije : public QDialog
{
    Q_OBJECT

public:
    explicit kategorije(QWidget *parent = 0);
    ~kategorije();

private slots:
    void napolni_tabelo_kategorij();

    void on_btn_izhod_clicked();
    void on_btn_shrani_clicked();
    void on_btn_zbrisi_clicked();

    void on_rb_vsi_toggled();
    void on_rb_aktivni_toggled();

    void on_tbl_kategorije_itemSelectionChanged();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

private:
    Ui::kategorije *ui;
};

#endif // KATEGORIJE_H

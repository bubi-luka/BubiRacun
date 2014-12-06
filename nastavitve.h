#ifndef NASTAVITVE_H
#define NASTAVITVE_H

#include <QDialog>

namespace Ui {
class nastavitve;
}

class nastavitve : public QDialog
{
    Q_OBJECT

public:
    explicit nastavitve(QWidget *parent = 0);
    ~nastavitve();

private slots:
    void on_btn_pot_clicked();
    void on_btn_popusti_clicked();
    void on_btn_ponastavi_clicked();
    void on_btn_izhod_clicked();
    void on_btn_shrani_clicked();
    void on_btn_import_clicked();
    void on_btn_export_clicked();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

private:
    Ui::nastavitve *ui;
};

#endif // NASTAVITVE_H

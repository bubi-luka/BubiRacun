#ifndef SIF_CENAMALICE_H
#define SIF_CENAMALICE_H

#include <QDialog>

namespace Ui {
class sif_cenamalice;
}

class sif_cenamalice : public QDialog
{
    Q_OBJECT
    
public:
    explicit sif_cenamalice(QWidget *parent = 0);
    ~sif_cenamalice();

private slots:
    void on_btn_izhod_clicked();
    void on_btn_sprejmi_clicked();

    void on_txt_cena_textChanged();

    void keyPressEvent(QKeyEvent *event);

    QString pretvori_v_double(QString besedilo);
    QString pretvori_iz_double(QString besedilo);
    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    void prejem();

private:
    Ui::sif_cenamalice *ui;
};

#endif // SIF_CENAMALICE_H

#ifndef SIF_DELAVNIKI_H
#define SIF_DELAVNIKI_H

#include <QDialog>
#include <QNetworkReply>

namespace Ui {
class sif_delavniki;
}

class sif_delavniki : public QDialog
{
    Q_OBJECT

public:
    explicit sif_delavniki(QWidget *parent = 0);
    ~sif_delavniki();


private slots:
    void on_btn_izhod_clicked();
    void on_btn_osvezi_clicked();
    void on_btn_brisi_clicked();
    void konec_odziva(QNetworkReply *odgovor);
    void napolni_leta();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

private:
    Ui::sif_delavniki *ui;
};

#endif // SIF_DELAVNIKI_H

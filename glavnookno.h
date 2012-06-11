#ifndef GLAVNOOKNO_H
#define GLAVNOOKNO_H

#include <QMainWindow>

namespace Ui {
    class GlavnoOkno;
}

class GlavnoOkno : public QMainWindow
{
    Q_OBJECT

public:
    explicit GlavnoOkno(QWidget *parent = 0);
    ~GlavnoOkno();

private slots:
    void sekundnik();
    void on_actionUporabniki_triggered();
    void on_actionPodjetje_triggered();
    void on_actionPotni_nalogi_triggered();
    void on_actionPrejeti_ra_uni_triggered();
    void on_actionStranke_triggered();
    void on_actionProjekti_triggered();
    void on_actionKuponi_triggered();
    void on_actionVizitka_triggered();
    void on_actionIzdani_ra_uni_triggered();
    void on_actionPopusti_triggered();
    void on_action_asovnice_triggered();
    void on_actionOpombe_pri_ra_unih_triggered();
    void on_actionKilometrina_triggered();
    void on_actionDnevnice_triggered();
    void on_actionPrihodek_triggered();
    void on_actionIzdatek_triggered();
    void on_actionPoslovanje_triggered();
    void varnost_id_changed();
    void on_actionOsnovni_Pogled_2_triggered();
    void keyPressEvent(QKeyEvent *event);

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

signals:
    void prenos(QString beseda);

private:
    Ui::GlavnoOkno *ui;
};

#endif // GLAVNOOKNO_H

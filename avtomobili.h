#ifndef AVTOMOBILI_H
#define AVTOMOBILI_H

#include <QDialog>

namespace Ui {
class avtomobili;
}

class avtomobili : public QDialog
{
    Q_OBJECT

public:
    explicit avtomobili(QWidget *parent = 0);
    ~avtomobili();

private slots:
    void on_btn_izhod_clicked();
    void on_btn_izbrisi_clicked();
    void on_btn_shrani_clicked();

    void on_rb_podjetje_toggled();
    void on_rb_zasebnik_toggled();

    void napolni_lastnike();

    void prejem(QString beseda);

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

signals:
    void poslji(QString beseda);

private:
    Ui::avtomobili *ui;
};

#endif // AVTOMOBILI_H

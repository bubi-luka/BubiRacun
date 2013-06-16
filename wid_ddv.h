#ifndef WID_DDV_H
#define WID_DDV_H

#include <QWidget>

namespace Ui {
class wid_ddv;
}

class wid_ddv : public QWidget
{
    Q_OBJECT

public:
    explicit wid_ddv(QWidget *parent = 0);
    ~wid_ddv();

private slots:
    void on_btn_nov_clicked();
    void on_tbl_ddv_doubleClicked();

    void on_rb_aktivni_toggled();
    void on_rb_vsi_toggled();

    void osvezi(QString beseda);
    void napolni();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);
    QString pretvori_v_double(QString besedilo);
    QString pretvori_iz_double(QString besedilo);

signals:
    void prenos(QString beseda);
    void prenesi();
    void tiskaj(QString vrsta, QString stevilke, QString format);

private:
    Ui::wid_ddv *ui;
};

#endif // WID_DDV_H

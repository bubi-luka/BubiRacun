#ifndef WID_UPORABNIK_H
#define WID_UPORABNIK_H

#include <QWidget>

namespace Ui {
    class wid_uporabnik;
}

class wid_uporabnik : public QWidget
{
    Q_OBJECT

public:
    explicit wid_uporabnik(QWidget *parent = 0);
    ~wid_uporabnik();

private slots:
    void on_cb_mesto_currentIndexChanged();
    void on_cb_pogodba_currentIndexChanged();

    void on_btn_brisi_clicked();
    void on_btn_nov_clicked();
    void on_tbl_uporabnik_doubleClicked();

    void osvezi(QString beseda);
    void napolni();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

signals:
    void prenos(QString beseda);

private:
    Ui::wid_uporabnik *ui;
};

#endif // WID_UPORABNIK_H

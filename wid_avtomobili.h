#ifndef WID_AVTOMOBILI_H
#define WID_AVTOMOBILI_H

#include <QWidget>

namespace Ui {
class wid_avtomobili;
}

class wid_avtomobili : public QWidget
{
    Q_OBJECT

public:
    explicit wid_avtomobili(QWidget *parent = 0);
    ~wid_avtomobili();

private slots:
    void on_btn_brisi_clicked();
    void on_btn_nov_clicked();
    void on_btn_refresh_clicked();

    void on_tbl_avtomobili_doubleClicked();

    void osvezi(QString beseda);
    void napolni();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

signals:
    void prenos(QString beseda);

private:
    Ui::wid_avtomobili *ui;
};

#endif // WID_AVTOMOBILI_H

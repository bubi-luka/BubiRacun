#ifndef WID_STRANKE_H
#define WID_STRANKE_H

#include <QWidget>

namespace Ui {
    class wid_stranke;
}

class wid_stranke : public QWidget
{
    Q_OBJECT

public:
    explicit wid_stranke(QWidget *parent = 0);
    ~wid_stranke();

private slots:
	void on_tbl_stranke_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();
	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos(QString beseda);

private:
    Ui::wid_stranke *ui;
};

#endif // WID_STRANKE_H

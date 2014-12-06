#ifndef WID_PODJETJE_H
#define WID_PODJETJE_H

#include <QWidget>

namespace Ui {
    class wid_podjetje;
}

class wid_podjetje : public QWidget
{
    Q_OBJECT

public:
    explicit wid_podjetje(QWidget *parent = 0);
    ~wid_podjetje();

private slots:
	void on_btn_brisi_clicked();
	void on_btn_nov_clicked();
	void on_tbl_podjetje_doubleClicked();
	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos(QString beseda);

private:
    Ui::wid_podjetje *ui;
};

#endif // WID_PODJETJE_H

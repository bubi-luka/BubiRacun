#ifndef WID_POTOVANJA_H
#define WID_POTOVANJA_H

#include <QWidget>

namespace Ui {
    class wid_potovanja;
}

class wid_potovanja : public QWidget
{
    Q_OBJECT

public:
    explicit wid_potovanja(QWidget *parent = 0);
    ~wid_potovanja();

private slots:
	void on_btn_brisi_clicked();
	void on_btn_nov_clicked();
	void on_tbl_potovanja_doubleClicked();
	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void prejem(QString beseda);

signals:
	void prenos(QString beseda);
	void prenesi();

private:
    Ui::wid_potovanja *ui;
};

#endif // WID_POTOVANJA_H

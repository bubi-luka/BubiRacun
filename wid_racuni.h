#ifndef WID_RACUNI_H
#define WID_RACUNI_H

#include <QWidget>

namespace Ui {
    class wid_racuni;
}

class wid_racuni : public QWidget
{
    Q_OBJECT

public:
    explicit wid_racuni(QWidget *parent = 0);
    ~wid_racuni();

private slots:
	void on_tbl_racuni_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();
	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void prejem(QString besedilo);

signals:
	void prenos(QString beseda);
	void prenesi();

private:
    Ui::wid_racuni *ui;
};

#endif // WID_RACUNI_H

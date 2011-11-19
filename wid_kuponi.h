#ifndef WID_KUPONI_H
#define WID_KUPONI_H

#include <QWidget>

namespace Ui {
    class wid_kuponi;
}

class wid_kuponi : public QWidget
{
    Q_OBJECT

public:
    explicit wid_kuponi(QWidget *parent = 0);
    ~wid_kuponi();

private slots:
	void on_tbl_kuponi_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();
	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos(QString beseda);

private:
    Ui::wid_kuponi *ui;
};

#endif // WID_KUPONI_H

#ifndef WID_PREJETIRACUNI_H
#define WID_PREJETIRACUNI_H

#include <QWidget>

namespace Ui {
    class wid_prejetiracuni;
}

class wid_prejetiracuni : public QWidget
{
    Q_OBJECT

public:
    explicit wid_prejetiracuni(QWidget *parent = 0);
    ~wid_prejetiracuni();

private slots:
	void on_btn_refresh_clicked();
	void on_btn_print_clicked();
	void on_btn_print_pdf_clicked();
	void on_btn_print_seznam_clicked();

	void on_cb_mesec_currentIndexChanged();
	void on_cb_leto_currentIndexChanged();
	void on_cb_izdajatelj_currentIndexChanged();
	void on_cb_placilo_currentIndexChanged();
	void on_cb_racunovodstvo_currentIndexChanged();

	void on_tbl_racuni_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();

	void osvezi(QString beseda);
	void prejem(QString besedilo);
	void napolni();

	void print(QString id);
	void printpdf(QString id);

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos(QString beseda);
	void prenesi();

private:
    Ui::wid_prejetiracuni *ui;
};

#endif // WID_PREJETIRACUNI_H

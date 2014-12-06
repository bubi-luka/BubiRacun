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
	void on_btn_refresh_clicked();
	void on_btn_print_clicked();
	void on_btn_print_pdf_clicked();
	void on_btn_print_seznam_clicked();

	void on_cb_racun_currentIndexChanged(int indeks);
	void on_cb_mesec_currentIndexChanged(int indeks);
	void on_cb_leto_currentIndexChanged(int indeks);
	void on_cb_placilo_currentIndexChanged(int indeks);
	void on_cb_racunovodstvo_currentIndexChanged(int indeks);

	void on_tbl_racuni_doubleClicked();
	void on_tbl_racuni_itemSelectionChanged();
	void on_tbl_sorodniki_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();
	void on_btn_kopiraj_clicked();
	void on_btn_storno_clicked();
	void on_btn_dobropis_clicked();

	void osvezi(QString beseda);
	void prejem(QString besedilo);
	void tip(QString besedilo);
	void napolni();
	void napolni_sorodnike();
    QString stevilka_racuna(QString tip);

	void on_btn_prestevilci_clicked();

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

signals:
	void prenos(QString beseda);
	void prenesi();
	void tiskaj(QString vrsta, QString stevilke, QString format);

private:
	Ui::wid_racuni *ui;
};

#endif // WID_RACUNI_H

#ifndef WID_POTNINALOGI_H
#define WID_POTNINALOGI_H

#include <QWidget>

namespace Ui {
    class wid_potninalogi;
}

class wid_potninalogi : public QWidget
{
    Q_OBJECT

public:
    explicit wid_potninalogi(QWidget *parent = 0);
    ~wid_potninalogi();

private slots:
	void on_btn_refresh_clicked();
	void on_btn_print_clicked();
	void on_btn_print_pdf_clicked();

	void on_cb_mesec_currentIndexChanged();
	void on_cb_leto_currentIndexChanged();
	void on_cb_namen_currentIndexChanged();
	void on_cb_stranka_currentIndexChanged();
	void on_cb_kraj_currentIndexChanged();
	void on_cb_prevoz_currentIndexChanged();

	void on_tbl_potninalogi_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();

	void osvezi(QString beseda);
	void prejem(QString besedilo);
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
    Ui::wid_potninalogi *ui;
};

#endif // WID_POTNINALOGI_H

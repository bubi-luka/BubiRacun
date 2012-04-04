#ifndef WID_PRIHODKI_H
#define WID_PRIHODKI_H

#include <QWidget>

namespace Ui {
    class wid_prihodki;
}

class wid_prihodki : public QWidget
{
    Q_OBJECT

public:
    explicit wid_prihodki(QWidget *parent = 0);
    ~wid_prihodki();

private slots:
	// mesecni pregled
	void on_txt_leto_currentIndexChanged();
	void on_txt_mesec_currentIndexChanged();

	void on_txt_sklop_currentIndexChanged();
	void on_txt_skupina_currentIndexChanged();
	void on_txt_storitev_currentIndexChanged();

	void on_cb_vsa_opravila_toggled();

	// letni pregled
	void on_txt_leto_3_currentIndexChanged();

	void on_txt_sklop_3_currentIndexChanged();
	void on_txt_skupina_3_currentIndexChanged();
	void on_txt_storitev_3_currentIndexChanged();

	void on_cb_vsa_opravila_3_toggled();

	// skupni pregled
	void on_txt_sklop_4_currentIndexChanged();
	void on_txt_skupina_4_currentIndexChanged();
	void on_txt_storitev_4_currentIndexChanged();

	void on_cb_vsa_opravila_4_toggled();

	// napolni mesecni pregled
	void napolni_mesec();
	void napolni_mesec_tabela();

	// napolni letni pregled
	void napolni_letni();
	void napolni_letni_tabela();

	// napolni skupni pregled
	void napolni_skupni();
	void napolni_skupni_tabela();

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

private:
    Ui::wid_prihodki *ui;
};

#endif // WID_PRIHODKI_H

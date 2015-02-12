#ifndef WID_CASOVNICE_H
#define WID_CASOVNICE_H

#include <QWidget>

namespace Ui {
	class wid_casovnice;
}

class wid_casovnice : public QWidget
{
	Q_OBJECT

public:
	explicit wid_casovnice(QWidget *parent = 0);
	~wid_casovnice();

public slots:
	void on_cb_aktivnost_toggled();
	void on_txt_leto_currentIndexChanged();
	void on_txt_mesec_currentIndexChanged();
	void on_txt_avtor_currentIndexChanged();
	void on_txt_stranka_currentIndexChanged();
	void on_txt_projekt_currentIndexChanged();
	void on_txt_racun_currentIndexChanged();

	void napolni_sezname();
	void napolni();
	void preracunaj_casovnice(QString opravilo);

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

private:
	Ui::wid_casovnice *ui;
};

#endif // WID_CASOVNICE_H

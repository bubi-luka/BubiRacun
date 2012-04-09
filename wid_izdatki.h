#ifndef WID_IZDATKI_H
#define WID_IZDATKI_H

#include <QWidget>

namespace Ui {
    class wid_izdatki;
}

class wid_izdatki : public QWidget
{
    Q_OBJECT

public:
    explicit wid_izdatki(QWidget *parent = 0);
    ~wid_izdatki();

private slots:
	// napolni mesecne podatke
	void on_txt_leto_currentIndexChanged();
	void on_txt_mesec_currentIndexChanged();

	// mesec
	void napolni_izbrani_mesec();
	void napolni_trenutni_mesec();

	// leto
	void napolni_leto();

	// skupaj
	void napolni_izbrani_skupni();
	void napolni_trenutni_skupni();

	// splosno
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

private:
    Ui::wid_izdatki *ui;
};

#endif // WID_IZDATKI_H

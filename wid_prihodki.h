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

	// letni pregled
    void on_txt_leto_3_currentIndexChanged();

	// napolni mesecni pregled
    void napolni_mesec();

	// napolni letni pregled
    void napolni_letni();

	// napolni skupni pregled
    void napolni_skupni();

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

private:
    Ui::wid_prihodki *ui;
};

#endif // WID_PRIHODKI_H

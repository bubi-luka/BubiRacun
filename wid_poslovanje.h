#ifndef WID_POSLOVANJE_H
#define WID_POSLOVANJE_H

#include <QWidget>

namespace Ui {
    class wid_poslovanje;
}

class wid_poslovanje : public QWidget
{
    Q_OBJECT

public:
    explicit wid_poslovanje(QWidget *parent = 0);
    ~wid_poslovanje();

private slots:
	// napolni mesecne podatke
	void on_txt_leto_currentIndexChanged();

	// mesec
	void napolni_mesec();

	// leto
	void napolni_leto();

	// skupaj
	void napolni_skupni();

	// splosno
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

private:
    Ui::wid_poslovanje *ui;
};

#endif // WID_POSLOVANJE_H

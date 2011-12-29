#ifndef PROJEKTI_H
#define PROJEKTI_H

#include <QDialog>

namespace Ui {
    class projekti;
}

class projekti : public QDialog
{
    Q_OBJECT

public:
    explicit projekti(QWidget *parent = 0);
    ~projekti();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();

	void on_txt_stranka_currentIndexChanged(QString besedilo);

	void on_txt_stranka_id_textChanged(QString besedilo);

	void keyPressEvent(QKeyEvent *event);

	void on_cb_popust_fb1_toggled(bool stanje);
	void on_cb_popust_fb2_toggled(bool stanje);
	void on_cb_popust_komb1_toggled(bool stanje);
	void on_cb_popust_komb2_toggled(bool stanje);
	void on_cb_popust_stalna_toggled(bool stanje);
	void on_cb_popust_kupon_toggled(bool stanje);
	void on_cb_popust_akcija_toggled(bool stanje);

	void on_cb_podrazitev_vikend_toggled(bool stanje);
	void on_cb_podrazitev_hitrost_toggled(bool stanje);
	void on_cb_podrazitev_zapleti_toggled(bool stanje);

	void on_txt_popust_fb1_textChanged(QString besedilo);
	void on_txt_popust_fb2_textChanged(QString besedilo);
	void on_txt_popust_komb1_textChanged(QString besedilo);
	void on_txt_popust_komb2_textChanged(QString besedilo);
	void on_txt_popust_stalna_stranka_textChanged(QString besedilo);
	void on_txt_popust_kupon_textChanged(QString besedilo);

	void izracunaj_popuste(double vrednost, int polje);
	void izracunaj();

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	void prejem(QString besedilo);
	void napolni_podatke();

signals:
	void poslji(QString beseda);
	void prenos(QString beseda);

private:
    Ui::projekti *ui;
};

#endif // PROJEKTI_H

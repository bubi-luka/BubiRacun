#ifndef STRANKE_H
#define STRANKE_H

#include <QDialog>

namespace Ui {
    class stranke;
}

class stranke : public QDialog
{
    Q_OBJECT

public:
    explicit stranke(QWidget *parent = 0);
    ~stranke();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_vnesi_clicked();
	void on_btn_novkupon_clicked();
	void on_btn_novprojekt_clicked();

	void on_rb_fizicna_toggled(bool stanje);
	void on_rb_stalna_toggled(bool stanje);
	void on_txt_vir_currentIndexChanged(int stanje);
	void on_txt_posta_currentIndexChanged(QString besedilo);
	void on_txt_postna_stevilka_textChanged(QString besedilo);
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

	void on_txt_popust_fb1_editingFinished();
	void on_txt_popust_fb2_editingFinished();
	void on_txt_popust_komb1_editingFinished();
	void on_txt_popust_komb2_editingFinished();
	void on_txt_popust_stalna_stranka_editingFinished();
	void on_txt_popust_kupon_editingFinished();
	void on_txt_popust_akcija_editingFinished();
	void on_txt_podrazitev_vikend_editingFinished();
	void on_txt_podrazitev_hitrost_editingFinished();
	void on_txt_podrazitev_zapleti_editingFinished();

	void on_cb_mesec_currentIndexChanged();
	void on_cb_leto_currentIndexChanged();
	void on_cb_projekt_currentIndexChanged();

    void on_txt_banka_currentIndexChanged();

	void on_tbl_projekti_doubleClicked();
	void on_tbl_kuponi_doubleClicked();

	void prejem(QString besedilo);
	void osvezi(QString beseda);

	void izracunaj_popuste(double vrednost, int polje);

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	void napolni_projekte();
	void napolni_kupone();

signals:
	void prenos(QString beseda);
	void poslji(QString beseda);

private:
    Ui::stranke *ui;
};

#endif // STRANKE_H

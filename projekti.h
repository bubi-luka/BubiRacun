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

	void on_txt_pricetek_dateChanged();

	void on_txt_stranka_currentIndexChanged(QString besedilo);
	void on_txt_stranka_id_textChanged(QString besedilo);
	void on_txt_status_projekta_currentIndexChanged();

	void keyPressEvent(QKeyEvent *event);

	void on_cb_popust_fb1_toggled(bool stanje);
	void on_cb_popust_fb2_toggled(bool stanje);
	void on_cb_popust_komb1_toggled(bool stanje);
	void on_cb_popust_komb2_toggled(bool stanje);
	void on_cb_popust_stalna_toggled(bool stanje);
	void on_cb_popust_akcija_toggled(bool stanje);

	void on_cb_podrazitev_vikend_toggled(bool stanje);
	void on_cb_podrazitev_hitrost_toggled(bool stanje);
	void on_cb_podrazitev_zapleti_toggled(bool stanje);

	void on_txt_popust_fb1_textChanged();
	void on_txt_popust_fb2_textChanged();
	void on_txt_popust_komb1_textChanged();
	void on_txt_popust_komb2_textChanged();
	void on_txt_popust_stalna_stranka_textChanged();

	void on_txt_popust_fb1_editingFinished();
	void on_txt_popust_fb2_editingFinished();
	void on_txt_popust_komb1_editingFinished();
	void on_txt_popust_komb2_editingFinished();
	void on_txt_popust_stalna_stranka_editingFinished();
	void on_txt_popust_akcija_editingFinished();
	void on_txt_podrazitev_vikend_editingFinished();
	void on_txt_podrazitev_hitrost_editingFinished();
	void on_txt_podrazitev_zapleti_editingFinished();

	void izracunaj_popuste(int polje);
	void izracunaj();
	void stevilka_racuna();

	void on_btn_vnesi_zapis_clicked();
	void on_btn_pocisti_clicked();
	void on_tbl_zapisi_clicked();
	void on_btn_izbris_zapisa_clicked();
	void napolni_zapise();

	void napolni_casovnice();
	void napolni_casovnice_tabelo();
	void on_gb_trajanje_od_do_toggled();
	void on_gb_trajanje_skupaj_toggled();
	void on_txt_cas_racun_1_currentIndexChanged();
	void on_txt_cas_racun_2_currentIndexChanged();
	void on_txt_cas_opravilo_1_currentIndexChanged();
	void on_txt_cas_opravilo_2_currentIndexChanged();
	void on_txt_cas_od_timeChanged();
	void on_txt_cas_do_timeChanged();
	void on_btn_pocisti_casovnico_clicked();
	void on_btn_vnesi_casovnico_clicked();
	void osvezi_opravilo(QString opravilo);
	void on_btn_brisi_casovnico_clicked();

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

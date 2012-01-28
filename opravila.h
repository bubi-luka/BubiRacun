#ifndef OPRAVILA_H
#define OPRAVILA_H

#include <QObject>
#include <QDialog>

namespace Ui {
    class opravila;
}

class opravila : public QDialog
{
    Q_OBJECT

public:
    explicit opravila(QWidget *parent = 0);
    ~opravila();

private slots:
	void keyPressEvent(QKeyEvent *event);

	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();

	void on_btn_izracunaj_clicked();

	void on_txt_skupina_currentIndexChanged();
	void on_txt_storitev_currentIndexChanged();

	void on_txt_urna_postavka_brez_ddv_textChanged();
	void on_txt_ddv_currentIndexChanged();

	void on_txt_urna_postavka_textChanged();
	void on_cb_vikend_toggled();
	void on_cb_hitrost_toggled();
	void on_cb_zapleti_toggled();

	void on_txt_id_stranka_textChanged();
	void on_txt_id_projekt_textChanged();
	void on_txt_id_racun_textChanged();
	void on_txt_id_tip_textChanged();

	void on_rb_predracun_toggled();
	void on_rb_casovnice_toggled();
	void on_rb_rocni_vnos_toggled();
	void on_txt_predracun_textChanged();
	void on_txt_casovnice_textChanged();
	void on_txt_rocni_vnos_textChanged();

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

	void izracunaj_popuste(double vrednost, int polje);

	void izracunaj_urno_postavko_z_ddv();
	void napolni_polja();
	void izracunaj_racun();

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	void prejem(QString beseda);

signals:
//	void prenos(QString beseda);
	void poslji(QString beseda);

private:
    Ui::opravila *ui;
};

#endif // OPRAVILA_H

#ifndef RACUN_H
#define RACUN_H

#include <QDialog>

namespace Ui {
    class racun;
}

class racun : public QDialog
{
    Q_OBJECT

public:
    explicit racun(QWidget *parent = 0);
    ~racun();
private slots:
	void on_btn_izhod_clicked();
	void on_btn_izpisi_clicked();
	void on_btn_sprejmi_clicked();
	void on_btn_brisi_opravilo_clicked();
	void on_btn_opravilo_clicked();
	void on_btn_izracunaj_clicked();
	void on_btn_racun_clicked();
	void on_btn_predplacilni_racun_clicked();
	void on_tbl_opravila_doubleClicked();

	void on_txt_stranka_currentIndexChanged();
	void on_txt_stranka_id_textChanged();
	void on_txt_projekt_currentIndexChanged();
	void on_txt_projekt_id_textChanged();

	void on_txt_odstotek_avansa_editingFinished();
	void on_txt_datum_izdaje_racuna_dateChanged();
	void on_txt_status_oddaje_racuna_currentIndexChanged();

	void on_rb_predracun_toggled();
	void on_rb_predplacilo_toggled();
	void on_rb_racun_toggled();

	void on_btn_vnesi_zapis_2_clicked();
	void on_btn_pocisti_2_clicked();
	void on_tbl_zapisi_2_doubleClicked();
	void napolni_zapise();
	void on_cb_stara_stevilka_racuna_toggled();

	void on_txt_vse_opombe_textChanged();
	void on_txt_vnesene_opombe_textChanged();
	void on_tbl_vse_opombe_doubleClicked();
	void on_tbl_vnesene_opombe_doubleClicked();
	void napolni_vse_opombe();
	void napolni_vnesene_opombe();

	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

	void stevilka_racuna();
	void izracunaj();
	void napolni();
	void osvezi(QString beseda);

signals:
	void poslji(QString beseda);
	void prenos(QString beseda);
	void tiskaj(QString vrsta, QString stevilke, QString format);

private:
    Ui::racun *ui;
};

#endif // RACUN_H

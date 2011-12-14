#ifndef PREJETIRACUNI_H
#define PREJETIRACUNI_H

#include <QDialog>
#include <QDate>

namespace Ui {
    class prejetiracuni;
}

class prejetiracuni : public QDialog
{
    Q_OBJECT

public:
    explicit prejetiracuni(QWidget *parent = 0);
    ~prejetiracuni();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_izpisi_clicked();
	void on_btn_sprejmi_clicked();

	void keyPressEvent(QKeyEvent *event);

	void on_txt_znesek_brez_ddv_0_editingFinished();
	void on_txt_znesek_brez_ddv_85_editingFinished();
	void on_txt_znesek_brez_ddv_20_editingFinished();

	void on_txt_izberi_podjetje_currentIndexChanged(QString besedilo);
	void on_txt_izberi_osebo_currentIndexChanged(QString besedilo);
	void on_txt_izberi_projekt_currentIndexChanged(QString besedilo);

	void on_cb_podjetje_toggled(bool stanje);
	void on_cb_oseba_toggled(bool stanje);
	void on_cb_projekt_toggled(bool stanje);

	void on_txt_posta_currentIndexChanged(QString besedilo);
	void on_txt_postna_stevilka_textChanged(QString besedilo);

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori_v_double(QString besedilo);

	void prejem(QString besedilo);
	void izracunaj();

signals:
	void poslji(QString beseda);

private:
    Ui::prejetiracuni *ui;
};

#endif // PREJETIRACUNI_H

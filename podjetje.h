#ifndef PODJETJE_H
#define PODJETJE_H

#include <QDialog>

namespace Ui {
	class podjetje;
}

class podjetje : public QDialog
{
	Q_OBJECT

public:
	explicit podjetje(QWidget *parent = 0);
	~podjetje();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_potrdi_clicked();

	void on_btn_logotip_clicked();

	void on_txt_zavezanec_stateChanged();

	void on_txt_lastnik_currentIndexChanged(QString besedilo);
	void on_txt_kontaktna_currentIndexChanged(QString besedilo);
	void on_txt_odgovorna_currentIndexChanged(QString besedilo);

	void on_txt_skd_besedilo_currentIndexChanged();
	void on_txt_skd_textChanged(QString besedilo);

	void on_txt_banka_currentIndexChanged();

	void on_txt_posta_currentIndexChanged();
	void on_txt_postna_stevilka_textChanged(QString besedilo);

	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
	Ui::podjetje *ui;
};

#endif // PODJETJE_H

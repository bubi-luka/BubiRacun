#ifndef POPUSTI_H
#define POPUSTI_H

#include <QDialog>

namespace Ui {
	class popusti;
}

class popusti : public QDialog
{
	Q_OBJECT

public:
	explicit popusti(QWidget *parent = 0);
	~popusti();

private slots:
	void on_btn_zapri_clicked();
	void on_btn_vnesi_clicked();

	void on_txt_popust_fb1_editingFinished();
	void on_txt_popust_fb2_editingFinished();
	void on_txt_popust_priporocilo_editingFinished();
	void on_txt_popust_stalna_stranka_editingFinished();
	void on_txt_popust_akcija_editingFinished();
	void on_txt_vsi_popusti_facebook_editingFinished();
	void on_txt_popusti_skupaj_editingFinished();
	void on_txt_podrazitev_vikend_editingFinished();
	void on_txt_podrazitev_hitrost_editingFinished();
	void on_txt_podrazitev_zapleti_editingFinished();

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	void napolni();

private:
	Ui::popusti *ui;
};

#endif // POPUSTI_H

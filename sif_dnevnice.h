#ifndef SIF_DNEVNICE_H
#define SIF_DNEVNICE_H

#include <QDialog>

namespace Ui {
    class sif_dnevnice;
}

class sif_dnevnice : public QDialog
{
    Q_OBJECT

public:
    explicit sif_dnevnice(QWidget *parent = 0);
    ~sif_dnevnice();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_sprejmi_clicked();

	void on_txt_dnevnica_1_textChanged();
	void on_txt_dnevnica_2_textChanged();
	void on_txt_dnevnica_3_textChanged();

	void keyPressEvent(QKeyEvent *event);

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void prejem();

private:
    Ui::sif_dnevnice *ui;
};

#endif // SIF_DNEVNICE_H

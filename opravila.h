#ifndef OPRAVILA_H
#define OPRAVILA_H

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
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_vnesi_clicked();
	void on_btn_izracunaj_clicked();

	void on_cb_facebook_toggled(bool stanje);
	void on_cb_twitter_toggled(bool stanje);
	void on_cb_google_toggled(bool stanje);
	void on_cb_blog_toggled(bool stanje);
	void on_cb_forum_toggled(bool stanje);
	void on_cb_sfacebook_toggled(bool stanje);
	void on_cb_stwitter_toggled(bool stanje);
	void on_cb_skuponi_toggled(bool stanje);
	void on_cb_obrazec_toggled(bool stanje);
	void on_cb_kupon_toggled(bool stanje);
	void on_cb_akcija_toggled(bool stanje);
	void on_cb_vip_toggled(bool stanje);

	void on_cb_k_vikend_toggled(bool stanje);
	void on_cb_k_kratek_toggled(bool stanje);
	void on_cb_k_zahtevnost_toggled(bool stanje);
	void on_cb_k_neumnost_toggled(bool stanje);
	void on_cb_k_komunikacija_toggled(bool stanje);

	void on_txt_storitev_currentIndexChanged(QString besedilo);

	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
    Ui::opravila *ui;
};

#endif // OPRAVILA_H

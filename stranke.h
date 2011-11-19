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
	void on_btn_nov_clicked();
	void on_btn_novprojekt_clicked();
	void on_rb_fizicna_toggled(bool stanje);
	void on_rb_stalna_toggled(bool stanje);
	void on_txt_vir_currentIndexChanged(int stanje);
	void keyPressEvent(QKeyEvent *event);

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

	void on_tbl_projekti_doubleClicked();
	void on_tbl_kuponi_doubleClicked();

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void osvezi(QString beseda);

	void napolni_projekte();
	void napolni_kupone();

signals:
	void prenos(QString beseda);
	void poslji(QString beseda);

private:
    Ui::stranke *ui;
};

#endif // STRANKE_H

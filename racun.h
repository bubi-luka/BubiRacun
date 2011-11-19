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
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();
	void on_btn_brisi_opravilo_clicked();
	void on_btn_opravilo_clicked();
	void on_btn_izracunaj_clicked();
	void on_tbl_opravila_doubleClicked();

	void on_txt_stranka_currentIndexChanged(QString besedilo);
	void on_txt_strankaid_textChanged(QString besedilo);
	void on_txt_projekt_currentIndexChanged(QString besedilo);
	void on_txt_projektid_textChanged(QString besedilo);

	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void izracunaj();
	void napolni();
	void osvezi(QString beseda);

signals:
	void poslji(QString beseda);
	void prenos(QString beseda);

private:
    Ui::racun *ui;
};

#endif // RACUN_H

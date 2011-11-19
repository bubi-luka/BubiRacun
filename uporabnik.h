#ifndef UPORABNIK_H
#define UPORABNIK_H

#include <QDialog>

namespace Ui {
    class uporabnik;
}

class uporabnik : public QDialog
{
    Q_OBJECT

public:
    explicit uporabnik(QWidget *parent = 0);
    ~uporabnik();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();
	void on_btn_geslo_clicked();
	void keyPressEvent(QKeyEvent *event);
	void on_txt_zaposlen_toggled(bool stanje);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
    Ui::uporabnik *ui;
};

#endif // UPORABNIK_H

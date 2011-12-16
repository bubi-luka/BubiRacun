#ifndef POTNINALOGI_H
#define POTNINALOGI_H

#include <QDialog>

namespace Ui {
    class potninalogi;
}

class potninalogi : public QDialog
{
    Q_OBJECT

public:
    explicit potninalogi(QWidget *parent = 0);
    ~potninalogi();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_izvozi_clicked();
	void on_cb_predlagatelj_podjetje_toggled();
	void on_txt_predlagatelj_izbira_podjetje_currentIndexChanged();
	void on_cb_predlagatelj_oseba_toggled();
	void on_txt_predlagatelj_izbira_oseba_currentIndexChanged();
	void on_txt_prejemnik_izbira_osebe_currentIndexChanged();
	void on_txt_prevoz_currentIndexChanged(int index);
	void on_btn_sprejmi_clicked();
	void keyPressEvent(QKeyEvent *event);

	void on_txt_priznajdnevnico_toggled();
	void on_txt_zajtrk2_toggled();
	void on_txt_zajtrk3_toggled();

	void prejem(QString besedilo);
	void sprejmist();
	void sprejmipot();
	void izracun();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);
	void prenos(QString beseda);

private:
    Ui::potninalogi *ui;
};

#endif // POTNINALOGI_H

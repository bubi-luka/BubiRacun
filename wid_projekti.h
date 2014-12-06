#ifndef WID_PROJEKTI_H
#define WID_PROJEKTI_H

#include <QWidget>

namespace Ui {
    class wid_projekti;
}

class wid_projekti : public QWidget
{
    Q_OBJECT

public:
    explicit wid_projekti(QWidget *parent = 0);
    ~wid_projekti();

private slots:
	void on_cb_stranka_currentIndexChanged();
	void on_cb_mesec_currentIndexChanged();
	void on_cb_leto_currentIndexChanged();
	void on_cb_projekt_currentIndexChanged();

	void on_tbl_projekti_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();
    void on_btn_kopiraj_clicked();

	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos(QString beseda);

private:
    Ui::wid_projekti *ui;
};

#endif // WID_PROJEKTI_H

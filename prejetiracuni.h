#ifndef PREJETIRACUNI_H
#define PREJETIRACUNI_H

#include <QDialog>

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
	void on_txt_znesekddv_0_editingFinished();
	void on_txt_znesekddv_85_editingFinished();
	void on_txt_znesekddv_20_editingFinished();

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void izracunaj();

signals:
	void poslji(QString beseda);

private:
    Ui::prejetiracuni *ui;
};

#endif // PREJETIRACUNI_H

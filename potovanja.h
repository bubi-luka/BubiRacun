#ifndef POTOVANJA_H
#define POTOVANJA_H

#include <QDialog>

namespace Ui {
    class potovanja;
}

class potovanja : public QDialog
{
    Q_OBJECT

public:
    explicit potovanja(QWidget *parent = 0);
    ~potovanja();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_nadaljuj_clicked();
	void on_btn_sprejmi_clicked();
	void keyPressEvent(QKeyEvent *event);

	void on_txt_posta_currentIndexChanged(QString besedilo);
	void on_txt_postna_stevilka_textChanged(QString besedilo);

	void on_txt_kraj_prihoda_textChanged(QString besedilo);
	void on_txt_naziv_ciljnega_podjetja_editingFinished();

	void on_txt_namen_currentIndexChanged();

	void prejem(QString besedilo);
	void shrani();

	QString preveri_napake();

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
    Ui::potovanja *ui;
};

#endif // POTOVANJA_H

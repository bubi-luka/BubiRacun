#ifndef PROJEKTI_H
#define PROJEKTI_H

#include <QDialog>

namespace Ui {
    class projekti;
}

class projekti : public QDialog
{
    Q_OBJECT

public:
    explicit projekti(QWidget *parent = 0);
    ~projekti();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();

	void on_txt_stranka_currentIndexChanged(QString besedilo);

	void on_txt_stranka_id_textChanged(QString besedilo);

	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void izracunaj();

signals:
	void poslji(QString beseda);
	void prenos(QString beseda);

private:
    Ui::projekti *ui;
};

#endif // PROJEKTI_H

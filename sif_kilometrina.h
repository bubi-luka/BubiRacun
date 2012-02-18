#ifndef SIF_KILOMETRINA_H
#define SIF_KILOMETRINA_H

#include <QDialog>

namespace Ui {
    class sif_kilometrina;
}

class sif_kilometrina : public QDialog
{
    Q_OBJECT

public:
    explicit sif_kilometrina(QWidget *parent = 0);
    ~sif_kilometrina();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_sprejmi_clicked();

	void on_txt_kilometrina_textChanged();

	void keyPressEvent(QKeyEvent *event);

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void prejem();

private:
    Ui::sif_kilometrina *ui;
};

#endif // SIF_KILOMETRINA_H

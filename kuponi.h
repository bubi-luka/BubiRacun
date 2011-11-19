#ifndef KUPONI_H
#define KUPONI_H

#include <QDialog>

namespace Ui {
    class kuponi;
}

class kuponi : public QDialog
{
    Q_OBJECT

public:
    explicit kuponi(QWidget *parent = 0);
    ~kuponi();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();
	void on_txt_uporabljen_toggled(bool stanje);
	void keyPressEvent(QKeyEvent *event);

	void on_txt_prejemnik_currentIndexChanged(QString besedilo);
	void on_txt_uveljavitelj_currentIndexChanged(QString besedilo);

	void on_txt_prejemnik_id_textChanged(QString besedilo);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
    Ui::kuponi *ui;
};

#endif // KUPONI_H

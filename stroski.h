#ifndef STROSKI_H
#define STROSKI_H

#include <QDialog>

namespace Ui {
    class stroski;
}

class stroski : public QDialog
{
    Q_OBJECT

public:
    explicit stroski(QWidget *parent = 0);
    ~stroski();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();
	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
    Ui::stroski *ui;
};

#endif // STROSKI_H

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
	void on_btn_brisi_clicked();
	void on_btn_sprejmi_clicked();
	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
    Ui::potovanja *ui;
};

#endif // POTOVANJA_H

#ifndef POPUSTI_H
#define POPUSTI_H

#include <QDialog>

namespace Ui {
    class popusti;
}

class popusti : public QDialog
{
    Q_OBJECT

public:
    explicit popusti(QWidget *parent = 0);
    ~popusti();

private slots:
	void on_btn_zapri_clicked();
	void on_btn_vnesi_clicked();

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	void napolni();

private:
    Ui::popusti *ui;
};

#endif // POPUSTI_H

#ifndef PRIJAVA_H
#define PRIJAVA_H

#include <QDialog>

#include "varnost.h"

namespace Ui {
    class prijava;
}

class prijava : public QDialog
{
    Q_OBJECT

public:
	explicit prijava(QWidget *parent = 0);
	~prijava();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_brisi_clicked();
	void on_btn_prijavi_clicked();
	void keyPressEvent(QKeyEvent *event);

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos1(QString besedilo);
	void prenos2(QString besedilo);
	void secret(QString pass);
	void obvestilo(QString obvestilo);

private:
    Ui::prijava *ui;
};

#endif // PRIJAVA_H

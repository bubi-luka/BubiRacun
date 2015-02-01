#ifndef TISKANJE_H
#define TISKANJE_H

#include <QDialog>

namespace Ui {
    class tiskanje;
}

class tiskanje : public QDialog
{
    Q_OBJECT

public:
    explicit tiskanje(QWidget *parent = 0);
    ~tiskanje();

public slots:
    void on_btn_izhod_clicked();
    void on_btn_natisni_clicked();
    void on_btn_nastavitve_racuni_clicked();
    void on_btn_nastavitve_potni_nalogi_clicked();

	void keyPressEvent(QKeyEvent *event);

	void prejem(QString vrsta, QString stevilke, QString format);

	QString pretvori_v_double(QString besedilo);
	QString pretvori_iz_double(QString besedilo);
	QString pretvori_v_int(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	void natisni_potni_nalog(QString id);
	void natisni_prejeti_racun(QString id);
	void natisni_izdani_racun(QString id);

	int natisni_glavo_potni_nalog(QPainter &painter, QString id);
	int natisni_nogo_potni_nalog(QPainter &painter, QString id, int &stevilka_strani);

	int natisni_glavo_prejeti_racun(QPainter &painter, QString id);
	int natisni_nogo_prejeti_racun(QPainter &painter, QString id, int &stevilka_strani);

	int natisni_glavo_izdani_racun(QPainter &painter, QString id);
	int natisni_nogo_izdani_racun(QPainter &painter, QString id, int &stevilka_strani, QString besedilo_noga);

private:
    Ui::tiskanje *ui;
};

#endif // TISKANJE_H

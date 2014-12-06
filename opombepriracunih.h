#ifndef OPOMBEPRIRACUNIH_H
#define OPOMBEPRIRACUNIH_H

#include <QDialog>

namespace Ui {
    class opombepriracunih;
}

class opombepriracunih : public QDialog
{
    Q_OBJECT

public:
    explicit opombepriracunih(QWidget *parent = 0);
    ~opombepriracunih();

private slots:
	void on_btn_izhod_clicked();
	void on_btn_sprejmi_clicked();

	void keyPressEvent(QKeyEvent *event);

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	void prejem(QString beseda);

signals:
	void poslji(QString beseda);

private:
    Ui::opombepriracunih *ui;
};

#endif // OPOMBEPRIRACUNIH_H

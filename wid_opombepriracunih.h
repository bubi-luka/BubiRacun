#ifndef WID_OPOMBEPRIRACUNIH_H
#define WID_OPOMBEPRIRACUNIH_H

#include <QWidget>

namespace Ui {
    class wid_opombepriracunih;
}

class wid_opombepriracunih : public QWidget
{
    Q_OBJECT

public:
    explicit wid_opombepriracunih(QWidget *parent = 0);
    ~wid_opombepriracunih();

private slots:
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();
	void on_tbl_opombe_doubleClicked();

	void osvezi(QString beseda);
	void napolni();

	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos(QString beseda);

private:
    Ui::wid_opombepriracunih *ui;
};

#endif // WID_OPOMBEPRIRACUNIH_H

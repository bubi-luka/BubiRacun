#ifndef WID_POTNINALOGI_H
#define WID_POTNINALOGI_H

#include <QWidget>

namespace Ui {
    class wid_potninalogi;
}

class wid_potninalogi : public QWidget
{
    Q_OBJECT

public:
    explicit wid_potninalogi(QWidget *parent = 0);
    ~wid_potninalogi();

private slots:
	void on_tbl_potninalogi_doubleClicked();
	void on_btn_nov_clicked();
	void on_btn_brisi_clicked();
	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void prenos(QString beseda);

private:
    Ui::wid_potninalogi *ui;
};

#endif // WID_POTNINALOGI_H

#ifndef WID_STROSKI_H
#define WID_STROSKI_H

#include <QWidget>

namespace Ui {
    class wid_stroski;
}

class wid_stroski : public QWidget
{
    Q_OBJECT

public:
    explicit wid_stroski(QWidget *parent = 0);
    ~wid_stroski();

private slots:
	void on_btn_brisi_clicked();
	void on_btn_nov_clicked();
	void on_tbl_stroski_doubleClicked();
	void osvezi(QString beseda);
	void napolni();
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);
	void prejem(QString besedilo);

signals:
	void prenos(QString beseda);
	void prenesi();

private:
    Ui::wid_stroski *ui;
};

#endif // WID_STROSKI_H

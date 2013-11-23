#ifndef RAZLOG_STORNACIJE_H
#define RAZLOG_STORNACIJE_H

#include <QDialog>

namespace Ui {
class razlog_stornacije;
}

class razlog_stornacije : public QDialog
{
	Q_OBJECT

public:
	explicit razlog_stornacije(QWidget *parent = 0);
	~razlog_stornacije();

private slots:
	void on_btn_brisi_clicked();
	void on_btn_shrani_clicked();
	void keyPressEvent(QKeyEvent *event);

	void prejem(QString besedilo);
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

signals:
	void poslji(QString beseda);

private:
	Ui::razlog_stornacije *ui;
};

#endif // RAZLOG_STORNACIJE_H

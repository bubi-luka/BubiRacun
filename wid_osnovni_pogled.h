#ifndef WID_OSNOVNI_POGLED_H
#define WID_OSNOVNI_POGLED_H

#include <QWidget>

namespace Ui {
class wid_osnovni_pogled;
}

class wid_osnovni_pogled : public QWidget
{
    Q_OBJECT

public:
    explicit wid_osnovni_pogled(QWidget *parent = 0);
    ~wid_osnovni_pogled();

private slots:
    void napolni_projekte();
    void napolni_stranke();
    void napolni_potne_naloge();
    void napolni_prejete_racune();
    void napolni_izdane_racune();

    QString pretvori(QString besedilo);
    QString prevedi(QString besedilo);

private:
    Ui::wid_osnovni_pogled *ui;
};

#endif // WID_OSNOVNI_POGLED_H

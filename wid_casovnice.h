#ifndef WID_CASOVNICE_H
#define WID_CASOVNICE_H

#include <QWidget>

namespace Ui {
    class wid_casovnice;
}

class wid_casovnice : public QWidget
{
    Q_OBJECT

public:
    explicit wid_casovnice(QWidget *parent = 0);
    ~wid_casovnice();

private:
    Ui::wid_casovnice *ui;
};

#endif // WID_CASOVNICE_H

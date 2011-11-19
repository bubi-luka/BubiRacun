#ifndef VIZITKA_H
#define VIZITKA_H

#include <QDialog>

namespace Ui {
    class Vizitka;
}

class Vizitka : public QDialog
{
    Q_OBJECT

public:
    explicit Vizitka(QWidget *parent = 0);
    ~Vizitka();

private:
    Ui::Vizitka *ui;
};

#endif // VIZITKA_H

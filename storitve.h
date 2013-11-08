#ifndef STORITVE_H
#define STORITVE_H

#include <QDialog>

namespace Ui {
class storitve;
}

class storitve : public QDialog
{
    Q_OBJECT
    
public:
    explicit storitve(QWidget *parent = 0);
    ~storitve();
    
private:
    Ui::storitve *ui;
};

#endif // STORITVE_H

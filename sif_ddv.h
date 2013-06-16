#ifndef SIF_DDV_H
#define SIF_DDV_H

#include <QDialog>

namespace Ui {
class sif_ddv;
}

class sif_ddv : public QDialog
{
    Q_OBJECT
    
public:
    explicit sif_ddv(QWidget *parent = 0);
    ~sif_ddv();
    
private:
    Ui::sif_ddv *ui;
};

#endif // SIF_DDV_H

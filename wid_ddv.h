#ifndef WID_DDV_H
#define WID_DDV_H

#include <QWidget>

namespace Ui {
class wid_ddv;
}

class wid_ddv : public QWidget
{
    Q_OBJECT
    
public:
    explicit wid_ddv(QWidget *parent = 0);
    ~wid_ddv();
    
private:
    Ui::wid_ddv *ui;
};

#endif // WID_DDV_H

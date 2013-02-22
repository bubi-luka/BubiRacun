#ifndef WID_STROSKIPREHRANE_H
#define WID_STROSKIPREHRANE_H

#include <QWidget>

namespace Ui {
class wid_stroskiprehrane;
}

class wid_stroskiprehrane : public QWidget
{
    Q_OBJECT
    
public:
    explicit wid_stroskiprehrane(QWidget *parent = 0);
    ~wid_stroskiprehrane();
    
private:
    Ui::wid_stroskiprehrane *ui;
};

#endif // WID_STROSKIPREHRANE_H

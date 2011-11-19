#include <QImage>

#include "vizitka.h"
#include "ui_vizitka.h"

Vizitka::Vizitka(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Vizitka)
{
    ui->setupUi(this);

	QImage slika(QApplication::applicationDirPath() + "/bubiji.png");
	ui->logo->setPixmap(QPixmap::fromImage(slika));

}

Vizitka::~Vizitka()
{
    delete ui;
}

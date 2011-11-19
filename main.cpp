#include <QApplication>

#include "main.h"
#include "prijava.h"
#include "potninalogi.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	prijava *okno_prijava = new prijava;
//	potninalogi *okno_prijava = new potninalogi;
	okno_prijava->show();
	return app.exec();
}

#include "main.h"
#include "prijava.h"
#include "varnost.h"

int main(int argc, char *argv[])
{
	varnost app(argc, argv);
	prijava *okno_prijava = new prijava;
	okno_prijava->show();
	return app.exec();
}

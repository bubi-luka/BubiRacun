#include <QPalette>
#include <QFile>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>

#include "main.h"
#include "varnost.h"
#include "glavnookno.h"

int main(int argc, char *argv[])
{
    varnost app(argc, argv);

    // create style for the application
    QString pot_do_stilske_datoteke = QApplication::applicationDirPath();
    QFile datoteka(pot_do_stilske_datoteke + "/sandy/sandy.qss");
    datoteka.open(QFile::ReadOnly);
    app.setStyleSheet(datoteka.readAll());
    datoteka.close();

    // create signal-slot connections
    GlavnoOkno *glavno_okno = new GlavnoOkno;
    QObject::connect(&app, SIGNAL(id_changed()), glavno_okno, SLOT(varnost_id_changed()));
    glavno_okno->show();
    return app.exec();

}

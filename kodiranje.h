#ifndef KODIRANJE_H
#define KODIRANJE_H

#include <QApplication>

class kodiranje
{
public:
    kodiranje();

public slots:
	QString zakodiraj (QString besedilo);
	QString odkodiraj (QString besedilo);
};

#endif // KODIRANJE_H

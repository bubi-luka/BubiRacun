#ifndef BAZA_H
#define BAZA_H

#include <QApplication>

class baza
{
public:
	baza();
	~baza();

	// Getters
	QString get_name();
	QString get_path();
	QString get_type();

	// Setters
	void set_name(QString name);
	void set_path(QString path);
	void set_type(QString type);

	// Methods
	void new_database();
	void ask_for_database();
	void open_database();
	void close_database();

/*
signals:
	void database_opened();
	void database_closed();
*/

private:
	static QString str_name;
	static QString str_path;
	static QString str_type;
};

#endif // BAZA_H

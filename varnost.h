#ifndef VARNOST_H
#define VARNOST_H

#include <QApplication>
#define vApp ((varnost *) qApp)

class varnost : public QApplication
{
	Q_OBJECT

public:
	explicit varnost(int &argc, char *argv[]);
	~varnost();

	// Getters
	QString id() const;
	QString name() const;
	QString surname() const;
	QString permission() const;
	QString state() const;

	// Setters
	void set_id(const QString &id);
	void set_permission(const QString &permission);
	void set_state(const QString &state);

signals:
	void id_changed();
	void permission_changed();
	void state_changed();

private:
	QString m_id;
	QString m_name;
	QString m_surname;
	QString m_permission;
	QString m_state;
};


#endif // VARNOST_H

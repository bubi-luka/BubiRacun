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
	QString user_name() const;
	QString permission() const;
	QString state() const;

	// Setters
	void set_user_name(const QString &user_name);
	void set_permission(const QString &permission);
	void set_state(const QString &state);

signals:
	void user_name_changed();
	void permission_changed();
	void state_changed();

private:
	QString m_user_name;
	QString m_permission;
	QString m_state;
};


#endif // VARNOST_H

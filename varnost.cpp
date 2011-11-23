#include "varnost.h"

varnost::varnost(int &argc, char *argv[]) :
QApplication(argc, argv) {

}

varnost::~varnost() {

}

// Getters
QString varnost::user_name() const {

	return m_user_name;

}

QString varnost::permission() const {

	return m_permission;

}

QString varnost::state() const {

	return m_state;

}

//Setters
void varnost::set_user_name(const QString &user_name) {

	m_user_name = user_name;
	emit user_name_changed();

}

void varnost::set_permission(const QString &permission)	{

	m_permission = permission;
	emit permission_changed();

}

void varnost::set_state(const QString &state) {

	m_state = state;
	emit state_changed();

}

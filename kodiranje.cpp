#include "kodiranje.h"

kodiranje::kodiranje()
{
}

QString kodiranje::zakodiraj(QString besedilo) {

	// postavi abecedo in kodirno tabelo
	int inta = 22; // stevilo abeced
	int intb = 22; // stevilo znakov v abecedi

	QString abeceda[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "r", "s", "t", "u", "v", "z"};
	QString zakodirano[inta][intb];

	int a = 0;
	int b = 0;
	int i = 0;

	for ( a = 0; a < inta; a++ ) { // stevilo abeced
		i = 0;
		for ( b = 0; b < intb; b++ ) { // stevilo znakov v abecedi
			if ( i + a + 1 >= intb ) {
				i = -a - 1;
			}
			zakodirano[a][b] = abeceda[i+a+1];
			i++;
		}
	}

	// izberi kodirne abecede
	QString kljuc = "avtocesta"; // za izbiro abeced
	int izbrane[kljuc.length()]; // izbrane abecede za kodiranje
	QString crka = "";
	QString nova_beseda = "";
	QString stara_beseda = kljuc;
	QString velikost = "";

	int zaporedna = -1; // nosi zaporedno stevilko crke iz arraya
	int abc = 0; // izbor ustrezne abecede

	for ( i = 0; i < kljuc.length(); i++ ) {
		crka = stara_beseda.left(1);
		stara_beseda = stara_beseda.right(stara_beseda.length() - 1);
		for ( a = 0; a < inta; a++ ) {
			if ( crka == zakodirano[a][0] ) {
				izbrane[i] = a;
			}
		}
	}

	// zamenjaj crko in jo vrni
	stara_beseda = besedilo;

	for ( i = 0; i < besedilo.length(); i++ ) { // za vsako crko besedila
		crka = stara_beseda.left(1);
		stara_beseda = stara_beseda.right(stara_beseda.length() - 1);

		for ( b = 0; b < intb; b++ ) { // poisci stevilko, pod katero se lahko najde crka
			if ( crka == abeceda[b] || crka == abeceda[b].toUpper() ) {
				zaporedna = b; // zaporedna nosi stevilko za mesto crke
			}
		}

		if ( zaporedna >= 0 ) {
			// ugotovi velikost pisave
			if ( crka == abeceda[zaporedna].toUpper() ) {
				velikost = "velika";
			}
			else {
				velikost = "mala";
			}

			// zakodiraj crko
			crka = zakodirano[izbrane[abc]][zaporedna];

			// spremeni v veliko, ce je bila originalna velika
			if ( velikost == "velika" ) {
				crka = crka.toUpper();
			}
		}

		abc++;
		if ( abc >= kljuc.length() ) {
			abc = 0;
		}
		nova_beseda = nova_beseda + crka;
		zaporedna = -1;

	}

	return nova_beseda;

}

QString kodiranje::odkodiraj(QString besedilo) {

	// postavi abecedo in kodirno tabelo
	int inta = 22; // stevilo abeced
	int intb = 22; // stevilo znakov v abecedi

	QString abeceda[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "r", "s", "t", "u", "v", "z"};
	QString zakodirano[inta][intb];

	int a = 0;
	int b = 0;
	int i = 0;

	for ( a = 0; a < inta; a++ ) { // stevilo abeced
		i = 0;
		for ( b = 0; b < intb; b++ ) { // stevilo znakov v abecedi
			if ( i + a + 1 >= intb ) {
				i = -a - 1;
			}
			zakodirano[a][b] = abeceda[i+a+1];
			i++;
		}
	}

	// izberi kodirne abecede
	QString kljuc = "avtocesta"; // za izbiro abeced
	int izbrane[kljuc.length()]; // izbrane abecede za kodiranje
	QString crka = "";
	QString nova_beseda = "";
	QString stara_beseda = kljuc;
	int zaporedna = -1; // nosi zaporedno stevilko crke iz arraya
	int abc = 0; // izbor ustrezne abecede
	QString velikost = "";

	for ( i = 0; i < kljuc.length(); i++ ) {
		crka = stara_beseda.left(1);
		stara_beseda = stara_beseda.right(stara_beseda.length() - 1);
		for ( a = 0; a < inta; a++ ) {
			if ( crka == zakodirano[a][0] ) {
				izbrane[i] = a;
			}
		}
	}

	// zamenjaj crko in jo vrni
	stara_beseda = besedilo;

	for ( i = 0; i < besedilo.length(); i++ ) { // za vsako crko besedila
		crka = stara_beseda.left(1);
		stara_beseda = stara_beseda.right(stara_beseda.length() - 1);

		// zamenjaj crko
		for ( b = 0; b < intb; b++ ) { // zanka gre preko cele abecede, da dobimo zaporedno st. crke
			if ( crka == zakodirano[izbrane[abc]][b] || crka == zakodirano[izbrane[abc]][b].toUpper() ) { // crka je enaka zakodirani crki kodne abecede
				zaporedna = b; // obdrzimo zaporedno st. crke
			}
		}

		if ( zaporedna >= 0 ) { // ce obstaja crka v kodni tabeli, z njo zamenjamo crko
			// preverimo, ce je crka slucajno velika
			if ( crka == zakodirano[izbrane[abc]] [zaporedna].toUpper() ) {
				velikost = "velika";
			}
			else {
				velikost = "mala";
			}

			// odkodiramo crko
			crka = abeceda[zaporedna];

			// nastavimo njeno velikost
			if ( velikost == "velika" ) {
				crka = crka.toUpper();
			}
		}

		abc++; // preidemo na naslednjo kodno abecedo
		if ( abc >= kljuc.length() ) { // ce smo na koncu kodnih abeced
			abc = 0; // vrnemo kodne abecede na zacetek
		}

		nova_beseda = nova_beseda + crka;
		zaporedna = -1;

	}

	return nova_beseda;

}

#ifndef SETUP_H
#define SETUP_H

#include <QDialog>

class setup
{
public:
	setup();
	void start_first_run();

private slots:
	QString pretvori(QString besedilo);
	QString prevedi(QString besedilo);

	// ustvari bazo podatkov
	void baza_podatkov();

	// ustvari glavno tabelo
	void glavna_tabela();

	// ustvari tabele
	void tabela_podjetje();
	void tabela_uporabnik();
	void tabela_potni_nalogi();
	void tabela_potovanja();
	void tabela_stroski();
	void tabela_prejeti_racuni();
	void tabela_stranke();
	void tabela_projekti();
	void tabela_racuni();
	void tabela_opravila();
	void tabela_opombe();
	void tabela_nastavitve();
	void tabela_avtomobili();
	void tabela_stroski_prehrane();
	void tabela_prioriteta_strank();

	// ustvari tabele sifrantov
	void tabela_skd();
	void tabela_posta();
	void tabela_nazivi();
	void tabela_pogodbe();
	void tabela_dovoljenja();
	void tabela_status_projekta();
	void tabela_status_racuna();
	void tabela_status_placila();
	void tabela_status_racunovodstva();
	void tabela_popusti();
	void tabela_namen_potnega_naloga();
	void tabela_prevoz();
	void tabela_predracuni();
	void tabela_kategorije();
	void tabela_podkategorije();
	void tabela_storitev();
	void tabela_oddaje_racuna();
	void tabela_opombe_pri_racunih();
	void tabela_dnevnice();
	void tabela_kilometrina();
	void tabela_cenamalice();
	void tabela_banke();
	void tabela_koda_namena();
	void tabela_ddv();
	void tabela_enote();

	// vnese podatke v tabele
	void vnesi_skd();
	void vnesi_posta();
	void vnesi_nazive();
	void vnesi_pogodbe();
	void vnesi_dovoljenja();
	void vnesi_status_projekta();
	void vnesi_status_racuna();
	void vnesi_status_placila();
	void vnesi_status_racunovodstva();
	void vnesi_popuste();
	void vnesi_namen_potnega_naloga();
	void vnesi_prevoz();
	void vnesi_predracune();
	void vnesi_oddaja_racuna();
	void vnesi_nastavitve();
	void vnesi_banke();
	void vnesi_koda_namena();
	void vnesi_stroski_prehrane();

	// posodobitev baze
	void posodobi_bazo();

};

#endif // SETUP_H

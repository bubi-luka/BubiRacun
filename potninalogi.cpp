#include <QKeyEvent>
#include <QDate>
#include <QMessageBox>
#include <QValidator>
#include <QFocusEvent>
#include <QtSql>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QSize>
#include <QSizeF>
#include <QFile>

#include "potninalogi.h"
#include "ui_potninalogi.h"
#include "prijava.h"
#include "glavnookno.h"
#include "kodiranje.h"
#include "wid_potovanja.h"
#include "wid_stroski.h"

potninalogi::potninalogi(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::potninalogi)
{
		ui->setupUi(this);

	// disable fields
	ui->txt_kilometri->setEnabled(false);
	ui->txt_cenaprevoza->setEnabled(false);
	ui->txt_skupaj->setEnabled(false);
	ui->txt_ostalo->setEnabled(false);
	ui->txt_cenaodsotnosti->setEnabled(false);
	ui->txt_dnevnica1->setEnabled(false);
	ui->txt_dnevnica2->setEnabled(false);
	ui->txt_dnevnica3->setEnabled(false);
	ui->txt_dnevi->setEnabled(false);
	ui->txt_ure->setEnabled(false);

	ui->txt_priznajdnevnico->setChecked(true);

	// hide car info
	ui->txt_avtomobil->setHidden(true);
	ui->txt_registrska->setHidden(true);
	ui->label_9->setHidden(true);
	ui->label_8->setHidden(true);

	QString leto = QDate::currentDate().toString("yyyy");
	leto = leto.insert(3, "\\");
	leto = leto.insert(2, "\\");
	leto = leto.insert(1, "\\");
	leto = leto.insert(0, "\\");

	ui->txt_stnaloga->setInputMask("P\\N-" + leto + "-999;_");

	leto = QDate::currentDate().toString("yyyy");

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
		int i = 1;
		QString stevilka = "";
		QSqlQuery sql_insert_stnaloga;
		sql_insert_stnaloga.prepare("SELECT * FROM potninalogi WHERE stevilkanaloga LIKE '" + pretvori("PN-" + leto) + "%'");
		sql_insert_stnaloga.exec();
		while (sql_insert_stnaloga.next()) {
			i++;
		}
		if ( i < 10 ) {
			stevilka = "00" + QString::number(i, 10);
		}
		else if ( i < 100 ) {
			stevilka = "0" + QString::number(i, 10);
		}
		else {
			stevilka = "" + QString::number(i, 10);
		}
		ui->txt_stnaloga->setText("PN-" + leto + "-" + stevilka);

		ui->txt_datum->setDate(QDate::currentDate());

		ui->txt_kilometrina->setText("0.37");
//		ui->txt_dnevnica->setText("10.00");

		// add empty first line to all combobox
		ui->txt_podjetje->addItem("");
		ui->txt_nazivpredlagatelja->addItem("");
		ui->txt_predlagatelj->addItem("");
		ui->txt_prevoz->addItem("");
		ui->txt_prejemnik->addItem("");
		ui->txt_prejemnikime->addItem("");
		ui->txt_nazivprejemnika->addItem("");
		ui->txt_namen->addItem("");

		QSqlQuery sql_fill_combo;
		sql_fill_combo.prepare("SELECT * FROM podjetje");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_podjetje->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("ime")).toString()));
		}
		sql_fill_combo.clear();

		sql_fill_combo.prepare("SELECT * FROM sif_nazivi");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_nazivpredlagatelja->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naziv")).toString()));
			ui->txt_nazivprejemnika->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("naziv")).toString()));
		}
		sql_fill_combo.clear();

		sql_fill_combo.prepare("SELECT * FROM sif_prevoz");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_prevoz->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("prevoz")).toString()));
		}
		sql_fill_combo.clear();

		sql_fill_combo.prepare("SELECT * FROM uporabniki");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_prejemnik->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("priimek")).toString()));
		}
		sql_fill_combo.clear();

		sql_fill_combo.prepare("SELECT * FROM sif_namen");
		sql_fill_combo.exec();
		while (sql_fill_combo.next()) {
			ui->txt_namen->addItem(prevedi(sql_fill_combo.value(sql_fill_combo.record().indexOf("namen")).toString()));
		}
		sql_fill_combo.clear();
	}
	base.close();

	// insert widgets
	wid_stroski *widst = new wid_stroski;
	ui->wid_st->setWidget(widst);

	wid_potovanja *widpot = new wid_potovanja;
	ui->wid_pot->setWidget(widpot);

}

potninalogi::~potninalogi()
{
		delete ui;
}

void potninalogi::on_btn_izhod_clicked() {

	close();

}

// zacasno printa :)
void potninalogi::on_btn_brisi_clicked() {

	int i;
//	int st_nalog = 21; // st stolpcev v tabeli potnih nalogov
	int st_besedilo = 100; // st vrstic namenjenih potnemu nalogu

	QString vsebina;
//	QString nalog[st_nalog];
	QString besedilo[st_besedilo];

// napolni array z besedilom na strani
	QFile datoteka("besedilo-potni-nalogi.txt");
	if (!datoteka.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	i = 0;
	QTextStream in(&datoteka);
	while (!in.atEnd()) {
		besedilo[i] = in.readLine().replace("\"", "");
		i++;
	}

// odpri podatke o potnem nalogu, prejemniku, stroskih...
	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
// zacetek
		vsebina.append("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
		vsebina.append("<html>\n");
		vsebina.append("<head>\n");
		vsebina.append("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
		vsebina.append("<title>" + besedilo[0] + ui->txt_stnaloga->text() + "</title>\n");
		vsebina.append("<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\" media=\"screen, print\" />");
		vsebina.append("</head>\n");
		vsebina.append("\n");
		vsebina.append("<body>\n");
// glava
		vsebina.append("<table border=1 width=\"100%\">\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"50%\"><strong>" + besedilo[1] + "</strong></td>\n");
		vsebina.append("<td width=\"50%\"><h1>" + besedilo[2] + "</h1></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td>" + ui->txt_predlagatelj->currentText() + "</td>\n");
		vsebina.append("<td><strong>" + besedilo[3] + "</strong>" + ui->txt_stnaloga->text() + "<strong>" + besedilo[4] + "</strong>" + ui->txt_datum->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("</table>\n");
// odredba
		vsebina.append("<ul>\n");
		vsebina.append("<li><strong>" + besedilo[5] + "</strong>" + ui->txt_prejemnik->currentText() + " " + ui->txt_prejemnikime->currentText() + "</li>\n");
		vsebina.append("<li><strong>" + besedilo[6] + "</strong>" + ui->txt_nazivprejemnika->currentText() + "</li>\n");
		vsebina.append("<li><strong>" + besedilo[7] + "</strong>" + ui->txt_naslovprejemnika->toPlainText().replace("\n", ", ") + "</li>\n");
		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM pot WHERE potninalog LIKE '" + pretvori(ui->txt_stnaloga->text()) + "'");
		sql_pot.exec();
		QDateTime dt_datum = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
		QString datum;
		QString ura;
		while ( sql_pot.next() ) {
			if ( dt_datum == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
				dt_datum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casodhod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( dt_datum.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casodhod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) < 0 ) {
				dt_datum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casodhod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
		}
		datum = dt_datum.toString("dd'.'MM'.'yyyy' 'hh':'mm");
		ura = datum.right(5);
		datum = datum.left(datum.length() - 6);
		vsebina.append("<li><strong>" + besedilo[8] + "</strong>" + datum + "<strong>" + besedilo[9] + "</strong>" + ura + "</li>\n");
		vsebina.append("<li><strong>" + besedilo[10] + "</strong></li>\n");
		sql_pot.clear();
		sql_pot.prepare("SELECT * FROM pot WHERE potninalog LIKE '" + pretvori(ui->txt_stnaloga->text()) + "'");
		sql_pot.exec();
		while ( sql_pot.next() ) {
			if ( prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()) != "" ) {
				vsebina.append("<li><strong>" + besedilo[11] + "</strong>" + prevedi(sql_pot.value(sql_pot.record().indexOf("naslov")).toString()).replace("\n", ", ") + "</li>\n");
			}
		}
		vsebina.append("<li><strong>" + besedilo[12] + "</strong>" + ui->txt_namen->currentText() + "</li>\n");
		sql_pot.clear();
		sql_pot.prepare("SELECT * FROM pot WHERE potninalog LIKE '" + pretvori(ui->txt_stnaloga->text()) + "'");
		sql_pot.exec();
		dt_datum = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
		while ( sql_pot.next() ) {
			if ( dt_datum == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
				dt_datum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casprihod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( dt_datum.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casprihod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) > 0 ) {
				dt_datum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casprihod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
		}
		datum = dt_datum.toString("dd'.'MM'.'yyyy' 'hh':'mm");
		datum = datum.left(datum.length() - 6);
		vsebina.append("<li>&nbsp;</li>\n");
		vsebina.append("<li><strong>" + besedilo[13] + "</strong>" + datum + "<strong>" + besedilo[14] + "</strong>" + ui->txt_dnevi->text());
		vsebina.append("<strong>" + besedilo[15] + "</strong></li>\n");
		vsebina.append("<li><strong>" + besedilo[16] + "</strong>" + ui->txt_prevoz->currentText() + ", " + ui->txt_avtomobil->text() + "</li>\n");
		vsebina.append("<li>&nbsp;</li>\n");
		vsebina.append("<li><strong>" + besedilo[17] + "</strong>" + ui->txt_podjetje->currentText() + "</li>\n");
		vsebina.append("<li><strong>" + besedilo[18] + "</strong></li>\n");
		vsebina.append("<li><strong>" + besedilo[19] + "</strong></li>\n");
		vsebina.append("<li><strong>" + besedilo[20] + "</strong></li>\n");
		vsebina.append("</ul>\n");
// predujem
		vsebina.append("<table border=1 width=\"100%\">\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\"></td>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\" rowspan=\"5\"><strong>" + besedilo[23] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\">" + ui->txt_predlagatelj->currentText() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\" rowspan=\"3\"><strong>" + besedilo[21] + "<br />" + besedilo[22] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\"><strong>" + besedilo[24] + "</strong></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\"></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\">" + ui->txt_prejemnik->currentText() + " " + ui->txt_prejemnik->currentText() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\"></td>\n");
		vsebina.append("<td width=\"33%\" valign=\"middle\"><strong>" + besedilo[25] + "</strong></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("</table>\n");
// obracun potnih stroskov - glava
		vsebina.append("<table border=1 width=\"100%\">\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td><strong align=\"right\">" + besedilo[26] + "</strong></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td><strong>" + besedilo[27] + "</strong>" + ui->txt_predlagatelj->currentText() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td><strong>" + besedilo[28] + "</strong>" + ui->txt_naslovpodjetja->toPlainText() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("</table>\n");
// obracun potnih stroskov - podatki
		vsebina.append("<table border=1 width=\"100%\">\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"3\"></td>\n");
		vsebina.append("<td colspan=\"3\"><strong>" + besedilo[29] + "</strong></td>\n");
		vsebina.append("<td rowspan=\"2\" valign=\"middle\"><strong>" + besedilo[30] + "</strong></td>\n");
		vsebina.append("<td rowspan=\"2\" valign=\"middle\"><strong>" + besedilo[31] + "</strong></td>\n");
		vsebina.append("<td rowspan=\"2\" valign=\"middle\"><strong>" + besedilo[32] + "</strong></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td><strong>" + besedilo[33] + "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td><strong>" + besedilo[34] + "</strong>" + ui->txt_avtomobil->text() + "<strong>" + besedilo[35] + "</strong></td>\n");
		vsebina.append("<td><strong>" + besedilo[36] + "</strong></td>\n");
		vsebina.append("<td><strong>" + besedilo[37] + "</strong></td>\n");
		vsebina.append("<td><strong>" + besedilo[38] + "</strong></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td><strong>" + besedilo[39] + "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td><strong>" + besedilo[34] + "</strong>" + ui->txt_avtomobil->text() + "<strong>" + besedilo[35] +  "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"8\">" + besedilo[40] + "</td>\n");
		vsebina.append("<td></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"8\"><strong>" + besedilo[41] + "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td rowspan=\"3\"><strong>" + besedilo[42] + "</strong></td>\n");
		vsebina.append("<td colspan=\"7\"><strong>" + besedilo[43] + "</strong>" + ui->txt_avtomobil->text() + "<strong>" + besedilo[44] +
					   "</strong>" + ui->txt_avtomobil->text() + "<strong>" + besedilo[45] + "</strong></td>\n");
		vsebina.append("<td rowspan=\"3\"><strong>" + besedilo[42] + "</strong></td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"7\"><strong>" + besedilo[46] + "</strong>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"7\"><strong>" + besedilo[47] + "</strong>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td><strong>" + besedilo[48] + "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td><strong>" + besedilo[49] + "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"8\"><strong>" + besedilo[50] + "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"8\"><strong>" + besedilo[51] + "</strong></td>\n");
		vsebina.append("<td>" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("</table>\n");
// noga, podpisi, zigi
		vsebina.append("<table border=1 width=\"100%\">\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"3\">" + besedilo[52] + "</td>\n");
		vsebina.append("</tr>\n");
		vsebina.append("<tr>\n");
		vsebina.append("<td colspan=\"3\"><strong>" + besedilo[53] + "</strong>" + besedilo[54] + "<strong>" + besedilo[55] + "</strong></td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[56] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\" rowspan=\"9\"><strong>" + besedilo[23] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\">" + ui->txt_avtomobil->text() +  "</td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\"></td>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[57] +  "</strong></td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[58] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\"></td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\"></td>\n");
		vsebina.append("<td width=\"33%\">" + ui->txt_avtomobil->text() +  "</td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[59] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[60] + "</strong></td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[61] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\"></td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\">&nbsp;</td>\n");
		vsebina.append("<td width=\"33%\">&nbsp;</td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\">" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("<td width=\"33%\">" + ui->txt_avtomobil->text() + "</td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("<tr>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[62] + "</strong></td>\n");
		vsebina.append("<td width=\"33%\"><strong>" + besedilo[63] + "</strong></td>\n");
		vsebina.append("</tr>\n");

		vsebina.append("</table>\n");

		vsebina.append("</body>\n");
		vsebina.append("</html>\n");

	}
	base.close();

	QFile file1("potninalog.html");
	if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file1);
	out << vsebina;
	file1.close();

	QString css;
	css = "html { margin:0; padding:0; } body { margin:0; padding:0; } table { margin:0; padding:0; border:0; margin-top:10px; margin-bottom:10px;} td ";
	css = css + "{ margin:0; padding:0; border:0; } tr { margin:0; padding:0; border:0; } ul { margin:0; padding:0; list-style-type:none; }";

	QTextDocument *document = new QTextDocument();
	document->addResource(QTextDocument::StyleSheetResource, QUrl("style.css"), QVariant(css));
	document->setHtml(vsebina);


	QPrinter printer;

	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName("potninalog.pdf");
	printer.setPaperSize(QPrinter::A4);
	printer.setResolution(300);
	printer.setOrientation(QPrinter::Portrait);

//	QPrintDialog *dialog = new QPrintDialog(&printer, this);
//	if (dialog->exec() != QDialog::Accepted)
//	return;

	document->print(&printer);

//	delete document;

}

void potninalogi::on_txt_podjetje_currentIndexChanged(int index) {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
		QSqlQuery sql_fill_podjetje;
		sql_fill_podjetje.prepare("SELECT * FROM podjetje WHERE id LIKE '" + QString::number(index, 10) + "'");
		sql_fill_podjetje.exec();
		ui->txt_predlagatelj->clear();
		ui->txt_predlagatelj->addItem("");
		ui->txt_nazivpredlagatelja->setCurrentIndex(0);
		while (sql_fill_podjetje.next()) {
			ui->txt_predlagatelj->addItem(prevedi(sql_fill_podjetje.value(sql_fill_podjetje.record().indexOf("odgovornaoseba")).toString()));
			ui->txt_predlagatelj->setCurrentIndex(1);
			bool ok;
			QSqlQuery sql_combo_naziv;
			sql_combo_naziv.prepare("SELECT * FROM sif_nazivi WHERE naziv LIKE '" +
									sql_fill_podjetje.value(sql_fill_podjetje.record().indexOf("nazivodgovorneosebe")).toString() + "'");
			sql_combo_naziv.exec();
			ui->txt_nazivpredlagatelja->setCurrentIndex(0);
			if (sql_combo_naziv.next()) {
				if ( ui->txt_predlagatelj->currentText() != "") {
					ui->txt_nazivpredlagatelja->setCurrentIndex(sql_combo_naziv.value(sql_combo_naziv.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				else {
					ui->txt_nazivpredlagatelja->setCurrentIndex(0);
				}
			}
			else {
				ui->txt_nazivpredlagatelja->setCurrentIndex(0);
			}
			ui->txt_naslovpodjetja->setPlainText(prevedi(sql_fill_podjetje.value(sql_fill_podjetje.record().indexOf("naslov")).toString()));
		}
	}
	base.close();
}

void potninalogi::on_txt_predlagatelj_currentIndexChanged(int index) {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
		QSqlQuery sql_fill_podjetje;
		sql_fill_podjetje.prepare("SELECT * FROM podjetje WHERE ime LIKE '" + ui->txt_podjetje->currentText() + "'");
		sql_fill_podjetje.exec();
		while (sql_fill_podjetje.next()) {
			bool ok;
			QSqlQuery sql_combo_naziv;
			sql_combo_naziv.prepare("SELECT * FROM sif_nazivi WHERE naziv LIKE '" +	sql_fill_podjetje.value(sql_fill_podjetje.record().indexOf("nazivodgovorneosebe")).toString() + "'");
			sql_combo_naziv.exec();
			if (sql_combo_naziv.next()) {
				if ( ui->txt_predlagatelj->currentText() != "") {
					ui->txt_nazivpredlagatelja->setCurrentIndex(sql_combo_naziv.value(sql_combo_naziv.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				else {
					ui->txt_nazivpredlagatelja->setCurrentIndex(0);
				}
			}
			else {
				ui->txt_nazivpredlagatelja->setCurrentIndex(0);
			}
		}
	}
	base.close();

}

void potninalogi::on_txt_prejemnik_currentIndexChanged(int index) {

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
		QSqlQuery sql_fill_ime;
		sql_fill_ime.prepare("SELECT * FROM uporabniki WHERE priimek LIKE '" + pretvori(ui->txt_prejemnik->currentText()) + "'");
		sql_fill_ime.exec();
		ui->txt_prejemnikime->clear();
		ui->txt_prejemnikime->addItem("");
		while (sql_fill_ime.next()) {
			ui->txt_prejemnikime->addItem(prevedi(sql_fill_ime.value(sql_fill_ime.record().indexOf("ime")).toString()));
		}
	}
	base.close();

}

void potninalogi::on_txt_prejemnikime_currentIndexChanged(int index){

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
		QSqlQuery sql_fill_prejemnik;
		sql_fill_prejemnik.prepare("SELECT * FROM uporabniki WHERE priimek LIKE '" + pretvori(ui->txt_prejemnik->currentText()) + "' AND ime LIKE '" + pretvori(ui->txt_prejemnikime->currentText()) + "'");
		sql_fill_prejemnik.exec();
		if (!sql_fill_prejemnik.next()) {
			ui->txt_nazivprejemnika->setCurrentIndex(0);
			ui->txt_naslovprejemnika->clear();
			ui->txt_prevoz->setCurrentIndex(0);
			ui->txt_avtomobil->setText("");
			ui->txt_registrska->setText("");
		}
		else {
			bool ok;
			QSqlQuery sql_check_naziv;
			sql_check_naziv.prepare("SELECT * FROM sif_nazivi WHERE naziv LIKE '" + sql_fill_prejemnik.value(sql_fill_prejemnik.record().indexOf("naziv")).toString() + "'");
			sql_check_naziv.exec();
			if (sql_check_naziv.next()) {
				ui->txt_nazivprejemnika->setCurrentIndex(sql_check_naziv.value(sql_check_naziv.record().indexOf("id")).toString().toInt(&ok, 10));
			}
			ui->txt_naslovprejemnika->setPlainText(prevedi(sql_fill_prejemnik.value(sql_fill_prejemnik.record().indexOf("naslov")).toString()));
			ui->txt_prevoz->setCurrentIndex(0);
			ui->txt_avtomobil->setText(prevedi(sql_fill_prejemnik.value(sql_fill_prejemnik.record().indexOf("avtomobil")).toString()));
			if (ui->txt_avtomobil->text() != "") {
				QSqlQuery sql_set_avto;
				sql_set_avto.prepare("SELECT * FROM sif_prevoz WHERE prevoz LIKE '" + pretvori("Avtomobil") + "'");
				sql_set_avto.exec();
				if (sql_set_avto.next()) {
					ui->txt_prevoz->setCurrentIndex(sql_set_avto.value(sql_set_avto.record().indexOf("id")).toString().toInt(&ok, 10));
				}
			}
			ui->txt_registrska->setText(prevedi(sql_fill_prejemnik.value(sql_fill_prejemnik.record().indexOf("registracija")).toString()));
		}

	}
	base.close();

}

void potninalogi::on_txt_prevoz_currentIndexChanged(int index) {

	if (index == 1) {
		ui->txt_avtomobil->setHidden(false);
		ui->txt_registrska->setHidden(false);
		ui->label_9->setHidden(false);
		ui->label_8->setHidden(false);
	}
	else {
		ui->txt_avtomobil->setHidden(true);
		ui->txt_registrska->setHidden(true);
		ui->label_9->setHidden(true);
		ui->label_8->setHidden(true);
	}
}

void potninalogi::on_btn_sprejmi_clicked() {

	izracun(); // just to be safe

	// check for correct fields
	QString napaka = "";

	// nastavitev polja za napako
	QFont font_error;
	font_error.setBold(true);
	font_error.setUnderline(true);

	QFont font_normal;
	font_normal.setBold(false);
	font_normal.setUnderline(false);

	QPalette palette_error;
	QPalette palette_normal;

	QBrush brush_error(QColor(255, 0, 0, 255));
	brush_error.setStyle(Qt::SolidPattern);

	QBrush brush_normal(QColor(0, 0, 0, 255));
	brush_normal.setStyle(Qt::SolidPattern);

	palette_error.setBrush(QPalette::Active, QPalette::WindowText, brush_error);
	palette_error.setBrush(QPalette::Inactive, QPalette::WindowText, brush_error);
	palette_normal.setBrush(QPalette::Active, QPalette::WindowText, brush_normal);
	palette_normal.setBrush(QPalette::Inactive, QPalette::WindowText, brush_normal);

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "uporabniki");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
		// st. naloga ne sme biti ze vnesena
		QSqlQuery sql_check;
		sql_check.prepare("SELECT * FROM potninalogi WHERE stevilkanaloga LIKE '" + ui->txt_stnaloga->text() + "'");
		sql_check.exec();
		if (sql_check.next()) {
			if (sql_check.value(sql_check.record().indexOf("id")).toString() == ui->txt_id->text()) {
				ui->label_2->setPalette(palette_normal);
				ui->label_2->setFont(font_normal);
			}
			else {
				ui->label_2->setPalette(palette_error);
				ui->label_2->setFont(font_error);
				napaka = "error";
				ui->txt_stnaloga->setFocus();
			}
		}
		else {
			ui->label_2->setPalette(palette_normal);
			ui->label_2->setFont(font_normal);
		}
		sql_check.clear();

		// polja, ki ne smejo biti prazna: zacetek, konec, kilometri, vsi podatki o izdajatelju
		// preveri se, ce je avto da ima tako tip kot registrsko

		if (ui->txt_kilometri->text() == "") {
			ui->label_18->setPalette(palette_error);
			ui->label_18->setFont(font_error);
			napaka = "error";
			ui->txt_kilometri->setFocus();
		}
		else {
			ui->label_18->setPalette(palette_normal);
			ui->label_18->setFont(font_normal);
		}

		if (ui->txt_podjetje->currentText() == "") {
			ui->label_4->setPalette(palette_error);
			ui->label_4->setFont(font_error);
			napaka = "error";
			ui->txt_podjetje->setFocus();
		}
		else {
			ui->label_4->setPalette(palette_normal);
			ui->label_4->setFont(font_normal);
		}
		if (ui->txt_predlagatelj->currentText() == "") {
			ui->label_13->setPalette(palette_error);
			ui->label_13->setFont(font_error);
			napaka = "error";
			ui->txt_predlagatelj->setFocus();
		}
		else {
			ui->label_13->setPalette(palette_normal);
			ui->label_13->setFont(font_normal);
		}
		if (ui->txt_nazivpredlagatelja->currentText() == "") {
			ui->label_5->setPalette(palette_error);
			ui->label_5->setFont(font_error);
			napaka = "error";
			ui->txt_nazivpredlagatelja->setFocus();
		}
		else {
			ui->label_5->setPalette(palette_normal);
			ui->label_5->setFont(font_normal);
		}
		if (ui->txt_naslovpodjetja->toPlainText() == "") {
			ui->label_6->setPalette(palette_error);
			ui->label_6->setFont(font_error);
			napaka = "error";
			ui->txt_naslovpodjetja->setFocus();
		}
		else {
			ui->label_6->setPalette(palette_normal);
			ui->label_6->setFont(font_normal);
		}

		if (ui->txt_prejemnik->currentText() == "") {
			ui->label_7->setPalette(palette_error);
			ui->label_7->setFont(font_error);
			napaka = "error";
			ui->txt_prejemnik->setFocus();
		}
		else {
			ui->label_7->setPalette(palette_normal);
			ui->label_7->setFont(font_normal);
		}
		if (ui->txt_prejemnikime->currentText() == "") {
			ui->label_22->setPalette(palette_error);
			ui->label_22->setFont(font_error);
			napaka = "error";
			ui->txt_prejemnikime->setFocus();
		}
		else {
			ui->label_22->setPalette(palette_normal);
			ui->label_22->setFont(font_normal);
		}
		if (ui->txt_nazivprejemnika->currentText() == "") {
			ui->label_10->setPalette(palette_error);
			ui->label_10->setFont(font_error);
			napaka = "error";
			ui->txt_nazivprejemnika->setFocus();
		}
		else {
			ui->label_10->setPalette(palette_normal);
			ui->label_10->setFont(font_normal);
		}
		if (ui->txt_naslovprejemnika->toPlainText() == "") {
			ui->label_11->setPalette(palette_error);
			ui->label_11->setFont(font_error);
			napaka = "error";
			ui->txt_naslovprejemnika->setFocus();
		}
		else {
			ui->label_11->setPalette(palette_normal);
			ui->label_11->setFont(font_normal);
		}
		if (ui->txt_prevoz->currentText() == "") {
			ui->label_12->setPalette(palette_error);
			ui->label_12->setFont(font_error);
			napaka = "error";
			ui->txt_prevoz->setFocus();
		}
		else {
			ui->label_12->setPalette(palette_normal);
			ui->label_12->setFont(font_normal);
		}
		if (ui->txt_kilometrina->text() == "") {
			ui->label_25->setPalette(palette_error);
			ui->label_25->setFont(font_error);
			napaka = "error";
			ui->txt_kilometrina->setFocus();
		}
		else {
			ui->label_25->setPalette(palette_normal);
			ui->label_25->setFont(font_normal);
		}
		if (ui->txt_naloga->text() == "") {
			ui->label_24->setPalette(palette_error);
			ui->label_24->setFont(font_error);
			napaka = "error";
			ui->txt_naloga->setFocus();
		}
		else {
			ui->label_24->setPalette(palette_normal);
			ui->label_24->setFont(font_normal);
		}
		if (ui->txt_namen->currentText() == "") {
			ui->label_23->setPalette(palette_error);
			ui->label_23->setFont(font_error);
			napaka = "error";
			ui->txt_namen->setFocus();
		}
		else {
			ui->label_23->setPalette(palette_normal);
			ui->label_23->setFont(font_normal);
		}

		if (ui->txt_prevoz->currentText() == "Avtomobil" && ui->txt_avtomobil->text() == "") {
			ui->label_9->setPalette(palette_error);
			ui->label_9->setFont(font_error);
			napaka = "error";
			ui->txt_avtomobil->setFocus();
		}
		else {
			ui->label_9->setPalette(palette_normal);
			ui->label_9->setFont(font_normal);
		}
		if (ui->txt_prevoz->currentText() == "Avtomobil" && ui->txt_registrska->text() == "") {
			ui->label_8->setPalette(palette_error);
			ui->label_8->setFont(font_error);
			napaka = "error";
			ui->txt_registrska->setFocus();
		}
		else {
			ui->label_8->setPalette(palette_normal);
			ui->label_8->setFont(font_normal);
		}

		// javi napake, ce ni napak vnesi v bazo
		if (napaka == "") {
			QSqlQuery sql_vnesi_uporabnika;
			if (ui->btn_sprejmi->text() == "Vnesi potni nalog") { // vnesi novega uporabnika

			sql_vnesi_uporabnika.prepare("INSERT INTO potninalogi (podjetje, stevilkanaloga, datum, naslovnik, naslovnikime, naziv, naslov, "
										 "naloga, namen, prevoznosredstvo, tipavtomobila, registrskastevilka, razdalja, kilometrina, opombe, "
										 "predlagatelj, nazivpredlagatelja, naslovpodjetja, zajtrk2, zajtrk3)"
										 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
			}
			else { // popravi ze obstojeci vnos
				sql_vnesi_uporabnika.prepare("UPDATE potninalogi SET podjetje = ?, stevilkanaloga = ?, datum = ?, naslovnik = ?, "
											 "naslovnikime = ?, naziv = ?, naslov = ?, naloga = ?, namen = ?, prevoznosredstvo = ?, tipavtomobila = ?, "
											 "registrskastevilka = ?, razdalja = ?, kilometrina = ?, opombe = ?, predlagatelj = ?, "
											 "nazivpredlagatelja = ?, naslovpodjetja = ?, zajtrk2 = ?, zajtrk3 = ? WHERE id LIKE '" + ui->txt_id->text() + "'");
			}
			sql_vnesi_uporabnika.bindValue(0, pretvori(ui->txt_podjetje->currentText()));
			sql_vnesi_uporabnika.bindValue(1, pretvori(ui->txt_stnaloga->text()));
			sql_vnesi_uporabnika.bindValue(2, pretvori(ui->txt_datum->text()));
			sql_vnesi_uporabnika.bindValue(3, pretvori(ui->txt_prejemnik->currentText()));
			sql_vnesi_uporabnika.bindValue(4, pretvori(ui->txt_prejemnikime->currentText()));
			sql_vnesi_uporabnika.bindValue(5, pretvori(ui->txt_nazivprejemnika->currentText()));
			sql_vnesi_uporabnika.bindValue(6, pretvori(ui->txt_naslovprejemnika->toPlainText()));
			sql_vnesi_uporabnika.bindValue(7, pretvori(ui->txt_naloga->text()));
			sql_vnesi_uporabnika.bindValue(8, pretvori(ui->txt_namen->currentText()));
			sql_vnesi_uporabnika.bindValue(9, pretvori(ui->txt_prevoz->currentText()));
			sql_vnesi_uporabnika.bindValue(10, pretvori(ui->txt_avtomobil->text()));
			sql_vnesi_uporabnika.bindValue(11, pretvori(ui->txt_registrska->text()));
			sql_vnesi_uporabnika.bindValue(12, pretvori(ui->txt_kilometri->text()));
			sql_vnesi_uporabnika.bindValue(13, pretvori(ui->txt_kilometrina->text().replace(",", ".")));
			sql_vnesi_uporabnika.bindValue(14, pretvori(ui->txt_opombe->toPlainText()));
			sql_vnesi_uporabnika.bindValue(15, pretvori(ui->txt_predlagatelj->currentText()));
			sql_vnesi_uporabnika.bindValue(16, pretvori(ui->txt_nazivpredlagatelja->currentText()));
			sql_vnesi_uporabnika.bindValue(17, pretvori(ui->txt_naslovpodjetja->toPlainText()));
			QString zajtrk = "";
			if ( ui->txt_zajtrk2->isChecked() ) {
				zajtrk = "zajtrk2";
			}
			sql_vnesi_uporabnika.bindValue(18, pretvori(zajtrk));
			zajtrk = "";
			if ( ui->txt_zajtrk3->isChecked() ) {
				zajtrk = "zajtrk3";
			}
			sql_vnesi_uporabnika.bindValue(19, pretvori(zajtrk));
			sql_vnesi_uporabnika.exec();

			// send signal to reload widget
			poslji("potninalog");

			// close this window
			close();
		}
		else {
			QMessageBox msgbox;
			msgbox.setText("Dolocena polja niso pravilno izpolnjena");
			msgbox.exec();
		}
	}
	base.close();

}

void potninalogi::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return)
	{
		this->on_btn_sprejmi_clicked();
	}
	else if (event->key() == Qt::Key_Escape)
	{
		this->on_btn_izhod_clicked();
	}
	else if ((event->key() == Qt::Key_Delete) && (event->modifiers() == Qt::AltModifier))
	{
		this->on_btn_brisi_clicked();
	}
}

void potninalogi::prejem(QString besedilo) {

	if (besedilo == "Nov nalog") {
		ui->btn_sprejmi->setText("Vnesi potni nalog");
	}
	else {
		ui->btn_sprejmi->setText("Popravi potni nalog");
		// besedilo nosi ID ze obstojecega uporabnika, potrebno je napolniti polja
		QString app_path = QApplication::applicationDirPath();
		QString dbase_path = app_path + "/base.bz";

		QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE");
		base.setDatabaseName(dbase_path);
		base.database();
		base.open();
		if(base.isOpen() != true){
			QMessageBox msgbox;
			msgbox.setText("Baze ni bilo moc odpreti");
			msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
			msgbox.exec();
		}
		else {
			QSqlQuery sql_napolni;
			sql_napolni.prepare("SELECT * FROM potninalogi WHERE id LIKE '" + besedilo + "'");
			sql_napolni.exec();
			if (sql_napolni.next()) {
				ui->txt_id->setText(sql_napolni.value(sql_napolni.record().indexOf("id")).toString());
				ui->txt_stnaloga->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("stevilkanaloga")).toString()));
				ui->txt_naloga->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naloga")).toString()));
				ui->txt_avtomobil->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("tipavtomobila")).toString()));
				ui->txt_registrska->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("registrskastevilka")).toString()));
				ui->txt_kilometri->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("razdalja")).toString()));
				ui->txt_kilometrina->setText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("kilometrina")).toString()));

				ui->txt_naslovprejemnika->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslov")).toString()));
				ui->txt_naslovpodjetja->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("naslovpodjetja")).toString()));
				ui->txt_opombe->setPlainText(prevedi(sql_napolni.value(sql_napolni.record().indexOf("opombe")).toString()));

				ui->txt_datum->setDate(QDate::fromString(prevedi(sql_napolni.value(sql_napolni.record().indexOf("datum")).toString()), "dd'.'MM'.'yyyy"));

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("zajtrk2")).toString()) == "zajtrk2") {
					ui->txt_zajtrk2->setChecked(true);
				}
				else {
					ui->txt_zajtrk2->setChecked(false);
				}

				if ( prevedi(sql_napolni.value(sql_napolni.record().indexOf("zajtrk3")).toString()) == "zajtrk3") {
					ui->txt_zajtrk3->setChecked(true);
				}
				else {
					ui->txt_zajtrk3->setChecked(false);
				}

				bool ok;
				QSqlQuery sql_combo;
				sql_combo.prepare("SELECT * FROM podjetje WHERE ime LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("podjetje")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_podjetje->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();
				sql_combo.prepare("SELECT * FROM uporabniki WHERE priimek LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("naslovnik")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_prejemnik->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();
				sql_combo.prepare("SELECT * FROM uporabniki WHERE ime LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("naslovnikime")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_prejemnikime->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();
				sql_combo.prepare("SELECT * FROM sif_nazivi WHERE naziv LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("naziv")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_nazivprejemnika->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();

				sql_combo.prepare("SELECT * FROM sif_namen WHERE namen LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("namen")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_namen->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();
				sql_combo.prepare("SELECT * FROM sif_prevoz WHERE prevoz LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("prevoznosredstvo")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_prevoz->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();
				sql_combo.prepare("SELECT * FROM podjetje WHERE odgovornaoseba LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("predlagatelj")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_predlagatelj->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();
				sql_combo.prepare("SELECT * FROM sif_nazivi WHERE naziv LIKE '" + sql_napolni.value(sql_napolni.record().indexOf("nazivpredlagatelja")).toString() + "'");
				sql_combo.exec();
				if (sql_combo.next()) {
					ui->txt_nazivpredlagatelja->setCurrentIndex(sql_combo.value(sql_combo.record().indexOf("id")).toString().toInt(&ok, 10));
				}
				sql_combo.clear();

			}
		}
		base.close();
	}

	// insert widgets
	wid_stroski *widst = new wid_stroski;
	ui->wid_st->setWidget(widst);
	QObject::connect(this, SIGNAL(prenos(QString)),
				 widst , SLOT(prejem(QString)));
	prenos(ui->txt_stnaloga->text());
	this->disconnect();

	QObject::connect(widst, SIGNAL(prenesi()),
					 this, SLOT(sprejmist()));

	wid_potovanja *widpot = new wid_potovanja;
	ui->wid_pot->setWidget(widpot);
	QObject::connect(this, SIGNAL(prenos(QString)),
				 widpot , SLOT(prejem(QString)));
	prenos(ui->txt_stnaloga->text());
	this->disconnect();

	QObject::connect(widpot, SIGNAL(prenesi()),
					 this, SLOT(sprejmipot()));

	izracun();

}

QString potninalogi::pretvori(QString besedilo) {

	return kodiranje().zakodiraj(besedilo);

}

QString potninalogi::prevedi(QString besedilo) {

	return kodiranje().odkodiraj(besedilo);

}

void potninalogi::izracun() {

	double kilometri = 0;
	double cenaprevoza = 0;
	double ostalo = 0;

	QDateTime prvidatum = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
	QDateTime zadnjidatum = QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm");
	int razlika;

	bool *ok;

	QString app_path = QApplication::applicationDirPath();
	QString dbase_path = app_path + "/base.bz";

	QSqlDatabase base = QSqlDatabase::addDatabase("QSQLITE", "izracun");
	base.setDatabaseName(dbase_path);
	base.database();
	base.open();
	if(base.isOpen() != true){
		QMessageBox msgbox;
		msgbox.setText("Baze ni bilo moc odpreti");
		msgbox.setInformativeText("Zaradi neznanega vzroka baza ni odprta. Do napake je prislo pri uvodnem preverjanju baze.");
		msgbox.exec();
	}
	else {
		// stroski, povezani s potjo
		QSqlQuery sql_pot;
		sql_pot.prepare("SELECT * FROM pot WHERE potninalog LIKE '" + pretvori(ui->txt_stnaloga->text()) + "'");
		sql_pot.exec();
		while ( sql_pot.next() ) {
			QString i = prevedi(sql_pot.value(sql_pot.record().indexOf("kilometri")).toString());
			double x = i.replace(",", ".").toDouble(ok);
			kilometri = kilometri + x;

			// fill dates
			if ( prvidatum == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
				prvidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casodhod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( zadnjidatum == QDateTime::fromString("01.01.1970 01:00", "dd'.'MM'.'yyyy' 'hh':'mm") ) {
				zadnjidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casprihod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( prvidatum.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casodhod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) < 0 ) {
				prvidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casodhod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
			if ( zadnjidatum.secsTo(QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casprihod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm")) > 0 ) {
				zadnjidatum = QDateTime::fromString(prevedi(sql_pot.value(sql_pot.record().indexOf("casprihod")).toString()), "dd'.'MM'.'yyyy' 'hh':'mm");
			}
		}

		ui->txt_kilometri->setText(QString::number(kilometri, 'f', 3));
		cenaprevoza = kilometri * ui->txt_kilometrina->text().toDouble(ok);
		ui->txt_cenaprevoza->setText(QString::number(cenaprevoza, 'f', 2));

		// dnevi
		razlika =	prvidatum.daysTo(zadnjidatum);
		ui->txt_dnevi->setText(QString::number(razlika, 10));

		//ure
		int minute = prvidatum.secsTo(zadnjidatum)/60;
		int ure = minute/60;
		minute = minute - ure*60;
		ure = ure - razlika*24;

		ui->txt_ure->setText(QString::number(ure, 10) + " ur " + QString::number(minute, 10) + " minut");

		ui->txt_dnevnica3->setText(ui->txt_dnevi->text());
		ui->txt_dnevnica2->setText("0");
		ui->txt_dnevnica1->setText("0");

		if ( ure >= 12 ) {
			ui->txt_dnevnica3->setText(QString::number(ui->txt_dnevi->text().toInt(ok) + 1, 10));
		}
		else if ( ure >= 8 ) {
			ui->txt_dnevnica2->setText("1");
		}
		else if ( ure >= 6 ) {
			ui->txt_dnevnica1->setText("1");
		}

		// izracun dnevnic
		if ( ui->txt_priznajdnevnico->isChecked() ) {
			double cenadnevnice = 0;
			cenadnevnice = 7.45 * ui->txt_dnevnica1->text().toDouble(ok);
			if ( ui->txt_zajtrk2->isChecked() ) {
				cenadnevnice = cenadnevnice + 10.68 * ui->txt_dnevnica2->text().toDouble(ok) * 0.85;
			}
			else {
				cenadnevnice = cenadnevnice + 10.68 * ui->txt_dnevnica2->text().toDouble(ok);
			}
			if ( ui->txt_zajtrk3->isChecked() ) {
				cenadnevnice = cenadnevnice + 21.39 * ui->txt_dnevnica3->text().toDouble(ok) * 0.90;
			}
			else {
				cenadnevnice = cenadnevnice + 21.39 * ui->txt_dnevnica3->text().toDouble(ok);
			}
			ui->txt_cenaodsotnosti->setText(QString::number(cenadnevnice, 'f', 2));
		}
		else {
			ui->txt_cenaodsotnosti->setText("0.00");
		}
		// ostali stroski
		QSqlQuery sql_ostalo;
		sql_ostalo.prepare("SELECT * FROM stroski WHERE potninalog LIKE '" + pretvori(ui->txt_stnaloga->text()) + "'");
		sql_ostalo.exec();
		while ( sql_ostalo.next() ) {
			ostalo = ostalo + prevedi(sql_ostalo.value(sql_ostalo.record().indexOf("cena")).toString()).replace(",", ".").toDouble(ok);
		}
		ui->txt_ostalo->setText(QString::number(ostalo, 'f', 2));

		// stroski skupaj
		ui->txt_skupaj->setText(QString::number(ui->txt_cenaprevoza->text().toDouble(ok) + ui->txt_cenaodsotnosti->text().toDouble(ok) +
												ui->txt_ostalo->text().toDouble(ok), 'f', 2));
	}
	base.close();

}

void potninalogi::sprejmist() {

	izracun();

}

void potninalogi::sprejmipot() {

	izracun();

}

void potninalogi::on_txt_priznajdnevnico_toggled() {

	izracun();

}

void potninalogi::on_txt_zajtrk2_toggled() {

	izracun();

}

void potninalogi::on_txt_zajtrk3_toggled() {

	izracun();

}

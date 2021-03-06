#include "emission.h"
#include <qdebug.h>
#include <QSerialPortInfo>
#include <iostream>
#include <QRegExp>
#include <qregularexpression.h>

#include <qserialportinfo>
#include <qserialport>
#include <qdebug>
#include <QRegExp>
#include <QtSql/QtSql>


Emission::Emission(QObject *parent) : QObject(parent) {

	// Initialisation connection port s?rie
	port = new QSerialPort( this );
	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(onSerialPortReadyRead()));
	
	port->setPortName("COM9");  // Port d'?coute

	// Param?tre de la communication : Cours communication s?rie avec QT de Mr.Gremont
	port->open(QIODevice::ReadWrite);
	port->setBaudRate(QSerialPort::Baud9600);
	port->setDataBits(QSerialPort::DataBits::Data8);
	port->setParity(QSerialPort::Parity::NoParity);
	port->setStopBits(QSerialPort::StopBits::OneStop);
	port->setFlowControl(QSerialPort::NoFlowControl);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Initialisation connection Base de donn?e
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.50"); // Adresse VM base de donn?e
	db.setUserName("superuser"); // Username base de donn?e
	db.setPassword("superuser"); // Mot de passe base de donn?e
	db.setDatabaseName("GPS"); // Table base de donn?e

	if (db.open())
	{
		qDebug() << "Connexion r?ussie";
	}
	else
	{
		qDebug() << "Connexion ?chou?";
	}
}


void Emission::onSerialPortReadyRead() {

	QByteArray TrameRecu = port->read(port->bytesAvailable()); // Lesture des messages recus
	QString str(TrameRecu);
	Trame += str; // Ajout des messages recu a la suite afin d'avoir une trame compl?te ( Reception 8 bit par 8 bit )

	QRegExp startMatch("GPGGA(.+)"); // Regex pour s?lectionner le d?but de la tramme car nous savons par quoi elle commence
	QRegExp stopMatch("(\\*42)");	// Regex pour s?lectioner la fin de la tramme car nous savons par quoi elle finit

	int startByte = startMatch.indexIn( Trame ); // J'utilise le d?but du regex pour charcher la trame compl?te la plus r?cente

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if ( startByte > -1 && stopMatch.indexIn( Trame, startByte + 1) > -1) {

		QStringList split = Trame.split(','); // Je pr?pare un d?coupage ? chaque " , "


		QRegExp regex("GPGGA,(.+)(\\*42)"); // Je combinne le d?but et fin de regex

		int test = regex.indexIn(Trame);	// J'applique mon regex complet sur la trame compl?te

		QStringList list = regex.capturedTexts();	// Je transforme en liste chaque information de la trame

		Trame.replace(0, stopMatch.indexIn(Trame, startByte + 1), "");

		
		QStringList data = list.at(1).split(',', Qt::SkipEmptyParts);// D?coupe de la chaine ? chaque virgules


		QString Latitude = data.at(1) // Case 2 = Longitude Trame
			, Longitude = data.at(3)	// Case 4 = Latitude Trame
			, Timestamp = data.at(0);	// Case 1 = Heure-Minute-Seconde Trame

		//qDebug() << Latitude << Longitude;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		int LongitudeFausse = Longitude.indexOf("."), LatitudeFausse = Latitude.indexOf(".");
			
		// Je d?cale mes valeurs de deux cran afin d'avoir des longeurs valide car valeur trop grande
		Longitude.insert(LongitudeFausse - 2, ","); 
		Latitude.insert(LatitudeFausse - 2, ",");


		// Conversion des valeurs en degr?/min
		QStringList LatitudeSplit = Latitude.split(","); // Je s?pare mon entier et mon d?cimal pour les traiter s?par?ment

		double LatitudeDecimal = LatitudeSplit.at(1).toDouble() / 60; // LatitudeSplit.at(1) = Entier Latitude non trait?
		double LatitudeEntier = LatitudeSplit.at(0).toDouble();		// LatitudeSplit.at(0) = D?cimal Latitude non trait?
		double LatitudeRelle = LatitudeDecimal + LatitudeEntier;



		// La longitude est au m?me format que la Latitude donc la conversion est la m?me
		QStringList LongitudeSplit = Longitude.split(",");

		double LongitudeDecimal = LongitudeSplit.at(1).toDouble() / 60;
		double LongitudeEntier = LongitudeSplit.at(0).toDouble();
		double LongitudeRelle = LongitudeDecimal + LongitudeEntier;

		qDebug() << LatitudeDecimal << "+" << LatitudeEntier << "=" << LatitudeRelle << "\n";
		qDebug() << LongitudeDecimal << "+" << LongitudeEntier << "=" << LongitudeRelle << "\n";

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		// Envoie des donn?e en Base de donn?e gr?ce a une querry
		QSqlQuery query("INSERT INTO `gps`(`latitude`, `longitude`, `heure`) VALUES (?,?,'" + Timestamp + "')");
		
		// Je fais un addBindValue pour ?viter une erreur : l'expression doit ?tre de type int?gral ou enum non d?limit?
		query.addBindValue(LatitudeRelle);
		query.addBindValue(LongitudeRelle);
		query.exec();
		
		
	}
	
}
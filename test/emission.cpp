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

	// Initialisation connection port série
	port = new QSerialPort( this );
	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(onSerialPortReadyRead()));
	
	port->setPortName("COM9");  // Port d'écoute

	// Paramètre de la communication : Cours communication série avec QT de Mr.Gremont
	port->open(QIODevice::ReadWrite);
	port->setBaudRate(QSerialPort::Baud9600);
	port->setDataBits(QSerialPort::DataBits::Data8);
	port->setParity(QSerialPort::Parity::NoParity);
	port->setStopBits(QSerialPort::StopBits::OneStop);
	port->setFlowControl(QSerialPort::NoFlowControl);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Initialisation connection Base de donnée
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.50"); // Adresse VM base de donnée
	db.setUserName("superuser"); // Username base de donnée
	db.setPassword("superuser"); // Mot de passe base de donnée
	db.setDatabaseName("GPS"); // Table base de donnée

	if (db.open())
	{
		qDebug() << "Connexion réussie";
	}
	else
	{
		qDebug() << "Connexion échoué";
	}
}


void Emission::onSerialPortReadyRead() {

	QByteArray TrameRecu = port->read(port->bytesAvailable()); // Lesture des messages recus
	QString str(TrameRecu);
	Trame += str; // Ajout des messages recu a la suite afin d'avoir une trame complète ( Reception 8 bit par 8 bit )

	QRegExp startMatch("GPGGA(.+)"); // Regex pour sélectionner le début de la tramme car nous savons par quoi elle commence
	QRegExp stopMatch("(\\*42)");	// Regex pour sélectioner la fin de la tramme car nous savons par quoi elle finit

	int startByte = startMatch.indexIn( Trame ); // J'utilise le début du regex pour charcher la trame complète la plus récente

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if ( startByte > -1 && stopMatch.indexIn( Trame, startByte + 1) > -1) {

		QStringList split = Trame.split(','); // Je prépare un découpage à chaque " , "


		QRegExp regex("GPGGA,(.+)(\\*42)"); // Je combinne le début et fin de regex

		int test = regex.indexIn(Trame);	// J'applique mon regex complet sur la trame complète

		QStringList list = regex.capturedTexts();	// Je transforme en liste chaque information de la trame

		Trame.replace(0, stopMatch.indexIn(Trame, startByte + 1), "");

		
		QStringList data = list.at(1).split(',', Qt::SkipEmptyParts);// Découpe de la chaine à chaque virgules


		QString Latitude = data.at(1) // Case 2 = Longitude Trame
			, Longitude = data.at(3)	// Case 4 = Latitude Trame
			, Timestamp = data.at(0);	// Case 1 = Heure-Minute-Seconde Trame

		//qDebug() << Latitude << Longitude;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		int LongitudeFausse = Longitude.indexOf("."), LatitudeFausse = Latitude.indexOf(".");
			
		// Je décale mes valeurs de deux cran afin d'avoir des longeurs valide car valeur trop grande
		Longitude.insert(LongitudeFausse - 2, ","); 
		Latitude.insert(LatitudeFausse - 2, ",");


		// Conversion des valeurs en degré/min
		QStringList LatitudeSplit = Latitude.split(","); // Je sépare mon entier et mon décimal pour les traiter séparément

		double LatitudeDecimal = LatitudeSplit.at(1).toDouble() / 60; // LatitudeSplit.at(1) = Entier Latitude non traité
		double LatitudeEntier = LatitudeSplit.at(0).toDouble();		// LatitudeSplit.at(0) = Décimal Latitude non traité
		double LatitudeRelle = LatitudeDecimal + LatitudeEntier;



		// La longitude est au même format que la Latitude donc la conversion est la même
		QStringList LongitudeSplit = Longitude.split(",");

		double LongitudeDecimal = LongitudeSplit.at(1).toDouble() / 60;
		double LongitudeEntier = LongitudeSplit.at(0).toDouble();
		double LongitudeRelle = LongitudeDecimal + LongitudeEntier;

		qDebug() << LatitudeDecimal << "+" << LatitudeEntier << "=" << LatitudeRelle << "\n";
		qDebug() << LongitudeDecimal << "+" << LongitudeEntier << "=" << LongitudeRelle << "\n";

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		// Envoie des donnée en Base de donnée grâce a une querry
		QSqlQuery query("INSERT INTO `gps`(`latitude`, `longitude`, `heure`) VALUES (?,?,'" + Timestamp + "')");
		
		// Je fais un addBindValue pour éviter une erreur : l'expression doit être de type intégral ou enum non délimité
		query.addBindValue(LatitudeRelle);
		query.addBindValue(LongitudeRelle);
		query.exec();
		
		
	}
	
}
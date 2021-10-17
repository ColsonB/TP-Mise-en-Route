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

	// Initialisation connection port s�rie
	port = new QSerialPort( this );
	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(onSerialPortReadyRead()));
	
	port->setPortName("COM9");  // Port d'�coute

	// Param�tre de la communication
	port->open(QIODevice::ReadWrite);
	port->setBaudRate(QSerialPort::Baud9600);
	port->setDataBits(QSerialPort::DataBits::Data8);
	port->setParity(QSerialPort::Parity::NoParity);
	port->setStopBits(QSerialPort::StopBits::OneStop);
	port->setFlowControl(QSerialPort::NoFlowControl);

	// Initialisation connection Base de donn�e
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.50"); // Adresse VM base de donn�e
	db.setUserName("superuser"); // Username base de donn�e
	db.setPassword("superuser"); // Mot de passe base de donn�e
	db.setDatabaseName("GPS"); // Table base de donn�e

	if (db.open())
	{
		qDebug() << "Connexion r�ussie";
	}
	else
	{
		qDebug() << "Connexion �chou�";
	}
}


void Emission::onSerialPortReadyRead() {

	QByteArray TrameRecu = port->read(port->bytesAvailable()); // Lesture des messages recus
	QString str(TrameRecu);
	Trame += str; // Ajout des messages recu a la suite afin d'avoir une trame compl�te ( Reception 8 bit par 8 bit )

	QRegExp startMatch("GPGGA(.+)"); // Regex pour s�lectionner le d�but de la tramme car nous savons par quoi elle commence
	QRegExp stopMatch("(\\*42)");	// Regex pour s�lectioner la fin de la tramme car nous savons par quoi elle finit

	int startByte = startMatch.indexIn( Trame ); // J'utilise le d�but du regex pour charcher la trame compl�te la plus r�cente

	if ( startByte > -1 && stopMatch.indexIn( Trame, startByte + 1) > -1) {

		QStringList split = Trame.split(','); // Je pr�pare un d�coupage � chaque " , "


		QRegExp regex("GPGGA,(.+)(\\*42)"); // Je combinne le d�but et fin de regex

		int test = regex.indexIn(Trame);	// J'applique mon regex complet sur la trame compl�te

		QStringList list = regex.capturedTexts();	// Je transforme en liste chaque information de la trame

		Trame.replace(0, stopMatch.indexIn(Trame, startByte + 1), "");

		
		QStringList data = list.at(1).split(',', Qt::SkipEmptyParts);// D�coupe de la chaine � chaque virgules

		QString Longitude = data.at(1) // Case 2 = Longitude Trame
			, Latitude = data.at(3)	// Case 4 = Latitude Trame
			, Timestamp = data.at(0);	// Case 1 = Heure-Minute-Seconde Trame

		qDebug() << Latitude << Longitude;

		QSqlQuery query;
		// Envoie des donn�e en Base de donn�e gr�ce a une querry
		query.exec("INSERT INTO `gps`(`latitude`, `longitude`, `heure`) VALUES ('"+ Latitude +"','"+ Longitude +"','"+ Timestamp +"')");
		
		
	}
	
}
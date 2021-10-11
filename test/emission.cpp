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

	port = new QSerialPort( this );
	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(onSerialPortReadyRead()));
	port->setPortName("COM9");
	port->open(QIODevice::ReadWrite);
	port->setBaudRate(QSerialPort::Baud9600);
	port->setDataBits(QSerialPort::DataBits::Data8);
	port->setParity(QSerialPort::Parity::NoParity);
	port->setStopBits(QSerialPort::StopBits::OneStop);
	port->setFlowControl(QSerialPort::NoFlowControl);


	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.50");
	db.setUserName("superuser");
	db.setPassword("superuser");
	db.setDatabaseName("GPS");

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

	QByteArray TrameRecu = port->read(port->bytesAvailable());
	QString str(TrameRecu);
	Trame += str;

	QRegExp startMatch("GPGGA(.+)");
	QRegExp stopMatch("(\\*42)");
	int startByte = startMatch.indexIn( Trame );

	if ( startByte > -1 && stopMatch.indexIn( Trame, startByte + 1) > -1) {

		QStringList split = Trame.split(',');


		QRegExp regex("GPGGA,(.+)(\\*42)");
		int test = regex.indexIn(Trame);
		QStringList list = regex.capturedTexts();
		Trame.replace(0, stopMatch.indexIn(Trame, startByte + 1), "");
		// -- Découpe notre chaine à chaque virgules
		QStringList data = list.at(1).split(',', Qt::SkipEmptyParts);

		QString Longitude = data.at(1) // data.at( 1 )
			, Latitude = data.at(3)
			, Timestamp = data.at(0);

		qDebug() << Latitude << Longitude;
		QSqlQuery query;
		query.exec("INSERT INTO `gps`(`latitude`, `longitude`, `heure`) VALUES ('"+ Latitude +"','"+ Longitude +"','"+ Timestamp +"')");
		
		
	}
	
}
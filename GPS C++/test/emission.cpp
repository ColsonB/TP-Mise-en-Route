#include "emission.h"
#include <qdebug.h>
#include <QSerialPortInfo>
#include <iostream>
#include <QRegExp>
#include <qregularexpression.h>


Emission::Emission(QObject *parent)
	: QObject(parent)
{
	port = new QSerialPort( this );
	QObject::connect(port, SIGNAL(readyRead()), this, SLOT(onSerialPortReadyRead()));
	port->setPortName("COM1");
	port->open(QIODevice::ReadWrite);
		port->setBaudRate(QSerialPort::Baud9600);
		port->setDataBits(QSerialPort::DataBits::Data8);
		port->setParity(QSerialPort::Parity::NoParity);
		port->setStopBits(QSerialPort::StopBits::OneStop);
		port->setFlowControl(QSerialPort::NoFlowControl);
}


void Emission::onSerialPortReadyRead() {

	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("192.168.64.50");
	db.setDatabaseName("GPS");
	db.setUserName("root");
	db.setPassword("root");
	QSqlQuery query(db);

	//QString InsertBDD = "INSERT INTO `gps`(`latitude`, `longitude`, `heure`, `altitude`) VALUES ('" + InfoTrame[2] + "','" + InfoTrame[4] + "','" + InfoTrame[1] + "','" + InfoTrame[9] + "')";

	QByteArray TrameRecu = port->readAll();
	qDebug() << TrameRecu.size();
	Trame = Trame + TrameRecu.toStdString().c_str();
	qDebug() << Trame;


	QRegExp regex("(.+)*0E");
	qDebug() << regex.indexIn(Trame);

	if (regex.indexIn(Trame) >> -1)
	{
		QStringList InfoTrame = Trame.split(QLatin1Char(','), Qt::SkipEmptyParts);

		for (int i = 0; i < InfoTrame.size(); i++)
		{
			qDebug() << InfoTrame[i];

		}

		this->db->InsertBDD(InfoTrame);

	}
	
	
	
	//const std::string str = "$GPGGA, 064036.289, 4836.5375, N, 00740.9373, E, 1, 04, 3.2, 200.2, M, , , , 0000*0E";
	//std::regex reg("[A-Z$*]");
	//std::string line = std::regex_replace(str, reg, "");
	//std::cout << line << "\n";

	
}
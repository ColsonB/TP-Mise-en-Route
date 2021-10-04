#pragma once

#include <QObject>
#include <QSerialPort>
#include <QtSql/QtSql>
#include <QSqlQuery>

class Emission : public QObject
{
	Q_OBJECT

public:
	Emission(QObject *parent = Q_NULLPTR);
public slots:
	void onSerialPortReadyRead();
private:
	QSerialPort * port;
	QString Trame;
	QSqlDatabase * db;

};

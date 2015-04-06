#pragma once

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>

#include "Service.hpp"

class ArduinoService : public QObject, public Service
{
	Q_OBJECT
public:
	ArduinoService();

	void disconnected(int client) override;

	void send(QString cmd);
	QStringList serialInterfaces() const;

	void disconnect();
	void connect();

	QString serialInterface() const;

public slots:
	void readData();

private:

	bool internalDisconnect();

	bool m_connected = false;
	QSerialPort serialPort;
	QString m_serialInterface;
};

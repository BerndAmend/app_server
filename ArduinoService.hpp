#pragma once

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>

#include "Service.hpp"

class ArduinoService : public Service
{
public:
	ArduinoService();

	void disconnected(int client) override;

	void send(QString cmd);
	QStringList serialInterfaces() const;

	void disconnect();
	void connect();

	QString serialInterface() const;

private:

	bool internalDisconnect();

	bool m_connected = false;
	QSerialPort serialPort;
	QString m_serialInterface;
};

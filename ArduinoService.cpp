#include "ArduinoService.hpp"

ArduinoService::ArduinoService()
	: Service("Arduino")
{
	connect();

	registerCommand("send", [this](int client, const QJsonObject &args, QJsonObject &result) {
		send(args["data"].toString());
	});
}

void ArduinoService::disconnected(int client) {

}

void ArduinoService::send(QString cmd)
{
	cmd += "\r";
	serialPort.write(cmd.toUtf8());
}


QStringList ArduinoService::serialInterfaces() const {
	QStringList result;

	for(const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
		if(info.manufacturer().startsWith("Arduino"))
			result.push_back(info.portName());
	}

	return result;
}

void ArduinoService::disconnect() {
	if(m_connected) {
		if(internalDisconnect()) {
			m_connected = false;
			//emit disconnected();
		}
	}
}

void ArduinoService::connect() {
	internalDisconnect();

	QStringList interfaces = serialInterfaces();
	if(!interfaces.isEmpty()) {

		// use the first interface if non was selected
		if(m_serialInterface.isEmpty()) {
			m_serialInterface = interfaces[0];
			//emit serialInterfaceChanged(m_serialInterface);
		}

		serialPort.setPortName(m_serialInterface);
		serialPort.setBaudRate(QSerialPort::Baud115200);
		serialPort.setDataBits(QSerialPort::Data8);
		serialPort.setParity(QSerialPort::NoParity);
		serialPort.setStopBits(QSerialPort::OneStop);
		serialPort.setFlowControl(QSerialPort::NoFlowControl);
		if (serialPort.open(QIODevice::ReadWrite)) {
			m_connected = true;
			//emit connected();
		}
	}
}

QString ArduinoService::serialInterface() const
{
	return m_serialInterface;
}

//void ArduinoService::setSerialInterface(QString arg)
//{
//	if (m_serialInterface == arg)
//		return;

//	m_serialInterface = arg;

//	connect();
//}

bool ArduinoService::internalDisconnect() {
	bool did_disconnect = false;
	if(serialPort.isOpen()) {
		serialPort.close();
		did_disconnect = true;
	}
	return did_disconnect;
}

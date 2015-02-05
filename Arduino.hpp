#pragma once

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QWebSocket>
#include <QWebSocketServer>
#include <memory>

class Arduino : public QObject
{
	Q_OBJECT

public:

private:
	Q_PROPERTY(quint16 webSocketServerPort READ webSocketServerPort WRITE setWebSocketServerPort NOTIFY webSocketServerPortChanged)

	Q_PROPERTY(bool connected READ isConnected)
	Q_PROPERTY(QString serialInterface READ serialInterface WRITE setSerialInterface NOTIFY serialInterfaceChanged)

public:
	explicit Arduino(QObject *parent = 0);
	~Arduino() override;

	Q_INVOKABLE void send(QString cmd);
	Q_INVOKABLE QStringList serialInterfaces() const {
		QStringList result;

		for(const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
			if(info.manufacturer().startsWith("Arduino"))
				result.push_back(info.portName());
		}

		return result;
	}

	void disconnect() {
		if(m_connected) {
			if(internal_disconnect()) {
				m_connected = false;
				emit disconnected();
			}
		}
	}

	Q_INVOKABLE void connect() {
		internal_disconnect();

		QStringList interfaces = serialInterfaces();
		if(!interfaces.isEmpty()) {

			// use the first interface if non was selected
			if(m_serialInterface.isEmpty()) {
				m_serialInterface = interfaces[0];
				emit serialInterfaceChanged(m_serialInterface);
			}

			serialPort.setPortName(m_serialInterface);
			serialPort.setBaudRate(QSerialPort::Baud115200);
			serialPort.setDataBits(QSerialPort::Data8);
			serialPort.setParity(QSerialPort::NoParity);
			serialPort.setStopBits(QSerialPort::OneStop);
			serialPort.setFlowControl(QSerialPort::NoFlowControl);
			if (serialPort.open(QIODevice::ReadWrite)) {
				m_connected = true;
				emit connected();
			}
		}
	}

	qint16 webSocketServerPort() const
	{
		return m_webSocketServerPort;
	}

	bool isConnected() const
	{
		return m_connected;
	}

	QString serialInterface() const
	{
		return m_serialInterface;
	}

signals:

	void connected();
	void disconnected();

	void serialInterfaceChanged(QString arg);

	void webSocketServerPortChanged(quint16 arg);

	void messageReceived(QString msg);

	void clientConnected(QUrl url);

public slots:

	void setWebSocketServerPort(quint16 arg)
	{
		if(m_webSocketServerPort == arg)
			return;

		m_webSocketServerPort = arg;

		internal_handleWebSocketServerState();

		emit webSocketServerPortChanged(arg);
	}

	void setSerialInterface(QString arg)
	{
		if (m_serialInterface == arg)
			return;

		m_serialInterface = arg;

		connect();

		emit serialInterfaceChanged(arg);
	}

	void messageFromClientReceived(QString cmd) {
		send(cmd);
	}

	void onNewWebSocketClientConnection() {
		QWebSocket *pSocket = m_qwebsocketServer->nextPendingConnection();

		QObject::connect(pSocket, &QWebSocket::textMessageReceived, this, &Arduino::messageFromClientReceived);
		QObject::connect(pSocket, &QWebSocket::disconnected, this, &Arduino::onWebSocketClientDisconnected);

		m_clients << pSocket;

		emit clientConnected(pSocket->requestUrl());
	}

	void onWebSocketClientDisconnected() {
		QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
		if (pClient) {
			m_clients.removeAll(pClient);
			pClient->deleteLater();
		}
	}

private:

	void internal_handleWebSocketServerState() {
		stopWebSocketServer();
		if (m_qwebsocketServer->listen(QHostAddress::Any, m_webSocketServerPort))
				qDebug() << "Arduino relay listening on port" << m_webSocketServerPort;
	}

	bool internal_disconnect() {
		bool did_disconnect = false;
		if(serialPort.isOpen()) {
			serialPort.close();
			did_disconnect = true;
		}
		m_qwebsocket.close();
		return did_disconnect;
	}

	void stopWebSocketServer() {
		if(m_qwebsocketServer) {
			m_qwebsocketServer->close();
		}
		qDeleteAll(m_clients.begin(), m_clients.end());
	}

	bool m_connected = false;
	QSerialPort serialPort;
	quint16 m_webSocketServerPort = 55777;
	QString m_serialInterface;

	std::unique_ptr<QWebSocketServer> m_qwebsocketServer;
	QList<QWebSocket*> m_clients;

	QWebSocket m_qwebsocket;
};

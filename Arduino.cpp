#include "Arduino.hpp"

Arduino::Arduino(QObject *parent)
	: QObject(parent)
	, m_qwebsocketServer(new QWebSocketServer(QStringLiteral("arduino relay"),
						 QWebSocketServer::NonSecureMode, this))
{
	QObject::connect(&m_qwebsocket, &QWebSocket::connected, this, &Arduino::connected);
	QObject::connect(&m_qwebsocket, &QWebSocket::disconnected, this, &Arduino::disconnected);
	QObject::connect(&m_qwebsocket, &QWebSocket::textMessageReceived, this, &Arduino::messageReceived);

	QObject::connect(m_qwebsocketServer.get(), &QWebSocketServer::newConnection,
					 this, &Arduino::onNewWebSocketClientConnection);
	//QObject::connect(m_qwebsocketServer, &QWebSocketServer::closed, this, &Arduino::closed);

	internal_handleWebSocketServerState();
}

Arduino::~Arduino()
{
	stopWebSocketServer();
}

void Arduino::send(QString cmd)
{
	cmd += "\r";
	serialPort.write(cmd.toUtf8());
}

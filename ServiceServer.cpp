#include "ServiceServer.hpp"
#include <QJsonDocument>
#include <QDebug>

ServiceServer::ServiceServer(quint16 port)
	: _server(QStringLiteral("WebSocketCommunication"), QWebSocketServer::NonSecureMode)
{
	QObject::connect(&_server, &QWebSocketServer::newConnection, this, &ServiceServer::onNewWebSocketClientConnection);

	_server.listen(QHostAddress::Any, port);
}

ServiceServer::~ServiceServer()
{
	_server.close();
	qDeleteAll(_client_lookup.begin(), _client_lookup.end());
}

void ServiceServer::addService(Service *service)
{
	_handler[service->getName()] = service;
	service->setHandler(this);
}

void ServiceServer::removeService(Service *service)
{
	_handler.remove(service->getName());
	service->setHandler(nullptr);
}

void ServiceServer::notify(const Service *service, int client, const QString &cmd, const QJsonObject &data)
{
	QJsonObject pkg;
	pkg["type"] = "event";
	pkg["service"] = service->getName();
	pkg["cmd"] = cmd;
	pkg["data"] = data;
	QString doc = QJsonDocument(pkg).toJson();
	if(client != 0) {
		if(_client_lookup.contains(client)) {
			_client_lookup[client]->sendTextMessage(doc);
		} else {
			qDebug() << "Client " << client << " is unknown";
		}
	} else {
		for(QWebSocket *socket : _client_lookup) {
			socket->sendTextMessage(doc);
		}
	}
}


void ServiceServer::onNewWebSocketClientConnection() {
	QWebSocket *pSocket = _server.nextPendingConnection();

	QObject::connect(pSocket, &QWebSocket::textMessageReceived, this, &ServiceServer::messageFromClientReceived);
	QObject::connect(pSocket, &QWebSocket::disconnected, this, &ServiceServer::onWebSocketClientDisconnected);

	_last_id++;
	_client_lookup[_last_id] = pSocket;
	_clients[pSocket] = _last_id;
}

void ServiceServer::onWebSocketClientDisconnected() {
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (pClient) {
		int id = _clients[pClient];
		for(Service *service : _handler)
			service->disconnected(id);

		_client_lookup.remove(id);
		_clients.remove(pClient);
		pClient->deleteLater();
	}
}

void ServiceServer::messageFromClientReceived(QString msg) {
	QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
	QJsonObject obj = doc.object();
	QString type = obj["type"].toString();
	QWebSocket *client = qobject_cast<QWebSocket *>(sender());
	if(type == "request") {
		QJsonObject result;
		result["type"] = "reply";
		result["id"] = obj["id"];
		result["service"] = obj["service"];

		if(_handler.contains(obj["service"].toString())) {
			Service *service = _handler[obj["service"].toString()];

			QJsonObject rObj;
			service->commandHandler(_clients[client], obj, rObj);
			result["data"] = rObj;
		} else {
			QJsonObject data;
			data["error"] = ErrorCode_Unknown_Service;
			result["data"] = data;
			qDebug() << "unknown service " << obj["service"].toString();
		}
		client->sendTextMessage(QJsonDocument(result).toJson());
	} else {
		qDebug() << "unknown message type received: " << type;
	}
}


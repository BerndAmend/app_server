#pragma once

#include <QObject>
#include <QString>
#include <QMap>

#include <QWebSocket>
#include <QWebSocketServer>

#include <Service.hpp>

// not thread-safe
class ServiceServer : public QObject
{
	Q_OBJECT
public:
	ServiceServer(quint16 port);
	~ServiceServer() override;

	void addService(Service *service);
	void removeService(Service *service);

	void notify(const Service *service, int client, const QString &cmd, const QJsonObject &data);

public slots:
	void onNewWebSocketClientConnection();
	void onWebSocketClientDisconnected();
	void messageFromClientReceived(QString msg);

private:
	QMap<QString, Service*> _handler;
	QWebSocketServer _server;

	int _last_id = 0;
	QMap<QWebSocket*, int> _clients;
	QMap<int, QWebSocket*> _client_lookup;
};

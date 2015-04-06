#pragma once

#include <QObject>
#include <QString>
#include <QMap>

#include <QWebSocket>
#include <QWebSocketServer>

#include <Service.hpp>
#include <mutex>

class ServiceServer : public QObject
{
	Q_OBJECT
public:
	ServiceServer(quint16 port);
	~ServiceServer() override;

	void notifyFromOtherThread(const Service *service, int client, const QString &cmd, const QJsonObject &data);

public slots:
	void addService(Service *service);
	void removeService(Service *service);

	void notify(const Service *service, int client, const QString &cmd, const QJsonObject &data);

	void onNewWebSocketClientConnection();
	void onWebSocketClientDisconnected();
	void messageFromClientReceived(QString msg);

signals:
	void notifySignal(const Service *service, int client, const QString cmd, const QJsonObject data);

private:
	QMap<QString, Service*> _handler;
	QWebSocketServer _server;

	int _last_id = 0;
	QMap<QWebSocket*, int> _clients;
	QMap<int, QWebSocket*> _client_lookup;
};

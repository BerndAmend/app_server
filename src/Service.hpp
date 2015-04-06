#pragma once

#include <QJsonObject>
#include <functional>
#include <QMap>

class ServiceServer;

enum ErrorCode {
	ErrorCode_None,
	ErrorCode_Unknown_Service,
	ErrorCode_Unknown_Cmd
};

class Service {
public:
	typedef std::function<void(int, const QJsonObject&args, QJsonObject &result)> CommandCallback;
	Service(const QString &name);
	virtual ~Service() {}

	QString getName() const;

protected:
	void unregisterCommand(const QString &name);
	void registerCommand(const QString &name, const CommandCallback &callback);

	// client == 0: broadcast
	void notify(int client, const QString &cmd, const QJsonObject &obj);
	void notifyFromOtherThread(int client, const QString &cmd, const QJsonObject &obj);

	virtual void disconnected(int client) = 0;

private:
	const QString _name;
	void setHandler(ServiceServer *server);

	ServiceServer *_server = nullptr;

	void commandHandler(int client, const QJsonObject &args, QJsonObject &result);

	QMap<QString, CommandCallback> _commands;

	friend class ServiceServer;
};

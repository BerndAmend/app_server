#include "Service.hpp"
#include "ServiceServer.hpp"
#include <QDebug>

Service::Service(const QString &name)
	: _name(name)
{}

QString Service::getName() const
{
	return _name;
}

void Service::unregisterCommand(const QString &name)
{
	_commands.remove(name);
}

void Service::registerCommand(const QString &name, const CommandCallback &callback)
{
	_commands[name] = callback;
}

void Service::notify(int client, const QString &cmd, const QJsonObject &obj) {
	if(_server)
		_server->notify(this, client, cmd, obj);
	else
		qDebug() << "_server == nullptr";
}

void Service::notifyFromOtherThread(int client, const QString &cmd, const QJsonObject &obj)
{
	if(_server)
		_server->notifyFromOtherThread(this, client, cmd, obj);
	else
		qDebug() << "_server == nullptr";
}

void Service::commandHandler(int client, const QJsonObject &args, QJsonObject &result)
{
	QString cmd = args["cmd"].toString();
	if(!_commands.contains(cmd)) {
		result["error"] = ErrorCode_Unknown_Cmd;
		qDebug() << "Unknown cmd " << args["cmd"].toString() << " in service " << getName();
	} else {
		result["error"] = ErrorCode_None;
		_commands[cmd](client, args["data"].toObject(), result);
	}
}

void Service::setHandler(ServiceServer *server) {
	_server = server;
}

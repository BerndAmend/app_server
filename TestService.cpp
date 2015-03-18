#include "TestService.hpp"
#include <iostream>
#include <QJsonDocument>

TestService::TestService()
	: Service("Test")
{
	connect(_timer, SIGNAL(timeout()), this, SLOT(doSomething()));
	_timer->start(5000);

	registerCommand("ping", [](int client, const QJsonObject &args, QJsonObject &result) {
		std::cout << "TestService: Client " << client << " called with " << QJsonDocument(args).toJson().toStdString() << std::endl;
		result["result"] = "pong";
	});

	registerCommand("sendOnlyToMe", [this](int client, const QJsonObject &args, QJsonObject &result) {
		std::cout << "TestService: Send future notifications only to " << client << std::endl;
		_notifyAdress = client;
	});
}

void TestService::disconnected(int client)
{
	std::cout << "TestService: Client " << client << " disconnect" << std::endl;
}

void TestService::doSomething()
{
	QJsonObject obj;
	obj["bla"] = "Fisch";
	obj["counter"] = _counter++;
	notify(_notifyAdress, "beep", obj);
}

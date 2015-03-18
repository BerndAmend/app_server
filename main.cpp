#include <QApplication>
#include <QQmlApplicationEngine>
#include "HttpServer.hpp"
#include "ArduinoService.hpp"

#include "ServiceServer.hpp"
#include "TestService.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	HttpServer server(8080);

	ServiceServer service_server(55777);

	TestService test_service;
	service_server.addService(&test_service);

	ArduinoService arduino_service;
	service_server.addService(&arduino_service);
	return app.exec();
}

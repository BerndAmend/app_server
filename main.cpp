#include <QApplication>
#include <QQmlApplicationEngine>
#include "HttpServer.hpp"
#include "Arduino.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	HttpServer server(8080);
	Arduino arduino;
	return app.exec();
}

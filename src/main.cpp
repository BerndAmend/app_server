#include <QCoreApplication>
#include "HttpServer.hpp"
#include "ArduinoService.hpp"

#include "ServiceServer.hpp"
#include "TestService.hpp"
#include "OpenCVCameraService.hpp"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	const QString baseDir = APP_SERVER_SRC_DIR;

	HttpServer server(8080, {baseDir + "/srv"});

	ServiceServer service_server(55777);

	TestService test_service;
	service_server.addService(&test_service);

	ArduinoService arduino_service;
	service_server.addService(&arduino_service);

#ifdef __linux__
	OpenCVCameraService camera_service;
	service_server.addService(&camera_service);
#endif
	return app.exec();
}

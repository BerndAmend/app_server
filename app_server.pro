TEMPLATE = app

QT += qml quick widgets websockets serialport script
CONFIG += C++11

SOURCES += main.cpp \
	HttpServer.cpp \
	ServiceServer.cpp \
	TestService.cpp \
	Service.cpp \
    ArduinoService.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
	HttpServer.hpp \
	ServiceServer.hpp \
	Service.hpp \
	TestService.hpp \
    ArduinoService.hpp

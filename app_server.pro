TEMPLATE = app

QT += qml quick widgets websockets serialport
CONFIG += C++11

SOURCES += main.cpp \
	HttpServer.cpp \
	Arduino.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
	HttpServer.hpp \
	Arduino.hpp

TEMPLATE = app

CONFIG += C++14

QT += websockets script

!android {
	DEFINES += HAS_QSERIALPORT
	QT += serialport
}

linux {
	CONFIG +=link_pkgconfig
	PKGCONFIG += opencv
}

INCLUDEPATH += src

SOURCES += src/main.cpp \
	src/HttpServer.cpp \
	src/ServiceServer.cpp \
	src/TestService.cpp \
	src/Service.cpp \
	src/ArduinoService.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
	src/HttpServer.hpp \
	src/ServiceServer.hpp \
	src/Service.hpp \
	src/TestService.hpp \
	src/ArduinoService.hpp

linux {
	HEADERS += src/OpenCVCameraService.hpp
	SOURCES += src/OpenCVCameraService.cpp
}

DEFINES += APP_SERVER_SRC_DIR=\\\"$$PWD\\\"

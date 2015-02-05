#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QBuffer>
#include <QDir>

 class HttpServer : public QTcpServer
 {
	 Q_OBJECT
 public:
	 HttpServer(quint16 port, QObject* parent = 0);

	 void incomingConnection(qintptr socket) override;

 private slots:
	 void readClient();

	 void discardClient();

 private:
	QMimeDatabase _mime_database;

	QMap<QString, QString> _custom_mime_types = {{"fs", "application/sforth"}};

	QMap<int, QString> _return_codes	= {	{200, "OK"},
											{400, "Bad Request"},
											{403, "Forbidden"},
											{404, "Not Found"}};
 };

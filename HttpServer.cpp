#include "HttpServer.hpp"

HttpServer::HttpServer(quint16 port, QObject *parent)
	: QTcpServer(parent)
{
	if(!listen(QHostAddress::Any, port))
		qDebug() << "Couldn't listen on port";
}

void HttpServer::incomingConnection(qintptr socket)
{
	// When a new client connects, the server constructs a QTcpSocket and all
	// communication with the client is done over this QTcpSocket. QTcpSocket
	// works asynchronously, this means that all the communication is done
	// in the two slots readClient() and discardClient().
	QTcpSocket* s = new QTcpSocket(this);
	connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
	connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
	s->setSocketDescriptor(socket);

	qDebug() << "New Connection";
}

void HttpServer::readClient()
{
	// This slot is called when the client sent data to the server. The
	// server looks if it was a get req uest and sends a very simple HTML
	// document back.
	QTcpSocket* socket = (QTcpSocket*)sender();
	if (socket->canReadLine()) {
		QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
		if (tokens[0] == "GET") {

			int return_code = 400;
			QString mime_type = "text/html";
			QByteArray data;

			QString filename = "." + tokens[1];
			QFileInfo fileinfo { filename };
			QFile file { fileinfo.fileName() };
			if(fileinfo.isFile() && fileinfo.isReadable()) {
				return_code = 200;
				if(_custom_mime_types.contains(fileinfo.suffix())) {
					mime_type = _custom_mime_types[fileinfo.suffix()];
				} else {
					mime_type = _mime_database.mimeTypeForFile(fileinfo).name();
				}

				if (file.open(QIODevice::ReadOnly)) {
					data = file.readAll();
				} else {
					return_code = 404;
				}
			} else if(fileinfo.isDir()) {
				QTextStream os(&data);
				return_code = 200;
				os << "<!doctype html>\n"
					  "<html>\n"
					  "<head>\n"
					  "<meta charset=\"utf-8\">\n"
					  "<title>Index of " << fileinfo.fileName() << "</title>\n"
																   "</head>\n"
																   "<body>\n"
																   "<h1>Index of " << fileinfo.fileName() << "</h1>\n\n";

				for(const auto &file : QDir(fileinfo.fileName()).entryInfoList()) {
					os  << "<a href=\"" << file.fileName() << "\">" << file.fileName() << "</a><br>";
				}

				os << "</body></html>";

			} else {
				return_code = 404;
			}

			{
				QTextStream os(socket);
				os.setAutoDetectUnicode(true);
				os << "HTTP/1.0 " << return_code << _return_codes[return_code] << " \r\n"
				   << "Content-Type: " << mime_type << "; charset=\"utf-8\"\r\n"
				   << "Content-Length: " << data.length() << "\r\n"
				   << "\r\n";
			}

			socket->write(data);

			socket->close();

			qDebug() << "Wrote to client";

			if (socket->state() == QTcpSocket::UnconnectedState) {
				delete socket;
				qDebug() << "Connection closed";
			}
		}
	}
}

void HttpServer::discardClient()
{
	QTcpSocket* socket = (QTcpSocket*)sender();
	socket->deleteLater();

	qDebug() << "Connection closed";
}

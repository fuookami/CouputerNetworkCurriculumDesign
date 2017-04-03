#include "Server.h"
#include <string>

Server::Server()
	: server(new QUdpSocket(nullptr))
{
	server->bind(QHostAddress::LocalHost, 10000);
	connect(server, SIGNAL(readyRead()), this, SLOT(processDatagrams()));
}

void Server::processDatagrams(void)
{
	qDebug() << "Hi";
}
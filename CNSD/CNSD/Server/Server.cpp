#include "Server.h"

Server::Server()
	: tcpServer(new QTcpServer(nullptr)), tcpSocket(nullptr)
{
	tcpServer->listen(QHostAddress::LocalHost, 10000);

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(getConnection()));
}

void Server::getConnection()
{
	tcpSocket = tcpServer->nextPendingConnection();
}

#include "Server.h"

Server::Server()
	: tcpServer(new QTcpServer(nullptr))
{
	tcpServer->listen(QHostAddress::LocalHost, 19999);

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(getConnection()));
}

void Server::getConnection()
{
	qDebug() << QString::fromLocal8Bit("一个连接");
	unsigned int thisID(ServerThread::getThreadCounter());
	tcpSocketThreads.insert(std::make_pair(thisID, 
		std::shared_ptr<ServerThread>(new ServerThread(tcpServer->nextPendingConnection()))));
	ServerThread *serverThread(tcpSocketThreads[thisID].get());
	connect(serverThread, SIGNAL(cilentDisconnected(unsigned short)),
		this, SLOT(cilentDisconnectedSlot(unsigned short)));
	serverThread->start();
}

void Server::cilentDisconnectedSlot(const unsigned short id)
{
	qDebug() << QString::fromLocal8Bit("一个连接断开");
	tcpSocketThreads[id].reset();
	tcpSocketThreads.erase(id);
}
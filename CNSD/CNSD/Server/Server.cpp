#include "Server.h"

Server::Server()
	: tcpServer(new QTcpServer(this))
{
	tcpServer->listen(QHostAddress::LocalHost, 10000);

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(getConnection()));
}

void Server::getConnection()
{
	qDebug() << "一个连接";
	unsigned int thisID(ServerThread::counter);
	tcpSocketThreads.insert(std::make_pair(thisID, 
		std::shared_ptr<ServerThread>(new ServerThread(tcpServer->nextPendingConnection()))));
	ServerThread *serverThread(tcpSocketThreads[thisID].get());
	
	connect(serverThread, SIGNAL(cilentDisconnected(unsigned short id)), 
		this, SLOT(cilentDisconnectedSlot(unsigned short id)));
}

void Server::cilentDisconnectedSlot(const unsigned int id)
{
	tcpSocketThreads[id].reset();
	tcpSocketThreads.erase(id);
}
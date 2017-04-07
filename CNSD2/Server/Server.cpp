#include "Server.h"

Server::Server(const QHostAddress host, quint16 port)
	: tcpServer(new QTcpServer(nullptr))
{
	

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(getConnection()));
}

void Server::getConnection()
{
	unsigned int thisID(ServerThread::getThreadCounter());
	tcpSocketThreads.insert(std::make_pair(thisID, 
		std::shared_ptr<ServerThread>(new ServerThread(tcpServer->nextPendingConnection(), thisID))));
	ServerThread *serverThread(tcpSocketThreads[thisID].get());
	connect(serverThread, SIGNAL(cilentDisconnected(unsigned short)),
		this, SLOT(cilentDisconnectedSlot(unsigned short)));
	connect(serverThread, SIGNAL(pushMsgSignal(const QString &)),
		this, SLOT(pushMsgSlot(const QString &)));
	serverThread->start();
}

void Server::pushMsgSlot(const QString & msg)
{
	emit pushMsg(msg);
}

void Server::cilentDisconnectedSlot(const unsigned short id)
{
	tcpSocketThreads[id].reset();
	tcpSocketThreads.erase(id);
}
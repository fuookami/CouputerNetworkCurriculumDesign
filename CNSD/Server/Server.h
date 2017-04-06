#pragma once

#include "ServerThread.h"
#include <memory>
#include <QtNetwork/QTcpServer>

class Server : public QObject
{
	Q_OBJECT

public:
	Server();

private slots:
	void getConnection();
	void cilentDisconnectedSlot(const unsigned short id);

private:
	QTcpServer *tcpServer;
	std::map<unsigned int, std::shared_ptr<ServerThread>> tcpSocketThreads;
};
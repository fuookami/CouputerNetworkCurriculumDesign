#pragma once

#include "SocketHandleThread.h"
#include <memory>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>

class Server : public QObject
{
	Q_OBJECT;

public:
	Server(const QHostAddress host, quint16 port);

signals:
	void pushMsg(const QString &msg);

private slots:
	void getConnection();
	void pushMsgSlot(const QString &msg);
	void cilentDisconnectedSlot(const unsigned short id);

private:
	QTcpServer *tcpServer;
	std::map<unsigned int, std::shared_ptr<ServerThread>> tcpSocketThreads;
};
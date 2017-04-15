#pragma once

#include "SocketHandleThread.h"
#include <memory>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>

class Server : public QObject
{
	Q_OBJECT;

public:
	Server();

	Public::RetCode listen(const QHostAddress host, quint16 port);
	Public::RetCode close();

signals:
	void pushMsg(const QString msg);
	void closed(void);

private slots:
	void getMsg(const QString msg, unsigned int id);
	void getData(const std::string data, unsigned int id);
	void getConnection();
	void cilentDisconnected(const unsigned short id);

	void socketHandleThreadStopped(unsigned int id);

private:
	std::string dispose(const std::string data);

private:
	QTcpServer *tcpServer;
	std::map<unsigned int, std::shared_ptr<SocketHandleThread>> tcpSocketThreads;
};
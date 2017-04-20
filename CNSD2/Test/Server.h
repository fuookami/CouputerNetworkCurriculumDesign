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
	void getData(const Public::DataType data, unsigned int id);
	void getConnection();
	void cilentDisconnected(const unsigned int id);

	void socketHandleThreadStopped(unsigned int id);

private:
	std::string dispose(const Public::DataType data);

private:
	QTcpServer *tcpServer;
	std::map<unsigned int, std::shared_ptr<SocketHandleThread>> tcpSocketThreads;
};
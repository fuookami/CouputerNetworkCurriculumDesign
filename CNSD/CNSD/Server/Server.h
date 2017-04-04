#pragma once

#include <QtNetwork/QTcpServer>

class Server : public QObject
{
	Q_OBJECT;

public:
	Server();

private slots:
	void getConnection();

private:
	QTcpServer *tcpServer;
};
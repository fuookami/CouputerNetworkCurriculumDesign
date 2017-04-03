#pragma once

#include <QtNetwork/QUdpSocket>

class Server : public QObject
{
	Q_OBJECT;

public:
	Server();

private slots:
	void processDatagrams(void);

public:
	QUdpSocket *server;
};
#pragma once

#include <QtNetwork/QTcpSocket>

class Cilent : public QObject
{
	Q_OBJECT;

public:
	Cilent();

	void connectToHost(const QHostAddress &host, quint16 port);
	void disconnect();

private:
	QTcpSocket *tcpSocket;
};
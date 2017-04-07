#pragma once

#include "Public.h"
#include <QtCore/QDataStream>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class ServerThread : public QThread
{
	Q_OBJECT;

public:
	ServerThread(QTcpSocket *_tcpSocket);
	static unsigned short getThreadCounter(void);
	void start(void);

protected:
	void run();

private:
	static unsigned short ThreadCounter;
	unsigned short id;

	QTcpSocket *tcpSocket;

	volatile bool stopped;
};
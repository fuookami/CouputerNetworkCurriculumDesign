#include "ServerThread.h"

ServerThread::ServerThread(QTcpSocket * _tcpSocket)
	: tcpSocket(_tcpSocket), stopped(false)
{
	run();
}

void ServerThread::run()
{
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(cilentDisconnected()));
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

	while (!stopped)
	{
		tcpSocket->waitForReadyRead(100);
	}
	stopped = false;
}

void ServerThread::cilentDisconnected()
{
	stopped = true;
}

void ServerThread::dataReceived()
{

}

#include "ServerThread.h"

unsigned short ServerThread::ThreadCounter = 0;

ServerThread::ServerThread(QTcpSocket * _tcpSocket)
	: tcpSocket(_tcpSocket), stopped(true), id(ThreadCounter)
{
	++ThreadCounter;
}

unsigned short ServerThread::getThreadCounter(void)
{
	return ThreadCounter;
}

void ServerThread::start(void)
{
	stopped = false;
	run();
}

void ServerThread::run()
{
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(cilentDisconnectedSlot()));

	while (!stopped)
	{
		tcpSocket->waitForReadyRead(500);
	}
	stopped = false;
}
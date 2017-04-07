#include "AbstractSocketHandleThread.h"

void SocketHandleThread::run()
{
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

	while (!stopped)
	{
		if (threadState == Public::Idle || threadState == Public::Receieving)
		{
			tcpSocket->waitForReadyRead(Public::MSOfTimePart);
		}
	}
}

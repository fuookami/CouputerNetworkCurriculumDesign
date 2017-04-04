#include "ServerThread.h"

ServerThread::ServerThread(QTcpSocket * _tcpSocket)
	: tcpSocket(_tcpSocket), stopped(false), id(counter)
{
	++counter;
	isSending = false;
	run();
}

void ServerThread::run()
{
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(cilentDisconnectedSlot()));
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

	while (!stopped)
	{
		tcpSocket->waitForReadyRead(100);
	}
	stopped = false;
}

void ServerThread::cilentDisconnectedSlot()
{
	stopped = true;
	emit cilentDisconnected(id);
}

void ServerThread::dataReceived()
{
	if (!isSending)
		presendDispose();
	else
		sendingDispose();
}

void ServerThread::presendDispose()
{
	Public::State currFrameState(Public::getRandomFrameState());

	switch (currFrameState)
	{
	case Public::FrameState::NoError:

	case Public::FrameState::Wrong:

	case Public::FrameState::NoReply:

	default:
		break;
	}
}

void ServerThread::sendingDispose()
{
	Public::State currFrameState(Public::getRandomFrameState());

	switch (currFrameState)
	{
	case Public::FrameState::NoError:
	case Public::FrameState::Lose:
	case Public::FrameState::Wrong:
	case Public::FrameState::NoReply:
	default:
		break;
	}
}

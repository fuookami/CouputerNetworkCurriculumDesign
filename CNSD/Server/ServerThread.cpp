#include "ServerThread.h"

unsigned short ServerThread::ThreadCounter = 0;

ServerThread::ServerThread(QTcpSocket * _tcpSocket)
	: tcpSocket(_tcpSocket), stopped(false), id(ThreadCounter)
{
	++ThreadCounter;
	isSending = false;
	stopped = true;
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
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

	while (!stopped)
	{
		tcpSocket->waitForReadyRead(500);
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
	qDebug() << QString::fromLocal8Bit("收到客户端的信息了");
	/*
	if (!isSending)
		presendDispose();
	else
		sendingDispose();
	*/
}

void ServerThread::presendDispose()
{
	Public::State currFrameState(Public::getRandomFrameState());

	switch (currFrameState)
	{
	case Public::FrameState::FrameNoError:

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
	case Public::FrameState::FrameNoError:
	case Public::FrameState::Lose:
	case Public::FrameState::Wrong:
	case Public::FrameState::NoReply:
	default:
		break;
	}
}

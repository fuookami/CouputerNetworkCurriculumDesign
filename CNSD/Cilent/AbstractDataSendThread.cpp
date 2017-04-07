#include "AbstractDataSendThread.h"

PKTTimer::PKTTimer(unsigned char _id)
	: id(_id), timer(new QTimer(nullptr))
{
	connect(timer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
}

void PKTTimer::startTimer(const unsigned short ms)
{
	timer->start(ms);
}

void PKTTimer::stopTimer()
{
	timer->stop();
}

void PKTTimer::timeoutSlot(void)
{
	emit timeoutSignal(id);
}

AbstractDataSendThread::AbstractDataSendThread(QTcpSocket *_tcpSocket)
	: tcpSocket(_tcpSocket), stopped(true)
{
	for (unsigned char i(0); i != Public::RouletteSize; ++i)
	{
		timers[i] = new PKTTimer(i);
		connect(timers[i], SIGNAL(timeoutSignal(unsigned char)), this, SLOT(packbackTimeout(unsigned char)));
	}
}

void AbstractDataSendThread::pushReply(Public::RequestType requestType, unsigned int arg /* = 0 */)
{
	std::string data(Public::ui2str(arg));
	reply.push_back(Public::DataFrame(0, requestType, data.begin(), data.end()));
	activeThread();
}

void AbstractDataSendThread::disposeRequest(Public::RequestType requestType, unsigned int arg /* = 0 */)
{
	if (!isSendingDataPackbage)
	{
		if (requestType == Public::RequestTypes::ACK && arg == -1)
		{
			reply.pop_front();
			isSendingDataPackbage = true;
			acceptFrameId = 0;
		}
	}
	else if (isSendingDataPackbage)
	{
		if (requestType == Public::RequestTypes::ACK)
		{
			for (; acceptFrameId != arg;)
			{
				datas.front()[acceptFrameId].pop_front();
				acceptFrameId = (acceptFrameId + 1) % Public::RouletteSize;
			}
			datas.front()[acceptFrameId].pop_front();
			acceptFrameId = (acceptFrameId + 1) % Public::RouletteSize;
		}
	}
}

void AbstractDataSendThread::run()
{
	while (!stopped)
	{
		tcpSocket->waitForBytesWritten(Public::getRandomWaitWriteTime());
		sendData();
	}
}

void AbstractDataSendThread::activeThread(void)
{
	if (stopped)
	{
		stopped = false;
		run();
	}
}

void AbstractDataSendThread::sendData(void)
{	
	if (!reply.empty())
	{
		tcpSocket->write(reply.front().toQByteArray());
	}
	else if (isSendingDataPackbage) 
	{
		for (unsigned int i(acceptFrameId), j(0); j != Public::WindowSize; ++j)
		{
			unsigned int k((i + j) % Public::RouletteSize);
			if (sendedButNotAcceptFrameId.find(k) != sendedButNotAcceptFrameId.end())
			{
				tcpSocket->write(datas.front()[k].front().toQByteArray());
				sendedButNotAcceptFrameId.insert(k);
				timers[k]->startTimer(Public::MSOfTimePart);
				break;
			}
		}
	}
}

void AbstractDataSendThread::packbackTimeout(unsigned char id)
{
	tcpSocket->write(datas.front()[id].front().toQByteArray());
	timers[id]->startTimer(Public::MSOfTimePart);
}
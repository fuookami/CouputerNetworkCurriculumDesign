#include "AbstractDataReceiveThread.h"

AbstractDataReceiveThread::AbstractDataReceiveThread(QTcpSocket *_tcpSocket)
	: tcpSocket(_tcpSocket), stopped(true), isSendingDataPackbage(false), currFrameNum(0), totalFrameNum(0), waitingFrameId(0)
{

}

void AbstractDataReceiveThread::startListenDataReceived()
{
	stopped = false;
	run();
}

void AbstractDataReceiveThread::dataReceived()
{
	if (isSendingDataPackbage)
		presendDispose();
	else
		sendingDispose();
}

void AbstractDataReceiveThread::run()
{
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

	while (!stopped)
	{
		tcpSocket->waitForReadyRead(Public::MSOfTimePart);
	}
	stopped = false;
}
// 先补齐这里的消息提醒
void AbstractDataReceiveThread::presendDispose()
{
	Public::State currFrameState(Public::getRandomFrameState());
	QDataStream in(tcpSocket);
	Public::DataFrame currFrame(in);

	if (currFrameState == Public::FrameState::Lose)
		return;
	if (currFrameState == Public::FrameState::Wrong
		|| !currFrame.isCorrect())
		return;

	Public::decode(currFrame.data);

	if (currFrame.request == Public::RequestTypes::SYN)
	{
		isSendingDataPackbage = true;
		totalFrameNum = Public::str2ui(currFrame.data);
		if (currFrameState == Public::FrameState::NoReply)
			return;
		emit pushReply(Public::RequestTypes::ACK, -1); // 可以开始发送数据
	}
	else if (currFrame.request == Public::RequestTypes::ACK)
	{
		emit pushRequest(Public::RequestTypes::ACK, Public::str2ui(currFrame.data));
	}
	else if (currFrame.request == Public::RequestTypes::PKT)
	{
		if (currFrameState == Public::FrameState::NoReply)
			return;
		emit pushReply(Public::RequestTypes::ACK, -1); // 数据已经接受完毕
	}
}

void AbstractDataReceiveThread::sendingDispose()
{
	Public::State currFrameState(Public::getRandomFrameState());
	QDataStream in(tcpSocket);
	Public::DataFrame currFrame(in);

	if (currFrameState == Public::FrameState::Lose)
		return;
	if (currFrameState == Public::FrameState::Wrong
		|| !currFrame.isCorrect())
		return;
	Public::decode(currFrame.data);

	if (currFrame.request == Public::RequestTypes::PKT)
	{
		unsigned int currFrameId(currFrame.id);
		recievingData[currFrameId].push_back(std::move(currFrame));

		if (currFrameId == waitingFrameId)
		{
			++currFrameNum;
			waitingFrameId = ++waitingFrameId % Public::RouletteSize;
			while (!buffFrameId.empty() && buffFrameId.find(waitingFrameId) != buffFrameId.cend())
			{
				++currFrameNum;
				buffFrameId.erase(waitingFrameId);
				waitingFrameId = ++waitingFrameId % Public::RouletteSize;
			}
			if (currFrameState == Public::FrameState::FrameNoError)
				emit pushReply(Public::RequestTypes::ACK, waitingFrameId == 0 ? Public::RouletteSize - 1 : waitingFrameId - 1);
		}
		else
		{
			buffFrameId.insert(currFrameId);
			if (currFrameState == Public::FrameState::FrameNoError)
				emit pushReply(Public::RequestTypes::ACK, waitingFrameId);
		}

		if (currFrameNum == totalFrameNum)
		{
			std::pair<Public::RequestType, std::string> data(Public::readDataRoulette<std::string>(recievingData));
			emit pushData(std::move(data.second));
			if (currFrameState == Public::FrameState::FrameNoError)
				emit pushReply(Public::RequestTypes::ACK, -1);
			waitingFrameId = totalFrameNum = currFrameNum = 0;
			isSendingDataPackbage = false;
		}
	}
}

void AbstractDataReceiveThread::pushMsg(const QString msg)
{
	qDebug() << msg;
	//emit pushMsgSignal(std::move(msg));
}

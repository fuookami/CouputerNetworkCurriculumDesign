#include "AbstractDataReceiveThread.h"

AbstractDataReceiveThread::AbstractDataReceiveThread(QTcpSocket *_tcpSocket)
	: tcpSocket(_tcpSocket), stopped(true), isSendingDataPackbage(false), 
	currFrameNum(0), totalFrameNum(0), waitingFrameId(0)
{

}

void AbstractDataReceiveThread::startListenDataReceived()
{
	stopped = false;
	run();
}

void AbstractDataReceiveThread::dataReceived()
{
	pushMsg(QString::fromLocal8Bit("收到来自客户端的数据包，准备进行解析\n"));
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

void AbstractDataReceiveThread::presendDispose()
{
	Public::State currFrameState(Public::getRandomFrameState());
	QDataStream in(tcpSocket);
	Public::DataFrame currFrame(in);

	std::ostringstream sout;
	sout << "当前随机得到的帧状态为" << Public::getFrameStateString(currFrameState) << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	if (currFrameState == Public::FrameState::Lose)
		return;
	if (currFrameState == Public::FrameState::Wrong
		|| !currFrame.isCorrect())
	{
		pushMsg(QString::fromLocal8Bit("帧校验出错，丢弃该帧\n"));
		return;
	}
	
	sout.clear();
	sout << "帧校验正确，数据为：" << currFrame.data << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	pushMsg(QString::fromLocal8Bit("准备进行解密\n"));
	Public::decode(currFrame.data);
	pushMsg(QString::fromLocal8Bit("解密完成\n"));
	sout.clear();
	sout << "解密后数据为：" << currFrame.data << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	if (currFrame.request == Public::RequestTypes::SYN)
	{
		waitingFrameId = currFrameNum = 0;
		isSendingDataPackbage = true;
		totalFrameNum = Public::str2ui(currFrame.data);
		
		std::ostringstream sout;
		sout << "接收到来自客户端的数据发送请求，共" << totalFrameNum << "个包，转入接收数据包状态" << std::endl;
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

		if (currFrameState == Public::FrameState::NoReply)
			return;

		pushMsg(QString::fromLocal8Bit("向客户端发送ACK信号，示意可发送数据包\n"));
		emit pushReply(Public::RequestTypes::ACK, -1);
	}
	else if (currFrame.request == Public::RequestTypes::ACK)
	{
		emit pushRequest(Public::RequestTypes::ACK, Public::str2ui(currFrame.data));
	}
	else if (currFrame.request == Public::RequestTypes::PKT)
	{
		std::ostringstream sout;
		sout << "接收到来自客户端的包，帧编号为" << currFrame.id << "，由于无等待接收数据包，将抛弃该帧" << std::endl;
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

		if (currFrameState == Public::FrameState::NoReply)
			return;

		pushMsg(QString::fromLocal8Bit("向客户端发送ACK(-1)信号，示意所有数据已接收完毕\n"));
		emit pushReply(Public::RequestTypes::ACK, -1);
	}
}

void AbstractDataReceiveThread::sendingDispose()
{
	Public::State currFrameState(Public::getRandomFrameState());
	QDataStream in(tcpSocket);
	Public::DataFrame currFrame(in);

	std::ostringstream sout;
	sout << "当前随机得到的帧状态为" << Public::getFrameStateString(currFrameState) << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	if (currFrameState == Public::FrameState::Lose)
		return;
	if (currFrameState == Public::FrameState::Wrong
		|| !currFrame.isCorrect())
	{
		pushMsg(QString::fromLocal8Bit("帧校验出错，丢弃该帧\n"));
		return;
	}
	sout.clear();
	sout << "帧校验正确，数据为：" << currFrame.data << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	pushMsg(QString::fromLocal8Bit("准备进行解密\n"));
	Public::decode(currFrame.data);
	pushMsg(QString::fromLocal8Bit("解密完成\n"));
	sout.clear();
	sout << "解密后数据为：" << currFrame.data << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	if (currFrame.request == Public::RequestTypes::PKT)
	{
		unsigned int currFrameId(currFrame.id);
		recievingData[currFrameId].push_back(std::move(currFrame));
		std::ostringstream sout;
		sout << "已收到数据包帧编号为" << currFrameId << std::endl;
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

		if (currFrameId == waitingFrameId)
		{
			++currFrameNum;
			waitingFrameId = ++waitingFrameId % Public::RouletteSize;
			sout.clear();
			while (!buffFrameId.empty() && buffFrameId.find(waitingFrameId) != buffFrameId.cend())
			{
				sout << "编号为" << waitingFrameId << "的帧已在帧轮盘中" << std::endl;
				++currFrameNum;
				buffFrameId.erase(waitingFrameId);
				waitingFrameId = ++waitingFrameId % Public::RouletteSize;
			}

			if (currFrameState == Public::FrameState::FrameNoError)
			{
				sout << "将向客户端发送ACK(" << (waitingFrameId - 1) << ")信号" << std::endl;
				emit pushReply(Public::RequestTypes::ACK, waitingFrameId == 0 ? Public::RouletteSize - 1 : waitingFrameId - 1);
			}

			pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		}
		else
		{
			buffFrameId.insert(currFrameId);
			sout.clear();
			sout << "希望收到的帧编号为" << waitingFrameId << "，将该帧装入帧轮盘中" << std::endl;
			if (currFrameState == Public::FrameState::FrameNoError)
			{
				sout << "将向客户端发送ACK(" << waitingFrameId << ")信号" << std::endl;
				emit pushReply(Public::RequestTypes::ACK, waitingFrameId);
			}
			pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		}

		if (currFrameNum == totalFrameNum)
		{
			std::pair<Public::RequestType, std::string> data(Public::readDataRoulette<std::string>(recievingData));
			pushMsg(QString::fromLocal8Bit("已收到所有数据包，向服务器推送数据，转入空闲状态\n"));
			emit pushData(std::move(data.second));
			isSendingDataPackbage = false;

			if (currFrameState == Public::FrameState::NoReply)
				return;

			pushMsg(QString::fromLocal8Bit("向客户端发送ACK(-1)信号，示意所有数据已接收完毕\n"));
			emit pushReply(Public::RequestTypes::ACK, -1);
		}
	}
	else 
	{
		std::ostringstream sout;
		sout << "此时不应收到" << Public::getFrameStateString(currFrame.request) << "信号，丢弃该帧" << std::endl;
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	}
}

void AbstractDataReceiveThread::pushMsg(const QString msg)
{
	qDebug() << msg;
	//emit pushMsgSignal(std::move(msg));
}

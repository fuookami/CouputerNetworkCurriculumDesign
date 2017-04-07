#include "AbstractSocketHandleThread.h"

void SocketHandleThread::run()
{
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

	while (!stopped)
	{
		if (threadState == Public::ThreadState::Idle)
		{
			if (sendingInfo.sendingData.empty())
				tcpSocket->waitForReadyRead(Public::MSOfTimePart);
			else
			{
				// 转至准备发送态并发送SYN信号
				threadState = Public::ThreadState::WaitForSending;
				sendFrame(Public::RequestTypes::SYN, Public::countFrames(sendingInfo.sendingData.front()));
			}
		}
		else if (threadState == Public::ThreadState::Receieving
			|| threadState == Public::ThreadState::WaitForSending)
		{
			tcpSocket->waitForReadyRead(Public::MSOfTimePart);
		}
		else if (threadState == Public::ThreadState::Sending)
		{
			// 查看是否有可发送的数据包
			// 如果有，发送
			// 没有则等待ACK回应
			if (checkDataDeque())
				sendFrames();
			else	
				tcpSocket->waitForReadyRead(Public::MSOfTimePart);
		}
	}
}

void SocketHandleThread::dataReceived()
{
	Public::State currFrameState(Public::getRandomFrameState());
	QDataStream in(tcpSocket);
	Public::DataFrame currFrame(in);

	std::ostringstream sout;
	sout << "当前随机得到的帧状态为" << Public::getFrameStateString(currFrameState) << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	if (currFrameState == Public::FrameState::Lose)
		return;
	if (currFrameState == Public::FrameState::Wrong
		|| !currFrame.isCorrect())
	{
		emit pushMsg(QString::fromLocal8Bit("帧校验出错，丢弃该帧\n"));
		return;
	}

	sout.clear();
	sout << "帧校验正确，数据为：" << currFrame.data << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	emit pushMsg(QString::fromLocal8Bit("准备进行解密\n"));
	Public::decode(currFrame.data);
	emit pushMsg(QString::fromLocal8Bit("解密完成\n"));
	sout.clear();
	sout << "解密后数据为：" << currFrame.data << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	switch (threadState)
	{
	case Public::ThreadState::Idle:
		dataReceivedForIdea(currFrame, currFrameState);
		break;
	case Public::ThreadState::Receieving:
		dataReceivedForReceiving(currFrame, currFrameState);
		break;
	case Public::ThreadState::Sending:
		dataReceivedForSending(currFrame, currFrameState);
		break;
	case Public::ThreadState::WaitForSending:
		dataReceivedForWaitSending(currFrame, currFrameState);
		break;
	default:
		break;
	}
}

void SocketHandleThread::dataReceivedForIdea(const Public::DataFrame &currFrame, Public::State frameState)
{
	// 如果收到SYN转移到收状态，并返回ACK(-1)
}

void SocketHandleThread::dataReceivedForReceiving(const Public::DataFrame &currFrame, Public::State frameState)
{
	// 帧轮盘接收数据，并返回对应的ACK
	// 如果数据接收完毕，向上级发送数据并转移到空闲状态，并返回ACK(-1)示意对方发送完毕
}

void SocketHandleThread::dataReceivedForWaitSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	// 如果收到ACK(-1)转移到发送状态
}

void SocketHandleThread::dataReceivedForSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	// 如果发送完毕或收到ACK(-1)，则转移到空闲状态
	// 否则，移动发送窗口
}

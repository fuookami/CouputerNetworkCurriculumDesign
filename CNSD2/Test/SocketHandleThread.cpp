#include "SocketHandleThread.h"

unsigned int SocketHandleThread::threadCounter = 0;

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

SocketHandleThread::SocketHandleThread(QTcpSocket * _tcpSocket, unsigned int _id, bool _isServer)
	: tcpSocket(_tcpSocket), id(_id), isServer(_isServer), threadState(Public::ThreadState::Close)
{
	for (unsigned int i(0), j(sendingInfo.timers.size()); i != j; ++i)
	{
		sendingInfo.timers[i] = new PKTTimer(i);
		connect(sendingInfo.timers[i], SIGNAL(timeoutSignal(unsigned char)), this, SLOT(PKTTimeOut(unsigned char)));
	}
	timePartTimer = new QTimer();
	stopped = true;
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnectedSlot()));
}

unsigned int SocketHandleThread::getThreadCounter()
{
	return threadCounter;
}

void SocketHandleThread::start()
{
	stopped = false;
	threadState = Public::ThreadState::Idle;
	run();
}

void SocketHandleThread::stop()
{
	if (sendingInfo.sendingData.size() == 1
		&& (Public::ThreadState::Sending || Public::ThreadState::WaitForSending))
	{
		stopped = true;
		emit pushMsg(QString::fromLocal8Bit("数据队列中只有一个数据包正在发送，待该数据包发送完毕后，管程进入准备关闭状态\n"), id);
	}
	else if (!sendingInfo.sendingData.empty())
	{
		emit pushMsg(QString::fromLocal8Bit("数据队列中有数据包待发送，管程进入准备关闭状态失败\n"), id);
	}
	else
	{
		stopped = true;
		switch (threadState)
		{
		case Public::ThreadState::Receieving:
			emit pushMsg(QString::fromLocal8Bit("数据队列为空，待该数据包接收完毕后，管程进入准备关闭状态\n"), id);
			break;
		case Public::ThreadState::Idle:
			emit pushMsg(QString::fromLocal8Bit("数据队列为空，且目前为空闲状态，管程进入准备关闭状态\n"), id);
			break;
		default:
			break;
		}
	}
}

void SocketHandleThread::run()
{
	if (threadState != Public::ThreadState::Close)
	{
		emit pushMsg(QString::fromLocal8Bit("管程进入运行状态，将等待数据包的收发请求。\n"), id);
		connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
		connect(timePartTimer, SIGNAL(timeout()), this, SLOT(mainLoop()));
		timePartTimer->start(Public::MSOfTimePart);

		exec();
	}
}

void SocketHandleThread::dataReceived()
{
	emit pushMsg(QString::fromLocal8Bit("收到数据帧，准备进行解析\n"), id);

	//Public::State currFrameState(Public::getRandomFrameState());
	Public::State currFrameState(Public::FrameState::FrameNoError);
	QDataStream in(tcpSocket);
	in.setVersion(QDataStream::Qt_5_8);
	Public::DataFrame currFrame(in);

	std::ostringstream sout;

	sout << "当前随机得到的帧状态为" << Public::getFrameStateString(currFrameState) << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);

	if (currFrameState == Public::FrameState::Lose)
		return;
	if (currFrameState == Public::FrameState::Wrong
		|| !currFrame.isCorrect())
	{
		emit pushMsg(QString::fromLocal8Bit("帧校验出错，丢弃该帧\n"), id);
		return;
	}

	sout.str("");
	sout << "帧校验正确，数据为：" << Public::data2uiHex(currFrame.data) << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
	emit pushMsg(QString::fromLocal8Bit("准备进行解密\n"), id);
	Public::decode(currFrame.data);
	emit pushMsg(QString::fromLocal8Bit("解密完成\n"), id);
	sout.str("");
	sout << "解密后数据为：" << Public::data2uiHex(currFrame.data) << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);

	switch (threadState)
	{
	case Public::ThreadState::Idle:
		dataReceivedForIdle(currFrame, currFrameState);
		break;
	case Public::ThreadState::Receieving:
		dataReceivedForReceiving(std::move(currFrame), currFrameState);
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

void SocketHandleThread::PKTTimeOut(unsigned char id)
{
	if (id == sendingInfo.lastAcceptId + 1)
	{
		std::ostringstream sout;
		sout << "编号为" << (unsigned int)id << "的数据帧等待回复超时，将重新发送" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
		sendFrame(sendingInfo.sendingData.front()[id].front());
	}
}

void SocketHandleThread::socketDisconnectedSlot()
{
	stopped = true;
	emit pushMsg(QString::fromLocal8Bit("客户端断开连接， 进入关闭状态\n"), id);
	emit socketDisconnected(id);
}

void SocketHandleThread::mainLoop(void)
{
	if (threadState == Public::ThreadState::Idle && !stopped)
	{
		if (!sendingInfo.sendingData.empty())
		{
			// 转至准备发送态并发送SYN信号
			emit pushMsg(QString::fromLocal8Bit("管程目前为空闲状态，并有数据包待发送，将向目标发起发送数据包请求，并转入等待发送状态\n"), id);
			threadState = Public::ThreadState::WaitForSending;
			timePartTimer->stop();
			timePartTimer->start(Public::MSOfTimePart * Public::timeOfRetryTimePart);
			sendFrame(Public::RequestTypes::SYN, Public::countFrames(sendingInfo.sendingData.front()));
		}
		else 
		{
			timePartTimer->start(Public::MSOfTimePart);
		}
	}
	else if (threadState == Public::ThreadState::WaitForSending)
	{
		emit pushMsg(QString::fromLocal8Bit("数据发送请求回复超时，将重新发送SYN信号"), id);
		timePartTimer->start(Public::MSOfTimePart);
		sendFrame(Public::RequestTypes::SYN, Public::countFrames(sendingInfo.sendingData.front()));
	}
	else if (threadState == Public::ThreadState::Idle && stopped)
	{
		threadState = Public::ThreadState::Close;
		disconnect(tcpSocket, SIGNAL(readyRead()));
		disconnect(timePartTimer, SIGNAL(timeout()));
		emit pushMsg(QString::fromLocal8Bit("进入关闭状态"), id);
		emit stoped(id);
	}
}

void SocketHandleThread::sendFrames(void)
{
	for (; calIdDistance(sendingInfo.lastSendedId, sendingInfo.lastAcceptId) < Public::WindowSize;)
	{
		sendingInfo.lastSendedId = ++sendingInfo.lastSendedId % Public::RouletteSize;
		if (!sendingInfo.sendingData.front()[sendingInfo.lastSendedId].empty())
		{
			// 发送窗口内有未发送的数据帧
			// 发送数据帧，并启动帧对应的计时器
			std::ostringstream sout;
			sout << "发现编号为" << sendingInfo.lastSendedId << "的数据帧在窗口内且未被发送，将发送" << std::endl;
			emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
			sendFrame(sendingInfo.sendingData.front()[sendingInfo.lastSendedId].front());
			sendingInfo.timers[sendingInfo.lastSendedId]->startTimer(Public::MSOfTimePart * 2);
		}
	}
}

void SocketHandleThread::sendFrame(Public::RequestType requestType, unsigned int arg)
{
	Public::DataType data(Public::ui2data(arg));
	Public::encode(data);
	sendFrame(Public::DataFrame(0, requestType, data.begin(), data.end()));
}

void SocketHandleThread::sendFrame(const Public::DataFrame & frame)
{
	QByteArray block;
	frame.getQByteArray(block);
	tcpSocket->write(block);
}

unsigned int SocketHandleThread::calIdDistance(const unsigned int lastSendedId, const unsigned int lastAcceptId)
{
	if (lastSendedId < lastAcceptId && lastAcceptId != -1)
		return lastSendedId + Public::RouletteSize - lastAcceptId;
	else
		return lastSendedId - lastAcceptId;
}

void SocketHandleThread::dataReceivedForIdle(const Public::DataFrame &currFrame, Public::State frameState)
{
	if (currFrame.request == Public::RequestTypes::SYN)
	{
		// 初始化数据接收槽并转入接收状态
		threadState = Public::ThreadState::Receieving;

		recievingInfo.waitingFrameId = recievingInfo.currFrameNum = 0;
		recievingInfo.buffFrameId.clear();
		for (std::deque<Public::DataFrame> &currDeque : recievingInfo.recievingData)
			currDeque.clear();
		recievingInfo.totalFrameNum = Public::data2ui(currFrame.data);

		std::ostringstream sout;
		sout << "接收到数据发送请求，共" << recievingInfo.totalFrameNum << "个包，转入接收数据包状态" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);

		// 返回ACK(-1)
		if (frameState == Public::FrameState::NoReply)
			return;

		emit pushMsg(QString::fromLocal8Bit("向对方发送ACK信号，示意可发送数据包\n"), id);
		sendFrame(Public::RequestTypes::ACK, (unsigned char)-1);
	}
	else if (currFrame.request == Public::RequestTypes::PKT)
	{
		// 错误帧，丢弃该帧，并返回ACK(-1)告知对方，所有帧已接收完毕
		std::ostringstream sout;
		sout << "接收到数据包，帧编号为" << currFrame.id << "，由于无等待接收数据包，将抛弃该帧" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);

		if (frameState == Public::FrameState::NoReply)
			return;

		emit pushMsg(QString::fromLocal8Bit("向对方发送ACK(-1)信号，示意所有数据已接收完毕\n"), id);
		sendFrame(Public::RequestTypes::ACK, (unsigned char)-1);
	}
	else if (currFrame.request == Public::RequestTypes::ACK)
	{
		// 错误帧，丢弃该帧
		std::ostringstream sout;
		sout << "接收到ACK信号，由于处于空闲状态，不应受到ACK信号，将抛弃该帧" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
	}
}

void SocketHandleThread::dataReceivedForReceiving(Public::DataFrame currFrame, Public::State frameState)
{
	if (currFrame.request == Public::RequestTypes::PKT)
	{
		// 帧轮盘接收数据，并返回对应的ACK
		// 如果收到帧编号为希望收到的帧编号，则更新希望收到的帧编号，并发送ACK信号，参数为新的希望收到的帧编号
		unsigned int currFrameId(currFrame.id);

		if (calIdDistance(currFrameId, recievingInfo.waitingFrameId) >= Public::WindowSize)
		{
			// 帧已在数据轮盘中
			// to do
			// 应当给对方回一句ACK
			emit pushMsg(QString::fromLocal8Bit("该帧已在数据轮盘中，丢弃该数据包\n"), id);
		}
		else
		{
			recievingInfo.recievingData[currFrameId].push_back(std::move(currFrame));
			std::ostringstream sout;
			sout << "已收到数据包帧编号为" << currFrameId << std::endl;
			emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);

			if (currFrameId == recievingInfo.waitingFrameId)
			{
				++recievingInfo.currFrameNum;
				recievingInfo.waitingFrameId = ++recievingInfo.waitingFrameId % Public::RouletteSize;
				sout.str("");
				while (!recievingInfo.buffFrameId.empty() && recievingInfo.buffFrameId.find(recievingInfo.waitingFrameId) != recievingInfo.buffFrameId.cend())
				{
					sout << "编号为" << recievingInfo.waitingFrameId << "的帧已在帧轮盘中" << std::endl;
					++recievingInfo.currFrameNum;
					recievingInfo.buffFrameId.erase(recievingInfo.waitingFrameId);
					recievingInfo.waitingFrameId = ++recievingInfo.waitingFrameId % Public::RouletteSize;
				}

				if (frameState == Public::FrameState::FrameNoError)
				{
					sout << "将向客户端发送ACK(" << (recievingInfo.waitingFrameId - 1) << ")信号" << std::endl;
					sendFrame(Public::RequestTypes::ACK, recievingInfo.waitingFrameId == 0 ? Public::RouletteSize - 1 : recievingInfo.waitingFrameId - 1);
				}

				emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
			}
			// 如果收到帧编号不是希望收到的帧编号，则装入帧轮盘中，并发送ACK信号，参数为希望收到的帧编号
			else
			{
				recievingInfo.buffFrameId.insert(currFrameId);
				sout.str("");
				sout << "希望收到的帧编号为" << recievingInfo.waitingFrameId << "，将该帧装入帧轮盘中" << std::endl;
				if (frameState == Public::FrameState::FrameNoError)
				{
					sout << "将向客户端发送ACK(" << recievingInfo.waitingFrameId << ")信号" << std::endl;
					sendFrame(Public::RequestTypes::ACK, recievingInfo.waitingFrameId);
				}
				emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
			}

			// 如果数据接收完毕，向上级发送数据并转移到空闲状态，并返回ACK(-1)示意对方发送完毕
			if (recievingInfo.currFrameNum == recievingInfo.totalFrameNum)
			{
				Public::DataType data(Public::readDataRoulette(recievingInfo.recievingData));
				emit pushMsg(QString::fromLocal8Bit("已收到所有数据包，向服务器推送数据，转入空闲状态\n"), id);
				emit pushData(std::move(data), id);
				threadState = Public::ThreadState::Idle;
				timePartTimer->start(Public::MSOfTimePart);

				if (frameState == Public::FrameState::NoReply)
					return;

				emit pushMsg(QString::fromLocal8Bit("向客户端发送ACK(-1)信号，示意所有数据已接收完毕\n"), id);
				sendFrame(Public::RequestTypes::ACK, (unsigned char)-1);

				timePartTimer->start(Public::MSOfTimePart);
			}
		}
	}
	else if (currFrame.request == Public::RequestTypes::SYN)
	{
		emit pushMsg(QString::fromLocal8Bit("接收到SYN信号，将重新发送ACK(-1)信号，示意对方可以发送\n"), id);
		sendFrame(Public::RequestTypes::ACK, (unsigned char)-1);
	}
	else
	{
		// 错误帧，丢弃该帧
		emit pushMsg(QString::fromLocal8Bit("接收到ACK信号，由于处于接收状态，不应收到ACK信号，将抛弃该帧\n"), id);
	}
}

void SocketHandleThread::dataReceivedForWaitSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	if (currFrame.request == Public::RequestTypes::ACK)
	{
		unsigned int data(Public::data2ui(currFrame.data));
		if (data == (unsigned char)-1)
		{
			// 初始化发送槽，并转入发送状态
			timePartTimer->stop();
			emit pushMsg(QString::fromLocal8Bit("已收到ACK(-1)信号，转入发送数据包状态\n"), id);

			sendingInfo.lastAcceptId = sendingInfo.lastSendedId = -1;
			threadState = Public::ThreadState::Sending;

			sendFrames();
		}
		else
		{
			// 错误帧，丢弃该帧
			std::ostringstream sout;
			sout << "接收到ACK(" << data << ")信号，由于处于等待发送状态，不应收到ACK(-1)以外的信号，将抛弃该帧" << std::endl;
			emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
		}
	}
	else if (currFrame.request == Public::RequestTypes::SYN)
	{
		// 如果收到SYN表示对方也转移到了待发送状态
		if (isServer)
		{
			emit pushMsg(QString::fromLocal8Bit("接收到SYN信号，由于处于等待发送状态，服务器优先发送，转入发送数据包状态"), id);
			// 如果本线程是服务器线程
			// 初始化发送槽，并转入发送状态
			timePartTimer->stop();
			sendingInfo.lastAcceptId = 0;
			sendingInfo.lastSendedId = -1;
			threadState = Public::ThreadState::Sending;
			sendFrames();
		}
		else
		{
			emit pushMsg(QString::fromLocal8Bit("接收到SYN信号，由于处于等待发送状态，服务器优先发送，客户端的发送请求阻塞执行，将转入接收数据包状态"), id);
			// 如果本线程是客户端线程
			// 则初始化接收槽，并转移到接收状态，并发送ACK(-1)示意对方可发送数据包
			timePartTimer->stop();
			recievingInfo.waitingFrameId = recievingInfo.currFrameNum = 0;
			recievingInfo.buffFrameId.clear();
			for (std::deque<Public::DataFrame> &currDeque : recievingInfo.recievingData)
				currDeque.clear();
			recievingInfo.totalFrameNum = Public::data2ui(currFrame.data);

			std::ostringstream sout;
			sout << "接收到数据发送请求，共" << recievingInfo.totalFrameNum << "个包，转入接收数据包状态" << std::endl;
			emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);
			threadState = Public::ThreadState::Receieving;

			// 返回ACK(-1)
			if (frameState == Public::FrameState::NoReply)
				return;

			emit pushMsg(QString::fromLocal8Bit("向对方发送ACK信号，示意可发送数据包\n"), id);
			sendFrame(Public::RequestTypes::ACK, (unsigned char)-1);
		}
	}
	else if (currFrame.request == Public::RequestTypes::PKT)
	{
		// 错误帧，丢弃该帧
		emit pushMsg(QString::fromLocal8Bit("接收到PKT信号，由于处于等待发送状态，不应收到PKT信号，将抛弃该帧\n"), id);
	}
}

void SocketHandleThread::dataReceivedForSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	if (currFrame.request == Public::RequestTypes::ACK)
	{
		unsigned int data(Public::data2ui(currFrame.data));
		if (data == -1)
		{
			// 如果收到ACK(-1)，则将当前数据轮盘出队并停止所有的数据轮盘计时器，然后转移到空闲状态
			threadState = Public::ThreadState::Idle;
			emit pushMsg(QString::fromLocal8Bit("所有帧已被确认收到，转入空闲状态。"), id);

			sendingInfo.sendingData.pop_front();
			timePartTimer->start(Public::MSOfTimePart);
			for (unsigned int i(0); i != Public::RouletteSize; ++i)
				sendingInfo.timers[i]->stopTimer();

			timePartTimer->start(Public::MSOfTimePart);
		}
		else
		{
			if (calIdDistance(data, sendingInfo.lastAcceptId) >= Public::WindowSize)
			{
				// 帧已被确认接收
				emit pushMsg(QString::fromLocal8Bit("帧已被确认收到，将丢弃该数据包\n"), id);
			}
			else
			{
				// 否则，停止刚Accept数据包对应的计时器
				// 移动发送窗口，并将数据轮盘队列相应的数据帧出队
				while (sendingInfo.lastAcceptId != data)
				{
					sendingInfo.lastAcceptId = ++sendingInfo.lastAcceptId % Public::RouletteSize;
					std::ostringstream sout;
					sout << "帧" << sendingInfo.lastAcceptId << "已被确认收到" << std::endl;
					emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()), id);

					sendingInfo.sendingData.front()[sendingInfo.lastAcceptId].pop_front();
					sendingInfo.timers[sendingInfo.lastAcceptId]->stopTimer();
				}
				// 如果已没有数据帧待发送，将当前数据轮盘出队并转移到空闲状态
				if (Public::countFrames(sendingInfo.sendingData.front()) == 0)
				{
					threadState = Public::ThreadState::Idle;
					emit pushMsg(QString::fromLocal8Bit("所有帧已被确认收到，转入空闲状态。"), id);

					sendingInfo.sendingData.pop_front();
					timePartTimer->start(Public::MSOfTimePart);
					for (unsigned int i(0); i != Public::RouletteSize; ++i)
						sendingInfo.timers[i]->stopTimer();

					timePartTimer->start(Public::MSOfTimePart);
				}
				else 
				{
					sendFrames();
				}
			}
		}
	}
	else
	{
		// 错误帧，丢弃该帧
		emit pushMsg(QString::fromLocal8Bit("接收到PKT信号或SYN信号，由于处于发送状态，不应收到ACK以外的信号，将抛弃该帧\n"), id);
	}
}

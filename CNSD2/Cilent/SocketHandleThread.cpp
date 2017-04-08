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
	: tcpSocket(_tcpSocket), id(_id), isServer(_isServer)
{
	for (unsigned int i(0), j(sendingInfo.timers.size()); i != j; ++i)
	{
		sendingInfo.timers[i] = new PKTTimer(i);
	}
}

void SocketHandleThread::start()
{
	stopped = false;
	run();
}

void SocketHandleThread::run()
{
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

	while (!stopped)
	{
		if (threadState == Public::ThreadState::Idle)
		{
			if (!sendingInfo.sendingData.empty())
			{
				// ת��׼������̬������SYN�ź�
				threadState = Public::ThreadState::WaitForSending;
				sendFrame(Public::RequestTypes::SYN, Public::countFrames(sendingInfo.sendingData.front()));
			}
		}
		else if (threadState == Public::ThreadState::Sending)
		{
			// ����Ƿ�����Ҫ���͵�����֡
			if (checkDataDeque())
				// ����У�����
				sendFrames();
			// ����ȴ�ACK�ظ�
		}
		else if (threadState == Public::ThreadState::WaitForSending)
		{
			// �ȴ�ACK�ظ�
		}
		else if (threadState == Public::ThreadState::Receieving)
		{
			// �ȴ�PKT���ݰ�����
		}
	}
}

void SocketHandleThread::dataReceived()
{
	emit pushMsg(QString::fromLocal8Bit("�յ�����֡��׼�����н���\n"));

	Public::State currFrameState(Public::getRandomFrameState());
	QDataStream in(tcpSocket);
	Public::DataFrame currFrame(in);

	std::ostringstream sout;
	sout << "��ǰ����õ���֡״̬Ϊ" << Public::getFrameStateString(currFrameState) << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	if (currFrameState == Public::FrameState::Lose)
		return;
	if (currFrameState == Public::FrameState::Wrong
		|| !currFrame.isCorrect())
	{
		emit pushMsg(QString::fromLocal8Bit("֡У�����������֡\n"));
		return;
	}

	sout.clear();
	sout << "֡У����ȷ������Ϊ��" << currFrame.data << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	emit pushMsg(QString::fromLocal8Bit("׼�����н���\n"));
	Public::decode(currFrame.data);
	emit pushMsg(QString::fromLocal8Bit("�������\n"));
	sout.clear();
	sout << "���ܺ�����Ϊ��" << currFrame.data << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	switch (threadState)
	{
	case Public::ThreadState::Idle:
		dataReceivedForIdle(currFrame, currFrameState);
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

void SocketHandleThread::dataReceivedForIdle(const Public::DataFrame &currFrame, Public::State frameState)
{
	if (currFrame.request == Public::RequestTypes::SYN)
	{
		// ��ʼ�����ݽ��ղ۲�ת�����״̬
		recievingInfo.waitingFrameId = recievingInfo.currFrameNum = 0;
		recievingInfo.buffFrameId.clear();
		for (std::deque<Public::DataFrame> &currDeque : recievingInfo.recievingData)
			currDeque.clear();
		recievingInfo.totalFrameNum = Public::str2ui(currFrame.data);

		std::ostringstream sout;
		sout << "���յ����ݷ������󣬹�" << recievingInfo.totalFrameNum << "������ת��������ݰ�״̬" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		threadState = Public::ThreadState::Receieving;
		
		// ����ACK(-1)
		if (frameState == Public::FrameState::NoReply)
			return;

		emit pushMsg(QString::fromLocal8Bit("��Է�����ACK�źţ�ʾ��ɷ������ݰ�\n"));
		sendFrame(Public::RequestTypes::ACK, -1);
	}
	else if (currFrame.request == Public::RequestTypes::PKT)
	{
		// ����֡��������֡��������ACK(-1)��֪�Է�������֡�ѽ������
		std::ostringstream sout;
		sout << "���յ����ݰ���֡���Ϊ" << currFrame.id << "�������޵ȴ��������ݰ�����������֡" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

		if (frameState == Public::FrameState::NoReply)
			return;

		emit pushMsg(QString::fromLocal8Bit("��Է�����ACK(-1)�źţ�ʾ�����������ѽ������\n"));
		sendFrame(Public::RequestTypes::ACK, -1);
	}
	else if (currFrame.request == Public::RequestTypes::ACK)
	{
		// ����֡��������֡
		std::ostringstream sout;
		sout << "���յ�ACK�źţ����ڴ��ڿ���״̬����Ӧ�ܵ�ACK�źţ���������֡" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	}
}

void SocketHandleThread::dataReceivedForReceiving(const Public::DataFrame &currFrame, Public::State frameState)
{
	if (currFrame.request == Public::RequestTypes::PKT)
	{
		// ֡���̽������ݣ������ض�Ӧ��ACK
		// ����յ�֡���Ϊϣ���յ���֡��ţ������ϣ���յ���֡��ţ�������ACK�źţ�����Ϊ�µ�ϣ���յ���֡���
		unsigned int currFrameId(currFrame.id);
		recievingInfo.recievingData[currFrameId].push_back(std::move(currFrame));
		std::ostringstream sout;
		sout << "���յ����ݰ�֡���Ϊ" << currFrameId << std::endl;
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

		if (currFrameId == recievingInfo.waitingFrameId)
		{
			++recievingInfo.currFrameNum;
			recievingInfo.waitingFrameId = ++recievingInfo.waitingFrameId % Public::RouletteSize;
			sout.clear();
			while (!recievingInfo.buffFrameId.empty() && recievingInfo.buffFrameId.find(recievingInfo.waitingFrameId) != recievingInfo.buffFrameId.cend())
			{
				sout << "���Ϊ" << recievingInfo.waitingFrameId << "��֡����֡������" << std::endl;
				++recievingInfo.currFrameNum;
				recievingInfo.buffFrameId.erase(recievingInfo.waitingFrameId);
				recievingInfo.waitingFrameId = ++recievingInfo.waitingFrameId % Public::RouletteSize;
			}

			if (frameState == Public::FrameState::FrameNoError)
			{
				sout << "����ͻ��˷���ACK(" << (recievingInfo.waitingFrameId - 1) << ")�ź�" << std::endl;
				sendFrame(Public::RequestTypes::ACK, recievingInfo.waitingFrameId == 0 ? Public::RouletteSize - 1 : recievingInfo.waitingFrameId - 1);
			}

			pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		}
		// ����յ�֡��Ų���ϣ���յ���֡��ţ���װ��֡�����У�������ACK�źţ�����Ϊϣ���յ���֡���
		else
		{
			recievingInfo.buffFrameId.insert(currFrameId);
			sout.clear();
			sout << "ϣ���յ���֡���Ϊ" << recievingInfo.waitingFrameId << "������֡װ��֡������" << std::endl;
			if (frameState == Public::FrameState::FrameNoError)
			{
				sout << "����ͻ��˷���ACK(" << recievingInfo.waitingFrameId << ")�ź�" << std::endl;
				sendFrame(Public::RequestTypes::ACK, recievingInfo.waitingFrameId);
			}
			pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		}

		// ������ݽ�����ϣ����ϼ��������ݲ�ת�Ƶ�����״̬��������ACK(-1)ʾ��Է��������
		if (recievingInfo.currFrameNum == recievingInfo.totalFrameNum)
		{
			std::pair<Public::RequestType, std::string> data(Public::readDataRoulette<std::string>(recievingInfo.recievingData));
			pushMsg(QString::fromLocal8Bit("���յ��������ݰ�����������������ݣ�ת�����״̬\n"));
			emit pushData(std::move(data.second));
			threadState = Public::ThreadState::Idle;

			if (frameState == Public::FrameState::NoReply)
				return;

			pushMsg(QString::fromLocal8Bit("��ͻ��˷���ACK(-1)�źţ�ʾ�����������ѽ������\n"));
			sendFrame(Public::RequestTypes::ACK, -1);
		}
	}
	else
	{
		// ����֡��������֡
		emit pushMsg(QString::fromLocal8Bit("���յ�ACK�źŻ�SYN�źţ����ڴ��ڽ���״̬����Ӧ�յ�PKT������źţ���������֡\n"));
	}
}

void SocketHandleThread::dataReceivedForWaitSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	if (currFrame.request == Public::RequestTypes::ACK)
	{
		unsigned int data(Public::str2ui(currFrame.data));
		if (data == -1)
		{
			// ��ʼ�����Ͳۣ���ת�뷢��״̬
			emit pushMsg(QString::fromLocal8Bit("���յ�ACK(-1)�źţ�ת�뷢�����ݰ�״̬"));

			sendingInfo.waitingAcceptId = 0;
			sendingInfo.lastSendedId = -1;
			threadState = Public::ThreadState::Sending;
		} 
		else 
		{
			// ����֡��������֡
			std::ostringstream sout;
			sout << "���յ�ACK(" << data << ")�źţ����ڴ��ڵȴ�����״̬����Ӧ�յ�ACK(-1)������źţ���������֡" << std::endl;
			emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		}
	}
	else if (currFrame.request == Public::RequestTypes::SYN)
	{
		// ����յ�SYN��ʾ�Է�Ҳת�Ƶ��˴�����״̬
		if (isServer)
		{
			emit pushMsg(QString::fromLocal8Bit("���յ�SYN�źţ����ڴ��ڵȴ�����״̬�����������ȷ��ͣ�ת�뷢�����ݰ�״̬"));
			// ������߳��Ƿ������߳�
			// ��ʼ�����Ͳۣ���ת�뷢��״̬
			sendingInfo.waitingAcceptId = 0;
			sendingInfo.lastSendedId = -1;
			threadState = Public::ThreadState::Sending;
		}
		else 
		{
			emit pushMsg(QString::fromLocal8Bit("���յ�SYN�źţ����ڴ��ڵȴ�����״̬�����������ȷ��ͣ��ͻ��˵ķ�����������ִ�У���ת��������ݰ�״̬"));
			// ������߳��ǿͻ����߳�
			// ���ʼ�����ղۣ���ת�Ƶ�����״̬��������ACK(-1)ʾ��Է��ɷ������ݰ�
			recievingInfo.waitingFrameId = recievingInfo.currFrameNum = 0;
			recievingInfo.buffFrameId.clear();
			for (std::deque<Public::DataFrame> &currDeque : recievingInfo.recievingData)
				currDeque.clear();
			recievingInfo.totalFrameNum = Public::str2ui(currFrame.data);

			std::ostringstream sout;
			sout << "���յ����ݷ������󣬹�" << recievingInfo.totalFrameNum << "������ת��������ݰ�״̬" << std::endl;
			emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
			threadState = Public::ThreadState::Receieving;

			// ����ACK(-1)
			if (frameState == Public::FrameState::NoReply)
				return;

			emit pushMsg(QString::fromLocal8Bit("��Է�����ACK�źţ�ʾ��ɷ������ݰ�\n"));
			sendFrame(Public::RequestTypes::ACK, -1);
		}
	}
	else if (currFrame.request == Public::RequestTypes::PKT)
	{
		// ����֡��������֡
		emit pushMsg(QString::fromLocal8Bit("���յ�PKT�źţ����ڴ��ڵȴ�����״̬����Ӧ�յ�PKT�źţ���������֡\n"));
	}
}

void SocketHandleThread::dataReceivedForSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	// ���������ϻ��յ�ACK(-1)���򽫵�ǰ�������̳��Ӳ�ת�Ƶ�����״̬
	// ����ֹͣ��Accept���ݰ���Ӧ�ļ�ʱ�������ƶ����ʹ��ڣ��������µ����ݰ�
}

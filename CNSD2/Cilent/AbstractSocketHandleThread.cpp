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
				// ת��׼������̬������SYN�ź�
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
			// �鿴�Ƿ��пɷ��͵����ݰ�
			// ����У�����
			// û����ȴ�ACK��Ӧ
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
	// ����յ�SYNת�Ƶ���״̬��������ACK(-1)
}

void SocketHandleThread::dataReceivedForReceiving(const Public::DataFrame &currFrame, Public::State frameState)
{
	// ֡���̽������ݣ������ض�Ӧ��ACK
	// ������ݽ�����ϣ����ϼ��������ݲ�ת�Ƶ�����״̬��������ACK(-1)ʾ��Է��������
}

void SocketHandleThread::dataReceivedForWaitSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	// ����յ�ACK(-1)ת�Ƶ�����״̬
}

void SocketHandleThread::dataReceivedForSending(const Public::DataFrame &currFrame, Public::State frameState)
{
	// ���������ϻ��յ�ACK(-1)����ת�Ƶ�����״̬
	// �����ƶ����ʹ���
}

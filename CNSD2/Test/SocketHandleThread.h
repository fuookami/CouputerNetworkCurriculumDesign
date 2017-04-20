#pragma once

#include "Public.h"
#include <set>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class PKTTimer : public QObject
{
	Q_OBJECT;

public:
	PKTTimer(unsigned char _id);
	void startTimer(const unsigned short ms = Public::MSOfOnceTry * Public::timeOfRetryTimePart);
	void stopTimer();

signals:
	void timeoutSignal(unsigned char);

	private slots:
	void timeoutSlot(void);

private:
	QTimer *timer;
	unsigned char id;
};

struct RecievingInfo
{
	unsigned int waitingFrameId;
	unsigned int totalFrameNum;
	unsigned int currFrameNum;
	std::set<unsigned int> buffFrameId;
	Public::DataRoulette recievingData;
};

struct SendingInfo
{
	unsigned int lastAcceptId;
	unsigned int lastSendedId;
	Public::DataDeque sendingData;
	std::array<PKTTimer *, Public::RouletteSize> timers;
};

class SocketHandleThread : public QThread
{
	Q_OBJECT;

public:
	SocketHandleThread(QTcpSocket *_tcpSocket, unsigned int id = 0, bool _isServer = false);
	static unsigned int getThreadCounter();
	void start();
	void stop();
	template<class Iter>
	void sendData(Iter bgIt, Iter edIt);

signals:
	void pushMsg(const QString msg, unsigned int id);
	void pushData(const Public::DataType data, unsigned int id);
	void socketDisconnected(unsigned int id);
	void stoped(unsigned int id);

protected:
	void run();

private slots:
	void dataReceived(void);
	void PKTTimeOut(unsigned char id);
	void socketDisconnectedSlot();
	void mainLoop(void);

private:
	void sendFrames(void);
	void sendFrame(Public::RequestType requestType, unsigned int arg);
	void sendFrame(const Public::DataFrame &frame);
	unsigned int calIdDistance(const unsigned int lastSendedId, const unsigned int lastAcceptId);

	void dataReceivedForIdle(const Public::DataFrame &currFrame, Public::State frameState = Public::FrameState::FrameNoError);
	void dataReceivedForReceiving(Public::DataFrame currFrame, Public::State frameState = Public::FrameState::FrameNoError);
	void dataReceivedForWaitSending(const Public::DataFrame &currFrame, Public::State frameState = Public::FrameState::FrameNoError);
	void dataReceivedForSending(const Public::DataFrame &currFrame, Public::State frameState = Public::FrameState::FrameNoError);

private:
	static unsigned int threadCounter;
	unsigned int id;
	bool isServer;

	RecievingInfo recievingInfo;
	SendingInfo sendingInfo;

	volatile Public::State threadState;
	QTcpSocket *tcpSocket;
	QTimer *timePartTimer;

	volatile bool stopped;
};

template<class Iter>
inline void SocketHandleThread::sendData(Iter bgIt, Iter edIt)
{
	if (stopped)
	{
		emit pushMsg(QString::fromLocal8Bit("管程已经进入准备关闭状态或已经是关闭状态，将无视该数据发送请求。"), id);
	}
	else 
	{
		sendingInfo.sendingData.push_back(Public::makeDataRoulette(Public::to_datatype(bgIt, edIt)));
	}
}
	

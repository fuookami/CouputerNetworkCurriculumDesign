#pragma once

#include "Public.h"
#include <set>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

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
	unsigned int waitingAcceptId;
	unsigned int lastSendedId;
	Public::DataDeque sendingData;
	std::array<PKTTimer *, Public::RouletteSize> timers;
};

class PKTTimer : public QObject
{
	Q_OBJECT;

public:
	PKTTimer(unsigned char _id);
	void startTimer(const unsigned short ms = Public::MSOfOnceTry);
	void stopTimer();

signals:
	void timeoutSignal(unsigned char);

	private slots:
	void timeoutSlot(void);

private:
	QTimer *timer;
	unsigned char id;
};

class SocketHandleThread : public QThread
{
	Q_OBJECT;

public:
	SocketHandleThread(QTcpSocket *_tcpSocket, unsigned int id);
	void start();
	template<class T>
	void sendData(const T &data);

signals:
	void pushMsg(const QString msg);
	void pushData(const std::string data);

protected:
	void run();

private slots:
	void dataReceived(void);
	void PKTTimeOut(unsigned char id);

private:
	bool checkDataDeque(void);
	void sendFrames(void);
	void sendFrame(Public::RequestType requestType, unsigned int data);

	void dataReceivedForIdea(const Public::DataFrame &currFrame, Public::State frameState = Public::FrameState::FrameNoError);
	void dataReceivedForReceiving(const Public::DataFrame &currFrame, Public::State frameState = Public::FrameState::FrameNoError);
	void dataReceivedForWaitSending(const Public::DataFrame &currFrame, Public::State frameState = Public::FrameState::FrameNoError);
	void dataReceivedForSending(const Public::DataFrame &currFrame, Public::State frameState = Public::FrameState::FrameNoError);

private:
	static unsigned int threadCounter;

	RecievingInfo recievingInfo;
	SendingInfo sendingInfo;

	Public::State threadState;
	QTcpSocket *tcpSocket;

	volatile bool stopped;
};
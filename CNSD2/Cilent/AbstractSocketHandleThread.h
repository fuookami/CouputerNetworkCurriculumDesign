#pragma once

#include "Public.h"
#include <set>
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
};

class SocketHandleThread : public QThread
{
public:
	SocketHandleThread(QTcpSocket *_tcpSocket, unsigned int id);

signals:
	void pushMsgSignal(const QString msg);
	void pushData(const std::string data);

protected:
	void run();

private slots:
	void dataReceived();

private:
	RecievingInfo recievingInfo;
	SendingInfo sendingInfo;

	Public::State threadState;
	QTcpSocket *tcpSocket;

	volatile bool stopped;
};
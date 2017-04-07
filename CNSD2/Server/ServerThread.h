#pragma once

#include "Public.h"
#include <set>
#include <iostream>
#include <sstream>
#include <QtCore/QDataStream>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class ServerThread : public QThread
{
	Q_OBJECT;

public:
	ServerThread(QTcpSocket *_tcpSocket, unsigned int id);
	static unsigned short getThreadCounter(void);
	void start(void);

signals:
	void pushMsgSignal(const QString msg);
	void pushData(const std::string data);
	void cilentDisconnected(unsigned short);

private slots:
	void dataReceived();
	void cilentDisconnectedSlot();

protected:
	void run();

private:
	void presendDispose();
	void sendingDispose();
	void pushMsg(const QString msg);

private:
	static unsigned short ThreadCounter;
	unsigned short id;

	bool isSendingDataPackbage;
	unsigned int waitingFrameId;
	unsigned int totalFrameNum;
	unsigned int currFrameNum;
	std::set<unsigned int> buffFrameId;
	Public::DataRoulette recievingData;

	QTcpSocket *tcpSocket;

	volatile bool stopped;
};
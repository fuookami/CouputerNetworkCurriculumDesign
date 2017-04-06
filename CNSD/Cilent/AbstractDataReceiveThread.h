#pragma once

#include "Public.h"
#include <set>
#include <QtCore/QDataStream>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class AbstractDataReceiveThread : public QThread
{
	Q_OBJECT

public:
	AbstractDataReceiveThread(QTcpSocket *_tcpSocket);
	void startListenDataReceived();

signals:
	void pushMsgSignal(const QString msg);
	void pushReply(Public::RequestType requestType, unsigned int arg = 0);
	void pushRequest(Public::RequestType requestType, unsigned int arg = 0);
	void pushData(const std::string data);

private slots:
	void dataReceived();

protected:
	void run();

private:
	void presendDispose();
	void sendingDispose();
	void pushMsg(const QString msg);

private:
	bool isSendingDataPackbage;
	unsigned int waitingFrameId;
	unsigned int totalFrameNum;
	unsigned int currFrameNum;
	std::set<unsigned int> buffFrameId;
	Public::DataRoulette recievingData;

	QTcpSocket *tcpSocket;
	volatile bool stopped;
};
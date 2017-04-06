#pragma once

#include "Public.h"
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class ServerThread : public QThread
{
	Q_OBJECT

public:
	ServerThread(QTcpSocket *_tcpSocket);
	static unsigned short getThreadCounter(void);
	void start(void);

signals:
	void cilentDisconnected(const unsigned short);
	void pushMsg(const std::string);

protected:
	void run();

private slots:
	void cilentDisconnectedSlot();
	void dataReceived();

private:
	void presendDispose();
	void sendingDispose();

private:
	static unsigned short ThreadCounter;
	unsigned short id;

	QTcpSocket *tcpSocket;

	bool isSending;
	unsigned int currFrameNum;
	unsigned int currOrder;
	Public::DataRoulette dataRoulette;

	volatile bool stopped;
};
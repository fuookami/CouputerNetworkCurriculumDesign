#pragma once

#include "Public.h"
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class ServerThread : public QThread
{
	Q_OBJECT;

public:
	static unsigned short counter;

public:
	ServerThread(QTcpSocket *_tcpSocket);

protected:
	void run();

signals:
	void cilentDisconnected(unsigned short id);
	void push(const std::string msg);

private slots:
	void cilentDisconnectedSlot();
	void dataReceived();

private:
	void presendDispose();
	void sendingDispose();

private:
	unsigned short id;
	QString cilentName;
	QTcpSocket *tcpSocket;
	bool isSending;
	unsigned int currFrameNum;
	unsigned int currOrder;
	Public::DataRoulette dataRoulette;
	volatile bool stopped;
};

unsigned short ServerThread::counter = 0;
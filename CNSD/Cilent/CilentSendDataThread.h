#pragma once

#include "Public.h"
#include <QtCore/QThread>
#include <QtCore/QDataStream>
#include <QtNetwork/QTcpSocket>

class Cilent;

class CilentSendDataThread : public QThread
{
	friend class Cilent;

	Q_OBJECT

private:
	CilentSendDataThread(QTcpSocket *);
	template<class T>
	void addData(Public::RequestType requestType, const T &data);
	unsigned char getDataQueueSize(void) const;

protected:
	void run();

private:
	unsigned char currACKOrder;
	unsigned char currSendedOrder;
	std::deque<std::string> datas;
	//Public::DataQueue datas;
	QTcpSocket *tcpSocket;
	volatile bool stopped;
};

template<class T>
inline void CilentSendDataThread::addData(Public::RequestType requestType, const T & data)
{
	datas.push_back("Fuck you!");
	//datas.push(std::move(Public::makeDataRoulette(requestType, data)));
	if (stopped)
	{
		stopped = false;
		run();
	}
}

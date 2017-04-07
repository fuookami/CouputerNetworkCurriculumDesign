#pragma once

#include "Public.h"
#include <set>
#include <QtCore/QTimer>
#include <QtCore/QDataStream>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

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

class AbstractDataSendThread : public QThread
{
	Q_OBJECT;

public:
	AbstractDataSendThread(QTcpSocket *_tcpSocket);
	void pushReply(Public::RequestType requestType, unsigned int arg = 0);
	void disposeRequest(Public::RequestType requestType, unsigned int arg = 0);
	template <class T>
	void addData(const T &data);

signals:
	void pushMsg(const QString msg);

protected:
	void run();

private slots:
	void packbackTimeout(unsigned char id);

private:
	void activeThread(void);
	void sendData(void);

private:
	bool isSendingDataPackbage;
	unsigned int acceptFrameId;
	std::set<unsigned int> sendedButNotAcceptFrameId;

	std::deque<Public::DataFrame> reply;

	std::array<PKTTimer *, Public::RouletteSize> timers;
	Public::DataDeque datas;

	QTcpSocket *tcpSocket;

	volatile bool stopped;
};

template<class T>
inline void AbstractDataSendThread::addData(const T & data)
{
	datas.push_back(Public::makeDataRoulette(data));

	std::string data(Public::ui2str(Public::countFrames(datas.back())));
	reply.push_back(Public::DataFrame(0, Public::RequestTypes::SYN, data.begin(), data.end())); // Ìí¼ÓSYNÐÅºÅ
	
	activeThread();
}
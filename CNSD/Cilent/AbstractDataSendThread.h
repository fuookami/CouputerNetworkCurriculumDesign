#pragma once

#include "Public.h"
#include <set>
#include <QtCore/QTimer>
#include <QtCore/QDataStream>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class PKTTimer : public QTimer
{
	Q_OBJECT

public:
	PKTTimer(unsigned char _id);

signals:


private slots:


private:
	unsigned char id;
};

class AbstractDataSendThread : public QObject
{
	Q_OBJECT

public:
	AbstractDataSendThread(QTcpSocket *_tcpSocket);
	void pushReply(Public::RequestType requestType, unsigned int arg = 0);
	template <class T>
	void addData(const T &data);

signals:
	void pushMsg(const QString msg);

private:
	bool isSendingDataPackbage;
	unsigned int acceptFrameId;
	std::set<unsigned int> sendedButNotAcceptFrameId;
	std::deque<Public::DataFrame> replys;
	Public::DataDeque datas;
};
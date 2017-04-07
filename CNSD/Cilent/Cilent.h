#pragma once

#include "AbstractDataReceiveThread.h"
#include "AbstractDataSendThread.h"

class Cilent : public QObject
{
	Q_OBJECT;

public:
	Cilent();

	Public::RetCode connectToHost(const QHostAddress &host, quint16 port, 
		unsigned char MaxRetryTime = Public::MaxRetryTime,
		unsigned short MSOfOnceTry = Public::MSOfOnceTry);
	template<class T>
	Public::RetCode wirteToHost(Public::RequestType requestType, const T &data);
	Public::RetCode disconnectFromHost();

signals:
	void pushMsgSignal(const QString);

private slots :
	void connectSucceed();
	void disconnectSucceed();

	void pushMsgSlot(const QString &msg);
	void dataReceivedSlot(const std::string data);
	void stateChangedSlot(QAbstractSocket::SocketState socketState);

private:
	void pushMsg(const QString &msg);

private:
	AbstractDataSendThread *sendDataThread;
	AbstractDataReceiveThread *receiveThread;
	QTcpSocket *tcpSocket;
};
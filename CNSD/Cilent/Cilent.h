#pragma once

#include "CilentSendDataThread.h"

class Cilent : public QObject
{
	Q_OBJECT

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
	CilentSendDataThread *sendDataThread;
	QTcpSocket *tcpSocket;
};

template<class T>
inline Public::RetCode Cilent::wirteToHost(Public::RequestType requestType, const T & data)
{
	if (sendDataThread == nullptr || tcpSocket->state() != QAbstractSocket::ConnectedState)
		return Public::RetCodes::StateError;

	sendDataThread->addData(requestType, data);
	std::ostringstream sout;
	sout << "数据已进入队列，" << "正在队列中的数据数量为：" << (unsigned int)sendDataThread->getDataQueueSize() << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	return Public::RetCodes::NoError;
}

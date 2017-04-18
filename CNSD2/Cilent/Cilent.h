#pragma once

#include "SocketHandleThread.h"
#include <QtNetwork/QHostAddress>

class Cilent : public QObject
{
	Q_OBJECT;

public:
	Cilent();

	Public::RetCode connectToHost(const QHostAddress &host, quint16 port,
		unsigned char MaxRetryTime = Public::MaxRetryTime,
		unsigned short MSOfOnceTry = Public::MSOfOnceTry);
	Public::State state() const;
	Public::RetCode disconnectFromHost();
	template<class T>
	Public::RetCode wirteToHost(const T &data);

signals:
	void pushMsg(const QString msg);
	void connected(void);
	void disconnected(void);

private slots:
	void getMsg(const QString msg);
	void getData(const std::string data);

	void connectSucceed();
	void stopThreadSucceed();
	void disconnectSucceed();
	void getStateChangedMsg(QAbstractSocket::SocketState socketState);

private:
	QTcpSocket *tcpSocket;
	SocketHandleThread *handleThread;
};

template<class T>
inline Public::RetCode Cilent::wirteToHost(const T & data)
{
	if (state() != QTcpSocket::ConnectedState)
		return Public::RetCodes::StateError;

	handleThread->sendData(data);
	return Public::RetCodes::NoError;
}

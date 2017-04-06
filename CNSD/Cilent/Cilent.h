#pragma once

#include "Public.h"
#include <QtNetwork/QTcpSocket>

class Cilent : public QObject
{
	Q_OBJECT

public:
	Cilent();

	Public::RetCode connectToHost(const QHostAddress &host, quint16 port);
	Public::RetCode disconnectFromHost();

signals:
	void pushMsg(const QString);

private slots :
	void connectSucceed();
	void disconnectSucceed();

	void stateChangedSlot(QAbstractSocket::SocketState socketState);

private:
	QTcpSocket *tcpSocket;
};
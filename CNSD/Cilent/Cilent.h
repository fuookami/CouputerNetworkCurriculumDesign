#pragma once

#include "Public.h"
#include <QtNetwork/QTcpSocket>

class Cilent : public QObject
{
	Q_OBJECT;

public:
	Cilent();

	void connectToHost(const QHostAddress &host, quint16 port);
	void disconnect();

signals:
	void pushMsg(const QString msg);

private slots :
	void connectSucceed();
	void stateChangedSlot();

private:
	QTcpSocket *tcpSocket;
};
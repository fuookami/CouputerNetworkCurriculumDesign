#include "Cilent.h"

Cilent::Cilent()
	: tcpSocket(new QTcpSocket(this))
{
	connect(tcpSocket, SIGNAL(stateChanged()), this, SLOT(stateChangedSlot()));
}

RetCode Cilent::connectToHost(const QHostAddress &host, quint16 port)
{
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connectSucceed()));

	tcpSocket->connectToHost(host, port);
}

void Cilent::connectSucceed()
{
	qDebug() << "连接成功";
}

void Cilent::stateChangedSlot()
{
	qDebug() << tcpSocket->state();
}
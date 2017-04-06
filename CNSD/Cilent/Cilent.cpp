#include "Cilent.h"

Cilent::Cilent()
	: tcpSocket(new QTcpSocket(nullptr))
{
	connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChangedSlot(QAbstractSocket::SocketState)));
}

Public::RetCode Cilent::connectToHost(const QHostAddress &host, quint16 port)
{
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connectSucceed()));

	tcpSocket->connectToHost(host, port);

	return Public::RetCodes::NoError;
}

Public::RetCode Cilent::disconnectFromHost()
{
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));

	tcpSocket->disconnectFromHost();

	return Public::RetCodes::NoError;
}

void Cilent::connectSucceed()
{
	qDebug() << QString::fromLocal8Bit("���ӳɹ�");

	disconnect(tcpSocket, SIGNAL(connected()));

	char *data = "fuck you!";
	tcpSocket->write(data, strlen(data));
}

void Cilent::disconnectSucceed()
{
	qDebug() << QString::fromLocal8Bit("�Ͽ��ɹ�");

	disconnect(tcpSocket, SIGNAL(disconnected()));
}

void Cilent::stateChangedSlot(QAbstractSocket::SocketState socketState)
{
	qDebug() << socketState;
}
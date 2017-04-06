#include "Cilent.h"
#include <QtNetwork/QHostAddress>

Cilent::Cilent()
	: tcpSocket(new QTcpSocket(nullptr))
{
	connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChangedSlot(QAbstractSocket::SocketState)));
}

Public::RetCode Cilent::connectToHost(const QHostAddress &host, quint16 port, unsigned char MaxRetryTime, unsigned short MSOfOnceTry)
{
	if (tcpSocket->state() != QAbstractSocket::UnconnectedState)
		return Public::RetCodes::StateError;

	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connectSucceed()));
	std::ostringstream sout;
	sout << "���ڳ��Ե�һ�����ӣ�Ŀ��Ϊ" << host.toString().toStdString() << ":" << port << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	tcpSocket->connectToHost(host, port);

	for (unsigned int i(1); i != MaxRetryTime && tcpSocket->state() == QAbstractSocket::UnconnectedState; ++i)
	{
		sout.clear();
		sout << "����ʧ�ܣ�" << MSOfOnceTry << "ms���ٴγ��Ե�" << i + 1 << " / " << MaxRetryTime << "������" << std::endl;
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		tcpSocket->waitForConnected(MSOfOnceTry);
	}
	
	if (tcpSocket->state() == QAbstractSocket::ConnectedState)
		return Public::RetCodes::NoError;
	else
		return Public::RetCodes::ServerLose;
}

Public::RetCode Cilent::disconnectFromHost()
{
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));

	tcpSocket->disconnectFromHost();

	return Public::RetCodes::NoError;
}

void Cilent::connectSucceed()
{
	pushMsg(QString::fromLocal8Bit("���ӳɹ�������׼���շ�����"));
	disconnect(tcpSocket, SIGNAL(connected()));

	sendDataThread = new CilentSendDataThread(tcpSocket);

	for (unsigned int i(0); i != 10000; ++i)
		wirteToHost(0, 1);
}

void Cilent::disconnectSucceed()
{
	pushMsg(QString::fromLocal8Bit("�Ͽ��ɹ�"));
	disconnect(tcpSocket, SIGNAL(disconnected()));

	delete sendDataThread;
}

void Cilent::pushMsgSlot(const QString &msg)
{
	pushMsg(msg);
}

void Cilent::dataReceivedSlot(const std::string data)
{
	pushMsg(QString::fromLocal8Bit(data.c_str()));
}

void Cilent::stateChangedSlot(QAbstractSocket::SocketState socketState)
{
	switch (socketState)
	{
	case QAbstractSocket::HostLookupState:
		pushMsg(QString::fromLocal8Bit("�Ѿ��ҵ�Ŀ�������"));
		break;
	case QAbstractSocket::ConnectingState:
		pushMsg(QString::fromLocal8Bit("���ڳ�������"));
		break;
	case QAbstractSocket::ClosingState:
		pushMsg(QString::fromLocal8Bit("���ڳ��ԶϿ��������������"));
		break;
	default:
		break;
	}
}

void Cilent::pushMsg(const QString &msg)
{
	qDebug() << msg;
	//emit pushMsgSignal(std::move(msg));
}

#include "Cilent.h"

Cilent::Cilent()
	: tcpSocket(new QTcpSocket(nullptr)), handleThread(new SocketHandleThread(tcpSocket))
{
	connect(handleThread, SIGNAL(pushMsg(const QString)), this, SLOT(getMsg(const QString)));
	connect(handleThread, SIGNAL(pushData(const QString)), this, SLOT(getData(const QString)));
	connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(getStateChangedMsg(QAbstractSocket::SocketState)));
}

Public::RetCode Cilent::connectToHost(const QHostAddress &host, quint16 port, 
	unsigned char MaxRetryTime /* = Public::MaxRetryTime */, unsigned short MSOfOnceTry /* = Public::MSOfOnceTry */)
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
	{
		handleThread->start();
		emit pushMsg(QString::fromLocal8Bit("�ѿ���Socket�ܳ�\n"));
		return Public::RetCodes::NoError;
	}
	else
		return Public::RetCodes::ServerLose;
}

Public::State Cilent::state() const
{
	return tcpSocket->state();
}

Public::RetCode Cilent::disconnectFromHost()
{
	if (state() != QTcpSocket::ConnectedState)
		return Public::RetCodes::StateError;

	handleThread->stop();
	connect(handleThread, SIGNAL(stoped()), this, SLOT(stopThreadSucceed()));

	return Public::RetCodes::NoError;
}

void Cilent::getMsg(const QString msg)
{
	emit pushMsg(std::move(msg));
}

void Cilent::getData(const std::string data)
{
	emit pushMsg(QString::fromLocal8Bit(data.c_str()));
}

void Cilent::connectSucceed()
{
	emit pushMsg(QString::fromLocal8Bit("���ӳɹ�������׼���շ�����\n"));
	disconnect(tcpSocket, SIGNAL(connected()));

	handleThread->start();
}

void Cilent::stopThreadSucceed()
{
	disconnect(handleThread, SIGNAL(stoped()));
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));
	
	emit pushMsg(QString::fromLocal8Bit("�ѹر�Socket�ܳ�\n"));
	std::ostringstream sout;
	emit pushMsg(QString::fromLocal8Bit("���ڳ��Խ��е�һ�ζϿ�����\n"));
	tcpSocket->disconnectFromHost();

	while (state() != QAbstractSocket::UnconnectedState)
	{
		sout << "�Ͽ�����ʧ�ܣ������������ļ����ڶ�ȡ�����У�" << Public::MSOfOnceTry << "ms���ٴγ���" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		tcpSocket->waitForDisconnected(Public::MSOfOnceTry);
	}
}

void Cilent::disconnectSucceed()
{
	emit pushMsg(QString::fromLocal8Bit("�Ͽ��ɹ�\n"));
	disconnect(tcpSocket, SIGNAL(disconnected()));
}

void Cilent::getStateChangedMsg(QAbstractSocket::SocketState socketState)
{
	switch (socketState)
	{
	case QAbstractSocket::HostLookupState:
		emit pushMsg(QString::fromLocal8Bit("�Ѿ��ҵ�Ŀ�������\n"));
		break;
	case QAbstractSocket::ConnectingState:
		emit pushMsg(QString::fromLocal8Bit("���ڳ�������\n"));
		break;
	case QAbstractSocket::ClosingState:
		emit pushMsg(QString::fromLocal8Bit("���ڳ��ԶϿ��������������\n"));
		break;
	default:
		break;
	}
}
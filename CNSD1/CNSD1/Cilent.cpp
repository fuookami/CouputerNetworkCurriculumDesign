#include "Cilent.h"
#include <iostream>
#include <sstream>

Cilent::Cilent()
	:socket(new QAbstractSocket(QAbstractSocket::UnknownSocketType, nullptr))
{
	connect(socket, SIGNAL(error()), this, SLOT(pushErrorMsg()));
}

RetCode Cilent::connectToHost(const QString & host, quint16 port,
	quint8 MaxRetryTime, quint16 MSOfOnceTry)
{
	if (state() != QAbstractSocket::UnconnectedState)
		return BasicSetting::RetCodes::StateError;

	connect(socket, SIGNAL(hostFound()), this, SLOT(hostFounded()));

	unsigned char counter = 1;
	socket->connectToHost(host, port);
	pushMsg(QString::fromLocal8Bit("���ڳ��Խ��е�1������"));

	for (; state() == QAbstractSocket::UnconnectedState &&
		counter != MaxRetryTime; ++counter)
	{
		std::ostringstream sout;
		sout << "����ʧ�ܣ�" << MSOfOnceTry << "ms�󽫳��Ե�" << counter + 1 << "/" << MaxRetryTime << "������";
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		socket->waitForConnected(MSOfOnceTry);
	}

	return state() == QAbstractSocket::ConnectedState ? 
		BasicSetting::RetCodes::NoError : BasicSetting::RetCodes::ServerLose;
}

RetCode Cilent::disconnectFromHost(quint16 MSOfOnceTry = BasicSetting::MSOfOnceTry)
{
	if (state() != QAbstractSocket::ConnectedState)
		return BasicSetting::RetCodes::StateError;

	connect(socket, SIGNAL(disconnected()), this, SLOT());

	socket->disconnectFromHost();
	pushMsg(QString::fromLocal8Bit("���ڳ��ԶϿ�����"));

	for (; state() == QAbstractSocket::ConnectedState;)
	{
		std::ostringstream sout;
		sout << "�Ͽ�����ʧ�ܣ����ܴ�����������ͨ�������У�" << MSOfOnceTry << "ms������´γ���";
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		socket->waitForDisconnected(MSOfOnceTry);
	}

	return BasicSetting::RetCodes::NoError;
}

State Cilent::state(void) const
{
	return socket->state();
}

void Cilent::hostFounded(void)
{
	disconnect(socket, SIGNAL(hostFound()));
	connect(socket, SIGNAL(connected()), this, SLOT(connectSucceed()));
	emit pushedMsg(QString::fromLocal8Bit("���ҵ�Ŀ���������׼���������ӡ�"));
}

void Cilent::connectSucceed(void)
{
	disconnect(socket, SIGNAL(connected()));
	emit pushedMsg(QString::fromLocal8Bit("�ѳɹ�����Ŀ�������"));
}

void Cilent::disconnectSucceed(void)
{
	disconnect(socket, SIGNAL(disconnect()));
	emit pushedMsg(QString::fromLocal8Bit("�ѳɹ���������Ͽ�����"));
}

void Cilent::pushErrorMsg(void)
{
	pushMsg(socket->errorString());
}

void Cilent::pushMsg(const QString & msg)
{
	emit pushedMsg(msg);
}
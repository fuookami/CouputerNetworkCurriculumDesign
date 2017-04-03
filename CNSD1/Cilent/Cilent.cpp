#include "Cilent.h"
#include <QtWidgets/QMessageBox>
#include <iostream>
#include <sstream>

Cilent::Cilent()
	:udpSocket(new QUdpSocket(nullptr))
{
	//connect(socket, SIGNAL(error()), this, SLOT(pushErrorMsg()));
}

RetCode Cilent::connectToHost(const QHostAddress & host, quint16 port,
	quint8 MaxRetryTime, quint16 MSOfOnceTry)
{
	if (state() != QAbstractSocket::UnconnectedState)
		return BasicSetting::RetCodes::StateError;

	connect(udpSocket, SIGNAL(hostFound()), this, SLOT(hostFounded()));
	connect(udpSocket, SIGNAL(connected()), this, SLOT(connectSucceed()));

	unsigned char counter = 1;
	pushMsg(QString::fromLocal8Bit("���ڳ��Խ��е�1������"));
	udpSocket->connectToHost(QHostAddress::LocalHost, port);

	for (; state() == QAbstractSocket::UnconnectedState &&
		counter != MaxRetryTime; ++counter)
	{
		std::ostringstream sout;
		sout << "����ʧ�ܣ�" << MSOfOnceTry << "ms�󽫳��Ե�" << counter + 1 << "/" << MaxRetryTime << "������";
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		udpSocket->waitForConnected(MSOfOnceTry);
	}

	disconnect(udpSocket, SIGNAL(hostFound()));

	return state() == QAbstractSocket::ConnectedState ? 
		BasicSetting::RetCodes::NoError : BasicSetting::RetCodes::ServerLose;
}

RetCode Cilent::disconnectFromHost(quint16 MSOfOnceTry)
{
	if (state() != QAbstractSocket::ConnectedState)
		return BasicSetting::RetCodes::StateError;

	connect(udpSocket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));

	pushMsg(QString::fromLocal8Bit("���ڳ��ԶϿ�����"));
	udpSocket->disconnectFromHost();

	for (; state() == QAbstractSocket::ConnectedState;)
	{
		std::ostringstream sout;
		sout << "�Ͽ�����ʧ�ܣ����ܴ�����������ͨ�������У�" << MSOfOnceTry << "ms������´γ���";
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		udpSocket->waitForDisconnected(MSOfOnceTry);
	}

	disconnect(udpSocket, SIGNAL(disconnect()));
	return BasicSetting::RetCodes::NoError;
}

State Cilent::state(void) const
{
	return udpSocket->state();
}

void Cilent::hostFounded(void)
{
	pushMsg(QString::fromLocal8Bit("���ҵ�Ŀ���������׼���������ӡ�"));
}

void Cilent::connectSucceed(void)
{
	disconnect(udpSocket, SIGNAL(connected()));
	pushMsg(QString::fromLocal8Bit("�ѳɹ�����Ŀ�������"));
}

void Cilent::disconnectSucceed(void)
{
	pushMsg(QString::fromLocal8Bit("�ѳɹ���������Ͽ�����"));
}

void Cilent::pushErrorMsg(void)
{
	pushMsg(udpSocket->errorString());
}

void Cilent::pushMsg(const QString & msg)
{
	qDebug() << msg;
	//emit pushedMsg(msg);
}
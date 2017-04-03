#include "Cilent.h"
#include <QtWidgets/QMessageBox>
#include <iostream>
#include <sstream>

Cilent::Cilent()
	:socket(new QAbstractSocket(QAbstractSocket::UnknownSocketType, nullptr))
{
	//connect(socket, SIGNAL(error()), this, SLOT(pushErrorMsg()));
}

RetCode Cilent::connectToHost(const QString & host, quint16 port,
	quint8 MaxRetryTime, quint16 MSOfOnceTry)
{
	if (state() != QAbstractSocket::UnconnectedState)
		return BasicSetting::RetCodes::StateError;

	connect(socket, SIGNAL(hostFound()), this, SLOT(hostFounded()));
	connect(socket, SIGNAL(connected()), this, SLOT(connectSucceed()));

	unsigned char counter = 1;
	socket->connectToHost(host, port);
	pushMsg(QString::fromLocal8Bit("正在尝试进行第1次链接"));

	for (; state() == QAbstractSocket::UnconnectedState &&
		counter != MaxRetryTime; ++counter)
	{
		std::ostringstream sout;
		sout << "链接失败，" << MSOfOnceTry << "ms后将尝试第" << counter + 1 << "/" << MaxRetryTime << "次链接";
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		socket->waitForConnected(MSOfOnceTry);
	}

	disconnect(socket, SIGNAL(hostFound()));
	disconnect(socket, SIGNAL(connected()));

	return state() == QAbstractSocket::ConnectedState ? 
		BasicSetting::RetCodes::NoError : BasicSetting::RetCodes::ServerLose;
}

RetCode Cilent::disconnectFromHost(quint16 MSOfOnceTry)
{
	if (state() != QAbstractSocket::ConnectedState)
		return BasicSetting::RetCodes::StateError;

	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));

	socket->disconnectFromHost();
	pushMsg(QString::fromLocal8Bit("正在尝试断开链接"));

	for (; state() == QAbstractSocket::ConnectedState;)
	{
		std::ostringstream sout;
		sout << "断开链接失败，可能存在数据仍在通道传输中，" << MSOfOnceTry << "ms后进行下次尝试";
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		socket->waitForDisconnected(MSOfOnceTry);
	}

	disconnect(socket, SIGNAL(disconnect()));
	return BasicSetting::RetCodes::NoError;
}

State Cilent::state(void) const
{
	return socket->state();
}

void Cilent::hostFounded(void)
{
	pushMsg(QString::fromLocal8Bit("已找到目标服务器，准备尝试链接。"));
}

void Cilent::connectSucceed(void)
{
	pushMsg(QString::fromLocal8Bit("已成功链接目标服务器"));
}

void Cilent::disconnectSucceed(void)
{
	pushMsg(QString::fromLocal8Bit("已成功与服务器断开链接"));
}

void Cilent::pushErrorMsg(void)
{
	pushMsg(socket->errorString());
}

void Cilent::pushMsg(const QString & msg)
{
	qDebug() << msg;
	//emit pushedMsg(msg);
}
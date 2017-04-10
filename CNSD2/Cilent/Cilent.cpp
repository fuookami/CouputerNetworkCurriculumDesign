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
	sout << "正在尝试第一次连接，目标为" << host.toString().toStdString() << ":" << port << std::endl;
	pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	tcpSocket->connectToHost(host, port);

	for (unsigned int i(1); i != MaxRetryTime && tcpSocket->state() == QAbstractSocket::UnconnectedState; ++i)
	{
		sout.clear();
		sout << "连接失败，" << MSOfOnceTry << "ms后再次尝试第" << i + 1 << " / " << MaxRetryTime << "次连接" << std::endl;
		pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		tcpSocket->waitForConnected(MSOfOnceTry);
	}

	if (tcpSocket->state() == QAbstractSocket::ConnectedState)
	{
		handleThread->start();
		emit pushMsg(QString::fromLocal8Bit("已开启Socket管程\n"));
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
	emit pushMsg(QString::fromLocal8Bit("连接成功，可以准备收发数据\n"));
	disconnect(tcpSocket, SIGNAL(connected()));

	handleThread->start();
}

void Cilent::stopThreadSucceed()
{
	disconnect(handleThread, SIGNAL(stoped()));
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectSucceed()));
	
	emit pushMsg(QString::fromLocal8Bit("已关闭Socket管程\n"));
	std::ostringstream sout;
	emit pushMsg(QString::fromLocal8Bit("正在尝试进行第一次断开连接\n"));
	tcpSocket->disconnectFromHost();

	while (state() != QAbstractSocket::UnconnectedState)
	{
		sout << "断开连接失败，可能是仍有文件正在读取或发送中，" << Public::MSOfOnceTry << "ms后再次尝试" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		tcpSocket->waitForDisconnected(Public::MSOfOnceTry);
	}
}

void Cilent::disconnectSucceed()
{
	emit pushMsg(QString::fromLocal8Bit("断开成功\n"));
	disconnect(tcpSocket, SIGNAL(disconnected()));
}

void Cilent::getStateChangedMsg(QAbstractSocket::SocketState socketState)
{
	switch (socketState)
	{
	case QAbstractSocket::HostLookupState:
		emit pushMsg(QString::fromLocal8Bit("已经找到目标服务器\n"));
		break;
	case QAbstractSocket::ConnectingState:
		emit pushMsg(QString::fromLocal8Bit("正在尝试连接\n"));
		break;
	case QAbstractSocket::ClosingState:
		emit pushMsg(QString::fromLocal8Bit("正在尝试断开与服务器的连接\n"));
		break;
	default:
		break;
	}
}
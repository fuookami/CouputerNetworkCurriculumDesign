#include "Cilent.h"

Cilent::Cilent()
	: tcpSocket(new QTcpSocket(nullptr)), handleThread(new SocketHandleThread(tcpSocket)), connectTimer(new QTimer()), thisMSOfOnceTry(0)
{
	connect(handleThread, SIGNAL(pushMsg(const QString, unsigned int)), this, SLOT(getMsg(const QString, unsigned int)));
	connect(handleThread, SIGNAL(pushData(const std::string, unsigned int)), this, SLOT(getData(const std::string, unsigned int)));
	connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(getStateChangedMsg(QAbstractSocket::SocketState)));
}

Public::RetCode Cilent::connectToHost(const QHostAddress &host, quint16 port, 
	unsigned char MaxRetryTime /* = Public::MaxRetryTime */, unsigned short MSOfOnceTry /* = Public::MSOfOnceTry */)
{
	if (tcpSocket->state() != QAbstractSocket::UnconnectedState)
		return Public::RetCodes::StateError;

	thisMSOfOnceTry = MSOfOnceTry;
	maxTryTime = MaxRetryTime;
	thisTryTime = 0;
	thisHost = host;
	thisPort = port;

	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connectSucceed()));
	std::ostringstream sout;
	sout << "正在尝试第一次连接，目标为" << host.toString().toStdString() << ":" << port << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
	tcpSocket->connectToHost(host, port);

	return Public::RetCodes::NoError;
}

Public::State Cilent::state() const
{
	return tcpSocket->state();
}

Public::RetCode Cilent::disconnectFromHost()
{
	if (state() != QTcpSocket::ConnectedState)
		return Public::RetCodes::StateError;

	connect(handleThread, SIGNAL(stoped(unsigned int)), this, SLOT(stopThreadSucceed(unsigned int)));
	handleThread->stop();

	return Public::RetCodes::NoError;
}

void Cilent::getMsg(const QString msg, unsigned int id)
{
	emit pushMsg(std::move(msg));
}

void Cilent::getData(const std::string data, unsigned int id)
{
	emit pushMsg(QString::fromLocal8Bit(data.c_str()));
}

void Cilent::connectSucceed()
{
	emit cilentConnected();
	emit pushMsg(QString::fromLocal8Bit("连接成功，可以准备收发数据\n"));
	disconnect(tcpSocket, SIGNAL(connected()));

	emit pushMsg(QString::fromLocal8Bit("已开启Socket管程\n"));
	handleThread->start();
}

void Cilent::stopThreadSucceed(unsigned int)
{
	disconnect(handleThread, SIGNAL(stoped(unsigned int)));
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
	emit cilentDisconnected();
	emit pushMsg(QString::fromLocal8Bit("断开成功\n"));
	disconnect(tcpSocket, SIGNAL(disconnected()));
}

void Cilent::getStateChangedMsg(QAbstractSocket::SocketState socketState)
{
	switch (socketState)
	{
	case QAbstractSocket::HostLookupState:
		emit pushMsg(QString::fromLocal8Bit("已经找到目标地址\n"));
		break;
	case QAbstractSocket::ConnectingState:
		emit pushMsg(QString::fromLocal8Bit("正在尝试连接\n"));
		break;
	case QAbstractSocket::UnconnectedState:
		if (thisMSOfOnceTry != 0)
		{
			if (++thisTryTime == maxTryTime)
			{
				thisMSOfOnceTry = 0;
				emit serverLose();
				emit pushMsg(QString::fromLocal8Bit("超过最大尝试次数，服务器丢失，请确认网络是否正常\n"));
			}
			else 
			{
				std::ostringstream sout;
				sout << "连接失败，" << thisMSOfOnceTry << "ms后再次尝试第" << thisTryTime + 1 << " / " << maxTryTime << "次连接" << std::endl;
				emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
				tcpSocket->waitForConnected(thisMSOfOnceTry);
				tcpSocket->connectToHost(thisHost, thisPort);
			}
		}
		break;
	case QAbstractSocket::ClosingState:
		emit pushMsg(QString::fromLocal8Bit("正在尝试断开与服务器的连接\n"));
		break;
	default:
		break;
	}
}
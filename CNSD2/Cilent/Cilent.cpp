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
	sout << "���ڳ��Ե�һ�����ӣ�Ŀ��Ϊ" << host.toString().toStdString() << ":" << port << std::endl;
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
	emit pushMsg(QString::fromLocal8Bit("���ӳɹ�������׼���շ�����\n"));
	disconnect(tcpSocket, SIGNAL(connected()));

	emit pushMsg(QString::fromLocal8Bit("�ѿ���Socket�ܳ�\n"));
	handleThread->start();
}

void Cilent::stopThreadSucceed(unsigned int)
{
	disconnect(handleThread, SIGNAL(stoped(unsigned int)));
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
	emit cilentDisconnected();
	emit pushMsg(QString::fromLocal8Bit("�Ͽ��ɹ�\n"));
	disconnect(tcpSocket, SIGNAL(disconnected()));
}

void Cilent::getStateChangedMsg(QAbstractSocket::SocketState socketState)
{
	switch (socketState)
	{
	case QAbstractSocket::HostLookupState:
		emit pushMsg(QString::fromLocal8Bit("�Ѿ��ҵ�Ŀ���ַ\n"));
		break;
	case QAbstractSocket::ConnectingState:
		emit pushMsg(QString::fromLocal8Bit("���ڳ�������\n"));
		break;
	case QAbstractSocket::UnconnectedState:
		if (thisMSOfOnceTry != 0)
		{
			if (++thisTryTime == maxTryTime)
			{
				thisMSOfOnceTry = 0;
				emit serverLose();
				emit pushMsg(QString::fromLocal8Bit("��������Դ�������������ʧ����ȷ�������Ƿ�����\n"));
			}
			else 
			{
				std::ostringstream sout;
				sout << "����ʧ�ܣ�" << thisMSOfOnceTry << "ms���ٴγ��Ե�" << thisTryTime + 1 << " / " << maxTryTime << "������" << std::endl;
				emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
				tcpSocket->waitForConnected(thisMSOfOnceTry);
				tcpSocket->connectToHost(thisHost, thisPort);
			}
		}
		break;
	case QAbstractSocket::ClosingState:
		emit pushMsg(QString::fromLocal8Bit("���ڳ��ԶϿ��������������\n"));
		break;
	default:
		break;
	}
}
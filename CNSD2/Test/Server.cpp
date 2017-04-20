#include "Server.h"

Server::Server() : tcpServer(new QTcpServer(nullptr))
{

}

Public::RetCode Server::listen(const QHostAddress host, quint16 port)
{
	tcpServer->listen(host, port);

	std::ostringstream sout;
	sout << "��ʼ����" << host.toString().toStdString() << ":" << port << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(getConnection()));

	return Public::RetCodes::NoError;
}

Public::RetCode Server::close()
{
	if (tcpSocketThreads.empty())
	{
		emit pushMsg(QString::fromLocal8Bit("��ǰû�пͻ�������������ӣ���ֱ�ӹرշ�������\n"));
		emit closed();
	}
	else {
		std::ostringstream sout;
		sout << "��ǰ��" << tcpSocketThreads.size() << "���ͻ�������������ӣ�������Ͽ����Ӻ��ٹرշ�������" << std::endl;
		emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));
		for (auto & th : tcpSocketThreads)
		{
			th.second->stop();
			connect(th.second.get(), SIGNAL(stopped(unsigned int)), this, SLOT(socketHandleThreadStopped(unsigned int)));
		}
	}
	
	return Public::RetCodes::NoError;
}

void Server::getMsg(const QString msg, unsigned int id)
{
	std::ostringstream sout;
	sout << "�ͻ���" << id << "��";
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()) + std::move(msg));
}

void Server::getData(const Public::DataType data, unsigned int id)
{
	std::ostringstream sout;
	sout << "�ͻ���" << id << "�����ݴ��룺" << Public::data2uiHex(data) << "��׼�������������ݡ�" << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	std::string ret(dispose(data));
	sout.clear();
	sout << "�ͻ���" << id << "�������������ݺ�׼���ظ�" << ret << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	tcpSocketThreads[id]->sendData(std::cbegin(ret), std::cend(ret));
}

void Server::getConnection()
{
	unsigned int thisID(SocketHandleThread::getThreadCounter());
	std::ostringstream sout;
	sout << "�������пͻ��˽��룬������Ϊ" << thisID << std::endl;
	emit pushMsg(QString::fromLocal8Bit(sout.str().c_str()));

	tcpSocketThreads.insert(std::make_pair(thisID,
		std::shared_ptr<SocketHandleThread>(new SocketHandleThread(tcpServer->nextPendingConnection(), thisID))));
	SocketHandleThread *serverThread(tcpSocketThreads[thisID].get());
	connect(serverThread, SIGNAL(socketDisconnected(unsigned int)),
		this, SLOT(cilentDisconnected(unsigned int)));
	connect(serverThread, SIGNAL(pushMsg(const QString, unsigned int)),
		this, SLOT(getMsg(const QString, unsigned int)));
	connect(serverThread, SIGNAL(pushData(const std::string, unsigned int)),
		this, SLOT(getData(const std::string, unsigned int)));
	serverThread->start();
}

void Server::cilentDisconnected(const unsigned int id)
{
	tcpSocketThreads[id].reset();
	tcpSocketThreads.erase(id);
}

void Server::socketHandleThreadStopped(unsigned int id)
{
	tcpSocketThreads[id].reset();
	tcpSocketThreads.erase(id);
}

std::string Server::dispose(const Public::DataType data)
{
	unsigned char count(0);
	for (unsigned int i(0), j(data.size()); i != j; ++i)
		count += data[i];
	return std::to_string(count);
}

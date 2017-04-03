#include "Server.h"
#include <string>

Server::Server()
	: server(new QUdpSocket(nullptr))
{

}

RetCode Server::listen(const QHostAddress &host, quint16 port)
{
	if (state() != QUdpSocket::UnconnectedState)
		return BasicSetting::StateError;

	if (server->bind(host, port))
	{
		currHost = host;
		currPort = port;
		connect(server, SIGNAL(readyRead()), this, SLOT(processDatagrams()));
		return BasicSetting::NoError;
	}

	return state() == QUdpSocket::BoundState ? BasicSetting::NoError
		: BasicSetting::BoundFail;
}

QHostAddress Server::getHost(void) const
{
	return currHost;
}

quint16 Server::getPort(void) const
{
	return currPort;
}

QString Server::getHostAndPortStr(void) const
{
	return currHost.toString() + QString::fromLocal8Bit(":") + QString(std::to_string(currPort).c_str());
}

State Server::state(void) const
{
	return server->state();
}

void Server::processDatagrams(void)
{
	QByteArray datagram;

	do {
		datagram.resize(server->pendingDatagramSize());
		server->readDatagram(datagram.data(), datagram.size());
	} while (server->hasPendingDatagrams());
}
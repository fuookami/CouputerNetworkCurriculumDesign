#include "Cilent.h"

Cilent::Cilent()
	:socket(QAbstractSocket::UnknownSocketType, nullptr)
{
}

void Cilent::connectToHost(const QString & host, quint16 port)
{
	connect(&socket, SIGNAL(hostFound()), this, SLOT(connectSucceed()));
	connect(&socket, SIGNAL(error()), this, SLOT(connectUnsucceed()));

	socket.connectToHost(host, port);
}

void Cilent::connectSucceed()
{
}

void Cilent::connectUnsucceed()
{

}
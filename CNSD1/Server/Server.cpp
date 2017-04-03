#include "Server.h"

Server::Server()
	:socket(new QAbstractSocket(QAbstractSocket::UnknownSocketType, nullptr))
{
	bindToHost();
}

State Server::state(void) const
{
	return socket->state();
}

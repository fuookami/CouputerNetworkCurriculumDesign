#include "Server.h"

Server::Server()
	:socket(QAbstractSocket::UnknownSocketType, nullptr)
{
	socket.bind(BasicSetting::port);
}

State Server::state(void) const
{
	return socket.state();
}

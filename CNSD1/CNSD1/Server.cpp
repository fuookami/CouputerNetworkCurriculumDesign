#include "Server.h"

Server::Server()
	:socket(QAbstractSocket::UnknownSocketType, nullptr)
{
	socket.bind(BasicSetting::port);
}

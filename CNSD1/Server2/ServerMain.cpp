#include "Server.h"
#include <QtWidgets/QApplication>
#include <QtNetwork/QHostAddress>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	Server server;

	return a.exec();
}

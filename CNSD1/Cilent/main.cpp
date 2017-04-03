#include <QtWidgets/QApplication>
#include "Server.h"
#include "Cilent.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Server server;
	Cilent cilent;

	cilent.connectToHost(BasicSetting::host, BasicSetting::port);

	return a.exec();
}

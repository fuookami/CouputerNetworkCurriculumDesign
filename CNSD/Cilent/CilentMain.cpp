#include "Cilent.h"
#include <QtNetwork/QHostAddress>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Cilent cilent;
	cilent.connectToHost(QHostAddress::LocalHost, 19999);
	cilent.disconnectFromHost();
	return a.exec();
}

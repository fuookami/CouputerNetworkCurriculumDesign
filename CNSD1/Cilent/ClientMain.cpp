#include <QtWidgets/QApplication>
#include "Cilent.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Cilent cilent;
	cilent.connectToHost(QHostAddress::LocalHost, BasicSetting::port);

	return a.exec();
}

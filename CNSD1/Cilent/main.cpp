#include <QtWidgets/QApplication>
#include "Cilent.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Cilent cilent;
	cilent.connectToHost(BasicSetting::host, BasicSetting::port);

	return a.exec();
}

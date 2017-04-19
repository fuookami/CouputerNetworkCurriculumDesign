#include "CilentWindow.h"
#include "ServerWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CilentWindow cw;
	cw.show();
	ServerWindow sw;
	sw.show();
	return a.exec();
}

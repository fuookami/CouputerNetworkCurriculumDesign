#include "ServerWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ServerWindow *w = new ServerWindow();
	w->show();
	return a.exec();
}

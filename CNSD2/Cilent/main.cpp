#include "CilentWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CilentWindow w;
	w.show();
	return a.exec();
}

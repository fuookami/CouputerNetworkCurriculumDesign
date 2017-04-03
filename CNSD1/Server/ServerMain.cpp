#include <QtWidgets/QApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Server server;
	if (server.listen() == BasicSetting::NoError)
		qDebug() << QString::fromLocal8Bit("�����������ɹ�") << server.getHostAndPortStr();
	else
		qDebug() << QString::fromLocal8Bit("����������ʧ��");

	return a.exec();
}

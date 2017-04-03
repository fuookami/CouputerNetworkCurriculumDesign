#include <QtWidgets/QApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Server server;
	if (server.listen() == BasicSetting::NoError)
		qDebug() << QString::fromLocal8Bit("服务器构建成功") << server.getHostAndPortStr();
	else
		qDebug() << QString::fromLocal8Bit("服务器构建失败");

	return a.exec();
}

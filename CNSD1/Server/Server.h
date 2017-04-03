#pragma once

#include "Public.h"
#include <QtNetwork/QAbstractSocket>

class Server : public QObject
{
	Q_OBJECT;

public:
	Server();

public:
	State state(void) const;

private:
	RetCode bindToHost(const QString &host = BasicSetting::host, quint16 port = BasicSetting::port);
	
private:
	QAbstractSocket *socket;
};
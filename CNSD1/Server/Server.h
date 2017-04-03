#pragma once

#include "Public.h"
#include <QtNetwork/QUdpSocket>

class Server : public QObject
{
	Q_OBJECT;

public:
	Server();

public:
	RetCode listen(const QHostAddress &host = QHostAddress::LocalHost,
		quint16 port = BasicSetting::port);
	QHostAddress getHost(void) const;
	quint16 getPort(void) const;
	QString getHostAndPortStr(void) const;

	void close(void);

private:
	State state(void) const;

private slots:
	void processDatagrams(void);
	
private:
	QHostAddress currHost;
	quint16 currPort;

	QUdpSocket *server;
};
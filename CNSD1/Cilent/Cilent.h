#pragma once

#include "Public.h"
#include <QtNetwork/QUdpSocket>

class Cilent : public QObject
{
	Q_OBJECT;

public:
	Cilent();

public:
	RetCode connectToHost(const QHostAddress &host, quint16 port, 
		quint8 MaxRetryTime = BasicSetting::MaxRetryTime,
		quint16 MSOfOnceTry = BasicSetting::MSOfOnceTry);
	RetCode disconnectFromHost(quint16 MSOfOnceTry = BasicSetting::MSOfOnceTry);

	inline State state(void) const;

signals:
	void pushedMsg(const QString &errMsg);

private slots:
	void hostFounded(void);
	void connectSucceed(void);
	void disconnectSucceed(void);

	void pushErrorMsg(void);

private:
	void pushMsg(const QString &msg);

private:
	QUdpSocket *udpSocket = nullptr;
};
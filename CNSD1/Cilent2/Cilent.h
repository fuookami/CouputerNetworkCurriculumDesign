#pragma once
#include <QtNetwork/QUdpSocket>

class Cilent : public QObject
{
	Q_OBJECT;

public:
	Cilent();

private slots:
	void conectSucceed(void);

public:
	QUdpSocket *cilent;
};
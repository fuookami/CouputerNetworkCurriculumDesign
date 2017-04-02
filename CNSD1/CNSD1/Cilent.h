#pragma once

#include "Public.h"
#include <QtNetwork/QAbstractSocket>

class Cilent : public QObject
{
	Q_OBJECT;

public:
	Cilent();

public:
	void connectToHost(const QString &host, quint16 port);

private slots:
	void connectSucceed(void);
	void connectUnsucceed(void);

private:
	QAbstractSocket socket;
};
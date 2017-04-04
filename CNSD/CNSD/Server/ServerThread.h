#pragma once

#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class ServerThread : public QThread
{
	Q_OBJECT;

public:
	ServerThread(QTcpSocket *_tcpSocket);

protected:
	void run();

private slots:
	void cilentDisconnected();
	void dataReceived();

private:
	QTcpSocket *tcpSocket;
	volatile bool stopped;
};
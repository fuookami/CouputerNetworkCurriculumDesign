#include "CilentSendDataThread.h"

CilentSendDataThread::CilentSendDataThread(QTcpSocket *_tcpSocket)
	: tcpSocket(_tcpSocket), stopped(true), currACKOrder(0), currSendedOrder(0)
{
	
}

unsigned char CilentSendDataThread::getDataQueueSize(void) const
{
	return datas.size();
}

void CilentSendDataThread::run()
{
	while (!datas.empty() && !stopped)
	{
		QByteArray block;
		QDataStream out(&block, QIODevice::WriteOnly);
		out << QString::fromLocal8Bit(datas.front().c_str());
		datas.pop_front();
		tcpSocket->write(block);
	}
	stopped = true;
}

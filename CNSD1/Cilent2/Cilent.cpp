#include "Cilent.h"
#include <QDataStream>

Cilent::Cilent()
	: cilent(new QUdpSocket(nullptr))
{
	QByteArray datagram;
	QDataStream out(&datagram, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_8);
	out << "Hi";

	cilent->writeDatagram(datagram, QHostAddress::LocalHost, 10000);
}

void Cilent::conectSucceed()
{
}
#include "Cilent.h"

Cilent::Cilent()
	: tcpSocket(new QTcpSocket(this))
{
}

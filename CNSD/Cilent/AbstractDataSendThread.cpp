#include "AbstractDataSendThread.h"

PKTTimer::PKTTimer(unsigned char _id)
	: id(_id), timer(new QTimer(nullptr))
{
	connect(timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
}

void PKTTimer::startTimer(const unsigned short ms)
{
	timer->start(ms);
}

void PKTTimer::stopTimer()
{
	timer->stop();
}

void PKTTimer::timeOutSlot(void)
{
	emit timeoutSignal(id);
}
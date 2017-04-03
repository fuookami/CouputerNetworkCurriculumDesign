#pragma once

#include <QtCore/QString>
#include <QtNetwork/QHostAddress>

using State = unsigned char;
using RetCode = unsigned char;

namespace BasicSetting
{
	static const unsigned short port(10000);
	static const unsigned char MaxRetryTime(10);
	static const unsigned short MSOfOnceTry(1000);

	enum RetCodes
	{
		NoError = 1,
		StateError,
		ServerLose,
		BoundFail,
	};
}
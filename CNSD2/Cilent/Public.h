#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <array>
#include <deque>
#include <map>
#include <random>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>

namespace Public
{
	static const unsigned char FrameMaxSize = 255;
	static const unsigned char WindowSize = 5;
	static const unsigned char RouletteSize = WindowSize * 2;
	static const unsigned char MaxRetryTime(10);
	static const unsigned short MSOfOnceTry(1000);
	static const unsigned char MSOfTimePart(100);

	unsigned char getRandomWaitWriteTime(void);

	using RetCode = unsigned char;
	enum RetCodes
	{
		NoError,
		StateError,
		ServerLose,
	};

	using RequestType = unsigned char;
	enum RequestTypes
	{
		SYN,
		ACK,
		PKT,
	};

	static const unsigned char FrameStateNum = 4;
	using State = unsigned char;
	enum ThreadState
	{
		Idle,
		WaitForSending,
		Sending,
		Receieving
	};
	enum FrameState
	{
		FrameNoError,
		Lose,
		Wrong,
		NoReply,
	};
	static std::vector<unsigned int> randomNumberMap;
	void generateRandomNumberMap(void);
	State getRandomFrameState(void);
	std::string getFrameStateString(State state);

	struct DataFrame
	{
		DataFrame(QDataStream &in);
		DataFrame(unsigned int id, RequestType _request, std::string::iterator bgIt, std::string::iterator edIt);
		~DataFrame() {}

		QByteArray toQByteArray(void) const;
		bool isCorrect(void) const;

		unsigned int id;
		RequestType request;
		unsigned char checkNum;
		unsigned char frameSize;
		std::string data;
	};

	using DataRoulette = std::array<std::deque<DataFrame>, 10>;
	template <class T>
	DataRoulette makeDataRoulette(T data);
	unsigned int countFrames(const DataRoulette &dataqRoulette);
	template <class T>
	std::pair<RequestType, T> readDataRoulette(DataRoulette &dataRoulette);
	using DataDeque = std::deque<DataRoulette>;

	void encode(std::string &data);
	void decode(std::string &data);

	std::string ui2str(const unsigned int num);
	unsigned int str2ui(const std::string &str);
};

template <class T>
Public::DataRoulette Public::makeDataRoulette(T data)
{
	static auto HasPutAllData([]
	(const unsigned int i, const unsigned j, const std::string &dataStr)->bool
	{
		return (i * RouletteSize + j) * FrameMaxSize >= dataStr.size();
	});

	DataRoulette dataRoulette;

	std::ostringstream sout;
	sout << data;
	std::string &dataStr(sout.str());
	encode(dataStr);
	std::string::iterator currIt(dataStr.begin());
	for (unsigned int i(0); !HasPutAllData(i, 0, dataStr); ++i)
	{
		for (unsigned int j(0); j != RouletteSize; ++j)
		{
			if (HasPutAllData(i, j + 1, dataStr))
			{
				dataRoulette[j].push_back(DataFrame(j, Public::RequestTypes::PKT, currIt, dataStr.end()));
				break;
			}
			else
			{
				dataRoulette[j].push_back(DataFrame(j, Public::RequestTypes::PKT, currIt, currIt + FrameMaxSize));
				currIt += FrameMaxSize;
			}
		}
	}

	return std::move(dataRoulette);
}

template<class T>
std::pair<Public::RequestType, T> Public::readDataRoulette(DataRoulette & dataRoulette)
{
	std::string block;
	RequestType requestType;
	T data;

	for (unsigned int i(0), j(dataRoulette.size()); i != j; ++i)
	{
		for (unsigned int k(0), l(dataRoulette[i].size()); k != l; ++k)
			std::move(dataRoulette[i][k].data.begin(), dataRoulette[i][k].data.end(), block.end());
		dataRoulette[i].clear();
	}

	std::istringstream sin(block);
	sin >> data;
	return std::make_pair(requestType, std::move(data));
}
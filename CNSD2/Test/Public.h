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
	static const unsigned short FrameMaxSize = 256;
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
		Close,
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

	using DataType = std::vector<unsigned char>;
	struct DataFrame
	{
		DataFrame(QDataStream &in);
		DataFrame(unsigned int id, RequestType _request, std::vector<unsigned char>::iterator bgIt, std::vector<unsigned char>::iterator edIt);
		~DataFrame() {}

		void getQByteArray(QByteArray &block) const;
		bool isCorrect(void) const;

		unsigned int id;
		RequestType request;
		unsigned char checkNum;
		unsigned char frameSize;
		std::vector<unsigned char> data;
	};

	using DataRoulette = std::array<std::deque<DataFrame>, 10>;
	template <class T>
	DataRoulette makeDataRoulette(T data);
	unsigned int countFrames(const DataRoulette &dataqRoulette);
	template <class T>
	std::pair<RequestType, T> readDataRoulette(DataRoulette &dataRoulette);
	using DataDeque = std::deque<DataRoulette>;

	void encode(DataType &data);
	void decode(DataType &data);

	DataType ui2data(const unsigned int num);
	unsigned int data2ui(const DataType &data);
	std::string data2uiHex(const DataType &data);
};

template <class T>
Public::DataRoulette Public::makeDataRoulette(T data)
{
	static auto HasPutAllData([]
	(const unsigned int i, const unsigned j, const std::vector<unsigned char> &dataVec)->bool
	{
		return (i * RouletteSize + j) * FrameMaxSize >= dataVec.size();
	});

	DataRoulette dataRoulette;

	std::ostringstream sout;
	sout << data;
	DataType dataVec(sout.str().begin(), sout.str().end());
	encode(dataVec);
	std::vector<unsigned char>::iterator currIt(dataVec.begin());
	for (unsigned int i(0); !HasPutAllData(i, 0, dataVec); ++i)
	{
		for (unsigned int j(0); j != RouletteSize; ++j)
		{
			if (HasPutAllData(i, j + 1, dataVec))
			{
				dataRoulette[j].push_back(DataFrame(j, Public::RequestTypes::PKT, currIt, dataVec.end()));
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
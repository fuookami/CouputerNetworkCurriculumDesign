#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <array>
#include <deque>
#include <map>
#include <random>
#include <iterator>
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
	template<class Iter>
	DataType to_datatype(Iter bgIt, Iter edIt)
	{
		return DataType(bgIt, edIt);
	}

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
	DataRoulette makeDataRoulette(DataType data);
	unsigned int countFrames(const DataRoulette &dataqRoulette);
	DataType readDataRoulette(DataRoulette &dataRoulette);
	using DataDeque = std::deque<DataRoulette>;

	void encode(DataType &data);
	void decode(DataType &data);

	DataType ui2data(const unsigned int num);
	unsigned int data2ui(const DataType &data);
	std::string data2uiHex(const DataType &data);
};


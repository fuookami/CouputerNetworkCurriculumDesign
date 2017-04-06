#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <array>
#include <deque>
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
	template <class T>
	std::pair<RequestType, T> readDataRoulette(DataRoulette &dataRoulette);
	using DataDeque = std::deque<DataRoulette>;

	void encode(std::string &data);
	void decode(std::string &data);

	std::string ui2str(const unsigned int num);
	unsigned int str2ui(const std::string &str);
};
#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <array>
#include <queue>
#include <random>

namespace Public
{
	static const unsigned short FrameMaxSize = 256;
	static const unsigned char WindowSize = 5;
	static const unsigned char RouletteSize = WindowSize * 2;
	static const unsigned char MaxRetryTime(10);
	static const unsigned short MSOfOnceTry(1000);

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
		NoError,
		Lose,
		Wrong,
		NoReply,
	};
	static std::vector<unsigned int> randomNumberMap;
	void generateRandomNumberMap(void);
	State getRandomFrameState(void);

	struct DataFrame
	{
		DataFrame(unsigned int id, std::string::iterator bgIt, std::string::iterator edIt);
		~DataFrame() {}

		unsigned int id;
		std::string data;
		unsigned char checkNum;
	};

	using DataRoulette = std::array<std::queue<DataFrame>, 10>;
	template <class T>
	DataRoulette makeDataRoulette(RequestType requestType, T data);
	template <class T>
	std::pair<RequestType, T> readDataRoulette(const DataRoulette &dataRoulette);
	using DataQueue = std::queue<DataRoulette>;

	void encode(std::string &data);
	void decode(std::string &data);
};
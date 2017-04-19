#include "Public.h"

Public::DataFrame::DataFrame(QDataStream & in)
{
	QString qData;
	in >> id >> request >> checkNum >> frameSize;
	unsigned int len = frameSize == 0 ? Public::FrameMaxSize : frameSize;
	char *rawData = new char[len];
	in.readRawData(rawData, len);
	data = std::string(rawData, rawData + len);
	delete rawData;
}

Public::DataFrame::DataFrame(unsigned int _id, RequestType _request, std::string::iterator bgIt, std::string::iterator edIt)
	: id(_id), request(_request), checkNum(0), data(bgIt, edIt)
{
	frameSize = data.size();
	for (unsigned short i(0), j(data.size()); i != j; ++i)
		checkNum += (unsigned char)data[i];
}

void Public::DataFrame::getQByteArray(QByteArray &block) const
{
	QDataStream out(&block, QIODevice::WriteOnly);
	out << id << request << checkNum << frameSize;
	out.writeRawData(data.c_str(), data.size());
}

bool Public::DataFrame::isCorrect(void) const
{
	unsigned char tempCheckNum(0);
	for (unsigned short i(0), j(data.size()); i != j; ++i)
		tempCheckNum += (unsigned char)data[i];
	return tempCheckNum == checkNum;
}

unsigned char Public::getRandomWaitWriteTime(void)
{
	static std::random_device rd;
	return MSOfTimePart + rd() % MSOfTimePart;
}

void Public::generateRandomNumberMap(void)
{
	for (unsigned int i(0); i != FrameStateNum; ++i)
		for (unsigned int j(FrameStateNum - 1); j != -1; --j)
			randomNumberMap.push_back(i);
}

Public::State Public::getRandomFrameState()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::poisson_distribution<> d(FrameStateNum);
	
	if (randomNumberMap.empty())
		generateRandomNumberMap();
	
	unsigned int currNum(d(gen));

	return currNum >= randomNumberMap.size() ? (FrameStateNum - 1) : randomNumberMap[currNum];
}

std::string Public::getFrameStateString(State state)
{
	static std::map<State, std::string> frameStateString =
	{
		std::make_pair(FrameState::FrameNoError, "正常"),
		std::make_pair(FrameState::Lose, "帧丢失"),
		std::make_pair(FrameState::Wrong, "帧错误"),
		std::make_pair(FrameState::NoReply, "不回复")
	};

	return frameStateString[state];
}

unsigned int Public::countFrames(const DataRoulette & dataqRoulette)
{
	unsigned int counter(0);
	for (unsigned int i(0); i != Public::RouletteSize; ++i)
		counter += dataqRoulette[i].size();
	return counter;
}

void Public::encode(std::string & data)
{
	unsigned char k(0);
	for (unsigned int i(0), j(data.size()); i != j; ++i, ++k)
		data[i] += k;
}

void Public::decode(std::string & data)
{
	unsigned char k(0);
	for (unsigned char i(0), j(data.size()); i != j; ++i, ++k)
		data[i] = (unsigned char)(data[i] - k);
}

std::string Public::ui2str(unsigned int num)
{
	std::string str;
	if (num == 0)
	{
		str = std::string("\0");
	}
	else 
	{
		for (unsigned int i(0); num != 0; ++i, num >>= 8)
			str.insert(str.begin(), (unsigned char)(num & 0x000000ff));
	}
	return std::move(str);
}

unsigned int Public::str2ui(const std::string & str)
{
	unsigned int num(0);
	for (unsigned int i(0), j(str.size()); i != j; ++i)
	{
		num <<= 8;
		num += (unsigned char)str[i];
	}
	return num;
}

std::string Public::str2uiHex(const std::string & str)
{
	std::ostringstream sout;
	for (const unsigned char b : str)
		sout << std::hex << (unsigned int)b;
	return std::move(sout.str());
}

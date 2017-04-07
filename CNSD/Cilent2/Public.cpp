#include "Public.h"

Public::DataFrame::DataFrame(QDataStream & in)
{
	in >> id >> request >> checkNum >> frameSize;
	QByteArray rawData;
	in.readRawData(rawData.data(), frameSize);
	std::move(rawData.begin(), rawData.end(), data.begin());
}

Public::DataFrame::DataFrame(unsigned int _id, RequestType _request, std::string::iterator bgIt, std::string::iterator edIt)
	: id(_id), request(_request), checkNum(0)
{
	std::move(bgIt, edIt, data.begin());
	frameSize = data.size();
	for (unsigned short i(0), j(data.size()); i != j; ++i)
		checkNum += data[i];
}

QByteArray Public::DataFrame::toQByteArray(void) const
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out << id << request << checkNum << frameSize;
	out.writeBytes(data.c_str(), data.size());
	return std::move(block);
}

bool Public::DataFrame::isCorrect(void) const
{
	unsigned char tempCheckNum(0);
	for (unsigned short i(0), j(data.size()); i != j; ++i)
		tempCheckNum += data[i];
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
		std::make_pair(FrameState::FrameNoError, "Õý³£"),
		std::make_pair(FrameState::Lose, "Ö¡¶ªÊ§"),
		std::make_pair(FrameState::Wrong, "Ö¡´íÎó"),
		std::make_pair(FrameState::NoReply, "²»»Ø¸´")
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
		data[i] -= k;
}

std::string Public::ui2str(unsigned int num)
{
	std::string str;
	for (unsigned int i(0), j(4); i != j; ++i, num >>= 8)
		str.insert(str.begin(), (unsigned char)(num & 0x000000ff));
	return std::move(str);
}

unsigned int Public::str2ui(const std::string & str)
{
	unsigned int num(0);
	for (unsigned int i(0), j(str.size()); i != j; ++i, num <<= 8)
		num += str[i];
	return num;
}

#include "Public.h"

std::vector<unsigned int> randomNumberMap;

Public::DataFrame::DataFrame(QDataStream & in)
{
	QString qData;
	in >> id >> request >> checkNum >> frameSize;
	unsigned int len = frameSize == 0 ? Public::FrameMaxSize : frameSize;
	char *rawData = new char[len];
	in.readRawData(rawData, len);
	data = std::vector<unsigned char>(rawData, rawData + len);
	delete rawData;
}

Public::DataFrame::DataFrame(unsigned char _id, RequestType _request, std::vector<unsigned char>::iterator bgIt, std::vector<unsigned char>::iterator edIt)
	: id(_id), request(_request), checkNum(0), data(bgIt, edIt)
{
	encode(data);
	frameSize = data.size();
	for (unsigned short i(0), j(data.size()); i != j; ++i)
		checkNum += (unsigned char)data[i];
}

void Public::DataFrame::getQByteArray(QByteArray &block) const
{
	QDataStream out(&block, QIODevice::WriteOnly);
	out << id << request << checkNum << frameSize;
	out.writeRawData(reinterpret_cast<char *>(data.begin()._Ptr), data.size());
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
		for (unsigned int j(FrameStateNum - i); j != -1; --j)
			randomNumberMap.push_back(i);
}

Public::State Public::getRandomFrameState()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::poisson_distribution<> d(FrameStateNum);

	if (randomNumberMap.empty())
		generateRandomNumberMap();

	unsigned int currNum((d(gen) - 1 + randomNumberMap.size()) % randomNumberMap.size());

	return randomNumberMap[currNum];
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


Public::DataRoulette Public::makeDataRoulette(DataType data)
{
	static auto HasPutAllData([]
	(const unsigned int i, const unsigned j, const DataType &data)->bool
	{
		return (i * RouletteSize + j) * FrameMaxSize >= data.size();
	});

	DataRoulette dataRoulette;

	DataType::iterator currIt(data.begin());
	for (unsigned int i(0); !HasPutAllData(i, 0, data); ++i)
	{
		for (unsigned int j(0); j != RouletteSize; ++j)
		{
			if (HasPutAllData(i, j + 1, data))
			{
				dataRoulette[j].emplace_back(DataFrame(j, Public::RequestTypes::PKT, currIt, data.end()));
				break;
			}
			else
			{
				dataRoulette[j].emplace_back(DataFrame(j, Public::RequestTypes::PKT, currIt, currIt + FrameMaxSize));
				currIt += FrameMaxSize;
			}
		}
	}

	return std::move(dataRoulette);
}

Public::DataType Public::readDataRoulette(DataRoulette & dataRoulette)
{
	DataType block;
	bool flag(true);

	while (flag)
	{
		for (unsigned int i(0), j(dataRoulette.size()); i != j; ++i)
		{
			if (dataRoulette[i].empty())
			{
				flag = false;
				break;
			}

			block.insert(block.end(), dataRoulette[i].front().data.cbegin(), dataRoulette[i].front().data.cend());
			dataRoulette[i].pop_front();
		}
	}

	return std::move(block);
}

void Public::encode(DataType & data)
{
	unsigned char k(0);
	for (unsigned int i(0), j(data.size()); i != j; ++i, ++k)
		data[i] += k;
}

void Public::decode(DataType & data)
{
	unsigned char k(0);
	for (unsigned char i(0), j(data.size()); i != j; ++i, ++k)
		data[i] = (unsigned char)(data[i] - k);
}

Public::DataType Public::ui2data(unsigned int num)
{
	DataType data;
	if (num == 0)
	{
		data.push_back(0);
	}
	else
	{
		for (unsigned int i(0); num != 0; ++i, num >>= 8)
			data.insert(data.begin(), (unsigned char)(num & 0x000000ff));
	}
	return std::move(data);
}

unsigned int Public::data2ui(const DataType & data)
{
	unsigned int num(0);
	for (unsigned int i(0), j(data.size()); i != j; ++i)
	{
		num <<= 8;
		num += (unsigned char)data[i];
	}
	return num;
}

std::string Public::data2uiHex(const DataType & data)
{
	std::ostringstream sout;
	for (unsigned int i(0), j(data.size()); i != j; ++i)
	{
		sout << std::hex << (unsigned int)data[i];
	}
	return std::move(sout.str());
}

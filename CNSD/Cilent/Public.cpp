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

template <class T>
Public::DataRoulette Public::makeDataRoulette(T data)
{
	static auto HasPutAllData([RouletteSize, FrameMaxSize]
		(const unsigned int i, const unsigned j, const QByteArray &block)->bool
	{
		return (i * RouletteSize + j) >= block.size();
	});

	DataRoulette dataRoulette;

	std::ostringstream sout;
	sout << data;
	std::string &dataStr(sout.str());
	encode(dataStr);
	std::string::iterator currIt(dataStr.begin());
	for (unsigned int i(0); !HasPutAllData(i, 0, block); ++i)
	{
		for (unsigned int j(0); j != RouletteSize; ++j)
		{
			if (HasPutAllData(i, j + 1, block))
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

#include "Public.h"

Public::DataFrame::DataFrame(unsigned int _id, std::string::iterator bgIt, std::string::iterator edIt)
	: id(_id), checkNum(0)
{
	std::move(bgIt, edIt, data.begin());
	for (unsigned short i(0), j(data.size()); i != j; ++i)
		checkNum += data[i];
}

template <class T>
Public::DataRoulette Public::makeDataRoulette(RequestType requestType, T data)
{
	static auto HasPutAllData([RouletteSize, FrameMaxSize]
		(const unsigned int i, const unsigned j, const QByteArray &block)->bool
	{
		return (i * RouletteSize + j) >= block.size();
	});

	DataRoulette dataRoulette;

	std::ostringstream sout;
	sout << requestType << data;
	std::string &dataStr(sout.str());
	encode(dataStr);
	std::string::iterator currIt(dataStr.begin());
	for (unsigned int i(0); !HasPutAllData(i, 0, block); ++i)
	{
		for (unsigned int j(0); j != RouletteSize; ++j)
		{
			if (HasPutAllData(i, j + 1, block))
			{
				dataRoulette[j].push(DataFrame(j, currIt, block.end()));
				break;
			}
			else
			{
				dataRoulette[j].push(DataFrame(j, currIt, currIt + FrameMaxSize));
				currIt += FrameMaxSize;
			}
		}
	}

	return std::move(dataRoulette);
}

template<class T>
std::pair<Public::RequestType, T> Public::readDataRoulette(const DataRoulette & dataRoulette)
{
	std::stringstream io;
	RequestType requestType;
	T data;

	for (unsigned int i(0), j(dataRoulette.size()); i != j; ++i)
		for (unsigned int k(0), l(dataRoulette[i].size()); k != l; ++k)
			io << dataRoulette[i][k].data;
	std::string &str(io);

	io >> requestType >> data;
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

#include "samplesparser.h"
#include <regex>
#include <iostream>
#include <fstream>

SamplesParser::SamplesParser(std::vector<std::string> const & filesPath) :
	mDistances(new std::unordered_map<Pair, double, PairHash>())
{
	parseFiles(filesPath);
}

SamplesParser::SamplesParser()
{
}

std::shared_ptr<std::unordered_map<Pair, double, PairHash> > SamplesParser::getDistances() const
{
	return mDistances;
}

std::vector<Customer> SamplesParser::getCustomers() const
{
	return mCustomers;
}

std::vector<Truck> SamplesParser::getTrucks() const
{
	return mTrucks;
}

void SamplesParser::parseFiles(const std::vector<std::string> &filesPath)
{
	//std::vector<unsigned int> weights;
	std::unordered_map<unsigned int, std::pair<double, double>> coordsShift;
	std::size_t maxId = 0;
	for (const std::string& path: filesPath)
	{
		std::ifstream stream(path);
		if (!stream)
		{
		}
		std::string str;
		for (std::size_t i = 0; i < 3; ++i) // read first meaningless lines
		{
			std::getline(stream, str);
		}
		std::getline(stream, str);
		std::regex dimRegex("DIMENSION(\\s*):(\\s*)(\\d+)(\\s+)");
		std::smatch dimMatch;
		if (!std::regex_match(str, dimMatch, dimRegex))
		{
#if WITH_LOG
			std::cout << str << std::endl;
#endif
		}
		unsigned int dimension = std::stoi(dimMatch[3].str());
		std::getline(stream, str); //metrics info, we choose always euclid
		std::getline(stream, str);
		std::regex capacityRegex("CAPACITY(\\s*):(\\s*)(\\d+)(\\s+)");
		std::smatch capacityMatch;
		if (!std::regex_match(str, capacityMatch, capacityRegex))
		{
#if WITH_LOG
			std::cout << str << std::endl;
#endif
		}
		unsigned int capacity = std::stoi(capacityMatch[3].str());
		std::getline(stream, str);
		std::regex vehiclesRegex("VEHICLES(\\s*):(\\s*)(\\d+)(\\s+)");
		std::smatch vehiclesMatch;
		if (!std::regex_match(str, vehiclesMatch, vehiclesRegex))
		{
#if WITH_LOG
			std::cout << str << std::endl;
#endif
		}
		unsigned int vehicles = std::stoi(vehiclesMatch[3].str());
		for (unsigned int i = 0; i < vehicles; ++i)
		{
			mTrucks.push_back(Truck(mTrucks.size(), capacity, 0));
		}
		std::getline(stream, str); //NODE_COORD_SECTION
		std::unordered_map<unsigned int, std::pair<double, double>> coords;
		std::regex coordsRegex("(\\s*)(\\d+)(\\s+)((-)?\\d+(.\\d+)*)(\\s+)((-)?\\d+(.\\d+)*)(\\s*)");
		for (unsigned int i = 0; i < dimension; ++i)
		{
			std::getline(stream, str);
			std::smatch coordsMatch;
			if (!std::regex_match(str, coordsMatch, coordsRegex))
			{
#if WITH_LOG
				std::cout << str << std::endl;
#endif
			}
			unsigned int custId = std::stoi(coordsMatch[2]);
			double xCoord = std::stod(coordsMatch[4]);
			double yCoord = std::stod(coordsMatch[8]);
			coords.insert(std::make_pair(custId,
										 std::make_pair(xCoord, yCoord)));
#if WITH_LOG
			std::cout << "id is " << custId << " x " << xCoord << " y " << yCoord << std::endl;
#endif

		}
		std::getline(stream, str); //DEMAND_SECTION
		std::vector<std::pair<unsigned int, unsigned int>> weights;
		std::regex weightsRegex("(\\s*)(\\d+)(\\s+)(\\d+)(\\s+)");
		for (unsigned int i = 0; i < dimension; ++i)
		{
			std::getline(stream, str);
			std::smatch weightsMatch;
			if (!std::regex_match(str, weightsMatch, weightsRegex))
			{
#if WITH_LOG
				std::cout << str << std::endl;
#endif
			}
			double weight = std::stod(weightsMatch[4]);
			weights.push_back(std::make_pair(std::stoi(weightsMatch[2]), weight));
			//            if (std::abs(weight) > std::numeric_limits<double>::epsilon())
			//            {
			//                mCustomers.push_back(Customer(mCustomers.size() + 1, Package(weight, 0)));
			//            }
		}
		std::getline(stream, str); //DEPOT_SECTION
		std::getline(stream, str);
		std::regex depotReg("(\\s*)(\\d+)(\\s+)");
		std::smatch depotMatch;
		if (!std::regex_match(str, depotMatch, depotReg))
		{
#if WITH_LOG
			std::cout << str << std::endl;
#endif
		}
		unsigned int depotId = std::stoi(depotMatch[2]);
		if (depotId != 1)
		{
			std::cout << "RULE IS INCORECT! DEPOT NOT FIRST!!!" <<std::endl;
		}
		std::pair<double, double> depotCoord = coords.at(depotId);
		std::size_t curMaxId = maxId;
		for (std::pair<unsigned int, unsigned int> weight : weights)
		{
			if (weight.first == depotId)
			{
				continue;
			}
#ifdef WITH_LOG
			if (!mCustomers.empty() && maxId + weight.first <= mCustomers.back().id)
			{
				std::cout << "bad id creation!!!" << std::endl;
			}
#endif
			mCustomers.push_back(Customer(maxId + weight.first, Package(weight.second, 0)));
			curMaxId = std::max(maxId + weight.first, curMaxId);
		}
		for (auto coordPair : coords)
		{
			std::pair<double, double> curCoord = coordPair.second;
			if (coordPair.first != depotId)
			{
				//TODO::NOT THE SAME ORDER!!!!!
				coordsShift.insert(std::make_pair(coordPair.first + maxId,
												  std::make_pair(curCoord.first - depotCoord.first,
																 curCoord.second - depotCoord.second)));
			}
		}
		maxId = curMaxId + 1;
	}
	for (auto firstPair : coordsShift)
	{
		for (auto secondPair : coordsShift)
		{
			double first = firstPair.second.first - secondPair.second.first;
			double second = firstPair.second.second - secondPair.second.second;
			double distance = std::sqrt(first * first + second * second);
			//std::cout << distance << std::endl;
			mDistances->insert(std::make_pair(Pair(firstPair.first, secondPair.first), distance));
		}
		double first = firstPair.second.first;
		double second = firstPair.second.second;
		double distance = std::sqrt(first * first + second * second);
		mDistances->insert(std::make_pair(Pair(firstPair.first, 0), distance));
		mDistances->insert(std::make_pair(Pair(0, firstPair.first), distance));
	}
	mDistances->insert(std::make_pair(Pair(0, 0), 0));
}

#include "filereader.h"

#include <iostream>
#include <sstream>
#include <fstream>

const std::string TruckFileName = "trucksn.csv";
const std::string ClientFileName = "clients.csv";
const std::string DistanceFileName = "distance.csv";
const std::string TimeFileName = "time.csv";
const std::string VelocityRestrictionsName = "speed.csv";

template <class T>
static std::vector<T> parseFile(std::string const &fileName)
{
	std::vector<T> elements;
	std::ifstream stream(fileName);
	if (!stream)
	{
#ifdef WITH_LOG
		std::cout << "cant parse " << fileName << std::endl;
#endif
		return elements;
	}
	std::string str;
	std::getline(stream, str); //read first line with info
	while (!stream.eof())
	{
		if (stream.eof())
			break;
		std::getline(stream, str);
		if (str.empty())
			break;
		elements.push_back(T::parse(str));
	}
#ifdef WITH_LOG
	std::cout << "parsed " << fileName << std::endl;
#endif
	return elements;
}

FileReader::FileReader(const std::string &folder) :
	mFolder(folder)
{
}

std::vector<Truck> FileReader::getTrucks()
{
#ifdef WITH_LOG
	std::cout << "folder name is " << mFolder << std::endl;
#endif
	return parseFile<Truck>(mFolder + TruckFileName);
}

std::vector<Customer> FileReader::getCustomers()
{
	return parseFile<Customer>(mFolder + ClientFileName);
}

std::shared_ptr<std::unordered_map<Pair, double, PairHash> > FileReader::getDistances()
{
	return getMatrix(mFolder + DistanceFileName);
}

std::shared_ptr<std::unordered_map<Pair, double, PairHash> > FileReader::getTime()
{
	return getMatrix(mFolder + TimeFileName);
}

VelocityCalculator FileReader::getVelocityRestrictions()
{
	std::vector<SpeedDistance> restrInfo = parseFile<SpeedDistance>(mFolder + VelocityRestrictionsName);
	return VelocityCalculator(restrInfo);
}

std::shared_ptr<std::unordered_map<Pair, double, PairHash> > FileReader::getMatrix(const std::string &fileName)
{
	std::ifstream stream(fileName);
	std::shared_ptr<std::unordered_map<Pair, double, PairHash> > distances(new std::unordered_map<Pair, double, PairHash>());
	std::vector<int> ids;
	if (!stream)
	{
		return distances;
	}
	std::string str;
	std::getline(stream, str); //read first line with info
	std::istringstream idsStream(str);
	std::string defaultSym;
	std::getline(idsStream, defaultSym, ';'); //read symbol X
	while (!idsStream.eof())
	{
		std::string idStr;
		std::getline(idsStream, idStr, ';');
		if (idStr.empty())
			break;
		int id = std::stoi(idStr);
		ids.push_back(id);
	}
	while (!stream.eof())
	{
		std::getline(stream, str);
		if (str.empty())
			break;
		std::istringstream strStream(str);
		std::string idStr;
		std::getline(strStream, idStr, ';');
		int id = std::stoi(idStr);
		int pos = 0;
		while (!stream.eof())
		{
			std::string value;
			std::getline(strStream, value, ';');
			if (value.empty())
				break;
			double val = std::stod(value);
			distances->operator [](Pair(ids[pos], id)) = val;
			pos ++;
		}

	}
	return distances;
}
